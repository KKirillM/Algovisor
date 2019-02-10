#ifndef _IREPLCLIENTSTORAGE_H
#define _IREPLCLIENTSTORAGE_H

#include "../../../Plaza-II/P2Utils/Include/IRc.h"
#include "../../../Plaza-II/P2Utils/Include/SmartIRc.h"
#include "../../../Plaza-II/P2Utils/Include/ServiceDriver.h"
#include "../../../Plaza-II/P2SysRecord/Include/P2SysRecord.h"
#include "../../../Plaza-II-Repl/P2ReplCommon/Include/P2Repl.h"

/*
 * IStreamTools - ���������, ���������� �������� ������ ������ ��� ������ �� ����������
 */
class IStreamTools
{
public:
	// ��������� �������� ��������� ������ � ���� ������� ��������� P2REPL_DATASTREAM_STATE
	virtual UINT32 GetState() = 0;
	// ������� ������ ���������������� �����������. 
	// ����� ����� ����� ������� (������ �������� ��� �� ������, ������� � ���� ������� �������� ��� �� ������)
	// ������ � ����� ����� - ��������� ����������� ������ ����� ������� ����������� � �������� � ����� ��������
	// ������� ���������� true, ���� �� ����� ����������� ��������� ������ �� ���������� � false, ���� ���������� (��������, ����� ��� ������ ��� �����������)
	virtual bool NotifyAndCheckState(UINT32 notif, P2ERR reason = P2ERR_OK) = 0;
	virtual bool NotifyAndCheckStateEx(UINT32 notif, P2ERR reason, size_t tblIdx, const void* pBody = NULL, size_t bodySize = 0, P2DB_RECORD_HANDLE rec = NULL, bool sendNotif = true) = 0;
	// ������� ���������� ��������� ������� ������� ������ � ����
	virtual UINT32 GetRetryCount(void) = 0;
	virtual UINT32 GetRetryTimeout(void) = 0;

	virtual void *GetRecordPtr(P2DB_RECORD_HANDLE rec) = 0;
	virtual P2REPL_REVISION_TYPE GetRecordRev(P2DB_RECORD_HANDLE rec) = 0;
};

class P2ReplCliHostControl
{
public:
	enum { 
		MAJOR_VERSION = 1,
		MINOR_VERSION = 0 
	};
};

const UINT32 cMajorVer = 10;
const UINT32 cMinorVer = 0;

////////
//interface IReplClientStorage declaration
// ����������, ����������� IReplClientStorage
// 1. ��������� ���������� ����: ��������, ������������ � ����������-�������� �������
// 2. ��������� ���������� �������� ������� � ��������
// 3. ����� ���������� �����������
////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct IReplClientStorage : public IRc, public IMQPluginImpl<cMajorVer, cMinorVer, IReplClientStorage>
{
public:
	enum
	{
        MAJOR_VERSION = cMajorVer,
		MINOR_VERSION = cMinorVer 
	};

	/* ���� ����� ������� ������ ������������ ����� ��������� ������� ����������
	 * + OnChannelOpen - ������ ���������� �������� � ������� ���� �����. ���������� ���������� � ����� �� 
	 * ��������� ��������� �� �������. ������ ������ �� ������, ��� ���������� ����� ��� ����������.
	 * + OnChannelOpenFailed - ������ ������� � ������� � ������. ���������� ���������� � ����� �� 
	 * ��������� ��������� �� �������. ������ ������ �� ������.
	 * + OnBeforeChangeLifeNum - ������ ��������� ������������� ������� ����� �����. ���������� ���������� ���
	 * ��������� ����� ���������� ��� �������� ������ ��� ���������� � SetScheme.
	 * + OnSetScheme - ������ ���������� � ���������� ������ �� ���������� �����.
	 * + OnSetSchemeFailed - ������ ������� � ��������� ���������� �����
	 * + OnChannelClose - ����� ��� ������. � ���� ������ ������������� �������, ��������� � �����
	 * + OnReconnect - �� ������� ���� �������� ��������� RECONNECT. �������� clear - ������������!
	 * + OnError - ���������� � ������ �������� � ��������� ������.
	 */
	virtual P2ERR OnChannelOpen(P2DB_SCHEME_HANDLE	hServerScheme) = 0;
	virtual P2ERR OnChannelOpenFailed(UINT32 reason) = 0;
	virtual P2ERR OnBeforeChangeLifeNum(P2DB_SCHEME_HANDLE hServerScheme, P2DB_SCHEME_HANDLE hScheme) = 0;
	virtual P2ERR OnSetScheme(P2DB_SCHEME_HANDLE hScheme) = 0;
	virtual P2ERR OnSetSchemeFailed(UINT32 reason) = 0;
	virtual P2ERR OnChannelClose(P2ERR reason) = 0;
	virtual P2ERR OnReconnect(P2DB_SCHEME_HANDLE hScheme, UINT8 /*clear*/ /*ignore it!*/) = 0;
	virtual P2ERR OnError(P2ERR err) = 0;


	/* ���� ����� ������� �������� �� ��������� ����������. �������� ��� �������� ������� ��������:
	 * - ��������� ��������� ��������� ������ � ���� � ������������� �������-���������-����������� �.
	 * - ��������� �� ������������ ����������� ��������. � ���� ������ �������������� ��������� ��������� ������������
	 * + HasPersistRevisions - ����� ���������� true, ���� ��������� ��������� �������
	 * + DoInitRevisions
	 * + DoComposeRevisionsToMsg - ����� �������� ���� ��������� �������� ���������.
	 */
	virtual bool HasPersistRevisions() = 0;
	virtual P2ERR DoInitRevisions(P2DB_SCHEME_HANDLE hScheme, const P2REPL_REVISION_TYPE* pRevisions, size_t rev_count) = 0;
	virtual P2ERR DoComposeRevisionsToMsg(P2MSG_HANDLE hMsg) = 0;
	virtual P2ERR AdjustRevision(size_t tblIdx, P2REPL_REVISION_TYPE rev, UINT32 flags) = 0;

	/* ����� ������� ��� ��������� ����������. 
	 * ������������ ����������� ������: � DoRollback ���������� ����, ������������ ����� ��
	 * ������ rollback ������ ��
	 */
	virtual P2ERR DoBeginTransaction() = 0;
	virtual P2ERR DoCommit() = 0;
	virtual P2ERR DoRollback(bool rollbackDb = true) = 0;

	// ������� DoProcessRecord - ���������� ��������� �������.
	virtual P2ERR DoProcessRecord(size_t tblIndex, P2DB_RECORD_HANDLE rec) = 0;
	// DoClearDeleted - �������� �������� �� ������� �� ����������
	virtual P2ERR DoClearDeleted(P2DB_SCHEME_HANDLE hScheme, size_t tblIdx, P2REPL_REVISION_TYPE rev) = 0;
	// DoNotify �������� ���������� �����������
	virtual void DoNotify(UINT32 notif, P2ERR reason, size_t tblIdx = 0, const void* pBody = NULL, size_t bodySize = 0, 
						  P2DB_RECORD_HANDLE pRec = NULL, bool sendNotif = true, const char *redirectServer = "", 
						  void* pAddData = NULL) = 0;

	virtual P2ERR DoGetDBConn(P2DBConnection** ppDbConn) = 0;

	virtual UINT32 DoDecrementRetryCount() = 0;
	virtual void DoResetRetryCount() = 0;

	virtual void SetIStreamTools(IStreamTools *pStreamTools) = 0;

    /* ������ ��������/���������� ���������� ������.
     * ����������� ������ ��� ������� ��������.
     * ��� ���������� �������� ����� ��������� ���������� � ����������� ������, � ���������� ��������� ��������.
     */
    // LoadBinaryState: � ������ ���������� � ���� ���������� � ���������� ��������� ���������� ������ (*pBufferSz = 0)
    virtual P2ERR LoadBinaryState(void * pBuffer, size_t* pBufferSz) = 0;
    virtual P2ERR SaveBinaryState(const void * pBuffer, size_t bufferSz) = 0;
    virtual size_t GetRevisions(const P2REPL_REVISION_TYPE** ppRevisions) = 0;

    /* ������� �� �������� ���������� ���� (�� ���������� ����� ����) */
    virtual P2ERR DoClearStorage(P2DB_SCHEME_HANDLE hScheme) = 0; // � ���� ������ ���������/��������� ������ ����.
    /* ������� �� �������� ���� �� ����� */
    virtual P2ERR DoCreateStorage(P2DB_SCHEME_HANDLE hScheme) = 0; // ����� ��������� ����� ���� �� �������������.
};

typedef CRcPtr< IReplClientStorage > CRcReplClientStorage;

#endif // _IREPLCLIENTSTORAGE_H
