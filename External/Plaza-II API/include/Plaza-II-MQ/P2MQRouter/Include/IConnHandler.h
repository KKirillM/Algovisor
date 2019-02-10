// interface IConnHandler
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICONNHANDLER_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_)
#define AFX_ICONNHANDLER_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../Plaza-II/P2Utils/Include/IRc.h"
#include "../../../Plaza-II/P2Utils/Include/ICs.h"

//////////////////////////////////////////////////////////////////////
// IConnHandler

struct IConnHandler : public IRc
{
    virtual void OnOutConnectComplete(ICs* pICsCaller) = 0;
	virtual void OnOutConnectError(int error, ICs* pICsCaller) = 0;
	virtual void OnInConnectComplete(ICs* pICsCaller) = 0;
    virtual void OnClose(int error, bool isDead, ICs* pICsCaller) = 0;
	virtual void OnRecv(/*const*/ void* pData, size_t len, ICs* pICsCaller) = 0;

    CHECK_FOR_DELETE__(IConnHandler)
};

#endif // !defined(AFX_ICONNHANDLER_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_)
