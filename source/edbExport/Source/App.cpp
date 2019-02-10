#include "StdAfx.h"
#include <stuff.h>
#include "./App.h"
#include "./CsvWriter.h"
#include "./DumpWriter.h"

P2ERR CApp::Init(const TInputParams& ip)
{
    if (ip.InputFile.empty() || ip.SchemaIni.empty() || ip.SchemaName.empty())
    {
        return P2ERR_NOT_OK;
    }
    m_inFileName = ip.InputFile;
    unsigned splitBy = 0;
    if (!ip.SplitBy.empty())
    {
        int _splitBy = atoi(ip.SplitBy.c_str());
        if (_splitBy <= 0)
        {
            P2LogErrorWithCodeEx( P2ERR_COMMON_WRONG_INI_PARAMETER, LOG_CAT_, "Invalid 'split' value: '%s'", ip.SplitBy.c_str() );
            return P2ERR_COMMON_WRONG_INI_PARAMETER;
        }
       splitBy = static_cast< unsigned >(_splitBy);
    }
    string outputBaseFileName;
    if (!ip.OutputFile.empty())
    {
        outputBaseFileName = ip.OutputFile;
    }
    else
    {
        const char * p = ip.InputFile.c_str();
        const char * pLastDot = strrchr(p, '.');
        if (NULL == pLastDot)
            pLastDot = p + ip.InputFile.length();
        const char * pLastSlash = p;
        size_t pos = ip.InputFile.length();
        while (pos--)
        {
            if (p[pos] == '\\' || p[pos] == '/')
            {
                pLastSlash = p + pos + 1;
                break;
            }
        }
        outputBaseFileName.assign(pLastSlash, pLastDot - pLastSlash);
        if (outputBaseFileName.empty())
        {
            P2LogErrorWithCodeEx( P2ERR_COMMON_WRONG_INI_PARAMETER, LOG_CAT_, "Cannot make base name from '%s'", ip.InputFile.c_str() );
            return P2ERR_COMMON_WRONG_INI_PARAMETER;
        }
    }
    m_bFastMode = ip.FastMode;
    if (ip.Ranges.empty())
    {
        if (m_bFastMode) {
            P2LogErrorWithCodeEx( P2ERR_COMMON_WRONG_INI_PARAMETER, LOG_CAT_, "Cannot use fast mode without range" );
            return P2ERR_COMMON_WRONG_INI_PARAMETER;
        }
        m_ranges.insert(TRevRange(P2EREPL_MIN_REVISION, P2EREPL_MAX_REVISION));
    }
    else
    {
        const char * p = ip.Ranges.c_str();
        do 
        {
            const char * pSemicolon = strchr(p, ';');
            string range;
            if (NULL == pSemicolon)
            {
                range.assign(p);
                p += range.length();
            }
            else
            {
                range.assign(p, pSemicolon - p);
                p = pSemicolon + 1;
            }
            if (!ParseAndCheckRange(range))
            {
                return P2ERR_COMMON_WRONG_INI_PARAMETER;
            }
        }
        while (*p);
    }
    try
    {
        m_pWriter = ip.AsDump ? static_cast< IWriter * >(new CDumpWriter()) : static_cast< IWriter * >(new CCsvWriter());
    }
    catch(std::bad_alloc&)
    {
        return P2ERR_COMMON_NO_MEMORY;
    }
    P2ERR res;
    if (P2FAILEDV(m_pWriter->Init(outputBaseFileName, splitBy, ip.ReplaceNonPrintingChars,
        ip.IgnoreUnknownEvents, ip.SchemaIni, ip.SchemaName, ip.TimeHeader), res))
    {
        P2LogErrorWithCodeEx(res, LOG_CAT_, "Writer init error");
        delete m_pWriter;
        return res;
    }

    return P2ERR_OK;
}

bool CApp::ParseAndCheckRange(const string& range)
{
    if (range.empty())
    {
        return true;
    }
    const char * p = range.c_str();
    const char * pColon = strchr(p, ':');
    if (NULL == pColon)
    {
        P2LogErrorEx( LOG_CAT_, "Invalid range substring: '%s'", range.c_str() );
        return false;
    }
    string leftSide(p, pColon - p);
    P2EREPL_REVISION_TYPE minRev = P2EREPL_MIN_REVISION, maxRev = P2EREPL_MAX_REVISION;
    minRev = P2StrToI64(leftSide.c_str());
    if (*(++pColon))
    {
        maxRev = P2StrToI64(pColon);
    }
    TRevRange rr(minRev, maxRev);
    //P2SysPrintf("Range string '%s': %I64d - %I64d\n", range.c_str(), minRev, maxRev);
    if (!rr.isValid())
    {
        P2LogErrorEx(LOG_CAT_, "Invalid range " P2EREPL_REVISION_PRINTF_FORMAT "-" P2EREPL_REVISION_PRINTF_FORMAT "; substr '%s'",
            rr.m_minRev, rr.m_maxRev, range.c_str());
        return false;
    }
    if (m_ranges.end() == m_ranges.find(rr)) // check for intersection
    {
        m_ranges.insert(rr);
    }
    else
    {
        P2LogErrorEx(LOG_CAT_, "Intersected range " P2EREPL_REVISION_PRINTF_FORMAT "-" P2EREPL_REVISION_PRINTF_FORMAT, rr.m_minRev, rr.m_maxRev);
        return false;
    }
    return true;
}

P2ERR CApp::Run()
{
    P2ERR res;

    P2eReplStorageStartup();
    for ( ; ; )
    {
        P2AutoEReplStorage storageIn;
        string storageConnStr = "cache_type=Last1;flush_mode=PerTrans;file=";
        storageConnStr += m_inFileName;
        if (P2FAILEDV(P2eReplStorageOpen(storageConnStr.c_str(), &storageIn), res))
        {
            break;
        }
        
        m_activeRange = *m_ranges.begin();
        m_bWaitCommit = false;
        m_firstClearRev = P2EREPL_MAX_REVISION;
        
        if (m_bFastMode)
        {
            if (P2FAILEDV(P2eReplStorageLoad(storageIn), res)) {
                P2LogErrorWithCodeEx(res, LOG_CAT_, "P2eReplStorageLoad failed");
                break;
            }
            for( ; ; )
            {
                if (P2FAILEDV(P2eReplStorageEnumRecords(storageIn, m_activeRange.m_minRev, m_activeRange.m_maxRev,
                    [](const TEReplRec* pRec, void* pParam) -> P2ERR {
                        return static_cast< CApp * >(pParam)->ProcessRec(0, pRec);
                    }, this), res))
                {
                    if (P2ERR_NOT_OK == res) {
                        continue;
                    }
                    if (P2ERR_COMMON_CANCEL_ENUM == res)
                    {
                        P2TRACE_INFO_EX((LOG_CAT_, "All ranges were dumped"))
                        res = P2ERR_OK;
                        break;
                    }
                    P2LogErrorWithCodeEx(res, LOG_CAT_, "Enum failed");
                    break;
                }
            }
        }
        else
        {
            IP2eReplLLStorage * pInLLStorage = NULL;
            if (P2FAILEDV(P2eReplStorageGetLLStorage(storageIn, &pInLLStorage), res)) {
                break;
            }
            if (P2FAILEDV(pInLLStorage->Load(cbRec, cbCommit, this), res))
            {
                if (P2ERR_COMMON_CANCEL_ENUM == res)
                {
                    P2TRACE_INFO_EX((LOG_CAT_, "All ranges were dumped"))
                    res = P2ERR_OK;
                    break;
                }
            }
        }
        break;
    }
    m_pWriter->Deinit();
    delete m_pWriter;
    P2eReplStorageCleanup();
    return res;
}

P2ERR CApp::ProcessRec(INT64 offset, const TEReplRec* pRec)
{
    if (m_bAbortOperation)
        return P2ERR_COMMON_EXITCODE_TERMINATED;

    if (P2EREPL_MAX_REVISION == m_firstClearRev)
    {
        m_firstClearRev = pRec->rev;
    }
    //P2TRACE_INFO(("DATA(U) " P2EREPL_REVISION_PRINTF_FORMAT, pRec->rev))
    if (m_activeRange.inRange(pRec->rev) || m_bWaitCommit)
    {
        RETURN_P2ERR(m_pWriter->WriteEvent(pRec, offset));
        if (!m_bFastMode) {
            m_bWaitCommit = true;
        }
    }
    if (pRec->rev >= m_activeRange.m_maxRev)
    {
        TRanges::iterator it = m_ranges.upper_bound(TRevRange(pRec->rev + 1, pRec->rev + 1));
        if (m_ranges.end() != it) {
            m_activeRange = *it;
            if (m_bFastMode) {
                return P2ERR_NOT_OK; // special code for restart enum from new rev
            }
        }
        else if (m_bFastMode) {
            return P2ERR_COMMON_CANCEL_ENUM; // stop enum
        }
    }
    return P2ERR_OK;
}

P2ERR CApp::ProcessCommit(P2EREPL_REVISION_TYPE minRev, P2EREPL_REVISION_TYPE maxRev)
{
    //P2TRACE_INFO(("COMMIT(U); minRev " P2EREPL_REVISION_PRINTF_FORMAT "; maxRev " P2EREPL_REVISION_PRINTF_FORMAT, minRev, maxRev))
    if (minRev > m_firstClearRev)
    {
        P2LogErrorEx( LOG_CAT_, "ClearOldRev block detected in the input file.");
        P2LogErrorEx( LOG_CAT_, "Use edbtrim.exe to remove waste space from edb file before using this utility.");
        return P2ERR_COMMON_WRONG_ARGUMENTS;
    }
    else if (P2EREPL_MAX_REVISION == m_firstClearRev && P2EREPL_MIN_REVISION != minRev)
    {
        m_firstClearRev = minRev;
    }
    if (m_activeRange.inRange(maxRev) || m_bWaitCommit)
    {
        RETURN_P2ERR(m_pWriter->WriteCommit());
    }
    m_bWaitCommit = false;
    if (maxRev >= m_activeRange.m_maxRev)
    {
        TRanges::iterator it = m_ranges.upper_bound(TRevRange(maxRev + 1, maxRev + 1));
        if (m_ranges.end() == it)
        {
            return P2ERR_COMMON_CANCEL_ENUM;
        }
        else
        {
            m_activeRange = *it;
        }
    }
    return P2ERR_OK;
}
