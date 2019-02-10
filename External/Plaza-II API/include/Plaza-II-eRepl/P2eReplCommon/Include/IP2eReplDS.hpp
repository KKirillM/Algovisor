#ifndef _IP2EREPLDS_H_
#define _IP2EREPLDS_H_

#include "../../../Plaza-II/P2Utils/Include/IRc.h"
#include "../../../Plaza-II/P2ACL/Include/P2ACL.h"
#include "../../../Plaza-II-eRepl/P2eReplCommon/Include/P2eReplTypes.h"
#include "../../../Plaza-II-eRepl/P2eReplUtils/Include/P2eReplDSDriver.h"

//////////////////////////////////////////////////////////////////////////
#define P2EREPLDS_ENUM_FLAGS_HAS_UPPER 1
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
typedef P2ERR (*P2EREPL_DS_ENUM_FUNC)(const TEReplRec * pEvent, void * cbParam);
//////////////////////////////////////////////////////////////////////////

class IP2eReplDSVersion
{
public:
	enum { 
		MAJOR_VERSION = 2,
		MINOR_VERSION = 0 
	};
};

class IP2eReplDS : public IRc, public IP2eReplDSVersion 
{
public:
	P2ERR CheckVersion(UINT32 reqMajorVersion, UINT32 reqMinorVersion, char reqBuild);

	virtual ~IP2eReplDS() { };
	virtual P2ERR InitServer(void** ppObj) = 0;
	virtual P2ERR DeinitServer(void* pObj) = 0;
	virtual P2ERR GetHost(P2eDSHost**) = 0;
	virtual bool  CheckMyConn(IP2eReplDS* pDS) = 0;

	virtual P2ERR EnumRecords(void* pObj, P2ACL_HANDLE hAcl, PCSTR from, P2EREPL_REVISION_TYPE lowRev, UINT32 flags, P2EREPL_DS_ENUM_FUNC cb, void* cbParam) = 0;

	virtual P2ERR GetFirstRevision(void* pObj, P2EREPL_REVISION_TYPE *pRev) = 0;
    virtual P2ERR GetNextRevision(void* pObj, P2EREPL_REVISION_TYPE *pRev) = 0;
    
    // поддержка онлайн-сервера
	virtual P2ERR IsOnlineServerSupported(void* pObj, bool* pSupported) = 0;
	virtual P2ERR GetFirstRevisionForOnlineServer(void* pObj, P2EREPL_REVISION_TYPE *pRev) = 0;
    virtual P2ERR SetNextRevisionForOnlineServer(void* pObj, P2EREPL_REVISION_TYPE rev) = 0;

    virtual P2ERR FlushOnIdle() = 0;
};

inline P2ERR IP2eReplDS::CheckVersion(UINT32 reqMajorVersion, UINT32 reqMinorVersion, char reqBuild)
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

#endif // _IP2EREPLDS_H_
