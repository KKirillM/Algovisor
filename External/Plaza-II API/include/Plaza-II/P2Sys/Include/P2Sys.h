#ifndef P2SYS_H_INCLUDED
#define P2SYS_H_INCLUDED


#include "../../P2Common/Include/P2Types.h" 
#include "../../P2SysTypes/Include/P2SysTypes.h"
#include "../../../Plaza-II-MQ/P2MQ/Include/P2MQ.h"
#include "../../P2SysIni/Include/P2SysIni.h"
#include "../../P2SysLog/Include/P2SysLog.h"
#include "../../P2SysMsg/Include/P2SysMsg.h"
#include "../../P2SysRecord/Include/P2SysRecord.h"
#include "../../P2SysScheme/Include/P2SysScheme.h"
#include "../../P2SysParser/Include/P2SysParser.h"
#include "../../P2SysStructParser/Include/P2SysStructParser.h"
#include "../../P2SysRecordParser/Include/P2SysRecordParser.h"
#include "../../../Plaza-II-MQ/P2MQStat/Include/P2MQStat.h"
#include "P2Dll.h"



P2SYS_API P2ERR P2CALLSPEC P2GetVersion(const char ** pVer);


#endif
