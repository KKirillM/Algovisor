#pragma once

//#include "../../../Plaza-II-DB/P2DB/Include/P2DB.hpp"
#include "P2ReplUtils.h"
#include "../../P2ReplCommon/Include/P2Repl.h"
#include "../../../Plaza-II/P2Utils/Include/stuff.h"

/*inline'овый класс для автоматической установки в записях
правильных репликационных полей (id, revision, act).
Перед каждым вызовом ф-ции ***BeginTransaction, ***Commit, ***Rollback, ***Insert, ***Update или ***Delete
нужно вызывать соответствующую ф-цию Process*** класса CIdRevHandler'а.
*/

namespace nm_P2ReplUtils {


class CIdRevHandler
{
public:
	CIdRevHandler(void) : m_id(P2REPL_MIN_REVISION), m_rev(P2REPL_MIN_REVISION),
						  m_backupId(P2REPL_MIN_REVISION), m_backupRev(P2REPL_MIN_REVISION),
						  //m_maxId(P2REPL_MIN_REVISION), m_backupMaxId(P2REPL_MIN_REVISION),
						  m_inTrans(false), m_inited(false),
                          m_revMaxId(P2REPL_MIN_REVISION), m_backupRevMaxId(P2REPL_MIN_REVISION)					  
							{};
	~CIdRevHandler(void) {};

	inline P2ERR InitFromDB(PCSTR primaryTableName, P2DB_TABLE_SCHEME_HANDLE hTblScheme, P2DB_CONN_HANDLE hDbConn);
	//P2ERR Init(P2REPL_ID_TYPE id = P2REPL_MIN_REVISION, P2REPL_REVISION_TYPE rev = P2REPL_MIN_REVISION);
	inline P2ERR Init();
	inline P2ERR Init(P2REPL_ID_TYPE id, P2REPL_REVISION_TYPE maxIdRev, P2REPL_REVISION_TYPE rev);
	inline void Deinit();
	
	inline P2ERR ProcessBeginTransaction();
	inline P2ERR ProcessCommit();
	inline P2ERR ProcessRollback();

	inline P2ERR ProcessInsert(void* pRec);
	inline P2ERR ProcessUpdate(void* pRec);
	inline P2ERR ProcessDelete(void* pRec);

	P2ERR ProcessInsert(P2DB_RECORD_HANDLE hDbRec);
	P2ERR ProcessUpdate(P2DB_RECORD_HANDLE hDbRec);
	P2ERR ProcessDelete(P2DB_RECORD_HANDLE hDbRec);

	inline P2REPL_ID_TYPE GetNextId() {return m_id;};
	inline P2REPL_REVISION_TYPE GetNextRev() {return m_rev;};

	inline P2REPL_ID_TYPE GetMaxId() {return (m_id-1);};
	inline P2REPL_REVISION_TYPE GetRevMaxId() {return m_revMaxId;};

	inline P2REPL_ID_TYPE GetTransId() {return m_backupId;};
	inline P2REPL_ID_TYPE GetTransRev() {return m_backupRev;};

	inline P2ERR SetNextIdRev(P2REPL_ID_TYPE currId, P2REPL_REVISION_TYPE currRev);
	inline P2ERR SetNextRev(P2REPL_REVISION_TYPE nextRev);


	inline P2ERR ProcessClearOldRev(P2REPL_REVISION_TYPE rev);

private:
	P2REPL_ID_TYPE          m_id;
	P2REPL_REVISION_TYPE    m_rev;

	P2REPL_ID_TYPE          m_backupId;
	P2REPL_REVISION_TYPE    m_backupRev;

	bool                    m_inTrans;
	bool                    m_inited;


	P2REPL_REVISION_TYPE    m_revMaxId;
	P2REPL_REVISION_TYPE    m_backupRevMaxId;
};

inline P2ERR CIdRevHandler::InitFromDB(PCSTR /*primaryTableName*/, P2DB_TABLE_SCHEME_HANDLE hTblScheme, P2DB_CONN_HANDLE hDbConn)
{
	RETURN_P2ERR_COND_ASS((!m_inited), P2ERR_COMMON_ALREADY_INITIALIZED);

	RETURN_P2ERR(P2ReplGetLatestTableIdRevision(hDbConn, hTblScheme, &m_id, &m_rev, &m_revMaxId));

	++m_id;
	++m_rev;

	m_backupId = P2REPL_MIN_REVISION;
	m_backupRev = P2REPL_MIN_REVISION;
	//m_backupMaxId = P2REPL_MIN_REVISION;
	m_backupRevMaxId = P2REPL_MIN_REVISION;

	m_inited = true;
    	
	return P2ERR_OK;
}

//inline P2ERR CIdRevHandler::Init(P2REPL_ID_TYPE id /*=P2REPL_MIN_REVISION*/, P2REPL_REVISION_TYPE rev /*=P2REPL_MIN_REVISION*/)
/*{
	RETURN_P2ERR_COND_ASS((!m_inited), P2ERR_COMMON_ALREADY_INITIALIZED);

	if (id == P2REPL_MIN_REVISION)
	{
		m_revMaxId = P2REPL_MIN_REVISION;

		m_id = 1;
		m_rev = 1;

		m_backupId = P2REPL_MIN_REVISION;
		m_backupRev = P2REPL_MIN_REVISION;
		m_backupRevMaxId = P2REPL_MIN_REVISION;
	}
	else
	{
		m_revMaxId = rev;

		m_id = id + 1;
		m_rev = rev + 1;

		m_backupId = P2REPL_MIN_REVISION;
		m_backupRev = P2REPL_MIN_REVISION;
		m_backupRevMaxId = P2REPL_MIN_REVISION;
	}

	m_inited = true;

	return P2ERR_OK;
}
*/

inline P2ERR CIdRevHandler::Init()
{
	RETURN_P2ERR_COND_ASS((!m_inited), P2ERR_COMMON_ALREADY_INITIALIZED);
	
	m_revMaxId = P2REPL_MIN_REVISION;

	m_id = 1;
	m_rev = 1;

	m_backupId = P2REPL_MIN_REVISION;
	m_backupRev = P2REPL_MIN_REVISION;
	m_backupRevMaxId = P2REPL_MIN_REVISION;
	
	m_inited = true;

	return P2ERR_OK;
}

inline P2ERR CIdRevHandler::Init(P2REPL_ID_TYPE id, P2REPL_REVISION_TYPE maxIdRev, P2REPL_REVISION_TYPE rev)
{
	RETURN_P2ERR_COND_ASS((!m_inited), P2ERR_COMMON_ALREADY_INITIALIZED);
	
	m_revMaxId = maxIdRev;

	m_id = id + 1;
	m_rev = rev + 1;

	m_backupId = P2REPL_MIN_REVISION;
	m_backupRev = P2REPL_MIN_REVISION;
	m_backupRevMaxId = P2REPL_MIN_REVISION;
	
	m_inited = true;

	return P2ERR_OK;
}

inline P2ERR CIdRevHandler::ProcessBeginTransaction()
{
	RETURN_P2ERR_COND_ASS((m_inited), P2ERR_COMMON_OBJECT_NOT_INITED);
	RETURN_P2ERR_COND_ASS((!m_inTrans), P2ERR_ISAMSM_NESTED_TRANSACTIONS);

	m_backupId  = m_id;
	m_backupRev = m_rev;

	//m_backupMaxId = m_maxId;
	m_backupRevMaxId = m_revMaxId;

	m_inTrans = true;

	return P2ERR_OK;
}

inline P2ERR CIdRevHandler::ProcessCommit()
{
	RETURN_P2ERR_COND_ASS((m_inited), P2ERR_COMMON_OBJECT_NOT_INITED);
	RETURN_P2ERR_COND_ASS((m_inTrans), P2ERR_DB_COMMIT_WITHOUT_TRANSACTION);

	/*m_backupId  = P2REPL_MIN_REVISION;
	m_backupRev = P2REPL_MIN_REVISION;
	m_backupMaxId = P2REPL_MIN_REVISION;
	m_backupRevMaxId = P2REPL_MIN_REVISION;
	*/

	m_inTrans = false;

	return P2ERR_OK;
}

inline P2ERR CIdRevHandler::ProcessRollback()
{
	RETURN_P2ERR_COND_ASS((m_inited), P2ERR_COMMON_OBJECT_NOT_INITED);
	RETURN_P2ERR_COND_ASS((m_inTrans), P2ERR_DB_ROLLBACK_WITHOUT_TRANSACTION);
	
	m_id = m_backupId;
	m_rev = m_backupRev;
	//m_maxId = m_backupMaxId;
	m_revMaxId = m_backupRevMaxId;

	/*m_backupId  = P2REPL_MIN_REVISION;
	m_backupRev = P2REPL_MIN_REVISION;
	m_backupMaxId = P2REPL_MIN_REVISION;
	m_backupRevMaxId = P2REPL_MIN_REVISION;
	*/

	m_inTrans = false;

	return P2ERR_OK;
}
	
inline P2ERR CIdRevHandler::ProcessInsert(void* pRec)
{
	RETURN_P2ERR_COND_ASS((m_inited), P2ERR_COMMON_OBJECT_NOT_INITED);
	RETURN_NULL_ARG(pRec);

	TReplRecHdr& hdr = *reinterpret_cast<TReplRecHdr*>(pRec);

	hdr.id = m_id;
	hdr.rev = m_rev;
	hdr.deleted = P2REPL_DELETED_EXIST;

	//m_maxId = m_id;
	m_revMaxId = m_rev;

	++m_id;  
	++m_rev;

	return P2ERR_OK;
}

inline P2ERR CIdRevHandler::ProcessUpdate(void* pRec)
{
	RETURN_P2ERR_COND_ASS((m_inited), P2ERR_COMMON_OBJECT_NOT_INITED);
	RETURN_NULL_ARG(pRec);

	TReplRecHdr& hdr = *reinterpret_cast<TReplRecHdr*>(pRec);

	hdr.rev = m_rev;
	hdr.deleted = P2REPL_DELETED_EXIST;

	if (hdr.id == (m_id-1))
	{
        m_revMaxId = m_rev;
	}
	
	++m_rev;

	return P2ERR_OK;
}

inline P2ERR CIdRevHandler::ProcessDelete(void* pRec)
{
	RETURN_P2ERR_COND_ASS((m_inited), P2ERR_COMMON_OBJECT_NOT_INITED);
	RETURN_NULL_ARG(pRec);

	TReplRecHdr& hdr = *reinterpret_cast<TReplRecHdr*>(pRec);

	hdr.rev = m_rev;
	hdr.deleted = hdr.id;

	if (hdr.id == (m_id-1))
	{
        m_revMaxId = m_rev;
	}

	++m_rev;

	return P2ERR_OK;
}

inline P2ERR CIdRevHandler::ProcessInsert(P2DB_RECORD_HANDLE hDbRec)
{
	RETURN_P2ERR_COND_ASS((m_inited), P2ERR_COMMON_OBJECT_NOT_INITED);
	RETURN_NULL_ARG(hDbRec);

	RETURN_P2ERR(P2DBSetFieldByIndex(hDbRec, P2REPL_ID_FIELD_INDEX, &m_id, P2REPL_ID_FIELD_SIZE));
	RETURN_P2ERR(P2DBSetFieldByIndex(hDbRec, P2REPL_REVISION_FIELD_INDEX, &m_rev, P2REPL_REVISION_FIELD_SIZE));
	P2REPL_DELETED_TYPE deleted = P2REPL_DELETED_EXIST;
	RETURN_P2ERR(P2DBSetFieldByIndex(hDbRec, P2REPL_DELETED_FIELD_INDEX, &deleted, sizeof(deleted)));

	//m_maxId = m_id;
	m_revMaxId = m_rev;

	++m_id;  
	++m_rev;

	return P2ERR_OK;
}

inline P2ERR CIdRevHandler::ProcessUpdate(P2DB_RECORD_HANDLE hDbRec)
{
	RETURN_P2ERR_COND_ASS((m_inited), P2ERR_COMMON_OBJECT_NOT_INITED);
	RETURN_NULL_ARG(hDbRec);

	RETURN_P2ERR(P2DBSetFieldByIndex(hDbRec, P2REPL_REVISION_FIELD_INDEX, &m_rev, P2REPL_REVISION_FIELD_SIZE));
	P2REPL_DELETED_TYPE deleted = P2REPL_DELETED_EXIST;
	RETURN_P2ERR(P2DBSetFieldByIndex(hDbRec, P2REPL_DELETED_FIELD_INDEX, &deleted, sizeof(deleted)));

	P2REPL_ID_TYPE recId = P2REPL_MIN_REVISION;
	size_t idSize = sizeof(recId);
	RETURN_P2ERR(P2DBGetFieldByIndex(hDbRec, P2REPL_ID_FIELD_INDEX, &recId, &idSize));

	if (recId == (m_id-1))
	{
        m_revMaxId = m_rev;
	}

	++m_rev;

	return P2ERR_OK;
}

inline P2ERR CIdRevHandler::ProcessDelete(P2DB_RECORD_HANDLE hDbRec)
{
	RETURN_P2ERR_COND_ASS((m_inited), P2ERR_COMMON_OBJECT_NOT_INITED);
	RETURN_NULL_ARG(hDbRec);

	P2REPL_ID_TYPE id = P2REPL_MIN_REVISION;
	size_t size = 0;
	RETURN_P2ERR(P2DBGetFieldByIndex(hDbRec, P2REPL_ID_FIELD_INDEX, &id, &size));

	RETURN_P2ERR(P2DBSetFieldByIndex(hDbRec, P2REPL_REVISION_FIELD_INDEX, &m_rev, P2REPL_REVISION_FIELD_SIZE));
	P2REPL_DELETED_TYPE deleted = id;
	RETURN_P2ERR(P2DBSetFieldByIndex(hDbRec, P2REPL_DELETED_FIELD_INDEX, &deleted, sizeof(deleted)));

	if (id == (m_id-1))
	{
        m_revMaxId = m_rev;
	}

	++m_rev;

	return P2ERR_OK;
}

inline void CIdRevHandler::Deinit()
{
	m_id = P2REPL_MIN_REVISION;
	m_rev = P2REPL_MIN_REVISION;
	//m_maxId = P2REPL_MIN_REVISION;
	m_revMaxId = P2REPL_MIN_REVISION;

	m_backupId = P2REPL_MIN_REVISION;
	m_backupRev = P2REPL_MIN_REVISION;
	//m_backupMaxId = P2REPL_MIN_REVISION;
	m_backupRevMaxId = P2REPL_MIN_REVISION;

	m_inTrans = false;
	m_inited = false;
}

inline P2ERR CIdRevHandler::SetNextIdRev(P2REPL_ID_TYPE currId, P2REPL_REVISION_TYPE currRev)
{
	RETURN_P2ERR_COND_ASS((m_inited), P2ERR_COMMON_OBJECT_NOT_INITED);
	
	if (currRev < m_rev)
	{
		return P2ERR_COMMON_WRONG_ARGUMENTS;
	}

	if (currId >= (m_id-1))
	{
		m_id = currId + 1;
        m_revMaxId = currRev;                
	}
	
    m_rev = currRev + 1;		

	return P2ERR_OK;
}

inline P2ERR CIdRevHandler::SetNextRev(P2REPL_REVISION_TYPE nextRev)
{
	RETURN_P2ERR_COND_ASS((m_inited), P2ERR_COMMON_OBJECT_NOT_INITED);
	m_rev = nextRev; 
	m_id  = 0;			//минимальное значение, любое значение в SetNextIdRev установится (repl_id_type -- signed type)
	m_revMaxId = 0;
	return P2ERR_OK;
}

inline P2ERR CIdRevHandler::ProcessClearOldRev(P2REPL_REVISION_TYPE rev)
{
	RETURN_P2ERR_COND_ASS((m_inited), P2ERR_COMMON_OBJECT_NOT_INITED);

	if (rev >= m_rev)
	{
		m_revMaxId = P2REPL_MIN_REVISION;

		m_id = 1;
		m_rev = 1;
	}
	return P2ERR_OK;
}
}; //namespace nm_P2ReplUtils

// пространство имен было сделано во избежание коллизий символов при линковке в юниксовой ветке (где so по экспортирует все символы)
using namespace nm_P2ReplUtils;

