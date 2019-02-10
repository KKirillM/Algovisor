#ifndef _P2REPLCLIENT_H_
#define _P2REPLCLIENT_H_


#ifdef P2REPLCLIENT_EXPORTS
#define P2REPLCLIENT_API P2_API_EXPORTS
#else
#define P2REPLCLIENT_API P2_API_IMPORTS
#endif

#ifdef _WIN32
#include <BaseTsd.h> // for IntToPtr macro
#endif
#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II/P2SysLog/Include/P2SysLog.h"
#include "../../../Plaza-II/P2SysMsg/Include/P2SysMsg.h"
#include "../../../Plaza-II-MQ/P2MQ/Include/P2MQ.h"
#include "../../P2ReplCommon/Include/P2Repl.h"
#include "../../../Plaza-II-Repl/P2ReplUtils/Include/P2ReplComm.h"
#include "../../../Plaza-II-DB/P2DB/Include/P2DB.h"
#include "../../../Plaza-II-DB/P2Tbl/Include/P2Tbl.h"

#ifdef  __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////

#define P2REPL_CLIENT_PROTOCOL_VERSION  15

//////////////////////////////////////////////////////////////////////

#define P2REPL_OPEN_SEND_TIMEOUT         60000
#define P2REPL_SETSCHEME_SEND_TIMEOUT    60000
#define P2REPL_PENDING_NOTIF_TIMEOUT     50

//////////////////////////////////////////////////////////////////////
// ��� ���������� ���� ���������� � �����, ���������� ��� ���� � �������� ��� ����������
#define P2_STAT_TIME_FIELD_NAME "REC_TIME_FIELD"
#define P2_STAT_LOC_TIME_FIELD_NAME "LocalTimeField"
//////////////////////////////////////////////////////////////////////

#define P2_STAT_HIST_STEP_SIZE "histStepSize"
#define P2_STAT_HIST_STEP_NUM "histStepNum"
#define P2_STAT_HIST_LOC_STEP_SIZE "histLocStepSize"
#define P2_STAT_HIST_LOC_STEP_NUM "histLocStepNum"

typedef void* P2REPL_DATASTREAM_HANDLE;

//////////////////////////////////////////////////////////////////////

#define P2REPL_DATASTREAM_STATE_CLOSED              0
#define P2REPL_DATASTREAM_STATE_ERROR               1     // stream broken, for use it must be reopened
#define P2REPL_DATASTREAM_STATE_OPENING             10
#define P2REPL_DATASTREAM_STATE_OPENED              15
#define P2REPL_DATASTREAM_STATE_SETTING_SCHEME      20
#define P2REPL_DATASTREAM_STATE_READY               25    // ready for request data
#define P2REPL_DATASTREAM_STATE_RECEIVING_SNAPSHOT  30    // getting snapshot data
#define P2REPL_DATASTREAM_STATE_RECEIVING_ONLINE    35    // requested snapshot received

//////////////////////////////////////////////////////////////////////

typedef void (P2CALLSPEC *P2REPL_DATA_CALLBACK)(P2DB_RECORD_HANDLE rec, size_t tblIdx, P2MSG_HANDLE msg, void* cbParams);
typedef void (P2CALLSPEC *P2REPL_ISAM_DATA_CALLBACK)(const void* rec, size_t tblIdx, P2MSG_HANDLE msg, void* cbParams);

/*

Callback function parameters values

    Event       | rec           | tblIdx        | msg           | cbParams
    -----------------------------------------------------------------------------
    notification| 0             | 0             | notif_msg     | cbParams
    data        | data_rec      | tbl_idx       | 0             | cbParams
	preapply	| data_rec		| tbl_idx		| notif_msg		| cbParams

    where
        notif_msg:
            category = P2REPL_CLIENT_TO_SERVER_CATEGORY_SYSTEM  // req.
            type = <notification id>                            // req.
            body:                                               // req.
                for Event == P2REPL_CLIENT_NOTIF_CLEAR_DELETED:
                    <composed revisions for delete>
                for any other Events:
                    <P2ERR>
                        P2ERR_OK - on success
                        <error code> - on any error
*/
/*
�����������. ������. ��������� P2REPL_CLIENT_TO_SERVER_CATEGORY_SYSTEM � ���������� �������� ��������� ��� �����.
������ - ����� �� ������, �� ������ �������� ��� ��� �������������.

*/

//
#define P2REPL_CLIENT_NOTIF_UNDEFINED           0
#define P2REPL_CLIENT_NOTIF_OPENED              1
#define P2REPL_CLIENT_NOTIF_OPEN_FAILED         2
#define P2REPL_CLIENT_NOTIF_SCHEME_IS_SET       3
#define P2REPL_CLIENT_NOTIF_SCHEME_SET_FAILED   4
#define P2REPL_CLIENT_NOTIF_ONLINE              5
//#define P2REPL_CLIENT_NOTIF_CLOSED              6
#define P2REPL_CLIENT_NOTIF_CLOSED_BY_SERVER    7
#define P2REPL_CLIENT_NOTIF_ERROR               8
#define P2REPL_CLIENT_NOTIF_TIMEOUT             9
#define P2REPL_CLIENT_NOTIF_REQUEST_FAILED      10
// ��� �������� datastream-�, ��������� � ����� SNAPSHOT_ONLY, ����� ����� ������ ����� ��������� ��������� SNAPSHOTa
#define P2REPL_CLIENT_NOTIF_CLOSE_ON_COMPLETE   11
#define P2REPL_CLIENT_NOTIF_DB_WILL_BE_DELETED  12
#define P2REPL_CLIENT_NOTIF_DATA_BEGIN          13
#define P2REPL_CLIENT_NOTIF_DATA_END            14
#define P2REPL_CLIENT_NOTIF_CLEAR_DELETED       15
#define P2REPL_CLIENT_NOTIF_LIFE_NUM_CHANGED	16
#define P2REPL_CLIENT_NOTIF_TABLE_EOF			17
#define P2REPL_CLIENT_NOTIF_PREAPPLY_REC		18
// ����������� ������ ��� var arg �������!
#define P2REPL_CLIENT_NOTIF_RECORD				19

//////////////////////////////////////////////////////////////////////

#define P2REPL_DATASTREAM_TYPE_SNAPSHOT				0x0000  // (def.)
#define P2REPL_DATASTREAM_TYPE_ONLINE				0x0001
#define P2REPL_DATASTREAM_TYPE_ONLY_ONLINE			0x0002
#define P2REPL_DATASTREAM_TYPE_CONDITIONAL_ONLINE	0x0004
#define P2REPL_DATASTREAM_TYPE_AUTO_REQUEST			0x0000  // (def.)
#define P2REPL_DATASTREAM_TYPE_MANUAL_REQUEST		0x0100
#define P2REPL_DATASTREAM_TYPE_AUTO_SET_SCHEME		0x0000  // (def.)
#define P2REPL_DATASTREAM_TYPE_MANUAL_SET_SCHEME	0x0200
#define P2REPL_DATASTREAM_TYPE_DONT_CREATE_DB		0x0400  // use only with P2ReplOpenDatastreamDB()
#define P2REPL_DATASTREAM_TYPE_REMOVE_DELETED		0x0800  // use only with P2ReplOpenDatastreamDB()
#define P2REPL_DATASTREAM_TYPE_WITH_TRANSACTIONS	0x0000  // (def.)
#define P2REPL_DATASTREAM_TYPE_WITHOUT_TRANSACTIONS	0x1000  // use only with P2ReplOpenDatastreamDB()
#define P2REPL_DATASTREAM_TYPE_WITH_PREPARED		0x0000  // (def.)
#define P2REPL_DATASTREAM_TYPE_WITHOUT_PREPARED		0x2000  // use only with P2ReplOpenDatastreamDB()
#define P2REPL_DATASTREAM_TYPE_WITH_PREAPPLY		0x4000  // use only with P2ReplOpenDatastreamDB()
#define P2REPL_DATASTREAM_TYPE_WITHOUT_PREAPPLY		0x0000  // (def.)
#define P2REPL_DATASTREAM_TYPE_STRICT_INSERT		0x8000	// use only with P2ReplOpenDatastreamDB()
// ���� - ������ ������ �������� ��������� ������ � �������� �������.
// �������. � ������ ��������� 14 ��������� ������ �������� �������� ������ � ������ �������.
#define P2REPL_DATASTREAM_TYPE_REMOVE_DELETED_EX	0x20000

#define P2REPL_DATASTREAM_TYPE_KEEP_HISTORY			0x40000

//////////////////////////////////////////////////////////////////////
// general functions

P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplStartup(void);
P2REPLCLIENT_API void  P2CALLSPEC P2ReplCleanup(void);

//////////////////////////////////////////////////////////////////////
// client functions
// to be added:
// - open datastream using SQL-like conditions for data filtration 
//   (first of all for revisions: WHERE rev>N)
// - connect datastream to client's DB

P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplCreateDatastream(P2REPL_DATASTREAM_HANDLE* pHandle);
P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplFreeDatastream(P2REPL_DATASTREAM_HANDLE handle);

//client without database
P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplOpenDatastream(
    P2REPL_DATASTREAM_HANDLE    stream,     // [req.] datastream handle
    P2MQ_CONN_HANDLE            conn,       // [req.] MQ connection handle
    PCSTR                       name,       // [req.] name of requested replication datastream (== service name)
    UINT32	                    type,       // [req.] datastream type
    P2REPL_DATA_CALLBACK        cbProc,     // [req.]
    void*                       cbParams,   // [opt.]
    P2DB_SCHEME_HANDLE          custScheme, // [opt.]
    const P2REPL_REVISION_TYPE* pRevisions, // [opt.]
    size_t                      rev_count   // [opt.]
);

#define P2REPL_USE_SERVER_SCHEME (NULL)
#define P2REPL_RESUME_REPLICATION (static_cast< P2DB_SCHEME_HANDLE >(IntToPtr(-1)))


/**
* P2ReplOpenDatastreamEx - ������������� ���������� � �������� ����������. ���������� ��������� ���������� ������ ����������� �� ����������.
*
* ������� ��������� � ���������� �������� ���� ������ ("���� ���������"), ���������� � ������� ������� P2ReplClientGetResumeData,
* � ������������ ��� ����������� ���������� � ����� ����������� ����������.
* @see http://jira:8090/pages/viewpage.action?pageId=1212490

* @param stream [req] ����� ������ ������.
*
* @param conn [req] ����� MQ-����������.
*
* @param name [req] ��� ������������ ��������������� ������ ������ (��� �������).
*
* @param type [req] �������� ������ ������.
*
* @param cbProc [req] ��������� �� callback-�������.
*
* @param cbParams [opt] ��������, ������������ � callback-�������..
*
* @param custScheme [opt] ����� ���������������� ����� ����������.
*      ����� ���� �������� ��������� P2REPL_RESUME_REPLICATION ��� ����������� ���������� �� ����� �� "����� ���������" .
*
* @param pPrevStateData [opt] ��������� �� ���� ������ "����� ���������".
*
* @param prevStateDataSz [opt] ������ ����� ������ "����� ���������".
*
* @returns ��� ������.
*/
P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplOpenDatastreamEx(
    P2REPL_DATASTREAM_HANDLE    stream,     // [req.] datastream handle
    P2MQ_CONN_HANDLE            conn,       // [req.] MQ connection handle
    PCSTR                       name,       // [req.] name of requested replication datastream (== service name)
    UINT32	                    type,       // [req.] datastream type
    P2REPL_DATA_CALLBACK        cbProc,     // [req.]
    void*                       cbParams,   // [opt.]
    P2DB_SCHEME_HANDLE          custScheme, // [opt.]
    const void*                 pPrevStateData, // [opt.]
    size_t                      prevStateDataSz // [opt.]
);

//client with DB connection opened by himself
P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplOpenDatastreamDB(
    P2REPL_DATASTREAM_HANDLE    stream,     // [req.] datastream handle
    P2MQ_CONN_HANDLE            conn,       // [req.] MQ connection handle
    PCSTR                       name,       // [req.] name of requested replication datastream (== service name)
    UINT32	                    type,       // [req.] datastream type
    PCSTR                       dbConnStr,  // [req.] DB-connection string
    P2REPL_DATA_CALLBACK        cbProc,     // [req.]
    void*                       cbParams,   // [opt.]
    P2DB_SCHEME_HANDLE          custScheme  // [opt.]
);

//client with existed DB connection
P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplOpenDatastreamDBConn(
    P2REPL_DATASTREAM_HANDLE    stream,     // [req.] datastream handle
    P2MQ_CONN_HANDLE            conn,       // [req.] MQ connection handle
    PCSTR                       name,       // [req.] name of requested replication datastream (== service name)
    UINT32	                    type,       // [req.] datastream type
    P2DB_CONN_HANDLE            dbConn,		// [req.] DB-connection handle
    P2REPL_DATA_CALLBACK        cbProc,     // [req.]
    void*                       cbParams,   // [opt.]
    P2DB_SCHEME_HANDLE          custScheme  // [opt.]
);



//client with existed ISAM connection
P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplOpenDatastreamISAMConn(
    P2REPL_DATASTREAM_HANDLE    stream,     // [req.] datastream handle
    P2MQ_CONN_HANDLE            conn,       // [req.] MQ connection handle
    PCSTR                       name,       // [req.] name of requested replication datastream (== service name)
    UINT32	                    type,       // [req.] datastream type
    P2TBL_CONN_HANDLE           tblConn,		// [req.] DB-connection handle
    P2REPL_ISAM_DATA_CALLBACK   cbProc,     // [req.]
    void*                       cbParams,   // [opt.]
    P2DB_SCHEME_HANDLE          custScheme  // [opt.]
);

P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplCloseDatastream(P2REPL_DATASTREAM_HANDLE stream);

P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplSetDatastreamScheme(P2REPL_DATASTREAM_HANDLE stream, P2DB_SCHEME_HANDLE scheme);
P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplRequestDatastream(P2REPL_DATASTREAM_HANDLE stream, const P2REPL_REVISION_TYPE* pRevisions, size_t rev_count);
P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplRequestDatastreamDB(P2REPL_DATASTREAM_HANDLE stream);

P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplGetDatastreamScheme(P2REPL_DATASTREAM_HANDLE stream, P2DB_SCHEME_HANDLE* pScheme);
P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplGetDatastreamSrvScheme(P2REPL_DATASTREAM_HANDLE stream, P2DB_SCHEME_HANDLE* pSrvScheme);
P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplGetDatastreamState(P2REPL_DATASTREAM_HANDLE stream, UINT32* pState);
P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplGetDBConnection(P2REPL_DATASTREAM_HANDLE stream, P2DB_CONN_HANDLE* pDbConn);


/* 	�������� mq ����� ������� ����������.
* 	������� ��������� �������� ��� ���������� ������ � ���������� P2REPL_DATASTREAM_STATE_OPENED � ����, �.�. ����� �������� ������.
*/ 
P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplGetServerAddress(P2REPL_DATASTREAM_HANDLE stream, char* pAddr, /*in,out*/size_t* pAddrSize);

P2REPLCLIENT_API BOOL P2CALLSPEC P2ReplProcessMessage(P2MQ_CONN_HANDLE conn, P2MSG_HANDLE msg);

P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplSetFilter(P2REPL_DATASTREAM_HANDLE handle, size_t count, size_t* tableNos, const char** filters);
P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplSetDBBusyRetryOpt(UINT32 count, UINT32 timeout);


// ���� ����������� API ��� �������� ��������� ���������
struct IReplClientStorage;

P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplOpenDatastreamWithCustomStorage(
    P2REPL_DATASTREAM_HANDLE    stream,     // [req.] datastream handle
    P2MQ_CONN_HANDLE            conn,       // [req.] MQ connection handle
    PCSTR                       name,       // [req.] name of requested replication datastream (== service name)
    UINT32	                    type,       // [req.] datastream type
    P2DB_SCHEME_HANDLE          custScheme, // [opt.]
	IReplClientStorage			*pCustomStorage // [req.] custom storage interface
);

// ����� ������� �� P2ReplUtils
// ������� ������� � ���������, �������� rev
P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplClientClearOldRev(P2DB_CONN_HANDLE conn, P2DB_TABLE_SCHEME_HANDLE tblSch, P2REPL_REVISION_TYPE rev);
// ��������� ���������� ���������� ������� �� �������
P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplClientGetNextTableRevision(P2DB_CONN_HANDLE conn, P2DB_TABLE_SCHEME_HANDLE tblSch, P2REPL_REVISION_TYPE *pRev);
/* P2ReplClientInsertOrUpdatePreparedRecord - �����������, �������������� � �������
		P2DB_CONN_HANDLE conn - ����� ����
		P2DB_RECORD_HANDLE origRec - ������������ ������, ���������� �� P2DBNewRecord
		P2DB_RECORD_HANDLE insRec - ����������� ��� ������� ������, ��������� � ��� �� ��������. ���� ���, �� ���������� NULL
		P2DB_RECORD_HANDLE updRec - ����������� ��� ������� ������, ��������� � ��� �� ��������. ���� ���, �� ���������� NULL
		P2REPL_REVISION_TYPE *pNewRev - [out] ��������, �������� ����� ������
		char *primaryTblName - ��� �������. ��������� ��� �������� �� ������. ����� �������� NULL, ����� ����-���� ���������
		bool removeDeleted - ����� �� ������� ��������� ��������� ������ ( ���� ���� action != 0)
		UINT32 flag - P2REPL_POST_CHECK/INS/UPD - �����, ����������� ���������� ��������� �������� (�� ����� �������� ������� ������ �� Id). ���� ���������� ���� �� ������ � ����, ����� ������� P2REPL_POST_CHECK
*/
P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplClientInsertOrUpdatePreparedRecord(P2DB_CONN_HANDLE conn, P2DB_RECORD_HANDLE origRec, P2DB_RECORD_HANDLE insRec, P2DB_RECORD_HANDLE updRec, P2REPL_REVISION_TYPE *pNewRev, char *primaryTblName, bool removeDeleted, UINT32 flag);


#ifndef P2REPL_DBSCHEME_TABLE_NAME
// ������ ��� ��� �� $\Projects\Units\Plaza-II-Repl\P2ReplUtils\Include\P2ReplUtils.h  
#define P2REPL_DBSCHEME_TABLE_NAME  "P2ReplDbScheme"
#define P2REPL_DBSCHEME_TABLE_NAME2 "P2ReplDbSchemeV2"
#endif

#ifndef P2REPL_POST_CHECK
// ������ ��� ��� �� $\Projects\Units\Plaza-II-Repl\P2ReplUtils\Include\P2ReplDS.h
#define P2REPL_POST_CHECK	0
#define P2REPL_POST_INS		1
#define P2REPL_POST_UPD		2
#endif

#ifndef P2REPL_SNAPSHOT_SERVER_SUFFIX
#define P2REPL_SNAPSHOT_SERVER_SUFFIX "_SNAPSHOT"
#endif

// ���� � ��������� ����������� P2REPL_CLIENT_NOTIF_CLOSED_BY_SERVER
#define P2REPL_LAST_REVISIONS "P2ReplRevs"

// �������� ������ � ��������������� ���������
P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplOpenDatastreamDBWithRevisions(
    P2REPL_DATASTREAM_HANDLE    stream,     // [req.] datastream handle
    P2MQ_CONN_HANDLE            conn,       // [req.] MQ connection handle
    PCSTR                       name,       // [req.] name of requested replication datastream (== service name)
    UINT32	                    type,       // [req.] datastream type
    PCSTR                       dbConnStr,  // [req.] DB-connection string
    P2REPL_DATA_CALLBACK        cbProc,     // [req.]
    void*                       cbParams,   // [opt.]
    P2DB_SCHEME_HANDLE          custScheme, // [opt.]
    const P2REPL_REVISION_TYPE* pRevisions, // [opt.]
    size_t						rev_count   // [opt.]
);

P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplReadStateBlobFromDB(P2DB_CONN_HANDLE hDbConn, const char *pTableName, const char* pStreamName, void* pBuffer, size_t* pBufferSz);
P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplReadStateBlobFromDB2(const char *pConnStr, const char *pTableName, const char* pStreamName, void* pBuffer, size_t* pBufferSz);
P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplSaveStateBlobToDB(P2DB_CONN_HANDLE hDbConn, const char *pTableName, const char* pStreamName, const void* pBuffer, size_t bufferSz);
P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplDeleteStateBlobFromDB(P2DB_CONN_HANDLE hDbConn, const char *pTableName, const char* pStreamName);
P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplDeleteStateBlobFromDB2(const char *pConnStr, const char *pTableName, const char* pStreamName);

/**
* P2ReplGetResumeData - �������� �������� ���� ������, ������� ����� ���� ����������� ��� ����������� ����������.
*
* @see http://jira:8090/pages/viewpage.action?pageId=1212490
* ������� ������ ���������� ��� ��������� ����� �������� ������, ������� ������������ ��� ������ ���������� � ����� ����������� ����������.
* ���� ���� ������ ����� ���� ������� � ������� P2ReplOpenDatastreamEx.
* ����� ����� ��� ���������� �������� ����������.
* ����� ������� ������ ������������� ����� �������� ������ � ��������� P2REPL_DATASTREAM_STATE_CLOSED ��� P2REPL_DATASTREAM_STATE_ERROR,
* � ������ ���������� ������ ����� ���� �����������.
*
* @param stream [req] ����� ������ ������.
*
* @param pBuffer [opt] ��������� �� ���� ������, � ������� ����� �������� ������.
*      ���� ���� �������� NULL, �� �� ������, �� ������� ��������� ���������� pBufferSz ����� ������� ������
*      ����� ������, ����������� ��� ���������� ������ � ��������� ��� ������ P2ERR_COMMON_REAL_SIZE_RETURNED.
*
* @param pBufferSz [req] ��������� �� ����������, ���������� ������ ������.
*      ������� ����� ������� ������ = 0 � ������, ���� ���� ������ �� �������� �������� ���������� ��� ����������� ����������.
*
* @returns ��� ������.
*/
P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplGetResumeData(P2REPL_DATASTREAM_HANDLE stream, void* pBuffer, size_t* pBufferSz);


// 
typedef void (P2CALLSPEC *P2REPL_VA_DATA_CALLBACK)(UINT32 notif, void *cbParams, ...);
/*

cbProc(P2REPL_CLIENT_NOTIF_OPENED, m_cbParams);
cbProc(P2REPL_CLIENT_NOTIF_OPEN_FAILED, m_cbParams, reason);
cbProc(P2REPL_CLIENT_NOTIF_SCHEME_IS_SET, m_cbParams);
cbProc(P2REPL_CLIENT_NOTIF_SCHEME_SET_FAILED, m_cbParams, reason);

cbProc(P2REPL_CLIENT_NOTIF_DATA_BEGIN, m_cbParams);
cbProc(P2REPL_CLIENT_NOTIF_DATA_END, m_cbParams);
cbProc(P2REPL_CLIENT_NOTIF_RECORD, m_cbParams, pRec, tblIdx);

cbProc(P2REPL_CLIENT_NOTIF_LIFE_NUM_CHANGED, m_cbParams);
cbProc(P2REPL_CLIENT_NOTIF_CLEAR_DELETED, m_cbParams, clearParserBody, clearParserSize);
cbProc(P2REPL_CLIENT_NOTIF_ONLINE, m_cbParams, revisions, revisionSize);
cbProc(P2REPL_CLIENT_NOTIF_CLOSED_BY_SERVER, m_cbParams, reason, revisions, revisionSize, redirectServer);
cbProc(P2REPL_CLIENT_NOTIF_CLOSE_ON_COMPLETE, m_cbParams, revisions, revisionSize);
cbProc(P2REPL_CLIENT_NOTIF_DB_WILL_BE_DELETED, m_cbParams);
cbProc(P2REPL_CLIENT_NOTIF_TABLE_EOF, m_cbParams, tableIdx);

cbProc(P2REPL_CLIENT_NOTIF_ERROR, m_cbParams, reason);
cbProc(P2REPL_CLIENT_NOTIF_TIMEOUT, m_cbParams, reason);
cbProc(P2REPL_CLIENT_NOTIF_REQUEST_FAILED, m_cbParams, reason);

cbProc(P2REPL_CLIENT_NOTIF_PREAPPLY_REC, m_cbParams, pRec, tblIdx);

cbProc(notif, m_cbParams); ��������� ����� 
*/


//client without database
P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplOpenDatastreamVA(
    P2REPL_DATASTREAM_HANDLE    stream,     // [req.] datastream handle
    P2MQ_CONN_HANDLE            conn,       // [req.] MQ connection handle
    PCSTR                       name,       // [req.] name of requested replication datastream (== service name)
    UINT32	                    type,       // [req.] datastream type
    P2REPL_VA_DATA_CALLBACK     cbProc,     // [req.]
    void*                       cbParams,   // [opt.]
    P2DB_SCHEME_HANDLE          custScheme, // [opt.]
    const P2REPL_REVISION_TYPE* pRevisions, // [opt.]
    size_t                      rev_count   // [opt.]
);


/**
* P2ReplOpenDatastreamEx - ������������� ���������� � �������� ����������. ���������� ��������� ���������� ������ ����������� �� ����������.
*
* ������� ��������� � ���������� �������� ���� ������ ("���� ���������"), ���������� � ������� ������� P2ReplClientGetResumeData,
* � ������������ ��� ����������� ���������� � ����� ����������� ����������.
* @see http://jira:8090/pages/viewpage.action?pageId=1212490

* @param stream [req] ����� ������ ������.
*
* @param conn [req] ����� MQ-����������.
*
* @param name [req] ��� ������������ ��������������� ������ ������ (��� �������).
*
* @param type [req] �������� ������ ������.
*
* @param cbProc [req] ��������� �� callback-�������.
*
* @param cbParams [opt] ��������, ������������ � callback-�������..
*
* @param custScheme [opt] ����� ���������������� ����� ����������.
*      ����� ���� �������� ��������� P2REPL_RESUME_REPLICATION ��� ����������� ���������� �� ����� �� "����� ���������" .
*
* @param pPrevStateData [opt] ��������� �� ���� ������ "����� ���������".
*
* @param prevStateDataSz [opt] ������ ����� ������ "����� ���������".
*
* @returns ��� ������.
*/
P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplOpenDatastreamVAEx(
    P2REPL_DATASTREAM_HANDLE    stream,     // [req.] datastream handle
    P2MQ_CONN_HANDLE            conn,       // [req.] MQ connection handle
    PCSTR                       name,       // [req.] name of requested replication datastream (== service name)
    UINT32	                    type,       // [req.] datastream type
    P2REPL_VA_DATA_CALLBACK     cbProc,     // [req.]
    void*                       cbParams,   // [opt.]
    P2DB_SCHEME_HANDLE          custScheme, // [opt.]
    const void*                 pPrevStateData, // [opt.]
    size_t                      prevStateDataSz // [opt.]
);

/**
* P2ReplClientGetNextRevision - ������� ���������� ��������� ���������� ������� ��� �������.
*
* ������� ��������� �������� ��� ���������� ������ � ���������� P2REPL_DATASTREAM_STATE_RECEIVING_SNAPSHOT ��� P2REPL_DATASTREAM_STATE_RECEIVING_ONLINE.
* ���������� ������� �� ���������� ����������, �� �� ����.
* �� ������ � ��������� P2ReplClientGetNextTableRevision � P2ReplGetNextTableRevision
*
* @param stream [req] ����� ������ ������.
*
* @param tblIdx [req] ������ �������.
*
* @param pNextRevision [req] ��������� �� ����������, � ������� ����� �������� �������� ��������.
*
* @returns ��� ������.
*/
P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplClientGetNextRevision(P2REPL_DATASTREAM_HANDLE stream, size_t tblIdx, P2REPL_REVISION_TYPE* pNextRevision);

// ������� ������������� �������������� ���������������� ����������, ������� ����� �������� �� ������ � ������ OPEN
P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplSetExtraInfo(P2REPL_DATASTREAM_HANDLE stream, const char * pExtraInfoStr);

/**
* P2ReplClientGetServerFlagsInt - ������� ���������� ����� �������, � �������� ����������� ������.
*
* ������� ��������� �������� ��� ���������� ������ � ���������� P2REPL_DATASTREAM_STATE_OPENED � ����, �.�. ����� �������� ������.
*
* @param stream [req] ����� ������ ������.
*
* @param pServerFlags [req] ��������� �� ����������, � ������� ����� �������� �������� ��������� ������.
*
* @returns ��� ������.
*/
P2REPLCLIENT_API P2ERR P2CALLSPEC P2ReplClientGetServerFlagsInt(P2REPL_DATASTREAM_HANDLE stream, UINT32 * pServerFlags);


#ifdef  __cplusplus
}
#endif

#endif // _P2REPLCLIENT_H_
