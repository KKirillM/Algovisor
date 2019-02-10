#ifndef _PERFORMANCE_COUNTER_HPP_
#define _PERFORMANCE_COUNTER_HPP_

#ifdef _WIN32
#include "../../P2Common/Include/P2Windows.h"
#elif defined __MACH__
#include <time.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#else 
#include <time.h>
#include <assert.h>
#endif //_WIN32

#include "../../P2Common/Include/P2Types.h"

   // класс для обвязки QueryPerformance*** ф-ий. 

   // TODO: В юниксовой ветке сделан через микросекундный clock_monotonic, что вряд ли правильно 

		inline bool P2QueryPerformanceFrequency(LARGE_INTEGER *pFrequency)
		{
#ifdef _WIN32 
			return ::QueryPerformanceFrequency(pFrequency) != 0;
#elif defined __MACH__
            mach_timebase_info_data_t tbInfo;
            if (0 != mach_timebase_info(&tbInfo))
                return false;
            pFrequency->QuadPart = static_cast<LONGLONG>(tbInfo.denom) * 1000000000ll / static_cast<LONGLONG>(tbInfo.numer);
            
            return true;
#else
			pFrequency->QuadPart = 1000000ll;
			return true;
#endif 
		} 

		inline bool P2QueryPerformanceCounter(LARGE_INTEGER *pCounter) 
		{ 
#ifdef _WIN32
			return ::QueryPerformanceCounter(pCounter) != 0;
#elif defined __MACH__
            pCounter->QuadPart = mach_absolute_time();
            return true;
#else
            struct timespec ts;
            
			if(0 != clock_gettime(CLOCK_MONOTONIC, &ts))
			{
				assert(false);
				pCounter->QuadPart = 0;
				return false;
			}

			pCounter->QuadPart = static_cast<LONGLONG>(ts.tv_sec) * 1000000 + (ts.tv_nsec / 1000); 
			return true;
#endif 
		} 




	





#endif  //_PERFORMANCE_COUNTER_HPP_

