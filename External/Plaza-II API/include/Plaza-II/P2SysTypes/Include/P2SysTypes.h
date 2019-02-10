#ifndef _P2SYSTYPES_H_
#define _P2SYSTYPES_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include "P2SysTypes_spec.h" 
#include "P2SysBCDII.h"

//////////////////////////////////////////////////////////////////////
// Time functions


#define P2TIME_PART_MSEC   0
#define P2TIME_PART_SEC    1
#define P2TIME_PART_MIN    2
#define P2TIME_PART_HOUR   3
#define P2TIME_PART_DAY    4
#define P2TIME_PART_MONTH  5 // for use only in P2Time.add

P2SYSTYPES_API P2ERR P2CALLSPEC P2TimeCheck(const P2TIME* t);
P2SYSTYPES_API int P2CALLSPEC P2TimeCmp(const P2TIME* a, const P2TIME* b);
P2SYSTYPES_API P2ERR P2CALLSPEC P2TimeGetFromStr(P2TIME* t, const char* s);
//standard time format: YYYY/MM/DD [HH:NN[:SS[.CC]]]

P2SYSTYPES_API void P2CALLSPEC P2TimeSetNULL(P2TIME* t);
P2SYSTYPES_API BOOL P2CALLSPEC P2TimeIsNULL(const P2TIME* t);

P2SYSTYPES_API void P2CALLSPEC P2TimeSetMin(P2TIME* t);
P2SYSTYPES_API void P2CALLSPEC P2TimeSetMax(P2TIME* t);
P2SYSTYPES_API void P2CALLSPEC P2TimeSetMinEx(P2TIME* t, int tag);
P2SYSTYPES_API void P2CALLSPEC P2TimeSetUnixEpoch(P2TIME* t);
#define P2TimeSetMinMSec(__a__) P2TimeSetMinEx(__a__, P2TIME_PART_MSEC)
#define P2TimeSetMinSec(__a__)  P2TimeSetMinEx(__a__, P2TIME_PART_SEC)
#define P2TimeSetMinMin(__a__)  P2TimeSetMinEx(__a__, P2TIME_PART_MIN)
#define P2TimeSetMinHour(__a__) P2TimeSetMinEx(__a__, P2TIME_PART_HOUR)
#define P2TimeSetMinDay(__a__)  P2TimeSetMinEx(__a__, P2TIME_PART_DAY)

P2SYSTYPES_API int P2CALLSPEC P2TimeCmpEx(const P2TIME* a, const P2TIME* b, int tag);
#define P2TimeCmpMSec(__a__, __b__) P2TimeCmpEx(__a__, __b__, P2TIME_PART_MSEC)
#define P2TimeCmpSec(__a__, __b__)  P2TimeCmpEx(__a__, __b__, P2TIME_PART_SEC)
#define P2TimeCmpMin(__a__, __b__)  P2TimeCmpEx(__a__, __b__, P2TIME_PART_MIN)
#define P2TimeCmpHour(__a__, __b__) P2TimeCmpEx(__a__, __b__, P2TIME_PART_HOUR)
#define P2TimeCmpDay(__a__, __b__)  P2TimeCmpEx(__a__, __b__, P2TIME_PART_DAY)

P2SYSTYPES_API void P2CALLSPEC P2TimeAddEx(P2TIME* t, int tag, long long value);
#define P2TimeAddMSec(__t__, __val__) P2TimeAddEx(__t__, P2TIME_PART_MSEC, __val__)
#define P2TimeAddSec(__t__, __val__)  P2TimeAddEx(__t__, P2TIME_PART_SEC,  __val__)
#define P2TimeAddMin(__t__, __val__)  P2TimeAddEx(__t__, P2TIME_PART_MIN,  __val__)
#define P2TimeAddHour(__t__, __val__) P2TimeAddEx(__t__, P2TIME_PART_HOUR, __val__)
#define P2TimeAddDay(__t__, __val__)  P2TimeAddEx(__t__, P2TIME_PART_DAY,  __val__)

P2SYSTYPES_API int P2CALLSPEC P2TimeDiffCmpEx(const P2TIME* a, const P2TIME* b, int tag, int value);
#define P2TimeDiffCmpMSec(__a__, __b__, __val__) P2TimeDiffCmpEx(__a__, __b__, P2TIME_PART_MSEC, __val__)
#define P2TimeDiffCmpSec(__a__, __b__, __val__)  P2TimeDiffCmpEx(__a__, __b__, P2TIME_PART_SEC,  __val__)
#define P2TimeDiffCmpMin(__a__, __b__, __val__)  P2TimeDiffCmpEx(__a__, __b__, P2TIME_PART_MIN,  __val__)
#define P2TimeDiffCmpHour(__a__, __b__, __val__) P2TimeDiffCmpEx(__a__, __b__, P2TIME_PART_HOUR, __val__)
#define P2TimeDiffCmpDay(__a__, __b__, __val__)  P2TimeDiffCmpEx(__a__, __b__, P2TIME_PART_DAY,  __val__)

P2SYSTYPES_API int P2CALLSPEC P2TimeDiffEx(const P2TIME* a, const P2TIME* b, int tag);
#define P2TimeDiffMSec(__a__, __b__) P2TimeDiffEx(__a__, __b__, P2TIME_PART_MSEC)
#define P2TimeDiffSec(__a__, __b__)  P2TimeDiffEx(__a__, __b__, P2TIME_PART_SEC)
#define P2TimeDiffMin(__a__, __b__)  P2TimeDiffEx(__a__, __b__, P2TIME_PART_MIN)
#define P2TimeDiffHour(__a__, __b__) P2TimeDiffEx(__a__, __b__, P2TIME_PART_HOUR)
#define P2TimeDiffDay(__a__, __b__)  P2TimeDiffEx(__a__, __b__, P2TIME_PART_DAY)

P2SYSTYPES_API int P2CALLSPEC P2TimeGetWDay(const P2TIME* t);
P2SYSTYPES_API int P2CALLSPEC P2TimeGetDaysInYear(const int year);

P2SYSTYPES_API void P2CALLSPEC P2TimeGetCurrent(P2TIME* t);

//////////////////////////////////////////////////////////////////////

// Тут когда-то было старое БЦД, убито

//////////////////////////////////////////////////////////////////////

P2SYSTYPES_API P2ERR P2CALLSPEC P2SysTypeGetAlign(char p2type, size_t size, size_t* pAlign);
P2SYSTYPES_API P2ERR P2CALLSPEC P2SysTypeGetSizeAndAlign(char p2type, UINT32 size, UINT32 prec,
                                                         size_t* pSize, size_t* pAlign);

//////////////////////////////////////////////////////////////////////

P2SYSTYPES_API int P2CALLSPEC P2SysSNPrintf(char* zBuf, int n, const char* zFormat, ...);
P2SYSTYPES_API int P2CALLSPEC P2SysVSNPrintf(char* zBuf, int n, const char* zFormat, va_list ap);

P2SYSTYPES_API P2ERR P2CALLSPEC P2SysFreePrintfMemory(char* pMem);
P2SYSTYPES_API int P2CALLSPEC P2SysMPrintf(char** ppMem, const char* zFormat, ...);
P2SYSTYPES_API int P2CALLSPEC P2SysVMPrintf(char** ppMem, const char* zFormat, va_list ap);

P2SYSTYPES_API int P2CALLSPEC P2SysPrintf(const char* zFormat, ...);
P2SYSTYPES_API int P2CALLSPEC P2SysVPrintf(const char* zFormat, va_list ap);

//////////////////////////////////////////////////////////////////////

P2SYSTYPES_API PCSTR P2CALLSPEC P2ErrGetDescrStr(P2ERR err);

//////////////////////////////////////////////////////////////////////

#define P2TYPE_TO_STR_BINARY_FOR_SQLITE    0x0001

P2SYSTYPES_API P2ERR P2CALLSPEC P2TypeToStr(char* buf, size_t* pBufSize, const char* p2type, UINT32 p2size, UINT32 p2prec, const void* pValue, size_t realSize, UINT32 opt);

P2SYSTYPES_API P2ERR P2CALLSPEC P2StrToP2Type(void *outBuf, size_t *pOutSize, BOOL* isNULL, const char *type, UINT32 typeSize, UINT32 prec, const char* inBuf, UINT32 opt);

#ifdef  __cplusplus
}
#endif

#endif // _P2SYSTYPES_H_
