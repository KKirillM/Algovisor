#ifndef _TIME_WEIGHTED_AVERAGE_
#define _TIME_WEIGHTED_AVERAGE_

#include "../../../Plaza-II/P2Utils/Include/PerformanceCounter.hpp"
#include "Timer.h"
#include <algorithm>


class TimeWeightedAverage
{ 
	UINT64  m_value;
	UINT64  m_sum;
	INT64   m_start;				// начало расчёта среднего
	INT64   m_value_start;			// начало временного промежутка с текущим значением m_value

public:

	TimeWeightedAverage(UINT64 value): m_value(value)
	{ stat_reset(); }

	TimeWeightedAverage(): m_value(0)
	{ stat_reset(); }

	UINT32 stat_duration() const 
	{ return Timer::duration(m_start, Timer::current_moment()); }

	void stat_reset()
	{
		m_sum = 0;
		m_start = Timer::current_moment(); 
		m_value_start = m_start;
	}

	double average() 
	{ 
		const INT64 current_moment = Timer::current_moment();
		m_sum += m_value * (current_moment - m_value_start); 
		m_value_start = current_moment;
		return ((double)m_sum) / (current_moment - m_start); 
	}

	void update(UINT64 newValue)
	{
		const INT64 current_moment = Timer::current_moment();
		m_sum += m_value * (current_moment - m_value_start); 
		m_value_start = current_moment;
		m_value = newValue; 
	} 

	void update()
	{ update(m_value); } 

	UINT64 value() const 
	{ return m_value; } 

	void swap(TimeWeightedAverage& rhs)
	{
		std::swap(m_value, rhs.m_value);
		std::swap(m_sum, rhs.m_sum);
		std::swap(m_start, rhs.m_start); 
	}
};




#endif //_TIME_WEIGHTED_AVERAGE_
