#ifndef _P2DB_DRIVER_H_
#define _P2DB_DRIVER_H_

#include "../../../Plaza-II/P2Utils/Include/IRc.h"
#include "../../../Plaza-II-DB/P2DB/Include/P2DB.h"
#include "../../../Plaza-II/P2Utils/Include/ServiceDriver.h"
#include "../../../Plaza-II/P2SysMsg/Include/P2SysMsg.h"
#include "../../../Plaza-II/P2SysScheme/Include/P2SysScheme.h"
#include "../../../Plaza-II/P2SysRecord/Include/IRecord.h"

#include <string>
#include <vector>


// описание интерфейсов, реализуемых драйверами DB API
// при добавлении функции в интерфейсы нужно изменить реализации в указанных модулях
////////////////////////////////////////////////////////////////////////////////
// стандартный API для получения-присваивания фрагмента сиквельного оператора
struct P2DBClausePart  : virtual public IRc
{
	P2ERR InitSQL(const char *sql){ m_sql = sql; return P2ERR_OK; };
	virtual P2ERR GetSQL(char *sql, size_t *plen) = 0;

	std::string m_sql;
};

class P2DBHostControl
{
public:
	enum { 
		MAJOR_VERSION = 1,
		MINOR_VERSION = 0 
	};
};
////////////////////////////////////////////////////////////////////////////////
struct P2DBDriver;
typedef ServiceDriver<P2DBDriver, P2DBHostControl> P2DBHost;
////////////////////////////////////////////////////////////////////////////////
// собственно драйвер базы
// используется в реализациях баз P2DBSQLITE, P2DBODBC
// версия 4.0 убрала OpenConnection и добавила  CreateConnection из P2DBDriver
// версия 4.0 убрала добавила  Connect, Disconnect и GetHost и убрала Close в P2DBConnection
// версия 5.0 добавила IRc::PrepareForUpdate, PrepareForInsert
// версия 7.0 добавила P2DBConnection::CheckTableExistence
// версия 7.1 добавились ф-ции для работы с хинтами
struct P2DBDriver : public IMQPluginImpl<8, 0, P2DBHostControl>
{
	// соединение к базе
	virtual P2ERR CreateConnection(P2DBConnection**, P2DBHost *) = 0;
};
////////////////////////////////////////////////////////////////////////////////
// текущая запись в рекордсете
// используется в реализациях баз P2DBSQLITE, P2DBODBC для NewRecord/UpdateRecord и в P2ReplClient для отдаваемой пользователю записи
/*struct P2DBRecord  : public IRc
{
	// число полей в записи
	virtual P2ERR GetFieldCount(UINT32 *pCount) = 0;

    // получение полей при наличии схемы
	virtual P2ERR GetField(const char *name, void *outBuf, UINT32 *pSize) = 0;
	virtual P2ERR GetField(int idx, void *outBuf, UINT32 *pSize) = 0;
	// запись поля по имени
	virtual P2ERR SetField(const char *name, const void *inBuf, UINT32 len) = 0;
	virtual P2ERR SetFieldByIndex(int idx, const void *inBuf, UINT32 len) = 0;

	// функции для извлечения данных из записи без схемы
	virtual P2ERR GetField(int idx, void *outBuf, UINT32 *pSize, const char *p2type, UINT32 p2size, UINT32 prec = 0) = 0;

	virtual P2ERR CopyToStruct(void *out_buf, UINT32 *plen) = 0;
	virtual P2ERR CopyFromStruct(const void *out_buf, UINT32 len) = 0;

	virtual P2ERR GetScheme(P2DB_TABLE_SCHEME_HANDLE *ptblSch) = 0;
	//virtual P2ERR GetConnection(P2DB_CONN_HANDLE *pConn) = 0;

	virtual P2ERR InsertRec(void) = 0;
	virtual P2ERR UpdateRec(void) = 0;
	virtual P2ERR DeleteRec(void) = 0;

	virtual P2ERR PrepareForUpdate(void) = 0;
	virtual P2ERR PrepareForInsert(void) = 0;

	virtual P2ERR Clear(void) = 0;
};*/

////////////////////////////////////////////////////////////////////////////////
// рекордсет. также реализует функции доступа к своей текущей записи
// используется в реализациях баз P2DBSQLITE, P2DBODBC
struct P2DBRecordset  : public P2DBRecord
{
	enum RECORDSET_TYPE
	{
		FORWARD,
		BIDIRECT
	};

	virtual enum RECORDSET_TYPE GetType() = 0;
	virtual P2ERR First() = 0;
	virtual P2ERR Next() = 0;
	virtual P2ERR Last() = 0;
    
    virtual P2ERR EnumColumnsInfo(pfnRecordsetEnumColInfoProc cb, void* cbParam) = 0;
};

////////////////////////////////////////////////////////////////////////////////
// используется в реализациях баз P2DBSQLITE, P2DBODBC
struct P2DBConnection : public IRc
{
	virtual P2ERR Connect(P2DB_SCHEME_HANDLE sch, const char *params) = 0;
	virtual P2ERR Disconnect(void) = 0;

	virtual P2ERR GetHost(P2DBHost **) = 0;

	virtual P2ERR BeginTransaction(void) = 0;
	virtual P2ERR Commit(void) = 0;
	virtual P2ERR Rollback(void) = 0;


	virtual P2ERR CreateTable(P2DBTableScheme *) = 0;
	virtual P2ERR DeleteTable(P2DBTableScheme *) = 0;

	virtual P2ERR OpenTable(P2DBTableScheme *, P2DBTable **) = 0;
	virtual P2ERR OpenTable(const char *, P2DBTable **) = 0;

	virtual P2ERR CreateWhere(const char *, P2DBWhere **) = 0;
	virtual P2ERR CreateOrderBy(const char *, P2DBOrderBy **) = 0;
	virtual P2ERR CreateSet(const char *, P2DBSet **) = 0;

	virtual P2ERR JoinTables2Recordset(P2DBTable **, int numTables, P2DBWhere *, P2DBOrderBy* , P2DBRecordset **) = 0;
	virtual P2ERR SQLStatement2Recordset(const char *, P2DBRecordset **, P2DB_TABLE_SCHEME_HANDLE) = 0;
	virtual P2ERR SQLStatement2Enum(const char *, pfnRecordProcWithParam proc, void* cbParam) = 0;
	virtual P2ERR Execute(const char *sql) = 0;


	virtual P2ERR ConvertType(const char *plazaType, UINT32 typeSize, UINT32 precision, char *sqlType, size_t *pSize) = 0;
	virtual P2ERR GetScheme(P2DB_SCHEME_HANDLE *sch) = 0;

	virtual P2ERR CheckTableExistence(const char* tblName, BOOL* exists) = 0;

	virtual P2ERR LoadHintsFromIni(P2INI_HANDLE hIni) = 0;
	virtual P2ERR GetHint(UINT32 hint, PSTR value, size_t* pLen) = 0;
	virtual P2ERR SetHint(UINT32 hint, PCSTR value) = 0;

    virtual P2ERR LoadThresholdsFromIni(P2INI_HANDLE hIni) = 0;
    virtual P2ERR GetThresholdValue(UINT32 thresholdId, UINT64 *pValue) = 0;

    virtual P2ERR CreatePreparedQuery(P2DB_QUERY_HANDLE *pQuery, P2DB_RECORD_HANDLE *pParamRecord,
        const char *sql, P2DB_TABLE_SCHEME_HANDLE paramsSchema) = 0;
};

////////////////////////////////////////////////////////////////////////////////
// используется в реализациях баз P2DBSQLITE, P2DBODBC
struct P2DBTable : public IRc
{
	virtual P2ERR Delete() = 0;
	virtual P2ERR Truncate() = 0;

	virtual P2ERR DeleteRecords(P2DBWhere *) = 0;
	virtual P2ERR UpdateRecords(P2DBSet *, P2DBWhere *) = 0;

	virtual P2ERR GetRecordset(P2DBRecordset **, P2DBWhere * where = NULL, P2DBOrderBy * order = NULL) = 0;

	virtual P2ERR NewRecord(P2DBRecord **) = 0;

	virtual P2ERR GetScheme(P2DB_TABLE_SCHEME_HANDLE *ptblSch) = 0;

	virtual P2ERR GetRecordForUpdate(P2DBRecord **, P2DB_WHERE_HANDLE where) = 0;
};


////////////////////////////////////////////////////////////////////////////////
struct P2DBHints //: virtual public IRc
{
    P2DBHints()
	{
		m_hints.push_back(DEFAULT_HINT);
	};

	P2ERR SetHint(size_t hint, PSTR value, size_t* /*pLen*/)
	{
		if (hint < m_hints.size())
		{
			m_hints[hint] = value;	
			return P2ERR_OK;
		}
		return P2ERR_DB_NO_SUCH_ITEM;
	};

	P2ERR GetHint(size_t hint, PSTR value, size_t* pLen)
	{
		std::string hintVal = DEFAULT_HINT;
		if (hint < m_hints.size())
		{
			hintVal = m_hints.at(hint);
		}

		if (*pLen < (hintVal.length()+1))
		{
			*pLen = hintVal.length()+1;
			return P2ERR_COMMON_BUFFER_TOO_SMALL;
		}
		
		*pLen = hintVal.length()+1;
		strncpy(value, hintVal.c_str(), *pLen);

		return P2ERR_OK;
	};

	std::vector<std::string> m_hints;
};

////////////////////////////////////////////////////////////////////////////////
class P2DBThresholds
{
    public:
        P2DBThresholds()
        {
            m_thresholds.clear();
            m_thresholds.resize(THRESHOLD_MAX_COUNT, THRESHOLD_DEFAULT_VALUE);
        };

        P2ERR SetThresholdValue(size_t thresholdId, UINT64 value)
        {
            if (thresholdId < m_thresholds.size())
            {
                m_thresholds[thresholdId] = value;	
                return P2ERR_OK;
            }
            return P2ERR_DB_NO_SUCH_ITEM;
        };

        P2ERR GetThresholdValue(size_t thresholdId, UINT64 *pValue)
        {
            if (thresholdId >= m_thresholds.size())
            {
                return P2ERR_DB_NO_SUCH_ITEM;
            }
            *pValue = m_thresholds[thresholdId];
            return P2ERR_OK;
        };

    private:
        std::vector< UINT64 > m_thresholds;
};

////////////////////////////////////////////////////////////////////////////////
// Для выполнения запросов к базе используя Prepare()-Bind()
// Используется в реализациях баз P2DBSQLITE, P2DBODBC
struct P2DBPreparedQuery : public IRc
{
    virtual P2ERR Init(P2DB_RECORD_HANDLE *pRecord) = 0;
    virtual P2ERR Prepare() = 0;
    virtual P2ERR ExecuteNonQuery() = 0;
    virtual P2ERR Execute(P2DB_TABLE_SCHEME_HANDLE recordsetScheme, P2DB_RECORDSET_HANDLE *pRecordset) = 0;
};


P2DB_API P2ERR PrvBeginTransaction(P2DB_CONN_HANDLE conn, UINT32 *pTransCount, bool *pWasRollback);
P2DB_API P2ERR PrvCommit(P2DB_CONN_HANDLE conn, UINT32 *pTransCount, bool *pWasRollback);
P2DB_API P2ERR PrvRollback(P2DB_CONN_HANDLE conn, UINT32 *pTransCount, bool *pWasRollback);


P2DB_API P2ERR P2CALLSPEC PrvCreateHintsHandle(P2DB_HINTS_HANDLE* pHints);
P2DB_API P2ERR P2CALLSPEC PrvFreeHintsHandle(P2DB_HINTS_HANDLE hHints);
P2DB_API P2ERR P2CALLSPEC PrvLoadHintsFromIni(P2DB_HINTS_HANDLE hHints, P2INI_HANDLE hIni);
P2DB_API P2ERR P2CALLSPEC PrvGetHint(P2DB_HINTS_HANDLE hHints, UINT32 hintNum, PSTR value, size_t* pLen);
P2DB_API P2ERR P2CALLSPEC PrvSetHint(P2DB_HINTS_HANDLE hHints, UINT32 hintNum, PCSTR value);

P2DB_API P2ERR P2CALLSPEC PrvCreateThresholdsHandle(P2DB_THRESHOLDS_HANDLE* pThresholds);
P2DB_API P2ERR P2CALLSPEC PrvFreeThresholdsHandle(P2DB_THRESHOLDS_HANDLE hThresholds);
P2DB_API P2ERR P2CALLSPEC PrvLoadThresholdsFromIni(P2DB_THRESHOLDS_HANDLE hThresholds, P2INI_HANDLE hIni);
P2DB_API P2ERR P2CALLSPEC PrvGetThresholdValue(P2DB_THRESHOLDS_HANDLE hThresholds, UINT32 thresholdId, UINT64* pValue);

#endif
