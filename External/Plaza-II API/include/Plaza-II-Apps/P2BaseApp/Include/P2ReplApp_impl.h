// P2ReplApp.cpp: implementation of the CP2ReplApp class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_P2REPLAPP_IMPL_H__A3E65D82_D1A2_4FB1_A654_52452F401D82__INCLUDED_)
#define AFX_P2REPLAPP_IMPL_H__A3E65D82_D1A2_4FB1_A654_52452F401D82__INCLUDED_

#include "P2ReplApp.h"

#if defined(__linux__) || defined (__MACH__)
#include <limits.h>  // for PATH_MAX constant
#define MAX_PATH PATH_MAX
#endif

// Replication policies
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// CReplServerPolicy
//////////////////////////////////////////////////////////////////////

P2ERR CReplServerPolicy::DoOpenDbScheme()
{
	P2ERR res = P2ERR_OK;
	char dbSchemeName[1024];
	P2IniGetStr(dbSchemeName, sizeof(dbSchemeName), DB_SECTION, "dbscheme", "");

	char dbSchemeIni[1024];
	P2IniGetStr(dbSchemeIni, sizeof(dbSchemeIni), DB_SECTION, "dbschemeini", "");
	if (strcmp(dbSchemeIni, "") == 0)
	{
		if (P2FAILEDV(P2SchemeCreateSchemeFromIni(&m_hDbScheme, dbSchemeName), res))
		{
			P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot create db-scheme \'%s\' from ini file", dbSchemeName));
			return res;
		}
	}
	else
	{
		if (P2FAILEDV(P2SchemeCreateSchemeFromIni2(&m_hDbScheme, dbSchemeIni, dbSchemeName), res))
		{
			P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot create db-scheme \'%s\' from custom ini file %s", dbSchemeName, dbSchemeIni));
			return res;
		}
	}
	P2TRACE_INFO_EX_0((m_name, "Db-scheme \'%s\' created, scheme 0x%p", dbSchemeName, m_hDbScheme));

	return res;
}

P2ERR CReplServerPolicy::DoOpenReplScheme()
{
	P2ERR res = P2ERR_OK;

	char replSchemeName[1024];
	memset(replSchemeName, 0, sizeof(replSchemeName));
	P2IniGetStr(replSchemeName, sizeof(replSchemeName), REPL_SECTION, "replscheme", "");

	char replSchemeIni[MAX_PATH];
	P2IniGetStr(replSchemeIni, sizeof(replSchemeIni), REPL_SECTION, "replschemeini", "");
	if (strcmp(replSchemeIni, "") == 0)
	{
		if (P2FAILEDV(P2SchemeCreateSchemeFromIni(&m_hReplScheme, replSchemeName), res))
		{
			P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot create repl-scheme from ini file %s", replSchemeIni));
			return res;
		}
	}
	else
	{
		if (P2FAILEDV(P2SchemeCreateSchemeFromIni2(&m_hReplScheme, replSchemeIni, replSchemeName), res))
		{
			P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot create repl-scheme from custom ini file"));
			return res;
		}
	}
	P2TRACE_INFO_EX_0((m_name, "Repl-scheme \'%s\' created, scheme 0x%p", replSchemeName, m_hReplScheme));

	return res;
}

P2ERR CReplServerPolicy::DoOpenFilterScheme()
{
	P2ERR res = P2ERR_OK;

	char filterSchemeName[1024];
	filterSchemeName[0]=0;
	P2IniGetStr(filterSchemeName, sizeof(filterSchemeName), REPL_SECTION, "filterscheme", "");

	if (strcmp(filterSchemeName, "") == 0)
	{
		return P2ERR_NOT_OK;
	}

	char filterSchemeIni[MAX_PATH];
	P2IniGetStr(filterSchemeIni, sizeof(filterSchemeIni), REPL_SECTION, "filterschemeini", "");
	if (strcmp(filterSchemeIni, "") == 0)
	{
		if (P2FAILEDV(P2SchemeCreateSchemeFromIni(&m_hFilterScheme, filterSchemeName), res))
		{
			P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot create filter-scheme from ini;scheme-name \'%s\'", filterSchemeName));
			return res;
		}
	}
	else
	{
		if (P2FAILEDV(P2SchemeCreateSchemeFromIni2(&m_hFilterScheme, filterSchemeIni, filterSchemeName), res))
		{
			P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot create filter-scheme \'%s\' from custom ini file \'%s\'", filterSchemeName, filterSchemeIni));
			return res;
		}
	}
	P2TRACE_INFO_EX_0((m_name, "Filter-scheme \'%s\' created, scheme 0x%p", filterSchemeName, m_hFilterScheme));

	return res;
}


P2ERR CReplServerPolicy::OpenDatabase()
{
	P2ERR res = P2ERR_OK;

	RETURN_P2ERR(OnBeforeDatabaseOpen());
	
	if (P2FAILEDV(P2DBStartup(), res))
	{
		P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot startup DB"));
		return res;
	}

	if (P2FAILEDV(DoOpenDbScheme(), res))
	{
		P2DBCleanup();
		return res;
	}

	if (P2FAILEDV(DoOpenReplScheme(), res))
	{
        P2SchemeFreeScheme(m_hDbScheme);
        m_hDbScheme = NULL;
        P2DBCleanup();

		return res;
	}

	if (P2FAILEDV(DoOpenFilterScheme(), res))
	{
		if (res == P2ERR_NOT_OK)
		{
			res = P2DBMakeNewPtrScheme(m_hReplScheme, &m_hFilterScheme);
		}

		if (P2FAILED(res))
		{
			P2SchemeFreeScheme(m_hReplScheme);
			m_hReplScheme = NULL;

			P2SchemeFreeScheme(m_hDbScheme);
			m_hDbScheme = NULL;
			P2DBCleanup();

			return res;
		}
	}
	
	char dbConnStr[1024];
	const char errStr[] = "P2_ERROR_STR";
	P2IniGetStr(dbConnStr, sizeof(dbConnStr), DB_SECTION, "connect", errStr);
	if (strncmp(dbConnStr, errStr, strlen(errStr)) == 0)
	{
		P2TRACE_ERROR_WITH_CODE_EX((P2ERR_COMMON_NOT_INITIALIZED, m_name, "Cannot read conn-str from ini"));
        P2SchemeFreeScheme(m_hDbScheme);
		m_hDbScheme = NULL;
		P2SchemeFreeScheme(m_hReplScheme);
		m_hReplScheme = NULL;
		P2SchemeFreeScheme(m_hFilterScheme);
		m_hFilterScheme = NULL;

		P2DBCleanup();
	}
	
	if (P2FAILEDV(OpenConnection(dbConnStr), res))
	{
		P2SchemeFreeScheme(m_hDbScheme);
		m_hDbScheme = NULL;
		P2SchemeFreeScheme(m_hReplScheme);
		m_hReplScheme = NULL;
		P2SchemeFreeScheme(m_hFilterScheme);
		m_hFilterScheme = NULL;

		P2DBCleanup();
	}
	
	return res;
}


P2ERR CReplServerPolicy::OpenReplication(const P2MQ_CONN_HANDLE mqConnection)
{
	RETURN_NULL_ARG(mqConnection);

	m_hMQConnection = mqConnection;
	
	RETURN_P2ERR(OpenDatabase());

	P2ERR res = P2ERR_OK;

	if (P2FAILEDV(P2ReplServerStartup(), res))
	{
		CloseDatabase();
		P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "P2ReplServerStartup() failed"));
		return res;
	}

    if (P2FAILEDV(P2ReplServerCreateHandle(&m_hReplServer), res))
    {
        P2ReplServerCleanup();
        CloseDatabase();
        P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "P2ReplServerCreateHandle() failed"));
        return res;
    }
    P2TRACE_INFO_EX_0((m_name, "ReplServer handle created, repl-server 0x%p", m_hReplServer));

	if (P2FAILEDV(OnBeforeReplicationOpen(m_hReplServer), res))
	{
        P2ReplServerFreeHandle(m_hReplServer);
        m_hReplServer = NULL;
		P2ReplServerCleanup();
		CloseDatabase();
		P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "OnBeforeReplicationOpen() failed"));
		return res;
	}

	if (P2IniGetInt(REPL_SECTION, "noreplserver", 0) != 1)
	{
		char replStreamName[1024];
		memset(replStreamName, 0, sizeof(replStreamName));
		P2IniGetStr(replStreamName, sizeof(replStreamName), REPL_SECTION, "datastream", replStreamName);
		
		if (P2FAILEDV(BindReplServer(replStreamName), res))
		{
			P2ReplServerFreeHandle(m_hReplServer);
			m_hReplServer = NULL;
			P2ReplServerCleanup();
			CloseDatabase();
			P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "P2ReplServerBind() failed"));
			return res;
		}
		P2TRACE_INFO_EX((m_name, "Replication started"));
	}

	/*if (P2FAILEDV(SetACL(), res))
	{
		P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot set ACL;stream %s,server 0x%p", replStreamName, m_hReplServer));
	}

	if (P2FAILEDV(SetFilter(), res))
	{
        P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot set filters;stream %s,server 0x%p", replStreamName, m_hReplServer));
	}*/
	
	if (P2FAILEDV(OnAfterReplicationOpen(m_hReplServer), res))
	{
		CloseDatabase();
		P2ReplServerUnbind(m_hReplServer);
		P2ReplServerFreeHandle(m_hReplServer);
		m_hReplServer = NULL;
		P2ReplServerCleanup();

		P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "OnReplicationOpen() failed, repl-server 0x%p", m_hReplServer));
	}

	return res;
	
}

// TIMERF - нужен для программы, которая замеряет времена работы различных функций
// Например, P2ReplServerProcessMessage
#ifdef TIMERF
BOOL CReplServerPolicy::ProcessMessage(P2MSG_HANDLE msg)
{
	RINS(mapTimers.startTimer("P2ReplServerProcessMessage"));
	BOOL ret = P2ReplServerProcessMessage(msg);
	if(ret)
	{
		RINS(mapTimers.stopTimer("P2ReplServerProcessMessage"));
	}
	else
	{
		RINS(mapTimers.cancelTimer("P2ReplServerProcessMessage"));
	}
	return ret;
}
#else // TIMERF
BOOL CReplServerPolicy::ProcessMessage(P2MSG_HANDLE msg)
{
	return P2ReplServerProcessMessage(msg);
}
#endif // TIMERF
void CReplServerPolicy::CloseDatabase()
{
	if (m_hDbScheme != NULL)
	{
		P2SchemeFreeScheme(m_hDbScheme);
		m_hDbScheme = NULL;
	}

	if (m_hReplScheme != NULL)
	{
		P2SchemeFreeScheme(m_hReplScheme);
		m_hReplScheme = NULL;
	}

	if (m_hFilterScheme != NULL)
	{
		P2SchemeFreeScheme(m_hFilterScheme);
		m_hFilterScheme = NULL;
	}

	CloseConnection();
	P2DBCleanup();
}

void CReplServerPolicy::CloseReplication()
{
	OnReplicationClose();

	P2ReplServerUnbind(m_hReplServer);
	
	if (m_hReplServer != NULL)
	{
		P2ReplServerFreeHandle(m_hReplServer);
		m_hReplServer = NULL;
	}

	P2ReplServerCleanup();
	
	CloseDatabase();

	P2TRACE_INFO_EX((m_name, "Replication server was cleaned up"));
}

//////////////////////////////////////////////////////////////////////
// CReplServerDB
//////////////////////////////////////////////////////////////////////

P2ERR CReplServerDB::OpenConnection(PCSTR connString)
{
	P2ERR res = P2ERR_OK;
	AutoConnectionHandle hDbConnA;
	if (P2FAILEDV(P2DBOpenConnection(m_hDbScheme, connString, &hDbConnA), res))
	{
		P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot open DB connection; connection string %s, scheme 0x%p", connString, m_hDbScheme));
		return res;
	}
	P2TRACE_INFO_EX((m_name, "DB connection was opened; connection string \'%s\', scheme 0x%p, db-conn 0x%p", connString, m_hDbScheme, hDbConnA()));

	if (P2FAILEDV(OnDatabaseOpen(m_hDbScheme, hDbConnA()), res))
	{
		P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "OnOpenDatabase() failed; db-conn 0x%p, scheme 0x%p", hDbConnA(), m_hDbScheme));
		return res;
	}

	m_hDbConn = hDbConnA.Detach();

	return P2ERR_OK;;
}

P2ERR CReplServerDB::BindReplServer(PCSTR replStreamName)
{
	P2ERR res = P2ERR_OK;

	if (P2FAILEDV(P2ReplServerBind2(m_hReplServer, replStreamName, m_hMQConnection, m_hDbConn, m_hReplScheme, m_hFilterScheme), res))
	{
		P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "P2ReplServerBind() failed; streamname \'%s\', repl-server 0x%p, db-conn 0x%p", replStreamName, m_hReplServer, m_hDbConn));
	}
	else
	{
		P2TRACE_INFO_EX((m_name, "Repl-server was bound; streamname \'%s\', repl-server 0x%p, db-conn 0x%p", replStreamName, m_hReplServer, m_hDbConn));
	}

	return res;
}

void CReplServerDB::CloseConnection()
{
	OnDatabaseClose();
	if (m_hDbConn != NULL) 
	{
		P2DBCloseConnection(m_hDbConn);
		m_hDbConn = NULL;
	}
}

/*P2ERR CReplServerDB::SetACL()
{
	P2ERR res = P2ERR_OK;
	
	char aclDbConnect[1024];
	aclDbConnect[0]=0;
	P2IniGetStr(aclDbConnect, sizeof(aclDbConnect), REPL_SECTION, "acl_db_connect", "");

	char aclTblName[P2DB_MAX_TABLE_NAME_SIZE];
	aclTblName[0]=0;
	P2IniGetStr(aclTblName, sizeof(aclTblName), REPL_SECTION, "acl_table", ACL_TABLE);

	AutoSchemeHandle hDbSchemeA;
	AutoConnectionHandle hDbConnA;
	if (strcmp(aclDbConnect, "") != 0)
	{
		if ((P2FAILEDV(P2ReplServerGetACLTableScheme(&hDbSchemeA), res)) ||
			(P2FAILEDV(P2DBOpenConnection(hDbSchemeA, aclDbConnect, &hDbConnA), res))
			)
		{
			P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot open ACL database"));
			return res;
		}
	}
	
	P2DB_CONN_HANDLE hDbConn = hDbConnA() ? hDbConnA() : m_hDbConn;

	AutoTableHandle hDbTblA;
	if (P2FAILEDV(P2DBOpenTableByName(hDbConn, aclTblName, &hDbTblA), res))
	{
		P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot open ACL table"));
	}
	else
	{
        if (P2FAILEDV(P2ReplServerSetACLFromTable(m_hReplServer, hDbConn, hDbTblA), res))
		{
            P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot set ACL"));
		}
	}
	return res;
}

P2ERR CReplServerDB::SetFilter()
{
	P2ERR res = P2ERR_OK;
	
	char filterDbConnect[1024];
	filterDbConnect[0]=0;
	P2IniGetStr(filterDbConnect, sizeof(filterDbConnect), REPL_SECTION, "filter_db_connect", "");

	char filterTblName[P2DB_MAX_TABLE_NAME_SIZE];
	filterTblName[0]=0;
	P2IniGetStr(filterTblName, sizeof(filterTblName), REPL_SECTION, "filter_table", FILTER_TABLE);

	AutoSchemeHandle hDbSchemeA;
	AutoConnectionHandle hDbConnA;
	if (strcmp(filterDbConnect, "") != 0)
	{
		if ( (P2FAILEDV(P2ReplServerGetFilterTableScheme(&hDbSchemeA), res)) ||
			 (P2FAILEDV(P2DBOpenConnection(hDbSchemeA, filterDbConnect, &hDbConnA), res))
			)
		{
			P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot open Filter database"));
			return res;
		}
	}
	
	P2DB_CONN_HANDLE hDbConn = hDbConnA() ? hDbConnA() : m_hDbConn;

	AutoTableHandle hDbTblA;
	if (P2FAILEDV(P2DBOpenTableByName(hDbConn, filterTblName, &hDbTblA), res))
	{
		P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot open Filter table"));
	}
	else
	{
		if (P2FAILEDV(P2ReplServerSetFilterFromTable(m_hReplServer, hDbConn, hDbTblA), res))
		{
            P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot set filters"));
		}
	}
	return res;
}*/
		

//////////////////////////////////////////////////////////////////////
// CReplServerISAM
//////////////////////////////////////////////////////////////////////

P2ERR CReplServerISAM::OpenConnection(PCSTR connString)
{
	P2ERR res = P2ERR_OK;

	if (P2IniGetInt(DB_SECTION, "createdb", 1) == 1)
	{
		if (P2FAILEDV(P2TBLCreateDatabase(connString, m_hDbScheme, P2TBL_SKIP_IF_EXIST), res))
		{
			P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Can't create database,conn-str \'%s\',scheme 0x%p", connString, m_hDbScheme));
			return res;
		}
		P2TRACE_INFO_EX((m_name, "TBL database was created"));
	}

	P2AutoTblConnectionHandle hTblConnA;
	PCSTR pSep = strchr(connString, ';');
	char dll[256];
	size_t len = pSep - connString;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996) // warning C4996: This function or variable may be unsafe.
#endif // _MSC_VER
	strncpy(dll, connString, len);
#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

	dll[len] = 0; 
	if (P2FAILEDV(P2TBLCreateConnection(dll, &hTblConnA), res))
	{
		P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Can't create connection,dll %s,scheme 0x%p", dll, m_hDbScheme));
		return res;
	}

	UINT32 opt = 0;
	if ((P2IniGetInt(DB_SECTION, "flushffdb", 0) == 1))
	{
		opt |= P2TBL_ISAMFFDB_TURN_ON_FLUSH;
	}
	if ((P2IniGetInt(DB_SECTION, "asyncffdb", 0) == 1))
	{
		opt |= P2TBL_ISAMFFDB_ASYNC_IO;
	}
	if (P2FAILEDV(P2TBLSetOptions(hTblConnA(), opt), res))
	{
		P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot set tbl-conn options;opt %d", opt));
		return res;
	}
	if (P2FAILEDV(P2TBLConnect(hTblConnA, m_hDbScheme, ++pSep), res))
	{
		P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Can't connectn to database,conn-str %s,scheme 0x%p", pSep, m_hDbScheme));
		return res;
	}
	P2TRACE_INFO_EX((m_name, "TBL connection was opened; connection string \'%s\', tbl-conn 0x%p", connString, hTblConnA()));
        		
	if (P2FAILEDV(P2TBLLoadDatabase(hTblConnA()), res))
	{
		P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot load TBL database; tbl-conn 0x%p", hTblConnA()));
		return res;
	}
	P2TRACE_INFO_EX_0((m_name, "TBL database successfully loaded; tbl-conn 0x%p", hTblConnA()));

	if (P2FAILEDV(OnDatabaseOpen(m_hDbScheme, hTblConnA()), res))
	{
		P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "OnOpenDatabase() failed; tbl-conn 0x%p, scheme 0x%p", hTblConnA(), m_hDbScheme));
		return res;
	}
	
	m_hTblConn = hTblConnA.Detach();

	return P2ERR_OK;
}

P2ERR CReplServerISAM::BindReplServer(PCSTR replStreamName)
{
	char dbConnStr[1024];
	P2IniGetStr(dbConnStr, sizeof(dbConnStr), DB_SECTION, "connect", "P2_ERROR_STR");

	PCSTR pSep = strchr(dbConnStr, ';');
	if (pSep == NULL)
	{
		P2TRACE_ERROR_WITH_CODE_EX((P2ERR_COMMON_WRONG_ARGUMENTS, m_name, "Repl-server bind failed;wrong conn-str \'%s\';streamname \'%s\'", dbConnStr, replStreamName));
		return P2ERR_COMMON_WRONG_ARGUMENTS;
	}

	string dllName(dbConnStr, pSep - dbConnStr);

	P2DLL_HANDLE hDll = P2DLLoadLibrary(dllName.c_str(), DEFAULT_LOAD_FLAG);

	if (hDll == NULL)
	{
		P2TRACE_ERROR_WITH_CODE_EX((P2ERR_COMMON_WRONG_ARGUMENTS, m_name, "Repl-server bind failed;wrong dll \'%s\';streamname \'%s\'", dllName.c_str(), replStreamName));
		return P2ERR_COMMON_WRONG_ARGUMENTS;
	}
	P2AutoFreeLibrary hDllA(hDll);

	P2_BIND_TO_ISAM_FUNC pfBindToISAM = reinterpret_cast<P2_BIND_TO_ISAM_FUNC>(P2DLLGetProcAddress(hDll, "P2ReplServerBind2ISAM")); 
	if (pfBindToISAM == NULL)
	{
		P2TRACE_ERROR_WITH_CODE_EX((P2ERR_COMMON_WRONG_ARGUMENTS, m_name, "Repl-server bind failed,no P2ReplServerBind2ISAM func;streamname \'%s\'", replStreamName));
		return P2ERR_COMMON_WRONG_ARGUMENTS;
	}
	
	P2ERR res = P2ERR_OK;
	if (P2FAILEDV(pfBindToISAM(m_hReplServer, replStreamName, CReplServerPolicy::m_hMQConnection, m_hTblConn, m_hReplScheme, m_hFilterScheme), res))
	{
		P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "P2ReplServerBind() failed; streamname \'%s\', repl-server 0x%p, tbl-conn 0x%p", replStreamName, m_hReplServer, m_hTblConn));
	}
	else
	{
		P2TRACE_INFO_EX((m_name, "Repl-server was bound; streamname \'%s\', repl-server 0x%p, tbl-conn 0x%p", replStreamName, m_hReplServer, m_hTblConn));
	}

	return res;
}

/*{
	P2ERR res = P2ERR_OK;
	if (P2FAILEDV(P2ReplServerBind2ISAM(m_hReplServer, replStreamName, m_hMQConnection, m_hTblConn, m_hReplScheme, m_hFilterScheme), res))
	{
		P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "P2ReplServerBind() failed; streamname \'%s\', repl-server 0x%p, tbl-conn 0x%p", replStreamName, m_hReplServer, m_hTblConn));
	}
	else
	{
		P2TRACE_INFO_EX((m_name, "Repl-server was bound; streamname \'%s\', repl-server 0x%p, tbl-conn 0x%p", replStreamName, m_hReplServer, m_hTblConn));
	}

	return res;
}*/

void CReplServerISAM::CloseConnection()
{
	OnDatabaseClose();
	if (m_hTblConn != NULL) 
	{
		P2TBLCloseConnection(m_hTblConn);
		m_hTblConn = NULL;
	}
}




/*P2ERR CReplServerISAM::SetACLFromDb(PCSTR dbConnect, PCSTR tblName)
{
	P2ERR res = P2ERR_OK;

	AutoSchemeHandle hDbSchemeA;
	AutoConnectionHandle hDbConnA;
	
	if ((P2FAILEDV(P2ReplServerGetACLTableScheme(&hDbSchemeA), res)) ||
		(P2FAILEDV(P2DBOpenConnection(hDbSchemeA(), dbConnect, &hDbConnA), res))
		)
	{
		P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot open ACL database"));
		return res;
	}

	AutoTableHandle hDbTableA;
	if (P2FAILEDV(P2DBOpenTableByName(hDbConnA(), tblName, &hDbTableA), res))
	{
		P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot open ACL table"));
	}
	else
	{
        if (P2FAILEDV(P2ReplServerSetACLFromTable(m_hReplServer, hDbConnA(), hDbTableA()), res))
		{
            P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot set ACL"));
		}
	}
	return res;
}

P2ERR GetTableIndexByName(P2DB_SCHEME_HANDLE hDbScheme, PCSTR tblName, UINT32* pIndex)
{
	UINT32 count = 0;
	RETURN_P2ERR(P2SchemeGetTableCount(hDbScheme, &count));

	for(UINT32 i =0; i < count; ++i)
	{
		AutoTableSchemeHandle hTblSchemeA;
        RETURN_P2ERR(P2SchemeGetTableByIndex(hDbScheme, i, &hTblSchemeA));

		char name[P2DB_MAX_TABLE_NAME_SIZE+1];
		UINT32 size = sizeof(name);
		RETURN_P2ERR(P2SchemeGetTableName(hTblSchemeA(), name, &size));

		if (stricmp(name, tblName) == 0)
		{
            *pIndex = i;
			return P2ERR_OK;
		}
	}

	return 	P2ERR_DB_NO_SUCH_ITEM;
}

P2ERR CReplServerISAM::SetACL()
{
	P2ERR res = P2ERR_OK;
	
	char aclDbConnect[1024];
	aclDbConnect[0]=0;
	P2IniGetStr(aclDbConnect, sizeof(aclDbConnect), REPL_SECTION, "acl_db_connect", "");

	char aclTblName[P2DB_MAX_TABLE_NAME_SIZE];
	aclTblName[0]=0;
	P2IniGetStr(aclTblName, sizeof(aclTblName), REPL_SECTION, "acl_table", ACL_TABLE);

	if (strcmp(aclDbConnect, "") != 0)
	{
		return SetACLFromDb(aclDbConnect, aclTblName);
	}
	else
	{
		UINT32 index = 0;
		if (P2FAILEDV(GetTableIndexByName(m_hDbScheme, aclTblName, &index), res))
		{
			P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot get ACL table index"));
		}
		else
		{
			P2AutoTblTableHandle hTblTableA;
			if (P2FAILEDV(P2TBLOpenTableByIndex(m_hTblConn, index, &hTblTableA), res))
			{
				P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot open ACL table"));
			}
			else
			{
				if (P2FAILEDV(P2ReplServerSetACLFromTableISAM(m_hReplServer, hTblTableA()), res))
				{
					P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot set ACL"));
				}
			}
		}
	}
	return res;
}

P2ERR CReplServerISAM::SetFilterFromDb(PCSTR dbConnect, PCSTR tblName)
{
	P2ERR res = P2ERR_OK;

	AutoSchemeHandle hDbSchemeA;
	AutoConnectionHandle hDbConnA;
	
	if ((P2FAILEDV(P2ReplServerGetFilterTableScheme(&hDbSchemeA), res)) ||
		(P2FAILEDV(P2DBOpenConnection(hDbSchemeA(), dbConnect, &hDbConnA), res))
		)
	{
		P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot open Filter database"));
		return res;
	}

	AutoTableHandle hDbTableA;
	if (P2FAILEDV(P2DBOpenTableByName(hDbConnA(), tblName, &hDbTableA), res))
	{
		P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot open Filter table"));
	}
	else
	{
        if (P2FAILEDV(P2ReplServerSetFilterFromTable(m_hReplServer, hDbConnA(), hDbTableA()), res))
		{
            P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot set filters"));
		}
	}
	return res;
}

P2ERR CReplServerISAM::SetFilter()
{
	P2ERR res = P2ERR_OK;
	
	char filterDbConnect[1024];
	filterDbConnect[0]=0;
	P2IniGetStr(filterDbConnect, sizeof(filterDbConnect), REPL_SECTION, "filter_db_connect", "");

	char filterTblName[P2DB_MAX_TABLE_NAME_SIZE];
	filterTblName[0]=0;
	P2IniGetStr(filterTblName, sizeof(filterTblName), REPL_SECTION, "filter_table", FILTER_TABLE);

	if (strcmp(filterDbConnect, "") != 0)
	{
		return SetFilterFromDb(filterDbConnect, filterTblName);
	}
	else
	{
		UINT32 index = 0;
		if (P2FAILEDV(GetTableIndexByName(m_hDbScheme, filterTblName, &index), res))
		{
			P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot get Filter table index"));
		}
		else
		{
			P2AutoTblTableHandle hTblTableA;
			if (P2FAILEDV(P2TBLOpenTableByIndex(m_hTblConn, index, &hTblTableA), res))
			{
				P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot open Filter table"));
			}
			else
			{
				if (P2FAILEDV(P2ReplServerSetFilterFromTableISAM(m_hReplServer, hTblTableA()), res))
				{
					P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot set filters"));
				}
			}
		}
		return res;
	}
}*/


	
//////////////////////////////////////////////////////////////////////
// CReplClient
//////////////////////////////////////////////////////////////////////

P2ERR CReplClient::StartupDatabase()
{
	P2ERR res = P2ERR_OK;
	if (P2FAILEDV(P2DBStartup(), res))
	{
		P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot startup DB"));
		return res;
	}

	char dbSchemeName[1024];
	memset(dbSchemeName, 0, sizeof(dbSchemeName));
	P2IniGetStr(dbSchemeName, sizeof(dbSchemeName), DB_SECTION, "dbscheme", dbSchemeName);
	
	if (P2FAILEDV(P2SchemeCreateSchemeFromIni(&m_hDbScheme, dbSchemeName), res))
	{
		P2DBCleanup();
		P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot create scheme \'%s\' from ini file", dbSchemeName));
		return res;
	}
	P2TRACE_INFO_EX_0((m_name, "Db-scheme \'%s\' from ini file was created, scheme 0x%p", dbSchemeName, m_hDbScheme));

	memset(m_DbConnStr, 0, sizeof(m_DbConnStr));
	P2IniGetStr(m_DbConnStr, sizeof(m_DbConnStr), DB_SECTION, "connect", m_DbConnStr);

	return P2ERR_OK;
}

void CReplClient::CleanupDatabase()
{
	if (m_hDbScheme != NULL)
	{
		P2SchemeFreeScheme(m_hDbScheme);
		m_hDbScheme = NULL;
	}

	P2DBCleanup();
}

#define P2BASEAPP_RETURN_P2ERR_PRINT(err, msg) if (err != P2ERR_OK) { P2TRACE_ERROR_WITH_CODE_EX((err, m_name, msg)); return err;}

P2ERR CReplClient::OpenReplication(const P2MQ_CONN_HANDLE hMQConnection)
{
	RETURN_NULL_ARG(hMQConnection);
	m_hMQConnection = hMQConnection;

	if (m_replState < rsReplStartup)
	{
		P2BASEAPP_RETURN_P2ERR_PRINT(P2ReplStartup(), "P2ReplStartup() failed")
		m_replState = rsReplStartup;
	}

	if (m_replState < rsCreateDS)
	{
		P2BASEAPP_RETURN_P2ERR_PRINT(P2ReplCreateDatastream(&m_hReplStream), "P2ReplCreateDatastream() failed")
		P2TRACE_INFO_EX_0((m_name,"Datastream handle was created, stream 0x%p", m_hReplStream));
		m_replState = rsCreateDS;
	}
	
	char replStreamName[1024];
	P2IniGetStr(replStreamName, sizeof(replStreamName), REPL_SECTION, "datastream", replStreamName);

	if (m_replState < rsDBStartup)
	{
		P2BASEAPP_RETURN_P2ERR_PRINT(StartupDatabase(), "OpenDatabse() failed")
		m_replState = rsDBStartup;
	}

	if (m_replState < rsOnReplOpen)
	{
		UINT32 retry_count = P2IniGetInt(REPL_SECTION, "open_datastream_retry_count", 1);
		retry_count = retry_count ? retry_count : 1;
		UINT32 datastream_timeout = P2IniGetInt(REPL_SECTION, "open_datastream_timeout", 0);
		UINT32 sleep_interval = datastream_timeout / retry_count;
		sleep_interval = sleep_interval ? sleep_interval : 5;

		while(retry_count)
		{
			--retry_count;
			
			P2ERR res = P2ERR_OK;
			if (P2FAILEDV(P2ReplOpenDatastreamDB(m_hReplStream, m_hMQConnection, replStreamName,
												P2REPL_DATASTREAM_TYPE_SNAPSHOT | P2REPL_DATASTREAM_TYPE_ONLINE,
												m_DbConnStr, ReplClientCB, this, m_hDbScheme), res))
			{
				if (retry_count != 0)
				{
					Sleep(sleep_interval);
				}
				else
				{
					P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "P2ReplOpenDatastreamDB() failed; streamname %s, stream 0x%p, mq-conn 0x%p, scheme 0x%p, connection string \'%s\'", replStreamName, m_hReplStream, m_hMQConnection, m_hDbScheme, m_DbConnStr));
					return P2ERR_OK; // нефатально: работать можно и без репликационной БД
				}
			}
			else
			{
				P2TRACE_INFO_EX_0((m_name, "Datastream was opened; streamname %s, stream 0x%p, mq-conn 0x%p, scheme 0x%p, connection string \'%s\'", replStreamName, m_hReplStream, m_hMQConnection, m_hDbScheme, m_DbConnStr));
				m_replState = rsOpenDS;
				
				if (P2FAILEDV(OnReplicationOpen(m_hReplStream), res))
				{
					P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "OnReplicationOpen() failed; streamname '%s'", replStreamName));
					return res;
				}
				m_replState = rsOnReplOpen;

				break;
			}
		}
	}
		
	return P2ERR_OK;
}
	
BOOL CReplClient::ProcessMessage(P2MSG_HANDLE msg)
{
	if (CheckReplState())
	{
        return P2ReplProcessMessage(m_hMQConnection, msg);
	}

	return P2ERR_OK;
}

BOOL CReplClient::CheckReplState()
{
	if (m_replState != rsOnReplOpen)
	{
		P2ERR res = P2ERR_OK;
		if (P2FAILEDV(OpenReplication(m_hMQConnection), res))
		{
			P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Cannot open replication"));
			CloseReplication();
			return FALSE;
		}
	}

	return TRUE;
}

void CReplClient::CloseReplication()
{
	bool doCallback = false;
	if (m_replState == rsOnReplOpen)
	{
		OnBeforeReplicationClose();
		m_replState = rsOpenDS;
		doCallback = true;
	}
	
	if (m_replState == rsOpenDS)
	{
		P2ReplCloseDatastream(m_hReplStream);
		m_replState = rsDBStartup;
	}
	
	if (m_replState == rsDBStartup)
	{
		CleanupDatabase();
		m_replState = rsCreateDS;
	}
	
	if (m_replState == rsCreateDS)
	{
		if (m_hReplStream != NULL)
		{
			P2ReplFreeDatastream(m_hReplStream);
			m_hReplStream = NULL;
		}
		m_replState = rsReplStartup;
	}
	
	if (m_replState == rsReplStartup)
	{
		P2ReplCleanup();
		m_replState = rsNone;
	}
	
	if (doCallback)
	{
        OnAfterReplicationClose();
		P2TRACE_INFO_EX((m_name, "Replication client was cleaned up"));
	}
}


void CReplClient::ReplClientCB(P2DB_RECORD_HANDLE rec, size_t tblIdx, P2MSG_HANDLE msg, void* cbParams)
{
	_ASSERT(cbParams != NULL);

	CReplClient* pReplClient = (CReplClient*)cbParams;
	if (msg)
	{
		pReplClient->HandleReplState(msg);
	}
	else if (!rec)
	{
		P2LogFatalEx(pReplClient->m_name, "Error in %s (%d); ReplClientCB()", __FILE__, __LINE__);
		_ASSERT(false);
		return;
	}

	pReplClient->OnGetReplicationMessage(rec, tblIdx, msg);
}

void CReplClient::HandleReplState(P2MSG_HANDLE msg)
{
	UINT32 notif;
	P2ERR res = P2ERR_OK;
	if (P2FAILEDV(P2MsgCopyFieldUINT32(msg, P2MSG_PROP_TYPE, &notif), res))
	{
		P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Couldn't get notification code from notification message"));
		return;
	}

	switch (notif)
	{
	case P2REPL_CLIENT_NOTIF_SCHEME_IS_SET:
	case P2REPL_CLIENT_NOTIF_OPENED:
	case P2REPL_CLIENT_NOTIF_ONLINE:
	case P2REPL_CLIENT_NOTIF_DATA_BEGIN:        
	case P2REPL_CLIENT_NOTIF_DATA_END:
	case P2REPL_CLIENT_NOTIF_CLEAR_DELETED:
	case P2REPL_CLIENT_NOTIF_LIFE_NUM_CHANGED:
	case P2REPL_CLIENT_NOTIF_TABLE_EOF:
	case P2REPL_CLIENT_NOTIF_PREAPPLY_REC:
	case P2REPL_CLIENT_NOTIF_DB_WILL_BE_DELETED:
		break;

	// Process errors
	default:
		{
			switch (notif)
			{
			case P2REPL_CLIENT_NOTIF_CLOSE_ON_COMPLETE:
				P2TRACE_ERROR_EX((m_name, "Unexpected notification received from repl-cli-library; notification %u", notif));
				// no break;
			case P2REPL_CLIENT_NOTIF_ERROR:
			case P2REPL_CLIENT_NOTIF_OPEN_FAILED:
			case P2REPL_CLIENT_NOTIF_SCHEME_SET_FAILED:
			case P2REPL_CLIENT_NOTIF_CLOSED_BY_SERVER:
			case P2REPL_CLIENT_NOTIF_TIMEOUT:
			case P2REPL_CLIENT_NOTIF_REQUEST_FAILED:
				{
					CloseReplication();
					//ReopenReplication();
				}
				break;

			// Not error or unknown notification
			default:
				P2TRACE_ERROR_EX((m_name, "Unknown P2REPL_CLIENT notification received; notification %u", notif));
				return;
			}

			// Get reason
			size_t reasonSize = sizeof(res);
			if (P2FAILED(P2MsgCopyBody(msg, &res, &reasonSize)))
				res = P2ERR_OK;

			P2TRACE_ERROR_WITH_CODE_EX((res, m_name, "Upstream closed; notification %u", notif));
		}
	}
}

void CReplClient::OnAppIdle()
{
	if (CheckReplState())
	{
        P2ReplProcessMessage(m_hMQConnection, 0);
	}
    
	OnIdle();
}



#endif // !defined(AFX_P2REPLAPP_IMPL_H__A3E65D82_D1A2_4FB1_A654_52452F401D82__INCLUDED_)
