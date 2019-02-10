#ifndef _P2CONSOLE_HPP_
#define _P2CONSOLE_HPP_

#include "../../P2Common/Include/P2Types.h"

#ifdef _WIN32 
#include "../../P2Common/Include/P2Windows.h"
#elif defined(__linux__) || defined (__MACH__)
#include <signal.h>
#include <stdio.h>
#include <string.h>
#endif // defined(__linux__) || defined (__MACH__)

#ifdef _WIN32
	typedef PHANDLER_ROUTINE P2HANDLER_ROUTINE;
#elif defined(__linux__) || defined (__MACH__)  
    typedef void (*P2HANDLER_ROUTINE)(int);
#endif //defined(__linux__) || defined (__MACH__)


inline bool P2SetConsoleCtrlHandler(P2HANDLER_ROUTINE pHandler)
{
#ifdef _WIN32
	return ::SetConsoleCtrlHandler(pHandler, TRUE) != 0;
#elif defined(__linux__) || defined (__MACH__)
	struct sigaction act;
	memset(&act, 0x0, sizeof(act));
	act.sa_handler = pHandler; 
	if (-1 == sigaction(SIGINT, &act, NULL) ||
	    -1 == sigaction(SIGTERM, &act, NULL))
	{
		return false;
	}
	return true; 
#endif //defined(__linux__) || defined (__MACH__)
}

inline bool P2ResetConsoleCtrlHandler(P2HANDLER_ROUTINE pHandler)
{
#ifdef _WIN32
    return ::SetConsoleCtrlHandler(pHandler, FALSE) != 0;
#elif defined(__linux__) || defined (__MACH__)
    UNUSED(pHandler);
    struct sigaction act;
	memset(&act, 0x0, sizeof(act));
    act.sa_handler = SIG_DFL; 
    if (-1 == sigaction(SIGINT, &act, NULL) ||
        -1 == sigaction(SIGTERM, &act, NULL))
    {
        return false;
    }
    return true; 
#endif // defined(__linux__) || defined (__MACH__)
}

inline bool P2ConsoleSetTitle(const char* title)
{
#ifdef _WIN32
	return ::SetConsoleTitle(title) == TRUE;
#else 
	// В юникс-подобных ОС у приложения нет окна
	return true;
#endif // WIN32
}

#endif //_P2CONSOLE_HPP_
