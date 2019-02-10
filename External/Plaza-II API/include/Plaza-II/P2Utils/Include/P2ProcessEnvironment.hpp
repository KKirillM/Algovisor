#ifndef _P2PROCESS_ENVIRONMENT_HPP_
#define _P2PROCESS_ENVIRONMENT_HPP_

#ifdef _WIN32
#include "../../P2Common/Include/P2Windows.h"
#elif defined(__linux__) || defined (__MACH__)
#include <stdlib.h> 
#include <string.h>
#endif

#include "../../P2SysTypes/Include/P2SysTypes.hpp"
#include "stuff.h"

inline P2ERR P2GetEnvironmentVariable(const char* name, char* buf, size_t bufsz)
{ 
#ifdef _WIN32
	DWORD bytes = GetEnvironmentVariable(name, buf, static_cast<DWORD>(bufsz));
	if(bytes > bufsz) 	//too small buffer
		return P2ERR_COMMON_INTERNAL_ERROR;

	if(!bytes && GetLastError() != ERROR_ENVVAR_NOT_FOUND)
		return P2ERR_COMMON_SYSCALL_FAILED;

	if(!bytes)
		*buf = 0;
#elif defined(__linux__) || defined (__MACH__)
	const char* var = getenv(name); 
	if(!var)
		*buf = 0;
	else if(strlen(var) + 1 > bufsz) 
		return P2ERR_COMMON_INTERNAL_ERROR;
	else
		strcpy(buf, var); 
#endif 
	return P2ERR_OK; 
}

inline P2ERR P2SetEnvironmentVariable(const char* name, const char* value)
{
#ifdef _WIN32
	if(!SetEnvironmentVariable(name, value))
		return P2ERR_COMMON_SYSCALL_FAILED;
#elif defined(__linux__) || defined (__MACH__)
	const int overwrite = 1;
	if(-1 == setenv(name, value, overwrite))
		return P2ERR_COMMON_SYSCALL_FAILED;
#endif
	return P2ERR_OK; 
}

#if defined(__linux__) || defined (__MACH__)
inline P2ERR P2DeleteEnvironmentVariable(const char* name)
{
	if(-1 == unsetenv(name))
		return P2ERR_COMMON_SYSCALL_FAILED;
	return P2ERR_OK; 
}
#endif 


#ifdef _WIN32
inline P2ERR P2ConvertEnvironmentVariableToHandle(const char* name, HANDLE* ph)
{
	char buf[64];
	RETURN_P2ERR(P2GetEnvironmentVariable(name, buf, sizeof(buf))); 
    if (*buf == 0) {
		*ph = NULL;
    }
    else {
        P2StrToHandle(ph, buf);
    }
	return P2ERR_OK;
}
#elif defined(__linux__) || defined (__MACH__)
inline P2ERR P2ConvertEnvironmentVariableToHandle(const char* name, int* ph)
{
	char buf[64];
	RETURN_P2ERR(P2GetEnvironmentVariable(name, buf, sizeof(buf))); 
	if (*buf == 0)
		*ph = -1;
	else 
		P2StrToDescriptor(ph, buf);
	return P2ERR_OK;
}
#endif // _WIN32

#endif //_P2PROCESS_ENVIRONMENT_HPP_
