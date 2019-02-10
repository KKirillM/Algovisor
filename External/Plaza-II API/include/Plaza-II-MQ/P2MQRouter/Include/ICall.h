// ICall.h: interface ICall
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICALL_H__1132A3E2_34E0_49C0_872B_0942855F8298__INCLUDED_)
#define AFX_ICALL_H__1132A3E2_34E0_49C0_872B_0942855F8298__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../Plaza-II/P2Utils/Include/IRc.h"

struct ICall : public IRc
{
    virtual void Process() = 0;
};

#endif // !defined(AFX_ICALL_H__1132A3E2_34E0_49C0_872B_0942855F8298__INCLUDED_)
