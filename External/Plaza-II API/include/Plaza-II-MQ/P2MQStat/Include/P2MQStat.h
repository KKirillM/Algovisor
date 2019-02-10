#ifndef _P2MQSTAT_H_
#define _P2MQSTAT_H_

#include "../../../Plaza-II/P2Common/Include/P2Types.h"

#if defined(__linux__) || defined(__MACH__)
#include <string.h>
#endif // defined(__linux__) || defined(__MACH__)

#if defined(__linux__) || defined(__MACH__)
#include <limits.h>
#include <assert.h>
//!!!#define HANDLE void*

#ifdef __MACH__
#include <dlfcn.h>
typedef struct link_map //!!! fake link_map
{
	const char* l_name;
} link_map;
#define HMODULE2 struct link_map*

#else
#include <link.h>
#define HMODULE2 struct link_map*
#endif // __MACH__

#else
typedef void* HMODULE2;
#endif // defined(__linux__) || defined(__MACH__)

#ifdef P2SYS_EXPORTS
#define P2MQSTAT_API P2_API_EXPORTS
#else
#define P2MQSTAT_API P2_API_IMPORTS
#endif

#define P2MQSTAT_APP_NAME "@MQ_STAT@"
#define P2MSG_CATEGORY_MQSTAT "P2_MQ_STAT"

#include "../../P2MQ/Include/P2MQ.h"

#include "StatType.h"


typedef  struct P2StatVars
{
#if defined(__linux__) || defined(__MACH__)
	 HMODULE2				pModule;
#else
	 void*                 pModule;
#endif
	 const char*				moduleName;
	 P2STAT_MODULE_INDEX		moduleIndex;
	 char						ver[255];
	 char						path[MAX_PATH];

} P2TypeStatVars;

#if defined(__MACH__)
#define P2VARVER(SymbolForSearch, version)						     		\
{																			\
	Dl_info info;															\
	struct link_map HMOD; 													\
	/* Hackerish: works in GLIBC only, but how to get HMOD? */ 			    \
	/*const char anchor[] = "The constant string to identify the module.";*/\
	int err = dladdr((void*)SymbolForSearch, &info/*,		         		\
		reinterpret_cast<void**>(&HMOD), RTLD_DL_LINKMAP*/);		     	\
	if (err == 0)                                                           \
		printf("dladdr returns error: '%s'\n", dlerror());		            \
    memset(&SVars, 0, sizeof(SVars));                                       \
	strncpy(SVars.ver, version, sizeof(SVars.ver));                         \
    SVars.ver[sizeof(SVars.ver) - 1] = 0;                                   \
	assert(err);															\
	HMOD.l_name = info.dli_fname;                                           \
	P2MQStatSetBinaryModuleVersion(/*&HMOD*/(link_map*)info.dli_saddr, &info, \
		&SVars.pModule,	&SVars.moduleName, &SVars.moduleIndex,				\
		SVars.ver, sizeof(SVars.ver), SVars.path, sizeof(SVars.path) );	    \
}
//printf("module %X, name '%s, index %d'\n", &HMOD, info.dli_fname, SVars.moduleIndex);            
#elif defined(__linux__)
#define P2VARVER(SymbolForSearch, version)									\
{																			\
	Dl_info info;															\
	struct link_map * HMOD;													\
	/* Hackerish: works in GLIBC only, but how to get HMOD? */ 				\
	/*static const char anchor[] = "The constant string to identify the module.";*/\
	/*int err = dladdr1(reinterpret_cast<void*>(anchor), &info, */          \
	int err = dladdr1((void*)SymbolForSearch, &info,				        \
		reinterpret_cast<void**>(&HMOD), RTLD_DL_LINKMAP);					\
	if (err == 0)															\
	{																		\
		printf("dladdr1 could not find " #SymbolForSearch "\n");			\
	}		                                        						\
	memset(&SVars, 0, sizeof(SVars));                                       \
	strncpy(SVars.ver, version, sizeof(SVars.ver));                         \
    SVars.ver[sizeof(SVars.ver) - 1] = 0;                                   \
	strncpy(SVars.path, info.dli_fname, sizeof(SVars.path));                \
    SVars.path[sizeof(SVars.path) - 1] = 0;                                 \
	HMOD->l_name = (char*)info.dli_fname;                                   \
	assert(err);															\
	P2MQStatSetBinaryModuleVersion(HMOD, &info,							 	\
		&SVars.pModule,	&SVars.moduleName, &SVars.moduleIndex,				\
		SVars.ver, sizeof(SVars.ver), SVars.path, sizeof(SVars.path) );		\
}
#else //_WIN32

#define P2SETSTATVER() { P2MQStatSetBinaryModuleVersion(GetModuleHandle(NULL), &SVars.pModule, &SVars.moduleName, \
                        &SVars.moduleIndex, SVars.ver, sizeof(SVars.ver), SVars.path, sizeof(SVars.path) ); }

#define P2VARVER(HMOD)	{																	\
					if (HMOD == 0)															\
					{																		\
						HANDLE hModule__;													\
						hModule__ = GetModuleHandle(NULL);									\
						P2MQStatSetBinaryModuleVersion(hModule__,  &SVars.pModule,			\
												&SVars.moduleName, &SVars.moduleIndex,		\
							SVars.ver, sizeof(SVars.ver), SVars.path, sizeof(SVars.path) );	\
					}																		\
					else																	\
					{																		\
						P2MQStatSetBinaryModuleVersion(HMOD, &SVars.pModule,				\
												&SVars.moduleName, &SVars.moduleIndex,		\
							SVars.ver, sizeof(SVars.ver), SVars.path, sizeof(SVars.path) );	\
					}																		\
				}
#endif // __MACH__ __linux__ _else

extern "C" P2ERR P2MQStatStartupInt();

extern "C" void P2MQStatCleanupInt();

extern "C" P2MQSTAT_API  P2ERR P2MQStatUnregisterModule(P2STAT_MODULE_INDEX moduleIndex);

#define P2UNREG_VER() P2MQStatUnregisterModule(SVars.moduleIndex);

#define P2STATVARS static P2StatVars SVars;
		

extern "C" P2MQSTAT_API  P2ERR P2MQStatInternalCreateReply(P2MSG_HANDLE msg, P2MSG_HANDLE* pReply);

extern "C" P2MQSTAT_API  BOOL P2MQStatProcessMsg(P2MQ_CONN_HANDLE conn,P2MSG_HANDLE msg);

extern "C" P2MQSTAT_API  BOOL P2MQStatProcessMsgFromBlock(P2MQ_CONN_HANDLE conn, const UINT8 *pbinmsg, UINT32 binmsglen );

extern "C" P2MQSTAT_API  P2ERR P2MQStatRegisterModule(const void *pModule, const char *moduleName,
		P2STAT_MODULE_INDEX *moduleIndex ) ;


extern "C" P2MQSTAT_API  void P2MQStatUnregisterVersion(P2StatVars * pSVars);

extern "C" P2MQSTAT_API  P2ERR P2MQStatAnnounceStatisticForUserData(P2STAT_MODULE_INDEX moduleIndex,
		const char *statName, P2STAT_DATA_TYPE dataType, void *pUserData,
		size_t dataSize, BOOL synchro, P2StatisticData **pStatData);

extern "C" P2MQSTAT_API  P2ERR P2MQStatAnnounceFunctor(P2STAT_MODULE_INDEX moduleIndex, const char *statName,
		IStatFunctor *pStatFunctor, BOOL synchro, BOOL isCommand, P2StatisticData **pStatData);

extern "C" P2MQSTAT_API  P2ERR P2MQStatLockStatisticData(P2STAT_MODULE_INDEX moduleIndex, P2StatisticData *pStatData);

extern "C" P2MQSTAT_API  P2ERR P2MQStatUnlockStatisticData(P2STAT_MODULE_INDEX moduleIndex, P2StatisticData *pStatData);

extern "C" P2MQSTAT_API  P2ERR P2MQStatRevokeStatisticData(P2STAT_MODULE_INDEX moduleIndex,	P2StatisticData *pStatData);

extern "C" P2MQSTAT_API  P2ERR P2MQStatPurgeStatistics(P2STAT_MODULE_INDEX moduleIndex);

#if defined(__linux__) || defined(__MACH__)
extern "C" P2MQSTAT_API void P2MQStatSetBinaryModuleVersion(HMODULE2 hModule, Dl_info* pInfo,
				/*const*/ HMODULE2* pModule, const char** moduleName,
				P2STAT_MODULE_INDEX* moduleIndex, char* ver, UINT32 lenver, 
												char * path, UINT32 lenpath);
#else 
extern "C" P2MQSTAT_API void P2MQStatSetBinaryModuleVersion(HMODULE2 hModule, 
				/*const*/ HMODULE2* pModule, const char** moduleName,
				P2STAT_MODULE_INDEX* moduleIndex, char* ver, UINT32 lenver, 
												char * path, UINT32 lenpath);
#endif // defined(__linux__) || defined(__MACH__)


struct IModuleServices;
struct IStat;
extern "C" P2MQSTAT_API  bool P2MQStatGetService(IModuleServices* pIModSvcs, IStat **ppInterface);
extern "C" P2MQSTAT_API  void P2MQStatFreeService(void* pInterface);

#endif // _P2MQSTAT_H_
