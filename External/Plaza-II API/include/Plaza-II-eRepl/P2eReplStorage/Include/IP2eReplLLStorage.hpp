#ifndef _IP2EREPLLLSTORAGE_H__INCLUDED_
#define _IP2EREPLLLSTORAGE_H__INCLUDED_

#include "../../P2eReplCommon/Include/P2eReplTypes.h"

typedef P2ERR (P2CALLSPEC *P2EREPL_ENUM_SOLID_STORAGE_CALLBACK)(INT64 offset, const TEReplRec* pRec, void* pParam);
typedef P2ERR (P2CALLSPEC *P2EREPL_COMMIT_SOLID_STORAGE_CALLBACK)(void* pParam, P2EREPL_REVISION_TYPE minRev, P2EREPL_REVISION_TYPE maxRev);

class IP2eReplLLStorage
{
public:
    virtual ~IP2eReplLLStorage() { };
    virtual P2ERR Load(P2EREPL_ENUM_SOLID_STORAGE_CALLBACK cbRec, P2EREPL_COMMIT_SOLID_STORAGE_CALLBACK cbCommit, void* cbParam) = 0;
};

#endif // _IP2EREPLLLSTORAGE_H__INCLUDED_
