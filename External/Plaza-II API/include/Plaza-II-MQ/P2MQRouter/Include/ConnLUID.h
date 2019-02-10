// ConnLUID.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNLUID_H__3F8A0690_3333_4911_802A_1AF2E8D1005D__INCLUDED_)
#define AFX_CONNLUID_H__3F8A0690_3333_4911_802A_1AF2E8D1005D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../Plaza-II/P2Common/Include/P2Types.h"

namespace conn
{

// Run time (life) unique identifier for connection
typedef UINT64 LUID;

const LUID LUID_UNDEF = -1;

}; // namespace conn

#endif // !defined(AFX_CONNLUID_H__3F8A0690_3333_4911_802A_1AF2E8D1005D__INCLUDED_)
