#if !defined(_SQLEXECUTIONTIMEWATCHER_H___348587DE_212F_4279_A0CD_AD3ECB4F0F5C___INCLUDED_)
#define _SQLEXECUTIONTIMEWATCHER_H___348587DE_212F_4279_A0CD_AD3ECB4F0F5C___INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../Plaza-II/P2Utils/Include/PerformanceCounter.hpp"
#include "../../../Plaza-II/P2Utils/Include/IRc.h"
#include "../../../Plaza-II-DB/P2DB/Include/P2DBDriver.h"

#define LOG_CATEGORY "SQLExecutionTimeWatcher"

class CSQLExecutionTimeWatcher : public IRc
{
    public:
        IRC_IMPLEMENT();

        CSQLExecutionTimeWatcher(const void * ownerStmtId)
            : IRC_INIT(), m_ownerStmtId(ownerStmtId), m_TicksPerMcs(1.), m_bWatch(false),
            m_thresholdExecTotal(0), m_thresholdExecPerRec(0)
        {
            reset();
        }

        P2ERR Init(P2DB_THRESHOLDS_HANDLE hTimeThresholds)
        {
            if (!hTimeThresholds)
            {
                return P2ERR_COMMON_WRONG_ARGUMENTS;
            }
            LARGE_INTEGER m_PerfFreq;
            if (P2QueryPerformanceFrequency(&m_PerfFreq))
            {
                m_TicksPerMcs = m_PerfFreq.QuadPart * 1e-6;
            }
            else
            {
                P2TRACE_ERROR_EX((LOG_CATEGORY, "QueryPerformanceFrequency() failed"))
                return P2ERR_COMMON_INTERNAL_ERROR;
            }
            P2ERR res;
            if (P2FAILEDV(hTimeThresholds->GetThresholdValue(THRESHOLD_EXECUTE_TOTAL, &m_thresholdExecTotal), res) ||
                P2FAILEDV(hTimeThresholds->GetThresholdValue(THRESHOLD_EXECUTE_PER_RECORD, &m_thresholdExecPerRec), res))
            {
                P2TRACE_ERROR_WITH_CODE_EX((res, LOG_CATEGORY, "Init() failed; error: %0X", res))
                return res;
            }

            if (m_thresholdExecTotal || m_thresholdExecPerRec)
            {
                m_bWatch = true;
            }
           return P2ERR_OK;
        }

        void startCounter()
        {
            if (m_bWatch)
            {
                if (m_startTime.LowPart || m_startTime.HighPart)
                {
                    P2TRACE_ERROR_EX((LOG_CATEGORY, "counter has already been started"))
                    reset(); 
                }
                P2QueryPerformanceCounter(&m_startTime);
            }
        }

        void stopExecCounter()
        {
            if (m_bWatch)
            {
                m_execTime = stopCounter();
            }
        }

        void stopFetchCounter()
        {
            if (m_bWatch)
            {
                m_fetchTime += stopCounter();
                ++m_fetchCount;
            }
        }

        int getFetchCount() const
        {
            return m_fetchCount;
        }

        // Проверяет превышения порогов и обнуляет счётчики
        void checkThresholds()
        {
            if (m_bWatch)
            {
                double curExecTotal = (m_execTime + m_fetchTime) / m_TicksPerMcs;
                if (m_thresholdExecTotal > 0 && m_execTime)
                {
                    if (curExecTotal > m_thresholdExecTotal)
                    {
                        P2TRACE_WARNING_EX((LOG_CATEGORY, "exec_total exceeded; stmt 0x%p; current: %I64u mcs, "
                            "threshold: %I64u mcs (exec: %I64u mcs, %d fetches: %I64u mcs)",
                            m_ownerStmtId, static_cast< UINT64 >(curExecTotal),
                            m_thresholdExecTotal, static_cast< UINT64 >(m_execTime / m_TicksPerMcs),
                            m_fetchCount, static_cast< UINT64 >(m_fetchTime / m_TicksPerMcs)))
                    }
                }
		
                if (m_fetchCount && m_thresholdExecPerRec)
                {
                    double tperrec = curExecTotal / m_fetchCount;
                    if (tperrec > m_thresholdExecPerRec)
                    {
                        P2TRACE_WARNING_EX((LOG_CATEGORY, "exec_per_record exceeded; stmt 0x%p; current: %I64u mcs, "
                            "threshold: %I64u mcs (exec: %I64u mcs, %d fetches: %I64u mcs)",
                            m_ownerStmtId, static_cast< UINT64 >(tperrec),
                            m_thresholdExecPerRec, static_cast< UINT64 >(m_execTime / m_TicksPerMcs),
                            m_fetchCount, static_cast< UINT64 >(m_fetchTime / m_TicksPerMcs)))
                    }
                }
                reset();
            }
        }
        
    protected:
        ~CSQLExecutionTimeWatcher(void) { } // by RC

    private:
        const void * m_ownerStmtId; // для идентификации в логах
        double m_TicksPerMcs; // ticks per microsecond
        LARGE_INTEGER m_startTime;
        int m_fetchCount;
        LONGLONG m_execTime;
        LONGLONG m_fetchTime;
        bool m_bWatch;

        UINT64 m_thresholdExecTotal;
        UINT64 m_thresholdExecPerRec;

        void reset()
        {
            m_execTime = m_startTime.QuadPart = m_fetchTime = m_fetchCount = 0;
        }

        LONGLONG stopCounter()
        {
            LARGE_INTEGER finishTime;
            P2QueryPerformanceCounter(&finishTime);
            LONGLONG interval = finishTime.QuadPart - m_startTime.QuadPart;
            m_startTime.QuadPart = 0;
            return interval;
        }
};

#endif // !defined(_SQLEXECUTIONTIMEWATCHER_H___348587DE_212F_4279_A0CD_AD3ECB4F0F5C___INCLUDED_)
