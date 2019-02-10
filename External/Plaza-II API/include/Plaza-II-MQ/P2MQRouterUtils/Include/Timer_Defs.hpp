#ifndef _TIMER_DEFS_HPP_
#define _TIMER_DEFS_HPP_ 

#include "Timer.h"

namespace nm_TimerDefs {

static INT64 GetTicksPerSecond()
{ 
	LARGE_INTEGER tps;
	tps.QuadPart = 0;
	P2QueryPerformanceFrequency(&tps); 
	return tps.QuadPart;
} 

}; // nm_TimerDefs

const INT64 Timer::TPS 	 = 	nm_TimerDefs::GetTicksPerSecond();
const INT64 Timer::TPS_Q = 	nm_TimerDefs::GetTicksPerSecond() / 1000;
const INT64 Timer::TPS_R = 	nm_TimerDefs::GetTicksPerSecond() % 1000;


#endif //_TIMER_DEFS_H_
