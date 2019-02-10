#ifndef _P2TBL_IP2TBL_H_
#define _P2TBL_IP2TBL_H_

#include "../../../Plaza-II/P2Utils/Include/IRc.h"
#include "../../../Plaza-II/P2Utils/Include/ServiceDriver.h"
#include "../../../Plaza-II/P2SysRecord/Include/IRecord.h"
#include "P2Tbl.h"

////////////////////////////////////////////////////////////////////////////////
//

class P2TblHostControl
{
public:
	enum
    { 
		MAJOR_VERSION = 1,
		MINOR_VERSION = 0 
	};
};

struct P2TblDriver;
typedef ServiceDriver<P2TblDriver, P2TblHostControl> P2TblHost;

////////////////////////////////////////////////////////////////////////////////
//

struct P2TblDriver : public IMQPluginImpl<3, 7, P2TblHostControl>
{
	// соединение к базе
	virtual P2ERR CreateConnection(P2TblConnection** ppConn, P2TblHost* pHost) = 0;
};

////////////////////////////////////////////////////////////////////////////////
//
typedef P2ERR (*P2REPL_PROVIDER_ENUM_FUNC)(P2DB_RECORD_HANDLE rec, void* cbParam);

struct P2TblReplProvider
{
public:
	virtual P2ERR SetReplScheme(P2DB_SCHEME_HANDLE replSch) = 0;
	virtual P2ERR GetScheme(P2DB_SCHEME_HANDLE* pSch) = 0;
	virtual bool CheckMyConn(P2TBL_CONN_HANDLE conn) = 0;
	virtual P2ERR EnumRecords(P2DB_TABLE_SCHEME_HANDLE tblSch, P2REPL_REVISION_TYPE lowRev, P2REPL_REVISION_TYPE highRev, bool hasUpper, P2REPL_PROVIDER_ENUM_FUNC cb, void* cbParam) = 0;
		
	virtual P2ERR GetFirstTableRevision(P2DB_TABLE_SCHEME_HANDLE tblSch, const char *primaryTblName, P2REPL_REVISION_TYPE *pRev) = 0;
	virtual P2ERR GetLatestTableIdRevision(P2DB_TABLE_SCHEME_HANDLE tblSch, const char *primaryTblName, P2REPL_ID_TYPE *pId, P2REPL_REVISION_TYPE *pRev) = 0;
	virtual P2ERR GetNextTableRevision(P2DB_TABLE_SCHEME_HANDLE tblSch, const char *primaryTblName, P2REPL_REVISION_TYPE *pRev) = 0;
	virtual P2ERR InsertOrUpdateRecord(void* origRec, P2REPL_REVISION_TYPE *pNewRev, char *primaryTblName, bool removeDeleted, UINT32 flag) = 0;
	virtual P2ERR ClearOldRev(P2DB_TABLE_SCHEME_HANDLE tblSch, P2REPL_REVISION_TYPE rev) = 0;
	virtual P2ERR ClearDeleted(P2DB_TABLE_SCHEME_HANDLE tblSch) = 0;
	virtual P2ERR DeleteRecord(P2DB_TABLE_SCHEME_HANDLE tblSch, const char* primaryTblName,  P2REPL_ID_TYPE id, P2REPL_REVISION_TYPE rev_id) = 0;
	virtual P2ERR BeginTransaction(void) = 0;
	virtual P2ERR Commit(void) = 0;
	virtual P2ERR Rollback(void) = 0;
};

struct P2TblConnection : public IRc
{
	virtual P2ERR Connect(P2DB_SCHEME_HANDLE dbSch, const char* params, const char* options = NULL) = 0;
	virtual P2ERR Disconnect(void) = 0;

	virtual P2ERR GetHost(P2TblHost** ppHost) = 0;

	virtual P2ERR BeginTransaction(void) = 0;
	virtual P2ERR Commit(void) = 0;
	virtual P2ERR Rollback(void) = 0;

//	virtual P2ERR CreateTable(UINT32 tblIdx) = 0;
//	virtual P2ERR CreateTable(P2DB_TABLE_SCHEME_HANDLE tblSch) = 0;
//	virtual P2ERR DeleteTable(UINT32 tblIdx) = 0;
//	virtual P2ERR DeleteTable(P2DB_TABLE_SCHEME_HANDLE tblSch) = 0;

    virtual P2ERR OpenTable(P2TblTable** ppTbl, size_t tblIdx) = 0;
//	virtual P2ERR OpenTable(P2TblTable**, P2DB_TABLE_SCHEME_HANDLE) = 0;
//	virtual P2ERR OpenTable(P2TblTable**, const char*) = 0;
	
	virtual P2ERR GetScheme(P2DB_SCHEME_HANDLE* pDbSch) = 0;

    // «агрузить всю базу по схеме
    virtual P2ERR Load() = 0;
    virtual P2ERR Save() = 0;

	virtual P2ERR GetDBConn(P2DB_CONN_HANDLE* pDBConn) = 0;
	virtual P2ERR FlushChanges() = 0;
	
	virtual P2ERR SetOptions(UINT32 opt) = 0;
	virtual P2ERR GetOptions(UINT32* opt) = 0;

	virtual P2ERR GetReplDS(P2REPL_DS_HANDLE* ppDS ) = 0;

	virtual P2ERR CreateDatabase(PCSTR tblConnStr, P2DB_SCHEME_HANDLE hScheme, UINT32 opt) = 0;
	virtual P2ERR DeleteDatabase(PCSTR tblConnStr, P2DB_SCHEME_HANDLE hScheme) = 0;

	virtual P2ERR CreateSchemeFromDatabase(P2DB_SCHEME_HANDLE *pDestScheme, PCSTR tblConnStr, PCSTR srvcTableName, PCSTR streamName) = 0;
	virtual P2ERR SaveSchemeToDatabase(P2DB_SCHEME_HANDLE hSch, PCSTR tblConnStr, PCSTR srvcTableName, PCSTR streamName) = 0;
	virtual P2ERR DeleteSchemeFromDatabase(PCSTR tblConnStr, PCSTR srvcTableName, PCSTR streamName) = 0; 
	virtual P2ERR Commit2(const char* pEventName) = 0;
};

////////////////////////////////////////////////////////////////////////////////
//

/*struct P2TblRecordset : public IRc
{
    virtual P2ERR GetNextRec(void* pRec) = 0;
};*/

////////////////////////////////////////////////////////////////////////////////
//

struct P2TblTable : public IRc
{
//	virtual P2ERR Delete() = 0;
	virtual P2ERR Truncate() = 0;

	virtual P2ERR GetScheme(P2DB_TABLE_SCHEME_HANDLE* pTblSch) = 0;
	
    // ѕолучить запись, использу€ дл€ поиска пол€ из записи.
    // »ндекс должен быть уникальным.
    //COREERR tbl_get_rec( TABLEHANDLE th, int index_id, void* rec, BOOL* found );
        // ¬место found==FALSE возвращаетс€ код ошибки P2ERR_DB_EOF.
    virtual P2ERR GetRec(void* pRec, size_t indexId) = 0;

    // ѕеребирает записи в таблице, вызыва€ дл€ каждой 
    // callback(iteration_number, rec, callback_param).
    // iteration_number возрастает с 0.
    // rec_min  устанавливает ограничение >=, NULL - ограничение отсутствует
    // rec_max  устанавливает ограничение <=, NULL - ограничение отсутствует
    // rec_max может быть равен rec_min (устанавливает ограничение =)
    //COREERR tbl_enum_recs( TABLEHANDLE th, int index_id, 
    //                                const void* rec_min, const void* rec_max,
    //                                UINT32 flags,
    //                                TBL_ENUM_CALLBACK callback, void* callback_param );
        // «аполн€ет pRec и возвращает ppRS дл€ использовани€ с GetNextRec().
        // ѕосле использовани€ ссылку на ppRS нужно освободить, вызвав Release1().
        // ≈сли записей не найдено, возвращает P2ERR_DB_EOF.
//    virtual P2ERR GetFirstRec(void* pRec, P2TblRecordset** ppRS, UINT32 indexId, UINT32 flags,
//                              const void* pRecMin = 0, const void* pRecMax = 0) = 0;
    virtual P2ERR GetRecsCount(size_t* pRecsCnt) = 0;

	virtual P2ERR EnumRecs(size_t indexId, UINT32 flags, P2TBL_ENUM_CALLBACK cb, void* cb_param,
                           const void* pRecMin = 0, const void* pRecMax = 0) = 0;

    //COREERR tbl_get_min( TABLEHANDLE th, int index_id, 
    //                            const void* rec_min, const void* rec_max,
    //                            void* rec, BOOL* found );
        // ≈сли записей не найдено, возвращает P2ERR_DB_EOF.
    virtual P2ERR GetRecMin(void* pRec, size_t indexId,
                            const void* pRecMin = 0, const void* pRecMax = 0) = 0;
    virtual P2ERR GetRecMax(void* pRec, size_t indexId,
                            const void* pRecMin = 0, const void* pRecMax = 0) = 0;

    // ƒобавл€ет запись в таблицу. 
    //COREERR tbl_add_rec( TABLEHANDLE th, const void* rec );
    virtual P2ERR InsertRecord(void* pRec) = 0;

    // ќбновл€ет запись в таблице. 
    // ƒл€ поиска используютс€ пол€ из индекса index_id.
    // »ндекс должен быть уникальным.
    //COREERR tbl_update_rec( TABLEHANDLE th, int index_id, const void* rec );
    virtual P2ERR UpdateRecord(size_t indexId, void* pRec) = 0;

    // ”дал€ет запись в таблице.
    // ƒл€ поиска используютс€ пол€ из индекса index_id.
    // »ндекс должен быть уникальным.
    //COREERR tbl_delete_rec( TABLEHANDLE th, int index_id, const void* rec );
    virtual P2ERR DeleteRecord(size_t indexId, void* pRec) = 0;

    // ¬ернуть размер структуры записи
    virtual P2ERR GetRecSize(size_t* pSize) = 0;

    virtual P2ERR Load() = 0;
    virtual P2ERR Save() = 0;

	virtual P2ERR PostPrepared(const void* pRec) = 0;

	virtual P2ERR GetTableInfo(size_t* pTblIdx, P2TBL_CONN_HANDLE* pConn) = 0;
	virtual P2ERR ClearOldRev(P2REPL_REVISION_TYPE rev) = 0;
	
	virtual P2ERR ClearDeleted() = 0;

	virtual P2ERR GetLatestTableIdRevision(P2REPL_ID_TYPE *pId, P2REPL_REVISION_TYPE *pRev) = 0;

	virtual P2ERR PostPreparedEx(const void* pRec, UINT32 flags) = 0;
	virtual P2ERR ClearOldRev2(P2REPL_REVISION_TYPE rev, UINT32 flags) = 0;

	virtual P2ERR DeleteRecordsFromBegin(size_t /*RevsCnt*/)
	{
		return P2ERR_COMMON_NOT_IMPLEMENTED;
	}
};

////////////////////////////////////////////////////////////////////////////////

//P2ERR P2TblPrvBeginTransaction(P2TblConnection* pConn, UINT32* pTransCount, bool* pWasRollback);
//P2ERR P2TblPrvCommit(P2TblConnection* pConn, UINT32* pTransCount, bool* pWasRollback);
//P2ERR P2TblPrvRollback(P2TblConnection* pConn, UINT32* pTransCount, bool* pWasRollback);

#endif // _P2TBL_IP2TBL_H_