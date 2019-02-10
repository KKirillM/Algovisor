#ifndef _P2DLL_H_
#define _P2DLL_H_
 
#ifdef _WIN32 
#include "../../P2Common/Include/P2Windows.h"
#include "../../P2SysTypes/Include/P2SysTypes.h"
#elif defined(__linux__) || defined (__MACH__) 
#include <dlfcn.h>
//#include <string.h>
#endif //defined(__linux__) || defined (__MACH__)
 
#include "../../P2Common/Include/P2Types.h"
#include "../../P2Common/Include/P2Errors.h"
#include "P2SysAPI.h"
 
 ////////////////////////////////////////////////////////////////////////////////
 //  			функции для работы с *.DLL- и *.SO- файлами.
 #ifdef _WIN32 
	typedef HMODULE P2DLL_HANDLE; 
 // флаги, которые можно передавать в P2LoadLibrary. Используются только в юникс-ветке
	#define RTLD_NOW 		0
	#define RTLD_LAZY		0
	#define RTLD_GLOBAL		0
	#define RTLD_LOCAL		0 	
#elif defined(__linux__) || defined (__MACH__)
	#define FARPROC void* 
	typedef void*  	P2DLL_HANDLE;
#endif //_WIN32

#if defined (_WIN32) || defined (__MACH__)
	#define RTLD_DEEPBIND 	0 
#endif // defined (_WIN32) || defined (__MACH__)

#ifdef _DEBUG
#define _P2_LIBRARY_DEBUG_SUFFIX "D"
#else
#define _P2_LIBRARY_DEBUG_SUFFIX ""
#endif // _DEBUG

#ifdef _WIN64
#define _P2_LIBRARY_SUFFIX "64" _P2_LIBRARY_DEBUG_SUFFIX
#else //_WIN64
#define _P2_LIBRARY_SUFFIX _P2_LIBRARY_DEBUG_SUFFIX
#endif //_WIN64

#ifdef __linux__
#define _P2_LIBRARY_PREFIX "lib"
#define _P2_LIBRARY_EXT ".so"
#elif defined(__MACH__)
#define _P2_LIBRARY_PREFIX "lib"
#define _P2_LIBRARY_EXT ".dylib"
#else // win
#define _P2_LIBRARY_PREFIX ""
#define _P2_LIBRARY_EXT ".dll"
#endif // __linux__

#define DEFAULT_LOAD_FLAG   (RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND)	
 

/**
 * P2DLLoadLibrary - Загружает библиотеку.
 *
 * Функция используется для загрузки библиотеки.
 *
 * @param dllname [req] Указатель на строку с именем файла.
 *              Если имя файла указано полностью (с расширением) то оно будет использовано без изменений
 *              Если же расширение в имени файла не указано - полное имя файла будет автоматически достроено согласно следующим правилам:
 *                  "<prefix><dllname><x64><D>.<ext><version>"
 *                  - <prefix> - для сборки под MAC и LINUX равен "lib"; для Windows сборки - не используется и равен "" 
 *                  - <x64> - для сборки под 64-х битной Windows платформой равен "64" (или "x64", если <dllname> оканчивается цифрой); под другими платформами - не используется и равен ""
 *                  - <D> - для отладочной сборки равен "D"; для релизной - не используется и равен ""
 *                  - <ext> - для сборки под MAC и LINUX равен "so"; для Windows сборки - "dll"
 *                  - <version> - для сборки под LINUX равен ".1.<majorVer>"; для Windows и MAC сборки - не используется и равен ""
 *
 * @param flag [req] Параметр, передаваемый в функцию dlopen (на Windows платформе не используется)
 *
 * @returns Хэндл загруженного модуля.
 *
 */

P2SYS_API P2DLL_HANDLE 	P2DLLoadLibrary(const char* dllname, int flag);
P2SYS_API P2ERR 		P2DLLFreeLibrary(P2DLL_HANDLE hModule);
P2SYS_API const char* 	P2DLLGetLastError(char *pBuffer, size_t bufferSz);
P2SYS_API FARPROC 		P2DLLGetProcAddress(P2DLL_HANDLE hModule, const char* procName);
 
 
#endif //__P2DLL_H_
