// P2App_impl.h: implementation of the CP2App class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_P2APP_IMPL_H__A3E65D82_D1A2_4FB1_A654_52452F401D82__INCLUDED_)
#define AFX_P2APP_IMPL_H__A3E65D82_D1A2_4FB1_A654_52452F401D82__INCLUDED_

#include "P2App.h"
#include "../../../Plaza-II/P2Utils/Include/P2Process.hpp"
#include "../../../Plaza-II/P2Sys/Include/ProcessParameters.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996) // warning C4996: This function or variable may be unsafe.
#endif // _MSC_VER

inline bool P2IsEmptyStr(const char *s)
{
	if (NULL == s)
		return true;
	if ('\0' == s[0])
		return true;
	return false;
}

//////////////////////////////////////////////////////////////////////
// CP2App
//////////////////////////////////////////////////////////////////////

CP2App::CP2App(const char *appName) : 
    m_MQGetTimeout(DEFAULT_MQ_GET_MSG_TIMEOUT),
    m_mqSleepDelay(DEFAULT_MQ_SLEEP_DELAY),
    m_mqConnectTimeout(DEFAULT_CONNECT_TIMEOUT),
	m_bExitRequested(false),
	m_routerLoggedIn(false),
	m_initState(asNone), 
	m_llRequest(llrNone),
	m_hMQConnection(NULL),
	m_externExitEvent(NULL),
    m_exitCode(0)
{
	if (strlen(appName) > P2_MAX_POSSIBLE_ADDRESS_SIZE)
	{
		strncpy(m_appName, appName, P2_MAX_POSSIBLE_ADDRESS_SIZE);
		m_appName[P2_MAX_POSSIBLE_ADDRESS_SIZE] = '\0';
	}
	else
	{
		strcpy(m_appName, appName);
	}
}

CP2App::~CP2App(void)
{
}

P2ERR CP2App::InitExitEvent()
{
	P2ERR res;
	if(P2FAILEDV(P2ProcessGetRTSExitEvent(&m_externExitEvent),res) && res != P2ERR_COMMON_NO_SUCH_ITEM)
	{
		return res; 
	}
	else if(res == P2ERR_COMMON_NO_SUCH_ITEM)
	{ 
		RETURN_P2ERR(P2EventCreate(&m_externExitEvent, true, false)); 
		P2TRACE_INFO_EX((m_appName, "CP2App created EXTERN_EXIT_EVENT")); 
	} 
	else
	{ 
		P2TRACE_INFO_EX((m_appName, "CP2App got EXTERN_EXIT_EVENT"));
	}

	return P2ERR_OK;
}

void CP2App::DeinitExitEvent()
{ 
	_VERIFY_P2ERR(P2EventClose(m_externExitEvent));
	m_externExitEvent = NULL;
}

bool CP2App::WaitForExit(DWORD dwMilliseconds)
{ 
	return (P2ERR_COMMON_TIMEOUT == P2EventWait(m_externExitEvent, dwMilliseconds));
}

P2ERR CP2App::StartupMQ()
{
	P2ERR res = P2ERR_OK;
	if (P2FAILEDV(P2MQStartup(), res))
	{
		P2TRACE_ERROR_WITH_CODE_EX((res, m_appName, "P2MQStartup() failed"));
	}
	else
	{
		if (P2FAILEDV(P2MQCreateConnection(&m_hMQConnection), res))
		{
			P2MQCleanup();
			P2TRACE_ERROR_WITH_CODE_EX((res, m_appName, "Can't create MQ connection"));
		}
		P2TRACE_INFO_EX_0((m_appName, "MQ connection was created, mq-conn 0x%p", m_hMQConnection));
	}
	return res;
}

void CP2App::CleanupMQ()
{
	if (m_hMQConnection != NULL)
	{
		P2MQFreeConnection(m_hMQConnection);
		m_hMQConnection = NULL;
	}

	P2MQCleanup();
}

P2ERR CP2App::IniOpen(int argc, char *argv[])
{
	string iniFile;
    for (int i = 1; i < argc && iniFile.empty(); i++)
	{
		char* pArg = argv[i];
		if (*pArg == '/' || *pArg == '-')
		{
			pArg++;
			if (char* pSep = strchr(pArg, ':'))
			{
				string token(pArg, pSep - pArg);
				MakeUpperStr(token); 
				if (token == "INI")
					iniFile.assign(pSep + 1);
			}
		}
	}

	if (iniFile.empty()) 
	{
		printf("Error: incorrect command line; no ini-file specified \n");
		printf("Options:\n  /ini:<ini-file> - use custom ini-file \n"); 
		return P2ERR_INI_FILE_NOT_FOUND;
	}

	return P2IniOpen(iniFile.c_str());
}

P2ERR CP2App::Init(int argc, char *argv[])
{
	P2ERR res = P2ERR_OK;

	if (m_initState == asNone)
	{
		if (P2FAILEDV(IniOpen(argc, argv), res))
		{
			P2SysPrintf("Can't open settings subsystem; err %R\n", res);
			return res;
		}
		m_initState = asSettingsOk;
#ifdef _WIN32
		if (P2FAILEDV(InitializeProcessParamsFromIni(GetCurrentProcess(), APP_SECTION), res))
		{
			P2SysPrintf("Couldn't initialize process parameters from ini section '%s'\n", APP_SECTION);
			return res; 
		}
#endif // _WIN32
		P2SysPrintf("Settings subsystem was opened\n");
	}

	if (m_initState == asSettingsOk)
	{
		if (P2FAILEDV(P2LogOpen(P2LOG_DEF_INI_SECTION), res))
		{
			P2IniClose();
			m_initState = asNone;
			P2SysPrintf("Can't open log-file; iniSection %s; err %R\n", P2LOG_DEF_INI_SECTION, res);
			return res;
		}
		m_initState = asLogOk;
		P2TRACE_INFO_EX((m_appName, "Log was opened"));
		P2LogCurrentProcessInfo();
	}

	return res;
}

void CP2App::Deinit()
{
	if (m_initState == asLogOk)
	{
		P2LogClose();
		m_initState = asSettingsOk;
		P2SysPrintf("Log-file was closed\n");
	}

	if (m_initState == asSettingsOk)
	{
		P2IniClose();
		m_initState = asNone;
		P2SysPrintf("Settings subsystem was closed\n");
	}
}

P2ERR CP2App::Startup()
{
	P2ERR res = P2ERR_OK;

	if (m_initState < asLogOk)
	{
		P2SysPrintf("Application was not inited\n");
	}
	
	if (m_initState == asLogOk)
	{
		if (P2FAILEDV(InitExitEvent(), res))
		{
			P2TRACE_ERROR_EX((m_appName, "Cannot init exit event"));
			return res;
		}
		m_initState = asExitEventsOk;
	}

	if (m_initState == asExitEventsOk)
	{
		if (P2FAILEDV(StartupMQ(), res))
		{
			P2TRACE_ERROR_EX((m_appName, "Cannot create MQ connection"));
			return res;
		}
		m_initState = asMQOk;
	}

	return res;
}

void CP2App::Cleanup()
{
	if (IsConnectedToRouter())
	{
		Disconnect();
	}

	if (m_initState == asMQOk)
	{
		CleanupMQ();
		m_initState = asExitEventsOk;
		P2TRACE_INFO_EX((m_appName, "MQ subsystem was stopped"));
	}

	if (m_initState == asExitEventsOk)
	{
		DeinitExitEvent();
		m_initState = asLogOk;
		P2TRACE_INFO_EX((m_appName, "Exit event was deinited"));
	}

	Deinit();
}


P2ERR CP2App::Connect()
{
	P2ERR res = P2ERR_OK;

	if (!IsConnToRouterValid())
	{
		UINT32 errorClass = P2MQ_ERRORCLASS_OK;
		
		char mqConnectStr[1024];
		memset(mqConnectStr, 0, sizeof(mqConnectStr));
		P2IniGetStr(mqConnectStr, sizeof(mqConnectStr), MQ_SECTION, "connect", "");
		
		m_mqConnectTimeout = P2IniGetInt(MQ_SECTION, "connect_timeout", DEFAULT_CONNECT_TIMEOUT);
				
		m_MQGetTimeout = P2IniGetInt(MQ_SECTION, "msg_timeout", DEFAULT_MQ_GET_MSG_TIMEOUT);
		m_mqSleepDelay = P2IniGetInt(MQ_SECTION, "sleep_delay", DEFAULT_MQ_SLEEP_DELAY);

		if (P2FAILEDV(P2MQConnectEx(m_hMQConnection, mqConnectStr, m_mqConnectTimeout, &errorClass), res))
		{
			P2TRACE_ERROR_WITH_CODE_EX((res, m_appName, "Cannot establish connection to router; connection string \'%s\'", mqConnectStr));

			if (P2ERR_COMMON_WRONG_ARGUMENTS == res) errorClass = P2MQ_ERRORCLASS_IS_USELESS;
			
			if (m_llRequest == llrNone)
			{
				OnConnectFailed(res, errorClass);
			}
						
			//!!!! this->Stop(P2ERR_COMMON_EXITCODE_FATAL);
			
			return res;
		}
		
		P2TRACE_INFO_EX((m_appName, "Connection to router was established; connection string \'%s\', mq-conn 0x%p", mqConnectStr, m_hMQConnection));

		if (m_llRequest == llrNone)
		{
			res = OnConnectSucceeded(m_hMQConnection);
		}
	}
	else
	{
		if (m_llRequest == llrNone)
		{
			res = OnConnectSucceeded(m_hMQConnection);
		}
	}

	return res;
}

void CP2App::Disconnect()
{
	if (m_llRequest == llrNone)
	{
		OnBeforeDisconnect();
	}
	
	P2MQDisconnect(m_hMQConnection);

	m_routerLoggedIn = false;

	P2TRACE_INFO_EX((m_appName, "Application was disconnected"));
}

P2ERR CP2App::ProcessLoginMsg(unsigned& routerState)
{
	P2ERR res = P2ERR_OK;
	P2AutoMessage hMsgA;

	if (P2FAILEDV(P2MQGetMsg(m_hMQConnection, &hMsgA, m_MQGetTimeout), res))
	{
		if (res == P2ERR_MQ_TIMEOUT)
		{
			res = P2ERR_OK;
		}
		else
		{
			P2TRACE_ERROR_WITH_CODE_EX((res, m_appName, "P2MQGetMsg() failed"))
			return res;
		}
	}

	PCSTR pMsgCat = NULL;
	UINT32 msgType;
	if (hMsgA() != NULL &&
		P2SUCCEEDED(P2MsgGetStrField(hMsgA(), P2MSG_PROP_CATEGORY, &pMsgCat)) &&
		strcmp(pMsgCat, P2MSG_CATEGORY_SYSTEM) == 0 &&
		P2SUCCEEDED(P2MsgCopyFieldUINT32(hMsgA(), P2MSG_PROP_TYPE, &msgType)) &&
		msgType == P2MSG_TYPE_SYSTEM_ROUTER_STATE)
	{
		UINT32 mqState;
		if (P2SUCCEEDEDV(P2MQGetStatus(m_hMQConnection, &mqState), res))
		{
			routerState = mqState & P2MQ_ROUTER_STATUS_MASK;
		}
		else
		{
			P2TRACE_ERROR_WITH_CODE_EX((res, m_appName, "P2MQGetStatus() failed"))
			//!!!! Stop(res);
		}
	}
	return res;
}

P2ERR CP2App::WaitForLoginResult(UINT32 loginId)
{
	UINT32 timeout = P2IniGetInt(MQ_SECTION, "login_timeout", 10000);
	
	DWORD startTime = GetTickCount();
	do
	{
		P2ERR res;

		unsigned routerState = 0;
		if (P2FAILEDV(ProcessLoginMsg(routerState), res))
		{
			return res;
		}

		UINT32 stateLoginId;
		if (P2FAILEDV(P2MQGetStateLoginId(m_hMQConnection, &stateLoginId), res))
		{
			P2TRACE_ERROR_WITH_CODE_EX((res, m_appName, "Couldn't get state login_id"))
			return res;
		}

		// ѕропускаем сообщени€ о смене состо€ни€ роутера,
		// которые были посланы до нашего вызова Login()
		if (stateLoginId != loginId) continue;

		if (routerState == P2MQ_ROUTER_CONNECTED)
		{
			P2TRACE_INFO_EX((m_appName, "Login succeded"));
			return P2ERR_OK;
		}
		else if (routerState == P2MQ_ROUTER_LOGINFAILED)
		{
			P2TRACE_INFO_EX((m_appName, "Login failed"));
			return P2ERR_MQ_ACCESS_DENY;
		}
		else if (routerState == P2MQ_ROUTER_NOCONNECT)    
		{                                                   
			P2TRACE_INFO_EX((m_appName, "Router reached reconnect limit"));
			return P2ERR_MQ_ACCESS_DENY;
		}
		else
			continue; 
	}
	while (GetTickCount() - startTime < timeout);
	
	P2TRACE_INFO_EX((m_appName, "Login timeout"));
	
	return P2ERR_MQ_TIMEOUT;
}

P2ERR CP2App::RouterLogin()
{
	UINT32 loginId;
	P2ERR res = P2ERR_OK;
	
	if (!m_routerLoggedIn)
	{
		while(true)
		{
			if (P2FAILEDV(P2MQLogin(m_hMQConnection, m_loginStr, &loginId), res))
			{
				if (res == P2ERR_MQCRYPT_ALREADY_LOGGED_IN)
				{
					P2TRACE_ERROR_WITH_CODE_EX((res, m_appName, "Application was already logged in"));

					if (P2FAILEDV(P2MQLogout(m_hMQConnection), res))
					{
						P2LogFatalWithCodeEx(res, m_appName, "P2MQLogout() failed");
						//!!!! this->Stop(res);
						return res;
					}
					
					P2TRACE_INFO_EX((m_appName, "Application has logged out"));
					
					continue;
				}
				else
				{
					P2TRACE_ERROR_WITH_CODE_EX((res, m_appName, "P2MQLogin failed"))
					if (m_llRequest != llrNone)
					{
						OnConnectFailed(res, P2MQ_ERRORCLASS_IS_USELESS);
					}
					return res;
				}
			}
			break;
		}

		if (P2FAILEDV(WaitForLoginResult(loginId), res))
		{
			P2ERR err;
			if (P2FAILEDV(P2MQLogout(m_hMQConnection), err))
			{
				P2LogFatalWithCodeEx(err, m_appName, "P2MQLogout() failed");
				//!!!! this->Stop(err);
			}
			if (m_llRequest != llrNone)
			{
				OnConnectFailed(res, P2MQ_ERRORCLASS_IS_USELESS);
			}
			return res;
		}

		m_routerLoggedIn = true;

		if (m_llRequest != llrNone)
		{
			res = OnConnectSucceeded(m_hMQConnection);
		}
	}
	else
	{
        if (m_llRequest != llrNone)
		{
			res = OnConnectSucceeded(m_hMQConnection);
		}
	}
	
	return res;
}

void CP2App::RouterLogout()
{
	if (m_routerLoggedIn)
	{
		if (m_llRequest != llrNone)
		{
			OnBeforeDisconnect();
		}
		
		P2ERR res;
		if (P2FAILEDV(P2MQLogout(m_hMQConnection), res))
		{
			P2LogFatalWithCodeEx(res, m_appName, "P2MQLogout() failed");
			//!!!! this->Stop(res);
		}
		m_routerLoggedIn = false;

		P2TRACE_INFO_EX((m_appName, "Router was logged out"));
	}
}

P2ERR CP2App::RouterLoginStateCheck()
{
	P2ERR res = P2ERR_OK;;
	if (m_llRequest == llrLogin)
	{
		res = RouterLogin();
	}
	else if (m_llRequest == llrLogout)
	{
		RouterLogout();
	}
	return res;
}


P2ERR CP2App::Launch()
{
	do
	{
		if (P2FAILED(Connect()))
		{
			RouterLogout();
			Disconnect();
			
			OnIdleInUnconnectedState();

            if (!WaitForExit(0) || m_bExitRequested) {
                break;
            }
			continue;
		}
		// раньше здесь было m_MQGetTimeout, но это просто лишн€€ задержка на каждом цикле
		// ждать нужно в ProcessMsg, а здесь просто быстро провер€ть
		while (WaitForExit(0) && !m_bExitRequested)
		{
			if (P2FAILED(RouterLoginStateCheck()))
			{
				break;
			}
			
			if (P2FAILED(ProcessMsg()))
			{
				break;
			}

			if (m_mqSleepDelay != 0)
			{
				Sleep(m_mqSleepDelay);
			}
		}

		RouterLogout();
		Disconnect();

        if (m_bExitRequested) {
            P2EventSet(m_externExitEvent);
        }
	}
    while (WaitForExit(m_mqConnectTimeout));

    if (!m_bExitRequested)
    {
        P2TRACE_INFO_EX((m_appName, "Extern exit event received"));
    }
	return m_exitCode;
}

void CP2App::Stop(P2ERR exitCode)
{
	m_exitCode = exitCode;

#ifdef _WIN32
	if (P2FAILED(exitCode))
	{
		if (m_initState >= asLogOk)
		{
			P2TRACE_ERROR_WITH_CODE_EX((exitCode, m_appName, "CP2App::Stop() executed;err %R", exitCode));
		}
		else
		{
			P2SysPrintf("CP2App::Stop() executed;err %R", exitCode);
		}
	}
	else
	{
		P2TRACE_INFO_EX((m_appName, "CP2App::Stop() was called"));
	}
#endif // _WIN32
    m_bExitRequested = true;
}

void CP2App::RequestLogin(const char *login_str)
{
	strncpy(m_loginStr, login_str, sizeof(m_loginStr) - 1);
	m_llRequest = llrLogin;
	
	P2TRACE_INFO_EX((m_appName, "Login has requested"));
}

void CP2App::RequestLogout()
{
	m_llRequest = llrLogout;
	P2TRACE_INFO_EX((m_appName, "Logout has requested"));
}

P2ERR CP2App::ProcessMsg()
{
	P2ERR res = P2ERR_OK;
	P2AutoMessage hMsgA;
	
	if (P2FAILEDV(P2MQGetMsg(m_hMQConnection, &hMsgA, m_MQGetTimeout), res))
	{
		if (res == P2ERR_MQ_TIMEOUT)
		{
			OnAppIdle();
			res = P2ERR_OK;
		}
		else
		{
			P2TRACE_ERROR_WITH_CODE_EX((res, m_appName, "P2MQGetMsg() failed"))
			return res;
		}
	}
	else
	{
		res = OnMessage(hMsgA());
	}

	return res;
}

P2ERR CP2App::CreateDB()
{
	// Get db-scheme name
    char schemeName[P2DB_MAX_DB_NAME_SIZE + 1];
    P2IniGetStr(schemeName, sizeof(schemeName), DB_SECTION, "dbscheme", "");

    // Get connection string
    char dbConnStr[1024];
    P2IniGetStr(dbConnStr, sizeof(dbConnStr), DB_SECTION, "connect", "");

    // Load db-scheme
    AutoSchemeHandle hDbSchemeA;
    P2ERR res;
    if (P2FAILEDV(P2SchemeCreateSchemeFromIni(&hDbSchemeA, schemeName), res))
    {
        P2TRACE_ERROR_WITH_CODE_EX((res, m_appName, "Couldn't load db-scheme from ini"
                                    ";scheme-name '%s'", schemeName))
        return res;
    }

    AutoConnectionHandle hDdbConnA;
    if (P2FAILEDV(P2DBOpenConnection(hDbSchemeA, dbConnStr, &hDdbConnA), res))
    {
        P2TRACE_ERROR_WITH_CODE_EX((res, m_appName, "Couldn't open db-connection for create db"
                                    ";scheme-name '%s';db-conn '%s'", schemeName, dbConnStr))
        return res;
    }

    size_t tblCount;
    if (P2FAILEDV(P2SchemeGetTableCount(hDbSchemeA, &tblCount), res))
    {
        P2TRACE_ERROR_WITH_CODE_EX((res, m_appName, "Couldn't get tables count from scheme"))
    }
    else
    {
        for (UINT32 i = 0; i < tblCount; i++)
        {
            AutoTableSchemeHandle hTblSchemeA;
            if (P2FAILEDV(P2SchemeGetTableByIndex(hDbSchemeA, i, &hTblSchemeA), res))
            {
                P2TRACE_ERROR_WITH_CODE_EX((res, m_appName, "Couldn't get table scheme from db scheme"
                                            ";tblIdx %u", i))
                return res;
            }

            // Drop table (without check)
            P2DBDeleteTable(hDdbConnA, hTblSchemeA);

            // Create table
            
            if (P2FAILEDV(P2DBCreateTable(hDdbConnA, hTblSchemeA), res))
            {
                P2TRACE_ERROR_WITH_CODE_EX((res, m_appName, "Couldn't create table"
                                            ";tblIdx %u", i))
                return res;
            }
        }
    }

    P2TRACE_INFO_EX((m_appName, "Database created; scheme-name \'%s\';connection string \'%s\'", schemeName, dbConnStr))

    return res;
}

bool CP2App::IsConnectedToRouter()
{
	P2ERR err = P2ERR_OK;
	unsigned status = 0;
	
	if(m_hMQConnection == NULL)
	{
		return false;
	}
	
	if (P2FAILEDV(P2MQGetStatus(m_hMQConnection, &status), err))
	{
		P2TRACE_ERROR_WITH_CODE_EX((err, m_appName, "Cannot get mq-conn status"));
		return false;
	}
	
	if ((status & P2MQ_CONNECTION_CONNECTED) == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool CP2App::IsConnToRouterValid()
{
    P2ERR err = P2ERR_OK;
	unsigned status = 0;
	if (P2FAILEDV(P2MQGetStatus(m_hMQConnection, &status), err))
	{
		P2TRACE_ERROR_WITH_CODE_EX((err, m_appName, "Cannot get mq-conn status"));
		return false;
	}
	
	if ( ((status & P2MQ_CONNECTION_CONNECTED) != 0) && ((status & P2MQ_CONNECTION_INVALID) == 0) )
	{
		return true;
	}
	else
	{
		return false;
	}   
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#endif // !defined(AFX_P2APP_IMPL_H__A3E65D82_D1A2_4FB1_A654_52452F401D82__INCLUDED_)
