#ifndef _STUFF_DLL_H_

#if defined(__linux__) || defined(__MACH__)
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "../../P2Common/Include/P2Errors.h"

// TODO: сейчас механизм CLoadLibraryChecker не работает, потому что загрузка DSO делается с RTLD_LOCAL.
// И не надо, возможность загрузки дебажной и релизной версии в процесс низковероятна.
struct CLoadLibraryChecker
{
    CLoadLibraryChecker(const char* moduleName, int moduleLoaderChecker)
    { 
		char buf[512];
			
		sprintf(buf, "g_load_library_checker_%s", moduleName);

		void* sym = dlsym(RTLD_DEFAULT, buf);

		if (sym == NULL)
		{
			//см. комментарий выше
			//fprintf(stderr, "dlsym() returns NULL for moduleName '%s': %s\n", moduleName, dlerror());				
		} 
		else
		{ 
			if( *static_cast<int*>(sym) != moduleLoaderChecker)
			{
				Dl_info info;
				const char* previous; 
				if(dladdr(sym, &info))
					previous = info.dli_fname;
				else
					previous = "Unknown_module";

				fprintf(stderr, "ERROR: module '%s' already loaded from '%s'. Application will be terminated\n", moduleName, previous);
				fflush(stderr);
				exit(P2ERR_COMMON_WRONG_BUILD);
			}
		}

    }
};


// NB. Имена переменных g_load_library_checker_* указываются линкеру для того, чтобы экспортировать их из dynamic_symbol table
// (это необходимо, чтобы dlsym находил g_load_library_checker_* перменные в исп. файле)
// Менять нужно одновременно с  ../P2Common/syms.txt
// Символ объявлен weak, чтобы резолвиться на первое определение.
#ifdef _DEBUG
#define LOAD_LIBRARY_CHECK(s) 	int g_load_library_checker_##s __attribute__ ((weak, visibility ("default"))) = 0;	\
								CLoadLibraryChecker g_stuffdll_h__load_library_checker_##s(#s, 0 ); 
#else

#define LOAD_LIBRARY_CHECK(s) 	int g_load_library_checker_##s __attribute__ ((weak, visibility ("default"))) = 1;	\
								CLoadLibraryChecker g_stuffdll_h__load_library_checker_##s(#s, 1 ); 
#endif //_DEBUG
#else
#include "../../P2Common/Include/P2Windows.h"

extern "C" int __cdecl __crtMessageBoxA(const char* lpText, const char* lpCaption, UINT uType);

#include <fstream>
#include <string>

struct CLoadLibraryChecker
{
	CLoadLibraryChecker(const char* moduleName, int flag) 
	{
		m_atom = FindAtom(moduleName);                           
		if (m_atom == 0)
		{
			m_atom = AddAtom(moduleName);
		}
		else
		{
			if (!flag)
			{
				//debug
				const std::string moduleNameD = std::string(moduleName) + "D"; 
				__crtMessageBoxA("Module has already been loaded", moduleNameD.c_str(), MB_OK | MB_TASKMODAL | 0x00040000L);
			}
			else
			{
				//0x00040000L - MB_SERVICE_NOTIFICATION, чтобы не мучаться с define'ами в winuser.h
				__crtMessageBoxA("Module has already been loaded", moduleName, MB_OK | MB_TASKMODAL | 0x00040000L);
			}
			m_atom = 0;
		}
	}

	~CLoadLibraryChecker()
	{
		if (m_atom)
		{
			DeleteAtom(m_atom);
		}
	}
private:
	ATOM m_atom;
};

//static CLoadLibraryChecker g_checker("P2SysIni");

#ifdef _DEBUG 
#define LOAD_LIBRARY_CHECK(s) static CLoadLibraryChecker g_stuffdll_h__load_library_checker(#s, 0)
#else
#define LOAD_LIBRARY_CHECK(s) static CLoadLibraryChecker g_stuffdll_h__load_library_checker(#s, 1)
#endif //_DEBUG

#endif //defined(__linux__) || defined(__MACH__)

#endif //_STUFF_DLL_H_
