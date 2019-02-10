#ifndef _P2REPLSERVER_H_
#define _P2REPLSERVER_H_

#include "../../../Plaza-II/P2Common/Include/P2Types.h"

#ifdef P2REPLSERVER_EXPORTS
#define P2REPLSERVER_API P2_API_EXPORTS
#else
#define P2REPLSERVER_API P2_API_IMPORTS
#endif

#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II-DB/P2DB/Include/P2DB.h"
#include "../../../Plaza-II-MQ/P2MQ/Include/P2MQ.h"
#include "../../../Plaza-II-Repl/P2ReplCommon/Include/P2Repl.h"
#include "../../../Plaza-II-Repl/P2ReplUtils/Include/P2ReplDS.h"
#include "../../../Plaza-II-DB/P2Tbl/Include/P2Tbl.h"
#include "../../../Plaza-II/P2ACL/Include/P2ACL.h"

#ifdef  __cplusplus
extern "C" {
#endif

// Текущая версия протокола репликации
#define P2REPL_SERVER_PROTOCOL_VERSION 15

// Минимальная версия протокола репликации, с которой разрешено подключаться клиентам
// Может быть переопределена в большую сторону через настройку в ини-файле
#define P2REPL_SERVER_PROTOCOL_VERSION_MIN 12

// флаги для ф-ии P2ReplServerSetFlags
// флаги, пересылаемые на клиента должны располагаться в старшем слове
#define P2REPLSERVER_OPTIONS_CLEAR_CLIENT_DATA_ON_CONNECT   0x10000 // посылать ClearOldRev(P2REPL_MAX_REV) по всем таблицам клиенту при подключении
#define P2REPLSERVER_OPTIONS_ALLOW_CLIENT_FILTER            0x20000 // клиенту разрешено устанавливать VM-фильтр

#define P2REPLSERVER_OPTIONS_PASSTHROUGH_MASK               0xFFFF0000

// состояния сервера репликации
#define P2REPLSERVER_STATE_UNBOUND      0
#define P2REPLSERVER_STATE_ERROR        1
#define P2REPLSERVER_STATE_BOUND        2


struct P2ReplServer;
struct IReplServerPerUserSender;

typedef P2ReplServer *P2REPL_SERVER_HANDLE;


P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerStartup(void);
P2REPLSERVER_API void  P2CALLSPEC P2ReplServerCleanup(void);

P2REPLSERVER_API BOOL P2CALLSPEC P2ReplServerProcessMessage(P2MSG_HANDLE msg);

P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerCreateHandle(P2REPL_SERVER_HANDLE *pHandle);
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerFreeHandle(P2REPL_SERVER_HANDLE handle);

P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerSetFlags(P2REPL_SERVER_HANDLE handle, UINT32 flags);
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerGetFlags(P2REPL_SERVER_HANDLE handle, UINT32* pFlags);

P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerGetState(P2REPL_SERVER_HANDLE handle, UINT32* pState);

// связать с базой
// связывает хэндл с базой, коннекшном и схемой вьюшек (если NULL, то берется схема базы)
#define P2REPL_SERVER_SYNC_LIFE_NUM_WITH_DB_SCHEME_BIND_FLAG	1

P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerBind(P2REPL_SERVER_HANDLE handle, const char *serviceName, P2MQ_CONN_HANDLE connHandle, P2DB_CONN_HANDLE dbHandle, P2DB_SCHEME_HANDLE customScheme);
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerBindFromSection(P2REPL_SERVER_HANDLE handle, P2MQ_CONN_HANDLE connHandle, P2DB_CONN_HANDLE dbHandle, const char *sectionName);
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerBindFromSection2(P2REPL_SERVER_HANDLE handle, P2MQ_CONN_HANDLE connHandle, P2DB_CONN_HANDLE dbHandle, const char *sectionName, UINT32 flags);
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerUnbind(P2REPL_SERVER_HANDLE handle);

// добавить/обновить соответствующие записи 
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerInsertRecord(P2DB_CONN_HANDLE db, P2DB_RECORD_HANDLE primaryTableRecordHandle);
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerUpdateRecord(P2DB_CONN_HANDLE db, P2DB_RECORD_HANDLE primaryTableRecordHandle);


// удаление записи, соответствующей primaryTableRecordHandle
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerDeleteRecord(P2DB_CONN_HANDLE db, P2DB_TABLE_SCHEME_HANDLE tblSch, P2REPL_ID_TYPE id);


// read parameters from ini and set them
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerSetParamsFromSection(P2REPL_SERVER_HANDLE handle, P2INI_HANDLE hIni, const char* section);

P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerSetFilter(P2REPL_SERVER_HANDLE handle, const char* tblName, const void* byteCode, size_t size);
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerSetFilterFromTable(P2REPL_SERVER_HANDLE handle, P2DB_CONN_HANDLE conn, P2DB_TABLE_HANDLE tbl);
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerSetFilterFromTableISAM(P2REPL_SERVER_HANDLE handle, P2TBL_TABLE_HANDLE tbl);

P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplPostOnline(P2DB_CONN_HANDLE conn, const char *primaryTblName, P2REPL_REVISION_TYPE *pRev);
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplPostOnline2(P2DB_CONN_HANDLE conn, P2DB_TABLE_SCHEME_HANDLE tblSch, P2REPL_REVISION_TYPE *pRev);

P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerPostPreparedOnline(P2DB_CONN_HANDLE conn, P2DB_RECORD_HANDLE rec);


P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplBeginTransaction(P2DB_CONN_HANDLE conn);
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplCommit(P2DB_CONN_HANDLE conn);
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplRollback(P2DB_CONN_HANDLE conn);

// удалить все старые записи типа id == deleted и нотифицировать клиентов
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerClearDeleted(P2DB_CONN_HANDLE conn, P2DB_TABLE_SCHEME_HANDLE tblSch);
// удалить записи с указанного ревижна и нотифицировать клиентов
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerClearPrepared(P2DB_CONN_HANDLE conn, P2DB_TABLE_SCHEME_HANDLE tblSch, P2REPL_REVISION_TYPE rev);
// найти последнюю запись и нотифицировать клиентов
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerPostCleared(P2DB_CONN_HANDLE conn, P2DB_TABLE_SCHEME_HANDLE tblSch);

P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerPostPreparedOnlineEx(P2DB_CONN_HANDLE conn, P2DB_RECORD_HANDLE origRec, P2DB_RECORD_HANDLE insRec, P2DB_RECORD_HANDLE updRec);

// связывает хэндл с базой, коннекшном, 
// серверной схемой репликации (если NULL, то берется схема фильтрации данных)
// схемой для филльтрации данных(если NULL, то берется схема базы), 
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerBind2(P2REPL_SERVER_HANDLE handle, const char *serviceName, P2MQ_CONN_HANDLE connHandle, P2DB_CONN_HANDLE dbHandle, P2DB_SCHEME_HANDLE customScheme, P2DB_SCHEME_HANDLE filterScheme);

// назначение ACL для сервера из таблицы базы данных
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerSetACLFromTable(P2REPL_SERVER_HANDLE handle, P2DB_CONN_HANDLE conn, P2DB_TABLE_HANDLE tbl);
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerSetACLFromTableISAM(P2REPL_SERVER_HANDLE handle, P2TBL_TABLE_HANDLE tbl); 
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerSetACL(P2REPL_SERVER_HANDLE handle, P2ACL_HANDLE hACL);

// номер жизни схемы базы данных
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerSetLifeNum(P2REPL_SERVER_HANDLE handle, UINT32 LifeNum);

P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerReconnectClients(P2REPL_SERVER_HANDLE handle, UINT8 /*flag*/);

P2REPLSERVER_API P2ERR P2ReplServerPostPreparedOnlineEx2(P2DB_CONN_HANDLE conn, P2DB_RECORD_HANDLE origRec, P2DB_RECORD_HANDLE insRec, P2DB_RECORD_HANDLE updRec, UINT32 flag);

//////////////////////////////////////////////////////////////////////////
// for removing ISAM ReplServer API
//////////////////////////////////////////////////////////////////////////

P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerBindWithDSInt(P2REPL_SERVER_HANDLE handle, const char *serviceName, P2MQ_CONN_HANDLE connHandle, P2REPL_DS_HANDLE dsHandle, P2DB_SCHEME_HANDLE replScheme, P2DB_SCHEME_HANDLE filterScheme);
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerBindFromSectionWithDSInt(P2REPL_SERVER_HANDLE handle, P2MQ_CONN_HANDLE connHandle, P2REPL_DS_HANDLE dsHandle, const char *sectionName, UINT32 flags);
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerBindWithExternalSenderInt(P2REPL_SERVER_HANDLE handle, const char *serviceName, P2MQ_CONN_HANDLE connHandle, IReplServerPerUserSender* pSender);

P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerPostOnlineFromRevRangeInt(P2REPL_DS_CONN_HANDLE conn, const char *primaryTable, P2REPL_REVISION_TYPE revLow, P2REPL_REVISION_TYPE revHigh);

P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerPostOnlineFromRevInt(P2REPL_DS_CONN_HANDLE conn, const char *primaryTable, P2REPL_REVISION_TYPE rev);

P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerBeginReplTransactionInt(P2REPL_DS_CONN_HANDLE conn);

P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerCommitInt(P2REPL_DS_CONN_HANDLE conn);

// Блокировка RDS коннекшена и связанных с ним серверов
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerLockServersByConnection(P2REPL_DS_CONN_HANDLE conn);
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerUnlockServersByConnection(P2REPL_DS_CONN_HANDLE conn);

struct TTblChanges
{
    P2REPL_REVISION_TYPE rev;
    const char *tblName;
};

struct TTblChanges1
{
    size_t	tblIdx;
    P2REPL_REVISION_TYPE rev;
};

P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerRegTransactionTmp(P2REPL_DS_CONN_HANDLE ds, TTblChanges *pTblChg, size_t tblCnt);
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerRegTransactionTmp2(P2REPL_DS_CONN_HANDLE ds, TTblChanges1 *pTblChg, size_t tblCnt);

P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerRollbackInt(P2REPL_DS_CONN_HANDLE conn);

P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerPostClearDeletedInt(P2REPL_DS_CONN_HANDLE conn, const char *primaryTable, P2REPL_REVISION_TYPE rev, UINT32 flags);

P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerGetToken(P2REPL_SERVER_HANDLE handle, char * pBuf, size_t * pSz);


#ifndef P2REPL_SNAPSHOT_SERVER_SUFFIX
#define P2REPL_SNAPSHOT_SERVER_SUFFIX "_SNAPSHOT"
#endif // P2REPL_SNAPSHOT_SERVER_SUFFIX

//////////////////////////////////////////////////////////////////////////
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerCreateCustomDS(PCSTR dllName, PCSTR sectionName, P2DB_CONN_HANDLE hDbConn, P2REPL_DS_HANDLE* ppDs);
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerFreeCustomDS(P2REPL_DS_HANDLE pDs);

#ifdef _FAKECLIENT_SUPPORT
P2REPLSERVER_API P2ERR P2CALLSPEC P2ReplServerAddFakeClient(P2REPL_SERVER_HANDLE handle, const char * nodename, const char * schemeUrl);
#endif // _FAKECLIENT_SUPPORT

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _P2REPLSERVER_H_
