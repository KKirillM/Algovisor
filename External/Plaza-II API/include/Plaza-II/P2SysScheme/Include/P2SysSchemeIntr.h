#ifndef _P2SYSSCHEMEINTR_H_
#define _P2SYSSCHEMEINTR_H_

#include "../../P2Common/Include/P2Types.h"
#include "../../P2SysScheme/Include/P2SysScheme.h"

#ifdef __cplusplus
extern "C"{
#endif

#ifndef P2SCHEME_API
#ifdef P2SYSSCHEME_EXPORTS
#define P2SCHEME_API P2SYS_API_EXPORTS
#else
#define P2SCHEME_API P2SYS_API_IMPORTS
#endif
#endif // #ifndef P2SCHEME_API

// internal functions for use from P2DB
P2SCHEME_API void CheckOpenedSchemeHandles(void);
typedef P2ERR (P2CALLSPEC *pfnSaveSchemeEntry)(void *cbParams, UINT32 id, const char *section, const char *key, const char *value);
P2SCHEME_API P2ERR CbkSaveToDb(P2DB_SCHEME_HANDLE sch, void *cbParams, pfnSaveSchemeEntry fnCbk);
P2SCHEME_API P2DB_SCHEME_HANDLE IntCreateScheme(const char *schemeName);
P2SCHEME_API P2DB_TABLE_SCHEME_HANDLE IntCreateTableScheme(const char *tableName, UINT32 type);
P2SCHEME_API P2ERR IntAddTable(P2DB_SCHEME_HANDLE sch, P2DB_TABLE_SCHEME_HANDLE tblSch);
P2SCHEME_API P2ERR IntCheckScheme(P2DB_SCHEME_HANDLE sch);
P2SCHEME_API P2ERR IntAddTextCbk(const char* /*section*/, const char* key, const char* value, void* pTbl);
P2SCHEME_API void DumpTableScheme(P2DB_TABLE_SCHEME_HANDLE sch);
P2SCHEME_API P2ERR P2DBGetRecordsetCreationSQL(P2DB_TABLE_SCHEME_HANDLE pScheme, char * buf, size_t *psize, P2DB_CONN_HANDLE sch);

//потоконебезопасная ф-ция, можно использовать только для временных схем
P2SCHEME_API P2ERR P2SchemeSetCustomField(P2DB_TABLE_SCHEME_HANDLE tblSch, const char *name, const char * buf);

#ifdef __cplusplus
};
#endif

#endif // _P2SYSSCHEMEINTR_H_
