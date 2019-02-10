#ifndef _P2SYSTYPES_HPP_
#define _P2SYSTYPES_HPP_

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "P2SysTypes.h"
#include <assert.h>
#include <memory.h>
#include <stdio.h>

#if defined(__linux__) || defined(__MACH__)
#include <wchar.h>
#include <unistd.h>
#endif

#include "../../P2Utils/Include/AutoHandle.hpp"
#include "../../P2Utils/Include/TypeSafeSwitch.h"
#include "../../P2Utils/Include/P2Str.hpp"
#include "../../P2SysLog/Include/P2SysLog.h"

#ifdef _MSC_VER
//#pragma warning( disable : 4267)	// Delete when come to 64 bit OS
#endif

template<char chType>
struct P2TypeTraits
{
	//I/U/1/2/4/8, B
	static size_t memorySize(size_t size, size_t /*prec*/, const void * /*data*/ )
	{
		return size;
	}
	static size_t memoryMaxSize(size_t size, size_t /*prec*/)
	{
		return size;
	}
//	static UINT32 printableMaxSize(UINT32 size, UINT32 prec)
//    {
//		return size;
//    }
};

template<>
struct P2TypeTraits<'c'>
{
        static size_t memorySize(size_t /*size*/, size_t /*prec*/, const void *data)
	{
		const char *s = static_cast<const char *>(data);
		if (s == NULL)
		{
			return 0;
		}
		return strlen(s) + 1;
	}
	static size_t memoryMaxSize(size_t size, size_t /*prec*/)
	{
		return size + 1;
	}
};

template<>
struct P2TypeTraits<'t'>
{
	static size_t memorySize(size_t /*size*/, size_t /*prec*/, const void * /*data*/)
	{
		return sizeof(P2TIME);
	}
	static size_t memoryMaxSize(size_t /*size*/, size_t /*prec*/)
	{
		return sizeof(P2TIME);
	}
//	static UINT32 printableMaxSize(UINT32 , UINT32 )
//    {
//        // YYYY/MM/DD HH:NN:SS.mmm
//        return 23;
//    }
};

template<>
struct P2TypeTraits<'s'>
{
	static size_t memorySize(size_t size, size_t prec, const void * /*data*/)
	{
		return P2BCDIISIZETF(P2BCDIIMAKETYPE(size, prec));
	}
	static size_t memoryMaxSize(size_t size, size_t prec)
	{
		return P2BCDIISIZETF(P2BCDIIMAKETYPE(size, prec));
	}
//	static UINT32 printableMaxSize(UINT32 size, UINT32 )
//    {
//        return size + 3;
//    }

	static size_t makeTypeDescr(size_t size, size_t prec)
	{
		return P2BCDIIMAKETYPE(size, prec);
	}
};

template<>
struct P2TypeTraits<'d'>
{
	static size_t memorySize(size_t size, size_t prec, const void * /*data*/)
	{
		return P2BCDIISIZETF(P2BCDIIMAKETYPE(size, prec));
	}
	static size_t memoryMaxSize(size_t size, size_t prec)
	{
		return P2BCDIISIZETF(P2BCDIIMAKETYPE(size, prec));
	}
//	static UINT32 printableMaxSize(UINT32 size, UINT32 )
//    {
//        return size + 3;
//    }

	static size_t makeTypeDescr(size_t size, size_t prec)
	{
		return P2BCDIIMAKETYPE(size, prec);
	}
};

/*template<>
struct P2TypeTraits<'i'>
{
	static UINT32 printableMaxSize(UINT32 size, UINT32 prec)
    {
		return ;
    }
};

template<>
struct P2TypeTraits<'u'>
{
	static UINT32 printableMaxSize(UINT32 size, UINT32 prec)
    {
		return ;
    }
};

template<>
struct P2TypeTraits<'b'>
{
	static UINT32 printableMaxSize(UINT32 size, UINT32 )
    {
        // for hex representation
		return size * 2;
    }
};*/


template<>
struct P2TypeTraits<'a'>
{
	static size_t memorySize(size_t /*size*/, size_t /*prec*/, const void * /*data*/)
	{
		return sizeof(UINT8);
	}
	static size_t memoryMaxSize(size_t /*size*/, size_t /*prec*/)
	{
		return sizeof(UINT8);
	}
};

template<>
struct P2TypeTraits<'f'>
{
	static size_t memorySize(size_t /*size*/, size_t /*prec*/, const void * /*data*/)
	{
		return sizeof(double);
	}
	static size_t memoryMaxSize(size_t /*size*/, size_t /*prec*/)
	{
		return sizeof(double);
	}
};

template<>
struct P2TypeTraits<'w'>
{
	static size_t memorySize(size_t /*size*/, size_t /*prec*/, const void *data)
	{
		const P2WCHAR_T *s = static_cast< const P2WCHAR_T*> (data);
		if (NULL == s)
		{
			return 0;
		}
		return (P2WcsLen(s) + 1) * sizeof(P2WCHAR_T);
	}
    
    static size_t memoryMaxSize(size_t size, size_t /*prec*/)
	{
		return (size + 1) * sizeof(P2WCHAR_T);
	}
};

template<>
struct P2TypeTraits<'z'>
{
	static size_t memorySize(size_t /*size*/, size_t /*prec*/, const void* pd )
	{
//		return ( static_cast<const P2ZBLOCK*>(data)->data-static_cast<const UINT8*>(data) )
//				+ static_cast<const P2ZBLOCK*>(data)->size;
		return offsetof( P2ZBLOCK, data ) + static_cast<const P2ZBLOCK*>(pd)->size;
	}
	static size_t memoryMaxSize(size_t size, size_t /*prec*/)
	{
		size_t realSize = offsetof( P2ZBLOCK, data ) + size;
		size_t rem = realSize % 4;
		if (rem != 0)
		{
			realSize = realSize + 4 - rem;
		}
		return realSize;
	}
};



//////////////////////////////////////////////////////////////////////

inline size_t P2TypeMemorySize(const char *type, size_t size, size_t prec, const void *data)
{
	switch (type[0])
	{
	    T_CASE_C
		    return P2TypeTraits<'c'>::memorySize(size, prec, data);

	    T_CASE_I
		    return P2TypeTraits<'i'>::memorySize(size, prec, data);

	    T_CASE_U
		    return P2TypeTraits<'u'>::memorySize(size, prec, data);

	    T_CASE_B
		    return P2TypeTraits<'b'>::memorySize(size, prec, data);

	    T_CASE_D
		    return P2TypeTraits<'d'>::memorySize(size, prec, data);

	    T_CASE_S
		    return P2TypeTraits<'s'>::memorySize(size, prec, data);

	    T_CASE_T
		    return P2TypeTraits<'t'>::memorySize(size, prec, data);

	    T_CASE_A
		    return P2TypeTraits<'a'>::memorySize(size, prec, data);

	    T_CASE_F
		    return P2TypeTraits<'f'>::memorySize(size, prec, data);

	    T_CASE_W
		    return P2TypeTraits<'w'>::memorySize(size, prec, data);

     	T_CASE_Z
     		return P2TypeTraits<'z'>::memorySize(size, prec, data);
        
        default:
	        T_CHECK
            return 0;
	}
}

inline size_t P2TypeMemoryMaxSize(const char *type, size_t size, size_t prec)
{
	switch (type[0])
	{
	    T_CASE_C
		    return P2TypeTraits<'c'>::memoryMaxSize(size, prec);

	    T_CASE_I
		    return P2TypeTraits<'i'>::memoryMaxSize(size, prec);

	    T_CASE_U
		    return P2TypeTraits<'u'>::memoryMaxSize(size, prec);

	    T_CASE_B
		    return P2TypeTraits<'b'>::memoryMaxSize(size, prec);

	    T_CASE_D
		    return P2TypeTraits<'d'>::memoryMaxSize(size, prec);

	    T_CASE_S
		    return P2TypeTraits<'s'>::memoryMaxSize(size, prec);

	    T_CASE_T
		    return P2TypeTraits<'t'>::memoryMaxSize(size, prec);

	    T_CASE_A
		    return P2TypeTraits<'a'>::memoryMaxSize(size, prec);

	    T_CASE_F
		    return P2TypeTraits<'f'>::memoryMaxSize(size, prec);

	    T_CASE_W
		    return P2TypeTraits<'w'>::memoryMaxSize(size, prec);

	    T_CASE_Z
		    return P2TypeTraits<'z'>::memoryMaxSize(size, prec);

        default:
            T_CHECK
            return 0;
	}
}

inline size_t P2TypePrintableMaxSize(const char *type, size_t size, size_t /*prec*/)
{
	switch (type[0])
	{
	    T_CASE_C
	    T_CASE_W
		    return size;

	    T_CASE_I
            switch (size)
            {
                case 1: return 4;
                case 2: return 6;
                case 4: return 11;
                case 8: return 21;
            }
            break;

	    T_CASE_U
            switch (size)
            {
                case 1: return 3;
                case 2: return 5;
                case 4: return 10;
                case 8: return 20;
            }
            break;

	    T_CASE_B
	    T_CASE_Z
            // for hex representation
		    return size * 2;

	    T_CASE_D
	    T_CASE_S
            return size + 3;

	    T_CASE_T
            // YYYY/MM/DD HH:NN:SS.mmm
            return 23;

        T_CASE_A
            return 1;

        T_CASE_F
            return 22; // знак(+/-), десятичная точка, буковка E (обозначающая порядок), знак порядка, три цифры порядка, остальное - цифры мантисы
                       // 1+1+1+1+3+не знаю сколько, наверно 15
        default:
            T_CHECK
            break;
	}
    return 0;
}

inline P2ERR P2TypeZero(const char *type, size_t size, size_t prec, void *data)
{
	if( !( type && data ) ) 
	{
		return P2ERR_COMMON_WRONG_ARGUMENTS;
	}
	switch( type[0] ) 
	{
		// case 'c': case 'i':	case 'u': case 'b':
        // каждая скобочка на отдельной строке (даже там где скобки вообще не нужны), но при этом 4 кейса в одну строку - это круто!!!
        // оставлено для музея
        T_CASE_C
		T_CASE_W
        T_CASE_I
        T_CASE_U
        T_CASE_B
		T_CASE_Z
        T_CASE_A
        T_CASE_F
			memset(data, 0, P2TypeMemoryMaxSize(type, size, prec));
			return P2ERR_OK;
		T_CASE_D
        T_CASE_S
			P2BCDIIInit(data, P2BCDIIMAKETYPE(size, prec));
			P2BCDIISetZero(data);  // а почему не SetNAN ??
			return P2ERR_OK;
		T_CASE_T
			P2TimeSetNULL(static_cast<P2TIME*>(data));
			return P2ERR_OK;
		default:
            T_CHECK
			return P2ERR_COMMON_WRONG_ARGUMENTS;
	}
}

inline P2ERR P2TypeZeroEx(const char *type, size_t size, size_t prec, void *data, size_t dataSize)
{
	if( !( type && data ) ) 
	{
		return P2ERR_COMMON_WRONG_ARGUMENTS;
	}

	size_t maxSize = P2TypeMemoryMaxSize(type, size, prec);
	if ((maxSize > dataSize) && (type[0] != 'c') && (type[0] != 'w'))
	{
		return P2ERR_COMMON_BUFFER_TOO_SMALL;
	}

	switch( type[0] ) 
	{
		T_CASE_C
		T_CASE_W
			memset(data, 0, dataSize);
			return P2ERR_OK;
        T_CASE_I
        T_CASE_U
        T_CASE_B
		T_CASE_Z
			memset(data, 0, maxSize);
			return P2ERR_OK;
		T_CASE_D
        T_CASE_S
			P2BCDIIInit(data, P2BCDIIMAKETYPE(size, prec));
			P2BCDIISetZero(data);
			return P2ERR_OK;
		T_CASE_T
			P2TimeSetNULL(static_cast<P2TIME*>(data));
			return P2ERR_OK;
        T_CASE_A
            *static_cast<UINT8*>(data) = 0;
			return P2ERR_OK;
        T_CASE_F
            *static_cast<double*>(data) = 0.0; // memset ?
			return P2ERR_OK;
		default:
            T_CHECK
			return P2ERR_COMMON_WRONG_ARGUMENTS;
	}
}
//////////////////////////////////////////////////////////////////////
// Make properly aligned value

template <typename TType>
inline TType P2ReadUnaligned16(const void* ptr)
{
    assert(sizeof(TType) == 2);
    const BYTE* p = static_cast<const BYTE*>(ptr);
    return p[0] + (p[1] << 8);
}

template <typename TType>
inline TType P2ReadUnaligned32(const void* ptr)
{
    assert(sizeof(TType) == 4);
    const BYTE* p = static_cast<const BYTE*>(ptr);
    return p[0] + (p[1] << 8) + (p[2] << 2*8) + (p[3] << 3*8);
}

template <typename TType>
inline TType P2ReadUnaligned64(const void* ptr)
{
    assert(sizeof(TType) == 8);
    TType val;
    memcpy(&val, ptr, sizeof(val));
    return val;
}

static inline size_t GetAligned(size_t offset, size_t align)
{
    size_t rem = static_cast<size_t>( offset % align );
    return rem == 0 ? offset : offset + align - rem;
}

//////////////////////////////////////////////////////////////////////

// Упрощенная (по-умолчанию) трансляция флагов результата BCDII-операции
// в код P2ERR.
inline P2ERR P2BCDIIErr(UINT32 bcdRes)
{
    // Убрать не ошибочные флаги
    bcdRes &= ~(P2BCDII_RESULT_ZERO | P2BCDII_RESULT_NEGATIVE | P2BCDII_RESULT_PREC | P2BCDII_NOT_STRICT_FORMAT);

    if (bcdRes == 0) return P2ERR_OK;
    else if (bcdRes & P2BCDII_RESULT_OVERFLOW) return P2ERR_COMMON_OVERFLOW;
    else if (bcdRes & P2BCDII_OPERAND_NAN) return P2ERR_COMMON_WRONG_ARGUMENTS;
    else if (bcdRes & P2BCDII_OPERAND_BAD) return P2ERR_COMMON_WRONG_ARGUMENTS;
//    else if (bcdRes & P2BCDII_RESULT_BAD) return ; <- выставляется с др. флагами как признак, что результат неопределен
    return P2ERR_COMMON_INTERNAL_ERROR;
}

//////////////////////////////////////////////////////////////////////
// внутренняя функция для перевода флагов в ошибку с трейсом
inline P2ERR P2BCDIIErrTrace(UINT32 bcdRes, const char *expr, const char *file, int line)
{
    // Убрать не ошибочные флаги
    bcdRes &= ~(P2BCDII_RESULT_ZERO | P2BCDII_RESULT_NEGATIVE | P2BCDII_RESULT_PREC | P2BCDII_NOT_STRICT_FORMAT);

    if (bcdRes == 0) return P2ERR_OK;
	P2TRACE_ERROR_EX(("BCDII", "BCD fails with flags 0x%x; expr '%s';  file '%s'; line %d", bcdRes, expr, file, line));
    if (bcdRes & P2BCDII_RESULT_OVERFLOW) return P2ERR_COMMON_OVERFLOW;
    else if (bcdRes & P2BCDII_OPERAND_NAN) return P2ERR_COMMON_WRONG_ARGUMENTS;
    else if (bcdRes & P2BCDII_OPERAND_BAD) return P2ERR_COMMON_WRONG_ARGUMENTS;
//    else if (bcdRes & P2BCDII_RESULT_BAD) return ; <- выставляется с др. флагами как признак, что результат неопределен
    return P2ERR_COMMON_INTERNAL_ERROR;
}
//////////////////////////////////////////////////////////////////////
// Макрос, аналог P2BCDIIErr с записью информации об ошибке в лог
#define P2BCDIIErrT(expr) (P2BCDIIErrTrace(expr, #expr, __FILE__, __LINE__))

//////////////////////////////////////////////////////////////////////

typedef void* P2MemVoidHandle;  

inline P2ERR P2MemFree(P2MemVoidHandle pMem)
{
	free(pMem);
	return P2ERR_OK;
}

typedef AutoP2Handle<P2MemVoidHandle, P2MemFree> P2AutoMallocMemHandle;

//////////////////////////////////////////////////////////////////////

typedef PSTR P2StrHandle;

inline P2ERR P2StrDelete(P2StrHandle pStr)
{
	delete [] pStr;
	return P2ERR_OK;
}

typedef AutoP2Handle<P2StrHandle, P2StrDelete> P2AutoStrHandle;



//////////////////////////////////////////////////////////////////////

#ifdef _WIN32

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996) // warning C4996: This function or variable may be unsafe.
#endif // _MSC_VER

inline int P2HandleToStr(HANDLE h, char* buf, size_t bufSize)
{
	return _snprintf(buf, bufSize, "%p", h);
}

inline int P2StrToHandle(HANDLE* h, const char* buf)
{
	return sscanf(buf, "%p", h);
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#elif defined(__linux__) || defined(__MACH__)
inline int P2DescriptorToStr(int h, char* buf, size_t bufSize)
{
	return _snprintf(buf, bufSize, "%d", h);
}

inline int P2StrToDescriptor(int* h, const char* buf)
{
	return sscanf(buf, "%d", h);
}

#endif // _WIN32


#ifdef _WIN32

inline P2ERR P2CloseHandle(HANDLE h)
{
	if ((h != NULL) && (h != INVALID_HANDLE_VALUE))
	{
		CloseHandle(h);
	}
	return P2ERR_OK; 
}

typedef AutoP2Handle<HANDLE, P2CloseHandle> P2AutoCloseHandle;

#endif //_WIN32
//////////////////////////////////////////////////////////////////////



#endif // _P2SYSTYPES_HPP_
