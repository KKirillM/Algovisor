#ifndef __RDTSC_H__
#define __RDTSC_H__

#ifdef _WIN32

#include <intrin.h>
#pragma intrinsic(__rdtsc)
inline unsigned __int64 rdtsc() { return __rdtsc(); }

#elif (defined(__linux__) || defined(__MACH__))

inline unsigned long long rdtsc()
{
    unsigned a, d;
    __asm__ volatile("rdtsc" : "=a" (a), "=d" (d));
     return ((unsigned long long) a) | (((unsigned long long) d) << 32);
}

#endif // _WIN32

#endif // __RDTSC_H__
