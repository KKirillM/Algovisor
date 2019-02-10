#pragma once

#pragma warning (disable : 4100)
#pragma warning (disable : 4480)

//#ifndef _SECURE_ATL
//#define _SECURE_ATL 1
//#endif

#define _CRT_SECURE_NO_WARNINGS

//включаем отладочные функции кучи
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif



#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"
#include "tchar.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

//#ifndef _DEBUG
//	#define _AFXDLL
//#endif

//#ifdef _DEBUG
//	#undef _AFXDLL
//#endif



#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdlgs.h>
#include <afxdtctl.h>       // MFC support for Internet Explorer 4 Common Controls

#include "CrashRpt.h" //Crash Report library

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

//#include <stdexcept>
//#include <iomanip>
//#include "math.h"

#if (_MSC_VER == 1900)  // (Visual Studio 2015)
    /*MSVC++ 14.0 _MSC_VER == 1900 (Visual Studio 2015)
    MSVC++ 12.0 _MSC_VER == 1800 (Visual Studio 2013)
    MSVC++ 11.0 _MSC_VER == 1700 (Visual Studio 2012)
    MSVC++ 10.0 _MSC_VER == 1600 (Visual Studio 2010)
    MSVC++ 9.0  _MSC_VER == 1500 (Visual Studio 2008)
    MSVC++ 8.0  _MSC_VER == 1400 (Visual Studio 2005)
    MSVC++ 7.1  _MSC_VER == 1310 (Visual Studio 2003)
    MSVC++ 7.0  _MSC_VER == 1300
    MSVC++ 6.0  _MSC_VER == 1200
    MSVC++ 5.0  _MSC_VER == 1100*/
#include <stdio.h>
#include <conio.h>
//#pragma comment(lib, "legacy_stdio_definitions.lib")
#endif

#include <vector>
#include <list>
#include <map>
#include <set>
#include <string>
#include <fstream>
#include <algorithm>
#include <exception>
#include <iterator>
#include <locale>
#include <memory>
#include <vector>
#include <unordered_map>
#include <limits.h>
#include "StrSafe.h"

#include <sys/stat.h>
#include <sys/types.h>

//#include "RApi.h"
#include "RApiPlus.h"

#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/md5.h>

#include <curl.h>

#include <atlenc.h>  //это для base64encode

#include <rapidjson/writer.h>
#include <rapidjson/reader.h>
#include <rapidjson/stringbuffer.h>

#include <TlHelp32.h>

//ASProtect headers
#define USE_SF_PROTECTED_CLASSES

#if defined(_WIN64) && defined(ASPROTECT)
#include <PsaApi.h>		//текущая версия ASProtect расчитана только на 64-битную конфигурацию
#else
#define PsProtUInt32_1 UINT
#endif

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

//#define new DEBUG_NEW

//#ifdef _DEBUG
//	#ifdef _CRTDBG_MAP_ALLOC
//	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
//	#endif /* _CRTDBG_MAP_ALLOC */
//#endif /* _DEBUG */

//#ifdef _DEBUG
//   #ifndef DBG_NEW
//      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
//      #define new DBG_NEW
//   #endif
//#endif  // _DEBUG

#include "DEFINE.h"