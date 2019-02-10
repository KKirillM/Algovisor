//----------------------------------------------------------------------------
// Copyright: (c) 2006 RTS, Russia
//----------------------------------------------------------------------------
// Created:	    10 Feb 2006   18:10
// File:        p2ini
// Author:		Mikhail_Navernyuk
//----------------------------------------------------------------------------
// Description: Implementattion of the CP2Ini class
//----------------------------------------------------------------------------

#include "stdafx.h"

#include "p2ini.h"
#include "p2Log.h"

#if !defined(_WIN32)
typedef long HRESULT;
#define S_OK ((HRESULT)0L)
#define S_FALSE ((HRESULT)1L)
#endif

namespace fo2base
{

//----------------------------------------------------------------------------
/// Set Option flags for INI module
//----------------------------------------------------------------------------
void CP2Ini::SetOption(dword options)
{
    P2ERR rc;
    if(P2FAILEDV(P2IniSetOpt(options), rc))
    {
        throw CP2Error("SetOptions failed", rc);
    }
}

//----------------------------------------------------------------------------
void CP2Ini::Open(const string& fname)
{
    P2INI_HANDLE    h;
    P2ERR           rc;
	m_name = fname;
    if(P2FAILEDV(P2IniCustOpen(&h, fname.c_str()), rc))
    {
		switch (rc)
		{
		case P2ERR_INI_ALREADY_STARTED:
			{
				std::string s = "Ini file '" + fname + "' is already initialized";
				throw CP2Error(s, rc);
			}
		case P2ERR_COMMON_WRONG_ARGUMENTS:
			{
				std::string s = "Ini filename is empty";
				throw CP2Error(s, rc);
			}
		case P2ERR_INI_FILE_NOT_FOUND:
			{
				std::string s = "Ini file not found";
				throw CP2Error(s, rc);
			}
		}
    }
    Attach(h);
}

//----------------------------------------------------------------------------
void CP2Ini::Close()
{
    Release();
}

const string& CP2Ini::GetName() const
{
	return m_name;
}

//----------------------------------------------------------------------------
void CP2Ini::WriteInt(const string& sect, const string& key, int val) const
{
    P2ERR rc;
    if(P2FAILEDV(P2IniCustWriteInt(GetHandle(), sect.c_str(), key.c_str(), val), rc))
    {
        throw CP2Error(rc);
    }
}

//----------------------------------------------------------------------------
void CP2Ini::WriteStr(const string& sect, const string& key, const string& val) const
{
    P2ERR rc;
    if(P2FAILEDV(P2IniCustWriteStr(GetHandle(), sect.c_str(), 
        key.c_str(), val.c_str()), rc))
    {
        throw CP2Error(rc);
    }
}

//----------------------------------------------------------------------------
int CP2Ini::ReadInt(const string& sect, const string& key, int def) const
{
    return P2IniCustGetInt(GetHandle(), sect.c_str(), key.c_str(), def);
}

//----------------------------------------------------------------------------
string CP2Ini::ReadStr(const string& sect, const string& key, const string& def) const
{
    const size_t    bufSize = 512;
    char            buf[bufSize + 1];
    char*           p = buf;
    size_t          size;

    size = P2IniCustGetStr(GetHandle(), p, bufSize, sect.c_str(), 
        key.c_str(), def.c_str());
    if( size <= bufSize )
    {
        return p;
    }
    else
    {
        p = new char[size];
        if( p == null ) throw CP2Error(P2ERR_COMMON_NO_MEMORY);
        P2IniCustGetStr(GetHandle(), p, size, sect.c_str(), 
            key.c_str(), def.c_str());
        m_str = p;
        delete [] p;
        return m_str;
    }
}

P2ERR enum_sections_cb(const char* section, void* cbParams)
{
	P2ERR rc = P2ERR_OK;
	try
	{
		vector<string>* lst = (vector<string>*)cbParams;
		lst->push_back( section );
	}
	catch(CP2Error &e)
	{
		FO2TRACE_ERROR((e.what()));
		rc = e.GetCode();
	}
	catch(exception& e)
	{
		FO2TRACE_ERROR((e.what()));
		rc = P2ERR_COMMON_INTERNAL_ERROR;
	}
	catch(...)
	{
		FO2TRACE_ERROR(("Oops! 'catch(...)' occured in enum_sections_cb :("));
		rc = P2ERR_COMMON_INTERNAL_ERROR;
	}
	return rc;
}

P2ERR enum_keys_cb(const char* /*section*/, const char* key, const char* value, void* cbParams)
{
	P2ERR rc = P2ERR_OK;
	try
	{
		map<string,string>& lst = *(map<string,string>*)cbParams;
		lst[string(key)] = string(value);
	}
	catch(CP2Error &e)
	{
		FO2TRACE_ERROR((e.what()));
		rc = e.GetCode();
	}
	catch(exception& e)
	{
		FO2TRACE_ERROR((e.what()));
		rc = P2ERR_COMMON_INTERNAL_ERROR;
	}
	catch(...)
	{
		FO2TRACE_ERROR(("Oops! 'catch(...)' occured in enum_keys_cb :("));
		rc = P2ERR_COMMON_INTERNAL_ERROR;
	}
	return rc;
}

bool CP2Ini::EnumSections(vector<string>& list)
{
	return (P2IniCustEnumSections(GetHandle(), enum_sections_cb, &list)==S_OK);
}

bool CP2Ini::EnumKeys(map<string, string>& list, const string& section)
{
	return (P2IniCustEnumKeys(GetHandle(), section.c_str(), enum_keys_cb, &list) == S_OK);
}

//----------------------------------------------------------------------------
// class CP2DefaultIni
//----------------------------------------------------------------------------
CP2DefaultIni::CP2DefaultIni() : CP2Ini()
{
}

//----------------------------------------------------------------------------
CP2DefaultIni::~CP2DefaultIni()
{
//     if(IsOpen())
//     {
//         Close();
//     }
}

//----------------------------------------------------------------------------
void CP2DefaultIni::Open(const string& fname)
{
    P2ERR rc;
    m_name = fname;
    if(P2FAILEDV(P2IniOpen(fname.c_str()), rc))
    {
		switch (rc)
		{
		case P2ERR_INI_ALREADY_STARTED:
			{
				std::string s = "Ini file '" + fname + "' is already initialized";
				throw CP2Error(s, rc);
			}
		case P2ERR_COMMON_WRONG_ARGUMENTS:
			{
				std::string s = "Ini filename is empty";
				throw CP2Error(s, rc);
			}
		case P2ERR_INI_FILE_NOT_FOUND:
			{
				std::string s = "Ini file not found";
				throw CP2Error(s, rc);
			}
		}
    }

#pragma warning (disable: 4312)
    Attach(P2IniGetDefHandle());
#pragma warning (default: 4312)
}


} // namespace fo2base

