//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THREADWATCHDOG_H__66BD8F6E_A9DD_4ac5_AE9B_5BF58E6E6ED4__INCLUDED_)
#define AFX_THREADWATCHDOG_H__66BD8F6E_A9DD_4ac5_AE9B_5BF58E6E6ED4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined(__linux__) || defined(__MACH__)
#include <pthread.h>
#endif // defined(__linux__) || defined(__MACH__)

#include "../../../Plaza-II/P2Utils/Include/ServiceDriver.h"
#include "../../../Plaza-II/P2Utils/Include/P2Thread.hpp"
#include "IModuleServices.h"
#define  LEN_CATEGORY 32

#define TIMEOUT_WARNING 30000
#define TIMEOUT_ERROR   (5 * 60 * 1000)
#define WD_TIMEOUT   350

typedef	 struct  
{
    P2THREAD_ID     thID;	
	volatile LONG	timeStamp;
	volatile LONG	lastTimeStamp;
	UINT32			timeoutWarning;
	UINT32			timeoutError;
	char			category[LEN_CATEGORY];
} WatchDogData;
class CThreadWithStop;

struct IThreadWatchDog 
{
    virtual P2ERR Register(P2THREAD_ID thID, const char* cat, UINT32 timeoutWarning = TIMEOUT_WARNING, UINT32 timeoutError = TIMEOUT_ERROR) = 0;
	virtual void UnRegister() = 0;
	virtual void Start() = 0;
	virtual void Stop() = 0;
	virtual DWORD GetTlsIdx() = 0;
	virtual ~IThreadWatchDog() {}
};

class AutoWatchDogTimer
{
public:
inline	AutoWatchDogTimer(IThreadWatchDog* pTWD):m_pTWD(pTWD)
	{ 
		m_pTWD->Start();
	}
inline	~AutoWatchDogTimer()
	{ 
		m_pTWD->Stop();
	}
private:
	IThreadWatchDog *m_pTWD;
};

#endif // !defined(AFX_THREADWATCHDOG_H__66BD8F6E_A9DD_4ac5_AE9B_5BF58E6E6ED4__INCLUDED_)

