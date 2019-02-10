#pragma once

#define LOG_CAT_T "CHRTimer"

#include "../../../Plaza-II/P2Common/Include/P2Windows.h"
#include "../../../Plaza-II/P2SysLog/Include/P2SysLog.h"

class CHRTimer
{
public:
	inline CHRTimer();
	inline BOOL start();
	inline BOOL startWCounter();
	inline BOOL stop(UINT64 * lastTimeMcs = NULL);
	inline UINT64 getTimeSumInMcs();
	inline UINT32 getStartCounter();
	inline UINT64 getTimePerStartInMcs();
	inline void SetName(char * name);
private:
	LONGLONG m_timeSumInCounts;
	LONGLONG m_lastStartTimeInCounts;
	BOOL m_timerStarted;
	LONGLONG m_countsPerSec;
	BOOL m_isErr;
	UINT32 m_startCounter;
	char m_name[100];
};

class AutoTimer
{
public:
    inline AutoTimer(CHRTimer & timer) : m_timer(timer)
    {
		m_timer.startWCounter();
    }
    inline ~AutoTimer()
    {
	    m_timer.stop();
    }

	inline void SetName(char * name)
	{
		m_timer.SetName(name);
	}

private:
	CHRTimer& m_timer;
};
CHRTimer::CHRTimer()
{
	m_isErr = false;
	m_timerStarted = false;
	m_timeSumInCounts = 0;
	BOOL res;
	LARGE_INTEGER countsPerSec_;
	res = QueryPerformanceFrequency(&countsPerSec_);
	m_countsPerSec = countsPerSec_.QuadPart;
	m_isErr = !res;
	m_startCounter = 0;
	memset(m_name, 0, sizeof(m_name));
}

BOOL CHRTimer::start()
{
	// 0
	//P2TRACE_INFO_EX((LOG_CAT_T, "CHRTimer start"));
	//P2TRACE_INFO_EX((LOG_CAT_T, "m_countsPerSec %I64d", m_countsPerSec.QuadPart));
	if(m_isErr) 
	{
		P2TRACE_ERROR_EX((LOG_CAT_T, "Can't do start because m_isErr is already false"));
		return m_isErr;
	}
	BOOL res = false;
	if(!m_timerStarted)
	{
		m_timerStarted = true;
		LARGE_INTEGER lastStartTimeInCounts_;
		res = QueryPerformanceCounter(&lastStartTimeInCounts_);
		m_lastStartTimeInCounts = lastStartTimeInCounts_.QuadPart;
		if(!res)
		{
			P2TRACE_ERROR_EX((LOG_CAT_T, "Can't do start because QueryPerformanceCounter returned %d not true", res));
		}
	}
	else
	{
		P2TRACE_ERROR_EX((LOG_CAT_T, "Can't do start because m_timerStarted is true"));
	}
	//P2TRACE_INFO_EX((LOG_CAT_T, "m_lastStartTimeInCounts %I64d", m_lastStartTimeInCounts.QuadPart));
	return !res;
}

BOOL CHRTimer::startWCounter()
{
	++m_startCounter;
	return(start());
}

UINT32 CHRTimer::getStartCounter()
{
	return(m_startCounter);
}

UINT64 CHRTimer::getTimePerStartInMcs()
{
	if(m_startCounter)
	{
		return(getTimeSumInMcs()/m_startCounter);
	}
	else
	{
		return(0);
	}
}

BOOL CHRTimer::stop(UINT64 * lastTimeMcs /*= NULL*/)
{
	// 0
	//P2TRACE_INFO_EX((LOG_CAT_T, "CHRTimer stop"));
	if(m_isErr) 
	{
		P2TRACE_ERROR_EX((LOG_CAT_T, "Can't do stop because m_isErr is already false"));
		return m_isErr;	
	}
	BOOL res = false;
	if(m_timerStarted)
	{
		m_timerStarted = false;
		LARGE_INTEGER curTimeInCounts_;
		res = QueryPerformanceCounter(&curTimeInCounts_);
		LONGLONG curTimeInCounts = curTimeInCounts_.QuadPart;
		//P2TRACE_INFO_EX((LOG_CAT_T, "curTimeInCounts %I64d", curTimeInCounts.QuadPart));
		LONGLONG cur;
		cur = (curTimeInCounts - m_lastStartTimeInCounts);
		//UINT64 curMks = (cur*1000000)/m_countsPerSec;
		//P2TRACE_INFO_EX((LOG_CAT_T, "cur time for %s %d", m_name, curMks));
		//P2TRACE_INFO_EX((LOG_CAT_T, "avg time for %s %d", m_name, getTimePerStartInMcs()));
		m_timeSumInCounts += cur;
		if(lastTimeMcs)
		{
			*lastTimeMcs = (cur*1000000)/m_countsPerSec;
		}
	}
	else
	{
		P2TRACE_ERROR_EX((LOG_CAT_T, "Can't do stop because m_timerStarted is not true"));
	}
	return !res;
}

UINT64 CHRTimer::getTimeSumInMcs()
{
	UINT64 timeSumInMicrosec = (m_timeSumInCounts*1000000)/m_countsPerSec;
	return timeSumInMicrosec;
}
void CHRTimer::SetName(char * name)
{
	strcpy(m_name, name);
}