#ifndef _P2WINDOWS_H_
#define _P2WINDOWS_H_

// заменитель прямого включения windows.h

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501	// WinXP/Server 2003
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // _CRT_SECURE_NO_WARNINGS

#include <windows.h>

#endif // _WIN32

#endif // _P2WINDOWS_H_
