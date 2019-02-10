// CSI.h: interface for the CCSI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CSI_H__7F02D548_6366_437F_80FD_1A88DE97CCBC__INCLUDED_)
#define AFX_CSI_H__7F02D548_6366_437F_80FD_1A88DE97CCBC__INCLUDED_

#include "stuff.h"
#include "ICs.h"
#include "CS.h"


#if defined(_WIN32)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../../../Plaza-II/P2Common/Include/P2Windows.h"

#ifdef _MSC_VER
#ifdef _DEBUG
//#define __CS__DIAGNOSTICS__
#define __CS__RECURCOUNT_1__       // RecursionCount must be < 1 after Lock()
#endif // _DEBUG
#endif // _MSC_VER

#ifdef __CS__DIAGNOSTICS__
#pragma _pm(CRITICAL SECTIONS IN DEBUG MODE!!!!!)
#define __CS__EXPLICIT_NL_CHECKING__    // Check max nesting level only if m_maxNestLevel__
// is set. If not defined, then checked for all instances.
#endif // __CS__DIAGNOSTICS__

#endif // _WIN32

//////////////////////////////////////////////////////////////////////
// CCSI

class CCSI : public ICs  
{
public:
	CCSI();
	virtual ~CCSI();

#if defined(_WIN32)
#ifdef __CS__DIAGNOSTICS__
    int     m_maxNestLevel__;
    CCSI(int maxNestLevel); // For use explicit nesting level checking 
#endif // __CS__DIAGNOSTICS__
#endif // _WIN32

	void Lock();
	void Unlock();

#if defined(_WIN32)
#ifdef _MSC_VER
#ifdef _DEBUG
    void MBF__();
    void MBL__();
#endif // _DEBUG
#endif // _MSC_VER
#endif // _WIN32

private:
#if defined(_WIN32)
	CRITICAL_SECTION m_cs;
#elif (defined(__linux__) || defined(__MACH__))
	CCS	m_cs;
#endif

    DISABLE_CLASS_COPY_PV(CCSI)

#if defined(_WIN32)
#ifdef __CS__DIAGNOSTICS__
    int m_nestLevel__;
    char* _GetDbgStr(bool lock, char* pStr)
    {
        static void *pPrev;
        char indent[101];
        for (int i = 0; i < m_nestLevel__-1; i++) indent[i] = '\t';
        indent[i] = '\0';
        sprintf(pStr, "%s0x%08X %s\t0x%08X %c\n",
                      indent,
                      (int)GetCurrentThreadId(),
                      lock ? "lock" : "unlock",
                      (int)this,
                      pPrev && pPrev != this ? '*' : ' '
                      );
        pPrev = this;
        return pStr;
    }
#endif // __CS__DIAGNOSTICS__
#endif // _WIN32
};

//////////////////////////////////////////////////////////////////////

#if defined(_WIN32)

inline CCSI::CCSI()
#ifdef __CS__DIAGNOSTICS__
    : m_nestLevel__(0),
      m_maxNestLevel__(0)
#endif // __CS__DIAGNOSTICS__
{
	::InitializeCriticalSection(&m_cs);
}

#ifdef __CS__DIAGNOSTICS__
inline CCSI::CCSI(int maxNestLevel)
    : m_nestLevel__(0),
      m_maxNestLevel__(maxNestLevel)
{
    ::InitializeCriticalSection(&m_cs);
}
#endif // __CS__DIAGNOSTICS__

inline CCSI::~CCSI()
{
	_ASSERT(m_cs.LockCount == -1);	

	::DeleteCriticalSection(&m_cs);
}

inline void CCSI::Lock()
{
	::EnterCriticalSection(&m_cs);

#ifdef __CS__RECURCOUNT_1__
	_ASSERT(m_cs.RecursionCount == 1);
#endif // __CS__RECURCOUNT_1__

#ifdef __CS__DIAGNOSTICS__
	m_nestLevel__++;
    char dbgStr[100];
#ifdef __CS__EXPLICIT_NL_CHECKING__
    if (m_maxNestLevel__ > 0)
    {
		OutputDebugString(_GetDbgStr(true, dbgStr));
        _ASSERT(m_nestLevel__ <= m_maxNestLevel__);
    }
#else // !__CS__EXPLICIT_NL_CHECKING__
    OutputDebugString(_GetDbgStr(true, dbgStr));
    _ASSERT(m_nestLevel__ == 1);
#endif // __CS__EXPLICIT_NL_CHECKING__
#endif // __CS__DIAGNOSTICS__
}

inline void CCSI::Unlock()
{
#ifdef __CS__DIAGNOSTICS__
    m_nestLevel__--;
    char dbgStr[100];
#ifdef __CS__EXPLICIT_NL_CHECKING__
    if (m_maxNestLevel__ > 0)
    {
        OutputDebugString(_GetDbgStr(false, dbgStr));
    }
#else // !__CS__EXPLICIT_NL_CHECKING__
    OutputDebugString(_GetDbgStr(false, dbgStr));
#endif // __CS__EXPLICIT_NL_CHECKING__
#endif // __CS__DIAGNOSTICS__

	_ASSERT((size_t)m_cs.OwningThread == GetCurrentThreadId());

	::LeaveCriticalSection(&m_cs);
}

#ifdef _MSC_VER
#ifdef _DEBUG
inline void CCSI::MBF__()
{
    _ASSERT((size_t)m_cs.OwningThread != GetCurrentThreadId());
}
inline void CCSI::MBL__()
{
    _ASSERT((size_t)m_cs.OwningThread == GetCurrentThreadId());
}
#endif // _DEBUG
#endif // _MSC_VER

#elif defined(__linux__) || defined(__MACH__)

inline CCSI::CCSI()
{
	
}

inline CCSI::~CCSI()
{
	
}

inline void CCSI::Lock()
{
	m_cs.Lock();
}

inline void CCSI::Unlock()
{
	m_cs.Unlock();
}

#endif // defined(__linux__) || defined(__MACH__)

//////////////////////////////////////////////////////////////////////
class AutoCCSI
{
public:
	inline AutoCCSI(CCSI& ccsi);
	inline AutoCCSI(CCSI& ccsi, bool isLocked);
	inline ~AutoCCSI();
private:
	CCSI& m_ccsi;
};

inline AutoCCSI::AutoCCSI(CCSI& ccsi) : m_ccsi(ccsi)
{
	m_ccsi.Lock();
}

inline AutoCCSI::AutoCCSI(CCSI& ccsi, bool isLocked) : m_ccsi(ccsi)
{
	if (!isLocked) m_ccsi.Lock();
#ifdef _DEBUG
    else CSMBL__(&m_ccsi);
#endif // _DEBUG
}

inline AutoCCSI::~AutoCCSI()
{
	m_ccsi.Unlock();
}


#endif // !defined(AFX_CSI_H__7F02D548_6366_437F_80FD_1A88DE97CCBC__INCLUDED_)