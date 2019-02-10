#ifndef _P2MQROUTERUSER_H_
#define _P2MQROUTERUSER_H_

#include "../../P2MQRouter/Include/LLMsg.h" 

P2ERR LLMsgTracertBack_SecurityCheck(const void* body, size_t size, const char* pAddrFrom);
P2ERR LLMsgPost_SecurityCheck(const void* body, size_t size, const char* pAddrTo, const char* pAddrFrom);

#endif // _P2MQROUTERUSER_H_
