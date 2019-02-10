#ifndef _ROUTERREPLOPTS_H_
#define _ROUTERREPLOPTS_H_
#include "../../../Plaza-II/P2Common/Include/P2Types.h"

struct RouterReplOpts 
{
	char   appname[P2_MAX_POSSIBLE_ADDRESS_SIZE + 1]; 
	UINT32 protocol_version;
	UINT32 mq_service_type; 
	UINT32 msg_delay;
	UINT32 data_timeinterval;
}; 


#endif //_ROUTERREPLOPTS_H_






