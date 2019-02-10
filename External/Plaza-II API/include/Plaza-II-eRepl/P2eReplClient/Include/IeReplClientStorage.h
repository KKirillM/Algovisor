#ifndef _IREPLCLIENTSTORAGE_H
#define _IREPLCLIENTSTORAGE_H

#include "../../../Plaza-II/P2Utils/Include/IRc.h"
#include "../../../Plaza-II/P2Utils/Include/ServiceDriver.h"
#include "../../../Plaza-II/P2SysRecord/Include/P2SysRecord.h"
#include "../../P2eReplCommon/Include/P2eReplTypes.h"

/*
 * IStreamTools - ���������, ���������� �������� ������ ������ ��� ������ �� ����������
 */
class IStreamTools
{
public:
	// ��������� �������� ��������� ������ � ���� ������� ��������� P2EREPL_DATASTREAM_STATE
	virtual UINT32 GetState() = 0;
};

class P2eReplCliHostControl
{
public:
	enum { 
		MAJOR_VERSION = 1,
		MINOR_VERSION = 0 
	};
};

const UINT32 cMajorVer = 8;
const UINT32 cMinorVer = 0;

////////
//interface IReplClientStorage declaration
// ����������, ����������� IReplClientStorage
// 1. ��������� ���������� ����: ��������, ������������ � ����������-�������� �������
// 2. ��������� ���������� �������� ������� � ��������
// 3. ����� ���������� �����������
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ��������! ��� ��������� ��������� ���������� ��������� ������ ���������� IeReplClientStorage
struct TNotifData
{
    P2ERR                    m_reason;
    PCSTR                    m_pRedirectServer;
    UINT32                   m_lifeNum;
    P2EREPL_REVISION_TYPE    m_clearRev;
    UINT32                   m_clearRevFlags;

    TNotifData() : m_reason(P2ERR_OK), m_pRedirectServer(NULL), m_lifeNum(0), m_clearRev(P2EREPL_MIN_REVISION), m_clearRevFlags(0)
    {
    };
};

struct IeReplClientStorage : public IRc, public IMQPluginImpl<cMajorVer, cMinorVer, IeReplClientStorage>
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
	 * + OnChannelClose - ����� ��� ������. � ���� ������ ������������� �������, ��������� � �����
	 * + OnReconnect - �� ������� ���� �������� ��������� RECONNECT. �������� clear - ������������!
	 * + OnError - ���������� � ������ �������� � ��������� ������.
	 */
	virtual P2ERR OnChannelOpen() = 0;
	virtual P2ERR OnChannelOpenFailed(UINT32 reason) = 0;
	virtual P2ERR OnChannelClose(P2ERR reason) = 0;
	virtual P2ERR OnReconnect() = 0;
	virtual P2ERR OnError(P2ERR err) = 0;

    virtual P2ERR OnLifeNumChanged(UINT32 serverLifeNum) = 0;

	/* ���� ����� ������� �������� �� ��������� ����������. �������� ��� �������� ������� ��������:
	 * - ��������� ��������� ��������� ������ � ���� � ������������� �������-���������-����������� �.
	 * - ��������� �� ������������ ����������� ��������. � ���� ������ �������������� ��������� ��������� ������������
	 * + HasPersistRevisions - ����� ���������� true, ���� ��������� ��������� �������
	 * + DoInitRevisions
	 * + DoComposeRevisionsToMsg - ����� �������� ���� ��������� �������� ���������.
	 */
	/*virtual bool HasPersistRevisions() = 0;
	virtual P2ERR DoInitRevisions(P2DB_SCHEME_HANDLE hScheme, const P2REPL_REVISION_TYPE* pRevisions, size_t rev_count) = 0;
	*/

    virtual P2ERR DoInitRevision(P2EREPL_REVISION_TYPE rev) = 0;
    virtual P2ERR DoComposeRevisionToMsg(P2MSG_HANDLE hMsg) = 0;
    virtual P2ERR AdjustRevision(P2EREPL_REVISION_TYPE lastRev, UINT32 flags) = 0;

    /* ����� ������� ��� ��������� ����������. 
	 * ������������ ����������� ������: � DoRollback ���������� ����, ������������ ����� ��
	 * ������ rollback ������ ��
	 */
	virtual P2ERR DoBeginTransaction() = 0;
	virtual P2ERR DoCommit() = 0;
	virtual P2ERR DoRollback() = 0;

	// ������� DoProcessEvent - ���������� ��������� �������.
	virtual P2ERR DoProcessEvent(const TEReplRec* pEvent) = 0;
	// DoClearOldRev - �������� �������� �� ������� �� ����������
	virtual P2ERR DoClearOldRev(P2EREPL_REVISION_TYPE rev, UINT32 flags) = 0;
	// DoNotify �������� ���������� ����������� � ��������������� ����������
	virtual void DoNotify(UINT32 notif, const TNotifData* pNotifData) = 0;
    // DoFastNotify �������� ���������� ����������� ��� ����� ����������� ����������� (��� ���������� ���������������� ���������)
    virtual void DoFastNotify(UINT32 notif, const TEReplRec* pRec = NULL) = 0;

    /* ������� �� �������� ���������� ���� (�� ���������� ����� ����) */
    //virtual P2ERR DoClearStorage(P2DB_SCHEME_HANDLE hScheme) = 0; // � ���� ������ ���������/��������� ������ ����.
    /* ������� �� �������� ���� �� ����� */
    //virtual P2ERR DoCreateStorage(P2DB_SCHEME_HANDLE hScheme) = 0; // ����� ��������� ����� ���� �� �������������.

    virtual P2ERR DoClearStorage() = 0; // � ���� ������ ���������/��������� ������ ����.

    virtual void SetIStreamTools(IStreamTools *pStreamTools) = 0;
};

#endif