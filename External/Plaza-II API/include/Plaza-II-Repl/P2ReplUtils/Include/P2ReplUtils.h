#ifndef P2REPL_UTILS_H_
#define P2REPL_UTILS_H_

#include "../../P2ReplCommon/Include/P2Repl.h"
#include "../../../Plaza-II-DB/P2DB/Include/P2DB.hpp"
#include "../../../Plaza-II/P2SysTypes/Include/P2SysTypes.hpp"
#include "P2ReplDS.h"

#include <vector>

//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// ������� ��� ������� ���������� InsertOrUpdate ��� P2DB
// ������ �� ������ ��������� ������� P2ReplInsertOrUpdatePreparedRecordEx2
// ���������� ���, ���
// 1. ��� ����������� ������� ����������� ����������
// 2. �������������� ��� ���������� � ����� ����������

#define P2HISTORY_REC__MODE__FIELD_TYPE             UINT8
#define P2HISTORY_REC__MODE__FIELD_FORMAT           "u1"
#define P2HISTORY_REC__INS_MOMENT__FIELD_FORMAT     "t"

#define P2HISTORY_REC__TRANS_ID__FIELD_TYPE         INT32 // optional
#define P2HISTORY_REC__TRANS_ID__FIELD_FORMAT       "i4"
#define P2HISTORY_REC__TRANS_ID__FIELD_NAME_PART    "trans_id"

#define P2HISTORY_REC__MODE__FIELD_INDEX            1
#define P2HISTORY_REC__INS_MOMENT__FIELD_INDEX      2
#define P2HISTORY_REC__TRANS_ID__FIELD_INDEX        3


namespace nm_P2ReplUtils {

class CInsOrUpdHelper
{
public:
	CInsOrUpdHelper();
	~CInsOrUpdHelper();

	// tableIdx -- ����� � ������� � ����� hScheme, ���� hScheme==NULL, ������������ ����� ��
	P2ERR Open(P2DB_CONN_HANDLE dbConn, size_t tableIdx,P2DB_SCHEME_HANDLE hScheme = NULL);

	void  SetPreparedRecsUsage(bool bUseIt) {m_isPreparedRecsUsing = bUseIt; }
	bool  IsPreparedRecsUsing()				{ return m_isPreparedRecsUsing;  }

	P2ERR Close();
	void  Swap(CInsOrUpdHelper& rh);

	P2ERR InsOrUpd(P2DB_RECORD_HANDLE rec, bool removeDeleted, UINT32 flag, P2REPL_REVISION_TYPE *pRev = NULL);

	const char *GetTableName(void) const { return m_tblName; }
	P2DB_TABLE_HANDLE GetTableHandle(void) const { return m_tbl; }
	P2ERR DoKeepHistory(P2DB_CONN_HANDLE hHistDbConn, size_t tableIdx,
        P2DB_SCHEME_HANDLE hHistDbScheme, const INT32* pTransId);

protected:
    enum TRecMode
    {
            rmDelete = 0,
            rmInsert,
            rmUpdate
    };

protected:
	P2ERR CopyRecToHistoryRec(P2DB_RECORD_HANDLE dstRec, P2DB_RECORD_HANDLE srcRec);

protected:
	bool                m_keepHistory;
	//AutoTableHandle     m_hHistTableA;
	AutoRecordHandle    m_hHistRecA;
    P2REPL_ID_TYPE      m_nextLogID;

    size_t              m_histSpecialFieldsCnt; // 3 or 4
    const INT32*        m_pTransId;
    //char                    m_histTblName[P2DB_MAX_TABLE_NAME_SIZE + 1];
private:

	AutoRecordHandle		m_updRec;
	AutoRecordHandle		m_insRec;
	bool					m_isPrepared;
	P2ERR					Prepare();
	P2ERR GetPreparedInsRecord(P2DB_RECORD_HANDLE* pInsRec);
	P2ERR GetPreparedUpdRecord(P2DB_RECORD_HANDLE* pUpdRec);

	bool					m_isPreparedRecsUsing;
	bool					m_isInited;
	size_t					m_tableIdx;
	P2DB_CONN_HANDLE		m_dbConn;
	AutoTableHandle			m_tbl;

	AutoTableSchemeHandle	m_tblSch;
	P2DBFieldInfo			m_fiIndexFieldInfo;
	char m_tblName[P2DB_MAX_TABLE_NAME_SIZE];

private:
	P2ERR Insert(P2DB_RECORD_HANDLE rec);
	// ������ ��������� � update ���������� ��-�� ���������� � ���� ��������� ������
	// ���� ��� ���������� ����� ISAMCache, �� ������ �� �����������, � ����� ���� ����������� 
	// ���� id/rev/act. � ��������� id ��� ���������, �� ��� ����� ������ ��� �� ���������
	P2ERR Update(P2DB_RECORD_HANDLE rec, P2REPL_ID_TYPE recID, bool fastDelete);

	DISABLE_CLASS_COPY_ACCESS(CInsOrUpdHelper, private)
};

////////////////////////////////////////////////////////////////////////////////
// �����, ��������� ������� ��� ���� �����
class CInsOrUpdList
{
public:
	CInsOrUpdList();
	~CInsOrUpdList();

	// ���� hScheme==NULL, ������������ ����� ��
	P2ERR Open(PCSTR streamName, P2DB_CONN_HANDLE dbConn, P2DB_SCHEME_HANDLE hScheme = NULL, bool keepHistory = false);
	
    P2ERR BeginTransaction();
    P2ERR Commit();
    P2ERR Rollback();

	P2ERR SetPreparedRecsUsage(bool bUse);

	P2ERR Close();

	CInsOrUpdHelper& operator[](size_t idx) { return *m_recs[idx]; } //TODO: range

private:
	bool                    m_isInited;
	typedef std::vector<CInsOrUpdHelper *> VIUH;
	VIUH                    m_recs;
	AutoConnectionHandle    m_hHistDbConnA;
    bool                    m_bKeepHistory;
    INT32                   m_transNumber;

private:
	DISABLE_CLASS_COPY_ACCESS(CInsOrUpdList, private)
};

///////////////////////////////////////////////////////////////////////


// ���������������� ������� ������ � ������������� �����, ����� ��� ������� � �������

// �������� ��������� ������������ ���� id,rev
P2ERR P2ReplGetLatestTableIdRevision(P2DB_CONN_HANDLE conn, P2DB_TABLE_SCHEME_HANDLE tblSch, P2REPL_ID_TYPE *pId, P2REPL_REVISION_TYPE *pRev, P2REPL_REVISION_TYPE *pMaxIdRev = NULL);
P2ERR P2ReplGetNextTableRevision(P2DB_CONN_HANDLE conn, P2DB_TABLE_SCHEME_HANDLE tblSch, P2REPL_REVISION_TYPE *pRev);
P2ERR P2ReplGetFirstTableRevision(P2DB_CONN_HANDLE conn, P2DB_TABLE_SCHEME_HANDLE tblSch, P2REPL_REVISION_TYPE *pRev);
P2ERR P2ReplGetFirstNonDeletedTableRevision(P2DB_CONN_HANDLE conn, P2DB_TABLE_SCHEME_HANDLE tblSch, P2REPL_REVISION_TYPE *pRev);

// ������� deleted ������ � ����������� ���������
P2ERR P2ReplClearDeleted(P2DB_CONN_HANDLE conn, P2DB_TABLE_SCHEME_HANDLE tblSch);
// ������� ��� ������� ������� ����������
P2ERR P2ReplClearOldRev(P2DB_CONN_HANDLE conn, P2DB_TABLE_SCHEME_HANDLE tblSch, P2REPL_REVISION_TYPE rev);


// �������� ��� �������� ������. ������� �� ������ � ��������� �������
P2ERR P2ReplInsertOrUpdateRecord(P2DB_CONN_HANDLE conn, P2DB_RECORD_HANDLE rec, P2REPL_REVISION_TYPE *pNewRev, char *primaryTblName);
// ��������� �������� ��� �������� ��������� ������
P2ERR P2ReplInsertOrUpdateRecordEx(P2DB_CONN_HANDLE conn, P2DB_RECORD_HANDLE rec, P2REPL_REVISION_TYPE *pNewRev, char *primaryTblName, bool removeDeleted);


// �������� ��� �������� ������ ����� ����������� ������
P2ERR P2ReplInsertOrUpdatePreparedRecord(P2DB_CONN_HANDLE conn, P2DB_RECORD_HANDLE origRec, P2DB_RECORD_HANDLE insRec, P2DB_RECORD_HANDLE updRec, P2REPL_REVISION_TYPE *pNewRev, char *primaryTblName);
// �������� ��������� ������
P2ERR P2ReplInsertOrUpdatePreparedRecordEx(P2DB_CONN_HANDLE conn, P2DB_RECORD_HANDLE origRec, P2DB_RECORD_HANDLE insRec, P2DB_RECORD_HANDLE updRec, P2REPL_REVISION_TYPE *pNewRev, char *primaryTblName, bool removeDeleted);

// �������� ��� �������� ������. ������� �� ������ � ��������� �������. ���� �������� ������� ������ � ����. 
P2ERR P2ReplInsertOrUpdateRecordEx2(P2DB_CONN_HANDLE conn, P2DB_RECORD_HANDLE rec, P2REPL_REVISION_TYPE *pNewRev, char *primaryTblName, bool removeDeleted, UINT32 flag);

// ��������� ����� �������� ��� ��������������� ������
P2ERR P2ReplInsertOrUpdatePreparedRecordEx2(P2DB_CONN_HANDLE conn, P2DB_RECORD_HANDLE origRec, P2DB_RECORD_HANDLE insRec, P2DB_RECORD_HANDLE updRec, P2REPL_REVISION_TYPE *pNewRev, char *primaryTblName, bool removeDeleted, UINT32 flag);



P2ERR P2ReplCompareFilterWithDbScheme(P2DB_SCHEME_HANDLE dbSch, P2DB_SCHEME_HANDLE filterSch, bool bAllowDefaultFields);

P2ERR P2ReplCheckScheme(P2DB_SCHEME_HANDLE sch, bool checkIndexes);



// ������ ����������� � $\Projects\Units\Plaza-II-Repl\P2ReplClient2\Include\P2ReplClient.h �������� ������������!
// Repl DB scheme table name & scheme name
#define P2REPL_DBSCHEME_TABLE_NAME  "P2ReplDbScheme"
// Repl db scheme with life number, wasServerSchema flag and last revisions array
#define P2REPL_DBSCHEME_TABLE_NAME2 "P2ReplDbSchemeV2"
/*
	������� ��������� �������� ������� ����� ����� �������.
	������ client_to_server_table_num_map ��������� ����� ������� � ���������� ����� � ����� ������� � ��������� �����
	������ �������� client_to_server_field_map �������� ������ ����� � ��������� ����� �� ������ ������� � ���� � ���������� �����
*/
typedef std::vector<size_t>  VI32;
typedef std::vector<VI32>  VVI32;
P2ERR P2ReplMakeTableSubSchemeVector(P2DB_SCHEME_HANDLE sch, P2DB_SCHEME_HANDLE subSch, 
                            VVI32& client_to_server_field_map, VI32& client_to_server_table_num_map, UINT32 subSchItemType);

bool EqualFields(const P2DBFieldInfo& fi1, const P2DBFieldInfo& fi2, bool ignoreName);



//////////////////////////////////////////////////////////////////////////
typedef struct _RecStructCopyExtent {
    UINT32 offsetInFrom;    // ������ �������� � ������ ������-��������� (�������� �� ������ ������)
    UINT32 offsetInTo;      // ������ �������� � ������ ������-��������� (�������� �� ������ ������)
    UINT32 extent_sz;       // ������ �������� ��� ����������� � ������
    UINT32 firstIdxFrom;    // ������ ������� ���� � �������� �� ����� ������-���������(c 0)
    UINT32 firstIdxTo;      // ������ ������� ���� � �������� �� ���-�� (c 0)
    UINT32 fldNum;          // ����� ����� � ��������
} RecStructCopyExtent;
typedef std::vector< RecStructCopyExtent > RecCopyExtentVec;

// ������� ��������� ����� ������ ��� �������� ����������� ����������� �������� ����� memcpy.
// �����-�������� ������ ���� ������������� �����-���������.
// ������� ���� � ����� ������ ������������, � ����������� ����������� ����� ��������� memcpy.
P2ERR CopyRecStruct_Prepare(const P2DB_TABLE_SCHEME_HANDLE hSchRecSrc, const P2DB_TABLE_SCHEME_HANDLE hSchRecDest,
                           const VI32& dest2srcFieldOrder, /*OUT*/ RecCopyExtentVec& rc_extents);

// ����������, ���� ����������� ������ ������ �� ������ ��������������� ���������� �� ����������� ������
// ������� �������� �� �����������.
void CopyRecStruct_Copy(void * pRecDataTo, const void * pRecDataFrom, const RecCopyExtentVec& rc_extents);

}; //namespace nm_P2ReplUtils



// ������������ ���� ���� ������� �� ��������� �������� �������� ��� �������� � ��������� ����� (��� so �� ������������ ��� �������)
using namespace nm_P2ReplUtils;


#endif // P2REPL_UTILS_H_
