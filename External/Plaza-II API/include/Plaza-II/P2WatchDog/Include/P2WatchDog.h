// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the P2WATCHDOG_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// P2WATCHDOG_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef P2WATCHDOG_EXPORTS
#define P2WATCHDOG_API __declspec(dllexport)
#else
#define P2WATCHDOG_API __declspec(dllimport)
#endif

#include "..\..\..\Plaza-II\P2Common\Include\P2Errors.h"


P2WATCHDOG_API P2ERR  P2WatchDogStartup();
P2WATCHDOG_API void   P2WatchDogCleanup();
P2WATCHDOG_API P2ERR  P2WatchDogStart(UINT32 ms);
P2WATCHDOG_API P2ERR  P2WatchDogStop();


















