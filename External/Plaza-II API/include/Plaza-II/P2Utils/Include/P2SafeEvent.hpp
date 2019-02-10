#ifndef _P2SAFEEVENT_HPP_
#define _P2SAFEEVENT_HPP_

// ф-ии для работы с эвентом.
/*
В *NIX`ах из под SIGNAL handler`а можно вызывать только "SIGNAL safe" функции.
Подробно этот момент освещен в google > "SIG30-C. Call only asynchronous-safe functions within signal handlers"
Для корректного завершения приложений Plaza-II (в частности роутера) и был разработан P2SafeEvent.
Однако, по состоянию на 07.02.2013 под маком функция sem_init возвращает ошибку (not implemented).
По этому организация завершения приложения (только под маком) была пересмотрена и теперь выглядит так:
1. P2SafeEvent определен как простой P2Event 
2. В обработчике сигнала завершения евент не трогаем - вместо этого взводим флаг завершения (глобальная переменная)
3. В основном цикле приложения вместо P2SafeEventWait(INFINITE) делаем P2SafeEventWait(100), после чего проверяем флаг завершения
NOTE: значение таймаута 100 мс выбрано эвристически. В файле P2MQRouter/Source/Service.cpp оно задается переменной msWaitTimeout
*/

#include "../../../Plaza-II/P2SysTypes/Include/P2SysTypes.hpp" 
#include "AutoHandle.hpp"
#include <memory>

#ifdef _WIN32 
#include "../../P2Common/Include/P2Windows.h"
#endif //_WIN32


#if defined(_WIN32) || defined(__MACH__) 
#include "P2Event.hpp"
typedef P2EVENT_HANDLE  P2SAFEEVENT_HANDLE;

#define P2SafeEventCreate(hEvent, manualReset, initialState)      P2EventCreate(hEvent, manualReset, initialState)
#define P2SafeEventClose(hEvent)                                  P2EventClose(hEvent)
#define P2SafeEventSet(hEvent)                                    P2EventSet(hEvent)
#define P2SafeEventWait(hEvent, timeOut)                          P2EventWait(hEvent, timeOut)

typedef P2AutoEvent P2AutoSafeEvent;

#elif defined(__linux__)
#include <semaphore.h> 

namespace p2safeevent_impl
{

#ifdef _DEBUG
#define P2THREADSE_VERIFY(expr) _ASSERT((expr) == 0)
#else
#define P2THREADSE_VERIFY(expr) expr
#endif

class event_t
{
	sem_t m_sem;

	event_t()
    {
    }

	public: 

	static event_t* Create(bool bInitial)
	{
		event_t *event;
		std::auto_ptr<event_t> guard;

        event = new event_t();
        guard.reset(event);
        
        P2THREADSE_VERIFY(sem_init(&event->m_sem, 0, bInitial ? 1 : 0));

        guard.release();
		return event;
	}


	static void Destroy(event_t* event) 
	{
        P2THREADSE_VERIFY(sem_destroy(&(event->m_sem)));
        delete event;
	}

	void Set() 
	{ 	
        P2THREADSE_VERIFY(sem_post(&m_sem));
	}

	P2ERR Wait()
	{ 
		int r = sem_wait(&m_sem);
		assert(r == 0 || (r == -1 && errno == EINTR));
		(void)r;
		return P2ERR_OK;
	} 
};

}; //p2safeevent_impl


typedef p2safeevent_impl::event_t* P2SAFEEVENT_HANDLE;


inline P2ERR P2SafeEventCreate(P2SAFEEVENT_HANDLE* hEvent, bool manualReset, bool initialState)
{ 
	if(!hEvent) return P2ERR_COMMON_WRONG_ARGUMENTS;

	*hEvent = p2safeevent_impl::event_t::Create(initialState);

    if(!*hEvent) return P2ERR_COMMON_SYSCALL_FAILED;
	return P2ERR_OK;
}

inline P2ERR P2SafeEventClose(P2SAFEEVENT_HANDLE hEvent)
{
	if(!hEvent) return P2ERR_COMMON_WRONG_ARGUMENTS;
	p2safeevent_impl::event_t::Destroy(hEvent);
	return P2ERR_OK;
}


inline P2ERR P2SafeEventSet(P2SAFEEVENT_HANDLE hEvent)
{ 
	if(!hEvent) return P2ERR_COMMON_WRONG_ARGUMENTS;
	hEvent->Set(); 
	return P2ERR_OK; 
} 

inline P2ERR P2SafeEventWait(P2SAFEEVENT_HANDLE hEvent)
{ 
	return (hEvent != NULL) ? (hEvent->Wait()) : P2ERR_COMMON_WRONG_ARGUMENTS;
}

typedef AutoP2Handle<P2SAFEEVENT_HANDLE, P2SafeEventClose> P2AutoSafeEvent; 

#endif //defined(__linux__) || defined(__MACH__)


#endif // _P2SAFEEVENT_HPP_
