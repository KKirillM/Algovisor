#ifndef _P2MQ_REPLY_H_
#define _P2MQ_REPLY_H_

#ifdef P2MQREPLY_EXPORTS
#define P2MQREPLY_API //__declspec(dllexport)
#else
#define P2MQREPLY_API //__declspec(dllimport)
#endif

#include "../../../Plaza-II/P2Utils/Include/IRc.h"
#include "../../P2MQRouter/Include/LLMsg.h"
#include "../../P2MQRouter/Include/IMsgConnection.h"
#include "../../P2MQRouter/Include/IModuleServices.h"
#include "../../../Plaza-II/P2Utils/Include/WeakMsgID.h"
#include "../../../Plaza-II/P2Utils/Include/ServiceDriver.h"
#include "../../../Plaza-II/P2SysMsg/Include/P2SysMsg.h"


/*
��������� ����-������� ������� ������:

�������� ����-������� ������ ��� ������������� ��� ��������� ������-�����.

��� ����������� ������� �� ����-��������� ������������ ��� ��������� �� ���������.
���, ��� ��������� ������ �� ������������ ������������ P2LL_MSG_USER_SEND

��� � ��� �������� �� ������� ����������:
1. �������� ISendReplySeri�e
2. ������� ISendReplySeri�e->Send � �������� � ���� ���������, IReplyCallback, ��� ����������� � ����� ��������
3. ���������� ������. ������ ����� ��������� ����������
4. �� ������ IReplyCallback->Reply ���������� ������
5. �� ������ IReplyCallback->ReplyFailed ���������� ������ �������

��� � ��� �������� � ����������� �������:
1. ���������� ��������� �������� ���������. 
2. ��������� �������������� ���������� ������� � ���������� ����� ����� ISendReplySeri�e->reply

//////

��� ������ ������ ��������

����������:
1. ��������� IReplyCallback
2. ��������

�����������
1. ������� ��� ������ ��� �����
2. � ����������� ���� �������� �����

������ �� ���������� �������
1. ��������� �����
2. ���� ��
3. ���������� ������� � ��������
4. ������������ �������� ������ � �������� ��������
5. �������� ������� ��� ��������

������ �� ����������� �������
1. ��������� ������
2. �������������� �� � �����������
3. ��������� ������ �� �����������
4. �������� ��
*/

// ����������� ���������� ������
struct IReplyCallback : public IRc
{
	virtual P2ERR Reply(PCTLLMsgHdr pHdr, const void* pBody, IMsgConnection* pIMsgConn) = 0;
	virtual P2ERR ReplyFailed(P2ERR reason, const WeakID& pid) = 0;
};


// ����������� ��������-������������ �������
struct IReplyService 
{
    // ��������� ��� ��������������� timeout-a �� UINT32 (milliseconds)
    // � LONGLONG (100-nanosecond intervals): LONGLONG ns = ms * TIMEOUT_MS
    enum { TIMEOUT_MS = 10000 };
    static LONGLONG SendTimeFromMs(int ms)
    {
        return static_cast<LONGLONG>(ms) * TIMEOUT_MS;
    }

	// ������� �����. �� ���� �������� �������������� ����� (pData, len)
	//	, ��� ����� ������ type, ��������� ��������� ������ pCallback � id ����������� ���������
	virtual P2ERR Send(const char *to, const void *pData, size_t len, LONGLONG timeout, UINT32 type, IReplyCallback *pCallback, WeakID& id, UINT32 flags) = 0;

	// ����������� ������.
	// ���������� �������������� ����� (pData, len)
	// � �������� ���������. �����! ���������� ��������� �� _���_�����_���������!
	virtual P2ERR Reply(const void *pData, size_t len, const TLLMsgHdr *pOrigHdr, UINT32 flags) = 0; 
	virtual P2ERR ReplyEx(const void *pData, size_t len, const WeakID& id, UINT32 flags) = 0;

	virtual P2ERR GetIDByHeader(const TLLMsgHdr *pOrigHdr, WeakID& id) = 0;
	virtual P2ERR GetFromByHeader(const TLLMsgHdr *pOrigHdr, char *from, size_t len) = 0;
	
	// ��������� ������� Send �� ��������� id � ���������� �������. �����, ���� id ����������� � ����
	virtual P2ERR GetNewSendID(WeakID& id) = 0;
	virtual P2ERR SendWithID(const char *to, const void *pData, size_t len, LONGLONG timeout, UINT32 type, IReplyCallback *pCallback, const WeakID& id, UINT32 flags) = 0;

	// ������� ��� �������� ����� ������. �����, ���� ����� �� ���� ����� ������ ������ ����. ������� �� ���������
	// ������� ���������� �� ������� ���� � ������������ ������� ���� �� ���� to. orig_address - ����� ���������� send
	virtual P2ERR ReSend(const char *to, const void *pData, size_t len, const TLLMsgHdr *pOrigHdr, UINT32 flags) = 0;
	virtual P2ERR ReSendEx(const char *to, const void *pData, size_t len, UINT32 type, const WeakID& id, UINT32 flags) = 0; 

	// ���������� �����, addr_to ������� �� ��������� � pExcludeAddr
	virtual P2ERR ProcessNotMySENDMessage(const char* pExcludeAddr, const TLLMsgHdr* pHdr, const void* pBody, IMsgConnection* pIMsgConn, TLLMsgHdr* pOriginalMsgHdr, const void** ppOriginalMsgBody) = 0; 

	// ������� ��������� � ������� ������.
	virtual P2ERR CreateLLMsgReply(	IBytesWLenPriority* pIBytes,
									const UINT64 id,
									const char* pAddrTo,
									const void* pBody,
									size_t 		bodySz) const = 0;

	virtual P2ERR CreateLLMsgReply( IBytesWLenPriority* pIBytes, 
									const UINT64 id,
									const char* pAddrTo,
									P2MSG_FIELD* msgFields,
									size_t 		 msgFieldsCnt) const  = 0; 

	virtual P2ERR SimpleSendWithID(	const char *pAddrTo, 
									const void *pData, 
									size_t 		len, 
									LONGLONG 	timeout, 
									UINT32 		type, 
									UINT64 		id, 
									UINT32 		flags) const = 0;
   virtual ~IReplyService() {}
};


bool RouterReply_GetService(IModuleServices *pServices, IReplyService **pInterface);
void RouterReply_FreeService(IReplyService * pReplyService);



P2ERR LLMsgSend_SecurityCheck(const void* pBody, size_t size, const char* pAddrTo, const char* pAddrFrom);
P2ERR LLMsgReply_SecurityCheck(const void* pBody, size_t size, const char* pAddrFrom);



#endif // _P2MQ_REPLY_H_
