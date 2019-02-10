// optstr.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPTSTR_H__B88BAC6A_63D9_46C4_A7D5_C07D340CE8BB__INCLUDED_)
#define AFX_OPTSTR_H__B88BAC6A_63D9_46C4_A7D5_C07D340CE8BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../P2Common/Include/P2Types.h"
#include <string>

// Now (simple version):
// - Key characters: { = ; , \ }
// - Without key characters in the field names & values
// - Field names case insensitive

//  
#if defined(__linux__) || defined(__MACH__)
#define OPTSTR_API  __attribute__ ((visibility("default"))) 
#else 
#define OPTSTR_API 
#endif

namespace optstr
{

using std::string;

size_t find_last_not_of(PCSTR ps, PCSTR pset);
PCSTR trim(PCSTR ps, size_t* pLen);

OPTSTR_API PCSTR GetFldVal(PCSTR opt, PCSTR fldName, size_t* pLen, PCSTR* ppFldBegin = 0);

// Always return pVal (even if error or no such field)!
OPTSTR_API string* GetFldVal(PCSTR opt, PCSTR fldName, string* pVal);

// Get field value as integer. If no suchh fileld retruns defaultValue.
OPTSTR_API bool GetFldInt(const char* opt, const char* name, int* value);

// Get field value & remove field from pOpt string
OPTSTR_API void RemoveFld(string* pOpt, PCSTR fldName, string* pVal);

OPTSTR_API string* AddFld(string* pOpt, PCSTR fldName, PCSTR fldVal);

};  // namespace optstr

#endif // !defined(AFX_OPTSTR_H__B88BAC6A_63D9_46C4_A7D5_C07D340CE8BB__INCLUDED_)
