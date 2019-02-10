#ifndef _P2STR_H_
#define _P2STR_H_

#include <string.h>
#include <stdlib.h>

#include "../../P2Common/Include/P2Types.h" 

#if defined(__linux__) || defined (__MACH__)
#include <ctype.h>
#include <stdio.h>
#endif // defined(__linux__) || defined (__MACH__)


#if defined(_WIN32)
#pragma warning( push )
#pragma warning( disable : 4996 )   
#endif // defined(_WIN32)

#if defined(__linux__) || defined(__MACH__)
#define stricmp strcasecmp
#define strcmpi strcasecmp
#define _stricmp stricmp
#define _strnicmp strncasecmp
#endif //defined(__linux__) || defined(__MACH__)

inline int P2StrIgnoreCaseCompare(const char* s1, const char* s2)
{
#ifdef _WIN32
	return _strcmpi(s1,s2);
#elif defined(__linux__) || defined(__MACH__)
	return strcasecmp(s1,s2);
#endif // defined(__linux__) || defined(__MACH__) 
}

inline int P2StrIgnoreCaseCompare(const char* s1, const char* s2, size_t length)
{ 
#ifdef _WIN32
	return _strnicmp(s1, s2, length); 
#elif defined(__linux__) || defined(__MACH__)
	return strncasecmp(s1, s2, length);
#endif // defined(__linux__) || defined(__MACH__) 
}

inline char* P2StrDup(const char* s)
{
#ifdef _WIN32
	return _strdup(s);
#elif defined(__linux__) || defined(__MACH__)
	return strdup(s);
#endif // defined(__linux__) || defined(__MACH__)
}

inline char* P2StrConvertToUppercase(char* str)
{
#ifdef _WIN32
	return _strupr(str);
#elif defined(__linux__) || defined(__MACH__)
	for(; *str != 0; ++str) {
		*str = toupper(*str);
	}
	return str;
#endif //defined(__linux__) || defined(__MACH__)
}

inline char* P2Itoa10(int value, char* str)
{
#if defined(_WIN32)
	return _itoa(value, str, 10);
#elif defined(__linux__) || defined(__MACH__)
	sprintf(str, "%d", value);
	return str;
#endif
}

inline char* P2I64toa16(INT64 value, char* str)
{
#if defined(_WIN32)
	return _i64toa(value, str, 16);
#elif defined(__linux__) || defined(__MACH__)
	sprintf(str, "%llx", value);
	return str;
#endif
}

#if defined(_WIN32)
#define P2StrToI64 _atoi64
#elif defined(__linux__) || defined (__MACH__) 
#define P2StrToI64 atoll
#endif // __linux__

inline INT64 P2AtoI64(const char * pStr, int radix)
{
#if defined(__linux__) || defined (__MACH__)
    return strtoll(pStr, NULL, radix);
#elif defined(_WIN32)
    return _strtoi64(pStr, NULL, radix);
#endif
}

inline UINT64 P2AtoUI64(const char * pStr, int radix)
{
#if defined(__linux__) || defined (__MACH__)
    return strtoull(pStr, NULL, radix);
#elif defined(_WIN32)
    return _strtoui64(pStr, NULL, radix);
#endif
}

inline size_t P2WcsLen(const P2WCHAR_T * pP2Wstr)
{
    const P2WCHAR_T *eos = pP2Wstr;
    while( *eos++ ) ;
    return static_cast< size_t >(eos - pP2Wstr - 1);
}

inline P2WCHAR_T * P2WcsCpy(P2WCHAR_T * dst, const P2WCHAR_T * src)
{
#if defined(__linux__) || defined (__MACH__)
    P2WCHAR_T * cp = dst;
    while( 0 != (*cp++ = *src++) )
        ;   /* Copy src over dst */
    return( dst );
#elif defined(_WIN32)
    return wcscpy(dst, src);
#endif
}


#if defined(_WIN32)
#pragma warning( pop )
#endif 

#endif // _P2STR_H_
