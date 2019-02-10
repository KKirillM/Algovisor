//----------------------------------------------------------------------------
// Copyright: (c) 2006 RTS, Russia
//----------------------------------------------------------------------------
// Created:	    13 Mar 2006   11:46
// File:        CriticalSectionLock
// Author:		
//----------------------------------------------------------------------------
// Description: declaration of the multithread lock classes
//----------------------------------------------------------------------------

#pragma once

#include "global.h"

#if (__cplusplus >= 201103L)||(_MSC_VER >= 1700)
#include <atomic>
#include <mutex>
#endif

//#define DEF_CRIT_SEC_STANDART
#define DEF_CRIT_SEC_TICKET
//#define DEF_CRIT_SEC_SPIN
//#define CRIT_SEC_EVENT

#ifdef __GNUC__
#define _mm_pause() __asm volatile ("pause" ::: "memory")
#define _ReadWriteBarrier() __asm volatile("" ::: "memory")
#endif

namespace fo2base
{

    //---------------------------------------------------------------------------
    //  class CCriticalSectionLock
    // обеспечивает управление захватом критической секции
    //---------------------------------------------------------------------------
#if ((__cplusplus >= 201103L) || (_MSC_VER >= 1700)) && defined(USE_THE_CODE_THAT_WAS_NEVER_TESTED_UNDER_CXX11)
template<int N>
class GenericSharedMemoryCriticalSectionLock
{
public:
    typedef std::mutex Handle;
    GenericSharedMemoryCriticalSectionLock() {cs_.lock();}
    virtual ~GenericSharedMemoryCriticalSectionLock(){cs_.unlock();}
private:
    static Handle cs_;
    static long started_;
    static bool done_yet_;
};

typedef GenericSharedMemoryCriticalSectionLock<0> CSharedMemoryCriticalSectionLock;
typedef GenericSharedMemoryCriticalSectionLock<1> CSharedMemoryCriticalSectionLock1;

class CCritSection
{
    BAN_COPY_CONSTRUCTOR(CCritSection)
public:
    CCritSection(){}
    void Enter()
    {
        for (int i=0;i<spin_counter;++i) {if (m.try_lock()) return; }
        m.lock();
    }

    void Leave()
    {
        m.unlock();
    }

private:
    static const int spin_counter = 4096;
    std::mutex m;
};

class SpinedMutex: private CCritSection // BasicLockable concept
{
    BAN_COPY_CONSTRUCTOR(SpinedMutex)
public:
    void lock() { Enter(); }
    void unlock() { Leave(); }
};

class CSpinlock
{
    BAN_COPY_CONSTRUCTOR(CSpinlock)
public:
    CSpinlock() : lock(false){}
    void Enter()
    {
        bool isUnlocked = false;
        while (!lock.compare_exchange_weak(isUnlocked,true))
        {
            while (lock.load())
            {
                _mm_pause();
                _ReadWriteBarrier();
            }
        }
    }
    void Leave()
    {
        _ReadWriteBarrier();
        lock = 0;
    }
private:
    std::atomic<bool> lock;
};

//====================================================================================================================================
class CTicketLock
{
    BAN_COPY_CONSTRUCTOR(CTicketLock)
public:
    CTicketLock() : ticket(0), users(0){}
    void Enter()
    {
         unsigned int me = (++users)-1;
         while (ticket != me)
         {
             _mm_pause();
             _mm_pause();
             _ReadWriteBarrier();
         }
    }
    void Leave()
    {
        _ReadWriteBarrier();
        ++ticket;
    }
private:
    std::atomic<unsigned> ticket;
    std::atomic<unsigned> users;
};

class CCritA
{
	BAN_COPY_CONSTRUCTOR(CCritA)
public:
    CCritA(CCritSection& Section) : mSection(Section)
    {
        mSection.Enter();
    }

    ~CCritA()
    {
        mSection.Leave();
    }

private:
    CCritSection& mSection;
};
#else
    class CSharedMemoryCriticalSectionLock
    {
    public:
        typedef CRITICAL_SECTION Handle;
        CSharedMemoryCriticalSectionLock()
        {              
            if (InterlockedExchange(&started_,1)==0)
            {
                InitializeCriticalSection(&cs_);
                done_yet_ = true;
            }
            else
            {
                while (!done_yet_) _mm_pause();
            }   
            EnterCriticalSection( &cs_ );
        }

        virtual ~CSharedMemoryCriticalSectionLock() 
        {
            LeaveCriticalSection( &cs_ );
        }

    private:
        //критическа€ секци€, которой мы управл€ем
        static CRITICAL_SECTION cs_;
        static long started_;
        static bool done_yet_;
    };

    //---------------------------------------------------------------------------
    class CSharedMemoryCriticalSectionLock1
    {
    public:
        typedef CRITICAL_SECTION Handle;
        CSharedMemoryCriticalSectionLock1()
        {              
            if (InterlockedExchange(&started_,1)==0)
            {
                InitializeCriticalSection(&cs_);
                done_yet_ = true;
            }
            else
            {
                while (!done_yet_) _mm_pause();
            }   
            EnterCriticalSection( &cs_ );
        }

        virtual ~CSharedMemoryCriticalSectionLock1() 
        {
            LeaveCriticalSection( &cs_ );
        }

    private:
        //критическа€ секци€, которой мы управл€ем
        static CRITICAL_SECTION cs_;
        static long started_;
        static bool done_yet_;
    };

    class CCritSection
    { 
        friend class CCritA;
    public:
        CCritSection()
        {
            InitializeCriticalSectionAndSpinCount(&mCS,4096);
        }

        ~CCritSection()
        {
            DeleteCriticalSection(&mCS);
        }

    private: 
        CRITICAL_SECTION mCS;

    public:
        void Enter()
        {
            EnterCriticalSection(&mCS);
        }

        void Leave()
        {
            LeaveCriticalSection(&mCS);
        }
    };

//====================================================================================================================================
    class CSpinlock
    { 
    private: 
        long lock;
        long pad[15]; 
    public:
        CSpinlock() : lock(0){}
        void Enter()
        {
            while (_InterlockedCompareExchange((long*)&lock,1,0)==1)
            {
                while (lock!=0)
                {
                    _mm_pause();
                    _ReadWriteBarrier();
                }
            }
        }
        void Leave()
        {
            _ReadWriteBarrier();
            lock = 0;
        }
    };

//====================================================================================================================================
    class CTicketLock
    { 
    private: 
        unsigned int ticket;
        unsigned int users;
        long pad[14]; 
    public:
        CTicketLock() : ticket(0), users(0){}
        void Enter()
        {
             unsigned int me = _InterlockedIncrement((long*)&users)-1;
             while (ticket != me)
             {
                 _mm_pause();
                 _mm_pause();
                 _ReadWriteBarrier();
             }
        }
        void Leave()
        {
            _ReadWriteBarrier();
            ++ticket;
        }
    };

//====================================================================================================================================
    class CCritA
    { 
    public:
        CCritA(CCritSection& Section) : mSection(Section)
        {
            mSection.Enter();
        }

        ~CCritA()
        {
            mSection.Leave();
        }

        const CCritA& operator= (const CCritA& other)
        {
            mSection = other.mSection;
            return *this;
        }

    private:
        CCritSection& mSection;
    };
#endif

//====================================================================================================================================
#if defined(DEF_CRIT_SEC_STANDART)
    typedef CCritSection CCritSectionEx;
#elif defined(DEF_CRIT_SEC_TICKET)
    typedef CTicketLock CCritSectionEx;
#elif defined(DEF_CRIT_SEC_SPIN)
    typedef CSpinlock CCritSectionEx;
#endif
//====================================================================================================================================

    class CCritA2
    { 
    public:
        CCritA2(CCritSectionEx& Section) : mSection(Section)
        {
            mSection.Enter();
        }

        ~CCritA2()
        {
            mSection.Leave();
        }

    private:
        CCritSectionEx& mSection;

        CCritA2& operator=(const CCritA2&);
    };

    //====================================================================================================================================

#if defined(_WIN32)
    class CEventW
    {
    public:
        CEventW()
        {
            mHandle = CreateEvent(0, FALSE, FALSE, 0);
        }

        HANDLE GetEvt() { return mHandle;};

        void Wait() { WaitForSingleObject(mHandle, INFINITE);};

        ~CEventW()
        {
            CloseHandle(mHandle);
        };



    private:
        HANDLE mHandle;
    };
#endif

} // namespace fo2base
