#ifndef _P2SYSINI_H_
#define _P2SYSINI_H_

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef P2SYS_EXPORTS
#define P2SYSINI_API P2_API_EXPORTS
#else
#define P2SYSINI_API P2_API_IMPORTS
#endif

#include "../../P2Common/Include/P2Errors.h"

//////////////////////////////////////////////////////////////////////

#ifdef P2SYS_EXPORTS
  namespace NS_P2SysIni
  {
    class CIniMan;
  }
  using NS_P2SysIni::CIniMan;
#else
  struct CIniMan;
#endif

typedef void* P2INI_HANDLE;

typedef P2ERR (P2CALLSPEC *P2INI_ENUMKEYS_CALLBACK)(const char* section, const char* key, const char* value, void* cbParams);
typedef P2ERR (P2CALLSPEC *P2INI_ENUMSECTIONS_CALLBACK)(const char* section, void* cbParams);

#define P2INI_MAX_SECTION_SIZE  32767
#define P2INI_SECTIONS_BUF_SIZE 65535

//////////////////////////////////////////////////////////////////////
// Default INI

#define P2INI_DEF_HANDLE    ((P2INI_HANDLE)~0)
#define P2IniGetDefHandle() P2INI_DEF_HANDLE

P2SYSINI_API P2ERR P2CALLSPEC P2IniOpen(const char* name);
P2SYSINI_API BOOL P2CALLSPEC P2IniIsOpened();
P2SYSINI_API P2ERR P2CALLSPEC P2IniClose();

#define P2IniGetInt(section, key, defValue)                 P2IniCustGetInt(P2IniGetDefHandle(), section, key, defValue)
#define P2IniGetStr(pRet, maxSize, section, key, defValue)  P2IniCustGetStr(P2IniGetDefHandle(), pRet, maxSize, section, key, defValue)
#define P2IniGetSection(pRet, pSize, section)               P2IniCustGetSection(P2IniGetDefHandle(), pRet, pSize, section)
#define P2IniWriteInt(section, key, value)                  P2IniCustWriteInt(P2IniGetDefHandle(), section, key, value)
#define P2IniWriteStr(section, key, value)                  P2IniCustWriteStr(P2IniGetDefHandle(), section, key, value)
#define P2IniWriteSection(section, value)                   P2IniCustWriteSection(P2IniGetDefHandle(), section, value)
#define P2IniEnumKeys(section, cbFunc, cbParams)            P2IniCustEnumKeys(P2IniGetDefHandle(), section, cbFunc, cbParams)
#define P2IniEnumSections(cbFunc, cbParams)                 P2IniCustEnumSections(P2IniGetDefHandle(), cbFunc, cbParams)

//////////////////////////////////////////////////////////////////////
// Custom INI(s)

P2SYSINI_API P2ERR P2CALLSPEC P2IniCustOpen(P2INI_HANDLE* pIni, const char* name);
P2SYSINI_API P2ERR P2CALLSPEC P2IniCustClose(P2INI_HANDLE ini);

P2SYSINI_API int P2CALLSPEC P2IniCustGetInt(P2INI_HANDLE ini, const char* section, const char* key, int defValue);
P2SYSINI_API size_t P2CALLSPEC P2IniCustGetStr(P2INI_HANDLE ini, char* pRet, size_t maxSize,
                                               const char* section, const char* key, const char* defValue);
P2SYSINI_API P2ERR P2CALLSPEC P2IniCustGetSection(P2INI_HANDLE ini, char* pRet, size_t* pSize, const char* section);
P2SYSINI_API P2ERR P2CALLSPEC P2IniCustWriteInt(P2INI_HANDLE ini, const char* section, const char* key, int value);
P2SYSINI_API P2ERR P2CALLSPEC P2IniCustWriteStr(P2INI_HANDLE ini, const char* section, const char* key, const char* value);
P2SYSINI_API P2ERR P2CALLSPEC P2IniCustWriteSection(P2INI_HANDLE ini, const char* section, const char* value);

P2SYSINI_API P2ERR P2CALLSPEC P2IniCustEnumKeys(P2INI_HANDLE ini, const char* section,
                                                P2INI_ENUMKEYS_CALLBACK cbFunc, void* cbParams);
P2SYSINI_API P2ERR P2CALLSPEC P2IniCustEnumSections(P2INI_HANDLE ini, P2INI_ENUMSECTIONS_CALLBACK cbFunc, void* cbParams);

P2SYSINI_API P2ERR P2CALLSPEC P2IniGetFileName(P2INI_HANDLE ini, char* pFileName, size_t* pBufSize);

//////////////////////////////////////////////////////////////////////

#define P2INI_OPT_CREATE_IF_ABSENT      0x00000001

P2SYSINI_API P2ERR P2CALLSPEC P2IniSetOpt(UINT32 options);
P2SYSINI_API P2ERR P2CALLSPEC P2IniGetOpt(UINT32* pOptions);

#ifdef  __cplusplus
}
#endif

#endif // _P2SYSINI_H_
