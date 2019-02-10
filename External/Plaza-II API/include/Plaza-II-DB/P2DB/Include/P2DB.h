#ifndef _P2DB_H_
#define _P2DB_H_

#include "../../../Plaza-II/P2SysMsg/Include/P2SysMsg.h"
#include "../../../Plaza-II/P2SysScheme/Include/P2SysScheme.h"
#include "../../../Plaza-II/P2SysRecord/Include/P2SysRecord.h"
#include "../../../Plaza-II/P2SysIni/Include/P2SysIni.h"

#ifdef __cplusplus
extern "C"{
#endif

#ifdef P2DB_EXPORTS
#define P2DB_API P2_API_EXPORTS
#else
#define P2DB_API P2_API_IMPORTS
#endif //P2DB_EXPORTS

#define P2DB_MAX_SQL_STATEMENT_LENGTH   4096
#define P2DB_MAX_SQL_HINT_LENGTH        255

// композить без ограничения
#define P2DB_COMPOSED_SIZE_WITHOUT_LIMIT 0xFFFFFFFF

#define P2DB_STARTUP_INI_SECTION        "p2db:execute_on_startup"
#define P2DB_STARTUP_INI_SECTION_SHORT  "execute_on_startup"
#define P2DB_HINTS_SECTION              "p2db:hints"
#define P2DB_LOG_CATEGORY               "p2db"
#define P2DB_THRESHOLDS_SECTION         "p2db:options"

// описание хэндлов объектов базы 
struct P2DBTable;
typedef P2DBTable*          P2DB_TABLE_HANDLE;
struct P2DBClausePart;
typedef P2DBClausePart P2DBWhere;
typedef P2DBWhere*          P2DB_WHERE_HANDLE;
typedef P2DBClausePart P2DBOrderBy;
typedef P2DBOrderBy*        P2DB_ORDER_BY_HANDLE;
typedef P2DBClausePart P2DBSet;
typedef P2DBSet*            P2DB_SET_HANDLE;

//struct P2DBRecord;
//typedef P2DBRecord*         P2DB_RECORD_HANDLE;
struct P2DBRecordset;
typedef P2DBRecordset*      P2DB_RECORDSET_HANDLE;

struct P2DBHints;
typedef P2DBHints*          P2DB_HINTS_HANDLE;

class P2DBThresholds;
typedef P2DBThresholds*     P2DB_THRESHOLDS_HANDLE;

typedef P2ERR (P2CALLSPEC *pfnRecordProc)(P2DB_RECORD_HANDLE rec);

typedef P2ERR (P2CALLSPEC *pfnRecordProcWithParam)(P2DB_RECORD_HANDLE rec, void* cbParam);

typedef P2ERR (P2CALLSPEC *pfnRecordsetEnumColInfoProc)(const P2DBFieldInfo* pfi, void* cbParam);

//	общие
P2DB_API P2ERR P2CALLSPEC P2DBStartup();
P2DB_API void P2CALLSPEC P2DBCleanup();

// соединение с базой
P2DB_API P2ERR P2CALLSPEC P2DBCreateConnection(const char *dllName, P2DB_CONN_HANDLE *pConn);
P2DB_API P2ERR P2CALLSPEC P2DBConnect(P2DB_CONN_HANDLE conn, P2DB_SCHEME_HANDLE sch, const char *parameters);
// open = create + connect. 
P2DB_API P2ERR P2CALLSPEC P2DBOpenConnection(P2DB_SCHEME_HANDLE sch, const char *params, P2DB_CONN_HANDLE *pconn);
P2DB_API P2ERR P2CALLSPEC P2DBDuplicateConnectionHandle(P2DB_CONN_HANDLE src, P2DB_CONN_HANDLE *pDest);

P2DB_API P2ERR P2CALLSPEC P2DBDisconnect(P2DB_CONN_HANDLE conn);
P2DB_API P2ERR P2CALLSPEC P2DBFreeConnection(P2DB_CONN_HANDLE conn);
// close = disconnect + free
P2DB_API P2ERR P2CALLSPEC P2DBCloseConnection(P2DB_CONN_HANDLE conn);

P2DB_API P2ERR P2CALLSPEC P2DBGetSchemeFromConnection(P2DB_CONN_HANDLE conn, P2DB_SCHEME_HANDLE *sch);


// транзакции
P2DB_API P2ERR P2CALLSPEC P2DBBeginTransaction(P2DB_CONN_HANDLE conn);
P2DB_API P2ERR P2CALLSPEC P2DBCommit(P2DB_CONN_HANDLE conn);
P2DB_API P2ERR P2CALLSPEC P2DBRollback(P2DB_CONN_HANDLE conn);

// таблицы
P2DB_API P2ERR P2CALLSPEC P2DBCreateTable(P2DB_CONN_HANDLE conn, P2DB_TABLE_SCHEME_HANDLE tbl_sch);
P2DB_API P2ERR P2CALLSPEC P2DBDeleteTable(P2DB_CONN_HANDLE conn, P2DB_TABLE_SCHEME_HANDLE tbl_sch);
P2DB_API P2ERR P2CALLSPEC P2DBOpenTable(P2DB_CONN_HANDLE conn, P2DB_TABLE_SCHEME_HANDLE tbl_sch, P2DB_TABLE_HANDLE* ptbl);
P2DB_API P2ERR P2CALLSPEC P2DBOpenTableByName(P2DB_CONN_HANDLE conn, const char *tableName, P2DB_TABLE_HANDLE* ptbl);
P2DB_API P2ERR P2CALLSPEC P2DBOpenTableByIndex(P2DB_CONN_HANDLE conn, size_t idx, P2DB_TABLE_HANDLE* ptbl);
P2DB_API P2ERR P2CALLSPEC P2DBTruncateTable(P2DB_TABLE_HANDLE tbl);
P2DB_API P2ERR P2CALLSPEC P2DBCloseTable(P2DB_TABLE_HANDLE tbl);
P2DB_API P2ERR P2CALLSPEC P2DBGetSchemeFromTable(P2DB_TABLE_HANDLE rs, P2DB_TABLE_SCHEME_HANDLE *ptblSch);
P2DB_API P2ERR P2CALLSPEC P2DBCheckTableExistence(P2DB_CONN_HANDLE conn, const char* tableName, BOOL* exists);

// компиляторы для частей WHERE/ORDER BY/SET
P2DB_API P2ERR P2CALLSPEC P2DBCreateWhere(const char *str, P2DB_CONN_HANDLE conn, P2DB_WHERE_HANDLE *pwhere);
P2DB_API P2ERR P2CALLSPEC P2DBFreeWhere(P2DB_WHERE_HANDLE where);

P2DB_API P2ERR P2CALLSPEC P2DBCreateOrderBy(const char *str, P2DB_CONN_HANDLE conn, P2DB_ORDER_BY_HANDLE *porder);
P2DB_API P2ERR P2CALLSPEC P2DBFreeOrderBy(P2DB_ORDER_BY_HANDLE order);

P2DB_API P2ERR P2CALLSPEC P2DBCreateSet(const char *str, P2DB_CONN_HANDLE conn, P2DB_SET_HANDLE *pset);
P2DB_API P2ERR P2CALLSPEC P2DBFreeSet(P2DB_SET_HANDLE set);


// групповые операции
P2DB_API P2ERR P2CALLSPEC P2DBDeleteRecords(P2DB_TABLE_HANDLE tbl, P2DB_WHERE_HANDLE where);
P2DB_API P2ERR P2CALLSPEC P2DBUpdateRecords(P2DB_TABLE_HANDLE tbl, P2DB_SET_HANDLE set, P2DB_WHERE_HANDLE where);
P2DB_API P2ERR P2CALLSPEC P2DBExecute(P2DB_CONN_HANDLE conn, const char *sql);
P2DB_API P2ERR P2CALLSPEC P2DBExecuteIniSection(P2DB_CONN_HANDLE conn, P2INI_HANDLE ini, const char *sectionName, BOOL continueOnErrors);

//получение рекордсета
P2DB_API P2ERR P2CALLSPEC P2DBTableToRecordset(P2DB_TABLE_HANDLE tbl, P2DB_WHERE_HANDLE where, P2DB_ORDER_BY_HANDLE order, P2DB_RECORDSET_HANDLE *prs);
P2DB_API P2ERR P2CALLSPEC P2DBJoinedTablesToRecordset(P2DB_CONN_HANDLE conn, P2DB_TABLE_HANDLE *tbls, int numTables, P2DB_WHERE_HANDLE where, 
								  P2DB_ORDER_BY_HANDLE order, P2DB_RECORDSET_HANDLE * prs);
P2DB_API P2ERR P2CALLSPEC P2DBSQLStatementToRecordset(P2DB_CONN_HANDLE conn, const char * sql, P2DB_RECORDSET_HANDLE * rs, P2DB_TABLE_SCHEME_HANDLE tblSch);
P2DB_API P2ERR P2CALLSPEC P2DBGetSchemeFromRecordset(P2DB_RECORDSET_HANDLE rs, P2DB_TABLE_SCHEME_HANDLE *ptblSch);
P2DB_API P2ERR P2CALLSPEC P2DBGetSchemeFromRecord(P2DB_RECORD_HANDLE rs, P2DB_TABLE_SCHEME_HANDLE *ptblSch);
P2DB_API P2ERR P2CALLSPEC P2DBFreeRecordset(P2DB_RECORDSET_HANDLE rs);
P2DB_API P2ERR P2CALLSPEC P2DBRecordsetEnumColumnsInfo(P2DB_RECORDSET_HANDLE rs, pfnRecordsetEnumColInfoProc cb, void* cbParam);

// prepare функции
P2DB_API P2ERR P2CALLSPEC P2DBPrepareRecordForUpdate(P2DB_RECORD_HANDLE rec);
P2DB_API P2ERR P2CALLSPEC P2DBPrepareRecordForUpdateEx(P2DB_RECORD_HANDLE rec, size_t* whereIndexes, size_t cnt);
P2DB_API P2ERR P2CALLSPEC P2DBPrepareRecordForInsert(P2DB_RECORD_HANDLE rec);

// получить следующую запись (в том числе и первую)
P2DB_API P2ERR P2CALLSPEC P2DBRecordsetFetchNext(P2DB_RECORDSET_HANDLE rs);


P2DB_API P2ERR P2CALLSPEC P2DBGetFieldsCount(P2DB_RECORD_HANDLE rs, size_t *pCount);

P2DB_API P2ERR P2CALLSPEC P2DBGetField(P2DB_RECORD_HANDLE rec, const char *name, void *outBuf, size_t *pSize);

P2DB_API P2ERR P2CALLSPEC P2DBGetFieldINT8(P2DB_RECORD_HANDLE rec, const char *name, INT8 *val);
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldINT16(P2DB_RECORD_HANDLE rec, const char *name, INT16 *val);
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldINT32(P2DB_RECORD_HANDLE rec, const char *name, INT32 *val);
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldINT64(P2DB_RECORD_HANDLE rec, const char *name, INT64 *val);
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldUINT8(P2DB_RECORD_HANDLE rec, const char *name, UINT8 *val);
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldUINT16(P2DB_RECORD_HANDLE rec, const char *name, UINT16 *val);
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldUINT32(P2DB_RECORD_HANDLE rec, const char *name, UINT32 *val);
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldUINT64(P2DB_RECORD_HANDLE rec, const char *name, UINT64 *val);
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldP2TIME(P2DB_RECORD_HANDLE rec, const char *name, P2TIME *val);

P2DB_API P2ERR P2CALLSPEC P2DBGetFieldByIndex(P2DB_RECORD_HANDLE rec, size_t idx, void *out_buf, size_t *pSize);
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldByIndexINT8(P2DB_RECORD_HANDLE rec, size_t idx, INT8 *val);
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldByIndexINT16(P2DB_RECORD_HANDLE rec, size_t idx, INT16 *val);
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldByIndexINT32(P2DB_RECORD_HANDLE rec, size_t idx, INT32 *val);
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldByIndexINT64(P2DB_RECORD_HANDLE rec, size_t idx, INT64 *val);
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldByIndexUINT8(P2DB_RECORD_HANDLE rec, size_t idx, UINT8 *val);
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldByIndexUINT16(P2DB_RECORD_HANDLE rec, size_t idx, UINT16 *val);
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldByIndexUINT32(P2DB_RECORD_HANDLE rec, size_t idx, UINT32 *val);
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldByIndexUINT64(P2DB_RECORD_HANDLE rec, size_t idx, UINT64 *val);
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldByIndexP2TIME(P2DB_RECORD_HANDLE rec, size_t idx, P2TIME *val);

P2DB_API P2ERR P2CALLSPEC P2DBGetFieldByIndexWithoutScheme(P2DB_RECORD_HANDLE rs, size_t idx, void *out_buf, size_t *plen, const char *p2type, UINT32 p2size, UINT32 p2prec);
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldByIndexWithoutSchemeINT8(P2DB_RECORD_HANDLE rec, size_t idx, INT8 *val);
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldByIndexWithoutSchemeINT16(P2DB_RECORD_HANDLE rec, size_t idx, INT16 *val);
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldByIndexWithoutSchemeINT32(P2DB_RECORD_HANDLE rec, size_t idx, INT32 *val);
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldByIndexWithoutSchemeINT64(P2DB_RECORD_HANDLE rec, size_t idx, INT64 *val);
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldByIndexWithoutSchemeUINT8(P2DB_RECORD_HANDLE rec, size_t idx, UINT8 *val);
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldByIndexWithoutSchemeUINT16(P2DB_RECORD_HANDLE rec, size_t idx, UINT16 *val);
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldByIndexWithoutSchemeUINT32(P2DB_RECORD_HANDLE rec, size_t idx, UINT32 *val);
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldByIndexWithoutSchemeUINT64(P2DB_RECORD_HANDLE rec, size_t idx, UINT64 *val);
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldByIndexWithoutSchemeP2TIME(P2DB_RECORD_HANDLE rec, size_t idx, P2TIME *val);

P2DB_API P2ERR P2CALLSPEC P2DBSetField(P2DB_RECORD_HANDLE rs, const char *name, const void *buf, size_t len);
/*#define P2DBSetFieldTypeVal__(rec, name, val) P2DBSetField(rec, name, &(val), sizeof(val))
#define P2DBSetFieldTypePtr__(rec, name, pVal) P2DBSetField(rec, name, pVal, sizeof(*(pVal)))
#define P2DBSetFieldINT8(rec, name, val) P2DBSetFieldTypeVal__(rec, name, (INT8)(val))
#define P2DBSetFieldINT16(rec, name, val) P2DBSetFieldTypeVal__(rec, name, (INT16)(val))
#define P2DBSetFieldINT32(rec, name, val) P2DBSetFieldTypeVal__(rec, name, (INT32)(val))
#define P2DBSetFieldINT64(rec, name, val) P2DBSetFieldTypeVal__(rec, name, (INT64)(val))
#define P2DBSetFieldUINT8(rec, name, val) P2DBSetFieldTypeVal__(rec, name, (UINT8)(val))
#define P2DBSetFieldUINT16(rec, name, val) P2DBSetFieldTypeVal__(rec, name, (UINT16)(val))
#define P2DBSetFieldUINT32(rec, name, val) P2DBSetFieldTypeVal__(rec, name, (UINT32)(val))
#define P2DBSetFieldUINT64(rec, name, val) P2DBSetFieldTypeVal__(rec, name, (UINT64)(val))
#define P2DBSetFieldP2TIME(rec, name, pVal) P2DBSetFieldTypePtr__(rec, name, (const P2TIME*)(pVal))*/

P2DB_API P2ERR P2CALLSPEC P2DBSetFieldByIndex(P2DB_RECORD_HANDLE rs, size_t idx, const void *buf, size_t len);
/*#define P2DBSetFieldByIndexTypeVal__(rec, idx, val) P2DBSetFieldByIndex(rec, idx, &(val), sizeof(val))
#define P2DBSetFieldByIndexTypePtr__(rec, idx, pVal) P2DBSetFieldByIndex(rec, idx, pVal, sizeof(*(pVal)))
#define P2DBSetFieldByIndexINT8(rec, idx, val) P2DBSetFieldByIndexTypeVal__(rec, idx, (INT8)(val))
#define P2DBSetFieldByIndexINT16(rec, idx, val) P2DBSetFieldByIndexTypeVal__(rec, idx, (INT16)(val))
#define P2DBSetFieldByIndexINT32(rec, idx, val) P2DBSetFieldByIndexTypeVal__(rec, idx, (INT32)(val))
#define P2DBSetFieldByIndexINT64(rec, idx, val) P2DBSetFieldByIndexTypeVal__(rec, idx, (INT64)(val))
#define P2DBSetFieldByIndexUINT8(rec, idx, val) P2DBSetFieldByIndexTypeVal__(rec, idx, (UINT8)(val))
#define P2DBSetFieldByIndexUINT16(rec, idx, val) P2DBSetFieldByIndexTypeVal__(rec, idx, (UINT16)(val))
#define P2DBSetFieldByIndexUINT32(rec, idx, val) P2DBSetFieldByIndexTypeVal__(rec, idx, (UINT32)(val))
#define P2DBSetFieldByIndexUINT64(rec, idx, val) P2DBSetFieldByIndexTypeVal__(rec, idx, (UINT64)(val))
#define P2DBSetFieldByIndexP2TIME(rec, idx, pVal) P2DBSetFieldByIndexTypePtr__(rec, idx, (const P2TIME*)(pVal))*/


P2DB_API P2ERR P2CALLSPEC P2DBCopyToStruct(P2DB_RECORD_HANDLE rs, void *out_buf, size_t *plen);
P2DB_API P2ERR P2CALLSPEC P2DBCopyFromStruct(P2DB_RECORD_HANDLE rs, const void *out_buf, size_t len);

P2DB_API P2ERR P2CALLSPEC P2DBCopyRecord(P2DB_RECORD_HANDLE dstRec, P2DB_RECORD_HANDLE srcRec);

P2DB_API P2ERR P2CALLSPEC P2DBFreeRecord(P2DB_RECORD_HANDLE rec);

P2DB_API P2ERR P2CALLSPEC P2DBUpdateRecord(P2DB_RECORD_HANDLE rec);
P2DB_API P2ERR P2CALLSPEC P2DBInsertRecord(P2DB_RECORD_HANDLE rec);
P2DB_API P2ERR P2CALLSPEC P2DBDeleteRecord(P2DB_RECORD_HANDLE rec);

P2DB_API P2ERR P2CALLSPEC P2DBNewRecord(P2DB_TABLE_HANDLE tbl, P2DB_RECORD_HANDLE *prs);
P2DB_API P2ERR P2CALLSPEC P2DBGetRecordForUpdate(P2DB_RECORD_HANDLE *prs, P2DB_TABLE_HANDLE tbl, P2DB_WHERE_HANDLE where);


P2DB_API P2ERR P2CALLSPEC P2DBSQLStatement2Enum(P2DB_CONN_HANDLE conn, const char *sql, pfnRecordProc proc);
P2DB_API P2ERR P2CALLSPEC P2DBGetRecordFromRecordset(P2DB_RECORDSET_HANDLE, P2DB_RECORD_HANDLE *rs);

////////////////////////////////////////////////////////////////////////////////
P2DB_API P2ERR P2CALLSPEC P2DBCreateSchemeFromDatabase(P2DB_SCHEME_HANDLE *pScheme, const char *connection, const char *tableName, const char *schemeName);
P2DB_API P2ERR P2CALLSPEC P2DBCreateSchemeFromDatabase2(P2DB_SCHEME_HANDLE *pScheme, P2DB_CONN_HANDLE db, const char *tableName, const char *schemeName);
P2DB_API P2ERR P2CALLSPEC P2DBSaveSchemeToDatabase(P2DB_SCHEME_HANDLE sch, const char *connection, const char *tableName);
P2DB_API P2ERR P2CALLSPEC P2DBSaveSchemeToDatabase2(P2DB_SCHEME_HANDLE sch, P2DB_CONN_HANDLE conn, const char *tableName);

P2DB_API P2ERR P2CALLSPEC P2DBSaveSchemeToDatabaseEx(P2DB_SCHEME_HANDLE sch, const char *connection, const char *tableName, const char* name);
P2DB_API P2ERR P2CALLSPEC P2DBSaveSchemeToDatabaseEx2(P2DB_SCHEME_HANDLE sch, P2DB_CONN_HANDLE conn, const char *tableName, const char* name);
P2DB_API P2ERR P2CALLSPEC P2DBDeleteSchemeFromDatabase(const char *connection, const char *tableName, const char* name);
P2DB_API P2ERR P2CALLSPEC P2DBDeleteSchemeFromDatabase2(P2DB_CONN_HANDLE conn, const char *tableName, const char* name);
P2DB_API P2ERR P2CALLSPEC P2DBDeleteSchemeFromDatabase3(P2DB_CONN_HANDLE conn, const char *tableName, const char* name);

P2DB_API P2ERR P2CALLSPEC P2DBCreateRecordsetWithRecordsetScheme(P2DB_RECORDSET_HANDLE * prs, P2DB_TABLE_SCHEME_HANDLE pRecScheme, P2DB_CONN_HANDLE conn, P2DB_WHERE_HANDLE where, P2DB_ORDER_BY_HANDLE orderby);


P2DB_API P2ERR P2CALLSPEC P2DBParseToTable(P2DB_TABLE_HANDLE tbl, P2_ParseState *pState);
////////////////////////////////////////////////////////////////////////////////
typedef P2ERR (P2CALLSPEC *P2_DB_GETNEWRECORD_CALLBACK)(P2DB_RECORD_HANDLE *pRec, void *cbParam);

typedef P2ERR (P2CALLSPEC *P2_DB_PROCESSRECORD_CALLBACK)(P2DB_RECORD_HANDLE rec, void *cbParam);
P2DB_API P2ERR P2CALLSPEC P2DBParseToCallback(P2_DB_GETNEWRECORD_CALLBACK pfnNewRecord, 
	   P2_DB_PROCESSRECORD_CALLBACK pfnProcessRecord, 
	   P2DB_TABLE_SCHEME_HANDLE tblSch, P2_ParseState *pState, void *cbParam);

////////////////////////////////////////////////////////////////////////////////

P2DB_API P2ERR P2CALLSPEC P2DBCreateWhereByIndex(P2DB_TABLE_INDEX_HANDLE idx, P2DB_RECORD_HANDLE Max, P2DB_RECORD_HANDLE Min, P2DB_CONN_HANDLE conn, P2DB_WHERE_HANDLE *pwhere);
P2DB_API P2ERR P2CALLSPEC P2DBCreateWhereByEqualIndex(P2DB_TABLE_INDEX_HANDLE idx, P2DB_RECORD_HANDLE rec, P2DB_CONN_HANDLE conn, P2DB_WHERE_HANDLE *pwhere);

///////////////////////////////////////////////////////////////////////////////////////////////////
//Функции, возвращающие признак isNull, сигнализирующий о том, что в поле содержится значение NULL
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldEx(P2DB_RECORD_HANDLE rec, const char *name, void *outBuf, size_t *pSize, BOOL* isNull);
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldByIndexEx(P2DB_RECORD_HANDLE rec, size_t idx, void *out_buf, size_t *pSize, BOOL* isNull);
P2DB_API P2ERR P2CALLSPEC P2DBGetFieldByIndexWithoutSchemeEx(P2DB_RECORD_HANDLE rs, size_t idx, void *out_buf, size_t *plen, BOOL* isNull, const char *p2type, UINT32 p2size, UINT32 p2prec);

P2DB_API P2ERR P2CALLSPEC P2DBSetFieldNull(P2DB_RECORD_HANDLE rs, const char *name);

P2DB_API P2ERR P2CALLSPEC P2DBSetFieldNullByIndex(P2DB_RECORD_HANDLE rs, size_t idx);


///////////////////////////////////////////////////////////////////////////////////////////////////
P2DB_API P2ERR P2DBSQLStatement2EnumEx(P2DB_CONN_HANDLE conn, const char *sql, pfnRecordProcWithParam proc, void* cbParam);

//////////////////////////////////////////////////////////////////////////
P2DB_API P2ERR P2CALLSPEC P2DBCreateWhereByIndexEx(P2DB_TABLE_INDEX_HANDLE idx, P2DB_RECORD_HANDLE Max, P2DB_RECORD_HANDLE Min, P2DB_CONN_HANDLE conn, const char *sql, P2DB_WHERE_HANDLE *pwhere);

///////////////////////////////////////////////////////////////////////////////////////////////////

#define DEFAULT_HINT    ""
#define MAX_HINT        HINT4

enum P2DBSQLHint
{
    HINT_COMMON = 0,	
    HINT1, // подставляется в функции P2DBCreateRecordsetWithRecordsetScheme
    HINT2, // подставляется в функции P2ReplGetLatestTableIdRevision
    HINT3, // подставляется в ф-ции CTable::GetSelectAllSql() ODBC-драйвера
    HINT4, // подставляется в функции P2DBCreateRecordsetWithRecordsetScheme в конец запроса
    HINT5  // подставляется в ф-ции CTable::GetSelectAllSql() ODBC-драйвера в конец запроса (SPODPT-1597)
};

P2DB_API P2ERR P2CALLSPEC P2DBLoadHintsFromIni(P2DB_CONN_HANDLE hConn, P2INI_HANDLE hIni);

P2DB_API P2ERR P2CALLSPEC P2DBGetHint(P2DB_CONN_HANDLE hConn, UINT32 hintNum, PSTR value, size_t* pLen);

P2DB_API P2ERR P2CALLSPEC P2DBSetHint(P2DB_CONN_HANDLE hConn, UINT32 hintNum, PCSTR value);

////////////////////////////////////////////////////////////////////////////////////////////////////
// Контроль превышения времени исполнения SQL-запросов

#define THRESHOLD_DEFAULT_VALUE 0

enum P2DBSQLThreshold
{
    THRESHOLD_EXECUTE_TOTAL      = 0, // exec_total_threshold
    THRESHOLD_EXECUTE_PER_RECORD = 1  // exec_per_record_threshold
};

#define THRESHOLD_MAX_COUNT (THRESHOLD_EXECUTE_PER_RECORD + 1)

#define THRESHOLD_EXECUTE_TOTAL_VALUE_NAME "exec_total_threshold"
#define THRESHOLD_EXECUTE_PER_RECORD_VALUE_NAME "exec_per_record_threshold"

P2DB_API P2ERR P2CALLSPEC P2DBLoadThresholdsFromIni(P2DB_CONN_HANDLE hConn, P2INI_HANDLE hIni);

P2DB_API P2ERR P2CALLSPEC P2DBGetThreshold(P2DB_CONN_HANDLE hConn, UINT32 thresholdId, UINT64 *value);

////////////////////////////////////////////////////////////////////////////////////////////////////

struct P2DBPreparedQuery;
typedef P2DBPreparedQuery* P2DB_QUERY_HANDLE;

/**
* P2DBCreatePreparedQuery - Создаёт хендл prepared-запроса
*
* Prepared-запрос позволяет скомпилировать sql-запрос однажды и выполнить его несколько раз,
*   с возможностью изменения параметров запроса.
* @see http://msdn.microsoft.com/ru-ru/library/ms131667.aspx
* @see http://www.sqlite.org/capi3ref.html#sqlite3_stmt
* Освобождение хендла должно быть выполнено вызывающим с помощью P2DBFreePreparedQuery().
*
* Пример работы:
*@@ AutoQueryHandle autoQuery;
*@@ P2DB_RECORD_HANDLE paramsRecordA;
*@@ const char * sql = "SELECT * FROM Table1 WHERE f2=@f2 AND f3=@f2";
*@@ if (P2FAILEDV(P2DBCreatePreparedQuery(&autoQuery, &paramsRecordA, dbConn, sql, paramsSchemaAutoPtr), res))
*@@ {
*@@     P2TRACE_ERROR_WITH_CODE((res, "Couldn't prepare query"))
*@@     return res;
*@@ }
*@@ INT64 f2 = 4;
*@@ INT64 f3 = 3;
*@@ RETURN_P2ERR(P2DBSetField(paramsRecordA, "f2", &f2, sizeof(f2)));
*@@ RETURN_P2ERR(P2DBSetField(paramsRecordA, "f3", &f3, sizeof(f3)));
*@@ AutoRecordsetHandle autoRS;
*@@ if (P2FAILEDV(P2DBExecutePreparedQuery(autoQuery, NULL, &autoRS), res))
*@@ {
*@@      P2TRACE_ERROR_WITH_CODE((res, "Couldn't execute prepared query"))
*@@      return res;
*@@ }
*@@ ProcessRecordset(autoRS());
*@@ f2 = 5;
*@@ RETURN_P2ERR(P2DBSetField(paramsRecordA, "f2", &f2, sizeof(f2)));
*@@ AutoRecordsetHandle autoRS2;
*@@ if (P2FAILEDV(P2DBExecutePreparedQuery(autoQuery, NULL, &autoRS2), res))
*@@ {
*@@      P2TRACE_ERROR_WITH_CODE((res, "Couldn't execute prepared query"))
*@@      return res;
*@@ }
*@@ ProcessRecordset(autoRS2());
*
* @param pQuery [req] Указатель, по которому будет помещён созданный хендл.
*
* @param pParamRecord [opt] Указатель на хендл записи, по которому будет помещёна созданная запись.
*              Запись будет иметь схему, указанную в параметре paramsSchema.
*              Хендл записи используется для заполнения параметров запроса.
*              Если запрос не имеет параметров, этот параметр может быть NULL.
*              Освобождение записи не требуется.
*
* @param conn [req] Хендл соединения с базой.
*
* @param sql [req] Строка sql-запроса.
*              Подставляемые параметры запроса должны быть отмечены маркерами '?', например
*                SELECT * FROM T WHERE field1=? AND field2>?
*              Также параметры запроса могут быть именованными, в этом случае связывание идёт по именам полей схемы.
*                SELECT * FROM T WHERE field1=@param1 AND field2>@param2
*
* @param paramsSchema [opt] Хендл схемы параметров.
*              Если в запросе гарантированно нет подставляемых параметров, это значение может быть NULL,
*                и в pParamRecord в этом случае будет помещён NULL.
*              Если в запросе параметры отмечены маркерами '?', количество маркеров должно совпадать с
*                количеством полей схемы.
*              Если параметры запроса именованы, схема должна иметь поля с такими же именами (без символа '@').
*
* @returns Код ошибки.
*/
P2DB_API P2ERR P2CALLSPEC P2DBCreatePreparedQuery(P2DB_QUERY_HANDLE *pQuery, P2DB_RECORD_HANDLE *pParamRecord,
    P2DB_CONN_HANDLE conn, const char *sql, P2DB_TABLE_SCHEME_HANDLE paramsSchema);

/**
* P2DBExecutePreparedQuery - Выполняет запрос и возвращает рекордсет.
*
* @param query [req] Хендл созданного ранее запроса.
*
* @param recordsetScheme [opt] Схема рекордсета.
*
* @param pRecordset [req] Указатель, по которому будет помещён созданный хендл рекордсета.
*              Освобождение рекордсета должно быть выполнено вызывающим.
*
* @returns Код ошибки.
*/
P2DB_API P2ERR P2CALLSPEC P2DBExecutePreparedQuery(P2DB_QUERY_HANDLE query, P2DB_TABLE_SCHEME_HANDLE recordsetScheme,
        P2DB_RECORDSET_HANDLE *pRecordset);

/**
* P2DBExecutePreparedQueryNonQuery - Выполняет запрос, который не требует возвращения данных (INSERT, UPDATE, etc)
*
* @param query [req] Хендл созданного ранее запроса.
*
* @returns Код ошибки.
*/
P2DB_API P2ERR P2CALLSPEC P2DBExecutePreparedQueryNonQuery(P2DB_QUERY_HANDLE query);

/**
* P2DBFreePreparedQuery - Освобождает созданный ранее prepared-запрос.
*
* @param query [req] Хендл созданного ранее запроса.
*
* @returns Код ошибки.
*/
P2DB_API P2ERR P2CALLSPEC P2DBFreePreparedQuery(P2DB_QUERY_HANDLE query);
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
};
#endif

#endif // _P2DB_H_
