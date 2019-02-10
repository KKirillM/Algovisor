#ifndef _CRWLOCK_HPP_
#define _CRWLOCK_HPP_ 


#if defined(_WIN32)

#include "../../P2Common/Include/P2Windows.h" 

// Определение для _RTL_SRWLOCK cкопировано из winbase.h из Win SDK для Win7, потому что в SDK 2005 студии они недоступны. 
#if _MSC_VER <= 1400 // 1400 = Visual Studio 2005
typedef struct _RTL_SRWLOCK {
    PVOID Ptr;
} SRWLOCK, *PSRWLOCK;
#endif // _MSC_VER <= 1400

class CRWLock 
{ 
	union 
	{
		SRWLOCK 			m_rwLck;
		CRITICAL_SECTION 	m_csLck;
	} m_lck;

	typedef VOID (WINAPI *FPInitializeSRWLock)(PSRWLOCK); 
	typedef VOID (WINAPI *FPReleaseSRWLockExclusive)(PSRWLOCK); 
	typedef VOID (WINAPI *FPReleaseSRWLockShared)(PSRWLOCK);
	typedef VOID (WINAPI *FPAcquireSRWLockExclusive)(PSRWLOCK);
	typedef VOID (WINAPI *FPAcquireSRWLockShared)(PSRWLOCK);

	FPInitializeSRWLock			m_fpInitialize;
	FPAcquireSRWLockExclusive	m_fpLockExclusive;
	FPAcquireSRWLockShared		m_fpLockShared;
	FPReleaseSRWLockExclusive	m_fpUnlockExclusive;
	FPReleaseSRWLockShared		m_fpUnlockShared;

	bool m_useRWLock;

	public:

	CRWLock() 
	{ 
		HMODULE hModule = GetModuleHandle(TEXT("kernel32.dll"));
		if(hModule) 
		{ 
			m_fpInitialize      = reinterpret_cast<FPInitializeSRWLock>	     (GetProcAddress(hModule, "InitializeSRWLock"));
			m_fpLockExclusive   = reinterpret_cast<FPAcquireSRWLockExclusive>(GetProcAddress(hModule, "AcquireSRWLockExclusive"));
			m_fpUnlockExclusive = reinterpret_cast<FPReleaseSRWLockExclusive>(GetProcAddress(hModule, "ReleaseSRWLockExclusive"));
			m_fpLockShared	    = reinterpret_cast<FPAcquireSRWLockShared>   (GetProcAddress(hModule, "AcquireSRWLockShared")); 
			m_fpUnlockShared    = reinterpret_cast<FPReleaseSRWLockShared>   (GetProcAddress(hModule, "ReleaseSRWLockShared"));

			m_useRWLock = (m_fpInitialize != NULL) 	  && 
						  (m_fpLockExclusive != NULL) && (m_fpUnlockExclusive != NULL) && 
						  (m_fpLockShared !=NULL) 	  && (m_fpUnlockShared != NULL);
		} 
		else
		{
			m_useRWLock = false;
            // coverity волновался, что эти переменные могут остаться непроинициализированными.
            m_fpInitialize      = NULL;
			m_fpLockExclusive   = NULL;
			m_fpUnlockExclusive = NULL;
			m_fpLockShared	    = NULL; 
			m_fpUnlockShared    = NULL;
		} 

		if(m_useRWLock)
			(*m_fpInitialize)(&m_lck.m_rwLck); 
		else
			InitializeCriticalSection(&m_lck.m_csLck); 
	}

	~CRWLock()
	{
		if(!m_useRWLock)
		{ 
			DeleteCriticalSection(&m_lck.m_csLck);
		} 
	}

	bool IsRWLockUsed() const { return m_useRWLock; }

	void LockExclusive()
	{
		if(m_useRWLock)
			(*m_fpLockExclusive)(&m_lck.m_rwLck); 
		else
			EnterCriticalSection(&m_lck.m_csLck);
	}

	void UnlockExclusive()
	{
		if(m_useRWLock)
			(*m_fpUnlockExclusive)(&m_lck.m_rwLck); 
		else
			LeaveCriticalSection(&m_lck.m_csLck); 
	} 

	void LockShared()
	{
		if(m_useRWLock)
			(*m_fpLockShared)(&m_lck.m_rwLck); 
		else
			EnterCriticalSection(&m_lck.m_csLck);
	}

	void UnlockShared()
	{
		if(m_useRWLock)
			(*m_fpUnlockShared)(&m_lck.m_rwLck); 
		else
			LeaveCriticalSection(&m_lck.m_csLck);
	} 

	private: 
	CRWLock(const CRWLock&);
	CRWLock& operator=(const CRWLock&); 
};


#elif defined(__linux__) || defined (__MACH__)
// TODO:  здесь нужно не забыть использовать нативный Read/Write lock.
#include <pthread.h>

class CRWLock
{
	pthread_rwlock_t      m_lock;
    pthread_rwlockattr_t  m_lockAttr;

	public:
    CRWLock()
    {
        pthread_rwlockattr_init(&m_lockAttr);
        
#if !defined(__MACH__)
        
        pthread_rwlockattr_setkind_np(&m_lockAttr,
            PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);
        
#endif
        
        pthread_rwlock_init(&m_lock, &m_lockAttr);
    }

	~CRWLock()
	{
        pthread_rwlock_destroy(&m_lock);
        pthread_rwlockattr_destroy(&m_lockAttr); 
	}

	void LockExclusive()	 { pthread_rwlock_wrlock(&m_lock); }
	void UnlockExclusive()	 { pthread_rwlock_unlock(&m_lock); }
	void LockShared()		 { pthread_rwlock_rdlock(&m_lock); }
	void UnlockShared()		 { pthread_rwlock_unlock(&m_lock); } 

private: 
	CRWLock(const CRWLock&);
	CRWLock& operator=(const CRWLock&); 
}; 

#endif //defined(__linux__) || defined (__MACH__)


class AutoSharedLock 
{
	CRWLock& m_lck; 
	AutoSharedLock(const AutoSharedLock&);
	AutoSharedLock& operator=(const AutoSharedLock&); 

	public:
	AutoSharedLock(CRWLock& lck): m_lck(lck) 
	{
		m_lck.LockShared();
	}

	~AutoSharedLock()
	{
		m_lck.UnlockShared();
	} 
};


class AutoExclusiveLock 
{
	CRWLock& m_lck; 
	AutoExclusiveLock(const AutoExclusiveLock&);
	AutoExclusiveLock& operator=(const AutoExclusiveLock&); 
	public:
	AutoExclusiveLock(CRWLock& lck): m_lck(lck) 
	{
		m_lck.LockExclusive();
	}

	~AutoExclusiveLock()
	{
		m_lck.UnlockExclusive();
	} 
};






#endif // _CRWLOCK_HPP_
