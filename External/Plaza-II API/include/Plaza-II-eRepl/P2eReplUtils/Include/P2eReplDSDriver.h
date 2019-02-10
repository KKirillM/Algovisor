#pragma once

#include "../../../Plaza-II/P2Utils/Include/ServiceDriver.h"

////////////////////////////////////////////////////////////////////////////////
class P2eDSHostControl
{
public:
	enum { 
		MAJOR_VERSION = 1,
		MINOR_VERSION = 0 
	};
};
////////////////////////////////////////////////////////////////////////////////
struct P2eDSDriver;
typedef ServiceDriver< P2eDSDriver, P2eDSHostControl > P2eDSHost;

class IP2eReplDS;
class IP2eReplStorage;
typedef IP2eReplStorage* P2EREPL_STORAGE_HANDLE;

struct P2eDSDriver : public IMQPluginImpl< 1, 0, P2eDSHostControl >
{
	// соединение к базе
	virtual P2ERR CreateDS(IP2eReplDS** ppDs, P2EREPL_STORAGE_HANDLE conn, P2eDSHost* pHost, PCSTR iniSectionName /*NULL*/) = 0;
};
