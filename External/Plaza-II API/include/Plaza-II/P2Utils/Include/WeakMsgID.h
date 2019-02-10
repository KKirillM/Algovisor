#ifndef _WEAKMSGID_H_
#define _WEAKMSGID_H_

#include "../../P2SysTypes/Include/P2SysTypes.h"
#include "../../P2Utils/Include/rdtsc.h"

// WeakID нужно поддерживать как структуру, никаких виртуальных функций!
// структуру можно использовать в контейнерах от STL

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4201) // C4201: nonstandard extension used : nameless struct/union
#endif // _MSC_VER

struct WeakID
{
	union
	{
        struct
        {
            UINT64 /*m_session*/    : 16; // low bits
            UINT64 /*m_rnd*/        : 24;
            UINT64 m_cnt            : 24; // high bits
        };
		UINT64 m_bignum;
	};

	bool operator <(const WeakID& x) const
	{
		return m_bignum < x.m_bignum;
	}

	WeakID& operator =(const WeakID& x)
	{
		m_bignum = x.m_bignum; 
		return *this;
	}
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#endif // _WEAKMSGID_H_
