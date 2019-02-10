#pragma once

#include "../../P2DB/Include/P2DB.h"
#include "../../../ThirdParty/SQLite3/Source/sqlite3.h"

namespace NS_P2DBSQLITE3 {

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef P2DBSQLITE3_EXPORTS
#define P2DBSQLITE3_API P2_API_EXPORTS
#else
#define P2DBSQLITE3_API P2_API_IMPORTS
#endif



typedef void (P2CALLSPEC *pP2DBSQLITE3_CUSTOM_FUNC)(sqlite3_context *, int, sqlite3_value **);

/**
Allows to add custom SQL functions
* @hDbConn  [req] Database connection to which the SQL function is to be added
* @funcName [req] Name of the SQL function to be created or redefined, length of the name is limited to 255 bytes
* @argCount [req] Number of arguments that the SQL function takes 
  (if argCount == -1, then the SQL function or aggregate may take any number of arguments between 0 and the limit set by sqlite3_limit(SQLITE_LIMIT_FUNCTION_ARG))
* @pFunc    [req] Pointer to C-language function that implement the scalar SQL function

* @returns Error code
*/

P2DBSQLITE3_API P2ERR P2CALLSPEC P2SQLITE3CreateFunction(P2DB_CONN_HANDLE hDbConn, PCSTR funcName,
                                                         INT32 argCount, pP2DBSQLITE3_CUSTOM_FUNC pFunc);


#ifdef  __cplusplus
}
#endif

} //namespace NS_P2DBSQLITE3
