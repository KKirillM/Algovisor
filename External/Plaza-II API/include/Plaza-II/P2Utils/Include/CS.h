// CS.h: interface for the CCS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CS_H__3B84E56A_2C39_4040_97C2_F43ABB3D3E36__INCLUDED_)
#define AFX_CS_H__3B84E56A_2C39_4040_97C2_F43ABB3D3E36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined(__linux__) || defined(__MACH__)
#include <pthread.h>
#include <assert.h>
#else
#include "../../../Plaza-II/P2Common/Include/P2Windows.h"

#if _WIN32_WINNT < 0x0403
#error "Can't use spin count with _WIN32_WINNT < 0x0403"
#endif

#endif //defined(__linux__) || defined(__MACH__)

#include "stuff.h"

////////////////////////////////////////////////////////////////////////////
// CCS

class CCS
{
public:
	CCS(DWORD spinCount = 0);
	~CCS();

	void Lock();
	bool TryLock();
	void Unlock();

	void UseCS(bool useIt){ m_useCS = useIt;}
	bool UsingCS(void)const {return m_useCS;}

#if !(defined(__linux__) || defined(__MACH__))
#ifdef _DEBUG
    void MBF__();
    void MBL__();
#endif // _DEBUG
#endif // !(defined(__linux__) || defined(__MACH__))

private:
	
#if defined(_WIN32)	
	CRITICAL_SECTION	m_cs;
#elif (defined(__linux__) || defined(__MACH__))
	pthread_mutex_t		m_cs;
#endif
	
	bool				m_useCS;

    DISABLE_CLASS_COPY_PV(CCS)
};

//////////////////////////////////////////////////////////////////////
#if defined(_WIN32)

#define DWORD2HANDLE(d) reinterpret_cast<HANDLE>(static_cast<UINT64>(d))

inline CCS::CCS(DWORD spinCount) : m_useCS(true)
{
	if (spinCount == 0)
	{
		::InitializeCriticalSection(&m_cs);
	}
	else
	{
		::InitializeCriticalSectionAndSpinCount(&m_cs, spinCount);
	}
}

inline CCS::~CCS()
{
	if (m_useCS)
	{
		_ASSERT(m_cs.LockCount == -1);
	}
	::DeleteCriticalSection(&m_cs);
}

inline void CCS::Lock()
{
	if (m_useCS)
	{
		::EnterCriticalSection(&m_cs);
	}
}

inline void CCS::Unlock()
{
	if (m_useCS)
	{
		_ASSERT(m_cs.OwningThread == DWORD2HANDLE(GetCurrentThreadId()));

		::LeaveCriticalSection(&m_cs);
	}
}

#ifdef _DEBUG
inline void CCS::MBF__()
{
	if (m_useCS)
	{
		_ASSERT(m_cs.OwningThread != DWORD2HANDLE(GetCurrentThreadId()));
	}
}
inline void CCS::MBL__()
{
	if (m_useCS)
	{
		_ASSERT(m_cs.OwningThread == DWORD2HANDLE(GetCurrentThreadId()));
	}
}
#endif // _DEBUG

inline bool CCS::TryLock()
{
	return ( TRUE == TryEnterCriticalSection(&m_cs) );
}

#undef DWORD2HANDLE 

#elif defined(__linux__) || defined(__MACH__)


inline CCS::CCS(DWORD spinCount) : m_useCS(true)
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr); 
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE); 
	pthread_mutex_init(&m_cs, &attr);
	pthread_mutexattr_destroy(&attr);
}

inline CCS::~CCS()
{
	pthread_mutex_destroy(&m_cs);
}

inline void CCS::Lock()
{
	if (m_useCS)
	{
		pthread_mutex_lock(&m_cs);
	}
}

inline void CCS::Unlock()
{
	if (m_useCS)
	{
		pthread_mutex_unlock(&m_cs);
	}
}

inline bool CCS::TryLock()
{
	return ( 0 == pthread_mutex_trylock(&m_cs) );
}

#endif // defined(__linux__) || defined(__MACH__)

class CInterlockedLocker
{
	volatile LONG  m_Locker;
public:
	inline CInterlockedLocker()
	{
		InterlockedExchange(&m_Locker, 0);
	};
	
	inline ~CInterlockedLocker(){};
	
	inline bool Lock()
	{
		while(!TryLock());
		return true;
	}
	
	inline bool TryLock()
	{
		return 0 == InterlockedCompareExchange(&m_Locker, 1, 0);
	}
	
	inline void FreeLock()
	{
		InterlockedExchange(&m_Locker, 0);
	}
};

template <class TCS>
class AutoCCS_
{
public:
    inline AutoCCS_(TCS& ccs) : m_ccs(ccs)
    {
	    m_ccs.Lock();
    }

    inline AutoCCS_(TCS& ccs, bool isLocked) : m_ccs(ccs)
    {
	    if (!isLocked) m_ccs.Lock();
    #ifdef _DEBUG
        else CSMBL__(&m_ccs);
    #endif // _DEBUG
    }

    inline ~AutoCCS_()
    {
	    m_ccs.Unlock();
    }

private:
	TCS& m_ccs;
    AutoCCS_& operator = (const AutoCCS_&);
};

typedef AutoCCS_<CCS> AutoCCS;

#endif // !defined(AFX_CS_H__3B84E56A_2C39_4040_97C2_F43ABB3D3E36__INCLUDED_)
