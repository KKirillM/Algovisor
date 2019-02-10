#pragma once

#include "CommonStorage.h"

namespace nm_P2ReplClientTools {

class CBaselessStorage : public CCommonReplClientStorage
{
public:
	CBaselessStorage(P2DB_SCHEME_HANDLE custScheme, P2REPL_DATA_CALLBACK cbProc, void* cbParams,
						const P2REPL_REVISION_TYPE* pRevisions, size_t rev_count, UINT32 type);
    CBaselessStorage(P2REPL_DATA_CALLBACK cbProc, void* cbParams, UINT32 type);
	virtual ~CBaselessStorage(void);

	P2ERR OnBeforeChangeLifeNum(P2DB_SCHEME_HANDLE hServerScheme, P2DB_SCHEME_HANDLE hScheme);

	P2ERR OnSetScheme(P2DB_SCHEME_HANDLE hScheme);
	
	P2ERR DoInitRevisions(P2DB_SCHEME_HANDLE hScheme, const P2REPL_REVISION_TYPE* pRevisions, size_t rev_count);
	
	P2ERR DoClearDeleted(P2DB_SCHEME_HANDLE hScheme, size_t tblIdx, P2REPL_REVISION_TYPE rev);

	P2ERR OnReconnect(P2DB_SCHEME_HANDLE /*hScheme*/, UINT8) { return P2ERR_OK; };

	P2ERR OnChannelClose(UINT32 reason);

	void DoNotify(UINT32 notif, P2ERR reason, size_t tblIdx = 0, const void* pBody = NULL, size_t bodySize = 0, 
				  P2DB_RECORD_HANDLE pRec = NULL, bool sendNotif = true, const char *redirectServer = "", 
				  void* pAddData = NULL);
	P2ERR DoGetDBConn(P2DBConnection** /*ppDbConn*/) { return P2ERR_COMMON_NOT_SUPPORTED; }

	UINT32 DoDecrementRetryCount();
	void DoResetRetryCount();

	bool HasPersistRevisions() { return false; }

    virtual P2ERR LoadBinaryState(void * /*pBuffer*/, size_t* /*pBufferSz*/) { return P2ERR_COMMON_NOT_SUPPORTED; }
    virtual P2ERR SaveBinaryState(const void * /*pBuffer*/, size_t /*bufferSz*/) { return P2ERR_COMMON_NOT_SUPPORTED; }

    virtual P2ERR DoClearStorage(P2DB_SCHEME_HANDLE hScheme);
    virtual P2ERR DoCreateStorage(P2DB_SCHEME_HANDLE /*hScheme*/) { return P2ERR_OK; }

private:
	typedef std::vector<P2REPL_REVISION_TYPE> TRevisions;

	P2ERR IsConnected() { return P2ERR_OK; }
	P2ERR BeginTransaction() { return P2ERR_OK; }
	P2ERR GetRecRevision(size_t tblIndex, P2DB_RECORD_HANDLE hRecInMem, P2REPL_REVISION_TYPE* pRev);
	P2ERR InsUpdRec(size_t /*tblIndex*/, P2DB_RECORD_HANDLE /*pRec*/) { return P2ERR_OK; }
	P2ERR Commit() { return P2ERR_OK; }
	P2ERR Rollback() { return P2ERR_OK; }

private:
	P2REPL_DATA_CALLBACK    m_cbProc;
	void*                   m_cbParams;
};

}; //namespace nm_P2ReplClientTools


// пространство имен было сделано во избежание коллизий символов при линковке в юниксовой ветке (где so по экспортирует все символы)
using namespace nm_P2ReplClientTools;


