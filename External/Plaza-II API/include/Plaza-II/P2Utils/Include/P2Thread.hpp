#ifndef _P2THREAD_HPP_
#define _P2THREAD_HPP_

#include "../../../Plaza-II/P2Sys/Include/P2Sys.h"
#include "AutoHandle.hpp"
#include <assert.h>

/*
	Types:
		P2THREAD_HANDLE
		P2THREAD_ID			

	Functions: 
	// создание потока 
	P2ERR P2ThreadCreate(P2THREAD_HANDLE* hThread, unsigned (*start_routine)(void*), void* arg); 

	// ожидание завершения потока в течении msTimeout
	P2ERR P2ThreadWait(P2THREAD_HANDLE hThread, UINT32 msTimeout);

	//  поток должен быть завершен с помощью Wait или Terminate ф-ии.
	P2ERR P2ThreadFree(P2THREAD_HANDLE hThread);

	//  терминировать поток. (в юниксовой ветке сделано через pthread_cancel)
	void  P2ThreadTerminate(P2THREAD_HANDLE hThread);

	// равен ли поток hThread текущему потоку выполнения.
	bool  P2ThreadIsCurrentThreadEqual(P2THREAD_HANDLE hThread);

	// получить ID текущего потока
	P2THREAD_ID P2ThreadGetCurrentId()

	P2THREAD_ID P2ThreadGetId(P2THREAD_HANDLE hThread);
 */

	inline void* P2ThreadMemoryAllocate(size_t sz) { return P2SysParserMemAlloc(sz); } 
	inline void  P2ThreadMemoryFree(void* pMemory) { P2SysParserMemFree(pMemory); }

#if defined(__linux__) || defined(__MACH__)

#include <pthread.h>

namespace p2thread_impl {

	struct p2thread 
	{
		pthread_t  	m_nativeHandle;
		bool		m_bJoined;
	};
		
	typedef unsigned (*StartRoutine)(void*);

	struct StartRoutineWrapper
	{
		StartRoutine m_startRoutine;
		void	 	*m_startArg;
	};

	inline void* ThreadFuncWrapper(void* arg) 
	{
		StartRoutineWrapper *const wrapper 	= static_cast<StartRoutineWrapper*>(arg); 
		StartRoutine thread_func 			= wrapper->m_startRoutine;
		void *thread_func_arg 				= wrapper->m_startArg;

		P2ThreadMemoryFree(arg);

		(*thread_func)(thread_func_arg); 
		return NULL;
	} 

};

	typedef p2thread_impl::p2thread* 		P2THREAD_HANDLE;
	typedef pthread_t						P2THREAD_ID;
	#define P2THREAD_ID_FORMAT				"%lu"
	struct P2THREAD_OPTIONS
	{
	};

	inline P2ERR P2ThreadCreate(P2THREAD_HANDLE* hThread, unsigned (*start_routine)(void*), void* arg, P2THREAD_OPTIONS* opts = NULL)
	{ 
		using p2thread_impl::ThreadFuncWrapper;
		using p2thread_impl::StartRoutineWrapper;
		UNUSED(opts);

		pthread_t native_handle; 

		(*hThread) = static_cast<p2thread_impl::p2thread*>(P2ThreadMemoryAllocate(sizeof(p2thread_impl::p2thread))); 

		if(*hThread == NULL)
			return P2ERR_COMMON_NO_MEMORY;
		
		(*hThread)->m_bJoined = false;

		StartRoutineWrapper *pWrapper = static_cast<StartRoutineWrapper*>(P2ThreadMemoryAllocate(sizeof(StartRoutineWrapper)));

		if(pWrapper == NULL)
		{
			P2ThreadMemoryFree(*hThread);
			*hThread = 0;
			return P2ERR_COMMON_NO_MEMORY; 
		}

		pWrapper->m_startRoutine = start_routine;
		pWrapper->m_startArg 	 = arg; 

		int err = pthread_create(&native_handle, NULL, ThreadFuncWrapper, pWrapper); 

		if(err != 0)
		{
			P2ThreadMemoryFree(*hThread);
			P2ThreadMemoryFree(pWrapper);
			*hThread = NULL;
			errno = err;
			return P2ERR_COMMON_SYSCALL_FAILED;
		} 

		(*hThread)->m_nativeHandle = native_handle;

		return P2ERR_OK;
	}

	#ifdef __MACH__
	#include <CoreServices/CoreServices.h>
	#include <mach/mach.h>
	#include <mach/mach_time.h>
	#include <unistd.h>
	#endif


	inline P2ERR P2ThreadWait(P2THREAD_HANDLE hThread, UINT32 msTimeout)
	{

		int err;

		if(!hThread)
			return P2ERR_COMMON_WRONG_ARGUMENTS;

		if(hThread->m_bJoined)
			return P2ERR_OK;
#ifndef __MACH__
		if(msTimeout != INFINITE)
		{
			struct timespec ts;

			err = clock_gettime(CLOCK_REALTIME, &ts);

			if(err == -1)
				return P2ERR_COMMON_SYSCALL_FAILED; 

			ts.tv_sec  += (msTimeout / 1000) ;
			ts.tv_nsec += (msTimeout % 1000) * 1000000;
			ts.tv_sec  += (ts.tv_nsec / 1000000000);
			ts.tv_nsec %= 1000000000;

			err = pthread_timedjoin_np(hThread->m_nativeHandle, NULL, &ts); 
			if(err == ETIMEDOUT)
				return P2ERR_COMMON_TIMEOUT;
		}
		else
#endif // ifndef __MACH__
		{
			err = pthread_join(hThread->m_nativeHandle, NULL);
		}

		if(err != 0)
		{
			errno = err; 
			return P2ERR_COMMON_SYSCALL_FAILED;
		}

		hThread->m_bJoined = true; 
		return P2ERR_OK; 
	}


	inline P2ERR P2ThreadFree(P2THREAD_HANDLE hThread)
	{
		if(!hThread) 
			return P2ERR_COMMON_WRONG_ARGUMENTS;

		assert(hThread->m_bJoined); 

		P2ThreadMemoryFree(hThread); 
		return P2ERR_OK; 
	}

	inline 	void P2ThreadTerminate(P2THREAD_HANDLE hThread)
	{
		if(!hThread || hThread->m_bJoined) 
			return; 

		if(0 == pthread_cancel(hThread->m_nativeHandle))
		{
			pthread_join(hThread->m_nativeHandle, NULL); 
		}
		
		hThread->m_bJoined = true; 
	}	
	
	inline bool P2ThreadIsCurrentThreadEqual(P2THREAD_HANDLE hThread)
	{
		if(!hThread || hThread->m_bJoined)
			return false;

		return pthread_equal(pthread_self(), hThread->m_nativeHandle) != 0; 
	} 

	inline P2THREAD_ID P2ThreadGetCurrentId()
	{
		return pthread_self(); 
	}


	inline P2THREAD_ID P2ThreadGetId(P2THREAD_HANDLE hThread)
	{
		if(!hThread) 
			return 0;
		return hThread->m_nativeHandle;
	}

#elif defined _WIN32

#include "../../P2Common/Include/P2Windows.h"
#include <process.h>


namespace p2thread_impl {
	struct p2thread
	{
		HANDLE m_nativeHandle;
		DWORD  m_threadID; 
	};
};
	
	typedef p2thread_impl::p2thread* 		P2THREAD_HANDLE;
	typedef DWORD							P2THREAD_ID;
	#define P2THREAD_ID_FORMAT				"%u"

	struct P2THREAD_OPTIONS
	{
		P2THREAD_OPTIONS(): affinity(0), priority(0) {}
		UINT32 affinity;
		int    priority;
	};

	inline P2ERR P2ThreadSetOpts(P2THREAD_HANDLE h, P2THREAD_OPTIONS* opt)
	{
		if (!h) return P2ERR_COMMON_WRONG_ARGUMENTS;
		if (!opt) return P2ERR_OK;
		if (opt->affinity && !SetThreadAffinityMask(h->m_nativeHandle, static_cast<DWORD_PTR>(opt->affinity))) return P2ERR_COMMON_SYSCALL_FAILED;
		if (opt->priority && !SetThreadPriority(h->m_nativeHandle, opt->priority)) return P2ERR_COMMON_SYSCALL_FAILED; 
		return P2ERR_OK;
	}

	inline P2ERR P2ThreadCreate(P2THREAD_HANDLE* hThread, unsigned (__stdcall *start_routine)(void*), void* arg, P2THREAD_OPTIONS* opt = NULL)
	{ 
		(*hThread) = static_cast<p2thread_impl::p2thread*>(P2ThreadMemoryAllocate(sizeof(p2thread_impl::p2thread))); 

		if (*hThread == NULL) return P2ERR_COMMON_NO_MEMORY;

		unsigned thrdId;
		(*hThread)->m_nativeHandle = (HANDLE)_beginthreadex(NULL, 0, start_routine, arg, CREATE_SUSPENDED, &thrdId); 
		(*hThread)->m_threadID = thrdId;

		if ((*hThread)->m_nativeHandle == NULL) return P2ERR_COMMON_SYSCALL_FAILED;

		if (P2FAILED(P2ThreadSetOpts(*hThread, opt)) || ResumeThread((*hThread)->m_nativeHandle) == -1)
		{
			TerminateThread((*hThread)->m_nativeHandle, 1);
			CloseHandle((*hThread)->m_nativeHandle);
			(*hThread)->m_nativeHandle = NULL;
			(*hThread)->m_threadID = 0; 
			return P2ERR_COMMON_SYSCALL_FAILED;
		}
		return P2ERR_OK; 
	} 

	inline P2ERR P2ThreadWait(P2THREAD_HANDLE hThread, UINT32 msTimeout)
	{ 
		if(!hThread)
			return P2ERR_COMMON_WRONG_ARGUMENTS;

		if(hThread->m_nativeHandle == NULL)
		{
			assert(hThread->m_threadID == 0);
			return P2ERR_OK;
		}

		switch( WaitForSingleObject(hThread->m_nativeHandle, msTimeout) )
		{
			case WAIT_OBJECT_0: break; 
			case WAIT_TIMEOUT: 	return P2ERR_COMMON_TIMEOUT;
			case WAIT_FAILED:	return P2ERR_COMMON_SYSCALL_FAILED; 
		}

		CloseHandle(hThread->m_nativeHandle);

		hThread->m_nativeHandle = NULL;
		hThread->m_threadID = 0;

		return P2ERR_OK; 
	}

	inline P2ERR P2ThreadFree(P2THREAD_HANDLE hThread)
	{ 
		if(!hThread) 
			return P2ERR_COMMON_WRONG_ARGUMENTS;

		assert(hThread->m_nativeHandle == NULL && hThread->m_threadID == 0);

		P2ThreadMemoryFree(hThread); 
		return P2ERR_OK;
	}

	inline bool P2ThreadIsCurrentThreadEqual(P2THREAD_HANDLE hThread)
	{
		if(!hThread || hThread->m_nativeHandle == NULL)
			return false;

		return GetCurrentThreadId() == hThread->m_threadID;
	} 

	inline void P2ThreadTerminate(P2THREAD_HANDLE hThread)
	{
		if(!hThread || hThread->m_nativeHandle == NULL)
			return;
		TerminateThread(hThread->m_nativeHandle, P2ERR_COMMON_EXITCODE_TERMINATED);
		hThread->m_nativeHandle = NULL;
		hThread->m_threadID = 0; 
	} 

	inline P2THREAD_ID P2ThreadGetCurrentId()
	{
		return ::GetCurrentThreadId(); 
	}

	inline P2THREAD_ID P2ThreadGetId(P2THREAD_HANDLE hThread)
	{
		if(!hThread)
			return 0;
		return hThread->m_threadID;  
	}

#endif // _WIN32 

	typedef AutoP2Handle<P2THREAD_HANDLE, P2ThreadFree> P2AutoThread; 

#endif //_P2THREAD_HPP_


