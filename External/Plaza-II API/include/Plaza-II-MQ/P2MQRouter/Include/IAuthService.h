// interface IAuthService
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IAUTHSERVICE_H__C57329EE_5417_4497_8232_F9D03AD42DB8__INCLUDED_)
#define AFX_IAUTHSERVICE_H__C57329EE_5417_4497_8232_F9D03AD42DB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II/P2Utils/Include/IRc.h"
#include "IConnHandler.h"
#include "IConnCrypto.h"
#include "IConnection.h"
#include "ICryptoConnHandler.h"
#include "IMsgConnection.h"
#include "IModuleServices.h"
#include "../../../Plaza-II/P2Utils/Include/ServiceDriver.h"

struct ILoginCallback : public IRc
{
    virtual void OnSuccess(UINT32 loginId) = 0;
    virtual void OnFailure(P2ERR reason) = 0;
    virtual IMsgConnection* GetMsgConnection() = 0;
    virtual void OnCloseConnection() = 0;

    CHECK_FOR_DELETE__(ILoginCallback)
};

struct ILogoutCallback : public IRc
{
    virtual void OnSuccess() = 0;
    virtual void OnFailure(P2ERR reason) = 0;
    virtual const void* GetMsgConnection_() = 0; // without AddRef, just address
    virtual IMsgConnection* GetMsgConnection() = 0;

    CHECK_FOR_DELETE__(ILogoutCallback)
};

struct IAuthService : public IMQPluginImpl<5, 0> 
{
    // !!!
    // ReadConfiguration(), SetInfoFromUser(), Logout() должны вызываться
    // только из управляющего потока. Из других потоков нужно
    // вызывать аналогичные методы из IModuleServices.
    // !!!

	// - прочитать INI
    // - если удалось причитать аутентификационную информацию, то вызвать
    //   IRouter::SetNodeName(const char* pNodeName)
	virtual P2ERR ReadConfiguration() = 0;
	// Аутентификация по внешней команде
    // - если уже имеется аут.инфа, то вернуть ошибку
    // - разобрать блок аутентификационных данных
    // - в случае успеха вызвать IRouter::SetNodeName(const char* pNodeName)
    // - если эта функция вернет OK и pILoginCb != 0, то будет обязательно
    //   вызван один из методов ILoginCallback!
	virtual P2ERR SetInfoFromUser(const void *auth_blob, size_t len, ILoginCallback* pILoginCb) = 0;
    virtual void Logout(ILogoutCallback* pILogoutCb) = 0;

	virtual P2ERR CreateConnectionCryptoLayer(
                    IConnHandler** ppIConnHandler, IConnCrypto** ppIConnCrypto,
                    IConnection* pIConn, ICryptoConnHandler* pICryptoConnHandler,
                    bool isDefUpLink) = 0;
    // Должен вернуть ICryptoConnHandler*, который был передан в CreateConnectionCryptoLayer(),
    // при этом можно и себе оставить копию.
    virtual void DestroyConnectionCryptoLayer(
                    IConnHandler* pIConnHandler, ICryptoConnHandler** ppICryptoConnHandler) = 0;

    virtual P2ERR VerifyLocPwd(PCSTR appName, PCSTR locPwd, bool fromLocalHost) = 0;

    CHECK_FOR_DELETE__(IAuthService)
};


bool RouterCrypto_GetService(IModuleServices* pIModSvcs, IAuthService **ppInterface);
void RouterCrypto_FreeService(IAuthService* pInterface);



#endif // !defined(AFX_IAUTHSERVICE_H__C57329EE_5417_4497_8232_F9D03AD42DB8__INCLUDED_)
