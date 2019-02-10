// interface IBytesWLen
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IBYTESWLEN_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_)
#define AFX_IBYTESWLEN_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II/P2Utils/Include/IRc.h"
#include "../../../Plaza-II/P2Utils/Include/QueueMan.h"

//////////////////////////////////////////////////////////////////////
// IBytesWLen

struct IBytesWLen : public IRc, public CQueueElem
{
    virtual P2ERR SetData(const void* pData, size_t len) = 0;
    virtual P2ERR SetData(const IBytesWLen* pISrc) = 0;

    virtual BYTE*  GetData() const = 0;
	virtual const BYTE* GetDataForRead() const = 0;
    virtual size_t GetLen() const = 0;

    virtual P2ERR AllocateData(size_t len) = 0;
    virtual P2ERR ReallocateData(size_t len) = 0;

    CHECK_FOR_DELETE__(IBytesWLen)
};

//////////////////////////////////////////////////////////////////////
// IBytesWLenPriority

struct IBytesWLenPriority : public IBytesWLen
{
    virtual BYTE GetPriority() const = 0;
    virtual void SetPriority(BYTE)  = 0;

    CHECK_FOR_DELETE__(IBytesWLenPriority)
};

#endif // !defined(AFX_IBYTESWLEN_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_)
