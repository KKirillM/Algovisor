// interface IServices
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISERVICES_H__C57329EE_5417_4497_8232_F9D03AD42DB8__INCLUDED_)
#define AFX_ISERVICES_H__C57329EE_5417_4497_8232_F9D03AD42DB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II/P2Utils/Include/ServiceDriver.h"
#include "../../P2MQRouter/Include/IRouter.h"
#include "../../../Plaza-II/P2Utils/Include/IRc.h"

enum TServiceType
{
	SERVICE_TYPE_LOCAL,
	SERVICE_TYPE_GLOBAL,
	SERVICE_TYPE_NEIGHBOUR
};


struct IServResolveCallback : public IRc
{
    virtual void OnSuccess(PCSTR pSvcName, PCSTR pAddr) = 0;
    virtual void OnFailure(PCSTR pSvcName, P2ERR reason) = 0;
};

struct IServOperCallback : public IRc
{
    virtual void OnSuccess() = 0;
    virtual void OnFailure(P2ERR reason) = 0;
};

struct IServices 
{
    virtual void Resolve(IServResolveCallback* pICallback, PCSTR pSvcName, UINT32 timeout,
                         const PCSTR* ppExcludes = 0, size_t exclCount = 0) = 0;

    // јдрес сервиса беретс€ из 'addr'.
    // ≈сли addr - пуста€ строка, то адрес извлекаетс€ из pIMsgConn с помощью IRouter::GetConnectionAddress()
    // (вызов производитс€ из управл€ющего потока).
    virtual void RegisterLocalService(IServOperCallback* pICb, PCSTR svcName, PCSTR addr, enum TServiceType serviceType,
                                      IMsgConnection* pIMsgConn = 0) = 0;
    virtual void UnregisterLocalService(IServOperCallback* pICb, PCSTR svcName, PCSTR addr,
                                        IMsgConnection* pIMsgConn = 0) = 0;

    virtual void OnChangeNodeName(const char* name, IRouter::NodeType nodeType) = 0;

    CHECK_FOR_DELETE__(IServices)
};

bool RouterServices_GetService(IModuleServices* pIModSvcs, IServices **ppInterface); 
void RouterServices_FreeService(IServices* pInterface);


namespace NS_RouterServices {
	// Ёкспортируетс€ в публичном интерфейсе дл€ security-проверок
	P2ERR LLMsgResolveFromDown_ParseAddrFromAndId(const void* body, size_t size, const char** ppAddrFrom, UINT64* id);
}


#endif // !defined(AFX_ISERVICES_H__C57329EE_5417_4497_8232_F9D03AD42DB8__INCLUDED_)




