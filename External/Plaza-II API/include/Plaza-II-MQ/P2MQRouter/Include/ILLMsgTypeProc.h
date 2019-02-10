// interface ILLMsgTypeProc
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ILLMSGTYPEPROC_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_)
#define AFX_ILLMSGTYPEPROC_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II/P2Common/Include/P2Types.h"
#include "IMsgConnection.h"
#include "LLMsg.h"

//////////////////////////////////////////////////////////////////////
// ILLMsgTypeProc

struct ILLMsgTypeProc
{
    virtual P2ERR Process(PCTLLMsgHdr pHdr, const void* pBody, IMsgConnection* pIMsgConn) = 0;

    CHECK_FOR_DELETE__(ILLMsgTypeProc)
};

// второй интерфейс был придуман для обеспечения эффективной работы с памятью
struct ILLMsgTypeProc2
{
    virtual P2ERR Process(IBytesWLenPriority *pIBytes, IMsgConnection* pIMsgConn) = 0;

    CHECK_FOR_DELETE__(ILLMsgTypeProc2)
};



#endif // !defined(AFX_ILLMSGTYPEPROC_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_)
