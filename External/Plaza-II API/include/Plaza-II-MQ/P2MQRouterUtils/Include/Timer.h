#ifndef _TIMER_H_
#define _TIMER_H_

#include "../../../Plaza-II/P2Utils/Include/PerformanceCounter.hpp"
#include <limits>
#include <cassert>

#undef max

class Timer
{ 
	static const INT64 TPS;		// "ticks per second" : кол-во тиков в секунду (QueryPerformanceCounter)
	static const INT64 TPS_Q;   // TPS / 1000
	static const INT64 TPS_R;	// TPS % 1000

	//TPS_Q*UINT_MAX не переполняется в INT64, так как на совр. платформах TPS < 2^32 (=> TPS_Q < 2^32/1000). 

	public: 

	static INT64 TicksPerSecond() {
		return TPS;
	}

	static INT64 current_moment()
	{
		LARGE_INTEGER tick; 
		P2QueryPerformanceCounter(&tick); 
		return tick.QuadPart;
	}

	static INT64 future_moment(UINT32 ms)
	{ 
		LARGE_INTEGER now;
		P2QueryPerformanceCounter(&now); 
		return now.QuadPart + TPS_Q*ms + ((ms*TPS_R) / 1000);
	}

	static UINT32 duration(INT64 start, INT64 finish)
	{
		if(start >= finish)
			return 0; 

		const INT64 delta = finish - start;

		// duration = (delta*1000) / TPS; для того, чтобы не было переполнения вычисляем так
		// delta = TPS*q + r, r < TPS =>
		// duration = q*1000 + (r*1000 + TPS/2) / TPS;  TPS/2 добавляется для округления результата

		const INT64 duration = ((delta / TPS) * 1000 + ((delta % TPS) * 1000 + TPS/2) / TPS);

		assert(duration <= std::numeric_limits<UINT32>::max());

		return static_cast<UINT32>(duration);
	}

	bool expired() const 
	{ 
		return m_tick.QuadPart >= m_finish.QuadPart; 
	}

	void update() 
	{ 
		P2QueryPerformanceCounter(&m_tick); 
	}

	bool updated_expired()
	{
		update();
		return expired(); 
	}

	void reset(UINT32 ms)
	{
		P2QueryPerformanceCounter(&m_tick); 
		// finsih = now + (ms*TPS) / 1000 = now + TPS_Q*ms + (ms*TPS_R)/1000; 
		m_finish.QuadPart = m_tick.QuadPart + TPS_Q*ms + ((ms*TPS_R) / 1000);
	}

	UINT32 duration() const
	{ 
		return duration(m_tick.QuadPart, m_finish.QuadPart);
	} 

	INT64 microsecond_duration() const
	{ 
		if(m_tick.QuadPart >= m_finish.QuadPart)
			return 0; 

		const INT64 delta = m_finish.QuadPart - m_tick.QuadPart;

		const INT64 duration = ((delta / TPS) * 1000000 + ((delta % TPS)*1000000 + TPS/2) / TPS);

		return duration; 
	}

	Timer(UINT32 msecTimeout) 
	{ 
		reset(msecTimeout); 
	} 

	private:

	LARGE_INTEGER m_finish;
	LARGE_INTEGER m_tick; 
};


#endif //_TIMER_H_
