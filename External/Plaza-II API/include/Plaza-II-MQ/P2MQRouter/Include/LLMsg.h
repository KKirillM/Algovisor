// LLMsg.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LLMSG_H__47E65331_5347_40EB_90E4_18542063DFC2__INCLUDED_)
#define AFX_LLMSG_H__47E65331_5347_40EB_90E4_18542063DFC2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../Plaza-II/P2Common/Include/P2Types.h"
#include "../../../Plaza-II/P2Common/Include/P2Errors.h"

//////////////////////////////////////////////////////////////////////
// Low level message header

#pragma pack(push, r1,1)
struct TLLMsgHdr
{
    char    signature[4];
    UINT32  version;
    UINT32  size;
    UINT32  system_type;
    UINT32  flags;
};
#pragma pack(pop, r1)

typedef TLLMsgHdr *PTLLMsgHdr;
typedef const TLLMsgHdr *PCTLLMsgHdr;

// Maximum body length of low level message
#define LLMSG_MAX_BODY_LENGTH   0x1000000

// Flags
#define P2LLMSG_FLAG_TO_ROUTE       0x00000001
#define P2LLMSG_FLAG_P2SYSMSG       0x00000002
#define P2LLMSG_FLAG_PRIORITY_MASK  0x0000FF00  // Second byte used for priority

inline 
UINT32 LLMsgMakePriorityFlag(UINT8 priority)
{
    return priority << 8;
}

inline 
void LLMsgSetPriority(UINT32& flags, UINT8 priority)
{
//    flags |= priority << 8;
    flags |= LLMsgMakePriorityFlag(priority);
}

inline 
BYTE LLMsgGetPriority(UINT32 flags)
{
    return static_cast<BYTE>((flags & P2LLMSG_FLAG_PRIORITY_MASK) >> 8);
} 

#define P2LLMSG_PRIORITY_DEFAULT   LLMsgMakePriorityFlag(P2MSG_PRIORITY_DEFAULT)
#define P2LLMSG_PRIORITY_LOW       LLMsgMakePriorityFlag(P2MSG_PRIORITY_LOW)
#define P2LLMSG_PRIORITY_NORMAL    LLMsgMakePriorityFlag(P2MSG_PRIORITY_NORMAL)
#define P2LLMSG_PRIORITY_HIGH      LLMsgMakePriorityFlag(P2MSG_PRIORITY_HIGH)
#define P2LLMSG_PRIORITY_VERY_HIGH LLMsgMakePriorityFlag(P2MSG_PRIORITY_VERY_HIGH)
#define P2LLMSG_PRIORITY_SYSTEM    LLMsgMakePriorityFlag(P2MSG_PRIORITY_SYSTEM)

//////////////////////////////////////////////////////////////////////

inline 
UINT32 LLMsgMakeDisabledRoutes(UINT8 disabledRoutes)
{
	return static_cast<UINT32>(disabledRoutes) << 16;
}

inline 
UINT8 LLMsgGetDisabledRoutes(UINT32 flags)
{
	return static_cast<UINT8>((flags & 0x00FF0000) >> 16);
} 

// Connection type strings

#define P2ROUTER_CONN_TYPE_LOCAL    "Local"
#define P2ROUTER_CONN_TYPE_NORMAL   "Normal"

// ***********************************************************
// Утилиты лля работы с заголовком

void LLMsgHdrSet(TLLMsgHdr* pLLHdr, UINT32 system_type, size_t len, UINT32 flags);
P2ERR LLMsgHdrCheck(const TLLMsgHdr* pHdr);

// ***********************************************************
// Утилиты для проверки адресов
// Адрес - это строка вида <nodename>.<appname> или <nodename>

inline 
bool nodename_end(char c)
{
	return c == '\0' || c == '.'; 
} 

// Проверка равенства логинов в адресах
inline
bool nodename_equal(const char* a, const char* b) 
{
	if (!a || !b) return false;
	while (!nodename_end(*a) && *a == *b)
	{
		++a;
		++b; 
	}	
	return nodename_end(*a) && nodename_end(*b);
} 

// ПРоверка равенства логинов в 3х адресах.
// То же самое, что и nodename_equal(a, b) && nodename_equal(a, c)
inline
bool nodename_equal(const char* a, const char* b, const char* c) 
{
	if (!a || !b || !c) return false;
	while (!nodename_end(*a) && *a == *b && *a == *c)
	{
		++a;
		++b; 
		++c;
	}	
	return nodename_end(*a) && nodename_end(*b) && nodename_end(*c);
} 

inline
bool nodename_empty(const char* a)
{
	return a != NULL && nodename_end(*a);
}

inline
bool appname_router(const char* a)
{
	if (!a) return false;
	a = strchr(a, '.'); 
	return a != NULL && 0 == strcmp(++a, "router");
} 


#endif // !defined(AFX_LLMSG_H__47E65331_5347_40EB_90E4_18542063DFC2__INCLUDED_)
