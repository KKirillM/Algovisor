#ifndef _P2TBL_H_
#define _P2TBL_H_


#ifdef P2TBL_EXPORTS
#define P2TBL_API P2_API_EXPORTS
#else
#define P2TBL_API P2_API_IMPORTS
#endif

#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II/P2SysScheme/Include/P2SysScheme.h"
#include "../../../Plaza-II/P2SysRecord/Include/P2SysRecord.h"
#include "../../../Plaza-II-DB/P2DB/Include/P2DB.h"
#include "../../../Plaza-II-Repl/P2ReplCommon/Include/P2Repl.h"
#include "../../../Plaza-II-Repl/P2ReplUtils/Include/P2ReplDS.h"

#ifdef  __cplusplus
extern "C" {
#endif
//////////////////////////////////////////////////////////////////////

struct P2TblConnection;
typedef P2TblConnection*    P2TBL_CONN_HANDLE;
struct P2TblTable;
typedef P2TblTable*         P2TBL_TABLE_HANDLE;

//struct P2TblRecordset;
//typedef P2TblRecordset*     P2TBL_RECORDSET;

//////////////////////////////////////////////////////////////////////

// Callback для перебора записей в таблице.
// iteration_number увеличивается c 0.
// Если коллбэк возвращает не P2ERR_OK, то перебор прекращается
// и enum-функция возращает этот код ошибки.
typedef P2ERR (P2CALLSPEC *P2TBL_ENUM_CALLBACK)(size_t iteration_number, const void* pRec, void* pParam);

//////////////////////////////////////////////////////////////////////

// Порядок записей при переборе в ???():
// - по возрастанию в соответствии с индексом index_id,
#define P2TBL_ENUM_DIRECTION_FORWARD    0x00000000
// - по убыванию в соответствии с индексом index_id.
#define P2TBL_ENUM_DIRECTION_BACKWARD   0x00000001
#define P2TBL_ENUM_DIRECTION_MASK_      0x00000001

//////////////////////////////////////////////////////////////////////

// Опции работы базы данных

//опции, могут быть выставлены только после вызова connect (кроме P2TBL_ISAMFFDB_ASYNC_IO)
#define P2TBL_STARTUP_MASK		         0x0001 

// - передача данных в базу через prepared записи
#define P2TBL_PREPARED					 0x0001 
// Опции репликации данных из базы
#define P2TBL_REPL_MASK					 0x0006
// - кэш без репликации (только для P2ISAMCache)
// - режим по умолчанию
#define P2TBL_NO_REPL					 0x0000
// - была опция REPL_ISAM, теперь она неактуальна. Но битик пока зарезервируем
#define P2TBL_REPL_RESERVED				 0x0002
// - кэш с репликацией из базы (только для P2ISAMCache)
#define P2TBL_REPL_DB					 0x0004

#define P2TBL_ISAMSM_FAST_DELETE         0x0010
#define P2TBL_ISAMSM_TURN_OFF_FLUSH      0x0020

#define P2TBL_ISAM_TURN_OFF_TRANS_LOG	 0x0040
#define P2TBL_ISAM_TURN_OFF_CS			 0x0080
#define P2TBL_ISAM_OPTION_MASK			 (P2TBL_ISAM_TURN_OFF_TRANS_LOG | P2TBL_ISAM_TURN_OFF_CS)

#define P2TBL_ISAMFFDB_TURN_ON_FLUSH     0x0100
// - испольльзуем ли async IO для FFDB
#define P2TBL_ISAMFFDB_ASYNC_IO          0x0200

// - означает, что tbl-connection используется онлайн сервером с быстрым поднятием
#define P2TBL_ONLINE_SERVER_USE		 	0x0400

//////////////////////////////////////////////////////////////////////

// маска с помощью которой можно определить когда нужно выставлять эту опцию работы базы данных - до connect или после
#define P2TBL_BEFORE_CONNECT P2TBL_ISAMFFDB_ASYNC_IO

//Опции для создания базы данных

#define P2TBL_DROP_IF_EXIST				0x0000 //default
#define P2TBL_FAIL_IF_EXIST				0x0001
#define P2TBL_SKIP_IF_EXIST				0x0002

#define P2TBL_CREATE_ALL_INDEXES		0x0000 //default
#define P2TBL_CREATE_REPL_INDEXES		0x0004

//////////////////////////////////////////////////////////////////////
// опции для PostPreparedEx. равны P2REPL_POST_* из ReplClient.h
#define P2TBL_POST_MODE_CHECK		0
#define P2TBL_POST_MODE_INSERT		1
#define P2TBL_POST_MODE_UPDATE		2
#define P2TBL_STORE_DELETED_RECS	4
//////////////////////////////////////////////////////////////////////
// опции для ClearOldRev2
#define P2TBL_SKIP_FAKE_UPDATE			 1
#define P2TBL_REPL_SERVER_FIRST_REVISION 2  // ClearOldRev отослан сервером репликации и содержит first revision по таблице
#define P2TBL_CLEAR_AND_CLEAN			 4  // Долгий ClearOldRev FFDB
//////////////////////////////////////////////////////////////////////
P2TBL_API P2ERR P2CALLSPEC P2TBLStartup();
P2TBL_API void P2CALLSPEC P2TBLCleanup();

P2TBL_API P2ERR P2CALLSPEC P2TBLCreateConnection(const char* dllName, P2TBL_CONN_HANDLE* pConn);
P2TBL_API P2ERR P2CALLSPEC P2TBLFreeConnection(P2TBL_CONN_HANDLE conn);
P2TBL_API P2ERR P2CALLSPEC P2TBLDuplicateConnectionHandle(P2TBL_CONN_HANDLE src, P2TBL_CONN_HANDLE* pDest);

P2TBL_API P2ERR P2CALLSPEC P2TBLConnect(P2TBL_CONN_HANDLE conn, P2DB_SCHEME_HANDLE sch, const char* params);
P2TBL_API P2ERR P2CALLSPEC P2TBLDisconnect(P2TBL_CONN_HANDLE conn);
// open = create + connect. 
P2TBL_API P2ERR P2CALLSPEC P2TBLOpenConnection(P2DB_SCHEME_HANDLE sch, const char* params, P2TBL_CONN_HANDLE* pConn);
// close = disconnect + free
P2TBL_API P2ERR P2CALLSPEC P2TBLCloseConnection(P2TBL_CONN_HANDLE conn);

P2TBL_API P2ERR P2CALLSPEC P2TBLGetSchemeFromConnection(P2TBL_CONN_HANDLE conn, P2DB_SCHEME_HANDLE* pSch);

P2TBL_API P2ERR P2CALLSPEC P2TBLBeginTransaction(P2TBL_CONN_HANDLE conn);
P2TBL_API P2ERR P2CALLSPEC P2TBLCommit(P2TBL_CONN_HANDLE conn);
P2TBL_API P2ERR P2CALLSPEC P2TBLRollback(P2TBL_CONN_HANDLE conn);
P2TBL_API P2ERR P2CALLSPEC P2TBLCommit2(P2TBL_CONN_HANDLE conn, const char* eventName);

P2TBL_API P2ERR P2CALLSPEC P2TBLLoadDatabase(P2TBL_CONN_HANDLE conn);
P2TBL_API P2ERR P2CALLSPEC P2TBLSaveDatabase(P2TBL_CONN_HANDLE conn);

P2TBL_API P2ERR P2CALLSPEC P2TBLOpenTableByIndex(P2TBL_CONN_HANDLE conn, size_t tblIdx, P2TBL_TABLE_HANDLE* pTbl);
P2TBL_API P2ERR P2CALLSPEC P2TBLCloseTable(P2TBL_TABLE_HANDLE tbl);

P2TBL_API P2ERR P2CALLSPEC P2TBLTruncateTable(P2TBL_TABLE_HANDLE tbl);
P2TBL_API P2ERR P2CALLSPEC P2TBLGetSchemeFromTable(P2TBL_TABLE_HANDLE tbl, P2DB_TABLE_SCHEME_HANDLE* pTblSch);

P2TBL_API P2ERR P2CALLSPEC P2TBLGetRecsCount(P2TblTable* pTbl, size_t* pRecsCnt);

P2TBL_API P2ERR P2CALLSPEC P2TBLGetRec(P2TBL_TABLE_HANDLE tbl, size_t indexId, void* pRec);

P2TBL_API P2ERR P2CALLSPEC P2TBLEnumRecs(P2TBL_TABLE_HANDLE tbl, size_t indexId, UINT32 flags,
                                         const void* pRecMin, const void* pRecMax,
                                         P2TBL_ENUM_CALLBACK cb, void* cb_param);

P2TBL_API P2ERR P2CALLSPEC P2TBLGetRecMin(P2TBL_TABLE_HANDLE tbl, size_t indexId, const void* pRecMin, const void* pRecMax, void* pRec);
P2TBL_API P2ERR P2CALLSPEC P2TBLGetRecMax(P2TBL_TABLE_HANDLE tbl, size_t indexId, const void* pRecMin, const void* pRecMax, void* pRec);

P2TBL_API P2ERR P2CALLSPEC P2TBLInsertRecord(P2TBL_TABLE_HANDLE tbl, void* pRec);
P2TBL_API P2ERR P2CALLSPEC P2TBLUpdateRecord(P2TBL_TABLE_HANDLE tbl, size_t indexId, void* pRec);
P2TBL_API P2ERR P2CALLSPEC P2TBLDeleteRecord(P2TBL_TABLE_HANDLE tbl, size_t indexId, void* pRec);

P2TBL_API P2ERR P2CALLSPEC P2TBLGetTableRecSize(P2TBL_TABLE_HANDLE tbl, size_t* pSize);

P2TBL_API P2ERR P2CALLSPEC P2TBLLoadTable(P2TBL_TABLE_HANDLE tbl);
P2TBL_API P2ERR P2CALLSPEC P2TBLSaveTable(P2TBL_TABLE_HANDLE tbl);

P2TBL_API P2ERR P2CALLSPEC P2TBLGetDBConnection(P2TBL_CONN_HANDLE conn, P2DB_CONN_HANDLE* pDBConn);
P2TBL_API P2ERR P2CALLSPEC P2TBLFlushChanges(P2TBL_CONN_HANDLE conn);

P2TBL_API P2ERR P2CALLSPEC P2TBLPostPrepared(P2TBL_TABLE_HANDLE tbl, const void* pRec);
P2TBL_API P2ERR P2CALLSPEC P2TBLPostPreparedEx(P2TBL_TABLE_HANDLE tbl, const void* pRec, UINT32 flags);

P2TBL_API P2ERR P2CALLSPEC P2TBLSetOptionsEx(P2TBL_CONN_HANDLE conn, UINT32 opt, UINT32 mask);
P2TBL_API P2ERR P2CALLSPEC P2TBLSetOptions(P2TBL_CONN_HANDLE conn, UINT32 opt);
P2TBL_API P2ERR P2CALLSPEC P2TBLGetOptions(P2TBL_CONN_HANDLE conn, UINT32* opt);

P2TBL_API P2ERR P2CALLSPEC P2TBLGetTableInfo(P2TBL_TABLE_HANDLE tbl, size_t* pTblIdx, P2TBL_CONN_HANDLE* pConn);

P2TBL_API P2ERR P2CALLSPEC P2TBLClearOldRev(P2TBL_TABLE_HANDLE tbl, P2REPL_REVISION_TYPE rev);
P2TBL_API P2ERR P2CALLSPEC P2TBLClearOldRev2(P2TBL_TABLE_HANDLE tbl, P2REPL_REVISION_TYPE rev, UINT32 flag);

P2TBL_API P2ERR P2CALLSPEC P2TBLGetReplDS(P2TBL_CONN_HANDLE conn, P2REPL_DS_HANDLE* pDS);

P2TBL_API P2ERR P2CALLSPEC P2TBLClearDeleted(P2TBL_TABLE_HANDLE tbl);

P2TBL_API P2ERR P2CALLSPEC P2TBLGetLatestTableIdRevision(P2TBL_TABLE_HANDLE tbl, P2REPL_ID_TYPE *pId, P2REPL_REVISION_TYPE *pRev);

P2TBL_API P2ERR P2CALLSPEC P2TBLCreateDatabase(PCSTR tblConnStr, P2DB_SCHEME_HANDLE hScheme, UINT32 opt);
P2TBL_API P2ERR P2CALLSPEC P2TBLDeleteDatabase(PCSTR tblConnStr, P2DB_SCHEME_HANDLE hScheme);

P2TBL_API P2ERR P2CALLSPEC P2TBLCreateSchemeFromDatabase(P2DB_SCHEME_HANDLE *pDestScheme, PCSTR tblConnStr, PCSTR srvcTableName, PCSTR streamName);
P2TBL_API P2ERR P2CALLSPEC P2TBLSaveSchemeToDatabase(P2DB_SCHEME_HANDLE hSch, PCSTR tblConnStr, PCSTR srvcTableName, PCSTR streamName);
P2TBL_API P2ERR P2CALLSPEC P2TBLDeleteSchemeFromDatabase(PCSTR tblConnStr, PCSTR srvcTableName, PCSTR streamName);


#ifdef  __cplusplus
}
#endif

#endif // _P2TBL_H_
