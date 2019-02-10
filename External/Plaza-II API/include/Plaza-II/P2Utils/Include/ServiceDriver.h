#ifndef _SERVICE_DRIVER_H_
#define _SERVICE_DRIVER_H_

#if defined(__linux__) || defined(__MACH__)
#include <dlfcn.h>
#include <limits.h>
#include <string.h>
#else
#if defined(_MSC_VER)
	#include <crtdbg.h>
#endif // defined(_MSC_VER)
#endif // defined(__linux__) || defined(__MACH__)


#include <map>
#include <string>
#include "../../../Plaza-II/P2SysLog/Include/P2SysLog.h"
#include "../../../Plaza-II/P2Sys/Include/P2Dll.hpp"

// ������� �������� ��� ������������ �����. ��� ���-�� ����� ���������� � ������ ������
//_______________________________________________________________________________________
#ifndef _P2_BUILD_LIB						
	#ifdef  _DEBUG							
		#define LIB_SUFFIX "D.dll"
	#else
		#define LIB_SUFFIX ".dll"
		#endif
	#define P2_SET_MODULE_NAME(str,prefix)							\
	const char* str##_ModName = prefix#str LIB_SUFFIX;
#define EXT_C extern "C"
#else
#define EXT_C 

#if defined(__linux__) || defined(__MACH__)
typedef int (*DUMMY_FUNC)(void);
#define P2_SET_MODULE_NAME(str,prefix)			   \
	const char* str##_ModName = ":"#str":";		   
	//extern "C" int __cdecl str##_dummyFunc(void);  	   
    //static DUMMY_FUNC ptr##str = &str##_dummyFunc; 

#else	
#define P2_SET_MODULE_NAME(str,prefix)							\
	const char* str##_ModName = ":"#str":";			\
	extern "C" int __cdecl str##_dummyFunc(void);		\
	static void *volatile ptr##str = &str##_dummyFunc;			
#endif // defined(__linux__) || defined(__MACH__)
#endif	
//_______________________________________________________________________________________



struct IModuleServices;

/* 
	������� ������������ ������:

	�������� ������ (CoreInterface) -- ��� ��������� ��� ����������, ������� ��������� ������ (IMQPluginImpl). 
	� ��������� ������ � � ������� ��� ���������� ������� ������ ������� � ��������� ������.

	��� �������� ������� (�-�� ServiceDriver::GetInterface) ����������� ������ ServiceDriver::CoreInterfac� (��� ������ ��������� ������, ������� ������ ������) � 
	IMQPluginImpl::CoreInterface (��� ������ ��������� ������ � ������� ������������� ������). ��� �������� � �-��
	GetVersion. A ����� ����������� ��������� ������ ������� (��, � ������� ������������� ���. ������) � ������� ���������� �� �������.

	������� CheckInterfaceVersion ���������� ��������� � ������������ ������.  
	��������� IMQPlugin - ������� ��������� ��������.  
	IMQPluginImpl<major, minor> - �������� � ��������� �������������� GetVersion. ��� �������������
	���������� ������������ ��������� ������� �� IMQPluginImpl. �������� ���������������� ������� ������.

	�������� ServiceDriver ���������� ������ ������� � ���������.  
*/

inline P2ERR CheckInterfaceVersion(UINT32 reqMajor, UINT32 reqMinor, UINT32 offeredMajor, UINT32 offerredMinor)
{
	return ((reqMajor == offeredMajor) && (reqMinor <= offerredMinor)) ? P2ERR_OK : P2ERR_COMMON_INVALID_INTERFACE_VERSION;
}

struct IMQPlugin
{
	virtual P2ERR GetVersion(UINT32 coreMajorVersion, UINT32 coreMinorVersion, UINT32 *majorVersion, UINT32 *minorVersion, char coreBuild, char *build) = 0;
};

template <UINT32 major, UINT32 minor, class HostInterface = IModuleServices>
class IMQPluginImpl : public virtual IMQPlugin
{
public:

	enum { 
		MAJOR_VERSION = major,
		MINOR_VERSION = minor 
	};

	P2ERR GetVersion(UINT32 coreMajorVersion, UINT32 coreMinorVersion,	
			UINT32 *majorVersion, UINT32 *minorVersion, char coreBuild, char *build){
		*majorVersion = MAJOR_VERSION;
		*minorVersion = MINOR_VERSION;
		P2ERR err = CheckInterfaceVersion(HostInterface::MAJOR_VERSION, HostInterface::MINOR_VERSION, coreMajorVersion, coreMinorVersion);
#ifdef _DEBUG
		*build = 'd';
#else
		*build = 'r';
#endif
		if (err != P2ERR_OK)
		{
			P2LogError("Version mismatch: we want core v.%02d.%02d, was loaded v.%02d.%02d", HostInterface::MAJOR_VERSION, HostInterface::MINOR_VERSION, coreMajorVersion, coreMinorVersion);
		}
		else
		{
			if (coreBuild != *build)
			{
				err = P2ERR_COMMON_WRONG_BUILD;
				P2LogError("Build mismatch: we want core build '%c', was loaded '%c'", *build, coreBuild);
			}
		}
		return err;
	}
};


// ������ ��� ������� ����������� � DLL

/*
	��� ����� �� DLL?

	������� ���� ������� 

	bool GetService(IModuleServices *pServices, Interface **pInterface);
		GetService �������� ������ �� ������� � ���������� ��������� ����������� �������.
		��������� ������ ������������� �� IMQPlugin
		������������ true, ���� ��� ��, ����� false.
		���������� �� �������� ��������� ������!

	void FreeService(Interface * pService);
		�������� ��������� ������� � ����������� ���.

*/
/*
	��� ������������?

	ServiceDriver<IMyService> svc;
	if (svc.Load("aaa.dll"))
	{
		// ��
		IMyService *psvc = svc.GetService();
	}
	else
	{
		// ������
	}

	��� 

	ServiceDriver<IMyService> svc("aaa.dll");
	� ���� ������ ������ ��������� ������, �� ������ �� ��������� ����� ��������

	��������� ������� IncrementUnloadCounter/DecrementUnloadCounter. ������� ���������� ��� �������������� ������������� ��������

	��������� ����������� ��������� ������� �� ������
	����������� ������ ������ ���� �������������� ��������������� � map'� gServiceMap ��� ������ ������� RegisterInMemoryService( 

*/

// ������� ������ ������ �� ������
typedef  bool (P2CALLSPEC *pfnGetService)(IModuleServices *pServices, void **pInterface);
// ������� ������������ �������
typedef void (P2CALLSPEC *pfnFreeService)(void * pService);

typedef struct
{
	pfnGetService     get;
	pfnFreeService    free;
} TServiceFunctions;

typedef std::map<std::string, TServiceFunctions> TServiceMap;



extern inline TServiceMap& GetServiceMap()
{
	static TServiceMap gServiceMap;
	return gServiceMap;
}

inline void RegisterInMemoryService(PCSTR moduleName, TServiceFunctions& servFuncs)
{
	GetServiceMap().insert(TServiceMap::value_type(moduleName, servFuncs));
}

class ServiceAutoRegistrator
{
  public:
#if defined(__linux__) || defined(__MACH__)
    ServiceAutoRegistrator(PCSTR moduleName, pfnGetService getFunc, pfnFreeService freeFunc)
#else
    ServiceAutoRegistrator(PCSTR moduleName, void* getFunc, void* freeFunc)
#endif // defined(__linux__) || defined(__MACH__)
    {
      TServiceFunctions func_pair;

      func_pair.get = static_cast<pfnGetService>(getFunc);
      func_pair.free = static_cast<pfnFreeService>(freeFunc);

      RegisterInMemoryService(moduleName, func_pair);
    }
};

template <class Interface, class CoreInterface = IModuleServices> 
class ServiceDriver
{
public:
	
	// ������� ������ ������ �� ������
	typedef  bool (P2CALLSPEC *pfnGetService)(IModuleServices *pServices, Interface **pInterface);
	// ������� ������������ �������
	typedef void (P2CALLSPEC *pfnFreeService)(Interface * pService);

	ServiceDriver();
	ServiceDriver(const char *dllName);
	~ServiceDriver();

	bool Load(const char *dllName);
    void Free();
	Interface *GetService(IModuleServices *pServices);

    P2ERR GetLastErr() const { return m_lastError; }

	LONG IncrementUnloadCounter(void)
	{
		return InterlockedIncrement(&m_delayedCount);
	}
	LONG DecrementUnloadCounter(void)
	{
		return InterlockedDecrement(&m_delayedCount);
	}

private:
	// ����� ����
	P2DLL_HANDLE m_dll;
	// ������� ������ ������ �� ������
	pfnGetService m_get;
	// ������� ������������ �������
	pfnFreeService m_free;

	// ���������� ������
	Interface * m_service;

	// ������� ���������� ��������
	LONG m_delayedCount;

	//true - ���� ������ �������� �� ������, � �� �� dll
	bool m_inMemory;

    P2ERR m_lastError;
};

//
template <class Interface, class CoreInterface>
ServiceDriver<Interface, CoreInterface>::ServiceDriver() :
    m_dll(NULL), m_get(NULL), m_free(NULL), m_service(NULL),
    m_delayedCount(1), m_inMemory(false), m_lastError(P2ERR_OK)
{
}

template <class Interface, class CoreInterface>
ServiceDriver<Interface, CoreInterface>::ServiceDriver(const char *dllName) :
    m_dll(NULL), m_get(NULL), m_free(NULL), m_service(NULL),
    m_delayedCount(1), m_inMemory(false), m_lastError(P2ERR_OK)
{
    Load(dllName);
}

template <class Interface, class CoreInterface>
ServiceDriver<Interface, CoreInterface>::~ServiceDriver()
{
	Free();
}

template <class Interface, class CoreInterface>
bool ServiceDriver<Interface, CoreInterface>::Load(const char *dllName)
{
	Free();
	if (dllName[0] != ':')
	{
        m_lastError = P2ERR_COMMON_SYSCALL_FAILED;
		m_inMemory = false;
		m_dll = P2DLLoadLibrary(dllName, DEFAULT_LOAD_FLAG );
		if (m_dll == NULL)
		{
			char errstr[64];
			P2SystemLogEx(P2LOG_SEVERITY_WARNING, "servicedriver", TRUE, "Couldn't load library '%s' : '%s'", dllName, P2DLLGetLastError(errstr, sizeof(errstr)));
            m_lastError = P2ERR_COMMON_SYSCALL_FAILED;
		}

		if (m_dll != NULL)
		{
			char errstr[64];
			m_get = reinterpret_cast<pfnGetService>(P2DLLGetProcAddress(m_dll, "GetService"));
			if (m_get == NULL)
			{
				P2SystemLogEx(P2LOG_SEVERITY_WARNING, "servicedriver", TRUE, "Couldn't get 'GetService' address in dynamic library '%s' : '%s'", dllName, P2DLLGetLastError(errstr, sizeof(errstr)));
				return false;
			}
			m_free = reinterpret_cast<pfnFreeService>(P2DLLGetProcAddress(m_dll, "FreeService"));
			if (m_free == NULL)
			{
				P2SystemLogEx(P2LOG_SEVERITY_WARNING, "servicedriver", TRUE, "Couldn't get 'FreeService' address in dynamic library '%s': %s", dllName, P2DLLGetLastError(errstr, sizeof(errstr)));
				return false;
			}
            m_lastError = P2ERR_OK;
		}
		return m_dll != NULL;
	}
	else
	{
        m_lastError = P2ERR_COMMON_WRONG_ARGUMENTS;
		m_inMemory = true;
		PCSTR pLastSep = strrchr(dllName, ':');
		// ��������� �� 0 �� ����, �.�. ':' � ������ ����� ���� (����� �� ������ �� � else)

		ptrdiff_t len = pLastSep - dllName - 1;
		if ((len <= 0) || (len >= MAX_PATH))
		{
			return false;
		}
		else
		{
			char moduleName[MAX_PATH];			
			memcpy(moduleName, dllName+1, len);
			moduleName[len] = 0;

			TServiceMap::const_iterator it = (GetServiceMap()).find(moduleName);
			if (it == GetServiceMap().end())
			{
				return false;
			}

			m_get = reinterpret_cast<pfnGetService>((it->second).get);
			m_free = reinterpret_cast<pfnFreeService>((it->second).free);

            m_lastError = P2ERR_OK;
			return true;
		}
	}
}

template <class Interface, class CoreInterface>
void ServiceDriver<Interface, CoreInterface>::Free()
{
	if (((m_inMemory) || (m_dll != NULL)) && (DecrementUnloadCounter() == 0))
	{
		if (m_service != NULL) m_free(m_service);
		if (m_dll != NULL)
		{
			P2DLLFreeLibrary(m_dll);
		}
	}
    m_dll = NULL;
	m_service = NULL;
	m_get = NULL;
	m_free = NULL;

	m_inMemory = false;
}

template <class Interface, class CoreInterface>
Interface * ServiceDriver<Interface, CoreInterface>::GetService(IModuleServices *pServices)
{
	if ((m_dll == NULL) && (!m_inMemory)) return NULL;
	if (m_get == NULL) return NULL;
	bool res = true;
	if (m_service == NULL)
	{
		res = m_get(pServices, &m_service);
		if (res)
		{
			UINT32 maj, min;
#ifdef _DEBUG
			char build = 'd';
#else
			char build = 'r';
#endif
			char moduleBuild;
			m_lastError = m_service->GetVersion(CoreInterface::MAJOR_VERSION, CoreInterface::MINOR_VERSION, &maj, &min, build, &moduleBuild);
			switch(m_lastError)
			{
			case P2ERR_OK:
				break;

			case P2ERR_COMMON_INVALID_INTERFACE_VERSION: 
			case P2ERR_COMMON_WRONG_BUILD:
#if !(defined(__linux__) || defined(__MACH__)) 
				_ASSERTE(("�����, �� ����� ����� ����������������� _���_ ������?",0));
#endif 
			Free();
			return NULL;

			default:
				P2LogErrorWithCode(m_lastError, "Unknown load error");
				Free();
				return NULL;
			}

			m_lastError = CheckInterfaceVersion(Interface::MAJOR_VERSION, Interface::MINOR_VERSION, maj, min);
			switch(m_lastError)
			{
			case P2ERR_OK:
				break;

			case P2ERR_COMMON_INVALID_INTERFACE_VERSION:
#if !(defined(__linux__) || defined(__MACH__))
				_ASSERTE(("�����, �� ����� ����� ����������������� _���_ ������?",0));
#endif
				P2LogError("Version mismatch: we want module v.%02d.%02d, was loaded v.%02d.%02d", Interface::MAJOR_VERSION, Interface::MINOR_VERSION, maj, min);
				Free();
				return NULL;

			case P2ERR_COMMON_WRONG_BUILD:
				P2LogError("Build mismatch: we want core build '%c', was loaded '%c'", build, moduleBuild);
#if !(defined(__linux__) || defined(__MACH__))
				_ASSERTE(("�����, �� ����� ������ �������� � �������� ������?",0));
#endif
				Free();
				return NULL;

			default:
				P2LogErrorWithCode(m_lastError, "Unknown load error");
				Free();
				return NULL;
			}
		}
	}
	if (res)
	{
		return m_service;
	}
	else
	{
		return NULL;
	}
}
#endif // _SERVICE_DRIVER_H_
