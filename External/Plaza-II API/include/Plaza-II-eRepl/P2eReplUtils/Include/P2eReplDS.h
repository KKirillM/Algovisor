#ifndef _P2EREPLDS_H_
#define _P2EREPLDS_H_

//////////////////////////////////////////////////////////////////////////
#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II-eRepl/P2eReplCommon/Include/P2eReplTypes.h"
#include "../../../Plaza-II-eRepl/P2eReplCommon/Include/IP2eReplDS.hpp"

typedef class IP2eReplDS* P2EREPL_DS_HANDLE;

P2ERR P2eReplDSDuplicateDSHandle(P2EREPL_DS_HANDLE src, P2EREPL_DS_HANDLE* pDest);
P2ERR P2eReplDSFreeDS(P2EREPL_DS_HANDLE ds);

#endif // _P2EREPLDS_H_
