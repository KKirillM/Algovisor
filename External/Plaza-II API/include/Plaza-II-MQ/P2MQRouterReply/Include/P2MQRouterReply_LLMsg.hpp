#ifndef _REPLY_LLMSG_
#define _REPLY_LLMSG_

#include "../../../Plaza-II/P2SysParser/Include/FastBinComposer.h"
#include "../../P2MQRouter/Include/LLMsg.h"

namespace NS_RouterReply {

#define ROUTER_SUFFIX ".router"
#define ROUTER_SUFFIXSZ 8 // с учётом 0

inline P2ERR CreateSendLLMsg_(	IBytesWLenPriority* pIBytes, //out
							  	UINT32 			system_type, 
								UINT32 			flags, 
								UINT64      	id,
								UINT32 			type,
								const char* 	myRouter,
								UINT32 			myRouterSz,
								const char* 	pAddrTo, 	  
								LONGLONG    	timeout,
								const void*  	pData,
								size_t  	 	len)
{ 

	const char* pDot = strchr(pAddrTo,'.');

	if(!pDot)
		return P2ERR_ROUTE_INVALID_ADDRESS;

	const size_t nodeSz = (pDot - pAddrTo);

	typedef  CUncheckedFixedSizeComposer Composer;
	const size_t totalSz = 	Composer::SizeOf<TLLMsgHdr>() 					+
							Composer::SizeOf<UINT64>()					 	+
							Composer::SizeOf<UINT32>()					 	+
							Composer::SizeOf_B(myRouterSz) 			  	 	+
							Composer::SizeOf_2B(nodeSz, ROUTER_SUFFIXSZ) 	+
							Composer::SizeOf<UINT64>()				  	 	+
							Composer::SizeOf_B(static_cast<UINT32>(len));
							

	RETURN_P2ERR(pIBytes->AllocateData(totalSz));
	Composer co(pIBytes->GetData(), pIBytes->GetLen());

	TLLMsgHdr hdr;
	LLMsgHdrSet(&hdr, system_type, totalSz - sizeof(TLLMsgHdr), flags);

	co.ComposeData(&hdr, sizeof(hdr));
	co.Compose<UINT64>(id);
	co.Compose<UINT32>(type);
	co.ComposeB(myRouter, myRouterSz);
	co.Compose_2B(pAddrTo, nodeSz, ROUTER_SUFFIX, ROUTER_SUFFIXSZ);
	co.Compose<UINT64>(timeout);
	co.ComposeB0(pData, static_cast<UINT32>(len));

	return P2ERR_OK;
}



};  // NS_RouterReply


#endif // _REPLY_LLMSG_

