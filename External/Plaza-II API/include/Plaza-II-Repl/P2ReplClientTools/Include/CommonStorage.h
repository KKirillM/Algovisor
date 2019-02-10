#pragma once

#include "../../../Plaza-II/P2SysTypes/Include/P2SysTypes.h"
#include "../../P2ReplClient2/Include/P2ReplClient.h"
#include "../../P2ReplClient2/Include/IReplClientStorage.h"

#include <vector>
using std::vector;

////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define LOG_SUFFIX_FMT "%s"
#define LOG_SUFFIX_ARG m_logSuffix

namespace nm_P2ReplClientTools {

class CCommonReplClientStorage : public IReplClientStorage
{
public:
	IRC_IMPLEMENT()

    CCommonReplClientStorage() : IRC_INIT(), m_pStream(NULL), m_type(0), m_inTrans(false), m_retryCount(0)
    {
        memset(m_logSuffix, '\0', sizeof(m_logSuffix));
    };

    P2ERR Init(UINT32 type) { m_type = type; m_inTrans = false; m_retryCount = 0; return P2ERR_OK; }

	virtual ~CCommonReplClientStorage() { };

	virtual P2ERR OnChannelOpen(P2DB_SCHEME_HANDLE /*hServerScheme*/) { return P2ERR_OK; };
	virtual P2ERR OnChannelOpenFailed(UINT32 /*reason*/) { return P2ERR_OK; };

	virtual P2ERR OnSetSchemeFailed(UINT32 /*reason*/) { return P2ERR_OK; };

	virtual P2ERR DoComposeRevisionsToMsg(P2MSG_HANDLE hMsg);
	virtual P2ERR AdjustRevision(size_t tblIdx, P2REPL_REVISION_TYPE rev, UINT32 flags);

	virtual P2ERR DoBeginTransaction();

	virtual P2ERR DoProcessRecord(size_t tblIndex, P2DB_RECORD_HANDLE rec);

	virtual P2ERR DoCommit();

	virtual P2ERR DoRollback(bool rollbackDb /* = true*/);
	
	virtual P2ERR OnClose(UINT32 /*reason*/) { return P2ERR_OK; }

	virtual P2ERR OnError(P2ERR /*err*/) { return P2ERR_OK; }

	virtual UINT32 DoDecrementRetryCount();
	virtual void DoResetRetryCount();

	virtual void SetIStreamTools(IStreamTools *pStreamTools);

    virtual size_t GetRevisions(const P2REPL_REVISION_TYPE** ppRevisions);

protected:
	typedef std::vector<P2REPL_REVISION_TYPE> TRevisions;

protected:
	P2ERR CreateNotifMsgEx(P2MSG_HANDLE* pMsg, UINT32 notif, const void* pBody, size_t bodySize, const char *redirectServer);
	virtual P2ERR IsConnected() = 0;
	virtual P2ERR BeginTransaction() = 0;
	virtual P2ERR InsUpdRec(size_t tblIndex, P2DB_RECORD_HANDLE pRec) = 0;
	virtual P2ERR Commit() = 0;
	virtual P2ERR Rollback() = 0;

	P2ERR AdjustRevOnClearDeleted(size_t tblIdx, P2REPL_REVISION_TYPE rev);

	// здесь должен быть vector<bool>, но он некошерный. поэтому мухлюем
	typedef std::vector<char> BV;
	void DumpRevisions(const char *info, BV * pbv = NULL);
	void DumpChangedRevisions(const char *info, const TRevisions& revs);

	inline P2ERR CheckTableIndexRange(size_t tblIdx) const
	{
		return (tblIdx >= m_revisions.size()) ? (P2ERR) P2ERR_DB_NO_SUCH_INDEX : P2ERR_OK;
	}

	
protected:
	IStreamTools*      m_pStream;
	UINT32	           m_type;
	bool               m_inTrans;

	TRevisions         m_revisions;
	TRevisions         m_backupRevisions;

	UINT32	           m_retryCount;

    char               m_logSuffix[32];
};

#define P2_LOG_CAT "P2ReplStorage"

}; // namespace nm_P2ReplClientTools

// пространство имен было сделано во избежание коллизий символов при линковке в юниксовой ветке (где so по экспортирует все символы)
using namespace nm_P2ReplClientTools;
