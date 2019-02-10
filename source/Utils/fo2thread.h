#ifndef f2thread_h__
#define f2thread_h__

#include <windows.h>
#include <process.h>
#include <malloc.h>
#include <vector>
#include <cstdio>
#include <bitset>
//#include <boost/shared_array.hpp>
//#include "pcharhash.h"
//#include "p2log.h"

inline void pause()
{
#ifdef _MSC_VER
    _mm_pause();
#else
    __asm volatile ("pause" ::: "memory");
#endif
}


const DWORD MS_VC_EXCEPTION=0x406D1388;


struct Fo2Thread
{
private:
    static unsigned __stdcall ThreadFunc( void* pArguments )
    {
        Fo2Thread* thread(static_cast<Fo2Thread*>(pArguments));
        
        if (thread->threadName_[0])
        {
            thread->SetThreadName(thread->getThreadID(), thread->threadName_);
        }
        
        if (thread->affinityMask_!=0)
        {
            DWORD_PTR err = SetThreadAffinityMask(GetCurrentThread(), thread->affinityMask_);
			//std::bitset<32> affinityLow32Bits(static_cast<unsigned long>(thread->affinityMask_));
            if (thread->threadName_[0] && err!=0)
            {
                //FO2TRACE_INFO(("Processor number for %s is %d [AffinityMask: %s]", thread->threadName_, GetCurrentProcessorNumber() + 1, affinityLow32Bits.to_string().c_str()))
            }
			else if(thread->threadName_[0])
			{
				//FO2TRACE_ERROR(("Can't set affinity for '%s' [AffinityMask: %s]", thread->threadName_, affinityLow32Bits.to_string().c_str()))
			}
        }

        thread->run();
        //_endthreadex( 0 );
        return 0;
    } 

public:
    static void yield() {Sleep(1);}
    static void pause(size_t count=64)
    {
        for (size_t i=0 ;i<count;++i)
        {
            ::pause();
        }
    }
    
    Fo2Thread()
        : hThread_(0), threadID_(0), affinityMask_(0)
    {}

    virtual ~Fo2Thread()
    {
        join();
        closeHandle();
    }
    
    unsigned int getThreadID(){return threadID_;}
    virtual void start(const char* threadName = "", int priority=0, DWORD_PTR affinityMask = 0)
    {
        threadName_[0] = 0;
        if (threadName!=0 && threadName[0]!=0)
            strncpy_s(threadName_, sizeof(threadName_), threadName, _TRUNCATE);
        affinityMask_ = affinityMask;

        hThread_ = (HANDLE)_beginthreadex( NULL, 0, &ThreadFunc, (void*)this, 0, &threadID_ );
        SetThreadPriority(hThread_, priority);
    }

    bool startThread(const char* threadName, int priority=0, DWORD_PTR affinityMask=0)
    {
        start(threadName, priority, affinityMask);
        if( hThread_ == 0 )
            return false;

        return true;
    }

    bool join()
    {
        if( hThread_ == NULL )
            return true;

        DWORD dwWaitResult = WaitForSingleObject(hThread_, INFINITE);
        if (dwWaitResult != WAIT_OBJECT_0)
        {
            //FO2TRACE_ERROR(("WaitForSingleObject returns error in Fo2Thread::join, result: %d, GetLastError: %d", dwWaitResult, GetLastError()));
            return false;
        }
        return true;
    }

    void Terminate(DWORD exitCode)
    {
        _ASSERTE(("Thread terminated!", false));
        
        if ( hThread_ ){
            ::TerminateThread(hThread_, exitCode);
            closeHandle();
        }
    }

    void WaitTerminate(DWORD waitTimeout, DWORD exitCode)
    {
        if( hThread_ == NULL )
            return;

        if( ::WaitForSingleObject(hThread_, waitTimeout) != WAIT_OBJECT_0 )
        {
            Terminate(exitCode);
        }
        else
        {
            closeHandle();
        }
    }

    typedef const Fo2Thread* CPFo2Thread;
    static void join(const CPFo2Thread* threads, int count)
    {
        HANDLE* handles = new HANDLE[sizeof(HANDLE*)*count];
        //boost::shared_array<HANDLE> handles(new HANDLE[sizeof(HANDLE*)*count]);
        for (int i = 0; i < count ; i++)
        {
            handles[i] = threads[i]->hThread_;
            //handles.get()[i]=threads[i]->hThread_;       	
        }
        WaitForMultipleObjects(count,handles/*.get()*/,true,INFINITE);
    }
  
    bool isRunning()
    {
        return WaitForSingleObject(hThread_,0)==WAIT_TIMEOUT;
    }
protected:
    virtual void run()=0;

public:
    HANDLE       hThread_;
    unsigned int threadID_;
    char         threadName_[256];
    DWORD_PTR    affinityMask_;

private:

#pragma pack(push,8)
	typedef struct tagTHREADNAME_INFO
	{
		DWORD dwType; // Must be 0x1000.
		LPCSTR szName; // Pointer to name (in user addr space).
		DWORD dwThreadID; // Thread ID (-1=caller thread).
		DWORD dwFlags; // Reserved for future use, must be zero.
	} THREADNAME_INFO;
#pragma pack(pop)

	void SetThreadName(DWORD dwThreadID, char* threadName)
	{
		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = threadName;
		info.dwThreadID = dwThreadID;
		info.dwFlags = 0;
        
        __try
        {
			RaiseException( MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info );
		}
        __except (EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}

    void closeHandle()
    {
        if ( hThread_ ){
            ::CloseHandle(hThread_);
        }

        hThread_ = 0;
    }
};



#endif // f2thread_h__
