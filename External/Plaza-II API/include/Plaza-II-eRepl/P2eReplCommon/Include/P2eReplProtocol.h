#ifndef _P2EREPLPROTOCOL_H_
#define _P2EREPLPROTOCOL_H_

#include "./P2eReplTypes.h"

// �������� ��������� ����������
/* 
  ������ ��������� ��������������� � ���� �����������.
  ������, �������������� �������� � �������� ��������� � ���� #define � 
  ��������������� ��������� ������� 

  ���������� � ������� ��������� ��������� �������:
	
	Cli---Svr
	->OPEN(cli_ver)
	<-OPEN_ANS(ser_ver) ��� OPEN_FAIL(incompatible version)
	->DATA_REQUEST... ��� CLOSE(incompatible version)

  ������ ������������ �� ������ ����, � ������ �� ������.

  �������:
	->OPEN(1)
	<-OPEN_ANS(1) // ���� ���� ������� ������ � ������� 3, � ������ 1 �������������� ��������
	->DATA_REQUEST...
	
	->OPEN(1)
	<-OPEN_FAIL(incompatible version) // � ��� ������, ���� ������ ������������ ������ ������� �� 2-��

	->OPEN(3)
	<-OPEN_ANS(1) // ���� ������ ������, �� �� ����� ����������� �� ��������� � �������
	->CLOSE(incompatible version) // ���� ������ �� �������� ������ ������

	->OPEN(3)
	<-OPEN_ANS(1) // ���� ������ ������, �� �� ����� ����������� �� ��������� � �������
	->DATA_REQUEST... // ���� ������ �������� ������ ������
*/

/*
����� ���� (���������� � ������ ���������):
===============================================
P2MSG_PROP_CATEGORY         = P2EREPL_CLIENT_TO_SERVER_CATEGORY_SYSTEM / P2EREPL_SERVER_TO_CLIENT_CATEGORY_SYSTEM


-----------------------------------------------
OPEN (cli, send)
-----------------------------------------------
P2MSG_PROP_TYPE             = P2EREPL_PKT_OPEN
P2MSG_PROP_SERVICE_TO       = [c]<stream name>
P2EREPL_PROP_VERSION        = [u4]<client_version>


-----------------------------------------------
OPEN_ANS (srv, reply)
-----------------------------------------------
P2MSG_PROP_TYPE             = P2EREPL_PKT_OPEN_ANS
P2EREPL_PROP_VERSION        = [u4]<server_version>
P2EREPL_PROP_CHANNEL_INFO   = [u8]<new_channel_id>[u4]<session_id = 0>[u4]<session_order_num = 0>
[opt]P2EREPL_PROP_TOKEN     = [c]<server_token>
P2MSG_PROP_BODY             = [u4]<life_number>


-----------------------------------------------
OPEN_FAIL_ANS (srv, reply)
-----------------------------------------------
P2MSG_PROP_TYPE             = P2EREPL_PKT_OPEN_FAIL_ANS
P2MSG_PROP_BODY             = [u4]<p2err_reason>


-----------------------------------------------
DATA_REQUEST (cli, post)
-----------------------------------------------
P2MSG_PROP_TYPE             = P2EREPL_PKT_DATA_REQUEST
P2EREPL_PROP_CHANNEL_INFO   = [u8]<channel_id>[u4]<session_id>[u4]<session_order_num>
                                session_order_num = * ��� (session order num of DATA with ans_req ���� ������ ���� P2EREPL_FLAG_C2S__DATAREQ_DATA_ACK)
[opt]P2MSG_PROP_BODY        = [i8]<������� ������� ������� + 1> (���� ������ ����� �������� ������ � ������, �� ����� ������� ������ composer)
[opt]P2EREPL_PROP_FLAGS     = [u4]


-----------------------------------------------
DATA (srv, post)
-----------------------------------------------
P2MSG_PROP_TYPE             = P2EREPL_PKT_DATA
P2EREPL_PROP_CHANNEL_INFO   = [u8]<new channel_id>[u4]<session id>[u4]<++session_order_num = 0>
P2EREPL_PROP_DATA_INFO      = [composer:i8u4u4u4]<P2EREPL_FORMAT__DATA_INFO>
P2MSG_PROP_BODY             = [b]<compressed records>
[opt]P2EREPL_PROP_FLAGS     = [u4]


-----------------------------------------------
PING (cli, post)
// ������������ �������� � ������� ������� ������ (��������� channel_id)
-----------------------------------------------
P2MSG_PROP_TYPE             = P2EREPL_PKT_PING
P2EREPL_PROP_CHANNEL_INFO   = [u8]<channel_id>[u4]<session_id>[u4]<session_order_num = *>


-----------------------------------------------
PING (srv, post)
// ������������ �������� � ������� �������� ������ �� ������� ������,
// ����� ������� ������ (�������� ������) ������������ P2EREPL_PKT_COUNTED_PING
// ���� �� ��������� ��������� ������� ��� �������� ����������� ������ (�������� �������), 
// ������� ����� ���� ������ ������ ������
-----------------------------------------------
P2MSG_PROP_TYPE             = P2REPL_PKT_PING
P2EREPL_PROP_CHANNEL_INFO   = [u8]<channel_id>[u4]<session_id = *>[u4]<session_order_num = *>


-----------------------------------------------
COUNTED_PING (srv, post)
-----------------------------------------------
P2MSG_PROP_TYPE             = P2EREPL_PKT_COUNTED_PING
P2EREPL_PROP_CHANNEL_INFO   = [u8]<channel id>[u4]<session id>[u4]<session order num of DATA with ans_req>


-----------------------------------------------
CLOSE (cli/srv, post)
-----------------------------------------------
P2MSG_PROP_TYPE             = P2EREPL_PKT_CLOSE
P2EREPL_PROP_CHANNEL_INFO   = [u8]<channel_id>[u4]<session_id = *>[u4]<session_order_num = *>
P2MSG_PROP_BODY             = [u4]<p2err_reason>
[opt]P2EREPL_PROP_REDIRECT_ADDRESS = [c]<redirect_address>


-----------------------------------------------
RECONNECT (srv, post)
-----------------------------------------------
P2MSG_PROP_TYPE             = P2EREPL_PKT_RECONNECT
P2EREPL_PROP_CHANNEL_INFO   = [u8]<channel_id>[u4]<session_id = *>[u4]<session_order_num = *>
[opt]P2EREPL_PROP_FLAGS     = [u4]


-----------------------------------------------
CLEAR_OLD_REVS (srv, post)
-----------------------------------------------
P2MSG_PROP_TYPE             = P2EREPL_PKT_CLEAR_OLD_REVS
P2EREPL_PROP_CHANNEL_INFO   = [u8]<channel id>[u4]<session id>[u4]<session order num = 0>
P2MSG_PROP_BODY             = [i8]<������ ������ ������>
[opt]P2EREPL_PROP_FLAGS     = [u4]
*/


// ���� ������� ���������
enum P2EREPL_PKT
{
	P2EREPL_PKT_OPEN,
	P2EREPL_PKT_OPEN_ANS,
	P2EREPL_PKT_OPEN_FAIL_ANS,
	P2EREPL_PKT_DATA_REQUEST,
	P2EREPL_PKT_DATA,
	P2EREPL_PKT_PING,
	P2EREPL_PKT_COUNTED_PING,
	P2EREPL_PKT_CLOSE,
	P2EREPL_PKT_CLEAR_OLD_REVS,
	_P2EREPL_PKT_UNUSED,
	P2EREPL_PKT_TRANSPORT_ERROR
};

typedef UINT32 P2EREPL_VERSION;
typedef UINT64 P2EREPL_CHANNEL_HANDLE;
typedef UINT32 P2EREPL_SESSION_HANDLE;
typedef UINT32 P2EREPL_ORDER_NUM;


// ������������ ��������� ���������
#define P2EREPL_CLIENT_TO_SERVER_CATEGORY_SYSTEM "eRepl_C2S"
#define P2EREPL_SERVER_TO_CLIENT_CATEGORY_SYSTEM "eRepl_S2C"


///////////////////////////////////////////////////////////////////
// ������������ �����
// ������ ���������. ������������ � OPEN � OPEN_ANS
#define P2EREPL_PROP_VERSION                "eRepl_Ver"

// ����� �������. ������������ � OPEN_ANS
#define P2EREPL_PROP_TOKEN                  "eRepl_Token"

// ���������� ���� channel_id, session_id, session_order_num
// ������������ �� ���� ����� ��������� ����� OPEN � OPEN_FAIL_ANS.
#define P2EREPL_PROP_CHANNEL_INFO           "eRepl_I"

// ���������� � ����� ������������� ������. ������������ � DATA
#define P2EREPL_PROP_DATA_INFO              "eRepl_DI"

// ���� ��� ������
#define P2EREPL_PROP_FLAGS                  "eRepl_F"

//
#define P2EREPL_PROP_REDIRECT_ADDRESS       "P2_Redirect_Address"

// ����������, ������� ����� ���� ������������ ����� ����������� ������. �������� �� �������, ������� �������� ��� ���������� �  OpenAnswer. ������, 
// �������������� ������ ������������, �������� ��������� �������������� ������.
#define P2EREPL_PROP_GROUP_OPTIONS 			"eRepl_GO" 

// �������� ������ 
#define P2EREPL_PROP_PING_INTERVAL 			"eRepl_PI"

// �������������� ������ ������������ �� ������� �������
#define P2EREPL_EXTRA_INFO                  "eRepl_Extra"

///////////////////////////////////////////////////////////////////

// Flags
// client-to-server
#define P2EREPL_FLAG_C2S__DATAREQ_START_ONLINE	0x1
#define P2EREPL_FLAG_C2S__DATAREQ_DATA_ACK      0x2
#define P2EREPL_FLAG_C2S__OPEN_MULTICAST        0X3
// server-to-client
#define P2EREPL_FLAG_S2C__DATA_EOALL            0x1  // ����� ��������
#define P2EREPL_FLAG_S2C__DATA_ACKREQ           0x2  // ���������� ����������� ��������� ������ ������ P2EREPL_FLAG_C2S__DATA_ACK
#define P2EREPL_FLAG_S2C__DATA_ONLINE           0x10 // Online-������

// ��� ����� ������ ��������� �� �������� � ������ � ������� ��� P2eReplStorage (P2eReplStorage\Include\P2eReplStorage.h)
// P2eReplServer ������ ����������� �� �����������
// Options for P2eReplServerClearOldRevs
#define P2EREPL_FLAG_S2C__CLEAROLDREVS_FAST_CLEAR       0x0000 // P2EREPL_STORAGE_FAST_CLEAR
#define P2EREPL_FLAG_S2C__CLEAROLDREVS_CLEAR_AND_CLEAN  0x0001 // P2EREPL_STORAGE_CLEAR_AND_CLEAN

#define P2EREPL_FORMAT__CHANNEL_INFO    "u8u4u4"

// ������� ��� body
#define P2EREPL_FORMAT__OPEN_ANS        "u4" // server life_number
#define P2EREPL_FORMAT__OPEN_FAIL_ANS   "u4" // ������� ������������� �������� ���������� (P2ERR)
#define P2EREPL_FORMAT__DATA_REQUEST    P2EREPL_REVISION_FIELD_FORMAT
#define P2EREPL_FORMAT__CLOSE           "u4" // ������� �������� ���������� (P2ERR)
#define P2EREPL_FORMAT__CLEAR_OLD_REV   P2EREPL_REVISION_FIELD_FORMAT

// DATA-������
/* i8 - last processed revision
 * u4 - number of composed records
 * u4 - data compression type
 * u4 - size of unpacked data
 */
#define P2EREPL_FORMAT__DATA_INFO       "i8u4u4u4"

#define P2EREPL_FORMAT__VERSION         "u4"
#define P2EREPL_FORMAT__FLAGS           "u4"

// pings
#define P2EREPL_SERVER_TO_CLIENT_DEFAULT_PING_INTERVAL		60000     // �������� �� ��������� ��������� ������ "������ --> ������"
#define P2EREPL_CLIENT_TO_SERVER_PING_INTERVAL				60000     // �������� ������ "������ --> ������"
#define P2EREPL_SKIPPED_PINGS        						3

/*
	��������� ������� �� ������ �� �������� �������� ����� ������������ ���������
	� ������������� 1 ������ ����������������� ���������� �����.
*/
#pragma pack(push, 1)
typedef struct P2EREPL_CHANNEL_INFO_TAG
{
    P2EREPL_CHANNEL_HANDLE  m_channelId;
    P2EREPL_SESSION_HANDLE  m_session;
    P2EREPL_ORDER_NUM       m_order;
} P2EREPL_CHANNEL_INFO;

typedef struct P2EREPL_DATA_INFO_TAG
{
    INT64                   lastRev;
    UINT32                  eventsCount;
    UINT32                  compressionType;
    UINT32                  unpackedSize;
} P2EREPL_DATA_INFO;
#pragma pack (pop)

#endif // _P2EREPLPROTOCOL_H_
