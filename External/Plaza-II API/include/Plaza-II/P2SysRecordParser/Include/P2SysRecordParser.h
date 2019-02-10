#ifndef _P2SYSRECORDPARSER_H_
#define _P2SYSRECORDPARSER_H_

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef P2SYS_EXPORTS
#define P2SYSRECORDPARSER_API P2_API_EXPORTS
#else
#define P2SYSRECORDPARSER_API P2_API_IMPORTS
#endif

#include "../../P2SysParser/Include/P2SysParser.h"
#include "../../P2SysRecord/Include/P2SysRecord.h"

P2SYSRECORDPARSER_API P2ERR P2CALLSPEC P2SysComposeFromRecord(P2SYS_COMPOSER_HANDLE composer, P2DB_RECORD_HANDLE rec);
P2SYSRECORDPARSER_API P2ERR P2CALLSPEC P2SysParseToRecord(P2SYS_PARSER_HANDLE parser, P2DB_RECORD_HANDLE rec);

// функции для помещения произвольного поля в композер 
P2SYSRECORDPARSER_API P2ERR P2CALLSPEC P2SysComposeField(P2SYS_COMPOSER_HANDLE pIComposer, P2DB_RECORD_HANDLE pIRec, size_t fieldIdx);
P2SYSRECORDPARSER_API P2ERR P2CALLSPEC P2SysComposeFieldEx(P2SYS_COMPOSER_HANDLE pIComposer, P2DB_RECORD_HANDLE pIRec, size_t fieldIdx, BOOL* isNull);

#ifdef  __cplusplus
}
#endif

#endif // _P2SYSRECORDPARSER_H_
