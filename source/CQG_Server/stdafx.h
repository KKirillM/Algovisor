#include <SDKDDKVer.h>

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#define _AFX_ALL_WARNINGS

//Символ _AFXDLL определяется средой Visual C++, если вы используете библиотеку классов MFC как библиотеку DLL. 
//Если же код MFC подключается к приложению как статическая библиотека, этот символ не определен.
//#ifndef _DEBUG
//	#define _AFXDLL
//#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#include <tchar.h>
//#include <Windows.h>

#include <afxwin.h>         // MFC core and standard components

#include "StrSafe.h"

#include <atlbase.h>
#include <atlcom.h>      // IDispEventImpl

#include <stdexcept>
#include <vector>

#import "C:\Program Files (x86)\CQG\CQG Trader\Bin\CQGCEL-4_0.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids, auto_search
#import "C:\Program Files (x86)\CQG\CQG Trader\Bin\CQG.Trader.Services.CEL.Host.ActiveX.tlb" no_namespace, named_guids