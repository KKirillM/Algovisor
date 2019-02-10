// IProgramMonitor.h: IProgramMonitor interface
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPROGRAMMONITOR_H__4BD310AF_76B1_4D5F_81FE_990E37F73868__INCLUDED_)
#define AFX_IPROGRAMMONITOR_H__4BD310AF_76B1_4D5F_81FE_990E37F73868__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II/P2Utils/Include/ServiceDriver.h"
#include "../../P2MQRouter/Include/IModuleServices.h"

struct IProgramMonitor 
{
	enum SHUTDOWN_METHOD
	{
		NO_WAIT,
		WAIT 
	};

    virtual P2ERR Start() = 0;
    virtual void  Shutdown(SHUTDOWN_METHOD method) = 0;
	virtual bool  HasPrograms() const = 0;
}; 

bool RouterPM_GetService(IModuleServices* pIModSvcs, IProgramMonitor** ppInterface); 
void RouterPM_FreeService(IProgramMonitor* pInterface);

#endif // !defined(AFX_IPROGRAMMONITOR_H__4BD310AF_76B1_4D5F_81FE_990E37F73868__INCLUDED_)
