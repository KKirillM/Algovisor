// CSdbg.h: interface for the CCSdbg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CSDBG_H__8A47881D_5D37_4475_A3C0_22F2A265DBCF__INCLUDED_)
#define AFX_CSDBG_H__8A47881D_5D37_4475_A3C0_22F2A265DBCF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CS.h"
#include "../../P2SysLog/Include/P2SysLog.h"
#include <string.h>

////////////////////////////////////////////////////////////////////////////
// CCSdbg
//
//  - Don't use from P2ysLog! ;-)
//

#ifdef _MSC_VER
#pragma _pm(CCSdbg used - do not forget replace to CCS!)
#endif

class CCSdbg
{
public:
	CCSdbg(const char* pFile, int line)
        : m_line(line)
    {
        strncpy(m_file, pFile, sizeof(m_file)-1);
        m_file[sizeof(m_file)-1] = '\0';

        P2TRACE_INFO_EX(("CCSdbg", "CS created;cs '%s',%d", m_file, m_line))
    }

	void Lock()
    {
        m_cs.Lock();

        P2TRACE_INFO_EX(("CCSdbg", "CS locked   >>>;cs '%s',%d", m_file, m_line))
    }
	void Unlock()
    {
        P2TRACE_INFO_EX(("CCSdbg", "CS unlocked <<<;cs '%s',%d", m_file, m_line))

        m_cs.Unlock();
    }

#ifdef _MSC_VER
#ifdef _DEBUG
    void MBF__(){ m_cs.MBF__(); }
    void MBL__(){ m_cs.MBL__(); }
#endif // _DEBUG
#endif // _MSC_VER

private:
    char    m_file[300];
    int     m_line;
    CCS     m_cs;
};

#endif // !defined(AFX_CSDBG_H__8A47881D_5D37_4475_A3C0_22F2A265DBCF__INCLUDED_)
