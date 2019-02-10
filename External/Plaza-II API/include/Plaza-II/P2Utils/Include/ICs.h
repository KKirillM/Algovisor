// interface ICs
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICS_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_)
#define AFX_ICS_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stuff.h"

//////////////////////////////////////////////////////////////////////
// ICs

struct ICs
{
    virtual void Lock() = 0;
    virtual void Unlock() = 0;

    CHECK_FOR_DELETE__(ICs)
};

#endif // !defined(AFX_ICS_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_)
