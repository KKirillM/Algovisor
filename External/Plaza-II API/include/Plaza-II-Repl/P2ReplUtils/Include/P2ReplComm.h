#ifndef P2REPL_COMM_H_
#define P2REPL_COMM_H_

#include "../../P2ReplCommon/Include/P2Repl.h"

// �������� ��������� ����������

/* 

  ������ ��������� ��������������� � ���� �����������.
  ������, �������������� �������� � �������� ��������� � ���� #define � 
  ��������������� ��������� ������� 

  ���������� � ������� ��������� ��������� �������:
	
	Cli---Svr
	->OPEN(cli_ver)
	<-OPEN_ANS(ser_ver) ��� OPEN_FAIL(incompatible version)
	->SCHEME... ��� CLOSE(incompatible version)

  ������ ������������ �� ������ ����, � ������ �� �������.

  �������:
	->OPEN(1)
	<-OPEN_ANS(1) // ���� ���� ������� ������ � ������� 3, � ������ 1 �������������� ��������
	->SCHEME..

    ->OPEN(1)
    <-OPEN_FAIL(incompatible version) // � ��� ������, ���� ������ ������������ ������ ������� �� 2-��

	->OPEN(3)
	<-OPEN_ANS(1) // ���� ������ ������, �� �� ����� ����������� �� ��������� � �������
	->CLOSE(incompatible version) // ���� ������ �� �������� ������ ������

	->OPEN(3)
	<-OPEN_ANS(1) // ���� ������ ������, �� �� ����� ����������� �� ��������� � �������
	->SHEME... // ���� ������ �������� ������ ������

  14.01.04 v1
    - ��������� ������
  02.04.04 v2
    - ��������� ���������� ������
    - ������ 1 ������ �� ��������������
  18.05.04 v3
    - ��������� ������� ��������� ������ � �������� ���������
	- ��������� ���������
	- ������ 2 �� ��������������
  16.11.04 v4
	- �������� ������ �� ���������� ������ ����� �������
	- ������ 3 �� ��������������
  24.11.04 v5
    - ��������� ���������� ������� �� �������
	- ������ �������� � �������� 4 � 5
	- ������ �������� ������ � ������� 5

  23.05.2005 v6
	- ��������� ������� ���������� ��� ��������� ������ ����� �����
	- ������ 5 �� ��������������
  25.05.2007 v10
    - �������� � ���������� ������� ���� P2_Repl_Chan ��� �������� channel, session, order
	- ������ �������� � �������� 9 � 10
	- ������ �������� ������ � ������� 10
  30.09.2009
    - �������� ��������� P2REPL_PKT_START_ONLINE ��� ��������� ������ ONLINE ������
	- ������ �������� � �������� 9 � 10
	- ������ �������� ������ � ������� 10
  26.02.2010
	- � ����� � ������� ��������� ����� ���� c ���������� �������������� ��������� �� ��������
  ~  05.2011 v12 (� Plaza-2 v187)
    - �������� ������� � ������ RouterReplica
  06.02.2013 v13 (� Plaza-2 v196)
    - � �����  CLEAR_DELETED ��������� ���� ������
  28.05.2013 v13 (� Plaza-2 v197)
    - � ����� OPEN ��������� ���� � ��������������� ������� �������
  01.10.2013 v13 (� Plaza-2 v197.4)
    - � ����� OPEN_ANS ��������� ���� � ������� �������
  11.04.2014 v14 (� Plaza-2 v199)
    - ��������� FullRecComposing
    - ��������� ������ ������ �������� ���������
  10.09.2015 v14 (� Plaza-2 v204)
    - ����� P2REPL_PKT_RECONNECT �� ����������� �� �������. ������� �� CLOSE � ����� P2ERR_REPL_RECONNECT
  02.11.2015 v14 (� Plaza-2 v204) ���� P2REPL_START_FLAG_SNAPSHOT_SERVER_SUPPORTED ������ �� ������� � �������
  29.07.2016 v15 (Plaza-2   v207) ������ ��������� ���������� � ������ ������� �����2. 
*/

/*

����� ���� (���������� � ������ ���������):
===============================================
category = P2REPL_CATEGORY_SYSTEM

-----------------------------------------------
OPEN (cli, send)
-----------------------------------------------
type = P2REPL_PKT_OPEN
version = [u4]<client version>
service_to = [c]<stream name>
[P2_Repl_Extra] = [c]<client extra info>

-----------------------------------------------
OPEN_ANS (srv, reply)
-----------------------------------------------
type = P2REPL_PKT_OPEN_ANS
version = [u4]<server version>
channel = [u8]<new channel id>
body = [b]<server scheme>
token = [s]<server token>
flags = [u4]<server flags>

-----------------------------------------------
OPEN_ANS (srv, reply) VERSION 10
-----------------------------------------------
type = P2REPL_PKT_OPEN_ANS
version = [u4]<server version>
chan = [u8]<new channel id>[u4]<session id = 0>[u4]<session order num = 0>
body = [b]<server scheme>

-----------------------------------------------
OPEN_FAIL_ANS (srv, reply)
-----------------------------------------------
type = P2REPL_PKT_OPEN_FAIL_ANS
body = [p2err]<reason>

///////////////////////////////////////////////

-----------------------------------------------
SCHEME (cli, send)
-----------------------------------------------
type = P2REPL_PKT_SCHEME
channel = [u8]<channel id>
body = [b]<client scheme>
filter = [composer:"u4c..."]<����� �������, ������ ��� �������>

-----------------------------------------------
SCHEME_ANS (srv, reply)
-----------------------------------------------
type = P2REPL_PKT_SCHEME_ANS
channel = [u8]<channel id>
body = [p2err]<result>

-----------------------------------------------
SCHEME_ANS (srv, reply) VERSION 10
-----------------------------------------------
type = P2REPL_PKT_SCHEME_ANS
chan = [u8]<new channel id>[u4]<session id = 0>[u4]<session order num = 0>
body = [p2err]<result>

///////////////////////////////////////////////

-----------------------------------------------
START_ONLINE (cli, post)
-----------------------------------------------
type = P2REPL_PKT_START_ONLINE
channel = [u8]<channel id>
session = [u4]<new session id>

-----------------------------------------------
START_COMBINE (cli, post)
-----------------------------------------------
type = P2REPL_PKT_START_COMBINE
channel = [u8]<channel id>
session = [u4]<new session id>
body = [composer:"u4..."]<������ ������� �������� ������� + 1>
       (���� ������ ����� �������� ������ � ������, �� ����� ������� ������ composer)
start_flag = u4 ������������! ���� ������ ���� HANDLE_SNAPSHOT_SESSION

-----------------------------------------------
DATA (srv, post)
-----------------------------------------------
type = P2REPL_PKT_DATA
channel = [u8]<channel id>
session = [u4]<session id>
count = [u4]<++session order num = 0>
data_info=[composer:u4u4u4][composer:"u4u4u4u4u1..."]<P2REPL_FORMAT_DATA_HEADER><P2REPL_FORMAT_TBL_HEADER>
null_fields=[composer:"u1..."]
body=<compressed records>


-----------------------------------------------
DATA (srv, post) VERSION 10
-----------------------------------------------
type = P2REPL_PKT_DATA
chan = [u8]<channel id>[u4]<session id>[u4]<++session order num = 0>
data_info=[composer:u4u4u4][composer:"u4u4u4u4u1..."]<P2REPL_FORMAT_DATA_HEADER><P2REPL_FORMAT_TBL_HEADER>
data_info_2=[composer:"u4u8..."]<P2REPL_FORMAT_DATA_2_TBL_INFO>
null_fields=[composer:"u1..."]
body=<compressed records>

-----------------------------------------------
ACK_DATA  (cli, post)
-----------------------------------------------
type = P2REPL_PKT_ACK_DATA
channel = [u8]<channel id>
session = [u4]<session id>
count = [u4]<session order num of DATA with ans_req>
body = [composer:"u4..."]<������ ������� �������� ������� + 1>
       (���� ������ ����� �������� ������ � ������, �� ����� ������� ������ composer)

///////////////////////////////////////////////

-----------------------------------------------
PING  (cli, post)
// ������������ �������� � ������� ������� ������ (��������� channel_id)
-----------------------------------------------
type = P2REPL_PKT_PING
channel = [u8]<channel id>

-----------------------------------------------
PING  (cli, post) VERSION 11
// ������������ �������� � ������� ������� ������
-----------------------------------------------
type = P2REPL_PKT_PING
channel = [u8]<channel id>
session = [u4]<session id>

-----------------------------------------------
PING  (srv, post)
// ������������ �������� � ������� �������� ������ �� ������� ������,
// ����� ������� ������ (�������� ������) ������������ P2REPL_PKT_COUNTED_PING
���� �� ��������� ��������� ������� ��� �������� ����������� ������ (�������� �������), 
������� ����� ���� ������ ������ ������
-----------------------------------------------
type = P2REPL_PKT_PING
channel = [u8]<channel id>

-----------------------------------------------
PING  (srv, post) VERSION 10
-----------------------------------------------
type = P2REPL_PKT_PING
chan = [u8]<channel id>[u4]<session id = 0>[u4]<session order num = 0>

-----------------------------------------------
P2REPL_PKT_COUNTED_PING  (srv, post)
-----------------------------------------------
type = P2REPL_PKT_COUNTED_PING
channel = [u8]<channel id>
session = [u4]<session id>
count = [u4]<session order num of DATA with ans_req>

-----------------------------------------------
P2REPL_PKT_COUNTED_PING  (srv, post) VERSION 10
-----------------------------------------------
type = P2REPL_PKT_COUNTED_PING
chan = [u8]<channel id>[u4]<session id>[u4]<session order num of DATA with ans_req>

-----------------------------------------------
CLOSE (cli/srv, post)
-----------------------------------------------
type = P2REPL_PKT_CLOSE
channel = [u8]<channel id>
[body] = [p2err]<reason>

-----------------------------------------------
CLOSE (srv, post) VERSION 10
-----------------------------------------------
type = P2REPL_PKT_CLOSE
chan = [u8]<channel id>[u4]<session id = 0>[u4]<session order num = 0>
[body] = [p2err]<reason>

-----------------------------------------------
P2REPL_PKT_CLEAR_DELETED (srv, post) VERSION 12
-----------------------------------------------
type = P2REPL_PKT_CLEAR_DELETED
channel = [u8]<channel id>
session = [u4]<session id>
count = [u4]<++session order num = 0>
body = [composer:"u4(u4u8)..."]<������� � ���� ����, <����� �������, ������ ������ ������>>

-----------------------------------------------
P2REPL_PKT_CLEAR_DELETED (srv, post) 
-----------------------------------------------
type = P2REPL_PKT_CLEAR_DELETED
channel = [u8]<channel id>
session = [u4]<session id>
count = [u4]<++session order num = 0>
flags = [u4]<clear deleted flag>
body  = [composer:"u4(u4u8)..."]<������� � ���� ����, <����� �������, ������ ������ ������>>

-----------------------------------------------
P2REPL_PKT_CLEAR_DELETED (srv, post) VERSION 10
-----------------------------------------------
type = P2REPL_PKT_CLEAR_DELETED
chan = [u8]<channel id>[u4]<session id>[u4]<session order num = 0>
body = [composer:"u4(u4u8)..."]<������� � ���� ����, <����� �������, ������ ������ ������>>

// � Plaza-2 v204 ����� RECONNECT �� ����������� ��������
// -----------------------------------------------
// RECONNECT (srv, post)
// -----------------------------------------------
// type = P2REPL_PKT_RECONNECT
// channel = [u8]<channel id>
// [body] = [u1]<clear db flag>
// 
// -----------------------------------------------
// RECONNECT (srv, post) VERSION 10
// -----------------------------------------------
// type = P2REPL_PKT_RECONNECT
// chan = [u8]<channel id>[u4]<session id = 0>[u4]<session order num = 0>
// [body] = [u1]<clear db flag>
*/

// ������������ ��������� ���������
#define P2REPL_CLIENT_TO_SERVER_CATEGORY_SYSTEM "P2_Replication_C2S"
#define P2REPL_SERVER_TO_CLIENT_CATEGORY_SYSTEM "P2_Replication_S2C"

// ������������ �����

// ������ ���������. ������������ � P2REPL_FORMAT_OPEN/P2REPL_FORMAT_OPEN_ANS
#define P2REPL_PROP_VERSION "P2_Repl_Ver"
// channel id. ������������ �������� � P2REPL_FORMAT_OPEN_ANS, ������ ������ �������������� �� ���� ����������
#define P2REPL_PROP_CHANNEL_ID "P2_Repl_Channel"
// session id. ������������ �������� � START � �������������� � DATA/ACK_DATA
#define P2REPL_PROP_SESSION_ID "P2_Repl_Session"
// order count. ������������ �������� � DATA � �������������������� � ����������� DATA � ������ � ��������� ��������� � �������
#define P2REPL_PROP_ORDER_COUNT "P2_Repl_Count"
// answer required. ������������ �������� � DATA 
//#define P2REPL_PROP_ANS_REQ "P2_Repl_Ans_Req" <- ������ ����� ���� ������������ ���� � body: P2REPL_DATA_FLAG_ANSREQ
#define P2REPL_PROP_DATA_INFO "P2_Repl_DataInfo"
// ������ ��������� ������������ ��������
#define P2REPL_PROP_DATA_INFO_2 "P2RDI2"
// ���������� ���� channel id, session id, order count 
#define P2REPL_PROP_CHANNEL_INFO "P2_Repl_Chan"
#define P2REPL_CHANNEL_INFO_FORMAT "u8u4u4"

// �����, ������������ � START_COMBINE � CLEAR_DELETED
#define P2REPL_PROP_START_FLAGS "P2_Repl_Flags"

// ����� �������, ������������ �������
#define P2REPL_PROP_SERVER_FLAGS P2REPL_PROP_START_FLAGS

// filters, �������� �������� ��� ������� �����
#define P2REPL_PROP_FILTER "P2_Repl_Filter" 
// null fields, ������� ���� ������, P2ReplNullFields.hpp
#define P2REPL_PROP_NULL_FIELDS "P2_Repl_Null_Fields"

#define P2REPL_PROP_REDIRECT_ADDRESS "P2_Redirect_Address"

#define P2REPL_PROP_TOKEN "P2_Repl_Token"

#define P2REPL_EXTRA_INFO "P2_Repl_Extra"

// �������� ������ 
#define P2REPL_PROP_PING_INTERVAL "P2_Repl_Ping_Interval"

// ���� ������� ���������
enum P2REPL_PKT
{
	P2REPL_PKT_OPEN,
	P2REPL_PKT_OPEN_ANS,
	P2REPL_PKT_OPEN_FAIL_ANS,
	P2REPL_PKT_SCHEME,
	P2REPL_PKT_SCHEME_ANS,
	P2REPL_PKT_START_SNAPSHOT_OBSOLETE, // �����.
	P2REPL_PKT_START_ONLINE,
	P2REPL_PKT_START_COMBINE,
	P2REPL_PKT_START_ANS,       // ?
	P2REPL_PKT_DATA,
	//P2REPL_PKT_ONLINE,
	P2REPL_PKT_ACK_DATA,
	P2REPL_PKT_PING,
	P2REPL_PKT_COUNTED_PING,
	P2REPL_PKT_CLOSE,
	P2REPL_PKT_CLEAR_DELETED, // ��������� � ������ 3. ������� ������ ��������� ���������
	P2REPL_PKT_RECONNECT // �� ���������� ��������, �������� ��� ������������� �� ������� ���������
};

typedef UINT32 P2REPL_VERSION;
typedef UINT64 P2REPL_CHANNEL_HANDLE;
typedef UINT32 P2REPL_SESSION_HANDLE;
typedef UINT32 P2REPL_ORDER_NUM;

//#define P2REPL_FORMAT_OPEN ������� ������ �� ����

// ����� �������
#define P2REPL_FORMAT_OPEN_ANS "b"

// ��� ������ ��������
#define P2REPL_FORMAT_OPEN_FAIL_ANS "u4"

// �����, ��������� ��������
#define P2REPL_FORMAT_SCHEME "b"

// ������� ��� �����
#define P2REPL_FORMAT_FILTER "u4b"

// ��� ������ ��������� �����
#define P2REPL_FORMAT_SCHEME_ANS "u4"

// ����� ������ ��������� �������� + 1 ���� ������ � �����
#define P2REPL_FORMAT_START_SNAPSHOT_PART   P2REPL_REVISION_FIELD_FORMAT
#define P2REPL_FORMAT_START_COMBINE_PART    P2REPL_REVISION_FIELD_FORMAT

// ������ ��������� �������� �� �������
#define P2REPL_FORMAT_START_SNAPSHOT_ANS_PART   P2REPL_REVISION_FIELD_FORMAT
#define P2REPL_FORMAT_START_COMBINE_ANS_PART    P2REPL_REVISION_FIELD_FORMAT

//#define P2REPL_DATA_FLAG_BOT	1
#define P2REPL_DATA_FLAG_EOT	2
#define P2REPL_DATA_FLAG_EOALL	4
#define P2REPL_DATA_FLAG_ANSREQ	8   // ���������� ����������� ��������� ������ ����� P2REPL_PKT_ACK_DATA
#define P2REPL_DATA_FLAG_ONLINE	0x10
// 0x20 ������� � V14 �������. �� ��������� �������� � ����� �� ��������� ����� REMOVE_DELETED_EX �� �������, � ������ ������ ���
// �� �������� ���������� ������ ���� REMOVE_DELETED_EX ������������ - ������ ������ ������ ������ ������
//#define P2REPL_DATA_FLAG_SHORT_DELETED 0x20
// ����, ����������, ��� ����� ������ �������� ������ ����� ������-���������� (��-�� ���������� ������� �����), �,
// ��������, ����� ����� ��� ����� � �������, ������������ � ���� ����������
// �������� � ��������� 14, ���� �� ������� �� �������������.
#define P2REPL_DATA_FLAG_PART_OF_TRANS_ONLINE 0x40

//#define P2REPL_START_FLAG_SNAPSHOT_SERVER_SUPPORTED 1 // ������ � ������ Plaza-2 204
//#define P2REPL_START_FLAG_REMOVE_DELETED 2 // deprecated with V14

#define P2REPL_TABLE_FLAG_EOT	1
#define P2REPL_TABLE_FLAG_FRC	2 // ������ �� ������� ����������� ����������� � ��������� ������

// DATA � ONLINE ������: 
/* 4 - �����
 * 4 - size of decompressed data
 * 4 - ��� ����������

 * 4 - ����� �������
 * 4 - ���-�� �������
 * 4 - ������ ������ ������� � �������
 * 4 - ������ ������ NULL'�� ������� � �������
 * 1 - ������� ��������� ������ ��������
 */
#define P2REPL_FORMAT_DATA_HEADER "u4u4u4"
#define P2REPL_FORMAT_TBL_HEADER  "u4u4u4u4u1"
#define P2REPL_FORMAT_DATA_2_TBL_INFO "u4u8"

// ������ ��������� �������� + 1 ���� ������ � �����
#define P2REPL_FORMAT_ACK_DATA_PART P2REPL_REVISION_FIELD_FORMAT

// ������� �������� ���������� (P2ERR)
#define P2REPL_FORMAT_CLOSE "u4"

// ������� ���� ��� ���������������
#define P2REPL_FORMAT_RECONNECT "u1"
 
//#define P2REPL_PING_FLAG_DATA 1
//#define P2REPL_PING_FLAG_ONLINE 2

#define P2REPL_FORMAT_CLEAR_HEADER "u4"
#define P2REPL_FORMAT_CLEAR_PART "u4" P2REPL_REVISION_FIELD_FORMAT

#define STR_SIZE(x) ((x) + 1)

#define P2REPL_SERVER_TO_CLIENT_DEFAULT_PING_INTERVAL			60000     // �������� �� ��������� ��������� ������ "������ --> ������"
#define P2REPL_CLIENT_TO_SERVER_PING_INTERVAL					10000     // �������� ������ "������ --> ������"
#define P2REPL_SKIPPED_PINGS        							3

#define P2REPL_FORMAT(a,b) ""#a#b""

/*
	��������� ������� �� ������ �� �������� �������� ����� ������������ ���������
	� ������������� 1 ������ ����������������� ���������� �����.
*/
#pragma pack(push, 1)
typedef struct P2REPL_CHANNEL_INFO_TAG
{
	P2REPL_CHANNEL_HANDLE	m_channelId;
	P2REPL_SESSION_HANDLE	m_session;
	P2REPL_ORDER_NUM		m_order;
} P2REPL_CHANNEL_INFO;
#pragma pack (pop)

#endif

