#ifndef P2SYSEXT_INCLUDE
#define P2SYSEXT_INCLUDE

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the P2SYSEXT_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// P2SYSEXT_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#include "../../P2Common/Include/P2Types.h"

#ifdef P2SYSEXT_EXPORTS
#define P2SYSEXT_API P2_API_EXPORTS
#else
#define P2SYSEXT_API P2_API_IMPORTS
#endif

#include "../../P2ACL/Include/P2ACL.h"
#include "../../P2VM/Include/P2VM.h"
//#include "../../P2LRPC/Include/P2LRPC.h"

#endif
