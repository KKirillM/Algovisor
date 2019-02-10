// interface IAuthDriver
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IAUTHDRIVER_H__5A4C5C25_2A12_496D_BF39_DFFA6214FC9E__INCLUDED_)
#define AFX_IAUTHDRIVER_H__5A4C5C25_2A12_496D_BF39_DFFA6214FC9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../P2MQRouter/Include/IConnHandler.h"
#include "../../P2MQRouter/Include/IConnCrypto.h"
#include "../../P2MQRouter/Include/ICryptoConnHandler.h"
#include "../../P2MQRouter/Include/IConnection.h"
#include "../../../Plaza-II/P2Utils/Include/ServiceDriver.h"
#include "../../P2MQRouter/Include/IModuleServices.h"

struct IAuthDriver  : public IMQPluginImpl<2, 0> 
{
    // Аутентификация из INI
    // - Попытаться прочитать конфигурацию и в случае успеха заполнить nodeName
	virtual P2ERR ReadConfiguration(PSTR nodeName, size_t nodeSize) = 0;
	// Аутентификация по внешней команде
    // - разобрать блок аутентификационных данных
    // - в случае успеха заполнить nodeName
	virtual P2ERR SetAuthInfo(const void *auth_blob, size_t authLen,
                              PSTR nodeName, size_t nodeSize) = 0;
    virtual void ClearAuthInfo() = 0;

	virtual P2ERR CreateConnectionCryptoLayer(
                    IConnHandler** ppIConnHandler, IConnCrypto** ppIConnCrypto,
                    IConnection* pIConn, ICryptoConnHandler* pICryptoConnHandler,
                    bool isDefUpLink) = 0;
    // Должен вернуть ICryptoConnHandler*, который был передан в CreateConnectionCryptoLayer(),
    // при этом можно и себе оставить копию.
    virtual void DestroyConnectionCryptoLayer(
                    IConnHandler* pIConnHandler, ICryptoConnHandler** ppICryptoConnHandler) = 0;

    CHECK_FOR_DELETE__(IAuthDriver)
};

#endif // !defined(AFX_IAUTHDRIVER_H__5A4C5C25_2A12_496D_BF39_DFFA6214FC9E__INCLUDED_)
