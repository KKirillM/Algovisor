#if !defined(AFX_IP2APP_H__B3899553_7F07_4AEA_9255_C1FFDE2ABDF5__INCLUDED_)
#define AFX_IP2APP_H__B3899553_7F07_4AEA_9255_C1FFDE2ABDF5__INCLUDED_

#include "../../../Plaza-II/P2SysTypes/Include/P2SysTypes.h"
#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II-MQ/P2MQ/Include/P2MQ.h"

struct IP2App
{
    virtual ~IP2App() { };

    virtual P2ERR Init(int argc, char *argv[]) = 0;
    virtual void Deinit() = 0;

    virtual P2ERR Startup() = 0;
    virtual void Cleanup() = 0;

    virtual void RequestLogin(const char *login_str) = 0;
    virtual void RequestLogout() = 0;

    virtual P2ERR ProcessMsg() = 0;

    virtual P2ERR Launch() = 0;
    virtual void Stop(P2ERR exitCode) = 0;

    virtual P2ERR CreateDB() = 0;

protected:
    /*virtual P2ERR OnAfterConnect(P2MQ_CONN_HANDLE mqConnection) = 0;
    virtual void OnConnectFailed(P2ERR res, UINT32 errorClass) = 0;
    virtual P2ERR OnBeforeDisconnect() = 0;

    virtual P2ERR OnRouterLoginSucceeded() = 0;
    virtual P2ERR OnRouterLoginFailed(P2ERR res, UINT32 routerState) = 0;
    virtual P2ERR OnBeforeRouterLogout() = 0;*/
	
    virtual P2ERR OnConnectSucceeded(const P2MQ_CONN_HANDLE mqConnection) = 0;
    virtual void OnConnectFailed(P2ERR res, UINT32 errorClass) = 0;
    virtual void OnBeforeDisconnect() = 0;

    virtual P2ERR OnMessage(P2MSG_HANDLE msg) = 0;
    virtual void OnAppIdle() = 0;
    virtual void OnIdleInUnconnectedState() = 0;
    //
};

#endif // !defined(AFX_IP2APP_H__B3899553_7F07_4AEA_9255_C1FFDE2ABDF5__INCLUDED_)
