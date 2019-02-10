// P2ReplApp.h: interface for the CP2ReplApp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_P2REPLAPP_H__A119F9A3_738B_4CD1_9C75_FC717BC0AC79__INCLUDED_)
#define AFX_P2REPLAPP_H__A119F9A3_738B_4CD1_9C75_FC717BC0AC79__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996) // warning C4996: This function or variable may be unsafe.
#endif // _MSC_VER

#include "../../../Plaza-II-DB/P2DB/Include/P2DB.h"
#include "../../../Plaza-II-DB/P2Tbl/Include/P2Tbl.hpp"
#include "../../../Plaza-II-Repl/P2ReplServer/Include/P2ReplServer.h"
#include "../../../Plaza-II-Repl/P2ReplClient2/Include/P2ReplClient.h"

#include "../../../Plaza-II-DB/P2ReplISAMCache/Include/P2ReplISAMCache.h"
#include "../../../Plaza-II/P2Sys/Include/P2Dll.hpp"

#include "P2App.h"


#define REPL_SECTION           "options:repl"
#define MAX_APP_NAME_SIZE	   255

#define ACL_TABLE              "ACL"
#define FILTER_TABLE           "Filter"

//////////////////////////////////////////////////////////////////////
// Replication policies
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// CReplServerPolicy - server replication policy base class
//////////////////////////////////////////////////////////////////////
class CReplServerPolicy //: public IReplPolicy
{
protected:
	CReplServerPolicy(PCSTR name) : 
		m_hMQConnection(NULL), m_hDbScheme(NULL), m_hReplScheme(NULL), m_hFilterScheme(NULL),
		m_hReplServer(NULL)
	{
		//TODO: проверять длину, мать ее так!
		if (strlen(name) > MAX_APP_NAME_SIZE-1)
		{
			strncpy(m_name, name, MAX_APP_NAME_SIZE-1);
			m_name[MAX_APP_NAME_SIZE-1] = '\0';
		}
		else
		{
			strcpy(m_name, name);
		}
	}
	virtual ~CReplServerPolicy() {};

	virtual P2ERR DoOpenDbScheme();
	virtual P2ERR DoOpenReplScheme();
	virtual P2ERR DoOpenFilterScheme();

	virtual P2ERR OnBeforeDatabaseOpen() = 0;
	
	virtual P2ERR OnBeforeReplicationOpen(const P2REPL_SERVER_HANDLE hReplServer) = 0;
	virtual P2ERR OnAfterReplicationOpen(const P2REPL_SERVER_HANDLE hReplServer) = 0;
	virtual void OnReplicationClose() = 0;
	virtual void OnAppIdle() = 0;

protected:
	char                    m_name[MAX_APP_NAME_SIZE];
	P2MQ_CONN_HANDLE        m_hMQConnection;
	
	P2DB_SCHEME_HANDLE      m_hDbScheme;
	P2DB_SCHEME_HANDLE      m_hReplScheme;
	P2DB_SCHEME_HANDLE      m_hFilterScheme;
	
	P2REPL_SERVER_HANDLE    m_hReplServer;
	
	P2ERR OpenReplication(const P2MQ_CONN_HANDLE mqConnection);
	BOOL ProcessMessage(P2MSG_HANDLE msg);
	void CloseReplication();

	virtual P2ERR OpenConnection(PCSTR connString) = 0;
	virtual void CloseConnection() = 0;
	virtual P2ERR BindReplServer(PCSTR replStreamName) = 0;

protected:
	P2ERR OpenDatabase();
	void CloseDatabase();
};

//////////////////////////////////////////////////////////////////////
// CReplServerDB - server replication policy using DB
//////////////////////////////////////////////////////////////////////
class CReplServerDB : public CReplServerPolicy
{
protected:
	CReplServerDB() : CReplServerPolicy("ReplServerDBPolicy"), m_hDbConn(NULL) { };
	virtual ~CReplServerDB() { };

	virtual P2ERR OnBeforeDatabaseOpen() {return P2ERR_OK;};
	virtual P2ERR OnDatabaseOpen(const P2DB_SCHEME_HANDLE hDbScheme, const P2DB_CONN_HANDLE hDbConn) = 0;
	virtual void OnDatabaseClose() = 0;

	virtual void OnIdle() = 0;

	//P2ReplServerProcessMessage(0) необходим, чтобы в случае отсутствия MQ сообщений, мог обработаться timeout
	virtual void OnAppIdle() { P2ReplServerProcessMessage(NULL); OnIdle(); }

protected:
	P2DB_CONN_HANDLE    m_hDbConn;

private:
	
	virtual P2ERR OpenConnection(PCSTR connString);
	virtual void CloseConnection();

	virtual P2ERR BindReplServer(PCSTR replStreamName);

	//virtual P2ERR SetACL();
	//virtual P2ERR SetFilter();
};

//////////////////////////////////////////////////////////////////////
// CReplServerISAM - server replication policy using ISAM
//////////////////////////////////////////////////////////////////////

class CReplServerISAM : public CReplServerPolicy
{
protected:
	typedef P2ERR (P2CALLSPEC *P2_BIND_TO_ISAM_FUNC)(P2REPL_SERVER_HANDLE handle, const char *serviceName, P2MQ_CONN_HANDLE connHandle, P2TBL_CONN_HANDLE isamHandle, P2DB_SCHEME_HANDLE replScheme, P2DB_SCHEME_HANDLE filterScheme); 

protected:
	CReplServerISAM(PCSTR appName = "ReplServerISAMPolicy") : CReplServerPolicy(appName), m_hTblConn(NULL) { };
	virtual ~CReplServerISAM() { };
	
	virtual P2ERR OnBeforeDatabaseOpen() { return P2ERR_OK; }
	virtual P2ERR OnDatabaseOpen(const P2DB_SCHEME_HANDLE hDbScheme, const P2TBL_CONN_HANDLE hTblConn) = 0;
	virtual void OnDatabaseClose() = 0;

	virtual void OnIdle() = 0;

	//P2ReplServerProcessMessage(0) необходим, чтобы в случае отсутствия MQ сообщений, мог обработаться timeout
	virtual void OnAppIdle() { P2ReplServerProcessMessage(NULL); OnIdle(); }

	virtual P2ERR BindReplServer(PCSTR replStreamName);

protected:
	P2TBL_CONN_HANDLE    m_hTblConn;

private:
	
	virtual P2ERR OpenConnection(PCSTR connString);
	virtual void CloseConnection();

	P2ERR SetACLFromDb(PCSTR dbConnect, PCSTR tblName);
	P2ERR SetFilterFromDb(PCSTR dbConnect, PCSTR tblName);
	//virtual P2ERR SetACL();
	//virtual P2ERR SetFilter();
};


//////////////////////////////////////////////////////////////////////
// CReplClient - client replication policy
//////////////////////////////////////////////////////////////////////

enum TReplState
{
	rsNone,
	rsReplStartup,
	rsCreateDS,
	rsDBStartup,
	rsOpenDS,
	//rsDBConnReceived,
	rsOnReplOpen
};

class CReplClient
{
protected:
	CReplClient() : m_hMQConnection(NULL), m_hReplStream(NULL),
					m_hDbScheme(NULL), m_replState(rsNone)//, m_hDbConnection(NULL)
		{strcpy(m_name, "ReplClientPolicy");}

	P2ERR OpenReplication(const P2MQ_CONN_HANDLE mqConnection);
	BOOL ProcessMessage(P2MSG_HANDLE msg);
	void CloseReplication();
	void ReopenReplication();

	~CReplClient() {}

	//virtual P2ERR OnDatabaseOpen(const P2DB_SCHEME_HANDLE hDbScheme, const P2DB_CONN_HANDLE hConn) = 0;
	//virtual void OnDatabaseClose() = 0;

	virtual P2ERR OnReplicationOpen(const P2REPL_DATASTREAM_HANDLE hReplDatastream) = 0;
	virtual void OnBeforeReplicationClose() = 0;
	virtual void OnAfterReplicationClose() = 0;
	virtual void OnGetReplicationMessage(const P2DB_RECORD_HANDLE rec, size_t tblIdx, const P2MSG_HANDLE msg) = 0;

	virtual void OnIdle() = 0;

	//P2ReplProcessMessage(m_hMQConnection, 0) необходим, чтобы в случае отсутствия MQ сообщений, мог обработаться timeout
	void OnAppIdle(); // {P2ReplProcessMessage(m_hMQConnection, 0); OnIdle(); }; 

private:
	char                        m_name[MAX_APP_NAME_SIZE];
	P2MQ_CONN_HANDLE            m_hMQConnection; 
	P2REPL_DATASTREAM_HANDLE    m_hReplStream;
	
	P2DB_SCHEME_HANDLE          m_hDbScheme;
	//P2DB_CONN_HANDLE            m_hDbConnection;

	char                        m_DbConnStr[1024];
	TReplState                  m_replState;

	static void ReplClientCB(P2DB_RECORD_HANDLE rec, size_t tblIdx, P2MSG_HANDLE msg, void* cbParams);
	void HandleReplState(P2MSG_HANDLE msg);

	P2ERR StartupDatabase();
	void CleanupDatabase();
	BOOL CheckReplState();
};

//////////////////////////////////////////////////////////////////////
// CP2ReplApp
//////////////////////////////////////////////////////////////////////

//После использовании CP2ReplApp нужно не забывать вызывать Cleanup
template <class CReplPolicy>
class CP2ReplApp : public CP2App, public CReplPolicy 
{
public:

protected:
	CP2ReplApp(const char *appName = "P2ReplApp");

	virtual ~CP2ReplApp() {};

	virtual P2ERR OnRouterConnectSucceeded(const P2MQ_CONN_HANDLE mqConnection) = 0;
    virtual void OnRouterConnectFailed(P2ERR res, UINT32 errorClass) = 0;
    virtual void OnRouterBeforeDisconnect() = 0;

	virtual P2ERR OnGetMessage(P2MSG_HANDLE msg) = 0;
	virtual void OnAppIdle() {CReplPolicy::OnAppIdle();};
private:
	enum TAppState
	{
		//startup checkpoints
		asNone,
		asReplOpenOk
	};
	
	char         m_appName[P2_MAX_POSSIBLE_ADDRESS_SIZE + 1];
	TAppState    m_state;
	
	
	P2ERR OnMessage(P2MSG_HANDLE msg);

	//P2ERR OnRouterLoginSucceeded();
	//P2ERR OnBeforeRouterLogout();
	//P2ERR OnRouterLoginFailed(P2ERR res, UINT32 routerState) {return OnLoginFailed(res, routerState); };
	P2ERR OnConnectSucceeded(const P2MQ_CONN_HANDLE mqConnection);
    void OnConnectFailed(P2ERR res, UINT32 errorClass);
    void OnBeforeDisconnect();
	
};

//////////////////////////////////////////////////////////////////////
// CP2ReplApp
//////////////////////////////////////////////////////////////////////
template <class CReplPolicy>
inline CP2ReplApp<CReplPolicy>::CP2ReplApp(const char *appName) : CP2App(appName), CReplPolicy()
{
	m_state = asNone;

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

template <class CReplPolicy>
inline P2ERR CP2ReplApp<CReplPolicy>::OnMessage(P2MSG_HANDLE msg)
{
	BOOL res = CReplPolicy::ProcessMessage(msg);
	
	if (!res) 
	{
		//Message is not for replication
        if (NULL != msg) {
		    return OnGetMessage(msg);
        }
	}
	return P2ERR_OK;
}


template <class CReplPolicy>
inline P2ERR CP2ReplApp<CReplPolicy>::OnConnectSucceeded(const P2MQ_CONN_HANDLE mqConnection)
{
	RETURN_P2ERR(OnRouterConnectSucceeded(mqConnection));
	
	P2ERR res = P2ERR_OK;
	if (m_state == asNone)
	{
		if (P2FAILEDV(CReplPolicy::OpenReplication(mqConnection), res))
		{
			P2TRACE_ERROR_WITH_CODE_EX((res, m_appName, "Can't startup replication"));
            return res;
		}
		
		m_state = asReplOpenOk;
	}
		
	return res;
}

template <class CReplPolicy>
inline void CP2ReplApp<CReplPolicy>::OnConnectFailed(P2ERR res, UINT32 errorClass)
{
	OnRouterConnectFailed(res, errorClass);
}

template <class CReplPolicy>
inline void CP2ReplApp<CReplPolicy>::OnBeforeDisconnect()
{
	if (m_state == asReplOpenOk)
	{
		CReplPolicy::CloseReplication();
		m_state = asNone;
		P2TRACE_INFO_EX((m_appName, "Replication subsystem was closed"));
	}

	OnRouterBeforeDisconnect();
}

//#include "P2ReplApp_impl.h"

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#endif // !defined(AFX_P2REPLAPP_H__A119F9A3_738B_4CD1_9C75_FC717BC0AC79__INCLUDED_)
