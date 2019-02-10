#ifndef _P2ACLREPLCLIENT_H
#define _P2ACLREPLCLIENT_H


#ifdef P2ACLREPLCLIENT_EXPORTS
#define P2ACLREPLCLIENT_API P2_API_EXPORTS
#else
#define P2ACLREPLCLIENT_API P2_API_IMPORTS
#endif


#include "../../../Plaza-II/P2ACL/Include/P2ACL.hpp"
#include "../../../Plaza-II-MQ/P2MQ/Include/P2MQ.h"
#include "../../../Plaza-II-DB/P2DB/Include/P2DB.hpp"

//  Notif table
#define ACL_NOTIF_TABLE_VALID					100
#define ACL_NOTIF_TABLE_INVALID					101
#define ACL_NOTIF_STREAM_RESTART_REQUIRED		102
#define ACL_NOTIF_NEW_DATA						103
#define ACL_NOTIF_ERROR							104


// Acl table state
// we can use acl's
#define ACL_STATE_TABLE_VALID                   1000
// we cannot use acl's
#define ACL_STATE_TABLE_INVALID                 1001
#define ACL_STATE_TABLE_ERROR                   1002 

/*	����������� => ��������� �������
	ACL_NOTIF_TABLE_VALID				=> ACL_STATE_TABLE_VALID
	ACL_NOTIF_NEW_DATA					=> ACL_STATE_TABLE_VALID
	ACL_NOTIF_STREAM_RESTART_REQUIRED	=> ACL_STATE_TABLE_INVALID
	ACL_NOTIF_TABLE_INVALID             => ACL_STATE_TABLE_INVALID
    ACL_NOTIF_ERROR                     => ACL_STATE_TABLE_ERROR 
*/

class CACLStream;
typedef CACLStream* P2ACL_REPL_HANDLE;

/*
	���������: 
	  (�������������� ���������)
	   ACL_NOTIF_TABLE_INVALID
	   ACL_NOTIF_STREAM_RESTART_REQUIRED
	   ACL_NOTIF_ERROR
	   stream -- NULL, ACL -- NULL.
	   -----------------------------------------------------------------------------
       (����� ������ �� ACL ��� ������ stream ��� ������ � ������ stream �����������,
	   (� ���� ������ ��� ����� ��� stream �������))

	   ACL_NOTIF_NEW_DATA   : ��� ����� ������ � �������
	   ACL_NOTIF_TABLE_VALID: ��� ��������� �������
	   stream != NULL -- ��� stream, ACL -- ACL 
       
*/

typedef P2ERR (*P2_ACL_REPL_CALLBACK)( UINT32 notif,
									   const char* stream,
									   const P2ACL_HANDLE,
									   void* Param); 

#ifdef  __cplusplus
extern "C" {
#endif

P2ACLREPLCLIENT_API P2ERR P2CALLSPEC P2ReplACLFree(P2ACL_REPL_HANDLE handle);
P2ACLREPLCLIENT_API P2ERR P2CALLSPEC P2ReplACLCreate(P2ACL_REPL_HANDLE* handle);

/*  ���������
	handle,
	conn    -- ���������� � ��������,
	streams -- ����� �������,����������� ';', ��� ������� ����� ���������� ����������� �� ACL

    dbConnStr ��� dbConn   	-- ���������� � �� � ������� ����� ���� acl ����������
	cbProc   				-- ������� ����� ������� ���������� �����������
	cbParam  				-- �������� ��� cbProc
*/

P2ACLREPLCLIENT_API P2ERR P2CALLSPEC P2ReplACLOpen(
										P2ACL_REPL_HANDLE handle,
										P2MQ_CONN_HANDLE conn,
										const char* streams,			
										PCSTR dbConnStr,
										P2_ACL_REPL_CALLBACK cbProc,
										void* cbParam );

P2ACLREPLCLIENT_API P2ERR P2CALLSPEC P2ReplACLOpenDBConn(	
													P2ACL_REPL_HANDLE handle,
													P2MQ_CONN_HANDLE conn,
													const char* streams,			
													P2DB_CONN_HANDLE dbConn,
													P2_ACL_REPL_CALLBACK cbProc,
													void* cbParam);

P2ACLREPLCLIENT_API P2ERR P2CALLSPEC P2ReplACLClose(P2ACL_REPL_HANDLE handle);

/* returns ACL_TABLE_STATE_xxx constant */
P2ACLREPLCLIENT_API UINT32 P2CALLSPEC P2ReplACLGetTableState(P2ACL_REPL_HANDLE handle);

/* ������������� ��������� ���������� �����
   ��������� ����� �������������� � �-�� OpenReplication
	
   aclStream -- ��� ������, (by def. "ReplBLTSRights")
   aclTable -- 	��� ������� (by def. "WorkstationRights") */

P2ACLREPLCLIENT_API void 	P2CALLSPEC 	P2REPLACLSetParameters(P2ACL_REPL_HANDLE hACLStream, const char* pACLStream, const char* pACLTable); 
P2ACLREPLCLIENT_API P2ERR 	P2CALLSPEC 	P2ReplACLGetParameters(P2ACL_REPL_HANDLE hACLStream, char *pACLStream, size_t *pACLStreamSz, char *pACLTable, size_t *pACLTableSz); 
P2ACLREPLCLIENT_API P2ERR 	P2CALLSPEC 	P2ReplACLGetACLScheme(P2ACL_REPL_HANDLE handle, P2DB_SCHEME_HANDLE* psch);

#ifdef  __cplusplus
}
#endif

#endif
