// P2App.h: interface for the CP2App class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_P2APP_H__A3E65D82_D1A2_4FB1_A654_52452F401D82__INCLUDED_)
#define AFX_P2APP_H__A3E65D82_D1A2_4FB1_A654_52452F401D82__INCLUDED_

#include <map>
#include <string>

using std::string;
using std::map;
using std::pair;

#include "../../../Plaza-II/P2Utils/Include/stuff.h"
#include "../../../Plaza-II/P2Utils/Include/P2Event.hpp"
#include "../../../Plaza-II/P2SysMsg/Include/P2SysMsg.hpp"
#include "../../../Plaza-II/P2SysLog/Include/P2SysLog.h"
#include "../../../Plaza-II-MQ/P2MQ/Include/P2MQ.hpp"
#include "../../../Plaza-II/P2SysTypes/Include/P2SysTypes.hpp"
#include "../../../Plaza-II/P2SysScheme/Include/P2SysScheme.hpp"
#include "../../../Plaza-II-DB/P2DB/Include/P2DB.hpp"

#include "IP2App.h"


#define APP_SECTION             "options:app"
#define MQ_SECTION              "options:mq"
#define DB_SECTION              "options:db"
#define REPL_SECTION            "options:repl"


#define DEFAULT_CONNECT_TIMEOUT       1000
#define DEFAULT_MQ_GET_MSG_TIMEOUT    1000
#define DEFAULT_MQ_SLEEP_DELAY        0
#define DEFAULT_MQ_LOCAL_TIMEOUT      3000



//#define EXIT_EVENT_ENV_VAR_NAME "exitevent" //внешний event должен быть manual-reset

#define EXIT_EVENT_ENV_VAR_NAME "RTSExitEvent" //внешний event должен быть manual-reset

inline void MakeUpperStr(string& str)
{
    for (string::iterator from = str.begin(); from != str.end(); from++)
        *from = toupper(*from);
}

//////////////////////////////////////////////////////////////////////
// CP2App
//////////////////////////////////////////////////////////////////////

class CP2App : public IP2App
{
public:
	CP2App(const char *appName = "P2App");
	virtual ~CP2App();
	
	P2ERR Init(int argc, char *argv[]);
    void Deinit();

	P2ERR Startup();
    void Cleanup();

	P2ERR Connect();
	void Disconnect();

	void RequestLogin(const char *login_str);
	void RequestLogout();

    P2ERR ProcessMsg();

    P2ERR Launch();
    void Stop(P2ERR exitCode); 

	P2ERR CreateDB(); //нужна, если вдруг кто-то захочет работать с базой и ему не нужна репликация

protected:
	virtual P2ERR OnConnectSucceeded(const P2MQ_CONN_HANDLE mqConnection) = 0;
    virtual void OnConnectFailed(P2ERR res, UINT32 errorClass) = 0;
    virtual void OnBeforeDisconnect() = 0;

    virtual P2ERR OnMessage(P2MSG_HANDLE msg) = 0;
	virtual void OnAppIdle() = 0;
	virtual void OnIdleInUnconnectedState() {};
	
	bool IsConnectedToRouter();
	bool IsConnToRouterValid();

	//const P2MQ_CONN_HANDLE GetMQConnection() {return m_hMQConnection;};

protected:
	unsigned               m_MQGetTimeout;
	unsigned               m_mqSleepDelay;
	unsigned               m_mqConnectTimeout;

    volatile bool          m_bExitRequested;

private:
	//static CP2App* m_pInstance;

	enum TAppInitState
	{
		//startup checkpoints
		asNone,
		asSettingsOk,
		asLogOk,
		asExitEventsOk,
		asMQOk,
		asLoginOk
	};

	enum TLoginLogoutRequest
	{
		llrNone,
		llrLogin,
		llrLogout
	};

	char                   m_appName[P2_MAX_POSSIBLE_ADDRESS_SIZE + 1];

	bool                   m_routerLoggedIn;
	TAppInitState          m_initState;

	char                   m_loginStr[P2_MAX_POSSIBLE_ADDRESS_SIZE + 1024 + 1];
	TLoginLogoutRequest    m_llRequest;
	
	P2MQ_CONN_HANDLE       m_hMQConnection;

	P2EVENT_HANDLE         m_externExitEvent;
	P2ERR                  m_exitCode;
		
	P2ERR InitExitEvent();
	void DeinitExitEvent();
	bool WaitForExit(DWORD dwMilliseconds);
		
	P2ERR IniOpen(int argc, char *argv[]);

	P2ERR StartupMQ();
	void CleanupMQ();

	P2ERR RouterLoginStateCheck();
	P2ERR RouterLogin();
	P2ERR WaitForLoginResult(UINT32 loginId);
	P2ERR ProcessLoginMsg(unsigned& routerState);
	void RouterLogout();
	
};

//CP2App* CP2App::m_pInstance = NULL;

//#include "P2App_impl.h"

#endif // !defined(AFX_P2APP_H__A3E65D82_D1A2_4FB1_A654_52452F401D82__INCLUDED_)
