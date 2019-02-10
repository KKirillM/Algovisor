#ifndef _P2SYSSCHEME_HPP_
#define _P2SYSSCHEME_HPP_

#include "../../../Plaza-II/P2Utils/Include/AutoHandle.hpp"
#include "P2SysScheme.h"
////////////////////////////////////////////////////////////////////////////////
typedef AutoP2Handle<P2DB_TABLE_SCHEME_HANDLE, P2SchemeFreeTableScheme> AutoTableSchemeHandle;
typedef AutoP2Handle<P2DB_SCHEME_HANDLE, P2SchemeFreeScheme> AutoSchemeHandle;
typedef AutoP2Handle<P2DB_TABLE_INDEX_HANDLE, P2SchemeFreeIndex> AutoIndexHandle;
#endif // _P2DB_H_
