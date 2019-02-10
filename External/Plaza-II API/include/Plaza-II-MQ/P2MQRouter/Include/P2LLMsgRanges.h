#ifndef _P2LLMSGRANGES_H_ 
#define _P2LLMSGRANGES_H_
/*
 * ���� �������� �������������� ��������� 
 * �������� ������ �������� ������� ��������� � ������ ����
 * ����������� ������ �������������� ��� ��������� � ������� � ���� ��� P2Message!
 */


#include "../../../Plaza-II/P2Common/Include/P2Types.h"

#define P2LL_MSG_RANGE_SIZE    0x1000

#define P2LL_MSG_COMMON_BEGIN   0x0000
#define P2LL_MSG_COMMON_END     (P2LL_MSG_COMMON_BEGIN + P2LL_MSG_RANGE_SIZE - 1)

#define P2LL_MSG_ROUTE_BEGIN    0x1000
#define P2LL_MSG_ROUTE_END      (P2LL_MSG_ROUTE_BEGIN + P2LL_MSG_RANGE_SIZE - 1)


//// �� ������������
//#define P2LL_MSG_UNUSED_0X2_BEGIN 	0X2000 
//#define P2LL_MSG_UNUSED_0X2_END		(P2LL_MSG_UNUSED_0X2_BEGIN  + P2LL_MSG_RANGE_SIZE - 1)


#define P2LL_MSG_REPLY_BEGIN    0x3000
#define P2LL_MSG_REPLY_END      (P2LL_MSG_REPLY_BEGIN + P2LL_MSG_RANGE_SIZE - 1)

#define P2LL_MSG_MQ_BEGIN       0x4000
#define P2LL_MSG_MQ_END         (P2LL_MSG_MQ_BEGIN + P2LL_MSG_RANGE_SIZE - 1)

#define P2LL_MSG_MQCLI_BEGIN    0x5000
#define P2LL_MSG_MQCLI_END      (P2LL_MSG_MQCLI_BEGIN + P2LL_MSG_RANGE_SIZE - 1)

#define P2LL_MSG_SERV_BEGIN     0x6000
#define P2LL_MSG_SERV_END       (P2LL_MSG_SERV_BEGIN + P2LL_MSG_RANGE_SIZE - 1)

#define P2LL_MSG_TRACERT_BEGIN  0x7000
#define P2LL_MSG_TRACERT_END    (P2LL_MSG_TRACERT_BEGIN + P2LL_MSG_RANGE_SIZE - 1)

// �� ������������ 
//#define P2LL_MSG_GRNT_BEGIN     0x8000
//#define P2LL_MSG_GRNT_END       (P2LL_MSG_GRNT_BEGIN + P2LL_MSG_RANGE_SIZE - 1)


//#define P2LL_MSG_SPLITTER_BEGIN 0x9000
//#define P2LL_MSG_SPLITTER_END   (P2LL_MSG_SPLITTER_BEGIN + P2LL_MSG_RANGE_SIZE - 1)


#define P2LL_MSG_TIMESYNC_BEGIN 0x10000
#define P2LL_MSG_TIMESYNC_END   (P2LL_MSG_TIMESYNC_BEGIN + P2LL_MSG_RANGE_SIZE - 1)

#define P2LL_MSG_ROUTER_REPLICA_BEGIN 0x11000
#define P2LL_MSG_ROUTER_REPLICA_END   (P2LL_MSG_ROUTER_REPLICA_BEGIN + P2LL_MSG_RANGE_SIZE - 1)

#define P2LL_MSG_CUSTOM_HANDLER_BEGIN 0x12000
#define P2LL_MSG_CUSTOM_HANDLER_END  (P2LL_MSG_CUSTOM_HANDLER_BEGIN + P2LL_MSG_RANGE_SIZE - 1)
#define P2LL_MSG_IS_CUSTOM_HANDLER(x) ((x) >= P2LL_MSG_CUSTOM_HANDLER_BEGIN && (x) <= P2LL_MSG_CUSTOM_HANDLER_END) 

// ������ ���������� ������ ���� ���������� ����� ������� ����������� ���� ������ ����� ���������.
#define P2LL_MSG_USER_DEFINED_BEGIN 0x13000
#define P2LL_MSG_USER_DEFINED_END  (P2LL_MSG_USER_DEFINED_BEGIN + P2LL_MSG_RANGE_SIZE - 1)
#define P2LL_MSG_IS_USER_DEFINED(x) ((x) >= P2LL_MSG_USER_DEFINED_BEGIN && (x) <= P2LL_MSG_USER_DEFINED_END) 

// ��������� ��������� ������ ��������
//P2LL_MSG_ROUTE
enum 
{
	// ���� ������ �������� �� ����� ����� ����
	// ��������: ������ � �������
	P2LL_MSG_ROUTE_ADD_ROUTE = P2LL_MSG_ROUTE_BEGIN,
	// ���� ������ ������� ������� �� ���� ����� ����
	// ��������: ������ � �������
	P2LL_MSG_ROUTE_DEL_ROUTE,
	// ����. �� ������� �������� ����������. ����� ��� �������� ��������� ������
	P2LL_MSG_ROUTE_PING,
	// ����� �� ����
	P2LL_MSG_ROUTE_PING_REPLY,
	// ������� �� ���������� �������
	P2LL_MSG_ROUTE_DEL_EXTRA_ROUTER,
	// ������ ����, �� ��������� ������
	P2LL_MSG_ROUTE_DUMMY_PING,
	P2LL_MSG_ROUTE_LAST
};

enum
{
	P2LL_MSG_REPLY_SEND = P2LL_MSG_REPLY_BEGIN,
	P2LL_MSG_REPLY_REPLY,
	P2LL_MSG_REPLY_LAST
};

enum
{
    // ������-llmsg-type
    P2LL_MSG_USE_DEFAULT_MQ = P2LL_MSG_MQ_BEGIN,
    P2LL_MSG_MQ_LOC_LOGIN,
    P2LL_MSG_MQ_LOC_LOGOUT,
    P2LL_MSG_MQ_LOC_P2MSG_POST,
    P2LL_MSG_MQ_REM_P2MSG_POST,
    P2LL_MSG_MQ_LOC_P2MSG_POSTREPLY,
    P2LL_MSG_MQ_REM_P2MSG_POSTREPLY,
    P2LL_MSG_MQ_LOC_P2MSG_SEND,				 // unused
    P2LL_MSG_MQ_REM_P2MSG_SEND,
    P2LL_MSG_MQ_LOC_P2MSG_REPLYFAILED,
    P2LL_MSG_MQ_LOC_P2MSG_SENDSYNC,			// unused
    P2LL_MSG_MQ_LOC_P2MSG_POSTREPLYSYNC,	// unused
    P2LL_MSG_MQ_LOC_P2MSG_REPLYFAILEDSYNC,  // Unused
    P2LL_MSG_MQ_LOC_SERVICE_REGISTER,
    P2LL_MSG_MQ_LOC_SERVICE_UNREGISTER,
    P2LL_MSG_MQ_LOC_SERVICE_RESOLVE,
    P2LL_MSG_MQ_REM_P2MSG_POST_GRNT,		// unused
    P2LL_MSG_MQ_LOC_P2MSG_POST_GRNT_CONF,   // unused
    P2LL_MSG_MQ_LOC_P2MSG_POST_ALL,    
	// �� �� �����, ��� P2LL_MSG_MQ_LOC_P2MSG_POST_ALL: ���������������� ��������� PostMultipleMsgs 
	P2LL_MSG_MQ_LOC_P2MSG_POST_ALL2, 
	// ���� ��������� ��� ������� P2MQUser �� ���������� operres. 
     P2LL_MSG_MQ_LOC_P2MSG_POSTREPLY_NOOPERRES,
     P2LL_MSG_MQ_LOC_P2MSG_SEND_NOOPERRES,
     P2LL_MSG_MQ_LOC_P2MSG_SENDSYNC_NOOPERRES, // unused
	 P2LL_MSG_MQ_LAST
};

enum
{
    P2LL_MSG_MQCLI_OPER_RES = P2LL_MSG_MQCLI_BEGIN,
    P2LL_MSG_MQCLI_P2MSG_RES,  // ubused
    P2LL_MSG_MQCLI_NEW_ROUTER_STATUS,
    P2LL_MSG_MQCLI_LOGIN_RES,
    P2LL_MSG_MQCLI_SERVICE_RESOLVE_RES,
    
	P2LL_MSG_MQCLI_LAST
};

enum
{
    P2LL_MSG_SERV_RESOLVE_FROM_DOWN = P2LL_MSG_SERV_BEGIN,
    P2LL_MSG_SERV_ADD_FROM_DOWN,
    P2LL_MSG_SERV_ADD_FROM_DIRECT,
    P2LL_MSG_SERV_DEL_FROM_DOWN,
    P2LL_MSG_SERV_DEL_FROM_DIRECT,
//    P2LL_MSG_SERV_,
	P2LL_MSG_SERV_LAST
};

enum
{
    P2LL_MSG_TRACERT_FORWARD = P2LL_MSG_TRACERT_BEGIN,
    P2LL_MSG_TRACERT_BACK,
	P2LL_MSG_TRACERT_LAST
};

enum
{
	P2LL_MSG_TIMESYNC_ADDR = P2LL_MSG_TIMESYNC_BEGIN,
	P2LL_MSG_TIMESYNC_TO_LOC_POST,
	P2LL_MSG_TIMESYNC_TO_REM_POST,
	P2LL_MSG_TIMESYNC_LAST
};

enum 
{
	//��� �������������� ���������, ��������������� ��� ���������������� ��������� �������
	//����������-��������� ������ RouterReplica.
	P2LL_MSG_ROUTER_REPLICA = P2LL_MSG_ROUTER_REPLICA_BEGIN,
	P2LL_MSG_ROUTER_EREPLICA,
	P2LL_MSG_ROUTER_EREPL_C2S_RETRANSMIT_REQUEST,
	P2LL_MSG_ROUTER_EREPL_S2C_RETRANSMIT_FAILED, 
	P2LL_MSG_ROUTER_REPLICA_LAST 
};

// ���������, ���������� ������� �� ���������������� � p2mq. ��� ���������
// ������������ ������ ���������������� ���������� ������� ���� � p2mq.
enum 
{ 
	// ������ ������� ������ ���������� ��������� ���������� ������� �����������
	// <RouterSpecific>,c	����������, ������ ������ �������   
	// <AddrFrom> rawCStr   ����� ������� ����������
	// DataFields rawdata   ���� ������, ������� ���������� �����������
	P2LL_MSG_CUSTOM_EREPLICA = P2LL_MSG_CUSTOM_HANDLER_BEGIN, 
	P2LL_MSG_CUSTOM_REPLICA, 

	/* �������� ������ ������� �������� */
	P2LL_MSG_DATA_EVENTS,
	/* �������� ��� ������ P2ERR */
	P2LL_MSG_TRANSPORT_ERROR,
	/* ������������ ���� ��������� ����������� ��� �������� ���������� ������� */
	P2LL_MSG_SERVICE_REMOVED_NOTIFICATION,
	P2LL_MSG_CUSTOM_HANDLER_LAST 
};

// ��������� type processor'�, ����������� �� dll, ����� ������������ ���� ��������� �� ����� ���������.
enum 
{ 
	P2LL_MSG_P2MQ_ORDER_INJECTOR = P2LL_MSG_USER_DEFINED_BEGIN,
	P2LL_MSG_P2MQ_IOCTL,
	P2LL_MSG_USER_DEFINED_LAST
};

// ���������� ��������� ����� � ���������
#define  P2LLMSG_RANGE_COUNT(RANGE)  		(RANGE##_LAST - RANGE##_BEGIN)
// ���������� ����� ���������, � ������� �������� �������� ���
#define  P2LLMSG_RANGE_IDX(type)        	( (type) >> 12) 
// ��������, � ������� �������� �������� ���
#define  P2LLMSG_RANGE(type)    			( (type) & 0xfffff000 )
// ���������� ����� ���� � ����� ���������
#define  P2LLMSG_RANGE_TYPE_IDX(type)  		( (type) & 0x00000fff )

// ������� �������� ������������ ����������
#define  P2LLMSG_RANGE_SIZES  {	/*  0x0	 */			0								   ,						\
								/*  0x1	 */ 		P2LLMSG_RANGE_COUNT(P2LL_MSG_ROUTE), 						\
								/*  0x2	 */ 		0								   ,						\
								/*  0x3	 */ 		P2LLMSG_RANGE_COUNT(P2LL_MSG_REPLY), 						\
								/*  0x4	 */ 		P2LLMSG_RANGE_COUNT(P2LL_MSG_MQ), 						    \
								/*  0x5	 */ 		P2LLMSG_RANGE_COUNT(P2LL_MSG_MQCLI), 						\
								/*  0x6	 */ 		P2LLMSG_RANGE_COUNT(P2LL_MSG_SERV), 						\
								/*  0x7	 */ 		P2LLMSG_RANGE_COUNT(P2LL_MSG_TRACERT),				        \
								/*  0x8	 */ 	  	0								   ,                        \
								/*  0x9	 */ 		0								   ,						\
								/*  0xA	 */ 		0								   ,						\
								/*  0xB	 */ 		0								   ,						\
								/*  0xC	 */ 		0								   ,						\
								/*  0xD	 */ 		0								   ,						\
								/*  0xE	 */ 		0								   ,						\
								/*  0xF	 */ 		0								   ,						\
								/*  0x10 */ 		P2LLMSG_RANGE_COUNT(P2LL_MSG_TIMESYNC),						\
								/*  0x11 */			P2LLMSG_RANGE_COUNT(P2LL_MSG_ROUTER_REPLICA),				\
								/*  0x12 */			P2LLMSG_RANGE_COUNT(P2LL_MSG_CUSTOM_HANDLER),				\
								/*  0x13 */			P2LLMSG_RANGE_COUNT(P2LL_MSG_USER_DEFINED)  }




#endif // _P2LLMSGRANGES_H_



