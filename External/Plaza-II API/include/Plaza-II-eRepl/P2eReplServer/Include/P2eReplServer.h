#ifndef _P2EREPLSERVER_H_
#define _P2EREPLSERVER_H_

#ifdef P2EREPLSERVER_EXPORTS
#define P2EREPLSERVER_API P2_API_EXPORTS
#else
#define P2EREPLSERVER_API P2_API_IMPORTS
#endif

#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II-MQ/P2MQ/Include/P2MQ.h"
#include "../../../Plaza-II-eRepl/P2eReplCommon/Include/P2eReplTypes.h"
#include "../../../Plaza-II-eRepl/P2eReplUtils/Include/P2eReplDS.h"
#include "../../../Plaza-II-eRepl/P2eReplStorage/Include/P2eReplStorage.h"
#include "../../../Plaza-II-DB/P2Tbl/Include/P2Tbl.h"
#include "../../../Plaza-II/P2ACL/Include/P2ACL.h"

#ifdef  __cplusplus
extern "C" {
#endif

#define P2EREPL_SERVER_MIN_PROTOCOL_VERSION 100 // Используется исключительно в роутере. Версии протоколов е-реплики должны быть >=100. Не менять!

#define P2EREPL_SERVER_PROTOCOL_VERSION 100 // Версию протокола еРепликации продвигать здесь.

struct P2eReplServer;

typedef P2eReplServer *P2EREPL_SERVER_HANDLE;

#if P2EREPL_SERVER_MIN_PROTOCOL_VERSION < 100
#error Alarma!
#endif
#if P2EREPL_SERVER_MIN_PROTOCOL_VERSION > P2EREPL_SERVER_PROTOCOL_VERSION
#error Alarma!
#endif

P2EREPLSERVER_API P2ERR P2CALLSPEC P2eReplServerStartup(void);
P2EREPLSERVER_API void P2CALLSPEC P2eReplServerCleanup(void);

P2EREPLSERVER_API BOOL P2CALLSPEC  P2eReplServerProcessMessage(P2MSG_HANDLE msg);

P2EREPLSERVER_API P2ERR P2CALLSPEC P2eReplServerCreateHandle(P2EREPL_SERVER_HANDLE *pHandle);
P2EREPLSERVER_API P2ERR P2CALLSPEC P2eReplServerFreeHandle(P2EREPL_SERVER_HANDLE handle);

P2EREPLSERVER_API P2ERR P2CALLSPEC P2eReplServerBind(P2EREPL_SERVER_HANDLE handle, P2MQ_CONN_HANDLE connHandle, const char *serviceName, P2EREPL_STORAGE_HANDLE storageHandle);
P2EREPLSERVER_API P2ERR P2CALLSPEC P2eReplServerBindFromSection(P2EREPL_SERVER_HANDLE handle, P2MQ_CONN_HANDLE connHandle, const char *sectionName, P2EREPL_STORAGE_HANDLE storageHandle, UINT32 flags); 
P2EREPLSERVER_API P2ERR P2CALLSPEC P2eReplServerBindWithDSInt(P2EREPL_SERVER_HANDLE handle, P2MQ_CONN_HANDLE connHandle, const char *serviceName, P2EREPL_DS_HANDLE dsHandle);
P2EREPLSERVER_API P2ERR P2CALLSPEC P2eReplServerBindFromSectionWithDSInt(P2EREPL_SERVER_HANDLE handle, P2MQ_CONN_HANDLE connHandle, const char *sectionName, P2EREPL_DS_HANDLE dsHandle, UINT32 flags);
P2EREPLSERVER_API P2ERR P2CALLSPEC P2eReplServerUnbind(P2EREPL_SERVER_HANDLE handle);

// read parameters from ini and set them
P2EREPLSERVER_API P2ERR P2CALLSPEC P2eReplServerSetParamsFromSection(P2EREPL_SERVER_HANDLE handle, P2INI_HANDLE hIni, const char* section);

P2EREPLSERVER_API P2ERR P2CALLSPEC P2eReplServerBeginTransaction(P2EREPL_STORAGE_HANDLE db);
// добавить эвент с заполнением поля 'ревижен'
P2EREPLSERVER_API P2ERR P2CALLSPEC P2eReplServerInsertEvent(P2EREPL_STORAGE_HANDLE db, TEReplRec * pEvent);
// добавить эвент с установленным полем ревижен
P2EREPLSERVER_API P2ERR P2CALLSPEC P2eReplServerInsertEventInt(P2EREPL_STORAGE_HANDLE db, const TEReplRec * pEvent);
P2EREPLSERVER_API P2ERR P2CALLSPEC P2eReplServerCommitTransaction(P2EREPL_STORAGE_HANDLE db);
P2EREPLSERVER_API P2ERR P2CALLSPEC P2eReplServerRollbackTransaction(P2EREPL_STORAGE_HANDLE db);

// удалить записи с указанного ревижна и нотифицировать клиентов
P2EREPLSERVER_API P2ERR P2CALLSPEC P2eReplServerClearOldRevs(P2EREPL_STORAGE_HANDLE db, P2EREPL_REVISION_TYPE rev, UINT32 flags);
P2EREPLSERVER_API P2ERR P2CALLSPEC P2eReplServerPostClearOldRevsInt(P2EREPL_DS_HANDLE ds, P2EREPL_REVISION_TYPE rev, UINT32 flags);

// назначение ACL для сервера из таблицы базы данных
P2EREPLSERVER_API P2ERR P2CALLSPEC P2eReplServerSetACLFromTable(P2EREPL_SERVER_HANDLE handle, P2DB_CONN_HANDLE conn, P2DB_TABLE_HANDLE tbl);
P2EREPLSERVER_API P2ERR P2CALLSPEC P2eReplServerSetACLFromTableISAM(P2EREPL_SERVER_HANDLE handle, P2TBL_TABLE_HANDLE tbl);
P2EREPLSERVER_API P2ERR P2CALLSPEC P2eReplServerSetACL(P2EREPL_SERVER_HANDLE handle, P2ACL_HANDLE hACL);

// номер жизни базы данных
P2EREPLSERVER_API P2ERR P2CALLSPEC P2eReplServerSetLifeNum(P2EREPL_SERVER_HANDLE handle, UINT32 lifeNum);

P2EREPLSERVER_API P2ERR P2CALLSPEC P2eReplServerReconnectClients(P2EREPL_SERVER_HANDLE handle);

P2EREPLSERVER_API P2ERR P2CALLSPEC P2eReplServerLockServersByConnection(P2EREPL_STORAGE_HANDLE conn);
P2EREPLSERVER_API P2ERR P2CALLSPEC P2eReplServerUnlockServersByConnection(P2EREPL_STORAGE_HANDLE conn);

P2EREPLSERVER_API P2ERR P2CALLSPEC P2eReplServerPostOnlineFromRevInt(P2EREPL_DS_HANDLE ds, P2EREPL_REVISION_TYPE rev);

P2EREPLSERVER_API P2ERR P2CALLSPEC P2eReplServerGetToken(P2EREPL_SERVER_HANDLE handle, char * pBuf, size_t * pSz);

#ifndef P2EREPL_SNAPSHOT_SERVER_SUFFIX
#define P2EREPL_SNAPSHOT_SERVER_SUFFIX "_SNAPSHOT"
#endif

#ifdef __cplusplus
}
#endif

#endif // _P2EREPLSERVER_H_
