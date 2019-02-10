#ifndef _P2DBODBC_H_
#define _P2DBODBC_H_

#include "../../P2DB/Include/P2DB.h"
#include <sql.h>
#include <sqlext.h>

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef P2DBODBC_EXPORTS
#define P2DBODBC_API __declspec(dllexport)
#else
#define P2DBODBC_API __declspec(dllimport)
#endif

// See ODBC SQLTables()
P2DBODBC_API P2ERR P2CALLSPEC P2ODBCSchemaTables(P2DB_CONN_HANDLE conn, P2DB_RECORDSET_HANDLE* pRs,
                                                 PCSTR pCatalogName, PCSTR pSchemaName, PCSTR pTableName, PCSTR pTableType);

// See ODBC SQLColumns()
P2DBODBC_API P2ERR P2CALLSPEC P2ODBCSchemaColumns(P2DB_CONN_HANDLE conn, P2DB_RECORDSET_HANDLE* pRs,
                                                  PCSTR pCatalogName, PCSTR pSchemaName, PCSTR pTableName, PCSTR pColumnName);

// See ODBC SQLStatistics()
P2DBODBC_API P2ERR P2CALLSPEC P2ODBCSchemaStatistics(P2DB_CONN_HANDLE conn, P2DB_RECORDSET_HANDLE* pRs,
                                                     PCSTR pCatalogName, PCSTR pSchemaName, PCSTR pTableName,
                                                     SQLUSMALLINT unique, SQLUSMALLINT accuracy);

// See ODBC SQLGetTypeInfo()
P2DBODBC_API P2ERR P2CALLSPEC P2ODBCGetTypeInfo(P2DB_CONN_HANDLE conn, P2DB_RECORDSET_HANDLE* pRs, SQLINTEGER sqlType);

P2DBODBC_API P2ERR P2CALLSPEC P2ODBCSqlTypeToP2Type(P2DB_CONN_HANDLE conn,
                                                    SQLINTEGER sqlType, SQLINTEGER sqlSize1, SQLINTEGER sqlSize2,
                                                    char* pP2Type, size_t* pP2TypeBufSize, UINT32* pP2Size1, UINT32* pP2Size2);

P2DBODBC_API SQLHENV P2CALLSPEC P2ODBCGetHENV();

P2DBODBC_API P2ERR P2ODBCGetSqlErrDescr(SQLSMALLINT handleType, SQLHANDLE handle, const P2DB_CONN_HANDLE conn, P2ERR* pErrFromNative,
                                        /*OUT*/ char * pBuffer, size_t bufferSz);


#ifdef  __cplusplus
}
#endif

#endif // _P2DBODBC_H_
