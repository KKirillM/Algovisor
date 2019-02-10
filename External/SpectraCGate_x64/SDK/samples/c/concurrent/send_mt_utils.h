#ifndef _SEND_MT_UTILS_H_
#define _SEND_MT_UTILS_H_
#include <sys/time.h>
#ifdef _WIN32
	#define _gettimeofday gettimeofday_rdtsc
#else //_WIN32
	#define _gettimeofday gettimeofday
#endif //_WIN32

uint64_t cg_usec_clock()
{
	struct timeval tv;
#ifdef _WIN32
	gettimeofday_rdtsc(&tv, 0);
#else //_WIN32
	gettimeofday(&tv, 0);
#endif
	return tv.tv_sec * 1000000LL + tv.tv_usec;
}

#endif //_SEND_MT_UTILS_H_
