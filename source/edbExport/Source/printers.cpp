#include "stdafx.h"
#include "./printers.h"
#include "p2time.h"

P2ERR P2CALLSPEC printerBinary(char* buf, size_t* pBufSize, const char* p2type, UINT32 p2size, UINT32 p2prec, const void* pValue, size_t realSize, UINT32)
{
    return P2TypeToStr(buf, pBufSize, p2type, p2size, p2prec, pValue, realSize, P2TYPE_TO_STR_BINARY_FOR_SQLITE);
}

P2ERR P2CALLSPEC printerW(char* buf, size_t* pBufSize, const char*, UINT32, UINT32, const void* pValue, size_t, UINT32)
{
    size_t wstrBytes = (wcslen(static_cast< const wchar_t* >(pValue)) + 1) * sizeof(wchar_t);
    return P2TypeToStr(buf, pBufSize, "b", 0, 0, pValue, wstrBytes, P2TYPE_TO_STR_BINARY_FOR_SQLITE);
}

P2ERR P2CALLSPEC printerC(char* buf, size_t* pBufSize, const char*, UINT32, UINT32, const void* pValue, size_t, UINT32)
{
    char * origBuf = buf;
    const char * pStr = static_cast< const char * >(pValue);
    while (*pStr)
    {
        *buf++ = *pStr;
        if ('"' == *pStr++) // удваиваем кавычки
        {
            *buf++ = '"';
        }
    }
    *buf++ = 0;
    if (pBufSize)
    {
        *pBufSize = buf - origBuf;
    }
    return P2ERR_OK;
}

P2ERR P2CALLSPEC printerC_replaceNonPrint(char* buf, size_t* pBufSize, const char*, UINT32, UINT32, const void* pValue, size_t, UINT32)
{
    char * origBuf = buf;
    const char * pStr = static_cast< const char * >(pValue);
    while (*pStr)
    {
        if (*pStr < ' ' || ',' == *pStr) { // непечатные и запятую заменяем на пробелы
            *buf++ = ' ';
            pStr++;
        }
        else {
            *buf++ = *pStr;
            if ('"' == *pStr++) // удваиваем кавычки
            {
                *buf++ = '"';
            }
        }
    }
    *buf++ = 0;
    if (pBufSize)
    {
        *pBufSize = buf - origBuf;
    }
    return P2ERR_OK;
}


P2ERR P2CALLSPEC printerT(char* buf, size_t* pBufSize, const char*, UINT32, UINT32, const void* pValue, size_t, UINT32)
{
    if (!pBufSize)
    {
        return P2ERR_COMMON_WRONG_ARGUMENTS;
    }

    const P2TIME * pt = static_cast< const P2TIME * >(pValue);
    int nPrinted;
    if (-1 == (nPrinted = P2SysSNPrintf(buf, static_cast< int >(*pBufSize), "%04u%02u%02u %02u:%02u:%02u.%03u",
        pt->year, pt->month, pt->day, pt->hour, pt->minute, pt->second, pt->msec)))
    {
        return P2ERR_COMMON_INTERNAL_ERROR;
    }
    *pBufSize = static_cast< size_t >(nPrinted) + 1;

    return P2ERR_OK;
}

P2ERR P2CALLSPEC printerA(char* buf, size_t* pBufSize, const char*, UINT32, UINT32, const void* pValue, size_t, UINT32)
{
    size_t outLen = 2;
    const char * pChar = static_cast< const char * >(pValue);
    *buf++ = *pChar;
    if ('"' == *pChar) // удваиваем кавычки
    {
        *buf++ = '"';
        ++outLen;
    }
    *buf = 0;
    if (pBufSize)
    {
        *pBufSize = outLen;
    }
    return P2ERR_OK;
}

P2ERR P2CALLSPEC printerA_replaceNonPrint(char* buf, size_t* pBufSize, const char*, UINT32, UINT32, const void* pValue, size_t, UINT32)
{
    size_t outLen = 2;
    const char * pChar = static_cast< const char * >(pValue);
    if (*pChar < ' ' || ',' == *pChar) { // непечатные и запятую заменяем на пробелы
        *buf++ = ' ';
    }
    else {
        *buf++ = *pChar;
        if ('"' == *pChar) // удваиваем кавычки
        {
            *buf++ = '"';
            ++outLen;
        }
    }
    *buf = 0;
    if (pBufSize)
    {
        *pBufSize = outLen;
    }
    return P2ERR_OK;
}

P2ERR P2CALLSPEC printer_HR_flags(char* buf, size_t* pBufSize, const char*, UINT32, UINT32, const void* pValue, size_t, UINT32)
{
    return (-1 == P2SysSNPrintf(buf, static_cast< int >(*pBufSize), "0x%08X", *static_cast< const UINT32 * >(pValue)) ? P2ERR_COMMON_INTERNAL_ERROR : P2ERR_OK);
}

P2ERR P2CALLSPEC printer_HR_flags_64(char* buf, size_t* pBufSize, const char*, UINT32, UINT32, const void* pValue, size_t, UINT32)
{
    return (-1 == P2SysSNPrintf(buf, static_cast< int >(*pBufSize), "0x%016llX", *static_cast< const UINT64 * >(pValue)) ? P2ERR_COMMON_INTERNAL_ERROR : P2ERR_OK);
}

P2ERR P2CALLSPEC printer_HR_clientId(char* buf, size_t*, const char*, UINT32, UINT32, const void* pValue, size_t realSize, UINT32)
{
    strncpy(buf, static_cast< const char * >(pValue), realSize);
    return P2ERR_OK;
}

P2ERR P2CALLSPEC printer_HR_default(char* buf, size_t* pBufSize, const char* p2type, UINT32 p2size, UINT32 p2prec, const void* pValue, size_t realSize, UINT32)
{
    if (P2FAILED(P2TypeToStr(buf, pBufSize, p2type, p2size, p2prec, pValue, realSize, 0)))
    {
        strcpy(buf, "!!ERROR!! ");
        P2TypeToStr(buf + 10, pBufSize, "b", p2size, 0, pValue, realSize, 0);
    }
    return P2ERR_OK;
}

P2ERR P2CALLSPEC printer_HR_64_hex_dec(char* buf, size_t* pBufSize, const char*, UINT32, UINT32, const void* pValue, size_t, UINT32)
{
    return (-1 == P2SysSNPrintf(buf, static_cast< int >(*pBufSize), "0x%016llX (%lld)",
        *static_cast< const UINT64 * >(pValue), *static_cast< const UINT64 * >(pValue)) ? P2ERR_COMMON_INTERNAL_ERROR : P2ERR_OK);
}

P2ERR P2CALLSPEC printer_P2Time(char* buf, size_t* pBufSize, const char*, UINT32, UINT32, const void* pValue, size_t, UINT32)
{
    if (!pBufSize)
    {
        return P2ERR_COMMON_WRONG_ARGUMENTS;
    }

	fo2base::CP2Time time; 
	time = fo2base::CP2Time::FromInt64(*(uint64_t*)(pValue));
	int nPrinted = sprintf_s(buf, *pBufSize, time.ToString("%d-%m-%Y %H:%M:%S.%Z").c_str());

	*pBufSize = static_cast< size_t >(nPrinted) + 1;

	return P2ERR_OK;
}
