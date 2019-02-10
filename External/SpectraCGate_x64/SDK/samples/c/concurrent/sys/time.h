#ifndef _TIMEIMPL_H
#define _TIMEIMPL_H

#include <windows.h>
#include <time.h>

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

struct timezone
{
	int  tz_minuteswest; /* minutes W of Greenwich */
	int  tz_dsttime;     /* type of dst correction */
};


static unsigned __int64 cyclesInSec = 0;

static const void InitCycleInSec()
{
	LARGE_INTEGER nVal;
	if (!QueryPerformanceFrequency(&nVal))
		return;
	cyclesInSec = nVal.QuadPart;
}

static int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	FILETIME ft;

	unsigned __int64 tmpres = 0;
	static int tzflag = 0;

	if (NULL != tv)
	{
		GetSystemTimeAsFileTime(&ft);
		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;
		tmpres /= 10;
		tmpres -= DELTA_EPOCH_IN_MICROSECS;
		tv->tv_sec = (long)(tmpres / 1000000LL);
		tv->tv_usec = (long)(tmpres % 1000000LL);
	}

	if (NULL != tz)
	{
		if (!tzflag)
		{
			_tzset();
			tzflag++;
		}

		tz->tz_minuteswest = _timezone / 60;
		tz->tz_dsttime = _daylight;
	}

	return 0;
}

static int gettimeofday_rdtsc(struct timeval *tv, struct timezone *tz)
{
	LARGE_INTEGER nVal;
	if (!cyclesInSec) 
		InitCycleInSec();
	QueryPerformanceCounter(&nVal);
	tv->tv_sec = (long)(nVal.QuadPart/cyclesInSec);
	tv->tv_usec = (long)(( (nVal.QuadPart % cyclesInSec) * 1000000LL) / cyclesInSec);
	return 0;
}

#endif //TIMEIMPL_H

