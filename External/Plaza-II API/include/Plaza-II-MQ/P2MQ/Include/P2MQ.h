#ifndef _P2MQ_H_
#define _P2MQ_H_

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef P2SYS_EXPORTS
#define P2MQ_API P2_API_EXPORTS
#else
#define P2MQ_API P2_API_IMPORTS
#endif

#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II/P2SysMsg/Include/P2SysMsg.h"
#include "../../../Plaza-II/P2SysMsg/Include/P2SysMsgTypes.h"
#include "../../../Plaza-II/P2SysLog/Include/P2SysLog.h" 
#include "../../P2MQRouter/Include/P2LLMsgRanges.h" 
#include "P2MQIoctl.h"


//////////////////////////////////////////////////////////////////////

#define P2MQ_INFINITE 0xFFFFFFFF  // Infinite timeout

#define DEF_MQ_CONN_TIMEOUT         3000

typedef void* P2MQ_CONN_HANDLE;
typedef void (P2CALLSPEC *P2MQ_ASYNCSEND_CALLBACK)(P2MSG_HANDLE msgReply, void* cbParams);

#define P2MQ_ERRORCLASS_OK          0
#define P2MQ_ERRORCLASS_IS_USELESS  1

// Connection statuses
#define P2MQ_CONNECTION_STATUS_MASK     0x0000FFFF
#define P2MQ_CONNECTION_DISCONNECTED    0x00000001
#define P2MQ_CONNECTION_CONNECTED       0x00000002
#define P2MQ_CONNECTION_INVALID         0x00000004
#define P2MQ_CONNECTION_BUSY            0x00000008
#define P2MQ_CONNECTION_IS_USABLE(status) \
		( ((status) & P2MQ_CONNECTION_STATUS_MASK) == P2MQ_CONNECTION_CONNECTED)
// Router statuses
// Все статусы ненулевые константы. P2MQ соединение всегда имеет ненулевой статус рутера.
#define P2MQ_ROUTER_STATUS_MASK         0xFFFF0000
#define P2MQ_ROUTER_DISCONNECTED        0x00010000
#define P2MQ_ROUTER_RECONNECTING        0x00020000
#define P2MQ_ROUTER_CONNECTED           0x00040000
#define P2MQ_ROUTER_LOGINFAILED         0x00080000
#define P2MQ_ROUTER_NOCONNECT           0x00100000


#define P2_ROUTER_SERVICE_NAME "router"
#define P2_ROUTER_DOT_SERVICE_NAME ".router"
#define P2_ROUTER_DOT_SERVICE_NAME_SIZE 8   //size of the ".router" including zero at the end

#define P2MQ_SERVICE_TYPE_LOCAL		0
#define P2MQ_SERVICE_TYPE_GLOBAL	1
#define P2MQ_SERVICE_TYPE_NEIGHBOUR	2
//////////////////////////////////////////////////////////////////////


// Поведение P2MQ при нехватке памяти:
// P2MQCreateConnection, P2MQConnectEx  		
//    сильная гарантия:  при возврате ошибки библиотека находится в таком же состоянии как и до вызова
// P2MQFreeConnection, P2MQDisconnect 	  	
//    гарантия отстутствия ошибки: эти функции могут вернуть только ошибку P2ERR_MQ_INVALID_CONN_HANDLE при неправильном хэндле соединения
// все остальные операции P2MQxxx   
// 	  слабая гарантия: при возникновении ошибки соединение переходит в состояние P2MQ_CONNECTION_INVALID.  Соединение необходимо переподключить


P2MQ_API P2ERR P2CALLSPEC P2MQStartup();
P2MQ_API void P2CALLSPEC P2MQCleanup();

P2MQ_API P2ERR P2CALLSPEC P2MQCreateConnection(P2MQ_CONN_HANDLE* pConn);

P2MQ_API P2ERR P2CALLSPEC P2MQFreeConnection(P2MQ_CONN_HANDLE conn);

P2MQ_API P2ERR P2MQRegisterTimeSync(P2MQ_CONN_HANDLE pConn);
P2MQ_API P2ERR P2MQUnRegisterTimeSync(P2MQ_CONN_HANDLE pConn);

// Get current status of connection [& connected router]
P2MQ_API P2ERR P2CALLSPEC P2MQGetStatus(P2MQ_CONN_HANDLE conn, UINT32* pStatus);

// Get login_id of my last successfull P2MQLogin()
P2MQ_API P2ERR P2CALLSPEC P2MQGetLastLoginId(P2MQ_CONN_HANDLE conn, UINT32* pLoginId);

// Get login_id of last received router status message
P2MQ_API P2ERR P2CALLSPEC P2MQGetStateLoginId(P2MQ_CONN_HANDLE conn, UINT32* pLoginId);

// Get current node name of connected connection
// (if result == P2ERR_COMMON_BUFFER_TOO_SMALL, then *pSize == necessary size)
P2MQ_API P2ERR P2CALLSPEC P2MQGetNodeName(P2MQ_CONN_HANDLE conn, PSTR node, /* in, out */ size_t* pSize);

// Get current application name of connected connection
// (if result == P2ERR_COMMON_BUFFER_TOO_SMALL, then *pSize == necessary size)
P2MQ_API P2ERR P2CALLSPEC P2MQGetAppName(P2MQ_CONN_HANDLE conn, PSTR app, /* in, out */ size_t* pSize);

// Optional parameters:
//  - host, password (can be 0 or "")
//  - pErrorClass (can be 0)
P2MQ_API P2ERR P2CALLSPEC P2MQConnect(P2MQ_CONN_HANDLE conn, PCSTR appName, UINT16 port,
                                      UINT32 timeout, PCSTR host, PCSTR password,
                                      UINT32* pErrorClass);

// options - string with parameters: <name1>=<value1>;<name2>=<value2>...
// Now used simple version of parser:
//   - don't use key symbols in the field name and value!
//     (key symbols: ";,/=")
//   - field names case sensitive
// Standard field names:
//  Required:
//      PORT
//      APPNAME
//  Optional:
//      HOST
//      PASSWORD
// Optional parameters:
//  - pErrorClass (can be 0)
P2MQ_API P2ERR P2CALLSPEC P2MQConnectEx(P2MQ_CONN_HANDLE conn,
                                        PCSTR options, UINT32 timeout,
                                        UINT32* pErrorClass);

P2MQ_API P2ERR P2CALLSPEC P2MQConnectEx2(P2MQ_CONN_HANDLE conn,
                                        PCSTR options, 
                                        UINT32* pErrorClass);

P2MQ_API P2ERR P2CALLSPEC P2MQDisconnect(P2MQ_CONN_HANDLE conn);

P2MQ_API P2ERR P2CALLSPEC P2MQLogin(P2MQ_CONN_HANDLE conn, PCSTR loginStr, UINT32* pLoginId);
P2MQ_API P2ERR P2CALLSPEC P2MQLogout(P2MQ_CONN_HANDLE conn);

P2MQ_API P2ERR P2CALLSPEC P2MQPostMsg(P2MQ_CONN_HANDLE conn, P2MSG_HANDLE msg);


/* nPacket: count of packets(!=0), errs (!=0) -- array with size nPacket;
   return : P2ERR_OK -- router has been processed all, errs[i] == P2ERR_OK,
            P2ERR_MQ_NOT_ALL_POSTED -- errs[i] is the error of router msg process,
		    other -- (internal problem) errs[i] is undefined */
P2MQ_API P2ERR P2CALLSPEC P2MQPostMultipleMsgs(P2MQ_CONN_HANDLE conn,P2MSG_HANDLE* ppmsg,UINT32 nPacket,P2ERR* errs);

P2MQ_API P2ERR P2CALLSPEC P2MQAsyncSendMsg(P2MQ_CONN_HANDLE conn, P2MSG_HANDLE msg, UINT32 timeout, P2MQ_ASYNCSEND_CALLBACK cbFunc, void* cbParams);

P2MQ_API P2ERR P2CALLSPEC P2MQPostReplyMsg(P2MQ_CONN_HANDLE conn, P2MSG_HANDLE msgOrig, P2MSG_HANDLE msgReply);

P2MQ_API P2ERR P2CALLSPEC P2MQSendMsg(P2MQ_CONN_HANDLE conn, P2MSG_HANDLE msg, P2MSG_HANDLE* pmsgReply, UINT32 timeout);

P2MQ_API P2ERR P2CALLSPEC P2MQGetMsg(P2MQ_CONN_HANDLE conn, P2MSG_HANDLE* pmsg, UINT32 timeout);


P2MQ_API P2ERR P2CALLSPEC P2MQServiceRegister(P2MQ_CONN_HANDLE conn, PCSTR svcName, BOOL global);
P2MQ_API P2ERR P2CALLSPEC P2MQServiceRegister2(P2MQ_CONN_HANDLE conn, PCSTR svcName, UINT8 serviceType);
P2MQ_API P2ERR P2CALLSPEC P2MQServiceUnregister(P2MQ_CONN_HANDLE conn, PCSTR svcName);

P2MQ_API P2ERR P2CALLSPEC P2MQServiceResolve(P2MQ_CONN_HANDLE conn, 
											 PCSTR svcName,
                                             PSTR svcAddr, 
											 size_t* pAddrSize,
                                             const PCSTR* ppExcludes, 
											 UINT32 exclCount);

/* Асинхронный резолв сервиса.
 * При успешном резолве вызывается коллбэк cb в который передаётся P2MSG_HANDLE c полем
 *  	category : P2MSG_CATEGORY_SYSTEM,
 *  	type	 : P2MSG_TYPE_SYSTEM_RESOLVE_SUCCESS,
 *		body	 : содержит строку mq адреса сервиса.
 * При неуспешном резолве вызывается коллбэк с сообщением reply failed аналогично ф-ии P2MQAsyncSendMsg 
 *  	category : P2MSG_CATEGORY_SYSTEM,
 *  	type	 : P2MSG_TYPE_SYSTEM_REPLY_FAILED,
 *		body	 : содержит P2ERR ошибку резолва */
 	


P2MQ_API P2ERR P2MQServiceAsyncResolve(	P2MQ_CONN_HANDLE conn, 
										PCSTR svcName, 
										P2MQ_ASYNCSEND_CALLBACK cb, 
										void* cb_params,
										const char* const* ppExcludes, 
										UINT32 exclCount); 


P2MQ_API P2ERR P2CALLSPEC P2MQGetNodeNameFromAddr(PCSTR addr, PSTR name, size_t* pNameSize);
P2MQ_API P2ERR P2CALLSPEC P2MQGetAppNameFromAddr(PCSTR addr, PSTR name, size_t* pNameSize);
P2MQ_API P2ERR P2CALLSPEC P2MQAddrCompose(PCSTR node, PCSTR app, PSTR addr, size_t* pAddrSize);
P2MQ_API BOOL P2MQIsAdmin(char *tstAdmin, P2MQ_CONN_HANDLE pConn);



typedef void (P2CALLSPEC *P2MQ_LLMSG_HANDLER)(P2MQ_CONN_HANDLE conn, const UINT8* pData, size_t dataSz, void* context); 

// Зарегистрировать обработчик LLMsg с типом system_type.
// Если conn != NULL, то обработчик будет вызываться только для указанного соединения.
// Если conn == NULL, то обработчик будет вызываться для всех текущих соединений и для всех соединений, которые будут созданы в дальнейшем
// Если handler == NULL, то устанавливается дефолтный обработчик, который игнорирует все сообщения. Т.е. это фактически удаление текущего обработчика
// Для handler == NULL сохраняются правила о нулевом/ненулевом коннекшене.
// 
// Обработчик на конкретное соединение можно установить только в состоянии P2MQ_CONNECTION_CONNECTED
// Все обрабочики, установленные на конкретное соединение, пропадают при дисконнекте (!).  (сейчас так сделано для внутренней простоты)

P2MQ_API P2ERR P2CALLSPEC P2MQRegisterLLMsgHandler(P2MQ_CONN_HANDLE conn, UINT32 system_type, P2MQ_LLMSG_HANDLER handler, void* context);

P2MQ_API P2ERR P2CALLSPEC P2MQPostLLMsg(P2MQ_CONN_HANDLE conn, UINT32 system_type, UINT32 flags, const void* pBody, size_t size);



struct IMQTransportPluginImpl; 
typedef IMQTransportPluginImpl* (P2CALLSPEC *P2MQ_PLUGIN)();

P2MQ_API P2ERR P2MQRegisterPlugin(const char* name, P2MQ_PLUGIN factory);
P2MQ_API P2ERR P2MQUnregisterPlugin(const char* name);

P2MQ_API P2ERR P2MQIoctl(P2MQ_CONN_HANDLE conn, MQIoctl* ctl);

//////////////////////////////////////////////////////////////////////
#ifdef  __cplusplus
}
#endif

#endif // _P2MQ_H_
