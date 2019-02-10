#ifndef _TIMER_HPP_
#define _TIMER_HPP_

#include <cstdint>

#include "../../../../Plaza-II/P2Common/Include/P2Types.h"
#include "../../../../Plaza-II/P2Utils/Include/PerformanceCounter.hpp"

namespace transport {

	class Timeout 
	{
		uint64_t _finish; 
		static const uint64_t Q;    // TPS / 10^3 
		static const uint64_t R;    // TPS % 10^3 
		public: 
		Timeout(): _finish(0) {}
		Timeout(uint32_t ms) { set(ms); }
		void set_at(uint64_t now, uint32_t ms) { _finish = now + ms * Q + (ms * R) / (1000u); } 
		void set(uint32_t ms) { set_at(now(), ms); }
		void set_minimal() { set(1); } 
		bool expired_at(uint64_t time_point) const {  return _finish == 0 || time_point > _finish; }
		bool expired() const { return expired_at(now()); }
		void clear() { _finish = 0; }
		static uint64_t now(); 
		static const uint64_t TPS;  // "ticks per second" TPS = Q*10^3 + R 
	};


	class McsTimeout 
	{
		uint64_t _finish; 
		static const uint64_t Q;    // TPS / 10^6
		static const uint64_t R;    // TPS % 10^6  TPS = Q * 10^6 + R
		public: 
		McsTimeout(): _finish(0) {}
		void set_at(uint64_t now, uint32_t mcs) { _finish = now + mcs * Q + (mcs * R) / (1000000u); } 
		bool expired_at(uint64_t time_point) const {  return _finish == 0 || time_point > _finish; }
	};


	inline uint64_t Timeout::now() 
	{ 
		LARGE_INTEGER tick; 
		P2QueryPerformanceCounter(&tick); 
		return tick.QuadPart;
	}

	void microsecond_delay(uint32_t mcs);

	//
	// Ограничение скорости отправки пакетов.
	// Перед каждой отправкой пакета необходимо вызывать функцию control, которая обеспечит необходимую паузу.
	//
	class Rate
	{
		const uint64_t 	_delta;
		uint64_t 		_last;
		void operator=(const Rate&);
		public:
		// pps:  сколько пакетов в секунду можно отправлять.
		Rate(int pps):_delta((pps != 0) ? (Timeout::TPS / pps) : 0), _last(0) 
		{}

		void control()
		{
			if (_delta == 0)
				return;
			uint64_t now = Timeout::now();
			if (now > _last + _delta)
			{
				_last = now;
				return;
			} 
			_last += _delta;
			do { 
#if defined(_WIN32)
			_mm_pause(); 
#endif 
			} while (Timeout::now() < _last);
		} 
	};
} // namespace transport


#endif // _TIMER_HPP_ 

