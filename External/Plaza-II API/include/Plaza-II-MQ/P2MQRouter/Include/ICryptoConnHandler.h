// interface ICryptoConnHandler
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICRYPTOCONNHANDLER_H__12457C94_A838_458E_9D50_3D731D54086E__INCLUDED_)
#define AFX_ICRYPTOCONNHANDLER_H__12457C94_A838_458E_9D50_3D731D54086E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II/P2Utils/Include/IRc.h"
#include "../../../Plaza-II/P2Utils/Include/ICs.h"
#include "ICall.h"

struct ICryptoConnHandler : public IRc
{
	virtual void OnLoginSucceeded(bool isLocal, bool isOposDefault, bool isOposTrusted, const char *oposAddr, ICs* pICsCaller, ICall* pICallOnFinish) = 0;
    // Вызывается, когда текущая аут.инфа некорректна.
	virtual void OnLoginFailed(P2ERR error, ICs* pICsCaller) = 0;
    virtual void OnClose(int error, bool isDead, ICs* pICsCaller) = 0;
	virtual void OnRecv(const void* pData, size_t len, ICs* pICsCaller) = 0;

    CHECK_FOR_DELETE__(ICryptoConnHandler)
};

#endif // !defined(AFX_ICRYPTOCONNHANDLER_H__12457C94_A838_458E_9D50_3D731D54086E__INCLUDED_)
