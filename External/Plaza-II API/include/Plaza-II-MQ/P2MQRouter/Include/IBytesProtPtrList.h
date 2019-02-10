// interface IBytesProtPtrList
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IBYTESPROTPTRLIST_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_)
#define AFX_IBYTESPROTPTRLIST_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IBytesWLen.h"

//////////////////////////////////////////////////////////////////////
// IBytesProtPtrList

struct IBytesProtPtrList
{
	virtual IBytesWLen* RemoveHead(bool lock = true) = 0;
	virtual void DeleteAll(bool lock = true) = 0;
	virtual void Add(IBytesWLen* pI, bool lock = true) = 0;

    CHECK_FOR_DELETE__(IBytesProtPtrList)
};

//////////////////////////////////////////////////////////////////////
// IBytesPriorProtPtrList

struct IBytesPriorProtPtrList
{
	virtual IBytesWLenPriority* RemoveHead(bool lock = true) = 0;
	virtual void DeleteAll(bool lock = true) = 0;
	virtual void Add(IBytesWLenPriority* pI, bool lock = true) = 0;

    CHECK_FOR_DELETE__(IBytesPriorProtPtrList)
};

#endif // !defined(AFX_IBYTESPROTPTRLIST_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_)
