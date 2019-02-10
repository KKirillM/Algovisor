#include "stdafx.h"
#include "Profiler.h"
#include "HelpTools.h"

__int64 CStopwatch::Now(char* s) const //в s строка с именем файла для записи результата
{   
    LARGE_INTEGER liPerfNow;
    QueryPerformanceCounter(&liPerfNow);
    LONGLONG res = liPerfNow.QuadPart - m_liPerfStart.QuadPart;
    char delay[20];
    sprintf_s(delay, 20, "%I64d\n", res);
    HelpTools::WriteToFile(delay, s, true);
    return res;
}