#pragma once

#include "../../../Plaza-II/P2Utils/Include/ServiceDriver.h"
#include "./P2ReplDS.h"

////////////////////////////////////////////////////////////////////////////////

class P2DSHostControl
{
public:
	enum { 
		MAJOR_VERSION = 1,
		MINOR_VERSION = 0 
	};
};
////////////////////////////////////////////////////////////////////////////////
struct P2DSDriver;
typedef ServiceDriver<P2DSDriver, P2DSHostControl> P2DSHost;

struct P2ReplDS;
struct P2DSDriver : public IMQPluginImpl<1, 0, P2DSHostControl>
{
	// соединение к базе
	virtual P2ERR CreateDS(IP2ReplDS** ppDs, P2REPL_DS_CONN_HANDLE conn, P2DSHost* pHost, PCSTR iniSectionName /*NULL*/) = 0;
};


