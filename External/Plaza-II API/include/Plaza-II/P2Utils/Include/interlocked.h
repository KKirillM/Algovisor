#ifndef _INTERLOCKED_H_
#define _INTERLOCKED_H_

#ifdef _MSC_VER
#include "../../P2Common/Include/P2Windows.h"
#endif //_MSC_VER
#if defined(_MSC_VER)
	#include <crtdbg.h>
#endif // defined(_MSC_VER)

// микрософт - козлы. разные версии студии и сдк определяют прототипы InterlockedIncrement по-разному:
// LONG WINAPI InterlockedIncrement(LPLONG lpAddend);
// LONG __cdecl InterlockedIncrement(IN OUT LONG volatile *lpAddend);

// нижележащие макросы пытаются обойти эту дрянь

// объявляем здесь во избежание разночтения сигнатур в разных версиях хидеров
template <class T1, class T2> class TwoTypeMixer
{ 
	public:
	union
	{ 
	T1 m_t1;
	T2 m_t2;
	} u;

	TwoTypeMixer(const T1 t1) { u.m_t1 = t1; }
	TwoTypeMixer(const T2 t2) { u.m_t2 = t2; }

	operator T1 () const { return u.m_t1; }
	operator T2 () const { return u.m_t2; }
}; 

//typedef MultiType<PVOID,LONG> L2PV;
//typedef MultiType<PVOID*,volatile LONG *> PL2PPV;

typedef TwoTypeMixer<LONG *, volatile LONG *> TT_LONGP_VLONGP;

inline INT32 MyInterlockedIncrement(INT32 *lpAddend)
{
#ifdef __GNUC__
	return __sync_add_and_fetch(lpAddend, 1);
#else
	_ASSERT(sizeof(LONG) == sizeof(INT32));
	return InterlockedIncrement(TT_LONGP_VLONGP(reinterpret_cast<LONG *>(lpAddend)));
#endif
}

inline INT32 MyInterlockedDecrement(INT32 *lpAddend)
{
#ifdef __GNUC__
	return __sync_sub_and_fetch(lpAddend, 1);
#else
	_ASSERT(sizeof(LONG) == sizeof(INT32));
	return InterlockedDecrement(TT_LONGP_VLONGP(reinterpret_cast<LONG *>(lpAddend)));
#endif
}

inline PVOID P2InterlockedExchangePointer( PVOID volatile* Target, PVOID Value )
{
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4311 )
#pragma warning( disable : 4312 )
#endif // _MSC_VER

#ifdef __GNUC__
	return __sync_lock_test_and_set( Target, Value );
#else
	return InterlockedExchangePointer( Target, Value );
#endif

#if !(defined(__linux__) || defined (__MACH__))
#pragma warning( pop )
#endif //!(defined(__linux__) || defined (__MACH__))
}

#endif
