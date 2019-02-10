#ifndef _P2SYSINI_HPP_
#define _P2SYSINI_HPP_

#ifdef  __cplusplus

#include "../../P2Utils/Include/AutoHandle.hpp"
#include "P2SysIni.h"
#include "IP2Ini.h"
#include <string>

typedef AutoP2Handle<P2INI_HANDLE, P2IniCustClose> P2AutoIniHandle;

P2SYSINI_API P2ERR P2CALLSPEC P2IniOpenEx(const char* name, IP2Ini* pcustImplObj);
P2SYSINI_API P2ERR P2CALLSPEC P2IniCustOpenEx(P2INI_HANDLE* pIni, const char* name, IP2Ini* pcustImplObj);

// Convenient C++ wrappers for ini functions
inline 
std::string P2IniGetVal(const std::string& sect, const std::string& key, const std::string& default_value) 
{ 
	char opt[4096];
	P2IniGetStr(opt, sizeof(opt), sect.c_str(), key.c_str(), default_value.c_str());
	return opt; 
}

inline 
int P2IniGetVal(const std::string& sect, const std::string& key, int default_value) 
{
	return P2IniGetInt(sect.c_str(), key.c_str(), default_value); 
}

inline
std::string P2IniCustGetVal(P2INI_HANDLE ini, const std::string& sect, const std::string& key, const std::string& default_value) 
{
	char opt[4096];
	P2IniCustGetStr(ini, opt, sizeof(opt), sect.c_str(), key.c_str(), default_value.c_str()); 
	return opt;
} 

inline
int P2IniCustGetVal(P2INI_HANDLE ini, const std::string& sect, const std::string& key, int default_value) 
{
	return P2IniCustGetInt(ini, sect.c_str(), key.c_str(), default_value); 
} 

#endif  // __cplusplus

#endif // _P2SYSINI_HPP_
