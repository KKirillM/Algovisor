
#ifndef _REPLSERVERWITHACL_POLICY_IMPLEMENTATION_H_
#define _REPLSERVERWITHACL_POLICY_IMPLEMENTATION_H_

#include "ReplServerWithACL.h"
#include "P2App_impl.h"
#include "P2ReplApp_impl.h"

static const char* ACL_LOG_ = "p2acl_repl_policy";

//////////////////////   ReplServerPolicy with ACL Implementation ////////////////

void CPolicy_ACL::OnAppIdle()
{
	if(m_bUseACLStream)
	{
		P2ERR res;
		P2ReplProcessMessage(ServerReplication::m_hMQConnection,NULL);
		if( m_aclReplRestartRequired && GetTickCount() - m_aclReconnectTimestamp > m_aclReconnectTimeout)
		{
			if(P2FAILEDV(P2ReplACLOpen( m_aclRepl, ServerReplication::m_hMQConnection, m_replServerName, m_aclDbConnStr, aclReplCB, this),res))
			{
				P2TRACE_ERROR_WITH_CODE_EX((res,ACL_LOG_,"Start acl replication failed"));
			}
			else
			{
				m_aclReplRestartRequired = false;
			}
			m_aclReconnectTimestamp = GetTickCount();
		}
	}

	//P2ReplServerStrongLockInt(); 
	P2ReplServerProcessMessage(NULL);
	//P2ReplServerUnlockInt();
}

void CPolicy_ACL::CloseReplication()
{
	if(m_bUseACLStream)
	{
        m_aclRepl.Close();
        m_aclReplRestartRequired = false;
        P2ReplCleanup();
	}
	ServerReplication::CloseReplication();
}

P2ERR CPolicy_ACL::OpenReplication(const P2MQ_CONN_HANDLE mqConnection)
{
	m_bUseACLStream = P2IniGetInt(REPL_SECTION, "acl_use_stream", 0) != 0;
	if(!m_bUseACLStream)
	{
		return ServerReplication::OpenReplication(mqConnection);
	}

	P2ERR res = P2ERR_OK;

	P2AutoACLReplHandle				aclrepl;
	P2IniGetStr(m_aclStreamName,  sizeof m_aclStreamName,   REPL_SECTION, "acl_stream","ReplBLTSRights"); 
	P2IniGetStr(m_aclTableName,   sizeof m_aclTableName,    REPL_SECTION, "acl_table","WorkstationRights");
	P2IniGetStr(m_replServerName, sizeof m_replServerName,  REPL_SECTION, "datastream","");
	P2IniGetStr(m_aclDbConnStr,   sizeof m_aclDbConnStr,    REPL_SECTION, "acl_db_conn","");
	m_aclReconnectTimeout = P2IniGetInt(REPL_SECTION, "acl_reconnect_timeout", 3000);
	
	RETURN_P2ERR(P2ReplACLCreate(&aclrepl));
	P2REPLACLSetParameters(aclrepl, m_aclStreamName, m_aclTableName);  

	if(P2FAILEDV(ServerReplication::OpenReplication(mqConnection), res))
	{
		P2TRACE_ERROR_WITH_CODE_EX((res,ACL_LOG_,"Couldn't open replication"));
		return res;
	}

    if(P2FAILEDV(P2ReplServerSetACL(ServerReplication::m_hReplServer, m_hStoredAcl), res))
    {
        P2TRACE_ERROR_WITH_CODE_EX((res, ACL_LOG_, "Couldn't set acl to server %s", m_replServerName));
    }
    P2ACLDumpACL(m_hStoredAcl, P2ACLDUMP_ACL_ORDER_BY_USERS);

	RETURN_P2ERR(P2ReplStartup());
	m_aclRepl.Swap(aclrepl);
	m_aclReplRestartRequired = true;
	m_aclReconnectTimestamp = 0;
	
	return P2ERR_OK;
}

BOOL CPolicy_ACL::ProcessMessage(P2MSG_HANDLE msg)
{
	BOOL res;

	//P2ReplServerStrongLockInt(); 
	res = P2ReplServerProcessMessage(msg); 
	//P2ReplServerUnlockInt();

	if(m_bUseACLStream && res == FALSE)
	{
		res = P2ReplProcessMessage(ServerReplication::m_hMQConnection, msg);
	}

	return res; 
}

P2ERR CPolicy_ACL::aclReplCB(UINT32 notif, const char* stream, const P2ACL_HANDLE hACL, void* Param)
{
	RETURN_NULL_ARG(Param);
	CPolicy_ACL	*pAclPolicy	= reinterpret_cast<CPolicy_ACL*>(Param);
	pAclPolicy->ProcessAclNotification(notif, stream, hACL);
	return P2ERR_OK;
}

void CPolicy_ACL::ProcessAclNotification(UINT32 notif, const char* stream, const P2ACL_HANDLE hACL)
{
	P2ERR res = P2ERR_OK;
	switch(notif)
	{
		case ACL_NOTIF_STREAM_RESTART_REQUIRED:
		case ACL_NOTIF_ERROR:					//todo: maybe stop here ?
		case ACL_NOTIF_TABLE_INVALID:
			{
				if(P2FAILEDV(P2ReplACLClose(m_aclRepl), res))
				{
					P2TRACE_ERROR_WITH_CODE_EX((res, ACL_LOG_, "failed to close acl replication"));
				}
				m_aclReplRestartRequired = true;
				break;
			}
		case ACL_NOTIF_TABLE_VALID:
		case ACL_NOTIF_NEW_DATA:
			{
                P2AutoACLHandle hNewAcl;
                P2ACLMakeNewPtr(hACL, &hNewAcl);
                m_hStoredAcl.Swap(hNewAcl); // прихраниваем acl 
                if(P2FAILEDV(P2ReplServerSetACL(ServerReplication::m_hReplServer, m_hStoredAcl), res))
                {
                    P2TRACE_ERROR_WITH_CODE_EX((res, ACL_LOG_, "Couldn't set acl to server"));
                }
                break;
			}
		default:
			P2TRACE_ERROR_EX((ACL_LOG_, "Unknown acl repl notif %u", notif));
	};
}

// constructor && destructor
CPolicy_ACL::CPolicy_ACL() : m_aclReplRestartRequired(false), m_bUseACLStream(false)
{
    P2ERR res;
    P2AutoACLHandle hEmptyAcl; // пустой acl используется до получения действительного
    if(P2FAILEDV(P2ACLCreate(&hEmptyAcl), res))
    {
        P2TRACE_ERROR_WITH_CODE_EX((res, ACL_LOG_, "Couldn't create empty acl"));
        return;
    }
    m_hStoredAcl.Swap(hEmptyAcl);
}

CPolicy_ACL::~CPolicy_ACL()
{
}

#endif // _REPLSERVERWITHACL_POLICY_IMPLEMENTATION_H_
