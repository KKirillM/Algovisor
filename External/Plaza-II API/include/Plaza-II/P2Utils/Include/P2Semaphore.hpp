#ifndef _P2SEMAPHORE_HANDLE_HPP_
#define _P2SEMAPHORE_HANDLE_HPP_

// Функции для работы с семафором.

#include "../../../Plaza-II/P2SysTypes/Include/P2SysTypes.hpp" 
#include "AutoHandle.hpp"
#include <memory>

#ifdef _WIN32 
#include "../../P2Common/Include/P2Windows.h"
typedef HANDLE P2SEMAPHORE_HANDLE;

#elif defined(__linux__)
#include <pthread.h>
#include <semaphore.h>
typedef sem_t* P2SEMAPHORE_HANDLE;

#ifdef _DEBUG
#define P2THREAD_VERIFY(expr) _ASSERT((expr) == 0)
#else
#define P2THREAD_VERIFY(expr) expr
#endif

#elif defined(__MACH__)
#include <dispatch/dispatch.h>
#include <unistd.h>

typedef dispatch_semaphore_t P2SEMAPHORE_HANDLE;

#endif // defined(__MACH__)


inline P2SEMAPHORE_HANDLE P2SemaphoreCreate(unsigned int initialCount, unsigned int maxCount)
{
#ifdef _WIN32
	return ::CreateSemaphore(NULL, initialCount, maxCount, NULL); 
#elif defined(__linux__)
    UNUSED(maxCount);
    try
    {
        sem_t *sem = new sem_t;
        if (-1 == sem_init(sem, 0, initialCount)) {
            delete sem;
            sem = NULL;
        }
        return sem;
    }
    catch(std::bad_alloc&)
    {
        return NULL;
    }
#elif defined(__MACH__)
    UNUSED(maxCount);
    return dispatch_semaphore_create(initialCount);
#endif // defined(__MACH__)
}

inline P2ERR P2SemaphoreRelease(P2SEMAPHORE_HANDLE hSem, unsigned int releaseCount)
{ 
    if (!hSem)
        return P2ERR_COMMON_WRONG_ARGUMENTS;

#ifdef _WIN32
    return (TRUE == ::ReleaseSemaphore(hSem, releaseCount, NULL) ? P2ERR_OK : P2ERR_COMMON_OVERFLOW);
#elif defined(__linux__)
    for (unsigned int i = 0; i < releaseCount; ++i)
    {
        if (0 != sem_post(hSem)) {
            if (EINVAL == errno)
                return P2ERR_COMMON_WRONG_ARGUMENTS;
            else if (EOVERFLOW == errno)
                return P2ERR_COMMON_OVERFLOW;
        }
    }   
	return P2ERR_OK;
#elif defined(__MACH__)
    for (unsigned int i = 0; i < releaseCount; ++i)
    {
        dispatch_semaphore_signal(hSem);
    }
    return P2ERR_OK;
#endif // defined(__MACH__)
}

inline P2ERR P2SemaphoreWait(P2SEMAPHORE_HANDLE hSem, DWORD msTimeout)
{
    if (!hSem)
        return P2ERR_COMMON_WRONG_ARGUMENTS;
#ifdef _WIN32
    switch (WaitForSingleObject(hSem, msTimeout))
	{
		case WAIT_OBJECT_0: return P2ERR_OK;
		case WAIT_TIMEOUT:  return P2ERR_COMMON_TIMEOUT;
		default:			return P2ERR_COMMON_SYSCALL_FAILED;
	}
#elif defined(__linux__)
    if (INFINITE == msTimeout) {
        return (0 == sem_wait(hSem) ? P2ERR_OK : P2ERR_COMMON_SYSCALL_FAILED);
    }
    else {
        struct timespec ts;
        if (-1 == clock_gettime(CLOCK_REALTIME, &ts))
            return P2ERR_COMMON_SYSCALL_FAILED; 
            
        const unsigned seconds = ( msTimeout / 1000 );
        const unsigned nanoseconds = ( msTimeout % 1000 ) * 1000000;

        ts.tv_sec  += seconds;
        ts.tv_nsec += nanoseconds;
        ts.tv_sec  += (ts.tv_nsec / 1000000000);
        ts.tv_nsec %= 1000000000;
        
        if (0 == sem_timedwait(hSem, &ts)) {
            return P2ERR_OK;
        }
        else {
            return ETIMEDOUT == errno ? (P2ERR)P2ERR_COMMON_TIMEOUT : P2ERR_COMMON_SYSCALL_FAILED;
        }
    }
#elif defined(__MACH__)
    dispatch_time_t whenTime = (INFINITE == msTimeout ?
        DISPATCH_TIME_FOREVER :
        dispatch_time(DISPATCH_TIME_NOW, msTimeout * NSEC_PER_MSEC));
    return (0 == dispatch_semaphore_wait(hSem, whenTime) ? (P2ERR)P2ERR_OK : P2ERR_COMMON_TIMEOUT);
#endif // defined(__MACH__)
}

inline P2ERR P2SemaphoreClose(P2SEMAPHORE_HANDLE hSem)
{
    if (!hSem)
        return P2ERR_COMMON_WRONG_ARGUMENTS;
#ifdef _WIN32 
    return (::CloseHandle(hSem) != FALSE) ? (P2ERR)P2ERR_OK : P2ERR_COMMON_SYSCALL_FAILED;
#elif defined(__linux__)
    sem_destroy(hSem);
    delete hSem;
    return P2ERR_OK;
#elif defined(__MACH__)
    dispatch_release(hSem);
    return P2ERR_OK;
#endif // defined(__MACH__)
}

#ifdef _WIN32
inline HANDLE P2SemaphoreGetNativeHandle(P2SEMAPHORE_HANDLE handle)
{
	return handle;
}
#endif // _WIN32

typedef AutoP2Handle< P2SEMAPHORE_HANDLE, P2SemaphoreClose > P2AutoSemaphore;


#endif // _P2SEMAPHORE_HANDLE_HPP_
