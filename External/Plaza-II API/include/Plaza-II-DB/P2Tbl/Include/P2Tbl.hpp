#ifndef _P2TBL_HPP_
#define _P2TBL_HPP_

#include "../../../Plaza-II/P2Utils/Include/AutoHandle.hpp"
#include "P2Tbl.h"

typedef AutoP2Handle<P2TBL_CONN_HANDLE, P2TBLFreeConnection> P2AutoTblConnectionHandle;
typedef AutoP2Handle<P2TBL_TABLE_HANDLE, P2TBLCloseTable> P2AutoTblTableHandle;
//typedef AutoP2Handle<P2TBL_RECORDSET, P2TBLCloseRecordset> P2AutoTblRecordsetHandle;

#endif // _P2TBL_HPP_
