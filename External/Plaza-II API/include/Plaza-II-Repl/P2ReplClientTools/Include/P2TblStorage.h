#pragma once

#include "../../../Plaza-II-DB/P2Tbl/Include/P2Tbl.hpp"
#include "../Include/CommonStorage.h"

#include <vector>
using std::vector;

namespace nm_P2ReplClientTools {

class CP2TblStorage : public CCommonReplClientStorage
{
public:
    CP2TblStorage();

	P2ERR Init(P2TBL_CONN_HANDLE tblConn, P2REPL_ISAM_DATA_CALLBACK icProc, void* cbParams, UINT32 type);

	P2ERR OnBeforeChangeLifeNum(P2DB_SCHEME_HANDLE hServerScheme, P2DB_SCHEME_HANDLE hScheme);

	P2ERR OnSetScheme(P2DB_SCHEME_HANDLE hScheme);
	
	P2ERR DoInitRevisions(P2DB_SCHEME_HANDLE hScheme, const P2REPL_REVISION_TYPE* pRevisions, size_t rev_count);
	
	P2ERR OnChannelClose(UINT32 reason);

	P2ERR DoClearDeleted(P2DB_SCHEME_HANDLE hScheme, size_t tblIdx, P2REPL_REVISION_TYPE rev);

	P2ERR OnReconnect(P2DB_SCHEME_HANDLE hScheme, UINT8);

	void DoNotify(UINT32 notif, P2ERR reason, size_t tblIdx = 0, const void* pBody = NULL, size_t bodySize = 0, 
				  P2DB_RECORD_HANDLE pRec = NULL, bool sendNotif = true, const char *redirectServer = "", void* pAddData = NULL);
		
	P2ERR DoGetDBConn(P2DBConnection** /*ppDbConn*/) { return P2ERR_COMMON_NOT_IMPLEMENTED; }
	
	bool HasPersistRevisions() { return true; }

    virtual P2ERR LoadBinaryState(void * /*pBuffer*/, size_t* pBufferSz) { *pBufferSz = 0; return P2ERR_OK; }
    virtual P2ERR SaveBinaryState(const void * /*pBuffer*/, size_t /*bufferSz*/) { return P2ERR_OK; }

    virtual P2ERR DoClearStorage(P2DB_SCHEME_HANDLE /*hScheme*/) { return P2ERR_OK; }
    virtual P2ERR DoCreateStorage(P2DB_SCHEME_HANDLE /*hScheme*/) { return P2ERR_OK; }

protected:
    ~CP2TblStorage(void);

private:
	typedef vector<P2TBL_TABLE_HANDLE> TTables;
	typedef vector<P2REPL_REVISION_TYPE> TRevisions;
	
private:
	CP2TblStorage(CP2TblStorage& obj); //to hide
	CP2TblStorage& operator=(CP2TblStorage& obj); //to hide

	P2ERR ClearIsam(P2DB_SCHEME_HANDLE hScheme);

	static P2ERR CloseTables(TTables* tables)
	{
		TTables::iterator it = tables->begin(), endIt = tables->end();
		for( ;it != endIt;++it)
		{
			_VERIFY_P2ERR(P2TBLCloseTable(*it));
		}
		tables->clear();

		return P2ERR_OK;
	}
	
	P2ERR GetNextTableRevision(size_t tblIdx, P2REPL_REVISION_TYPE* pRev);

	P2ERR ClearDb(P2DB_SCHEME_HANDLE hScheme);

	P2ERR IsConnected()
	{
		if (m_hTblConnA())
		{
			return P2ERR_OK;
		}
		else
		{
			return P2ERR_DB_NOT_CONNECTED;
		}
	}
	P2ERR BeginTransaction() {return P2TBLBeginTransaction(m_hTblConnA());}
	P2ERR GetRecRevision(size_t tblIndex, P2DB_RECORD_HANDLE hRecInMem, P2REPL_REVISION_TYPE* pRev);
	P2ERR InsUpdRec(size_t tblIndex, P2DB_RECORD_HANDLE pRec);
	P2ERR Commit() {return P2TBLCommit(m_hTblConnA());};
	P2ERR Rollback() {return P2TBLRollback(m_hTblConnA());};

private:
	P2REPL_ISAM_DATA_CALLBACK    m_cbProc;
	void*                        m_cbParams;
	
	P2AutoTblConnectionHandle    m_hTblConnA;

	TTables                      m_tables;
};

}; //namespace nm_P2ReplClientTools

// пространство имен было сделано во избежание коллизий символов при линковке в юниксовой ветке (где so по экспортирует все символы)
using namespace nm_P2ReplClientTools;
