#ifndef _IMQ_CONNECTION_H_
#define _IMQ_CONNECTION_H_

#include "../../../Plaza-II/P2Utils/Include/IRc.h"
#include "../../../Plaza-II/P2SysMsg/Include/P2SysMsg.hpp" 

#include "P2MQ.h"

typedef const void* MULTIPLE_HANDLE;  

// уникальный идент. коннекшена; используется в качестве
// хэндла коннекшена(то есть безопасно кастится) к хэндлу. 0 -- невалидное значение. 

typedef size_t ConnID;


struct IMQConnection : public IRc
{
	virtual P2ERR   ConnectEx(const char* pConnStr, UINT32 timeout_, UINT32* pErrorClass) = 0;
	virtual void    Disconnect() = 0;
	virtual P2ERR	Login(PCSTR loginStr, UINT32* pLoginId) = 0;
	virtual P2ERR	Logout() = 0;
	virtual P2ERR	GetP2Msg(P2MSG_HANDLE* ppmsg, UINT32 timeout) = 0;
	virtual P2ERR	PostP2Msg(P2MSG_HANDLE pmsg) = 0;
	virtual P2ERR	AsyncSendP2Msg(P2MSG_HANDLE pmsg, UINT32 timeout,P2MQ_ASYNCSEND_CALLBACK cb_func = 0, void* cb_params = 0) = 0;
	virtual P2ERR	SendP2Msg(P2MSG_HANDLE pmsg, P2MSG_HANDLE* ppmsg_reply,UINT32 timeout) = 0;
	virtual P2ERR	PostP2MsgReply(P2MSG_HANDLE pmsgOrig, P2MSG_HANDLE pmsgReply) = 0;
    virtual UINT32	GetStatus() = 0;
    virtual UINT32	GetLastLoginId() = 0;
    virtual UINT32	GetStateLoginId() = 0;
    virtual P2ERR	GetNodeName(PSTR node, size_t* pSize) = 0;
    virtual P2ERR	GetAppName(PSTR app,size_t* pSize)    = 0;
	virtual P2ERR	ServiceRegister(PCSTR svcName, UINT8 type) = 0;
    virtual P2ERR	ServiceUnregister(PCSTR svcName) = 0;
    virtual P2ERR	ServiceResolve(PCSTR svcName, PSTR svcAddr, size_t* pAddrSize, const char* const* ppExcludes, UINT32 exclCount, P2MQ_ASYNCSEND_CALLBACK cb, void* cb_params) = 0;
	virtual bool	IsAdmin(const char* routerName) const = 0;
	virtual P2ERR   RegisterLLMsgHandler(UINT32 system_type, const std::pair<P2MQ_LLMSG_HANDLER, void*>& handler) = 0; 
 	virtual P2ERR   PostLLMsg(UINT32 system_type, UINT32 flags, const void* body, size_t size) = 0; 
	virtual P2ERR   Ioctl(MQIoctl* ctl) = 0;
};





#endif //<_IMQ_CONNECTION_H_>
