#ifndef _ICOMPOSABLE_H_INCLUDED_
#define _ICOMPOSABLE_H_INCLUDED_

#include "../../../Plaza-II/P2SysRecord/Include/P2SysRecord.hpp"
#include "../../../Plaza-II-Repl/P2ReplUtils/Include/P2ReplComm.h"

#define DEF_ICOMPOSABLE_VERSION    1

struct IComposable
{
    virtual P2ERR                   TestVersion(UINT32 reqVersion);
    virtual const char *            GetUser(void) const = 0;
    virtual void                    ComposeRecordOnlineX(P2DB_RECORD_HANDLE rec, size_t tableNoSrv) = 0;
    virtual const char*             GetUserExtraInfo() const = 0; 
    virtual P2REPL_CHANNEL_HANDLE   GetId() const = 0;
    // Получение клиентской схемы. Набрасывает ссылку.
    virtual P2ERR                   GetSchema(P2DB_SCHEME_HANDLE* pSchemaHandle) const = 0;    
    virtual bool                    IsOnline() const = 0;
};

inline P2ERR IComposable::TestVersion(UINT32 reqVersion)
{
    if( DEF_ICOMPOSABLE_VERSION != reqVersion )
        return P2ERR_COMMON_INVALID_INTERFACE_VERSION;

    return P2ERR_OK;
}

#endif // _ICOMPOSABLE_H_INCLUDED_
