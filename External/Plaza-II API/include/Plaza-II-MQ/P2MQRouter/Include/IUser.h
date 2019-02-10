// interface IUser
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IUSER_H__4C932BB0_A447_4FA0_A095_8EC32D0EE5B7__INCLUDED_)
#define AFX_IUSER_H__4C932BB0_A447_4FA0_A095_8EC32D0EE5B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II/P2SysMsg/Include/P2SysMsg.h"
#include "../../P2MQRouterReply/Include/P2MQRouterReply.h"
#include "P2LLMsgRanges.h"
#include "../../../Plaza-II/P2Utils/Include/ServiceDriver.h"
#include "IRouter.h"

struct IUser 
{
    // Если получатель MQ-compatible (локальное соединение, на другом конце
    // которого MQ.DLL), то system_type можно не задавать - будет использоваться
    // значение по умолчанию: P2LL_MSG_USE_DEFAULT_MQ)

    // - In msg must be set: "addr_to", "addr_from"
    virtual P2ERR P2MsgPost(P2MSG_HANDLE msg, UINT32 system_type = P2LL_MSG_USE_DEFAULT_MQ) = 0;
    // - In msg must be set: "addr_to", "addr_from"
    // - This msg will be delivered as LLMsg only to the destination node router,
    //   & receiver must catch LLMsg of specified system_type!
    virtual P2ERR P2MsgSend(P2MSG_HANDLE msg, IReplyCallback* pICb, UINT32 timeout,
                            WeakID& sendId, UINT32 system_type = P2LL_MSG_USE_DEFAULT_MQ) = 0;
    // - In msg must be set: "addr_to", "addr_from", "reply_id"
    virtual P2ERR P2MsgPostReply(P2MSG_HANDLE msg) = 0;

    //
    // Рассылка локальным соединениям информации об изменении состояния узла
    //

    // On change count of created outgoing (remote) connections
    // (called by Connection Manager)
    virtual void OnCreateOutgoingConnection(unsigned outgoing_count, conn::LUID luid) = 0;
    virtual void OnDestroyOutgoingConnection(unsigned outgoing_count) = 0;

    // On change count of established outgoing (remote) connections
    // (called by Router)
    virtual void OnLoginSucceeded(unsigned outgoing_rem_count, conn::LUID luid) = 0;
    virtual void OnClose(unsigned outgoing_rem_count) = 0;
    virtual void OnLoginFailed() = 0;
    virtual void OnLogout() = 0;

    // On change node name
    // (called by Router)
    //
    // !!! При поднятии исходящих соединений вызов OnChangeNodeName()
    //     должен производиться только после создания всех соединений,
    //     иначе может возникнуть состояние 'NOCONNECT' еще до создания
    //     всех соединений и будет произведен принудительный Logout().
    //
    virtual void OnChangeNodeName(const char* name, IRouter::NodeType nodeType, UINT32 loginId) = 0;

    // On change login LUID (before new login)
    virtual void OnChangeLoginLUID(conn::LUID loginLUID) = 0;

    virtual void PostCurrentNodeState(IMsgConnection* pIMsgConn) = 0;

 	virtual ~IUser() {}

    CHECK_FOR_DELETE__(IUser)
};

bool RouterUser_GetService(IModuleServices* pIModSvcs, IUser** ppInterface);
void RouterUser_FreeService(IUser* pInterface);

#endif // !defined(AFX_IUSER_H__4C932BB0_A447_4FA0_A095_8EC32D0EE5B7__INCLUDED_)
