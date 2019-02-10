// DataWLen.h: interface for the CBytesWLen class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATAWLEN_H__725C7B59_36EB_438C_B0CB_B67430CF6916__INCLUDED_)
#define AFX_DATAWLEN_H__725C7B59_36EB_438C_B0CB_B67430CF6916__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../Plaza-II/P2Utils/Include/stuff.h"
#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#ifdef __MACH__
#include <sys/malloc.h>
#else
#include <malloc.h>
#endif // __MACH__

//////////////////////////////////////////////////////////////////////
// CBytesWLen
//
//  - Length can be 0 (in this case pData == 0)!
//

class CBytesWLen  
{
public:
    CBytesWLen();
    virtual ~CBytesWLen();

    P2ERR SetData(const void* pData, size_t len);
    P2ERR SetData(const CBytesWLen& src);

    BYTE* GetData() const;
	const BYTE* GetDataForRead() const;
    size_t GetLen() const;

    P2ERR AllocateData(size_t len);
    P2ERR ReallocateData(size_t len);

protected: 

    void FreeData();

private:

    BYTE*	    m_pData;
    size_t	m_len;

    DISABLE_CLASS_COPY_PV(CBytesWLen)
};

//////////////////////////////////////////////////////////////////////

inline CBytesWLen::CBytesWLen()
: m_pData(0), m_len(0)
{
}

inline CBytesWLen::~CBytesWLen()
{
	FreeData();
}

inline P2ERR CBytesWLen::SetData(const void* pData, size_t len)
{
    if (len > 0)
    {
        _ASSERT(pData);
        if (!pData) return P2ERR_COMMON_WRONG_ARGUMENTS;
        P2ERR res = AllocateData(len);
        if (P2FAILED(res)) return res;
        memcpy(m_pData, pData, len);
    }
    else
    {
        FreeData();
    }
    return P2ERR_OK;
}

inline P2ERR CBytesWLen::SetData(const CBytesWLen& src)
{
    return SetData(src.m_pData, src.m_len);
}

inline BYTE* CBytesWLen::GetData() const
{
	return m_pData;
}


inline const BYTE* CBytesWLen::GetDataForRead() const
{
	return m_pData;
}

inline size_t CBytesWLen::GetLen() const
{
	return m_len;
}

inline P2ERR CBytesWLen::AllocateData(size_t len)
{
    return ReallocateData(len);
}

inline P2ERR CBytesWLen::ReallocateData(size_t len)
{
    void *pNew = realloc(m_pData, len);
    if (pNew == NULL && len > 0) return P2ERR_COMMON_NO_MEMORY;
    m_pData = static_cast<BYTE*>(pNew);
    m_len = len;
    return P2ERR_OK;
}

inline void CBytesWLen::FreeData()
{
    if (m_pData)
    {
        free(m_pData);
        m_pData = 0;
    }
    m_len = 0;
}

//////////////////////////////////////////////////////////////////////


#endif // !defined(AFX_DATAWLEN_H__725C7B59_36EB_438C_B0CB_B67430CF6916__INCLUDED_)
