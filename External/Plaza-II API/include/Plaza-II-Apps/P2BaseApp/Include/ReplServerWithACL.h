
#ifndef _REPLSERVER_WITH_ACL_POLICY
#define _REPLSERVER_WITH_ACL_POLICY


#include "../../../Plaza-II-Repl/P2ACLReplClient/Include/P2ACLReplClient.hpp"
///////////////////////////
#include "P2ReplApp.h"

/* 
   серверная полиси для p2replapp, которая начинает репликацию (bind server)  только после 
           получения записи об ACL от сервера tsrights, 	
*/

class CPolicy_ACL: public CReplServerISAM  //todo: наследование надо сделать шаблоном
{
public:
	CPolicy_ACL();
	~CPolicy_ACL();
	// p2replapp calls it as ReplPolicy::OpenReplication after connection
	P2ERR OpenReplication(const P2MQ_CONN_HANDLE mqConnection);
	// p2replapp calls it as ReplPolicy::CloseReplication before disconnect
	void  CloseReplication();             
	//////////////////
	void OnAppIdle(); 
	BOOL ProcessMessage(P2MSG_HANDLE msg);

	bool IsACLStreamUsed() { return m_bUseACLStream; }
	
private:
	typedef CReplServerISAM ServerReplication;   
	P2AutoACLReplHandle			m_aclRepl;
    P2AutoACLHandle             m_hStoredAcl;
	bool						m_aclReplRestartRequired;
	char						m_replServerName[256];   
	char						m_aclStreamName[256];
	char						m_aclDbConnStr[256];
	char						m_aclTableName[256];
	bool						m_bUseACLStream;		// false -- отключена репликация аклов, просто сервер репликации
	UINT32						m_aclReconnectTimeout;
	DWORD						m_aclReconnectTimestamp;

    static P2ERR	aclReplCB(UINT32,const char*,const P2ACL_HANDLE,void*);
    void			ProcessAclNotification(UINT32 notif,const char* stream,const P2ACL_HANDLE ACL);
};

#endif // _REPLSERVER_WITH_ACL_POLICY
