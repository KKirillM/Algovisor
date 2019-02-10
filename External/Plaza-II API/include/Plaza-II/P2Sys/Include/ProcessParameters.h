#ifndef _PROCESSPARAMETERS_H_
#define _PROCESSPARAMETERS_H_ 
#include "P2SysAPI.h"

#ifdef _WIN32
P2SYS_API P2ERR InitializeProcessParamsFromIni(HANDLE hProcess, const char* iniSection); 
#endif // _WIN32

#endif //_PROCESSPARAMETERS_H_

