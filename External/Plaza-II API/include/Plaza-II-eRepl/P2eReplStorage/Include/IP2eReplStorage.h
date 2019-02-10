#pragma once

#include "../../../Plaza-II/P2Utils/Include/IRc.h"
#include "../../P2eReplCommon/Include/P2eReplTypes.h"
#include "../../P2eReplCommon/Include/IP2eReplDS.hpp"


class IP2eReplStorageVersion
{
public:
	enum { 
		MAJOR_VERSION = 2,
		MINOR_VERSION = 0 
	};
};

//typedef P2ERR (P2CALLSPEC *P2EREPL_ENUM_CALLBACK)(size_t iteration_number, const TEReplRec* pRec, void* pParam);

typedef P2ERR (P2CALLSPEC *P2EREPL_ENUM_CALLBACK)(const TEReplRec* pRec, void* pParam);
typedef void (P2CALLSPEC *P2EREPL_ONFLUSH_CALLBACK)(P2EREPL_REVISION_TYPE lastFlushedRev, void * cbParam);

class IP2eReplStorageBase : public IP2eReplStorageVersion
{
public:
    P2ERR CheckVersion(UINT32 reqMajorVersion, UINT32 reqMinorVersion, char reqBuild);

    virtual ~IP2eReplStorageBase() {};
	
    virtual P2ERR Load() = 0;
    virtual P2ERR Truncate() = 0;
	
    virtual P2ERR BeginTransaction() = 0;
    virtual P2ERR Commit() = 0;
    virtual P2ERR Rollback() = 0;
    
    virtual P2ERR Insert(TEReplRec* pRec) = 0;
    virtual P2ERR InsertP(const TEReplRec* pRec) = 0;

	virtual P2ERR EnumRecs(P2EREPL_REVISION_TYPE lowRev, P2EREPL_REVISION_TYPE highRev, P2EREPL_ENUM_CALLBACK cb, void* cbParam) = 0;

    virtual P2ERR ClearOldRev(P2EREPL_REVISION_TYPE maxRev, UINT32 flag) = 0;

    virtual P2ERR GetNextRev(P2EREPL_REVISION_TYPE* pRev) = 0;

    virtual P2ERR GetReplDS(IP2eReplDS** ppDS) = 0;

    virtual P2ERR SetOnFlushCallback(P2EREPL_ONFLUSH_CALLBACK flushCb, void * cbParam) = 0;
};

inline P2ERR IP2eReplStorageBase::CheckVersion(UINT32 reqMajorVersion, UINT32 reqMinorVersion, char reqBuild)
{
	#ifdef _DEBUG
		char build = 'd';
	#else
		char build = 'r';
	#endif
	if (reqBuild != build)
	{
		P2LogError("Build mismatch: we want build '%c',was loaded '%c'", reqBuild, build);
		_ASSERTE(("Вадим, ты опять смешал релизные и дебажные модули?",0));
		return P2ERR_COMMON_WRONG_BUILD;
	}
	if ((reqMajorVersion == MAJOR_VERSION) && (reqMinorVersion <= MINOR_VERSION))
	{
		return P2ERR_OK;
	}
	else
	{
		P2LogError("Version mismatch: we want module v.%02d.%02d,was loaded v.%02d.%02d", MAJOR_VERSION, MINOR_VERSION, reqMajorVersion, reqMinorVersion);
		_ASSERTE(("Вадим, ты опять забыл перекомпилировать _ВСЕ_ модули?",0));
		return P2ERR_COMMON_INVALID_INTERFACE_VERSION;
	}
}

class IP2eReplStorage : public IP2eReplStorageBase, public IP2eReplDS
{
public:
    virtual ~IP2eReplStorage() {};

};