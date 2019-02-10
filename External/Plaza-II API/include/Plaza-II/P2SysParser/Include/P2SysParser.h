#ifndef _P2SYSPARSER_H_
#define _P2SYSPARSER_H_

#if defined(_MSC_VER) && defined(_DEBUG)
	#include <crtdbg.h>
#endif // defined(_MSC_VER) && defined(_DEBUG)

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef P2SYS_EXPORTS
#define P2SYSPARSER_API P2_API_EXPORTS
#else
#define P2SYSPARSER_API P2_API_IMPORTS
#endif

#include "../../P2Common/Include/P2Types.h"
#include "../../P2Common/Include/P2Errors.h"

typedef struct IP2SysComposer *P2SYS_COMPOSER_HANDLE;
typedef struct IP2SysParser *P2SYS_PARSER_HANDLE;

/*
   Аргументы для Compose():
   ------------------------
    i1      INT8
    i2      INT16
    i4      INT32
    i8      INT64
    u1      UINT8
    u2      UINT16
    u4      UINT32
    u8      UINT64
    cN      PCSTR
    bN      const void*, UINT32
    dN.M    const P2BCD(N)*
    sN.M    const P2BCD(N)*
    t       const P2TIME*

   Аргументы для Parse():
   ----------------------
    i1      INT8*
    i2      INT16*
    i4      INT32*
    i8      INT64*
    u1      UINT8*
    u2      UINT16*
    u4      UINT32*
    u8      UINT64*
    cN      PSTR
    bN      void*, [out]UINT32*
    dN.M    P2BCD(N)*
    sN.M    P2BCD(N)*
    t       P2TIME*
*/

P2SYSPARSER_API P2ERR P2CALLSPEC P2SysCreateBinaryComposer(P2SYS_COMPOSER_HANDLE *pComposer);
P2SYSPARSER_API P2ERR P2CALLSPEC P2SysCompose(P2SYS_COMPOSER_HANDLE composer, const char *format, ...);
P2SYSPARSER_API P2ERR P2CALLSPEC P2SysComposeAtIndex(P2SYS_COMPOSER_HANDLE composer, size_t index, const char *format, ...);
P2SYSPARSER_API P2ERR P2CALLSPEC P2SysComposeGetBuffer(P2SYS_COMPOSER_HANDLE composer, const void **buf);
P2SYSPARSER_API P2ERR P2CALLSPEC P2SysComposeGetSize(P2SYS_COMPOSER_HANDLE composer, size_t *size);
P2SYSPARSER_API P2ERR P2CALLSPEC P2SysFreeComposer(P2SYS_COMPOSER_HANDLE composer);
P2SYSPARSER_API P2ERR P2CALLSPEC P2SysResetComposer(P2SYS_COMPOSER_HANDLE composer);

P2SYSPARSER_API P2ERR P2CALLSPEC P2SysCreateBinaryParser(P2SYS_PARSER_HANDLE *pParser);
P2SYSPARSER_API P2ERR P2CALLSPEC P2SysInitParser(P2SYS_PARSER_HANDLE parser, const void *pData, size_t size);
P2SYSPARSER_API P2ERR P2CALLSPEC P2SysParse(P2SYS_PARSER_HANDLE parser, const char *format, ...);
P2SYSPARSER_API P2ERR P2CALLSPEC P2SysGetIndex(P2SYS_PARSER_HANDLE parser, size_t *pIdx);
P2SYSPARSER_API P2ERR P2CALLSPEC P2SysFreeParser(P2SYS_PARSER_HANDLE parser);

P2SYSPARSER_API P2ERR P2CALLSPEC P2SysGetNextType(PCSTR format, size_t* pTypeDescrLen, UINT32* pSize, UINT32* pPrec);

/*ф-ии аллокации модуля P2SysParser для обеспечения совместимости "внешних" парсероа-композеров таких как CFastBinComposer */
P2SYSPARSER_API void* P2CALLSPEC P2SysParserMemAlloc(size_t size);
P2SYSPARSER_API void* P2CALLSPEC P2SysParserMemRealloc(void* memblock,size_t size);
P2SYSPARSER_API void  P2CALLSPEC P2SysParserMemFree(void* memblock);

#ifdef  __cplusplus
}
#endif

#endif  // _P2SYSPARSER_H_
