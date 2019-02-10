// StateVer.h: interface for the CStateVer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATEVER_H__63C46D55_CDCF_4864_93E2_03C1B3E45743__INCLUDED_)
#define AFX_STATEVER_H__63C46D55_CDCF_4864_93E2_03C1B3E45743__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// CStateVer

template <class TState, class TVer = int>
class CStateVer
{
public:
	explicit CStateVer(TState state) : m_state(state), m_ver(0) {}
//	CStateVer(const CStateVer& state) 

	TVer GetVer() const { return m_ver; }
    TState GetState() const { return m_state; }

	bool Check(TState state, TVer ver) const
		{ return m_state == state && m_ver == ver; }
    void UpdateVer(){ m_ver++; }
    TVer GetNextVer(){ return m_ver + 1; }

    const CStateVer& operator=(TState state)
		{ UpdateVer(); m_state = state; return *this; }
	operator TState() const { return m_state; }
    bool operator==(const CStateVer &state) const
        { return Check(state.m_state, state.m_ver); }
    bool operator==(TState state) const
        { return m_state == state; }
    bool operator!=(const CStateVer &state) const
        { return !Check(state.m_state, state.m_ver); }
    bool operator!=(TState state) const
        { return m_state != state; }
private:
	TState	m_state;
	TVer	m_ver;
};

//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_STATEVER_H__63C46D55_CDCF_4864_93E2_03C1B3E45743__INCLUDED_)
