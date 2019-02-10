#ifndef _P2_EVENTMSG_H_
#define _P2_EVENTMSG_H_

#include "../../../Plaza-II/P2SysTypes/Include/P2SysTypes.h"

#define P2MSG_CATEGORY_EVENT      "_Event_"

enum
{
    P2MSG_TYPE_EVENT_ADD_EVENT
};

typedef struct 
{
	INT64		i8;	
	UINT64		u8;
	P2TIME		t;	
	P2BCDII(P2BCDIIMAKETYPE(16, 5))	d;
	char		c[256];
	
} P2EventMessage;


#endif
