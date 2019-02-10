#pragma once

#ifdef P2EREPLCLIENT_EXPORTS
#define P2EREPLCLIENT_API P2_API_EXPORTS
#else
#define P2EREPLCLIENT_API P2_API_IMPORTS
#endif

#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II/P2SysMsg/Include/P2SysMsg.h"
#include "../../../Plaza-II-MQ/P2MQ/Include/P2MQ.h"
#include "../../P2eReplCommon/Include/P2eReplTypes.h"

#ifdef  __cplusplus
extern "C" {
#endif

#define P2EREPL_CLIENT_PROTOCOL_VERSION 100 // Версии протоколов е-реплики должны быть >=100

typedef void* P2EREPL_DATASTREAM_HANDLE;

typedef void (P2CALLSPEC *P2EREPL_DATA_CALLBACK)(P2MSG_HANDLE msg, void* cbParams);
typedef void (P2CALLSPEC *P2EREPL_DATA_FAST_CALLBACK)(UINT32 notif, void* cbParams, const TEReplRec* pRec);

typedef void (P2CALLSPEC *P2EREPL_VA_DATA_CALLBACK)(UINT32 notif, void *cbParams, ...);

/*
cbProc(P2EREPL_CLIENT_NOTIF_OPENED, m_cbParams, serverLifeNum);
cbProc(P2EREPL_CLIENT_NOTIF_LIFE_NUM_CHANGED, m_cbParams, serverLifeNum);
cbProc(P2EREPL_CLIENT_NOTIF_OPEN_FAILED, m_cbParams, reason);

cbProc(P2EREPL_CLIENT_NOTIF_CLEAR_OLD_REV, m_cbParams, clearRev, clearFlags);

cbProc(P2EREPL_CLIENT_NOTIF_ONLINE, m_cbParams, m_revision);
cbProc(P2EREPL_CLIENT_NOTIF_CLOSED_BY_SERVER, m_cbParams, reason, m_revision, redirectServer);
cbProc(P2EREPL_CLIENT_NOTIF_CLOSE_ON_COMPLETE, m_cbParams);

cbProc(P2EREPL_CLIENT_NOTIF_ERROR, m_cbParams, reason);
cbProc(P2EREPL_CLIENT_NOTIF_TIMEOUT, m_cbParams, reason);
cbProc(P2EREPL_CLIENT_NOTIF_REQUEST_FAILED, m_cbParams, reason);

cbProc(notif, m_cbParams); дефолтный вызов 

cbProc(P2EREPL_CLIENT_NOTIF_DATA_BEGIN, m_cbParams);
cbProc(P2EREPL_CLIENT_NOTIF_DATA_END, m_cbParams);
cbProc(P2EREPL_CLIENT_NOTIF_RECORD, m_cbParams, pRec);

*/



struct IeReplClientStorage;

//////////////////////////////////////////////////////////////////////

#define P2EREPL_DATASTREAM_TYPE_SNAPSHOT			0x0000  // (def.)
#define P2EREPL_DATASTREAM_TYPE_ONLINE				0x0001
#define P2EREPL_DATASTREAM_TYPE_ONLY_ONLINE			0x0002
#define P2EREPL_DATASTREAM_TYPE_AUTO_REQUEST		0x0000  // (def.)
#define P2EREPL_DATASTREAM_TYPE_MANUAL_REQUEST		0x0100
#define P2EREPL_DATASTREAM_TYPE_MULTICAST			0X0200

//////////////////////////////////////////////////////////////////////

#define P2EREPL_DATASTREAM_STATE_CLOSED              0
#define P2EREPL_DATASTREAM_STATE_ERROR               1     // stream broken, for use it must be reopened
#define P2EREPL_DATASTREAM_STATE_OPENING             10
#define P2EREPL_DATASTREAM_STATE_READY               25    // ready for request data
#define P2EREPL_DATASTREAM_STATE_RECEIVING_SNAPSHOT  30    // getting snapshot data
#define P2EREPL_DATASTREAM_STATE_RECEIVING_ONLINE    35    // requested snapshot received

//////////////////////////////////////////////////////////////////////

#define P2EREPL_CLIENT_NOTIF_UNDEFINED           0
#define P2EREPL_CLIENT_NOTIF_OPENED              1
#define P2EREPL_CLIENT_NOTIF_OPEN_FAILED         2
#define P2EREPL_CLIENT_NOTIF_ONLINE              3
#define P2EREPL_CLIENT_NOTIF_CLOSED_BY_SERVER    4
#define P2EREPL_CLIENT_NOTIF_ERROR               5
#define P2EREPL_CLIENT_NOTIF_TIMEOUT             6
#define P2EREPL_CLIENT_NOTIF_REQUEST_FAILED      7
// При закрытии datastream-а, открытого с типом SNAPSHOT_ONLY, когда поток закрыт после успешного получения SNAPSHOTa
#define P2EREPL_CLIENT_NOTIF_CLOSE_ON_COMPLETE   8
#define P2EREPL_CLIENT_NOTIF_DATA_BEGIN          9
#define P2EREPL_CLIENT_NOTIF_DATA_END            10
#define P2EREPL_CLIENT_NOTIF_CLEAR_OLD_REV       11
#define P2EREPL_CLIENT_NOTIF_LIFE_NUM_CHANGED	 12
#define P2EREPL_CLIENT_NOTIF_RECORD				 13

//////////////////////////////////////////////////////////////////////

// поле сообщения в нотификациях NOTIF_CLOSED_BY_SERVER, NOTIF_ONLINE и NOTIF_CLOSE_ON_COMPLETE
#define P2EREPL_NOTIF_MSG_NEXT_REVISION     "eP2ReplNextRev"
#define P2EREPL_NOTIF_MSG_LAST_REVISION     P2EREPL_NOTIF_MSG_NEXT_REVISION // for compatibility
// поле сообщения в нотификации P2EREPL_CLIENT_NOTIF_LIFE_NUM_CHANGED, P2EREPL_CLIENT_NOTIF_OPENED
#define P2EREPL_NOTIF_MSG_SERVER_LIFENUM    "eP2ReplServerLifenum"
// поле сообщения в нотификации P2EREPL_CLIENT_NOTIF_CLOSED_BY_SERVER
#define P2EREPL_NOTIF_MSG_REDIRECT_ADDRESS  P2EREPL_PROP_REDIRECT_ADDRESS
// поля сообщения в нотификации P2EREPL_CLIENT_NOTIF_CLEAR_OLD_REV
#define P2EREPL_NOTIF_MSG_CLEAR_REVISION    "eP2ReplClearRev"
#define P2EREPL_NOTIF_MSG_CLEAR_FLAGS       "eP2ReplClearFlags"

//////////////////////////////////////////////////////////////////////
#define P2EREPL_OPEN_SEND_TIMEOUT         60000
#define P2EREPL_PENDING_NOTIF_TIMEOUT  	  50

//////////////////////////////////////////////////////////////////////

P2EREPLCLIENT_API P2ERR P2CALLSPEC P2eReplStartup(void);
P2EREPLCLIENT_API void  P2CALLSPEC P2eReplCleanup(void);

P2EREPLCLIENT_API P2ERR P2CALLSPEC P2eReplCreateDatastream(P2EREPL_DATASTREAM_HANDLE* pHandle);
P2EREPLCLIENT_API P2ERR P2CALLSPEC P2eReplFreeDatastream(P2EREPL_DATASTREAM_HANDLE handle);

P2EREPLCLIENT_API P2ERR P2CALLSPEC P2eReplOpenDatastream(
    P2EREPL_DATASTREAM_HANDLE      stream,     // [req.] datastream handle
    P2MQ_CONN_HANDLE               conn,       // [req.] MQ connection handle
    PCSTR                          name,       // [req.] name of requested replication datastream (== service name)
    UINT32                         type,       // [req.] datastream type
    P2EREPL_DATA_CALLBACK          cbProc,     // [req.]
    P2EREPL_DATA_FAST_CALLBACK     cbFastProc, // [req.] for frequent notifications
    void*                          cbParams,   // [opt.]
    P2EREPL_REVISION_TYPE          rev,        // [opt.]
    UINT32                         lifeNum     // [opt.]
);

P2EREPLCLIENT_API P2ERR P2CALLSPEC P2eReplCloseDatastream(P2EREPL_DATASTREAM_HANDLE stream);

P2EREPLCLIENT_API P2ERR P2CALLSPEC P2eReplRequestDatastream(P2EREPL_DATASTREAM_HANDLE stream, P2EREPL_REVISION_TYPE rev);

P2EREPLCLIENT_API P2ERR P2CALLSPEC P2eReplGetDatastreamState(P2EREPL_DATASTREAM_HANDLE stream, UINT32* pState);
P2EREPLCLIENT_API P2ERR P2CALLSPEC P2eReplGetServerAddress(P2EREPL_DATASTREAM_HANDLE stream, char* pAddr, /*in,out*/size_t* pAddrSize);

P2EREPLCLIENT_API BOOL P2CALLSPEC P2eReplProcessMessage(P2MQ_CONN_HANDLE conn, P2MSG_HANDLE msg);


// ниже описывается API для создания кастомных стораджей
P2EREPLCLIENT_API P2ERR P2CALLSPEC P2eReplOpenDatastreamWithCustomStorage(
    P2EREPL_DATASTREAM_HANDLE   stream,     // [req.] datastream handle
    P2MQ_CONN_HANDLE            conn,       // [req.] MQ connection handle
    PCSTR                       name,       // [req.] name of requested replication datastream (== service name)
    UINT32                      type,       // [req.] datastream type
    IeReplClientStorage         *pCustomStorage, // [req.] custom storage interface
    UINT32                      lifeNum     // [opt.]
);

P2EREPLCLIENT_API P2ERR P2CALLSPEC P2eReplOpenDatastreamVA(
    P2EREPL_DATASTREAM_HANDLE       stream,     // [req.] datastream handle
    P2MQ_CONN_HANDLE                conn,       // [req.] MQ connection handle
    PCSTR                           name,       // [req.] name of requested replication datastream (== service name)
    UINT32                          type,       // [req.] datastream type
    P2EREPL_VA_DATA_CALLBACK        cbProc,     // [req.]
    void*                           cbParams,   // [opt.]
    P2EREPL_REVISION_TYPE           rev,        // [opt.]
    UINT32                          lifeNum     // [opt.]
);

// Функция устанавливает дополнительную пользовательскую информацию, которая будет передана на сервер в пакете OPEN
P2EREPLCLIENT_API P2ERR P2CALLSPEC P2eReplSetExtraInfo(P2EREPL_DATASTREAM_HANDLE stream, const char * pExtraInfoStr);


//
// Расширенный формат описания имени потока, позволяющий указывать тип потока: {<hex>}<string>.
// Пример: "{0x202}fo2gen" -- поток 'fo2gen' c поддержкой мультикаста (0x200) в режиме ONLINE_ONLY (0x02).
// P2eReplClient по умолчанию обрабатывает указание транспорта (0x200) в строке, добавляя тип транспорта к заданному в функции P2eReplOpenDatastream*** типу.

P2EREPLCLIENT_API P2ERR P2eReplConvertStringToDatastreamName(const char** pname,  // [req.] [in, out], in: строка в расширенном формате, out: имя потока,
															 UINT32* ptype);      // [req.] [out],     тип потока.



#ifdef  __cplusplus
}
#endif







