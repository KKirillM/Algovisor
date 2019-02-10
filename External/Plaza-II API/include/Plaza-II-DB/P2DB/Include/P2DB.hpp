#ifndef _P2DB_HPP_
#define _P2DB_HPP_

#include "../../../Plaza-II/P2Utils/Include/AutoHandle.hpp"
#include "P2DB.h"
#include "../../../Plaza-II/P2SysScheme/Include/P2SysScheme.hpp"
#include "../../../Plaza-II/P2SysRecord/Include/P2SysRecord.hpp"

////////////////////////////////////////////////////////////////////////////////
typedef AutoP2Handle<P2DB_CONN_HANDLE, P2DBFreeConnection> AutoConnectionHandle;
typedef AutoP2Handle<P2DB_TABLE_HANDLE, P2DBCloseTable> AutoTableHandle;
typedef AutoP2Handle<P2DB_RECORDSET_HANDLE, P2DBFreeRecordset> AutoRecordsetHandle;
//typedef AutoP2Handle<P2DB_RECORD_HANDLE, P2DBFreeRecord> AutoRecordHandle;
typedef AutoP2Handle<P2DB_WHERE_HANDLE, P2DBFreeWhere> AutoWhereHandle;
typedef AutoP2Handle<P2DB_ORDER_BY_HANDLE, P2DBFreeOrderBy> AutoOrderByHandle;
typedef AutoP2Handle<P2DB_SET_HANDLE, P2DBFreeSet> AutoSetHandle;
typedef AutoP2Handle<P2DB_QUERY_HANDLE, P2DBFreePreparedQuery> AutoQueryHandle;

#endif // _P2DB_H_
