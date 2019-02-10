#ifndef _P2MQ_TIMESYNC_H_
#define _P2MQ_TIMESYNC_H_

#ifdef P2MQTIMESYNC_EXPORTS
#define P2MQTIMESYNC_API //__declspec(dllexport)
#else
#define P2MQTIMESYNC_API //__declspec(dllimport)
#endif

#include <vector>
#include "../../P2MQRouter/Include/Entrance.h"
#include "../../P2MQRouter/Include/IModuleServices.h"
#include "../../P2MQRouter/Include/P2LLMsgRanges.h"
#include "../../../Plaza-II/P2Utils/Include/IRc.h"
#include "../../P2MQRouter/Include/LLMsg.h"
#include "../../P2MQRouter/Include/IMsgConnection.h"
#include "../../P2MQRouter/Include/IBytesProtPtrList.h"
#include "../../../Plaza-II/P2Utils/Include/ServiceDriver.h"
#include "../../P2MQRouter/Include/IRouter.h"
#include "../../../Plaza-II/P2Utils/Include/WeakMsgID.h"
#include "../../P2MQRouter/Include/IScheduler.h"
#include "../../P2MQRouter/Include/ILLMsgTypeProc.h"
#include "../../../Plaza-II/P2SysScheme/Include/P2SysScheme.hpp"
//#include "../../../Plaza-II/P2Utils/Include/stuffdll.h"
#include "../../../Plaza-II/P2SysMsg/Include/P2SysMsg.hpp"
#include "../../../Plaza-II/P2SysMsg/Include/P2SysMsgTypes.h"
#include "../../../Plaza-II/P2SysTypes/Include/P2SysTypes.h"
#include "../../../Plaza-II/P2SysTypes/Include/P2SysTypes.hpp"

#include "../../P2MQStat/Include/IStat.h"
#include "../../../Plaza-II/P2Utils/Include/SmartIRc.h"

using  std::vector;

struct ITimeSyncService 
{
	virtual P2ERR Init() = 0;
	virtual ~ITimeSyncService() {}

};

namespace NS_RouterTimeSync {

class ScheduledTimeSyncEntry;

typedef struct
{
	ScheduledTimeSyncEntry *sse;
} Vectors;

class TimeSyncService : public ITimeSyncService, public ILLMsgTypeProc
{
public:
	TimeSyncService(IModuleServices *);
	P2ERR Process(PCTLLMsgHdr pHdr, const void* pBody, IMsgConnection* pIMsgConn);
	P2ERR Init();
	~TimeSyncService();
private:
	ScheduledTimeSyncEntry *m_pSchedEntry;
	CRcPtr<ISchJob>  m_pJob;
	IModuleServices *m_pISvcs;
	IRouter *m_pIRouter;

	/* stat */
	INT32				m_timeDiffMcsWithRoot;
	bool				m_bStatModuleRegistered;
	P2STAT_MODULE_INDEX m_statIdx;
};

class ScheduledTimeSyncEntry : public IIocpTask
{
public:
	IRC_IMPLEMENT()
//	IRC_IMPLEMENT_DBG__("ScheduledSplitEntry")
	// интерфейс шедулера
	void Process(ICs*);
	const char *GetDescr(void)const{ return "ScheduledTimeSyncEntry"; }
	ScheduledTimeSyncEntry(CCS &,  IModuleServices *);
	~ScheduledTimeSyncEntry();
private:
	IRouter *m_pIRouter;
	ITimeSyncService *m_lpTimeSyncSvc;
};

} // namespace NS_RouterTimeSync 

bool RouterTimeSync_GetService(IModuleServices *pServices, ITimeSyncService **pInterface); 
void RouterTimeSync_FreeService(ITimeSyncService * pTimeSyncService);



#endif // _P2MQ_TIMESYNC_H_
