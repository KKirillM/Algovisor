// interface IScheduler
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISCHEDULER_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_)
#define AFX_ISCHEDULER_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II/P2Common/Include/P2Types.h"
#include "../../../Plaza-II/P2Utils/Include/IRc.h"
#include "IIocp.h"

//////////////////////////////////////////////////////////////////////
// IScheduler

//typedef IIocpMsg ISchTask;

struct ISchJob : public IRc
{
	virtual IIocpTask* GetTask(void) = 0;
};

struct IScheduler
{
    // Константа для конвертирования timeout-a из UINT32 (milliseconds)
    // в LONGLONG (100-nanosecond intervals): LONGLONG ns = ms * TIMEOUT_MS
    enum { TIMEOUT_MS = 10000 };

//    virtual P2ERR Start(ISchTask* pITask,
//                        const LONGLONG& startTime, LONG period = 0,
//                        ISchJob** ppIJob = 0) = 0;

	// Rules for time settings in Start() like in ::SetWaitableTimer()
	/*
	 *	startTime [in] Time after which the state of the timer is to be set to 
	 *	signaled, in 100 nanosecond intervals. Use the format described by 
	 *  the FILETIME structure. Positive values indicate absolute time. 
	 *  Be sure to use a UTC-based absolute time, as the system uses UTC-based 
	 *  time internally. Negative values indicate relative time. The actual 
	 *  timer accuracy depends on the capability of your hardware. 
	 *
	 *  period [in] Period of the timer, in milliseconds. If lPeriod is zero, 
	 *  the timer is signaled once. If lPeriod is greater than zero, 
	 *  the timer is periodic. A periodic timer automatically reactivates 
	 *  each time the period elapses, until the timer is canceled using 
	 *  the CancelWaitableTimer function or reset using SetWaitableTimer. 
	 *  If lPeriod is less than zero, the function fails. 
	 */
	/*
	 * Нюанс. Из-за того, что теперь очередь iocp реализуют сами
	 * объекты IIocpMsg, стало невозможно вставить на исполнение
	 * два инстанса одного задания. Поэтому появилось новое ограничение
	 * согласно которому второй таск просто скипается. Таймер продолжает тикать,
	 * но два таска схлопываются.
     */
    virtual P2ERR Start(IIocpTask* pITask,
                        const LONGLONG& startTime, LONG period = 0,
                        ISchJob** ppIJob = 0) = 0;
    virtual void Cancel(const ISchJob* pIJob) = 0;

    CHECK_FOR_DELETE__(IScheduler)
};

//////////////////////////////////////////////////////////////////////

inline LONGLONG SchTimeFromMilliseconds(int ms)
{
    return static_cast<LONGLONG>(ms) * IScheduler::TIMEOUT_MS;
}

#endif // !defined(AFX_ISCHEDULER_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_)
