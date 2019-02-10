//----------------------------------------------------------------------------
// Copyright: (c) 2006 RTS, Russia
//----------------------------------------------------------------------------
// Created:	    10 Feb 2006   18:08
// File:        p2ini
// Author:		Mikhail_Navernyuk
//----------------------------------------------------------------------------
// Description: Interface of the CP2Ini class
//----------------------------------------------------------------------------

#pragma once

#include "global.h"
#include "exept.h"
#include "handle.h"
#include "singleton.h"
#include "p2sysini.h"

namespace fo2base
{

typedef CHandle<P2INI_HANDLE, P2IniCustClose> CP2IniHandle;

//---------------------------------------------------------------------------
// class CP2Ini
/// Обеспечивает доступ к INI файлу.
//---------------------------------------------------------------------------
class CP2Ini : public CP2IniHandle
{
public:
    CP2Ini() : CP2IniHandle(), m_str("") {};
    virtual ~CP2Ini() {};

    static void SetOption(dword options);

    virtual void Open(const string& fname);
    void Close();
	const string& GetName() const;

    bool IsOpen() const;

    void WriteInt(const string& sect, const string& key, int val) const;
    void WriteStr(const string& sect, const string& key, const string& val) const;

    int    ReadInt(const string& sect, const string& key, int def) const;
    string ReadStr(const string& sect, const string& key, const string& def="") const;
	bool EnumSections(vector<string>& list);
	bool EnumKeys(map<string, string>& list, const string& section);

private:
    BAN_COPY_CONSTRUCTOR(CP2Ini);

protected:
	string m_name;
    mutable string  m_str;

};  // CP2Ini

typedef CSingletonHolder<CP2Ini>  CP2IniHolder;

inline CP2Ini* gGetCP2Ini()
{ 
	return CP2IniHolder::GetInstance(); 
}

//---------------------------------------------------------------------------
// inline functions
//---------------------------------------------------------------------------
inline bool CP2Ini::IsOpen() const
{
   return IsInitialized();
}

//---------------------------------------------------------------------------
// class CP2DefaultIni
/// Обеспечивает доступ к INI файлу по умолчанию.
//---------------------------------------------------------------------------
class CP2DefaultIni : public CP2Ini
{
public:
    CP2DefaultIni();
    virtual ~CP2DefaultIni();

    virtual void Open(const string& fname);

private:
    BAN_COPY_CONSTRUCTOR(CP2DefaultIni);

};  // CP2DefaultIni

typedef CSingletonHolder<CP2DefaultIni>  CP2DefaultIniHolder;

inline CP2DefaultIni* gGetDefaultIni()
{ 
	return CP2DefaultIniHolder::GetInstance(); 
}

} // namespace fo2base
