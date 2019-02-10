#ifndef _IPREPLDB_H_
#define _IPREPLDB_H_

#include "../../../Plaza-II/P2Utils/Include/IRc.h"

#include "../../../Plaza-II/P2SysScheme/Include/P2SysScheme.h"
#include "../../../Plaza-II-Repl/P2ReplCommon/Include/P2Repl.h"
#include "../../../Plaza-II/P2Utils/Include/AutoHandle.hpp"
#include "../../../Plaza-II-DB/P2DB/Include/P2DB.h"
#include "../../../Plaza-II/P2ACL/Include/P2ACL.h"
#include "../../../Plaza-II-Repl/P2ReplUtils/Include/P2ReplDS.h"
#include "../../../Plaza-II-Repl/P2ReplUtils/Include/P2ReplDSDriver.h"

//////////////////////////////////////////////////////////////////////////
// IP2ReplDS
/*
��������� IP2ReplDS ������������ �������� ���������� ��� ��������������� �� ����������� ��������� ������
��� ��������� ������� ���������� �������� � P2DB, � P2Tbl � ���� � ��������� ���������� ������, ������������� � P2ReplPlayer.
��������� ��������� ������ ������ ��� ������ �� ���������. ��� ��������� ������ � �� �������� ���� IP2ReplDS. ������ ��������
��������� ����������� � ������� ������� P2ReplServerPostOnlineFromRevRangeInt, ������� ������������ ��������� � 
��������, ���� �� ����� ������ P2ReplServerCommitInt ��������� �� ��������� ������������ ������.

����� ������� ��������� ��������� ������� ������ ���������� �� ���������. ������ ������������������ ������ ���������� ����� ����
��������� IP2ReplDS, ���� �� ����� ������� ����� ������ ��������� ��������. � ������� �������, ������������ IP2ReplDS ������ �������� ���� �������� � ����. 
�������� ���������� ��� ����� P2REPL_DS_CONN_HANDLE � ������ ���� ���������� � �������� ����������. ������ � �������� ��������� ������������ ����������� 
� ���� P2REPL_DS_CONN_HANDLE �������� � P2DB ��� P2Tbl ����. ������ ���������� ���������� ����� CheckMyConn ��� �������� ����� �������������
� ������� ���������.

���� ������, ����������� IP2ReplDS, ����� ���� �������� � ���������� �������� ����������. ��� ����������� ������ ����������� �������
����� ������� ������������������� �������, ����������� � ������� �������. ��� �������� � ������ InitServer, ������� �������� ��������������
����� �������. ����� ����� �������� �� �� ����������� ������ (����� �����, �������� � ��������� ���) � ������� ��������� �� ������ � ������� � ���������
ppObj. ���� �������� �������� ��� ��������� � ������� ���������� � ������ ���� �������� � ������ DeinitServer. ��������� �� ������ ���������� �� 
��� ������ ���������� IP2ReplDS � ����� ���� ����������� ��� ��������� ������� ������ �� ���������.

������� ����� ���������� - ��� ����� EnumRecords. ����� �������� ���������� ��� ������� cb ��� ���� �������, �������������� � ������� �� ������ tblSch � ��������� ��������
�� lowRev �� ��������� ������. ���� ������ ������� ���� REPLDS_ENUM_FLAGS_HAS_UPPER, �� �������� ���������� ������� �������� �� �������� �� lowRev �� highRev. 
���� REPLDS_ENUM_FLAGS_SKIP_DELETED ������� ���������� ��������� ������ (TBD: ���� ���� ����� ����������� ������ ��� ������ �������� �������, �� ����������� ��������� ������). 
��� ����������� ������ �������� � ����� ���������� ����� ������� � ��������� from (� ������� ��� NULL) � ������� ��� � ��������� hAcl. ��� � ��� �����, ���� ������
����� ������������ P2ERR_COMMON_CANCEL_ENUM, ������������ �� ��������

������ ������ ���������� ������ ������� ������� �������� �� ������. ������ ��������� �� ���� ����� ����� ������� � ���������� ��������� ������.
����� GetFirstTableRevision ���������� ������ ������, �������������� � �������.
����� GetFirstNonDeletedTableRevision ���������� ������ ������, �������������� � �������, � �������� � ���� act ��������� ����. ����� ���������� �����������, �� ������ ��� �������� ��������� �������.
����� GetFirstTableRevForOnlineServer ���������� ������ ������, �������������� � �������, �� �������� ������������� � ������ �������. ���� ����� �������� ��� ������ ��������.
����� GetNextTableRevision ���������� ������, ������� ����� �������� ��������� ������ � �������.

��� �������� ��������� ������-������� ��� ������� ���� ����� IsOnlineServerSupported.
�������������� ����� GetScheme ���������� ����� ���������.

TBD GetHost

����� �������
DeinitServer ���������� �� P2ReplServer::Deconfig
InitServer ���������� �� P2ReplServer::ConfigDB
GetScheme ���������� �� P2ReplServer::ConfigDB
CheckVersion ���������� �� P2ReplServer::ConfigDB

CheckMyConn ���������� �� �������, ������� ������������ ���������� �������� ���� ������ �������.
	P2ReplServer::InitComposeOnlineX
	P2ReplServer::FinishComposeOnlineX
	P2ReplServer::PostOnlineFromChangedTablesX
	P2ReplServer::PostOnlineClearOldRevX
	P2ReplServer::HaveOnlineChannels

EnumRecords ������� ���������� ������ �� ���� ����: �� ������� �������� � �������.
	P2ReplServer::ComposeTableOnlineX
	ReplChannel::ComposeOneTableSnapshot

GetNextTableRevision ���������� ���, ��� ����� �������� ������� ��������� ������� ��� �������� �� � �����������
	ReplChannel::ComposeOneTableSnapshot ��� �������� ������� ���������� �������, �������� � ����
	ReplChannel::CalcDataSizeFromRevs ������ ������ ����������� � ������������ ������

��������� ������ �������� � �������� ������������ �� ���� �������: CheckClientRevisonsX, ��� �����������, ����� �� 
������� ������� �� ������-������ � GetClearDeletedRevisionsX, ��� �� ���� ������������ ������, ���� ��������
����� �������� �������. � ���� �� �������� ������������ IsOnlineServerSupported ��� ������ ������.

GetFirstTableRevision
	ReplChannel::CheckClientRevisonsX
	ReplChannel::GetClearDeletedRevisionsX
GetFirstNonDeletedTableRevision
	ReplChannel::GetClearDeletedRevisionsX
GetFirstTableRevForOnlineServer
	ReplChannel::CheckClientRevisonsX
	ReplChannel::GetClearDeletedRevisionsX
IsOnlineServerSupported
	ReplChannel::CheckClientRevisonsX
	ReplChannel::GetClearDeletedRevisionsX

*/
//////////////////////////////////////////////////////////////////////////
#define REPLDS_ENUM_FLAGS_HAS_UPPER 1
#define REPLDS_ENUM_FLAGS_SKIP_DELETED 2
//////////////////////////////////////////////////////////////////////////

class IP2ReplDSVersion
{
public:
	enum { 
		MAJOR_VERSION = 3,
		MINOR_VERSION = 0 
	};
};

class IP2ReplDS : public IRc, public IP2ReplDSVersion 
{
public:
	P2ERR CheckVersion(UINT32 reqMajorVersion, UINT32 reqMinorVersion, char reqBuild);

	virtual ~IP2ReplDS() { };
	virtual P2ERR InitServer(P2DB_SCHEME_HANDLE hFilterScheme, void** ppObj) = 0;
	virtual P2ERR DeinitServer(void* pObj) = 0;
	virtual P2ERR GetHost(P2DSHost**) = 0;
	virtual P2ERR GetScheme(P2DB_SCHEME_HANDLE* pSch) = 0;
	virtual bool  CheckMyConn(P2REPL_DS_CONN_HANDLE conn) = 0;

	virtual P2ERR EnumRecords(void* pObj, P2ACL_HANDLE hAcl, PCSTR from, P2DB_TABLE_SCHEME_HANDLE tblSch, P2REPL_REVISION_TYPE lowRev, P2REPL_REVISION_TYPE highRev, UINT32 flags, P2REPL_DS_ENUM_FUNC cb, void* cbParam) = 0;

	virtual P2ERR GetFirstTableRevision(void* pObj, P2DB_TABLE_SCHEME_HANDLE tblSch, P2REPL_REVISION_TYPE *pRev) = 0;
	virtual P2ERR GetFirstNonDeletedTableRevision(void* pObj, P2DB_TABLE_SCHEME_HANDLE tblSch, P2REPL_REVISION_TYPE *pRev) = 0;
	virtual P2ERR IsOnlineServerSupported(void* pObj, P2DB_TABLE_SCHEME_HANDLE tblSch, bool* pSupported) = 0;
	virtual P2ERR GetFirstTableRevForOnlineServer(void* pObj, P2DB_TABLE_SCHEME_HANDLE tblSch, P2REPL_REVISION_TYPE *pRev) = 0;
	virtual P2ERR GetNextTableRevision(void* pObj, P2DB_TABLE_SCHEME_HANDLE tblSch, P2REPL_REVISION_TYPE *pRev) = 0; 
	virtual P2ERR SetNextTableRevision(void* pObj,P2DB_TABLE_SCHEME_HANDLE tblSch,P2REPL_REVISION_TYPE rev) = 0;

	// ������� ������ ���������� ���������� ������������� ���������� � ��
	// ���� P2DB_CONN_HANDLE, ���� this ��� ISAM-�������� �����������
	virtual void* GetUniqueID() = 0;
    virtual UINT32 GetCapabilities() = 0;
};

#define IREPLDS_CAPABILITY__NULL_FIELDS         0x0001 // ������� ������������ nulls
#define IREPLDS_CAPABILITY__DEFVALUES_IN_FSCH   0x0002 // ������� ������������ default ���� � ������-�����


inline P2ERR IP2ReplDS::CheckVersion(UINT32 reqMajorVersion, UINT32 reqMinorVersion, char reqBuild)
{
	#ifdef _DEBUG
		char build = 'd';
	#else
		char build = 'r';
	#endif
	if (reqBuild != build)
	{
		P2LogError("Build mismatch: we want build '%c',was loaded '%c'", reqBuild, build);
		_ASSERTE(("�����, �� ����� ������ �������� � �������� ������?",0));
		return P2ERR_COMMON_WRONG_BUILD;
	}
	if ((reqMajorVersion == MAJOR_VERSION) && (reqMinorVersion <= MINOR_VERSION))
	{
		return P2ERR_OK;
	}
	else
	{
		P2LogError("Version mismatch: we want module v.%02d.%02d,was loaded v.%02d.%02d", MAJOR_VERSION, MINOR_VERSION, reqMajorVersion, reqMinorVersion);
		_ASSERTE(("�����, �� ����� ����� ����������������� _���_ ������?",0));
		return P2ERR_COMMON_INVALID_INTERFACE_VERSION;
	}
}

#endif  // _IPREPLDB_H_
