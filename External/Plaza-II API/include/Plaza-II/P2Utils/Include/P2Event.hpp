#ifndef _P2EVENT_HANDLE_HPP_
#define _P2EVENT_HANDLE_HPP_

	// ф-ии для работы с эвентом.

#include "../../../Plaza-II/P2SysTypes/Include/P2SysTypes.hpp" 
#include "AutoHandle.hpp"
#include <memory>

#ifdef _WIN32 
#include "../../P2Common/Include/P2Windows.h"
typedef HANDLE P2EVENT_HANDLE;

#elif defined(__linux__) || defined(__MACH__)

#include <pthread.h> 

#ifdef __MACH__
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <unistd.h>
#endif


namespace p2event_impl
{

#ifdef _DEBUG
#define P2THREAD_VERIFY(expr) _ASSERT((expr) == 0)
#else
#define P2THREAD_VERIFY(expr) expr
#endif

class event_t
{
	pthread_mutex_t m_mutex;
	pthread_cond_t 	m_condition;

	const bool  m_bManualReset;
	int  		m_bSignalled; 
	const bool	m_bShared;

	struct auto_mutex_lock
	{
		pthread_mutex_t* m_pMutex;
		auto_mutex_lock(pthread_mutex_t* pMutex) : m_pMutex(pMutex) { P2THREAD_VERIFY(pthread_mutex_lock(m_pMutex)); }
		~auto_mutex_lock() { P2THREAD_VERIFY(pthread_mutex_unlock(m_pMutex)); }
	};

	bool IsSignalSet() 
	{
		if (m_bSignalled)
		{  
			if (!m_bManualReset)
                m_bSignalled = false; 
			return true; 
		}
		return false; 
	}

	event_t(bool bManualReset, bool bInitial, bool bShared) : m_bManualReset(bManualReset), m_bSignalled(bInitial), m_bShared(bShared) { }

	int mutex_init(bool shared)
	{ 
		int err;
		pthread_mutexattr_t attr;

		if (shared)
		{ 
			pthread_mutexattr_init(&attr);
			pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
		}

		err = pthread_mutex_init(&m_mutex, shared? &attr:NULL);

		if (shared)
			pthread_mutexattr_destroy(&attr);

		return err;
	}

	int cond_init(bool shared)
	{ 
		int err;

		pthread_condattr_t attr;

		if (shared)
		{ 
			pthread_condattr_init(&attr);
			pthread_condattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
		}

		err = pthread_cond_init(&m_condition, shared? &attr : NULL);

		if (shared)
            pthread_condattr_destroy(&attr);

		return err;
	}

	public: 

	static event_t* Create(void* pShared, bool bManualReset, bool bInitial) 
	{
		int err;
		event_t *event;
		std::auto_ptr<event_t> guard;

		if (pShared) 
			event = new (pShared) event_t(bManualReset, bInitial, true);
		else {
			event = new event_t(bManualReset, bInitial, false);
			guard.reset(event);
		}

		if ((err = event->mutex_init(pShared != NULL)) != 0)
		{
			errno = err;
			return NULL;
		}

		if ((err = event->cond_init(pShared != NULL)) != 0) 
		{
			errno = err;
			pthread_mutex_destroy(&(event->m_mutex));
			return NULL;
		}

		if (!pShared)
			guard.release();

		return event;
	}

	static void Destroy(event_t* event) 
	{ 
		// todo: comment
		if (!event->m_bShared)
		{
			P2THREAD_VERIFY(pthread_mutex_destroy(&(event->m_mutex)));
			P2THREAD_VERIFY(pthread_cond_destroy(&(event->m_condition)));
			delete event;
		}
	}

	void Set() 
	{ 	
		P2THREAD_VERIFY(pthread_mutex_lock(&m_mutex));
		m_bSignalled = true; 

		if (!m_bManualReset) 
			pthread_cond_signal(&m_condition);
		else
			pthread_cond_broadcast(&m_condition);
        P2THREAD_VERIFY(pthread_mutex_unlock(&m_mutex));
	}

	void Reset() 
	{ 
		pthread_mutex_lock(&m_mutex);
		m_bSignalled = false; 
		pthread_mutex_unlock(&m_mutex);
	}

	P2ERR Wait(unsigned int msTimeout)
	{ 
		int err;
		auto_mutex_lock lck(&m_mutex);

		if (IsSignalSet())
            return P2ERR_OK;

		if (msTimeout == 0)
            return P2ERR_COMMON_TIMEOUT;

		if (msTimeout == INFINITE)
		{ 
			do
            {
				if ((err = pthread_cond_wait(&m_condition, &m_mutex)) != 0)
				{
					errno = err;
					return P2ERR_COMMON_SYSCALL_FAILED;
				}
			}
            while (!IsSignalSet());
		} 
		else
		{
			struct timespec ts;
            
#ifdef __MACH__
            struct timeval tv;
            gettimeofday(&tv, NULL);
            ts.tv_sec = tv.tv_sec + 0;
            ts.tv_nsec = tv.tv_usec * 1000;
#else
			if (-1 == clock_gettime(CLOCK_REALTIME, &ts))
				return P2ERR_COMMON_SYSCALL_FAILED; 
#endif // ifdef __MACH__
            
			const unsigned seconds = ( msTimeout / 1000 );
			const unsigned nanoseconds = ( msTimeout % 1000 ) * 1000000;

			ts.tv_sec  += seconds;
			ts.tv_nsec += nanoseconds;
			ts.tv_sec  += (ts.tv_nsec / 1000000000);
			ts.tv_nsec %= 1000000000;

			do
            {
				if ((err = pthread_cond_timedwait(&m_condition, &m_mutex, &ts)) != 0)
				{
					if (err == ETIMEDOUT)
                        return P2ERR_COMMON_TIMEOUT;
					errno = err;
					return P2ERR_COMMON_SYSCALL_FAILED;
				}
			}
            while (!IsSignalSet());
		}
		return P2ERR_OK;
	} 
};

}; //p2event_impl


typedef p2event_impl::event_t* P2EVENT_HANDLE;

#endif //defined(__linux__) || defined(__MACH__)

inline P2EVENT_HANDLE P2EventCreate(bool manualReset, bool initialState)
{
#ifdef _WIN32
	return ::CreateEvent(NULL, manualReset, initialState, NULL); 
#elif defined(__linux__) || defined(__MACH__)
	return p2event_impl::event_t::Create(NULL, manualReset, initialState);
#endif //defined(__linux__) || defined(__MACH__)
}

inline P2ERR P2EventCreate(P2EVENT_HANDLE* hEvent, bool manualReset, bool initialState)
{ 
    if (!hEvent)
        return P2ERR_COMMON_WRONG_ARGUMENTS;

#ifdef _WIN32
	*hEvent = ::CreateEvent(NULL, manualReset, initialState, NULL); 
#elif defined(__linux__) || defined(__MACH__)
	*hEvent = p2event_impl::event_t::Create(NULL, manualReset, initialState);
#endif //defined(__linux__) || defined(__MACH__)

    if (!*hEvent)
        return P2ERR_COMMON_SYSCALL_FAILED;
	return P2ERR_OK;
}

inline P2ERR P2EventSet(P2EVENT_HANDLE hEvent)
{ 
#ifdef _WIN32 
    if(!SetEvent(hEvent))
        return P2ERR_COMMON_SYSCALL_FAILED;
#elif defined(__linux__) || defined(__MACH__)
    if(!hEvent)
        return P2ERR_COMMON_WRONG_ARGUMENTS;
	hEvent->Set(); 
#endif //defined(__linux__) || defined(__MACH__)
	return P2ERR_OK; 
} 

inline P2ERR P2EventReset(P2EVENT_HANDLE hEvent)
{ 
#ifdef _WIN32
    if (!ResetEvent(hEvent))
        return P2ERR_COMMON_SYSCALL_FAILED;
#elif defined(__linux__) || defined(__MACH__)
    if (!hEvent)
        return P2ERR_COMMON_WRONG_ARGUMENTS;
	hEvent->Reset();
#endif //defined(__linux__) || defined(__MACH__)
	return P2ERR_OK;
}

inline P2ERR P2EventWait(P2EVENT_HANDLE hEvent, DWORD msTimeout)
{ 
#ifdef _WIN32
	switch(WaitForSingleObject(hEvent, msTimeout))
	{
		case WAIT_OBJECT_0: return P2ERR_OK;
		case WAIT_TIMEOUT:  return P2ERR_COMMON_TIMEOUT;
		default:			return P2ERR_COMMON_SYSCALL_FAILED; 
	}
#elif defined(__linux__) || defined(__MACH__)
	return (hEvent != NULL) ? (hEvent->Wait(msTimeout)) : P2ERR_COMMON_WRONG_ARGUMENTS;
#endif //defined(__linux__) || defined(__MACH__)
}

inline P2ERR P2EventClose(P2EVENT_HANDLE hEvent)
{
#ifdef _WIN32 
    return (::CloseHandle(hEvent) != 0) ? (P2ERR)P2ERR_OK : P2ERR_COMMON_SYSCALL_FAILED;
#elif defined(__linux__) || defined(__MACH__)
    if (!hEvent)
        return P2ERR_COMMON_WRONG_ARGUMENTS;
    p2event_impl::event_t::Destroy(hEvent);
    return P2ERR_OK;
#endif //defined(__linux__) || defined(__MACH__)
}

#ifdef _WIN32
#define P2EVENT_SHARED_SIZE  sizeof(HANDLE)
#elif defined(__linux__) || defined(__MACH__)
#define P2EVENT_SHARED_SIZE  sizeof(p2event_impl::event_t)
#endif

// ñîçäàòü íàñëåäóåìûé ýâåíò è ïîìåñòèòü åãî â ðàçäåëÿåìóþ ïàìÿòü; 
// Âûçûâàþùèé äîëæåí ãàðàíòèðîâàòü, ÷òî pSharedMem  èìååò âûðàâíèâàíèå 8 è  äîñòóïíîå êîëè÷åñòâî ïàìÿòè >=, ÷åì P2EVENT_SHARED_SIZE
inline P2ERR P2EventCreate(P2EVENT_HANDLE* pEvent, bool bManualReset, bool bInitialState, void *pSharedMem)
{
	if(!pSharedMem)
		return P2ERR_COMMON_WRONG_ARGUMENTS; 

#ifdef _WIN32
	SECURITY_ATTRIBUTES	sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE; 

	if (0 == (*pEvent = CreateEvent(&sa, bManualReset, bInitialState, NULL))) 
		return P2ERR_COMMON_SYSCALL_FAILED;

	memcpy(pSharedMem, pEvent, sizeof(HANDLE));

#elif defined(__linux__) || defined(__MACH__)
	using p2event_impl::event_t;
	if (NULL == (*pEvent = event_t::Create(pSharedMem, bManualReset, bInitialState)))
		return P2ERR_COMMON_SYSCALL_FAILED;
#endif
	return P2ERR_OK; 
}

// çíà÷åíèå pSharedMemory äîëæíî áûòü òåì æå ñàìûì, ÷òî è â  P2EventCreateShared.
inline P2ERR P2EventGet(P2EVENT_HANDLE* pEvent, void* pSharedMem)
{ 
	if(!pEvent)
		return P2ERR_COMMON_WRONG_ARGUMENTS; 

#ifdef _WIN32
	memcpy(pEvent, pSharedMem, sizeof(HANDLE));
#elif defined(__linux__) || defined(__MACH__)
	*pEvent = static_cast<p2event_impl::event_t*>(pSharedMem); 
#endif 
	return P2ERR_OK; 
}

#ifdef _WIN32
inline HANDLE P2EventGetNativeHandle(P2EVENT_HANDLE handle)
{
	return handle;
}
#endif


typedef AutoP2Handle<P2EVENT_HANDLE, P2EventClose> P2AutoEvent; 


#endif // _P2EVENT_HANDLE_HPP_
