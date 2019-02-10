// interface IRouter
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IROUTER_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_)
#define AFX_IROUTER_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II/P2Common/Include/P2Types.h"
#include "../../../Plaza-II/P2Utils/Include/ServiceDriver.h"
#include "IMsgConnection.h"
#include "ILLMsgStackProc.h"
#include <set>
#include "IModuleServices.h"

//////////////////////////////////////////////////////////////////////
// IRouter
/* ������� ������� �������:
 * ����� ����� (1) NODE.SERVICE
 * ����������� ����� (2) .SERVICE
 * ����������� ����� (3) NODE
 */

// ������� ��� ������������ ����������. ���� ���������� ���-�� �������� �� P2ERR_OK, �� ������������ ������������
// ������������ � ���������� � ��������, �������� ������.
typedef P2ERR (P2CALLSPEC *pfnEnumConn)(const char *name, IMsgConnection *pIMsgConn, bool isDefault, bool isLocal, void *pdata);

struct IRouter 
{
	// ������� ����� ����� �������
	enum AddressType
	{
		ADDR_TYPE_FULL = 0x01,
		ADDR_TYPE_SERVICE_ONLY = 0x02,
		ADDR_TYPE_NODE_ONLY = 0x04
	};

	enum NodeType
	{
		NODE_TYPE_CLIENT,
		NODE_TYPE_SERVER,
		NODE_TYPE_ROOT

	};

	// ���� ����������. �� ����� ����� �������� ���������.
	typedef UINT64 ConnectionKey;

    // ������������ ����� ���������� �����, ������� ���� �� ������ ������������.
    enum ConnectionType
    {
		// �������� ���� ����������. ������������ � GetRoute
        CONN_TYPE_DEF_UP    = 0x01,
        CONN_TYPE_DEF_DOWN  = 0x02,
        CONN_TYPE_DIRECT    = 0x04,
        CONN_TYPE_LOCAL     = 0x08,
		// ��������� ��� ���������� ��������
        CONN_TYPE_UNKNOWN   = 0x80,
		//��� ��������� ����� ������������ ������ � PostBroadcast
		CONN_TYPE_INCOMING  = 0x100,
		CONN_TYPE_OUTGOING  = 0x200
    };
    enum { CONN_TYPE_ALL = 0xff, CONN_TYPE_ALL_DIRECTIONS = 0x300 };

    // Router service
	// ������� ������� ������� (1), (2)
	// disable : ������ �������� � ����������� ������; ����� ����� ���������� (connectionType), ���� ����������� ������� 
	// � ����. ����� �������������� ������ �������� �� ��������� ����� � �� ��������� ������
    virtual P2ERR GetRoute(const char *address, IMsgConnection **ppIMsgConn, ConnectionType *pType = 0, UINT8 disable = 0) const = 0;

	// ������� ������� ������� (3)
	virtual void GetMyAddress(char (&address)[P2_MAX_POSSIBLE_ADDRESS_SIZE + 1]) const = 0;
	// �������� ��� ����� ���� <mynodename>.router.  pLength -- ������  ������������� ������ � ������ ����������� ����
	virtual P2ERR GetMyFullMqAddr(char* pAddressBuffer, size_t *pLength) const = 0;
	virtual P2ERR CheckAddressType(const char *address, UINT32 typeBitMask)const = 0;

	virtual P2ERR GetRouterByAddress(const char *original_address, char *router_address, size_t len)const = 0;
	virtual P2ERR GetConnectionAddress(IMsgConnection *pIMsgConn, char *address, size_t len,/*hint for searching*/ConnectionType connTypeHint)const = 0;

	virtual enum NodeType GetMyType(void) const = 0;
	// ������� ��������� �� ������. ���������� ��� GetRoute+Post
	/*
		������, ��������� � ��������� ������:
		1. ����� ���������� ������ � IMsgConnection::Post
		2. ���� ��������� P2LLMSG_FLAG_TO_ROUTE _�_ address == NULL, �� address ����������� �� ����
		3. ������ ���������� ������ �� ���� ������� �� ����� P2LLMSG_FLAG_P2MSG
		4. ���� ���� P2LLMSG_FLAG_P2MSG ���������, �� ����� ������� �� ���� to
		5. ���� ���� �� ���������, �� ���� �������� �� ��������� ������ "s...."
	*/
    virtual P2ERR Post(const char *address, UINT32 system_type, const void* pBody, size_t len, UINT32 flags)const = 0;
	/* Post �������, ����������� ����������;  � ������� pIBytes �������� LLMsg.  */ 
	virtual P2ERR Post(const char *address, IBytesWLenPriority* pIBytes) const = 0;

	//  ��������� ��������� ����� ����������, �������� ������ connKey.
	//  ���� ���������� � ����� ������ ���(��������, ���� ����� ����), �� ��������� ����� ������� �� address. ���� address == NULL, �� ������� ������ NO_ROUTE
	virtual P2ERR PostSync(const char* pAddrTo, IBytesWLenPriority* pIBytes, UINT64 connKey) = 0;
	
	// �������� ���� �����������
	virtual UINT64 GetConnectionKey(const IMsgConnection* pIMsgConn) const = 0; 

	// ��������� ��������� ��� ��������� �������� LL-���������
	// ����������� ����� address. ���� ��� ��� ����� (������ ��� ����� "��� ����"), �� ������������ P2ERR_OK
	// ���� ���, �� ��������� ���������� �� ��������. � ���� ������ ��� ��������  P2ERR_POSTED_TO_FOREIGN_ADDRESS, ���� ��� ��������� ��� ������ �� �����
	virtual P2ERR PostIfForeignAddress(const char *address, UINT32 system_type, const void* pBody, size_t len, UINT32 flags)const = 0;

    // ������ ��������� ������ �� ������ �������� ����������
    virtual bool IsInLocLinksEnabled() 	const = 0;
    virtual bool IsInRemLinksEnabled()	const = 0;

	/*�������� ���������� ��������� ����������, ������� ������ ���������� ������. �������� �� �������� */
	virtual UINT32 GetOutgoingLinksCount() const = 0;

    // ��������� (���������(login)/�������(logout)) ����� ����
	// - ���� ��� ��������, �� ������� ������ �� ��������
	//   � ��������� �� ������� ��������� �����
	// - � ������ ������ �������������� �������� �������� ������
	virtual P2ERR SetNodeName(const char *address, UINT32 loginId = 0) = 0;

    // ������� ��� ����������� ����������� LLMsg-�� � ����.
    // ���� ������ ����� ���� �������, �� ������ ������� ����� ����������,
    // ���� �� ����� - ���������� 0.
    virtual ILLMsgStackProc* GetLLMsgStackProc() = 0;

	virtual P2ERR ParseAddress(const char *addr, char *node, size_t *pNodeLen, char *service, size_t *pServiceLen)const = 0;

    /* ��������� ��������� ����������� ������ � �� ��� ��� �������� CallBack-�, ��� �������� ���� ������...
       ����� ���������� � �������������� ��������� ����� ����������� ��������� ����,
       �� ���� ������ ����� ���������� ����� ������� PostBroadcast()
	virtual P2ERR EnumConnections(pfnEnumConn fn, void *pdata)const = 0;*/
    virtual P2ERR PostBroadcast(UINT32 system_type, const void* pBody, size_t len, UINT32 flags, ConnectionType connMask)const = 0;

	// ��������� ������� (expandAddr ���������, ����� �� �������� ������ ����� ����������), ������ ��������� ������ ����� ������.
	virtual P2ERR CompareAddresses(const char *addr1, const char *addr2, bool expandAddr, bool *nodesEq, bool *appsEq)const = 0;

    virtual P2ERR GetConnectionType(IMsgConnection *pIMsgConn, ConnectionType* pType)const = 0;

	virtual P2ERR RegisterVirtualConnection(IMsgConnection* pIMsgConn, const char* oposSvcAddr) = 0;
	virtual void UnRegisterVirtualConnection(IMsgConnection* pIMsgConn) = 0;

    // ".a" -> "<node>.a"
    virtual P2ERR MakeFullAddress(const char *src, char *dest, /*[in,out]*/ size_t* pDestSize) const = 0;
	virtual void SetRouterAdmins(const char *src) = 0 ;
	virtual P2ERR GetRouterAdminsStr(char* buf, size_t bufsz) const = 0 ;
    virtual bool isAdmins(const std::string& node_name) const = 0 ;
	virtual P2ERR GetSrcDestFromLLMsg(const UINT32 flags, size_t len,const BYTE* pDta, const char** src, const char** dest) const = 0;
    CHECK_FOR_DELETE__(IRouter)
};



bool RouterRoute_GetService(IModuleServices* pIModuleServices, IRouter** ppIRouter); 
void RouterRoute_FreeService(IRouter* pRouter);

P2ERR LLMsgRoutePingReply_SecurityCheck(const void* body, size_t size, const char* pAddrFrom);


#endif // !defined(AFX_IROUTER_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_)

/*

2nd version
-----------

// ����� ����������:
// -----------------
// - �������� ������� ������� ��������� ������ �������� �������������� ����������
//
// ����������:
// -----------
// ������, ������������ �� ������������� ������������ ������:
//  - IGlobalAuth::
//      ReadConfiguration()
//      SetInfoFromUser()
//  - IRouter::
//      SetNodeName()
//  - ������ ������������������ � ������ ������������ ILinkNotifHandler
//
// - ��� ��������� � ���������� ������ ������� �� ����� �� ���������� ����������
//   ��������� ������, ����������� ILoginCallback, � ������� ������������� ������:
//      - IMsgConnection* ���������� ����������
//      - #login-a
//   ����� IRc-��������� �� ���� ������ + ���.���� �������� � ����������� �����,
//   ��� ������������ ����� IGlobalAuth::SetInfoFromUser(). � ������ ������
//   ������������ ����� ILoginCallback->OnFailure().

----------------------------------------------------------------------------------
- �������� ��������� ������
-- by INI
* �������� ����������� �������
	* ������ ������������ � ������������ � �������� ����������
* �������� ����������� ����
	* ���� ������������ � ������������ � �������� ����������
* ����� TryToConnect
	* .ReadConfiguration
	* ���� ���������� ��� ����c���, �� 
		* ������� IRouter::SetNodeName
			* � ������ ��������� ����� ���������� ��� ����� � ������� ���������
...
--- by user's login
* ����� TryToConnectWithUserInfo � ��������� �� user 
	* .SetUserInfo � ��������� �� user 
		* ���� ��� ��� ���� �� ������ ������
	* ���� ���������� ��� ����c���, �� 
		* ������� IRouter::SetNodeName
			* � ������ ��������� ����� ���������� ��� ����� � ������� ���������
* �� ��������� ������� ����������
	* ������� �������
----------------------------------------------------------------------------------

struct ILoginCallback : public IRc
{
    virtual void OnSuccess() = 0;
    virtual void OnFailure(P2ERR reason) = 0;
};

struct IAuthService
{
	// - ��������� INI
    // - ���� ������� ��������� ������������������ ����������, �� �������
    //   IRouter::SetNodeName(const char* pNodeName)
	virtual P2ERR ReadConfiguration() = 0;
	// �������������� �� ������� �������
    // - ���� ��� ������� ���.����, �� ������� ������
    // - ��������� ���� ������������������ ������
    // - � ������ ������ ������� IRouter::SetNodeName(const char* pNodeName)
    // - ���� ��� ������� ������ OK � pILoginCb != 0, �� ����� �����������
    //   ������ ���� �� ������� ILoginCallback!
	virtual P2ERR SetInfoFromUser(void *auth_blob, UINT32 len, ILoginCallback* pILoginCb) = 0;

    // ������� ���-���������� ������������
	virtual P2ERR CreateConnectionCryptoLayer(IConnCrypto** ppIConnCrypto,
                                IConnCryptoContainer* pIContainer, IConnection* pIConn) = 0;
};

struct IConnCrypto : public IRc
{
    //
    // ��������� ������� ����������
    //

    // - ��������� ���������� (���������/��������) ����� �������� �� IConnection
    // - ������� isDefUpLink ���������� ��� �������� ����� ������� � �������
	virtual void OnConnectComplete(ICs* pICsCaller) = 0;
	virtual void OnClose(int error, ICs* pICsCaller) = 0;

    //
    // ����������
    //

    virtual P2ERR Push(const char* pData, UINT32 len, ICs* pICsCaller) = 0;
    // (����/����� �������� �� ��� ���, ���� ����� �� ������ *pRealLen == 0)
    virtual P2ERR Pop(char* pData, UINT32 maxLen, UINT32 *pRealLen, ICs* pICsCaller) = 0;
    // (��� ���������� ���������� �������������, ����� ������ ����� �����
    //  �����������)
    virtual UINT32 GetPopLen() = 0;
};

struct IConnCryptoContainer : public IRc
{
	virtual void OnLoginSucceeded(bool isLocal,
                                  bool isOposDefault, const char *oposAddr,
                                  ICs* pICsCaller) = 0;
    // ����������, ����� ������� ���.���� �����������, ��� ���� ���������� ��������������
    // �� ����������� (����� ������� �����������).
    // � ������ ������ ������� ������������ ������ ��������� ���������� � �� ��������
    // ���������.
	virtual void OnLoginFailed(P2ERR error, ICs* pICsCaller) = 0;
};

struct IRouter
{
    // ...

    // ������ ��������� ������ �� ������ �������� ����������
    virtual bool IsInLocLinksEnabled() = 0;
    virtual bool IsInRemLinksEnabled() = 0;

    // ��������� (���������(login)/�������(logout)) ����� ����
	// - ���� ��� ��������, �� ������� ������ �� ��������
	//   � ��������� �� ������� ��������� �����
	// - � ������ ������ �������������� �������� �������� ������
	virtual P2ERR SetNodeName(const char *address) = 0;
};

//struct IConnection : ...
//{
//    // ...
//
//    virtual bool IsDefUpLink() = 0;
//};

struct IMsgConnection : ...
{
    // ...

    virtual bool IsDefUpLink() = 0;
};

struct ILinkNotifHandler : public IRc
{
    // - There are no OnOutLocalConnectComplete!!!!
    // - ������� ������� ������� (1), (3). ������ ������������.
    // - ����������� �������� � IMsgConnection
    // - ���� ����������� ���������� �� IMsgConnection (����� ������ ��� ��������� ��������)
    // - ����� ����������� ����� ������ ��� �������� ��������
    virtual void OnLoginSucceeded(IMsgConnection* pIMsgConn, bool isLocal,
                                  bool isOposDefault, const char *oposAddr,
                                  ICs* pICsCaller) = 0;
	virtual void OnLoginFailed(IMsgConnection* pIMsgConn, P2ERR error, ICs* pICsCaller) = 0;
	virtual void OnClose(IMsgConnection *pIMsgConn, int error,
                         ICs* pICsCaller) = 0;
};

struct IModuleServices
{
    // ...

    // ����������� link-�������
    virtual void RegisterLinkNotifHandler(ILinkNotifHandler* pILinkNotifHandler);
    virtual void UnRegisterLinkNotifHandler(ILinkNotifHandler* pILinkNotifHandler);
};

*/
