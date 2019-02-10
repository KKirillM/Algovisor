#ifndef _APP_H__INCLUDED_
#define _APP_H__INCLUDED_

#include <P2SysTypes.hpp>
#include <P2SysScheme.hpp>
#include <P2eReplTypes.h>
#include <P2eReplStorage.hpp>
#include <P2eReplLLStorage.h>
#include "./IWriter.h"

typedef struct _TInputParams
{
    // optional
    string Ranges;
    string SplitBy;
    string OutputFile;
	bool TimeHeader;
    bool ReplaceNonPrintingChars;
    bool AsDump;
    bool FastMode;
    bool IgnoreUnknownEvents;
    // required
    string InputFile;
    string SchemaIni;
    string SchemaName;

    _TInputParams() : ReplaceNonPrintingChars(false), AsDump(false), FastMode(false), IgnoreUnknownEvents(false), TimeHeader(false) { }
} TInputParams;

typedef struct _TRevRange
{
    P2EREPL_REVISION_TYPE m_minRev;
    P2EREPL_REVISION_TYPE m_maxRev;
    bool operator < (const _TRevRange& r) const
    {
        return (m_minRev < r.m_minRev && m_maxRev < r.m_minRev);
    }
    _TRevRange() { };
    _TRevRange(P2EREPL_REVISION_TYPE minRev, P2EREPL_REVISION_TYPE maxRev) : m_minRev(minRev), m_maxRev(maxRev) { }
    bool isValid() const { return m_minRev < m_maxRev; }
    bool inRange(P2EREPL_REVISION_TYPE rev) { return rev >= m_minRev && rev <= m_maxRev; }
} TRevRange;

class CApp
{
public:
    CApp() : m_bFastMode(false), m_bAbortOperation(false), m_pWriter(NULL) { }
    P2ERR Init(const TInputParams& ip);
    P2ERR Run();
    void Abort() { m_bAbortOperation = true; }

private:
    string  m_inFileName;
    bool    m_bFastMode;
    bool    m_bAbortOperation;

    typedef std::set< TRevRange > TRanges;
    TRanges m_ranges;
    TRevRange m_activeRange;
    bool m_bWaitCommit;

    P2EREPL_REVISION_TYPE m_firstClearRev;

    IWriter * m_pWriter;

    bool ParseAndCheckRange(const string& range);
    static P2ERR cbRec(INT64 offset, const TEReplRec* pRec, void* pParam)
    {
        return static_cast< CApp * >(pParam)->ProcessRec(offset, pRec);
    }

    static P2ERR cbCommit(void* pParam, P2EREPL_REVISION_TYPE minRev, P2EREPL_REVISION_TYPE maxRev)
    {
        return static_cast< CApp * >(pParam)->ProcessCommit(minRev, maxRev);
    }

    P2ERR ProcessRec(INT64 offset, const TEReplRec* pRec);
    P2ERR ProcessCommit(P2EREPL_REVISION_TYPE minRev, P2EREPL_REVISION_TYPE maxRev);
};

#endif // _APP_H__INCLUDED_
