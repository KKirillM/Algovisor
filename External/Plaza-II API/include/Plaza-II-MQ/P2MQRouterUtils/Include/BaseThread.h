// BaseThread.h: interface for the CBaseThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASETHREAD_H__4AA20046_99F6_4058_AD22_2D64E3A5E554__INCLUDED_)
#define AFX_BASETHREAD_H__4AA20046_99F6_4058_AD22_2D64E3A5E554__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _WIN32
#include "../../../Plaza-II/P2Common/Include/P2Windows.h"
#endif // _WIN32
#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II/P2Utils/Include/stuff.h"
#include "../../../Plaza-II-MQ/P2MQRouter/Include/IThreadWatchDog.h"
#include "../../P2MQRouter/Include/IModuleServices.h"

#include "../../../Plaza-II/P2Common/Include/P2Types.h"
#include "../../../Plaza-II/P2Utils/Include/CS.h"

#if defined(__linux__) || defined(__MACH__)
#include <boost/thread/thread.hpp>
#endif // defined(__linux__) || defined(__MACH__)

//////////////////////////////////////////////////////////////////////
// CBaseThread

class CBaseThread  
{
public:
	CBaseThread(const std::string& name): m_name(name) {}
	virtual ~CBaseThread() 
	{
		bool joined = Wait(0); 
		if (!joined)
		{
			P2TRACE_WARNING(("Thread destroyed while not joined;thread " P2THREAD_ID_FORMAT, P2ThreadGetId(m_hThread))); 
		} 
	}

	P2ERR 	Start(unsigned (__stdcall *start_addr)(void *), void *pArg);
	P2ERR 	StartWithWatchDog(unsigned (__stdcall *start_addr)(void *), void* pArg, unsigned initflag, IModuleServices* pIModuleServices, const char *pCategory, UINT32 timeoutWarning = TIMEOUT_WARNING, UINT32 timeoutError = TIMEOUT_ERROR);
	bool 	Wait(DWORD msTimeout);
	bool    StopThread(DWORD timeout);
	void    StopThreadOrDie(DWORD timeout);
	P2THREAD_ID GetThreadId() const; 

protected: 
    DISABLE_CLASS_COPY_PT(CBaseThread);
private: 
	static unsigned __stdcall WatchdogWrapper(void *pArg);
	P2AutoThread m_hThread;
	std::string	 m_name;
};

//////////////////////////////////////////////////////////////////////

inline P2ERR CBaseThread::Start(unsigned (__stdcall *start_addr)(void *), void *pArg)
{
	assert(m_hThread() == NULL);
	return P2ThreadCreate(&m_hThread, start_addr, pArg); 
} 

inline bool CBaseThread::Wait(DWORD msTimeout)
{
	P2ERR res;
	if (!m_hThread())
		return true;

	if (P2SUCCEEDEDV(P2ThreadWait(m_hThread, msTimeout), res))
	{
		m_hThread.Close(); 
	} 
	return P2SUCCEEDED(res); 
}

inline bool CBaseThread::StopThread(DWORD timeout)
{
	return Wait(timeout);
}


inline void CBaseThread::StopThreadOrDie(DWORD timeout)
{
	if (!Wait(timeout))
	{
		P2LogFatalEx("StopThreadOrDie", "Couldn't stop thread " P2THREAD_ID_FORMAT ", '%s'. Process stopped", P2ThreadGetId(m_hThread), m_name.c_str());
		P2LogFlush();
#if defined(_WIN32)
		::ExitProcess(1);
#else 
		abort();
#endif
	} 
}


inline P2THREAD_ID CBaseThread::GetThreadId() const
{
	return P2ThreadGetId(m_hThread);
}

#endif // !defined(AFX_BASETHREAD_H__4AA20046_99F6_4058_AD22_2D64E3A5E554__INCLUDED_)
