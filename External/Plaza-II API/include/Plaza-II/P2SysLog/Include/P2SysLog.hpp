
#ifndef _P2SYSLOG_HPP_
#define _P2SYSLOG_HPP_

#include "P2SysLog.h"
#include "../../P2Utils/Include/IRc.h"

struct ILogComposer : public IRc
{
	virtual int Compose(char* pLogMsgBuffer, size_t size) = 0;
};


extern "C" {
P2SYSLOG_API  void P2CALLSPEC P2LogCustComposer(P2LOG_HANDLE hLog, UINT32 severity, PCSTR category, ILogComposer* composer); 
};


#endif //_P2SYSLOG_HPP_
