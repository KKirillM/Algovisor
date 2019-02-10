#ifndef _PRINTERS_H__INCLUDED_
#define _PRINTERS_H__INCLUDED_

#include <P2SysTypes.hpp>

typedef P2ERR (P2CALLSPEC *PRINTERFN)(char* buf, size_t* pBufSize, const char* p2type, UINT32 p2size, UINT32 p2prec, const void* pValue, size_t realSize, UINT32 opt);

P2ERR P2CALLSPEC printerBinary(char* buf, size_t* pBufSize, const char* p2type, UINT32 p2size, UINT32 p2prec, const void* pValue, size_t realSize, UINT32 opt);
P2ERR P2CALLSPEC printerW(char* buf, size_t* pBufSize, const char* p2type, UINT32 p2size, UINT32 p2prec, const void* pValue, size_t realSize, UINT32 opt);
P2ERR P2CALLSPEC printerC(char* buf, size_t* pBufSize, const char* p2type, UINT32 p2size, UINT32 p2prec, const void* pValue, size_t realSize, UINT32 opt);
P2ERR P2CALLSPEC printerC_replaceNonPrint(char* buf, size_t* pBufSize, const char*, UINT32, UINT32, const void* pValue, size_t, UINT32);
P2ERR P2CALLSPEC printerT(char* buf, size_t* pBufSize, const char* p2type, UINT32 p2size, UINT32 p2prec, const void* pValue, size_t realSize, UINT32 opt);
P2ERR P2CALLSPEC printerA(char* buf, size_t* pBufSize, const char* p2type, UINT32 p2size, UINT32 p2prec, const void* pValue, size_t realSize, UINT32 opt);
P2ERR P2CALLSPEC printerA_replaceNonPrint(char* buf, size_t* pBufSize, const char* p2type, UINT32 p2size, UINT32 p2prec, const void* pValue, size_t realSize, UINT32 opt);

P2ERR P2CALLSPEC printer_HR_flags(char* buf, size_t* pBufSize, const char*, UINT32, UINT32, const void* pValue, size_t, UINT32);
P2ERR P2CALLSPEC printer_HR_flags_64(char* buf, size_t* pBufSize, const char*, UINT32, UINT32, const void* pValue, size_t, UINT32);
P2ERR P2CALLSPEC printer_HR_clientId(char* buf, size_t*, const char* p2type, UINT32 p2size, UINT32 p2prec, const void* pValue, size_t realSize, UINT32);
P2ERR P2CALLSPEC printer_HR_default(char* buf, size_t* pBufSize, const char* p2type, UINT32 p2size, UINT32 p2prec, const void* pValue, size_t, UINT32);
P2ERR P2CALLSPEC printer_HR_64_hex_dec(char* buf, size_t* pBufSize, const char*, UINT32, UINT32, const void* pValue, size_t, UINT32);

P2ERR P2CALLSPEC printer_P2Time(char* buf, size_t* pBufSize, const char*, UINT32, UINT32, const void* pValue, size_t, UINT32);

#endif // _PRINTERS_H__INCLUDED_
