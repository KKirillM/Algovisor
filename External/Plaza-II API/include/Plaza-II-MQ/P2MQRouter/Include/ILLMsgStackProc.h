// interface ILLMsgStackProc
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ILLMSGSTACKPROC_H__6FF23911_DBCE_471E_9643_C929D3119C21__INCLUDED_)
#define AFX_ILLMSGSTACKPROC_H__6FF23911_DBCE_471E_9643_C929D3119C21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IMsgConnection.h"
#include "LLMsg.h"

//////////////////////////////////////////////////////////////////////
// ILLMsgStackProc

struct ILLMsgStackProc
{
    // Return:
    //  true - continue processing by stack
    //  false - break stack processing
//    virtual bool Process(PTLLMsgHdr pHdr, const void* pBody, IMsgConnection* pIMsgConn) = 0;
    // Renamed for simple use together with type processor in the same class
//    virtual bool ProcessStack(PTLLMsgHdr pHdr, const void* pBody, IMsgConnection* pIMsgConn) = 0;
    virtual P2ERR ProcessStack(PTLLMsgHdr pHdr, const void* pBody, IMsgConnection* pIMsgConn) = 0;

    CHECK_FOR_DELETE__(ILLMsgStackProc)
};

#endif // !defined(AFX_ILLMSGSTACKPROC_H__6FF23911_DBCE_471E_9643_C929D3119C21__INCLUDED_)
