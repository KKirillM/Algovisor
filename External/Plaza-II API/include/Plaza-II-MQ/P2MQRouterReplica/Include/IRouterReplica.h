

#ifndef __IROUTER_REPLICA_H__
#define __IROUTER_REPLICA_H__

#include "../../../Plaza-II/P2Common/Include/P2Errors.h" 
#include "../../../Plaza-II/P2Utils/Include/ServiceDriver.h"
#include "../../P2MQRouter/Include/IModuleServices.h"
#include "../../P2MQRouter/Include/IRouter.h"


struct IRouterReplica 
{ 
    virtual void Shutdown() = 0; 
	virtual void OnChangeNodeName(const char* name, IRouter::NodeType nodeType) = 0;
};

bool RouterReplica_GetService(IModuleServices* pIModSvcs, IRouterReplica** ppInterface); 
void RouterReplica_FreeService(IRouterReplica* pInterface);


#endif



