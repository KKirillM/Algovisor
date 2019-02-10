// interface IRc
//
//////////////////////////////////////////////////////////////////////

/*
��� �������� ������� ����������� �������, ������������ ������ IRC_IMPLEMENT. 

������� ����� IRC_IMPLEMENT � IRC_IMPLEMENT2 ������� � ���, ��� ������ ����� �������� ���������� ���������� ������,
������� ��������� �������� �������������� ������� � �������� ���� ��������� ������.

1. ���� ��� ��� ��������� ������ ������� �������� ���������� ������  � ������� IRC_INIT_CPP(). ��� ����� ������� � .cpp-�����
2. � ���� ������� ������������ ������ ������� �������� ������ IRC_IMPLEMENT2(). �� ��������� ���������� ������ � ������ AddRef1/Release1
3. � ������������ ������� ������������ ������ ������� �������� ������������� IRC_INIT2(). �� �������������� �������.
4. � ��������� ���������� ��������� ����������� ����� ������ � .cpp-����� � ����������� ������. ��� �������� � ������� ������� IRC_INIT_CPP_NAMED(className)
5. ��������� IRC_INIT_CPP_NAMED ��������� �� ���������� �����, ����� ���� �������� ������� ��������� extern � ������� ������� IRC_INIT_CPP_EXT()
6. � ��� ����� ��� ����� ����������� ������� �������� ��������� �������� IRC_PRINT().
*/

#if !defined(AFX_IRC_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_)
#define AFX_IRC_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stuff.h"
#include "../../P2SysLog/Include/P2SysLog.h"

#include "../../P2Common/Include/P2Types.h"

//////////////////////////////////////////////////////////////////////
// IRc

struct IRc
{
protected:
	virtual ~IRc() {}
public:
    virtual LONG AddRef1() = 0;
    virtual LONG Release1() = 0;

//    CHECK_FOR_DELETE__(IRc)
};

//
// Implemetation for interface
//

#define IRC_IMPLEMENT()                                     \
    LONG m_RC_counter1__;                                   \
    LONG AddRef1()                                          \
    {                                                       \
        return ::InterlockedIncrement(&m_RC_counter1__);    \
    }                                                       \
    LONG Release1()                                         \
    {                                                       \
        LONG c = ::InterlockedDecrement(&m_RC_counter1__);  \
        if (c == 0) delete this;                            \
        return c;                                           \
    }

#define IRC_INIT() m_RC_counter1__(1)

#define IRC_GET_RC() m_RC_counter1__

//////////////////////////////////////////////////////////////////////
// for DEBUG

#define IRC_IMPLEMENT_DBG__(name)                           \
    LONG m_RC_counter1__;                                   \
    LONG AddRef1()                                          \
    {                                                       \
        LONG c = ::InterlockedIncrement(&m_RC_counter1__);  \
		P2TRACE(("AddRef1 on " #name "(0x%p), rc: %ld", this, c))  \
        return c;                                           \
    }                                                       \
    LONG Release1()                                         \
    {                                                       \
        LONG c = ::InterlockedDecrement(&m_RC_counter1__);  \
		P2TRACE(("Release1 on " #name "(0x%p), rc: %ld", this, c)) \
        if (c == 0) delete this;                            \
        return c;                                           \
    }

/////////////////////////////////////////////////////////////////////////
#include <map>
#include <string>
typedef std::map<std::string, LONG*> CInstCnts;
typedef std::pair <std::string, LONG*> InstCnts_Pair;

class CInstCntMng
{
private:
	CInstCnts m_instCnts;
public:
	LONG Add(const char * const className, LONG * pInstCnt)
	{
		*pInstCnt = 0;
		m_instCnts.insert(InstCnts_Pair ( className, pInstCnt ));
		return 0;
	}
	void Print()
	{
		for(CInstCnts::iterator it = m_instCnts.begin(); it != m_instCnts.end(); ++it)
		{
			if (*(it->second) != 0)
			{
				P2TRACE_INFO_EX(("IRC", "class %s, cnt %d", it->first.c_str(), *(it->second)));
			}
		}
	}
};


class CIncDec
{
public:
	CIncDec(LONG * pInstCnt)
	{
		m_pInstCnt = pInstCnt;
		::InterlockedIncrement(m_pInstCnt);
	}
	~CIncDec()
	{
		::InterlockedDecrement(m_pInstCnt);
	}
private:
	LONG * m_pInstCnt;
};

//
// Implemetation for interface
//

#define IRC_IMPLEMENT2()                                     \
    LONG m_RC_counter1__;                                   \
    LONG AddRef1()                                          \
    {                                                       \
        return ::InterlockedIncrement(&m_RC_counter1__);    \
    }                                                       \
    LONG Release1()                                         \
    {                                                       \
        LONG c = ::InterlockedDecrement(&m_RC_counter1__);  \
        if (c == 0) delete this;                            \
        return c;                                           \
    }														\
	static LONG m_instCnt;									\
	CIncDec m_IncDec;


	

#define IRC_INIT2() m_RC_counter1__(1), m_IncDec(&m_instCnt)
//#define IRC_GET_RC() m_RC_counter1__

#define IRC_INIT_CPP()		CInstCntMng& GetCInstCntMng()						\
							{                                                   \
								static CInstCntMng s_CInstCntMng;		        \
								return s_CInstCntMng;                           \
							}								                    \
	                                                                            
#define IRC_INIT_CPP_EXT()	CInstCntMng& GetCInstCntMng();	
	

#define IRC_INIT_CPP_NAMED(className)						\
	LONG className::m_instCnt = GetCInstCntMng().Add(#className, &(className::m_instCnt));	 

#define IRC_PRINT() GetCInstCntMng().Print();

// for debug
////////
///////

#define IRC_IMPLEMENT_DBG2__(name)                           \
    LONG m_RC_counter1__;                                   \
    LONG AddRef1()                                          \
    {                                                       \
        LONG c = ::InterlockedIncrement(&m_RC_counter1__);  \
		P2TRACE(("AddRef1 on " #name "(0x%p), rc: %ld", this, c))  \
        return c;                                           \
    }                                                       \
    LONG Release1()                                         \
    {                                                       \
        LONG c = ::InterlockedDecrement(&m_RC_counter1__);  \
		P2TRACE(("Release1 on " #name "(0x%p), rc: %ld", this, c)) \
        if (c == 0) delete this;                            \
        return c;                                           \
    }														\
	static LONG m_instCnt;									\
	CIncDec m_IncDec;



/////////
/////////

/////////////////////////////////////////////////////////////////////////
//#endif
#ifdef _DEBUG
  #define IRC_IMPLEMENT_DBG(name)   IRC_IMPLEMENT_DBG__(name)
  #define IRC_IMPLEMENT_DBG2(name)   IRC_IMPLEMENT_DBG2__(name)
#else // !_DEBUG
  #define IRC_IMPLEMENT_DBG(name)   IRC_IMPLEMENT()
  #define IRC_IMPLEMENT_DBG2(name)   IRC_IMPLEMENT2(name)
#endif // _DEBUG

#endif // !defined(AFX_IRC_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_)
