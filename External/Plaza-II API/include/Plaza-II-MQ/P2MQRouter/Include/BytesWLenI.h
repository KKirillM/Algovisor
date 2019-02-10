// BytesWLenRC.h: interface for the CBytesWLenI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BYTESWLENRC_H__014A1046_F960_4C5C_9E97_16F129481A4C__INCLUDED_)
#define AFX_BYTESWLENRC_H__014A1046_F960_4C5C_9E97_16F129481A4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IBytesWLen.h"
#include "DataWLen.h"
#include "../../../Plaza-II/P2SysMsg/Include/P2SysMsg.h"
#include "LLMsg.h" 
#include "../../../Plaza-II/P2SysLog/Include/P2SysLog.hpp"



//////////////////////////////////////////////////////////////////////
// CBytesWLenI

class CBytesWLenI : public IBytesWLen,
                    protected CBytesWLen    // not public!
{
public:
	CBytesWLenI();

    // IBytesWLen interface
    P2ERR SetData(const void* pData, size_t len);
    P2ERR SetData(const IBytesWLen* pISrc);
    BYTE* GetData() const;
	const BYTE* GetDataForRead() const;
    size_t GetLen() const;
    P2ERR AllocateData(size_t len);
    P2ERR ReallocateData(size_t len);

    IRC_IMPLEMENT2()

protected:
    virtual ~CBytesWLenI(); // by RC

private:
    void FreeData();
};

//////////////////////////////////////////////////////////////////////

inline CBytesWLenI::CBytesWLenI()
: IRC_INIT2()
{
}

inline CBytesWLenI::~CBytesWLenI()
{
}


inline P2ERR CBytesWLenI::SetData(const void* pData, size_t len)
{
    return CBytesWLen::SetData(pData, len);
}

inline P2ERR CBytesWLenI::SetData(const IBytesWLen* pISrc)
{
    return SetData(pISrc->GetDataForRead(), pISrc->GetLen());
}

inline BYTE* CBytesWLenI::GetData() const
{
    return CBytesWLen::GetData();
}

inline const BYTE* CBytesWLenI::GetDataForRead() const
{
    return CBytesWLen::GetDataForRead();
}

inline size_t CBytesWLenI::GetLen() const
{
    return CBytesWLen::GetLen();
}

inline P2ERR CBytesWLenI::AllocateData(size_t len)
{
    return CBytesWLen::AllocateData(len);
}

inline void CBytesWLenI::FreeData()
{
    CBytesWLen::FreeData();
}

inline P2ERR CBytesWLenI::ReallocateData(size_t len)
{
    return CBytesWLen::ReallocateData(len);
}

//////////////////////////////////////////////////////////////////////
// CBytesWLenPriorityI

class CBytesWLenPriorityI : public IBytesWLenPriority,
                            protected CBytesWLen    // not public!
{
public:
	CBytesWLenPriorityI(BYTE priority);
	CBytesWLenPriorityI();
    CBytesWLenPriorityI(const IBytesWLenPriority* pIBytes);

    // IBytesWLenPriority interface
    P2ERR SetData(const void* pData, size_t len);
    P2ERR SetData(const IBytesWLen* pISrc);
    BYTE* GetData() const;
    const BYTE* GetDataForRead() const;
    size_t GetLen() const;
    P2ERR AllocateData(size_t len);
    P2ERR ReallocateData(size_t len);
    BYTE GetPriority() const;
    void SetPriority(BYTE);

    IRC_IMPLEMENT2()

private:
    void FreeData();

    BYTE    m_priority; 
protected:
    virtual ~CBytesWLenPriorityI();    // by RC
};

//////////////////////////////////////////////////////////////////////

inline CBytesWLenPriorityI::CBytesWLenPriorityI(BYTE priority)
: IRC_INIT2(),
  m_priority(priority)
{
}

inline CBytesWLenPriorityI::CBytesWLenPriorityI()
: IRC_INIT2(),
  m_priority(P2MSG_PRIORITY_DEFAULT)
{
}


inline CBytesWLenPriorityI::CBytesWLenPriorityI(const IBytesWLenPriority* pIBytes)
: IRC_INIT2(),
  m_priority(pIBytes->GetPriority())
{
}

inline CBytesWLenPriorityI::~CBytesWLenPriorityI()
{
	return;
}


inline P2ERR CBytesWLenPriorityI::SetData(const void* pData, size_t len)
{
    return CBytesWLen::SetData(pData, len);
}

inline P2ERR CBytesWLenPriorityI::SetData(const IBytesWLen* pISrc)
{
    return SetData(pISrc->GetDataForRead(), pISrc->GetLen());
}

inline BYTE* CBytesWLenPriorityI::GetData() const
{
    return CBytesWLen::GetData();
}

inline const BYTE* CBytesWLenPriorityI::GetDataForRead() const
{
    return CBytesWLen::GetDataForRead();
}

inline size_t CBytesWLenPriorityI::GetLen() const
{
    return CBytesWLen::GetLen();
}

inline P2ERR CBytesWLenPriorityI::AllocateData(size_t len)
{
    return CBytesWLen::AllocateData(len);
}

inline void CBytesWLenPriorityI::FreeData()
{
    CBytesWLen::FreeData();
}

inline BYTE CBytesWLenPriorityI::GetPriority() const
{
    return m_priority;
}
inline void CBytesWLenPriorityI::SetPriority(BYTE priority) 
{
    m_priority = priority;
}

inline P2ERR CBytesWLenPriorityI::ReallocateData(size_t len)
{
    return CBytesWLen::ReallocateData(len);
}


inline CBytesWLenPriorityI* CreateCBytesWLen(UINT32 system_type, const void* pSrc, size_t len, UINT32 flags)
{ 
    CBytesWLenPriorityI* pItem = new (std::nothrow) CBytesWLenPriorityI(LLMsgGetPriority(flags));

	if (!pItem)
		return NULL;

    if (P2FAILED(pItem->AllocateData(sizeof(TLLMsgHdr) + len)))
    {
        pItem->Release1();
        return NULL;
    }

    LLMsgHdrSet(reinterpret_cast<TLLMsgHdr*>(pItem->GetData()), system_type, len, flags);

    if (len > 0)
	{
        memcpy(pItem->GetData() + sizeof(TLLMsgHdr), pSrc, len);
	}

	return pItem; 
}



////////////////////////////////////////////////////////////////////////////////////////////
//   class CLLMsgBuffer: TLLMsgHdr + body of the llmsg
//   hold preallocated array for data
//   
class CLLMsgBuffer: public IBytesWLenPriority
{
	BYTE* 		m_pBuf;
	size_t 	m_bufSize;
	BYTE		prealloc[256];
public:
	CLLMsgBuffer():m_pBuf(0),m_bufSize(0),IRC_INIT2() {}

	P2ERR SetData(const void* pData,size_t len)
	{ 
		RETURN_P2ERR(AllocateData(len));
		if(m_pBuf != NULL && len > 0)
			memcpy(m_pBuf,pData,len);
		return P2ERR_OK; 
	}

	P2ERR SetData(const IBytesWLen *pISrc)
	{
		return SetData(pISrc->GetDataForRead(), pISrc->GetLen());
	}

	BYTE* GetData() const    				{return m_pBuf;}
	const BYTE* GetDataForRead() const    	{return m_pBuf;}
	size_t GetLen() const    				{return m_bufSize;}

	P2ERR AllocateData(size_t len)
	{
		if(len == 0) 
		{
			m_pBuf = 0;
			m_bufSize = 0;
		}
		else if(len <= sizeof(prealloc)) 
		{
			m_pBuf = &prealloc[0];
			m_bufSize = len;
		}
		else 
		{
			 m_pBuf = reinterpret_cast<BYTE*>(malloc(len));
			 if(!m_pBuf)
				 return P2ERR_COMMON_NO_MEMORY;
			 m_bufSize = len;
		}

		return P2ERR_OK; 
	}

	P2ERR ReallocateData(size_t len)
	{
		//not optimal (not used realloc/ malloc only)
		P2ERR res;
		BYTE* pOldBuf = m_pBuf;
		size_t oldSize = m_bufSize;
		if(P2SUCCEEDEDV(AllocateData(len),res)) {
			if(m_pBuf && pOldBuf && __min(oldSize,m_bufSize) > 0)
				memmove(m_pBuf,pOldBuf,__min(oldSize,m_bufSize));
		}

		if(pOldBuf != &prealloc[0])
			free(pOldBuf); 
		return res; 
	}

	//todo:
	void SetPriority(BYTE) {}
	BYTE GetPriority() const {return P2LLMSG_PRIORITY_DEFAULT;}

    IRC_IMPLEMENT2()

	private:
		virtual ~CLLMsgBuffer() //By RC
		{ 
			FreeData();
		}

		void FreeData()
		{
			if(m_pBuf != &prealloc[0]) 
				free(m_pBuf);
			m_pBuf = 0;
			m_bufSize = 0;
		}
};

////////////////////////////////////////////////////////////////////////////////
// 	класс, который содержит внутри композер. Используется для
//	композинга сендов/реплаев и передачи данных из композерa как пакета с данными. Сделано с целью экономии аллокаций
class CComposerAsIBytesWLenPriority : public IBytesWLenPriority
{ 
	P2_ComposeState m_composer;
	//(m_pBuf != 0 & freed(composer)   -- use m_pBuf for data 
	//(m_pBuf == 0 & Inited(composer)) -- use composer for data
 	BYTE			*m_pBuf;   
	size_t			m_bufSize; 

public:
	CComposerAsIBytesWLenPriority():m_pBuf(0),m_bufSize(0),IRC_INIT2() {P2MsgInitCompose(&m_composer);} 

	P2ERR AllocateData(size_t len)
	{
		if(len == 0)
			return P2ERR_OK; 
		return ReallocateData(len);
	}

	P2ERR SetData(const void* pData,size_t len)
	{ 
		RETURN_P2ERR(AllocateData(len));
		if(pData && len > 0)
			memcpy(m_pBuf,pData,len);
		return P2ERR_OK; 
	}

	P2ERR SetData(const IBytesWLen *pISrc) 
	{
		return SetData(pISrc->GetDataForRead(), pISrc->GetLen());
	}

	BYTE* GetData() const 
	{ 
		return m_pBuf ? m_pBuf : static_cast<BYTE*>(m_composer.buf);
	}

	const BYTE* GetDataForRead() const 
	{ 
		return m_pBuf ? m_pBuf : static_cast<BYTE*>(m_composer.buf);
	}

	P2_ComposeState* GetComposer() 
	{
		if(m_pBuf) 
		{
			P2TRACE_ERROR_WITH_CODE_EX((P2ERR_COMMON_WRONG_ARGUMENTS,"CLLMsgComposer","Incorrect use of CSendMsgComposerClass"));
			return 0;
		}
		return &m_composer;
	} 

	size_t GetLen() const 
	{
		return m_pBuf ? m_bufSize : m_composer.size;
	}

	P2ERR ReallocateData(size_t len)
	{
		if(0 == len)
			return P2ERR_OK;
		if(m_pBuf) 
		{
			BYTE* pBuf = reinterpret_cast<BYTE*>(realloc(m_pBuf,len)); 
			if(!pBuf) return P2ERR_COMMON_NO_MEMORY; 
			m_pBuf = pBuf;
			m_bufSize = len; 
		}
		else 
		{
			m_pBuf = reinterpret_cast<BYTE*>(malloc(len));
			if(!m_pBuf)
				return P2ERR_COMMON_NO_MEMORY;
			memcpy(m_pBuf,m_composer.buf,__min(m_composer.size,len));
			m_bufSize = len; 
			P2MsgFreeCompose(&m_composer); //destroy all old data
		}
		return P2ERR_OK; 
	} 

    IRC_IMPLEMENT2()

	void SetPriority(BYTE) {}

	BYTE GetPriority() const 
	{
		return P2LLMSG_PRIORITY_DEFAULT;
	}

protected:
   	virtual ~CComposerAsIBytesWLenPriority() // by RC
	{ 
		FreeData();
	}

private:
	void FreeData() 
	{ 
		if(m_pBuf) 
		{
			free(m_pBuf);
			m_pBuf = 0;
			m_bufSize = 0;
		}
		else 
		{ 
			P2MsgFreeCompose(&m_composer);
		}
	}
};


// CLogableBytesWLen_Base 
// Базовый класс для сообщений, которые можно распечатать в лог через P2LogCustComposer.  

class CLogableBytesWLen_Base : public IBytesWLenPriority,
							   public ILogComposer
{
	// данные для отправки через интерфейс IBytesWLenPriority. Заполняются copy-on-write в GetData из m_pLogableData
	mutable BYTE*  m_pData;
	mutable size_t m_dataSz; 

	// данные для вывода в лог через интерфейс ILogComposer
	IBytesWLen* m_pLogableData;

protected:

	CLogableBytesWLen_Base(): m_pData(NULL), m_dataSz(0), m_pLogableData(NULL)
	{
	}

	CLogableBytesWLen_Base(IBytesWLen* pIBytes): m_pData(NULL), m_dataSz(0), m_pLogableData(pIBytes)
	{
		if(m_pLogableData) m_pLogableData->AddRef1();
	}


	~CLogableBytesWLen_Base()
	{
		if(m_pLogableData) m_pLogableData->Release1();
		delete [] m_pData;
	}

public: 

	// Ф-ия для создания и получения данных без copy-on-write 
    BYTE* GetLogableData() const
	{
		_ASSERT(m_pLogableData != NULL);
		return m_pLogableData->GetData();
	} 

    size_t GetLogableDataLen() const
	{
		_ASSERT(m_pLogableData != NULL);
		return m_pLogableData->GetLen();
	}

    P2ERR AllocateLogableData(size_t len)
	{
		_ASSERT( m_pLogableData == NULL );
		try
		{
			m_pLogableData = new CBytesWLenI;
			RETURN_P2ERR(m_pLogableData->AllocateData(len));
		}
		catch(std::bad_alloc&)
		{
			return P2ERR_COMMON_NO_MEMORY; 
		}
		return P2ERR_OK;
	} 

	void SetLogableData(IBytesWLen* pIBytes)
	{
		_ASSERT( m_pLogableData == NULL );
		m_pLogableData = pIBytes;
		if(m_pLogableData) m_pLogableData->AddRef1(); 
	}

    // ф-ии для отправки данных : IBytesWLen interface  
    P2ERR SetData(const void* pData, size_t len)
	{
		try
		{
			if(len == 0 || pData == NULL) 
				return P2ERR_OK;

			if(m_pData != NULL)
			{
				delete [] m_pData;
			}

			m_pData = new BYTE[len];
			m_dataSz = len;
			memcpy(m_pData, pData, len); 
		}
		catch(std::bad_alloc&)
		{
			return P2ERR_COMMON_NO_MEMORY;
		}	

		return P2ERR_OK; 
	}

    P2ERR SetData(const IBytesWLen* pISrc)
	{
		return SetData(pISrc->GetDataForRead(), pISrc->GetLen());  
	}

    BYTE* GetData() const   // can throws bad_alloc
	{ 
		_ASSERT(m_pLogableData != NULL );
		if(m_pData == NULL)
		{
			m_pData = new BYTE[ GetLogableDataLen() ];
			m_dataSz = GetLogableDataLen();
			memcpy(m_pData, GetLogableData(), GetLogableDataLen());
		}

		return m_pData; 
	}

    const BYTE* GetDataForRead() const   
	{ 
		if(m_pData == NULL)
		{
			return m_pLogableData->GetDataForRead();
		}
		else
		{ 
			return m_pData; 
		} 
	}

    size_t GetLen() const
	{
		if(m_pData == NULL)
		{
			return GetLogableDataLen();
		}
		else
		{
			return m_dataSz;
		} 
	}

    P2ERR AllocateData(size_t len)
	{
		// это ф-ия не должна вызываться, должна вызываться ф-ия  AllocateLogableData, данные для отправки копируеются из m_pLogableData
		_ASSERT(false);
		return P2ERR_COMMON_INTERNAL_ERROR; 
	}

	// TODO: убрать из интерфейса
    P2ERR ReallocateData(size_t len)
	{
		try
		{ 
			if(m_pData == NULL)
			{
				m_pData = new BYTE[__max(GetLogableDataLen(), len)];
				m_dataSz = len;
				memcpy(m_pData, GetLogableData(), GetLogableDataLen()); 
			} 
			else if(len > m_dataSz)
			{
				BYTE* const pNewData = new BYTE[len];
				memcpy(pNewData, m_pData, m_dataSz);
				m_dataSz = len;
				delete [] m_pData;
				m_pData = pNewData; 
			} 
		}
		catch(std::bad_alloc&)
		{
			return P2ERR_COMMON_NO_MEMORY; 
		}

		return P2ERR_OK; 
	}	


	// priority is not used
	void SetPriority(BYTE) {}
	BYTE GetPriority() const {return P2LLMSG_PRIORITY_DEFAULT;} 
};

#endif // !defined(AFX_BYTESWLENRC_H__014A1046_F960_4C5C_9E97_16F129481A4C__INCLUDED_)
