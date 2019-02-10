// IEmpty.h: interface for the IEmpty class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IEMPTY_H__0C9DE048_6473_4A9B_8655_13FAC0A86363__INCLUDED_)
#define AFX_IEMPTY_H__0C9DE048_6473_4A9B_8655_13FAC0A86363__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IModuleServices.h"
#include "../../../Plaza-II/P2Utils/Include/ServiceDriver.h"

struct IEmpty : public IMQPluginImpl<1, 0>
{
};

#endif // !defined(AFX_IEMPTY_H__0C9DE048_6473_4A9B_8655_13FAC0A86363__INCLUDED_)
