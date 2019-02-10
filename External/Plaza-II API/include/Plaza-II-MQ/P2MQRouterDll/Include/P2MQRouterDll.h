#ifndef _P2MQROUTER_DLL_H_
#define _P2MQROUTER_DLL_H_

#include "../../../Plaza-II/P2Common/Include/P2Errors.h"


#ifdef  __cplusplus
extern "C" {
#endif

#ifdef P2MQROUTERDLL_EXPORTS
#define P2MQRouter_API P2_API_EXPORTS
#else
#define P2MQRouter_API P2_API_IMPORTS
#endif

struct IMQTransportPluginImpl;
P2MQRouter_API  P2ERR P2MQRouterStartup(const char* options);
P2MQRouter_API  void P2MQRouterCleanup(); 
P2MQRouter_API 	IMQTransportPluginImpl* P2MQEmbeddedRouterPlugin();

#ifdef  __cplusplus
}
#endif

#endif // _P2MQROUTER_DLL_H_
