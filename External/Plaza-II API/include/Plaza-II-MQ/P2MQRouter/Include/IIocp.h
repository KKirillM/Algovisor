// interface IIocp
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IIOCP_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_)
#define AFX_IIOCP_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II/P2Common/Include/P2Types.h"
#include "../../../Plaza-II/P2Utils/Include/IRc.h"
#include "../../../Plaza-II/P2Utils/Include/ICs.h"
#include "../../../Plaza-II/P2Utils/Include/QueueMan.h"

//////////////////////////////////////////////////////////////////////
// IIocpMsg

struct IIocpTask : public IRc, public CQueueElem
{
    virtual void Process(ICs* pICsCaller) = 0;
	virtual const char *GetDescr(void)const = 0;

    CHECK_FOR_DELETE__(IIocpMsg)
};

//////////////////////////////////////////////////////////////////////
// IIocp

struct IIocp
{
	// Post iocp message
    virtual void AddTaskToWorkingPool(IIocpTask* pIMsg) = 0;
	virtual bool AddTaskToWorkingPoolNoDupe(IIocpTask* pIMsg) = 0;

    CHECK_FOR_DELETE__(IIocp)
};

#endif // !defined(AFX_IIOCP_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_)
