#ifndef _FASTBINCOMPOSER_H_
#define _FASTBINCOMPOSER_H_

#include "../../../Plaza-II/P2Utils/Include/stuff.h"
#include "../../../Plaza-II/P2Utils/Include/P2Str.hpp"
#include "../../../Plaza-II/P2SysTypes/Include/P2SysBCDII.h"
#include "../../../Plaza-II/P2SysParser/Include/P2SysBinParser.h"

#define STRING_POOL_INCREMENT 512

#ifdef _MSC_VER // Microsoft specific
#define FORCE_INLINE   __forceinline
#define FORCE_NOINLINE __declspec(noinline)
#else
#define FORCE_INLINE   inline
#define FORCE_NOINLINE
#endif // _MSC_VER

//// this file can be used with short wchar_t only (sizeof(wchar_t) must be equal 2)
//STATIC_ASSERT(sizeof(wchar_t) == 2, sizeof_wchar_t_must_be_equal_2)

// хорошо для небольших блоков
inline void p2memcpy(void * pDst, const void * pSrc, size_t nBytes)
{
    const char * pS = static_cast< const char * >(pSrc);
    char * pD = static_cast< char * >(pDst);
    while(nBytes--) *pD++ = *pS++;
}


template<void* (malloc_f)(size_t),
	     void* (realloc_f)(void*,size_t),
		 void  (free_f)(void*)>
class CFastBinComposer_ : /*protected*/ public P2_ComposeState
{
public:
    CFastBinComposer_()
    {
        buf = NULL, size = 0, alloc_size = 0;
    }

    template< typename T >
    FORCE_INLINE P2ERR Compose(T value)
    {
        if (sizeof(T) > alloc_size - size)
        {
            RETURN_P2ERR(GrowStateBuf(sizeof(T)));
        }
        *reinterpret_cast< T * >(reinterpret_cast< BYTE * >(buf) + size) = value;
        size += sizeof(T);
        return P2ERR_OK;
    }

    inline P2ERR ComposeT(const P2TIME* value)
    {
        if (sizeof(P2TIME) > alloc_size - size)
        {
            RETURN_P2ERR(GrowStateBuf(sizeof(P2TIME)));
        }
        ComposeNoCheck(value->year);
        ComposeNoCheck(value->month);
        ComposeNoCheck(value->day);
        ComposeNoCheck(value->hour);
        ComposeNoCheck(value->minute);
        ComposeNoCheck(value->second);
        ComposeNoCheck(value->msec);
        return P2ERR_OK;
    }

    inline P2ERR ComposeC(const char * value)
    {
        size_t dataSz = strlen(value) + 1;
        return ComposeWithLen(value, static_cast< UINT32 >(dataSz));
    }

	inline P2ERR ComposeC0(const char* value)
	{
		if(value == NULL)
		{
			return Compose<UINT32>(NULL_POOL_OFFSET); 
		}
		else
		{
			return ComposeC(value);
		}
	}

    inline P2ERR ComposeC(const char * value, size_t maxSz)
    {
        if (sizeof(UINT32) + maxSz > alloc_size - size)
        {
            RETURN_P2ERR(GrowStateBuf(sizeof(UINT32) + maxSz));
        }
        UINT32 * pLen = reinterpret_cast< UINT32 * >(static_cast< BYTE * >(buf) + size);
        char * p = static_cast< char * >(buf) + size + sizeof(UINT32);
        char * p0 = p;
        while ( 0 != (*p++ = *value++) ) ; // этот вариант быстрее при оптимизации через PGO
        /*do
        { *p++ = *value; } // этот вариант быстрее без PGO
        while (*value++);*/
        *pLen = static_cast< UINT32 >(p - p0);
        size += sizeof(UINT32) + *pLen;
        return P2ERR_OK;
    }

    inline P2ERR ComposeW(const P2WCHAR_T * value)
    {
        size_t dataSz = (P2WcsLen(value) + 1) * sizeof(P2WCHAR_T);
        return ComposeWithLen(value, static_cast< UINT32 >(dataSz));
    }

    inline P2ERR ComposeB(const void * value, UINT32 dataLen)
    {
        return ComposeWithLen(value, dataLen);
    }

	inline P2ERR ComposeB0(const void* value, UINT32 dataLen)
	{
		if(value == NULL)
		{
			return Compose<UINT32>(NULL_POOL_OFFSET);
		}
		else
			return ComposeWithLen(value,dataLen); 
	}

    inline P2ERR ComposeZ(const P2ZBLOCK * value)
    {
        return ComposeWithLen(value->data, value->size);
    }

    inline P2ERR ComposeBCD(const void * value, size_t dataLen)
    {
        if (dataLen > alloc_size - size)
        {
            RETURN_P2ERR(GrowStateBuf(dataLen));
        }
        //memcpy(static_cast< BYTE * >(buf) + size, value, dataLen);
        p2memcpy(static_cast< BYTE * >(buf) + size, value, dataLen);
        size += dataLen;
        return P2ERR_OK;
    }

    /*inline P2ERR ComposeBCD(const void * bcd)
    {
        size_t bcdMemSz = P2BCDIISIZETF(P2BCDIIGetType(bcd));
        if (bcdMemSz > alloc_size - size)
        {
            RETURN_P2ERR(GrowStateBuf(bcdMemSz));
        }
        memcpy(static_cast< BYTE * >(buf) + size, bcd, bcdMemSz);
        size += bcdMemSz;
        //const BYTE* pSrc = static_cast< const BYTE* >(bcd);
        //BYTE* pDst = static_cast< BYTE * >(buf) + size;
        //size += bcdMemSz;
        //while(bcdMemSz--)
        //    *pDst++ = *pSrc++;
        return P2ERR_OK;
    }*/

    inline const void * GetBuffer() const { return buf; }
    inline size_t GetSize() const { return size; }
    inline void Reset() { size = 0; }

	inline P2ERR SetComposePosition(size_t sz)
	{
		if (sz > alloc_size)
		{
			return P2ERR_COMMON_WRONG_ARGUMENTS;
		}
		size = sz;
		return P2ERR_OK;
	}

    inline void * Reserve(size_t reserveSz) 
    {
        if (reserveSz > alloc_size - size)
        {
            if (P2FAILED(GrowStateBuf(reserveSz)))
            {
                return NULL;
            }
        }
        BYTE * retval = static_cast< BYTE * >(buf) + size;
        size += reserveSz;
        return retval;
    }

    ~CFastBinComposer_()
    {
        if (NULL != buf)
        {
			(*free_f)(buf);
            buf = NULL;
        }
    }

    FORCE_NOINLINE P2ERR GrowStateBuf(size_t delta)
    {
        size_t new_alloc_size = alloc_size + 
            ((delta > alloc_size * 2) ? delta + STRING_POOL_INCREMENT : alloc_size * 2);

        BYTE *b = static_cast<BYTE *>((*realloc_f)(buf, new_alloc_size));
        if (NULL == b)
        {
            return P2ERR_COMMON_NO_MEMORY;
        }
        memset(static_cast< BYTE * >(b) + size, 0, new_alloc_size - alloc_size); // security clean
        buf = b;
        alloc_size = new_alloc_size;
        return P2ERR_OK;
    }

protected:
    template< typename T >
    FORCE_INLINE void ComposeNoCheck(T value)
    {
        *reinterpret_cast< T * >(static_cast< BYTE * >(buf) + size) = value;
        size += sizeof(T);
    }

    inline P2ERR ComposeWithLen(const void * pData, UINT32 dataSz)
    {
        if (sizeof(UINT32) + dataSz > alloc_size - size)
        {
            RETURN_P2ERR(GrowStateBuf(sizeof(UINT32) + dataSz));
        }
        ComposeNoCheck(dataSz);
        memcpy(static_cast <BYTE * >(buf) + size, pData, dataSz);
        size += dataSz;
        return P2ERR_OK;
    }
};

typedef CFastBinComposer_< ::malloc,::realloc,::free> CFastBinComposer; 
////////////////////////////////////////////////////////////////////////////////

/* класс, который обладает интерфейсом CFastBinComposer, но считает только  размер композера */
class CFastBinSizeComposer
{
	size_t m_size;
	public:
	CFastBinSizeComposer():m_size(0) {}

	template<typename T>
	FORCE_INLINE P2ERR Compose(T value)
	{
		m_size += sizeof(T);
		return P2ERR_OK;
	}

	inline P2ERR ComposeC(const char* value)
	{
		m_size += sizeof(UINT32) + strlen(value) + 1;
		return P2ERR_OK;
	}

	inline P2ERR ComposeC0(const char* value)
	{
		if(value == NULL)
			m_size += sizeof(UINT32);
		else
			m_size += sizeof(UINT32) + strlen(value) + 1;
		return P2ERR_OK;
	}

	inline P2ERR ComposeB(const void* /*value*/, UINT32 dataLen)
	{
		m_size += sizeof(UINT32) + dataLen;
		return P2ERR_OK;
	}

	inline P2ERR ComposeB0(const void* value, UINT32 dataLen)
	{
		if(value == NULL)
			m_size += sizeof(UINT32);
		else
			m_size += sizeof(UINT32) + dataLen;
		return P2ERR_OK;
	} 

    inline size_t GetSize() const { return m_size; }
};


/* класс композера для буфера фиксированного размера. Не использует аллокации 
 * Есть вариант с проверками размера и без них */

class CFixedSizeComposer_Base
{ 
	protected:

	void  *const m_pBuf;
	const size_t m_bufSz;
	size_t 		 m_size;	

	void WriteToBuffer(const void* pData, size_t dataSz)
	{ 
        memcpy(static_cast<char*>(m_pBuf) + m_size, pData, dataSz);
        m_size += dataSz;
	}

	template<typename T> void WriteToBuffer(T value)
	{ 
        *reinterpret_cast<T*>(static_cast<char*>(m_pBuf) + m_size) = value;
		m_size += sizeof(T);
	}

	CFixedSizeComposer_Base(void* pBuf, size_t size):m_pBuf(pBuf), m_bufSz(size), m_size(0) 
	{ 
		_ASSERT(m_pBuf != NULL && m_bufSz != 0);
	}

	public:

	// сколько места нужно в композере при различных способах композинга.  
	static size_t SizeOf_B(UINT32 dataSz) 					
	{ 
		return sizeof(UINT32) + dataSz; 
	} 

	static size_t SizeOf_C(const char* pStr) 				
	{ 
		return sizeof(UINT32) + (pStr ? strlen(pStr) + 1 : 0); 
	} 

	template<typename T> 
	static size_t SizeOf() 									
	{ return sizeof(T); } 

	static size_t SizeOf_2B(size_t sz, size_t sz1)			
	{ 
		return sizeof(UINT32) + sz + sz1; 
	}

    const void* GetBuffer() 	const 	{ return m_pBuf; }
    size_t 		GetSize() 		const 	{ return m_size; } 
	size_t		GetCapacity() 	const 	{ return m_bufSz;}
	void*		GetPosition()			{ return static_cast<char*>(m_pBuf) + m_size; }

    private:
    CFixedSizeComposer_Base& operator = (const CFixedSizeComposer_Base& rhs);
};

// композер над буфером фиксированного размера, который проверяет достаточно ли места при каждой записи в буфер.
class CFastBinFixedSizeComposer : public CFixedSizeComposer_Base
{
	public:

	CFastBinFixedSizeComposer(void* pBuf, size_t size):CFixedSizeComposer_Base(pBuf, size) 
	{ }

    P2ERR ComposeB(const void* pData, UINT32 dataSz)
    {
		if(m_size + dataSz + sizeof(UINT32) > m_bufSz)
		{
			return P2ERR_COMMON_BUFFER_TOO_SMALL;
		}

		WriteToBuffer<UINT32>(dataSz);
		WriteToBuffer(pData, dataSz); 
        return P2ERR_OK;
    }

	P2ERR Compose_2B(const void* pData, size_t sz, const void* pData1, size_t sz1)
	{
		if(m_size + sz + sz1 > m_bufSz)
		{
			return P2ERR_COMMON_BUFFER_TOO_SMALL;
		}

		WriteToBuffer<UINT32>(static_cast<UINT32>(sz + sz1));
		WriteToBuffer(pData, sz); 
		WriteToBuffer(pData1, sz1); 

		return P2ERR_OK;
	}

    template<typename T> 
	P2ERR Compose(T value)
    {
		if(m_size + sizeof(T) > m_bufSz)
		{
			return P2ERR_COMMON_BUFFER_TOO_SMALL;
		} 

		WriteToBuffer<T>(value); 
        return P2ERR_OK;
    }


    P2ERR ComposeC(const char* value)
    {
        return ComposeB(value, static_cast<UINT32>(strlen(value) + 1));
    }

	P2ERR ComposeC0(const char* value)
	{
		if(value == NULL)
		{
			return Compose<UINT32>(NULL_POOL_OFFSET); 
		}
		else
		{
			return ComposeB(value, static_cast<UINT32>(strlen(value) + 1));
		}
	}

	P2ERR ComposeB0(const void* value, UINT32 dataLen)
	{
		if(value == NULL)
		{
			return Compose<UINT32>(NULL_POOL_OFFSET);
		}
		else
		{
			return ComposeB(value, dataLen); 
		}
	}
};


class CUncheckedFixedSizeComposer : public CFixedSizeComposer_Base
{
	public:

	CUncheckedFixedSizeComposer(void* pBuf, size_t size): CFixedSizeComposer_Base(pBuf, size) 
	{ }

	void ComposeData(const void* pData, UINT32 dataSz)
	{ 
		WriteToBuffer(pData, dataSz); 
	}

    void ComposeB(const void* pData, UINT32 dataSz)
    { 
		WriteToBuffer<UINT32>(dataSz);
		WriteToBuffer(pData, dataSz); 
    }

	void Compose_2B(const void* pData, size_t sz, const void* pData1, size_t sz1)
	{ 
		WriteToBuffer<UINT32>(static_cast<UINT32>(sz + sz1));
		WriteToBuffer(pData, sz); 
		WriteToBuffer(pData1, sz1); 
	}

    template<typename T> void Compose(T value)
    { 
		WriteToBuffer<T>(value); 
    } 

    void ComposeC(const char* value)
    {
        ComposeB(value, static_cast<UINT32>(strlen(value) + 1));
    }

	void ComposeC0(const char* value)
	{
		if(value == NULL)
			Compose<UINT32>(NULL_POOL_OFFSET); 
		else
			ComposeB(value, static_cast<UINT32>(strlen(value) + 1));
	}

	void ComposeB0(const void* value, UINT32 dataLen)
	{
		if(value == NULL)
			Compose<UINT32>(NULL_POOL_OFFSET);
		else
			ComposeB(value, dataLen); 
	} 
};

////////////////////////////////////////////////////////////////////////////////
class CFastBinParser : public P2_ParseState
{
public:
	CFastBinParser(const void *p, size_t sz)
	{
		Init(p, sz);
	}

	inline void Init(const void *p, size_t sz)
	{
		_ASSERT(p != NULL || sz == 0);
		buf = p;
		size = sz;
		curIndex = 0;
	}

	inline void Reset(void)
	{
		curIndex = 0;
	}

	inline P2ERR Set(size_t pos)
	{
		if (pos > size)
		{
			return P2ERR_COMMON_WRONG_ARGUMENTS;
		}
		curIndex = pos;
		return P2ERR_OK;
	}

    inline const void * GetParsePtr() const { return static_cast< const UINT8 * >(buf) + curIndex; }

	inline P2ERR CheckBuf(size_t sz) const
	{
		return (size - curIndex >= sz) ? (P2ERR) P2ERR_OK : P2ERR_MSG_BAD_PARSE_BLOCK;
	}

	inline void CheckBufThrow(size_t sz) const 
	{
		if (size - curIndex < sz)
		{
			throw (P2ERR)P2ERR_MSG_BAD_PARSE_BLOCK;
		}
	}

	template<typename T>
	inline P2ERR Parse(T *pT)
	{
		RETURN_P2ERR(CheckBuf(sizeof(T)));
		const UINT8 *p = static_cast<const UINT8 *>(buf) + curIndex;
		*pT = *reinterpret_cast<const T *>(p);
		curIndex += sizeof(T);
		return P2ERR_OK;
	}
	template<typename T>
	inline void ParseThrow(T *pT) 
	{
		CheckBufThrow(sizeof(T));
		const UINT8 *p = static_cast<const UINT8 *>(buf) + curIndex;
		*pT = *reinterpret_cast<const T *>(p);
		curIndex += sizeof(T);
	}
	template<typename T>
	inline void ParseNoCheck(T *pT)
	{
		const UINT8 *p = static_cast<const UINT8 *>(buf) + curIndex;
		*pT = *reinterpret_cast<const T *>(p);
		curIndex += sizeof(T);
	}

	inline P2ERR ParseC(const char **ppc, size_t *pSize = NULL)
	{
		UINT32 sz;
		RETURN_P2ERR(Parse(&sz));
		const char *p = static_cast<const char *>(buf) + curIndex;
		RETURN_P2ERR(CheckBuf(sz));
		if (sz == 0 || p[sz - 1] != '\0') return P2ERR_MSG_BAD_PARSE_BLOCK; 
		curIndex += sz;
		*ppc = p;
		if (pSize != NULL)
		{
			*pSize = sz;
		}
		return P2ERR_OK;
	}
	inline P2ERR ParseW(const P2WCHAR_T **ppw, size_t *pSize = NULL)
	{
		UINT32 sz;
		RETURN_P2ERR(Parse(&sz));
		const P2WCHAR_T *p = reinterpret_cast<const P2WCHAR_T *>(static_cast<const UINT8 *>(buf) + curIndex);
		RETURN_P2ERR(CheckBuf(sz));
		if (sz < sizeof(P2WCHAR_T) || p[(sz / sizeof(P2WCHAR_T)) - 1] != L'\0') return P2ERR_MSG_BAD_PARSE_BLOCK;
		curIndex += sz;
		*ppw = p;
		if (pSize != NULL)
		{
			*pSize = sz;
		}
		return P2ERR_OK;
	}
	inline P2ERR ParseC0(const char** ppc, size_t *pSize = NULL)
	{ 
		UINT32 sz;
		RETURN_P2ERR(Parse(&sz));
		if(sz == NULL_POOL_OFFSET)
		{
			*ppc = NULL;
			if(pSize) *pSize = 0; 
		}
		else
		{
			const char *p = static_cast<const char *>(buf) + curIndex;
			RETURN_P2ERR(CheckBuf(sz));
			if (sz == 0 || p[sz - 1] != '\0') return P2ERR_MSG_BAD_PARSE_BLOCK; 
			curIndex += sz; 
			*ppc = p;
			if (pSize) *pSize = sz; 
		} 
		
		return P2ERR_OK; 
	}
	
	inline void ParseC0Throw(const char** ppc, size_t *pSize = NULL) 
	{ 
		// todo Дополнительные проверки пока не вставляем, чтоб понять выигрыш от использования throw.
		UINT32 sz;
		ParseThrow(&sz);
		if(sz == NULL_POOL_OFFSET)
		{
			*ppc = NULL;
			if(pSize) *pSize = 0; 
		}
		else
		{
			const char *p = static_cast<const char *>(buf) + curIndex;
			CheckBufThrow(sz); 
			if (sz == 0 || p[sz - 1] != '\0') throw P2ERR_MSG_BAD_PARSE_BLOCK; 
			curIndex += sz; 
			*ppc = p;
			if(pSize) *pSize = sz; 
		} 
	}

// 	inline void CopyC0Throw(char* pc, size_t _size) 
// 	{
// 		UINT32 sz;
// 		ParseThrow(&sz);
// 		if(sz == NULL_POOL_OFFSET)
// 		{
// 			memset(pc, 0, _size);
// 			return;
// 		}
// 		if(sz == 0)
// 		{
// 			throw P2ERR_COMMON_BAD_STRING;
// 		}
// 		// далее расчитываю что sz > 0;
// 		CheckBufThrow(sz);
// 		// даже если последний символ буфера строки не 0, мы все равно в результирующий буфер положим 0.
// 		if(sz > _size)
// 		{
// 			throw P2ERR_COMMON_BUFFER_TOO_SMALL;
// 		}
// 		const char *p = static_cast<const char *>(buf) + curIndex;
// 		size_t lenTo0 = 0;
// 		for(; lenTo0 < sz - 1; ++lenTo0) // sz - 1 - значит не копирую последний символ, так как его все равно буду записывать нулем.
// 		{
// 			if(p[lenTo0] != 0)
// 			{
// 				pc[lenTo0] = p[lenTo0];
// 			}
// 			else
// 			{
// 				break;
// 			}
// 		}
// 		if(sz != lenTo0 + 1) // добавляем 1 так как мы оперируем резмером буфера вместе с последним нулем
// 		{
// 			throw P2ERR_BUFFER_SIZES_MISMATCH;
// 		}
// 		pc[lenTo0] = 0;
// 		curIndex += sz;
// 	}

	inline P2ERR ParseB(const void **ppc, size_t *pSize)
	{
		UINT32 sz;
		RETURN_P2ERR(Parse(&sz));
		const UINT8 *p = static_cast<const UINT8 *>(buf) + curIndex;
		RETURN_P2ERR(CheckBuf(sz));
		curIndex += sz;
		*ppc = p;
		*pSize = sz;
		return P2ERR_OK;
	}

	inline P2ERR ParseB0(const void **ppc, size_t *pSize)
	{
		UINT32 sz;
		RETURN_P2ERR(Parse(&sz));
		if(sz == NULL_POOL_OFFSET)
		{
			*ppc = NULL;
			*pSize = 0; 
		}
		else
		{
			const UINT8 *p = static_cast<const UINT8 *>(buf) + curIndex;
			RETURN_P2ERR(CheckBuf(sz));
			curIndex += sz;
			*ppc = p;
			*pSize = sz;
		}
		return P2ERR_OK;
	}

	inline P2ERR ParseZ(const P2ZBLOCK **ppz)
	{
		const UINT8 *p = static_cast<const UINT8 *>(buf) + curIndex;
		UINT32 sz;
		RETURN_P2ERR(Parse(&sz));
		const P2ZBLOCK *pz = reinterpret_cast<const P2ZBLOCK *>(p);
		RETURN_P2ERR(CheckBuf(sz));
		curIndex += sz;
		*ppz = pz;
		return P2ERR_OK;
	}

	inline P2ERR ParseBCD(const void **ppbcd, size_t *pSize = NULL)
	{ 
		const UINT8 *p = static_cast<const UINT8 *>(buf) + curIndex;
		RETURN_P2ERR(CheckBuf(sizeof(P2BCDIIStoreType)));
		P2BCDIIType type = P2BCDIIGetType(p);
		UINT32 sz = P2BCDIISIZETF(type);
		RETURN_P2ERR(CheckBuf(sz));
		curIndex += sz;
		*ppbcd = p;
		if (pSize != NULL)
		{
			*pSize = sz;
		}
		return P2ERR_OK;
	}

	inline P2ERR ParseT(P2TIME *pT)
	{
		const size_t sizeOfP2TIME = 9;
		RETURN_P2ERR(CheckBuf(sizeOfP2TIME));
		ParseNoCheck<UINT16>(&pT->year);
		ParseNoCheck<UINT8>(&pT->month);
		ParseNoCheck<UINT8>(&pT->day);
		ParseNoCheck<UINT8>(&pT->hour);
		ParseNoCheck<UINT8>(&pT->minute);
		ParseNoCheck<UINT8>(&pT->second);
		ParseNoCheck<UINT16>(&pT->msec);
		return P2ERR_OK;
	}
	inline void ParseTThrow(P2TIME *pT) 
	{
		const size_t sizeOfP2TIME = 9;
		CheckBufThrow(sizeOfP2TIME);
		ParseNoCheck<UINT16>(&pT->year);
		ParseNoCheck<UINT8>(&pT->month);
		ParseNoCheck<UINT8>(&pT->day);
		ParseNoCheck<UINT8>(&pT->hour);
		ParseNoCheck<UINT8>(&pT->minute);
		ParseNoCheck<UINT8>(&pT->second);
		ParseNoCheck<UINT16>(&pT->msec);
	}
};


#undef FORCE_INLINE
#undef FORCE_NOINLINE

#endif // _FASTBINCOMPOSER_H_
