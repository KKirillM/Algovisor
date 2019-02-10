#pragma once

#include "../../../Plaza-II/P2SysTypes/Include/P2SysTypes.h"
//TODO: remove include
#include "../../P2ReplClient2/Include/P2ReplClient.h"
#include "../../P2ReplClient2/Include/IReplClientStorage.h"
//#include "CommonStorage.h"

#include <vector>
using std::vector;

class CBaselessStorageVarArgs : public IReplClientStorage
{
public:
	IRC_IMPLEMENT()

public:
	CBaselessStorageVarArgs(P2DB_SCHEME_HANDLE custScheme, P2REPL_VA_DATA_CALLBACK cbProc, void* cbParams,
						const P2REPL_REVISION_TYPE* pRevisions, size_t rev_count, UINT32 type);
    CBaselessStorageVarArgs(P2REPL_VA_DATA_CALLBACK cbProc, void* cbParams, UINT32 type);
	~CBaselessStorageVarArgs(void);

	P2ERR OnBeforeChangeLifeNum(P2DB_SCHEME_HANDLE hServerScheme, P2DB_SCHEME_HANDLE hScheme);

	P2ERR OnSetScheme(P2DB_SCHEME_HANDLE hScheme);
	
	P2ERR DoInitRevisions(P2DB_SCHEME_HANDLE hScheme, const P2REPL_REVISION_TYPE* pRevisions, size_t rev_count);
	
	P2ERR DoClearDeleted(P2DB_SCHEME_HANDLE hScheme, size_t tblIdx, P2REPL_REVISION_TYPE rev);

	P2ERR OnReconnect(P2DB_SCHEME_HANDLE /*hScheme*/, UINT8) { return P2ERR_OK; }

	P2ERR OnChannelClose(UINT32 reason);

	void DoNotify(UINT32 notif, P2ERR reason, size_t tblIdx = 0, const void* pBody = NULL, size_t bodySize = 0, 
				  P2DB_RECORD_HANDLE pRec = NULL, bool sendNotif = true, const char *redirectServer = "", void* pAddData = NULL);

	P2ERR DoGetDBConn(P2DBConnection** /*ppDbConn*/) { return P2ERR_COMMON_NOT_SUPPORTED; }

	UINT32 DoDecrementRetryCount();
	void DoResetRetryCount();

	bool HasPersistRevisions() {return false;}

    P2ERR LoadBinaryState(void * /*pBuffer*/, size_t* /*pBufferSz*/) { return P2ERR_COMMON_NOT_SUPPORTED; }
    P2ERR SaveBinaryState(const void * /*pBuffer*/, size_t /*bufferSz*/) { return P2ERR_COMMON_NOT_SUPPORTED; }

    P2ERR DoClearStorage(P2DB_SCHEME_HANDLE hScheme);
    P2ERR DoCreateStorage(P2DB_SCHEME_HANDLE /*hScheme*/) { return P2ERR_OK; }

	P2ERR DoProcessRecord(size_t tblIndex, P2DB_RECORD_HANDLE pInRec);

	P2ERR DoBeginTransaction();
	P2ERR DoCommit();
	P2ERR DoRollback(bool rollbackDb /* = true*/);


	P2ERR OnChannelOpen(P2DB_SCHEME_HANDLE/*hServerScheme*/) { return P2ERR_OK; }
	P2ERR OnChannelOpenFailed(UINT32 /*reason*/) { return P2ERR_OK; }

	P2ERR OnSetSchemeFailed(UINT32 /*reason*/) { return P2ERR_OK; }

	P2ERR DoComposeRevisionsToMsg(P2MSG_HANDLE hMsg);
	P2ERR AdjustRevision(size_t tblIdx, P2REPL_REVISION_TYPE rev, UINT32 flags);
	P2ERR OnClose(UINT32 /*reason*/) { return P2ERR_OK; }

	P2ERR OnError(P2ERR /*err*/) { return P2ERR_OK; }

	void SetIStreamTools(IStreamTools *pStreamTools);

    size_t GetRevisions(const P2REPL_REVISION_TYPE** ppRevisions);


private:
	typedef std::vector<P2REPL_REVISION_TYPE> TRevisions;

protected:
	P2ERR AdjustRevOnClearDeleted(size_t tblIdx, P2REPL_REVISION_TYPE rev);

	// здесь должен быть vector<bool>, но он некошерный. поэтому мухлюем
	typedef std::vector<char> BV;
	void DumpRevisions(const char *info, BV * pbv = NULL);
	void DumpChangedRevisions(const char *info, const TRevisions& revs);
	inline P2ERR CheckTableIndexRange(size_t tblIdx) const
	{
		return (tblIdx >= m_revisions.size()) ? (P2ERR) P2ERR_DB_NO_SUCH_INDEX : P2ERR_OK;
	}

private:
	P2REPL_VA_DATA_CALLBACK    m_cbProc;
	void*                   m_cbParams;

protected:
	IStreamTools*      m_pStream;
	UINT32	           m_type;
	bool               m_inTrans;

	TRevisions         m_revisions;
	TRevisions         m_backupRevisions;

	char                m_logSuffix[32];
};
