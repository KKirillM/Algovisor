// interface IModuleServices
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMODULESERVICES_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_)
#define AFX_IMODULESERVICES_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II/P2Common/Include/P2Types.h"
#include "../../../Plaza-II/P2Utils/Include/stuff.h"
#include "../../../Plaza-II/P2Utils/Include/IRc.h"
#include "../../../Plaza-II/P2Utils/Include/QueueMan.h"
#include "LLMsg.h"
#include "ILLMsgTypeProc.h"
#include "ConnLUID.h"

#if defined(__linux__) || defined(__MACH__)
#define HANDLE void*
#endif

//////////////////////////////////////////////////////////////////////
// IModuleServices
//
//  ��������� ������� � ���������� ������� ������
//  (���������� ����������� � ��������)
//
//  - ���������� ���������� ����� �������� ����� ������, �.�. ���������
//    �� ���������� ����������, ������� ���������� ����� IModuleServices,
//    ����� ������������� ������
//    (������� ����� ����� ��� ������, � ��� ����� � ��������� ����������� ������
//     ����� ��������� �������, ����������� ���������� ����������)
//  - ��������� �� ���������� ���������� ������������ (� �.�. ����� ������������)
//    ��� �������� ������
//  - ������ ��������� ����������� ���������� ����� ����������� ��������.
//    (��������, ���� ������, ����������� ������������� ���������� ���������,
//     ��� �� ������)
//    ??? �������, ��������, ��� ��������� ������������� ������, ����� �������, � ���������,
//    ���������� IModuleServices*, �� ����� ����������� ���������� ����������???
//  - �������� ���������� ����������� � ������������ ������� � ���� ������������ �����
//    ������������, ������� ��� ������������� ���������� ����������� � ������� �����
//    ������������� �������� (#include) ��������������� ������������ �����.
//

struct IRouter;
struct IIocp;
struct IScheduler;
struct IMsgConnection;
struct IReplyService;
struct ILLMsgTypeProc;
struct IAuthService;
struct IUser;
struct IServices;
struct ILinkNotifHandler;
struct ILoginCallback;
struct ILogoutCallback;
struct IBytesWLen;
struct IStat;
struct IProgramMonitor;
struct ITimeSyncService;
struct IThreadWatchDog;
struct IRouterReplica;

struct ICallInDedicatedThread : public IRc, public CQueueElem 
{
    virtual void Process() = 0;
};

struct NullCallInDedicatedThread : public ICallInDedicatedThread
{
	LONG AddRef1()  {return 0;}
	LONG Release1() {return 0;}
	void Process()  {}
};

struct IModuleServices
{
	enum{ MAJOR_VERSION = 24, MINOR_VERSION = 1};
    //
    // Global interfaces
    //
    //  - ������ ������ �� ������������
    //  - ����� ������� 0 (���� ��������� ��� �� ������)
    //
    virtual IRouter* GetRouter() = 0;
    virtual IIocp* GetIocp() = 0;
    virtual IScheduler* GetScheduler() = 0;
    virtual IReplyService* GetReplyService() = 0;
    virtual IAuthService* GetAuthService() = 0;
    virtual IUser* GetUser() = 0;
    virtual IServices* GetServices() = 0;
	virtual IStat* GetStat() = 0;
#if defined(_WIN32)
    virtual IProgramMonitor* GetProgramMonitor() = 0;
#endif //_WIN32
    virtual ITimeSyncService* GetTimeSync() = 0;
	virtual IThreadWatchDog*  GetThreadWatchDog() = 0;
    
#if !defined(__MACH__)
	virtual IRouterReplica*   GetRouterReplicaService() = 0;
#endif //__MACH__
    //
    // Global functions
    //
    // (������� ������ ����� ��������������,
    //  ������� �� ������������� ��������)
    //

    // Create outgoing connection
    virtual P2ERR CreateMsgConnection(LPCSTR host, UINT port,
                                      IMsgConnection** ppIMsgConn,
                                      bool isDefUpLink) = 0;

    // Get LUID (birthday) of last created connection
    virtual conn::LUID GetLastConnLUID() = 0;
    virtual conn::LUID GetNextConnLUID() = 0;

    // Close all connections
    enum
    {
        DISCONNECT_ALL_OUTGOING     = 0x01,
        DISCONNECT_ALL_INGOING      = 0x02
    };
    virtual P2ERR DisconnectAllConnections(bool setDead, bool graceful,
                                           BYTE type = DISCONNECT_ALL_OUTGOING | DISCONNECT_ALL_INGOING) = 0;

    // Process LL-message
    // (����������� ��������������� ��������� ��������� �������������� �������,
    //  �.�. ��� ������ ������������� ���, ��� �������� ���� �����!)
	/*
		�� ������ ��� ��, ��� ���������� �� ������������ ������������� 
		��������� - ������ ��� ������ ����, ��� �������� ��������� �� ���������!
		� ���� ��, � �� ������, ��� ����� �������� ���� ����� �� 
		����������� ������. ��������, CDataDisp ������� � ����������� ������ 
		��������� ���������� ���� ������ � �������������� LL-��������� 
		��������� � �������, � �����, ����� �� ����������� ������ � 
		�������������� � ����� ������ �������� ���������� ��� LL-��������� 
		�� �������. (� ������� ������ CSTP(STP.h) ���������� ��������, 
		�������������, ��� ��� ������� ���������� � ������ ������ ������� 
		������ ���� ����� ������������ LL-��������� �� ������� �������)
	 */
    virtual P2ERR LLMsgProcess(PTLLMsgHdr pHdr, const void* pBody,
                               IMsgConnection* pIMsgConn) = 0;
    virtual P2ERR LLMsgProcess(UINT32 system_type, const void* pBody, size_t len,
                               IMsgConnection* pIMsgConn, UINT32 flags = 0) = 0;
    virtual P2ERR LLMsgProcess(IBytesWLenPriority *pIBytes, IMsgConnection* pIMsgConn) = 0;

    // Register processor for type range of LL-messages
    virtual P2ERR LLMsgTypeProcAdd(UINT32 type_from, UINT32 type_to, ILLMsgTypeProc* pIProc,  bool override = false) = 0;
    virtual P2ERR LLMsgTypeProcAdd(UINT32 type_from, UINT32 type_to, ILLMsgTypeProc2* pIProc, bool override = false) = 0;

    // ����������� link-�������
    // (��� ���������� ������ 2-������� ��������� ��������� �����������
    //  ����� ���������� ����������� ������ ��� �������������� �������������
    //  ������)
    virtual void RegisterLinkNotifHandler(ILinkNotifHandler* pILinkNotifHandler) = 0;
    virtual void UnRegisterLinkNotifHandler(ILinkNotifHandler* pILinkNotifHandler) = 0;

    // Set auth.info by user
    virtual void SetAuthInfoByUser(ILoginCallback* pILoginCb, IBytesWLen* pIAuthData) = 0;
    virtual void Logout(ILogoutCallback* pILogoutCb) = 0;

    // ���������� ������������� ����� callback-������� � ���������� ���������� ������
    typedef void (P2CALLSPEC *SyncCB)(void* cbData);
    virtual void CallInDedicatedThread(SyncCB cbFunc, void* cbData) = 0;
    virtual void CallInDedicatedThread(ICallInDedicatedThread* pICall) = 0;
    virtual bool IsCurrentThreadDedicated() = 0;

    // ��������� ������� �����
    virtual void AsyncShutdown(P2ERR exitCode, const std::string& reason) = 0;
	virtual PCSTR LLMsgMakeLogDescr(PCTLLMsgHdr pLLHdr, const void* pBody, char* const buf, unsigned size) = 0;
	virtual PCSTR LLMsgMakeLogDescrEx(PCTLLMsgHdr pLLHdr, const void* pBody, char* const buf, unsigned size) = 0;
								
	virtual void SetTitle(void) = 0;

    CHECK_FOR_DELETE__(IModuleServices)
};

// P2MQRouter INI section name
#define P2MQROUTER_INI_SECTION "P2MQROUTER"

#endif // !defined(AFX_IMODULESERVICES_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_)
