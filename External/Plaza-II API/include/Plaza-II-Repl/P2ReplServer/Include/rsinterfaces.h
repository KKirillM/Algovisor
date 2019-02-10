#ifndef _P2REPLSERVER_RSINTERFACES_H_
#define _P2REPLSERVER_RSINTERFACES_H_

#include "../../../Plaza-II-Repl/P2ReplServer/Include/IComposable.h"
#include "../../../Plaza-II-Repl/P2ReplCommon/Include/IP2ReplDS.hpp"
#include "../../../Plaza-II-Repl/P2ReplServer/Include/P2ReplServer.h"

#define DEF_RSINTERFACES_VERSION    1

struct IRSInterfacesVersionTester
{
    virtual P2ERR TestVersion(UINT32 reqVersion);
};

enum ClientState {  CLIENT_STATE_ONLINE,
                    CLIENT_STATE_SNAPSHOT};

struct IReplServerNotified : public IRSInterfacesVersionTester
{   
    // Подключение клиента
    virtual P2ERR OnClientConnecting(IComposable * pClient) = 0;  
    // Отключение клиента
    virtual void  OnClientDisconnecting(IComposable * pClient) = 0;
    // Изменение состояния клиента Snapshot/Online
    virtual void  OnClientStateChanged(IComposable * pClient, ClientState state) = 0;
    // Изменение ACL сервера
    virtual void  OnACLChanged(P2ACL_HANDLE acl) = 0; 
};

struct IReplServer : public IRSInterfacesVersionTester
{  
    // Установить уведомляемого по событиям сервера репликации
    virtual P2ERR           SetNotified(IReplServerNotified* pNotified) = 0;
    // Отсылка данных клиенту
    virtual P2ERR           Commit(IComposable * pClient, TTblChanges1 *pTblChg, size_t tblCnt) = 0;
    // Очистка старых данных таблицы для данного клиента
    virtual P2ERR           ClearOldRevs(IComposable * pClient, size_t tblIdx, P2REPL_REVISION_TYPE rev, UINT32 flags) = 0;
    // Разорвать соединение с клиентом
    virtual void            Disconnect(IComposable * pClient, P2ERR reason) = 0;
    // Получить ACL
    virtual P2ACL_HANDLE    GetServerACL() = 0;
};

struct IReplServerPerUserSender : public IRSInterfacesVersionTester
{   
    // Получить IP2ReplDS
    virtual IP2ReplDS*              GetReplDS() const = 0;
    // Cхема репликации 
    virtual P2DB_SCHEME_HANDLE      GetReplSchema() const = 0;    
    // Номер жизни клиента
    virtual P2ERR                   GetClientLifeNumber(IComposable * pClient, /*OUT*/ UINT32* pLifeNum) const = 0; 
};

inline P2ERR IRSInterfacesVersionTester::TestVersion(UINT32 reqVersion)
{
    if( DEF_RSINTERFACES_VERSION != reqVersion )
        return P2ERR_COMMON_INVALID_INTERFACE_VERSION;

    return P2ERR_OK;
}

#endif // _P2REPLSERVER_RSINTERFACES_H_
