#ifndef _P2PROCESS_HPP_
#define _P2PROCESS_HPP_

#include "P2Event.hpp"
#include "P2ProcessEnvironment.hpp"
#include "P2SharedMem.hpp"
#include "../../../Plaza-II/P2Sys/Include/P2Sys.h"

#ifdef _WIN32
#include <vector>
#elif defined(__linux__) || defined (__MACH__)
#include <sys/wait.h>
#include <signal.h>
#endif // _WIN32

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996) // warning C4996: This function or variable may be unsafe.
#endif // _MSC_VER

namespace p2process_impl {
	struct Process;
};

typedef p2process_impl::Process* P2PROCESS_HANDLE;

namespace p2process_impl {

// при создании процесса-потомка создаётся разделяемая память, которая наследуется процесcом-потомком. Она представляет собой массив наследумых объектов
//

// Адрес массива объектов и дескриптор разделяемой памяти передаётся процессу потомку через environment 
const char *const P2PROCESS_TABLE_ADDRESS = "P2PROCESS_TABLE_DESCRIPTOR";
const char *const P2PROCESS_TABLE_DESCRIPTOR = "P2PROCESS_TABLE_ADDRESS";

// аллокация-деаллокация должны быть определены в одной DSO. todo: лучшая альтернатива -- использовать ::HeapAlloc(GetProcessHeap(), 0, sz);
inline void* allocate(size_t sz) 	 	{ return P2SysParserMemAlloc(sz); } 
inline void  deallocate(void* pMemory) 	{ P2SysParserMemFree(pMemory); }


const size_t MAX_NAMESZ = 32;  	// максимальная длина имени объекта. 
const size_t MAX_OBJS = 32;		// количество объектов

struct InheritableObj 
{
	char name[MAX_NAMESZ];  	// имя объекта
	// типы объектов, которые могут наследоваться
	union { 
		UINT64  		align;
		char 			event[P2EVENT_SHARED_SIZE];  // дескриптор эвента  
		char    		file[P2FILE_HANDLE_SIZE];    // системный дескриптор (linux: int, windows: handle)
		volatile UINT32 uint32;
	} value; 
};

struct Process
{ 
	P2SHARED_MEM_HANDLE   m_hSharedMem; 		// дескриптор наследуемой памяти
	InheritableObj* 	  m_objects; 			// указатель на наследуемую память
    unsigned int		  m_exitStatus;
#ifdef _WIN32
	HANDLE				  m_hNativeHandle; 
	DWORD 				  m_processID;
#elif defined(__linux__) || defined (__MACH__)
	pid_t				  m_processID;
#endif
};

inline void ZeroProcess(P2PROCESS_HANDLE process)
{ 
	process->m_hSharedMem = P2SHARED_MEM_INVALID_HANDLE;
	process->m_objects = NULL;
	process->m_processID = 0;
    process->m_exitStatus = 0;
#ifdef _WIN32
	process->m_hNativeHandle = NULL;	
#endif // _WIN32
}

// nb. no thread safe 
// В процессе-потомке получить наследуемые объекты
inline P2ERR GetCurrentProcessInfo(P2PROCESS_HANDLE process)
{ 
	ZeroProcess(process); 
	char addrStr[128]; 
	 RETURN_P2ERR(P2GetEnvironmentVariable(P2PROCESS_TABLE_ADDRESS, addrStr, sizeof(addrStr))); 
	 if(*addrStr == 0)
	 { 
		 RETURN_P2ERR(P2ConvertEnvironmentVariableToHandle(P2PROCESS_TABLE_DESCRIPTOR, &(process->m_hSharedMem))); 
		 if(process->m_hSharedMem == P2SHARED_MEM_INVALID_HANDLE) 
			 return P2ERR_OK; 
		 process->m_objects = static_cast<InheritableObj*>(P2SharedMemMap(process->m_hSharedMem, MAX_OBJS * sizeof(InheritableObj))); 
		 if(!process->m_objects) 
			 return P2ERR_COMMON_SYSCALL_FAILED; 
		 _snprintf(addrStr, sizeof(addrStr), "%p", process->m_objects ); 
		 RETURN_P2ERR(P2SetEnvironmentVariable(P2PROCESS_TABLE_ADDRESS, addrStr));
	 }
	 else
	 {
		 sscanf(addrStr, "%p", &(process->m_objects));  //todo: check addrStr wrong format
	 } 
	 return P2ERR_OK; 
}

// в процессе-потомке найти объект с заданным именем 
inline P2ERR GetObj(InheritableObj** ppObject, const char* name)
{ 
	if (!name || *name == '\0') return P2ERR_COMMON_WRONG_ARGUMENTS;
	Process process; 
	RETURN_P2ERR(GetCurrentProcessInfo(&process)); 
	InheritableObj* obj = process.m_objects;
	if (!obj) return P2ERR_COMMON_NO_SUCH_ITEM;
	for (size_t i = 0; i < MAX_OBJS; ++i, ++obj)
	{
		if (strcmp(obj->name, name) == 0)
		{
			*ppObject = obj;
			return P2ERR_OK;
		}
	} 
	return P2ERR_COMMON_NO_SUCH_ITEM;
}

// в процессе родителе создать пустой объект с заданным именем. В пустой объект можно будет записать настоящий объект
inline P2ERR CreateObj(P2PROCESS_HANDLE process, InheritableObj** ppObject, const char* name)
{
	if (!name || *name == 0) return P2ERR_COMMON_WRONG_ARGUMENTS; 
	if (strlen(name) + 1 > MAX_NAMESZ) return P2ERR_COMMON_BAD_STRING; 

	if (process->m_objects == NULL)
	{ 
		P2SHARED_MEM_HANDLE sh;
		RETURN_P2ERR(P2SharedMemCreate(&sh, P2FILE_INVALID_HANDLE, MAX_OBJS * sizeof(InheritableObj), NULL, &sh)); 
		process->m_objects = static_cast<InheritableObj*>(P2SharedMemMap(sh, MAX_OBJS * sizeof(InheritableObj))); 
		if (!process->m_objects) 
		{
			LAST_ERROR err = GetLastError(); 
			P2SharedMemClose(sh);
			SetLastError(err);
			return P2ERR_COMMON_SYSCALL_FAILED;
		} 
		process->m_hSharedMem = sh;
	}

	InheritableObj* obj = process->m_objects;
	for (size_t i = 0; i < MAX_OBJS; ++i, ++obj)
	{
		if (obj->name[0] == '\0')
		{
			strcpy(obj->name, name); 
			*ppObject = obj;
			return P2ERR_OK;
		}
	} 
	return P2ERR_COMMON_BUFFER_TOO_SMALL;  
}


#ifdef _WIN32  

inline void CopyToEnvironment(const char* pStr, std::vector<char>& env)
{
	while(*pStr != 0)
		env.push_back(*pStr++);
	env.push_back('\0');
}

inline bool VarNameEqual(const char* first, const char* second)
{
	if(!first || !second) 
		return false;

	while(  (*first  != '=') &&
			(*second != '\0') &&
			(*first == *second))
	{
		++first;
		++second; 
	}

	return ((*first == '=' ) && (*second == '\0')); 
}

//  получить окружение для процесса-потомка. Окружение задаётся в виде таблицы строк вида key=value с завершающей пустой строкой.
inline void CreateProcessEnvironment(std::vector<char>& env, P2PROCESS_HANDLE process)
{
	char* pEnvMemory(GetEnvironmentStrings());
	const char* pCurrentEnv = pEnvMemory; 
	env.reserve(512);

	char td[64]; 
	P2SysSNPrintf(td, sizeof(td), "%s=%p", P2PROCESS_TABLE_DESCRIPTOR, process->m_hSharedMem);

	while(*pCurrentEnv != 0)
	{
		// можно создавть процессы-потомки второго порядка (родитель-потомок-потомок). Поэтому заменяем дескриптор разделяемой памяти на свой, если мы потомок и создаём нового потомка
		if(!VarNameEqual(pCurrentEnv, P2PROCESS_TABLE_ADDRESS))
		{
			if(VarNameEqual(pCurrentEnv, P2PROCESS_TABLE_DESCRIPTOR))
			{
				CopyToEnvironment(td, env);
				td[0] = '\0';
			}
			else
			{
				CopyToEnvironment(pCurrentEnv, env);
			}
		}
		pCurrentEnv += strlen(pCurrentEnv) + 1;
	}

	if(td[0] != '\0')
		CopyToEnvironment(td, env); 
	env.push_back('\0');

	FreeEnvironmentStrings(pEnvMemory);
}

// получить командную строку для процесса-потомка из списка аргументов
inline P2ERR CreateProcessCommandLine(char **ppArgument, char* pCommandLine, size_t szCommandLine)
{
	*pCommandLine = 0;
	while(*ppArgument != NULL)
	{
		size_t n = strlen(*ppArgument);
		if(n + 2 > szCommandLine)
			return P2ERR_COMMON_BAD_STRING;

		strcpy(pCommandLine, *ppArgument);
		pCommandLine += n;
		*pCommandLine++ = ' ';
		*pCommandLine = '\0';
		szCommandLine-= (n + 1); 
		++ppArgument;
	}

	return P2ERR_OK; 
}

#elif defined(__linux__) || defined (__MACH__) 

inline P2ERR CreateProcessEnvironment(P2PROCESS_HANDLE process)
{
	RETURN_P2ERR(P2DeleteEnvironmentVariable(P2PROCESS_TABLE_ADDRESS)); 

	char td[64]; 
	P2SysSNPrintf(td, sizeof(td), "%d", process->m_hSharedMem); 
	return P2SetEnvironmentVariable(P2PROCESS_TABLE_DESCRIPTOR, td); 
}

#endif 

}; // namespace p2process_impl 

inline P2ERR P2ProcessCreate(P2PROCESS_HANDLE* process)
{
	using namespace p2process_impl; 
	if (!process) return P2ERR_COMMON_WRONG_ARGUMENTS; 
	if ((*process = static_cast<P2PROCESS_HANDLE>(allocate(sizeof(Process)))) == NULL) return P2ERR_COMMON_NO_MEMORY; 
	ZeroProcess(*process); 
	return P2ERR_OK; 
}

inline P2ERR P2ProcessClose(P2PROCESS_HANDLE process)
{ 
	using namespace p2process_impl; 
	if (!process) return P2ERR_COMMON_WRONG_ARGUMENTS;

#ifdef _WIN32
	if (process->m_hNativeHandle != NULL) 
		CloseHandle(process->m_hNativeHandle);
#endif 
	if (process->m_objects)
	{
		P2SharedMemClose(process->m_hSharedMem);
		P2SharedMemUnMap(process->m_objects, MAX_OBJS * sizeof(InheritableObj));
	} 
	deallocate(process); 
	return P2ERR_OK;
}


// argv - первый аргумент - путь к программе

#ifdef _WIN32

#define P2PROCESS_CREATE_NO_WINDOW 	 CREATE_NO_WINDOW      // launch process without console window
#define P2PROCESS_CREATE_NEW_CONSOLE CREATE_NEW_CONSOLE   
#define P2PROCESS_PARENT "PARENT_PROCESS"

inline P2ERR P2ProcessLaunch(P2PROCESS_HANDLE process, char* argv[], UINT32 flag = P2PROCESS_CREATE_NEW_CONSOLE) 
{ 
	using namespace p2process_impl; 

	PROCESS_INFORMATION pi; 
	STARTUPINFO si; 
	std::vector<char> env;
	char commandLine[4096];

	if (!process) return P2ERR_COMMON_WRONG_ARGUMENTS;

	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO); 
	si.lpTitle = "."; 

	RETURN_P2ERR(CreateProcessCommandLine(argv, commandLine, sizeof(commandLine))); 
	CreateProcessEnvironment(env, process);

	// В Windows добавляем в наследуемые объекты дескриптор текущего процесса. 
	// Это позволит потомкам отслеживать статус процесса-родителя и, например, завершаться если родитель завершился.

	HANDLE hCurrentProcess;
	if (!DuplicateHandle(GetCurrentProcess(), GetCurrentProcess(), GetCurrentProcess(), &hCurrentProcess, 0, TRUE, DUPLICATE_SAME_ACCESS)) 
		return P2ERR_COMMON_SYSCALL_FAILED;

	if(!CreateProcess(NULL, commandLine, NULL, NULL, TRUE, flag, &env[0], NULL, &si, &pi))
	{ 
		CloseHandle(hCurrentProcess);
		return P2ERR_COMMON_SYSCALL_FAILED;
	}

	InheritableObj* obj; 
	RETURN_P2ERR(CreateObj(process, &obj, P2PROCESS_PARENT));  
	memcpy(obj->value.file, &hCurrentProcess, sizeof(hCurrentProcess));

	CloseHandle(pi.hThread);
	CloseHandle(hCurrentProcess);

	process->m_hNativeHandle = pi.hProcess;
	process->m_processID	 = pi.dwProcessId;

	return P2ERR_OK;
}

#elif defined(__linux__) || defined (__MACH__)

#define P2PROCESS_CREATE_NO_WINDOW 	 0      // ignore on linux platform
#define P2PROCESS_CREATE_NEW_CONSOLE 0   

inline P2ERR P2ProcessLaunch(P2PROCESS_HANDLE process, char* argv[], UINT32 flag = 0x0)
{
	UNUSED(flag);

	if(!process)
		return P2ERR_COMMON_WRONG_ARGUMENTS;

	process->m_processID = fork();
	if(process->m_processID  < 0)
	{
		process->m_processID = 0;
		return P2ERR_COMMON_SYSCALL_FAILED;
	}
	else if(process->m_processID == 0)
	{ 
		P2ERR res;
		if(P2FAILEDV(p2process_impl::CreateProcessEnvironment(process), res))
		{
			_exit(res);
		} 
		execvp(argv[0], argv);  
		fprintf(stderr, "P2Process couldn't exec '%s'; errno '%s'", argv[0], strerror(errno));
		_exit(errno); 
	} 

	return P2ERR_OK; 
}

#endif	//defined(__linux__) || defined (__MACH__)


inline P2ERR P2ProcessWait(P2PROCESS_HANDLE process, UINT32 timeout)
{
	if(!process) 
		return P2ERR_COMMON_WRONG_ARGUMENTS;

	if(process->m_processID == 0)
		return P2ERR_OK;

#ifdef _WIN32
	switch( WaitForSingleObject(process->m_hNativeHandle, timeout) )
	{
		case WAIT_OBJECT_0:
        {
			process->m_processID = 0;
            DWORD exitCode;
            GetExitCodeProcess(process->m_hNativeHandle, &exitCode);
            process->m_exitStatus = exitCode;
			return P2ERR_OK;
        }
		case WAIT_TIMEOUT:
			return P2ERR_COMMON_TIMEOUT;
		default:
			return P2ERR_COMMON_SYSCALL_FAILED; 
	}

#elif defined(__linux__) || defined (__MACH__)

	pid_t pid;  
	int	  status;
	unsigned int start = 0;
	static const unsigned MCS_SLEEP_TIMEOUT = 100*1000; /*100 ms*/

	if(timeout != 0)
		start = GetTickCount(); 

	while(true)
	{
		pid = waitpid(process->m_processID, &status, WNOHANG);

		if(pid == -1)
			return P2ERR_COMMON_SYSCALL_FAILED; 
		else if(pid > 0)
		{
			process->m_processID = 0; 
			process->m_exitStatus = status; 
			return P2ERR_OK;
		}
		else
        {
			if(timeout != 0 && GetTickCount() - start < timeout)
				usleep(MCS_SLEEP_TIMEOUT);
			else
				return P2ERR_COMMON_TIMEOUT;
        }
	} 
#endif 
}


inline P2ERR P2ProcessTerminate(P2PROCESS_HANDLE process)
{
	if(!process || process->m_processID == 0)
		return P2ERR_COMMON_WRONG_ARGUMENTS; 
#ifdef _WIN32
	TerminateProcess(process->m_hNativeHandle, P2ERR_COMMON_EXITCODE_TERMINATED);
#elif defined(__linux__) || defined (__MACH__)
	kill(process->m_processID, SIGTERM); 
#endif
	return P2ERR_OK;
}

/* Возвращает код завершения дочернего процесса.
   Процесс к моменту вызова должен быть завершен (P2ProcessWait возвращает OK)
   ВНИМАНИЕ: под Linux код возврата содержит только 8 младших битов!        */
inline P2ERR P2ProcessGetExitCode(P2PROCESS_HANDLE process, UINT32* pExitCode)
{
    if(!process || !pExitCode|| process->m_processID != 0)
        return P2ERR_COMMON_WRONG_ARGUMENTS;
#ifdef _WIN32
    *pExitCode = process->m_exitStatus;
#elif defined(__linux__) || defined (__MACH__)
    if (!WIFEXITED(process->m_exitStatus))
        return P2ERR_COMMON_WRONG_ARGUMENTS;
    *pExitCode = WEXITSTATUS(process->m_exitStatus);
#endif
    return P2ERR_OK;
}


#ifdef _WIN32
inline HANDLE P2ProcessGetNativeHandle(P2PROCESS_HANDLE process)
{
	return process->m_hNativeHandle;
}
#endif




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline P2ERR P2ProcessCreateInheritableEvent(P2EVENT_HANDLE* pEvent, P2PROCESS_HANDLE process, const char* name, bool bManualReset, bool bInitialState)
{ 
	using namespace p2process_impl;

	P2ERR res; 
	InheritableObj* obj; 
	RETURN_P2ERR(CreateObj(process, &obj, name)); 
	if(P2FAILEDV(P2EventCreate(pEvent, bManualReset, bInitialState, obj->value.event), res))
	{
		obj->name[0] = 0; 
	} 
	return res; 
}

inline P2ERR P2ProcessGetInheritedEvent(P2EVENT_HANDLE* pEvent, const char* name)
{
	using namespace p2process_impl; 
	InheritableObj* obj;
	RETURN_P2ERR(GetObj(&obj, name)); 
	return P2EventGet(pEvent, obj->value.event); 
}

inline P2ERR P2ProcessCreateInheritableSharedMem(P2SHARED_MEM_HANDLE* pHandle, 
											 	 P2PROCESS_HANDLE	process,
											 	 P2FILE_HANDLE 		hFile,
											 	 UINT32 			size,
											 	 const char* 		name)
{
	using namespace p2process_impl;
 
	P2ERR res; 
	InheritableObj* obj; 
	RETURN_P2ERR(CreateObj(process, &obj, name)); 
	if(P2FAILEDV(P2SharedMemCreate(pHandle, hFile, size, NULL, obj->value.file), res))
	{
		obj->name[0] = 0; 
	} 
	return res; 
}

inline P2ERR P2ProcessGetInheritedSharedMem(P2SHARED_MEM_HANDLE* pHandle, const char *name)
{ 
	using namespace p2process_impl;
  
	InheritableObj* obj(NULL); 
	RETURN_P2ERR(GetObj(&obj, name)); 
	return P2SharedMemGet(pHandle, obj->value.file); 
}

inline P2ERR P2ProcessCreateInheritableValue(P2PROCESS_HANDLE process, volatile UINT32** ppValue, const char* name)
{ 
	using namespace p2process_impl; 
	InheritableObj* obj; 
	RETURN_P2ERR(CreateObj(process, &obj, name)); 
	*ppValue = &(obj->value.uint32); 
	return P2ERR_OK; 
}

inline P2ERR P2ProcessSetInheritableValue(P2PROCESS_HANDLE process, UINT32 value, const char* name)
{ 
	volatile UINT32 *pnum;
	RETURN_P2ERR(P2ProcessCreateInheritableValue(process, &pnum, name));
	*pnum = value; 
	return P2ERR_OK; 
}

inline P2ERR P2ProcessGetInheritedValue(volatile UINT32** ppValue, const char* name)
{ 
	using namespace p2process_impl; 
	InheritableObj* obj;
	RETURN_P2ERR(GetObj(&obj, name)); 
	*ppValue = &(obj->value.uint32); 
	return P2ERR_OK; 
}

#ifdef _WIN32
inline P2ERR P2ProcessGetParent(HANDLE* pHandle)
{
	using namespace p2process_impl; 
	if (!pHandle) return P2ERR_COMMON_WRONG_ARGUMENTS; 
	InheritableObj* obj;
	RETURN_P2ERR(GetObj(&obj, P2PROCESS_PARENT)); 
	memcpy(pHandle, obj->value.file, sizeof(HANDLE)); 
	return P2ERR_OK;
}
#endif 

// В виндовой ветке для совместимости со старым кодом поддерживается два варианта: считывание
// дескриптора эвента из переменной окружения или из разделяемой памяти. 
#define RTS_EXIT_EVENT_NAME  "RTSExitEvent"
#define RTS_EXIT_FLAG_NAME	 "RTSExitFlag" // RTSExitFlag имеет тот же смысл, что RTSExitEvent. Программы могут поверять флаг, а не эвент для скорости.
inline P2ERR P2ProcessGetRTSExitEvent(P2EVENT_HANDLE* pHandle, volatile UINT32** ppFlag = NULL)
{
	P2ERR res;
	if (pHandle)
	{
		if(P2FAILEDV(P2ProcessGetInheritedEvent(pHandle, RTS_EXIT_EVENT_NAME), res) && res != P2ERR_COMMON_NO_SUCH_ITEM)
		{
			return res;
		} 

		if(res == P2ERR_COMMON_NO_SUCH_ITEM)
		{
#ifdef _WIN32
			char buf[64];
			RETURN_P2ERR(P2GetEnvironmentVariable(RTS_EXIT_EVENT_NAME, buf, sizeof(buf)));
			if(*buf == 0 || 1 != sscanf(buf, "%p", pHandle))
				return P2ERR_COMMON_NO_SUCH_ITEM; 
#elif defined(__linux__) || defined (__MACH__)
			return res;
#endif
		} 
	}

	if (ppFlag)
	{
		RETURN_P2ERR(P2ProcessGetInheritedValue(ppFlag, RTS_EXIT_FLAG_NAME));
	} 
	return P2ERR_OK;
}




typedef AutoP2Handle<P2PROCESS_HANDLE, P2ProcessClose> P2AutoProcess; 

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#endif  // _P2PROCESS_HPP_ 
