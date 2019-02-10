#pragma once
#include <string>
#include "../../../Plaza-II-DB/P2DB/Include/P2DB.hpp"
#include "../../P2ReplUtils/Include/P2ReplUtils.h"
#include "../../../Plaza-II/P2SysScheme/Include/P2SysScheme.hpp"


#include "CommonStorage.h"

#define TRACE_LOG_CAT() P2_LOG_CAT

////////////////////////////////////////////////////////////////////////////////////////////
//class CDbPolicy
///////////////////////////////////////////////////////////////////////////////////////////

namespace nm_P2ReplClientTools {

using nm_P2ReplUtils::CInsOrUpdList; 
using nm_P2ReplUtils::CInsOrUpdHelper;

class CP2DBStorage : public CCommonReplClientStorage
{
public:
    CP2DBStorage();
	virtual ~CP2DBStorage(void);

    P2ERR Init(P2REPL_DATA_CALLBACK cbProc, void* cbParams, UINT32 type, const char * streamName);

    P2ERR DoInitRevisions(P2DB_SCHEME_HANDLE hScheme, const P2REPL_REVISION_TYPE* pRevisions, size_t rev_count);
	
	P2ERR DoClearDeleted(P2DB_SCHEME_HANDLE hScheme, size_t tblIdx, P2REPL_REVISION_TYPE rev);

	P2ERR OnReconnect(P2DB_SCHEME_HANDLE hScheme, UINT8);

	virtual void DoNotify(UINT32 notif, P2ERR reason, size_t tblIdx = 0, const void* pBody = NULL, size_t bodySize = 0, 
						  P2DB_RECORD_HANDLE pRec = NULL, bool sendNotif = true, const char *redirectServer = "", void* pAddData = NULL);

	P2ERR DoGetDBConn(P2DBConnection** ppDbConn);

	bool HasPersistRevisions() { return true; }

	P2ERR OnChannelClose(UINT32 reason);

protected:
	virtual P2ERR ClearDb(P2DB_SCHEME_HANDLE hScheme) = 0;
	P2ERR InitDbRecs(P2DB_SCHEME_HANDLE hScheme);
	virtual P2ERR ClearDbRecs();

    P2ERR BeginTransaction();
    P2ERR Commit();
    P2ERR Rollback();

    static std::string MakeInternalStreamName(const std::string& s);

protected:
	AutoConnectionHandle    m_hDbConnA;
	CInsOrUpdList           m_insOrUpdList; // класс для вставки в базу
    std::string             m_name;
    bool                    m_dbErrorOccured;

private:
	CP2DBStorage(CP2DBStorage& obj); //to hide
	CP2DBStorage& operator=(CP2DBStorage& obj); //to hide

	P2ERR IsConnected()
	{
		if (m_hDbConnA())
		{
			return P2ERR_OK;
		}
		else
		{
			return P2ERR_DB_NOT_CONNECTED;
		}
	}

	P2ERR GetRecRevision(size_t tblIndex, P2DB_RECORD_HANDLE hRecInMem, P2REPL_REVISION_TYPE* pRev);
	P2ERR InsUpdRec(size_t tblIndex, P2DB_RECORD_HANDLE pRec);
	
private:
	typedef std::vector<P2DB_RECORD_HANDLE> TDbRecs;

	P2REPL_DATA_CALLBACK     m_cbProc;
	void*                    m_cbParams;

	bool                     m_inTrans;
	bool                     m_removeDeleted;

	TDbRecs                  m_dbRecs;
};


////////////////////////////////////////////////////////////////////////////////////////////
//class CP2DBStrStorage
///////////////////////////////////////////////////////////////////////////////////////////

class CP2DBStrStorage : public CP2DBStorage
{
public:
    CP2DBStrStorage();
	~CP2DBStrStorage(void);

    P2ERR Init(const char *pDbConnStr, P2REPL_DATA_CALLBACK cbProc, void* cbParams, UINT32 type,
        const char *streamName, const P2REPL_REVISION_TYPE* pRevs = NULL, size_t revCount = 0);
	
	P2ERR OnBeforeChangeLifeNum(P2DB_SCHEME_HANDLE hServerScheme, P2DB_SCHEME_HANDLE hScheme);

	P2ERR OnSetScheme(P2DB_SCHEME_HANDLE hScheme);

	P2ERR DoInitRevisions(P2DB_SCHEME_HANDLE hScheme, const P2REPL_REVISION_TYPE* pRevisions, size_t rev_count);

    virtual P2ERR LoadBinaryState(void * pBuffer, size_t* pBufferSz);
    virtual P2ERR SaveBinaryState(const void * pBuffer, size_t bufferSz);

    virtual P2ERR DoClearStorage(P2DB_SCHEME_HANDLE hScheme);
    virtual P2ERR DoCreateStorage(P2DB_SCHEME_HANDLE hScheme);

protected:
    std::string    m_dbConnStr;

private:
	CP2DBStrStorage(CP2DBStrStorage& obj);

	P2ERR ClearDb(P2DB_SCHEME_HANDLE hScheme);

	P2ERR SaveRevToScheme(P2DB_CONN_HANDLE hDbConn, PCSTR schemeName, P2DB_TABLE_SCHEME_HANDLE hTableScheme, PCSTR revStr);
};


////////////////////////////////////////////////////////////////////////////////////////////
//class CP2DBConnStorage
///////////////////////////////////////////////////////////////////////////////////////////

class CP2DBConnStorage : public CP2DBStorage
{
public:
	CP2DBConnStorage();
	~CP2DBConnStorage(void);

	P2ERR Init(P2DB_CONN_HANDLE dbConn, P2REPL_DATA_CALLBACK cbProc, void* cbParams, UINT32 type, PCSTR streamName);
		
	P2ERR OnBeforeChangeLifeNum(P2DB_SCHEME_HANDLE hServerScheme, P2DB_SCHEME_HANDLE hScheme);
	
	P2ERR OnSetScheme(P2DB_SCHEME_HANDLE hScheme);

    virtual P2ERR LoadBinaryState(void * /*pBuffer*/, size_t* pBufferSz)
    {
        if (pBufferSz)
            { *pBufferSz = 0; return P2ERR_OK; }
        else
            return P2ERR_COMMON_WRONG_ARGUMENTS;
    }
    virtual P2ERR SaveBinaryState(const void * /*pBuffer*/, size_t /*bufferSz*/) { return P2ERR_OK; }
    virtual P2ERR DoClearStorage(P2DB_SCHEME_HANDLE hScheme);
    virtual P2ERR DoCreateStorage(P2DB_SCHEME_HANDLE /*hScheme*/) { return P2ERR_OK; }

private:
	CP2DBConnStorage(CP2DBConnStorage& obj);

	P2ERR ClearDb(P2DB_SCHEME_HANDLE hScheme);
};

}; //namespace nm_P2ReplClientTools

// пространство имен было сделано во избежание коллизий символов при линковке в юниксовой ветке (где so по экспортирует все символы)
using namespace nm_P2ReplClientTools;
