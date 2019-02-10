#if !defined(_TIMEPROFILER_H__INCLUDED_)
#define _TIMEPROFILER_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _MSC_VER
#include "../../P2Common/Include/P2Windows.h"
#endif // _MSC_VER


#include <string>
#include <vector>

using std::string;
using std::vector;

#include "../../../Plaza-II/P2SysLog/Include/P2SysLog.h"
#include "../../../Plaza-II/P2SysTypes/Include/P2SysTypes.h"
#include "PerformanceCounter.hpp"

#ifdef TIME_PROFILING_ON
  #define IFTIMEP(expr) expr
#else
  #define IFTIMEP(expr)
#endif // TIME_PROFILING_ON

#define LOG_CATEGORY "***TP***"

class CTimeProfiler
{
    public:
        CTimeProfiler()
        {
            LARGE_INTEGER m_PerfFreq;
            if (P2QueryPerformanceFrequency(&m_PerfFreq))
            {
                m_TicksPerMcs = m_PerfFreq.QuadPart * 1e-6;
            }
            else
            {
                P2TRACE_ERROR_EX(( LOG_CATEGORY, "QueryPerformanceFrequency() failed" ));
                m_TicksPerMcs = -1;
            }
            reset();
            m_buf.reserve(256);
        }

        size_t registerCounter(string name)
        {
            for (size_t i = 0; i < m_tcds.size(); i++ )
            {
                if (m_tcds[i].m_name == name)
                {
                    P2TRACE_ERROR_EX(( LOG_CATEGORY, "Counter '%s' has already registered", name.c_str() ));
                    return i;
                }
            }
            m_tcds.push_back(TimeCounterData(name));
            return m_tcds.size()-1;
        }

        void startCounter(size_t timerId)
        {
            m_tcds[timerId].startCounter();
        }

        void stopCounter(size_t timerId)
        {
            m_tcds[timerId].stopCounter();
        }

        void cancelCounter(size_t timerId)
        {
            m_tcds[timerId].cancelCounter();
        }

        double getMeasurementSumTime(size_t timerId) const // returns in mcs
        {
            return m_tcds[timerId].getMeasurementSumTime(m_TicksPerMcs);
        }

        size_t getMeasurementCount(size_t timerId) const
        {
            return m_tcds[timerId].getMeasurementCount();
        }

        void print()
        {
            m_buf.clear();
            for (size_t i = 0; i < m_tcds.size(); i++ )
            {
                m_buf.append(m_tcds[i].print(m_TicksPerMcs));
                m_buf.append("  ");
            }
            P2TRACE_INFO_EX(( LOG_CATEGORY, "%s", m_buf.c_str() ))
            reset();
        }

		void reset(bool cleanup = false)
        {
			if (cleanup)
			{
				m_tcds.clear();
			}
			else
			{
				for (size_t i = 0; i < m_tcds.size(); i++ )
				{
					m_tcds[i].reset();
				}
			}
        }

    protected:
        typedef struct TimeCounterData
        {
            string m_name;
            LARGE_INTEGER m_startTime;
            LONGLONG m_sumTime;
            size_t m_count;
            char m_printBuf[128];

            TimeCounterData(string name) : m_name(name)
            {
                m_printBuf[0] = 0;
                reset();
            }

            void reset()
            {
                m_startTime.QuadPart = m_sumTime = 0;
                m_count = 0;
            }

            void startCounter()
            {
                if (m_startTime.LowPart || m_startTime.HighPart)
                {
                    P2TRACE_ERROR_EX(( LOG_CATEGORY, "Counter '%s' has already started", m_name.c_str() ))
                    reset();
                }
                P2QueryPerformanceCounter(&m_startTime);
            }

            void cancelCounter()
            {
				m_startTime.QuadPart = 0;
            }


            void stopCounter()
            {
                LARGE_INTEGER finishTime;
                P2QueryPerformanceCounter(&finishTime);
                m_sumTime += (finishTime.QuadPart - m_startTime.QuadPart);
                m_startTime.QuadPart = 0;
                ++m_count;
            }

            const char* print(double ticksPerMcs)
            {
                int len = P2SysSNPrintf(m_printBuf, sizeof(m_printBuf), "%s: %.2f mcs", m_name.c_str(),
                    m_sumTime / ticksPerMcs);
                if (m_count > 1)
                {
                    double tmp = m_sumTime / ticksPerMcs;
                    tmp /= m_count;
                    P2SysSNPrintf(m_printBuf + len, int(sizeof(m_printBuf) - len), " (%d x %.2f mcs)",
                        m_count, tmp);
                }
                return m_printBuf;
            }

            double getMeasurementSumTime(double ticksPerMcs) const { return m_sumTime / ticksPerMcs; }
            size_t getMeasurementCount() const { return m_count; }

        } TimeCounterData;

        double m_TicksPerMcs; // ticks per microsecond
        vector< TimeCounterData > m_tcds;
        string m_buf;
       
};

#endif // !defined(_TIMEPROFILER_H__INCLUDED_)
