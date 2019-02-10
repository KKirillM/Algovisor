#ifndef _P2MQ_REPLY_H_
#define _P2MQ_REPLY_H_

#ifdef P2MQREPLY_EXPORTS
#define P2MQREPLY_API //__declspec(dllexport)
#else
#define P2MQREPLY_API //__declspec(dllimport)
#endif

#include "../../../Plaza-II/P2Utils/Include/IRc.h"
#include "../../P2MQRouter/Include/LLMsg.h"
#include "../../P2MQRouter/Include/IMsgConnection.h"
#include "../../P2MQRouter/Include/IModuleServices.h"
#include "../../../Plaza-II/P2Utils/Include/WeakMsgID.h"
#include "../../../Plaza-II/P2Utils/Include/ServiceDriver.h"
#include "../../../Plaza-II/P2SysMsg/Include/P2SysMsg.h"


/*
идеология сенд-реплаев низкого уровня:

механизм сенд-реплаев сделан для автоматизации пар сообщений запрос-ответ.

для определения сервиса на узле-ответчике используется тип сообщения из диапазона.
так, для получения реплая от пользователя используется P2LL_MSG_USER_SEND

как с ним работать со стороны получателя:
1. получить ISendReplySeriсe
2. вызвать ISendReplySeriсe->Send и передать в него сообщение, IReplyCallback, тип обработчика и время таймаута
3. продолжать работу. реплай будет обработан асинхронно
4. по вызову IReplyCallback->Reply обработать реплай
5. по вызову IReplyCallback->ReplyFailed обработать ошибку посылки

как с ним работать с принимающей стороны:
1. обработчик диапазона получает сообщение. 
2. сообщение обрабатывается надлежащим образом и посылается ответ через ISendReplySeriсe->reply

//////

что делает каждый участник

посылающий:
1. реализует IReplyCallback
2. посылает

принимающий
1. заводит тип пакета под сенды
2. в обработчике типа посылает ответ

сервис на посылающей стороне
1. принимает сенды
2. шлет их
3. выставляет таймаут в шедулере
4. обрабатывает принятые пакеты и вызывает коллбеэк
5. вызывает коллбэк при таймаует

сервис на принимающей стороне
1. принимает пакеты
2. перенаправляет их к обработчику
3. принимает реплаи от обработчика
4. посылает их
*/

// реализуется посылающим реплаи
struct IReplyCallback : public IRc
{
	virtual P2ERR Reply(PCTLLMsgHdr pHdr, const void* pBody, IMsgConnection* pIMsgConn) = 0;
	virtual P2ERR ReplyFailed(P2ERR reason, const WeakID& pid) = 0;
};


// реализуется сервисом-обработчиком реплаев
struct IReplyService 
{
    // Константа для конвертирования timeout-a из UINT32 (milliseconds)
    // в LONGLONG (100-nanosecond intervals): LONGLONG ns = ms * TIMEOUT_MS
    enum { TIMEOUT_MS = 10000 };
    static LONGLONG SendTimeFromMs(int ms)
    {
        return static_cast<LONGLONG>(ms) * TIMEOUT_MS;
    }

	// посылка сенда. на вход подается низкоуровневый пакет (pData, len)
	//	, тип этого пакета type, интерфейс получения реплая pCallback и id посылаемого сообщения
	virtual P2ERR Send(const char *to, const void *pData, size_t len, LONGLONG timeout, UINT32 type, IReplyCallback *pCallback, WeakID& id, UINT32 flags) = 0;

	// отправление реплая.
	// передается низкоуровневый пакет (pData, len)
	// и исходный заголовок. важно! передавать указатель на _тот_самый_заголовок!
	virtual P2ERR Reply(const void *pData, size_t len, const TLLMsgHdr *pOrigHdr, UINT32 flags) = 0; 
	virtual P2ERR ReplyEx(const void *pData, size_t len, const WeakID& id, UINT32 flags) = 0;

	virtual P2ERR GetIDByHeader(const TLLMsgHdr *pOrigHdr, WeakID& id) = 0;
	virtual P2ERR GetFromByHeader(const TLLMsgHdr *pOrigHdr, char *from, size_t len) = 0;
	
	// разбиение функции Send на генерацию id и собственно посылку. Нужна, если id вставляется в тело
	virtual P2ERR GetNewSendID(WeakID& id) = 0;
	virtual P2ERR SendWithID(const char *to, const void *pData, size_t len, LONGLONG timeout, UINT32 type, IReplyCallback *pCallback, const WeakID& id, UINT32 flags) = 0;

	// функции для проброса сенда дальше. Нужна, если ответ на сенд может выдать другой узел. Функция не оставляет
	// никакой информации на текущем узле и пробрасывает обычный сенд на узел to. orig_address - адрес пославшего send
	virtual P2ERR ReSend(const char *to, const void *pData, size_t len, const TLLMsgHdr *pOrigHdr, UINT32 flags) = 0;
	virtual P2ERR ReSendEx(const char *to, const void *pData, size_t len, UINT32 type, const WeakID& id, UINT32 flags) = 0; 

	// обработать сенды, addr_to которых не совпадает с pExcludeAddr
	virtual P2ERR ProcessNotMySENDMessage(const char* pExcludeAddr, const TLLMsgHdr* pHdr, const void* pBody, IMsgConnection* pIMsgConn, TLLMsgHdr* pOriginalMsgHdr, const void** ppOriginalMsgBody) = 0; 

	// создать сообщение в формате реплая.
	virtual P2ERR CreateLLMsgReply(	IBytesWLenPriority* pIBytes,
									const UINT64 id,
									const char* pAddrTo,
									const void* pBody,
									size_t 		bodySz) const = 0;

	virtual P2ERR CreateLLMsgReply( IBytesWLenPriority* pIBytes, 
									const UINT64 id,
									const char* pAddrTo,
									P2MSG_FIELD* msgFields,
									size_t 		 msgFieldsCnt) const  = 0; 

	virtual P2ERR SimpleSendWithID(	const char *pAddrTo, 
									const void *pData, 
									size_t 		len, 
									LONGLONG 	timeout, 
									UINT32 		type, 
									UINT64 		id, 
									UINT32 		flags) const = 0;
   virtual ~IReplyService() {}
};


bool RouterReply_GetService(IModuleServices *pServices, IReplyService **pInterface);
void RouterReply_FreeService(IReplyService * pReplyService);



P2ERR LLMsgSend_SecurityCheck(const void* pBody, size_t size, const char* pAddrTo, const char* pAddrFrom);
P2ERR LLMsgReply_SecurityCheck(const void* pBody, size_t size, const char* pAddrFrom);



#endif // _P2MQ_REPLY_H_
