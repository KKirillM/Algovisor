//----------------------------------------------------------------------------
// Copyright: (c) 2006 RTS, Russia
//----------------------------------------------------------------------------
// Created:	    10 Feb 2006   18:08
// File:        p2time
// Author:		Mikhail_Navernyuk
//----------------------------------------------------------------------------
// Description: Interface of the CP2TimeSpan class
//----------------------------------------------------------------------------

#pragma once

#include "p2time.h"

namespace fo2base
{

//---------------------------------------------------------------------------
// class CP2TimeSpan
//---------------------------------------------------------------------------
class CP2TimeSpan
{
public:
    CP2TimeSpan();
    CP2TimeSpan(dword day, byte hour = 0, byte minute = 0, byte sec = 0, 
        word msec = 0);
    CP2TimeSpan(const CP2TimeSpan& other);
    CP2TimeSpan(int64_t msec);
    virtual ~CP2TimeSpan();

    CP2TimeSpan& operator= (const CP2TimeSpan& other);

    bool operator== (const CP2TimeSpan& other) const;
    bool operator!= (const CP2TimeSpan& other) const;
    bool operator< (const CP2TimeSpan& other) const;
    bool operator<= (const CP2TimeSpan& other) const;
    bool operator> (const CP2TimeSpan& other) const;
    bool operator>= (const CP2TimeSpan& other) const;

    const CP2TimeSpan& operator-= (const CP2TimeSpan& other);
    const CP2TimeSpan& operator+= (const CP2TimeSpan& other);
    const CP2TimeSpan& operator- ();
    CP2TimeSpan operator- (const CP2TimeSpan& other) const;
    CP2TimeSpan operator+ (const CP2TimeSpan& other) const;

    string ToString(const string& fmt) const; // for format see doc on strftime
    string ToString() const;

    long GetDays() const;
    byte GetHours() const;
    byte GetMinutes() const;
    byte GetSeconds() const;
    word GetMilliseconds() const;

    long GetTotalHours() const;
    int64_t GetTotalMinutes() const;
    int64_t GetTotalSeconds() const;
    int64_t GetTotalFractions() const;

protected:
    int64_t     m_span;

//    friend class CP2TimeSpan;

};  // CP2TimeSpan

//---------------------------------------------------------------------------
// inline functions
//---------------------------------------------------------------------------
inline CP2TimeSpan::CP2TimeSpan()
{
    m_span = 0;
}

//---------------------------------------------------------------------------
inline CP2TimeSpan::CP2TimeSpan(dword day, byte hour, byte minute, byte sec, word msec)
{
    m_span = ((((int64_t(day) * 24) + hour) * 60 + minute) * 60 + sec) * 1000 + msec;
}

//---------------------------------------------------------------------------
inline CP2TimeSpan::CP2TimeSpan(const CP2TimeSpan& other)
{
    m_span = other.m_span;
}

//---------------------------------------------------------------------------
inline CP2TimeSpan::CP2TimeSpan(int64_t msec)
{
    m_span = msec;
}

//---------------------------------------------------------------------------
inline CP2TimeSpan::~CP2TimeSpan()
{
}

//---------------------------------------------------------------------------
inline CP2TimeSpan& CP2TimeSpan::operator= (const CP2TimeSpan& other)
{
    m_span = other.m_span;
    return *this;
}

//---------------------------------------------------------------------------
inline bool CP2TimeSpan::operator== (const CP2TimeSpan& other) const
{
    return (m_span == other.m_span);
}

//---------------------------------------------------------------------------
inline bool CP2TimeSpan::operator!= (const CP2TimeSpan& other) const
{
    return (m_span != other.m_span);
}

//---------------------------------------------------------------------------
inline bool CP2TimeSpan::operator< (const CP2TimeSpan& other) const
{
    return (m_span < other.m_span);
}

//---------------------------------------------------------------------------
inline bool CP2TimeSpan::operator<= (const CP2TimeSpan& other) const
{
    return (m_span <= other.m_span);
}

//---------------------------------------------------------------------------
inline bool CP2TimeSpan::operator> (const CP2TimeSpan& other) const
{
    return (m_span > other.m_span);
}

//---------------------------------------------------------------------------
inline bool CP2TimeSpan::operator>= (const CP2TimeSpan& other) const
{
    return (m_span >= other.m_span);
}

//---------------------------------------------------------------------------
inline const CP2TimeSpan& CP2TimeSpan::operator-= (const CP2TimeSpan& other)
{
    m_span -= other.m_span;
    return *this;
}

//---------------------------------------------------------------------------
inline const CP2TimeSpan& CP2TimeSpan::operator+= (const CP2TimeSpan& other)
{
    m_span += other.m_span;
    return *this;
}

//---------------------------------------------------------------------------
inline const CP2TimeSpan& CP2TimeSpan::operator- ()
{
    m_span = -m_span;
    return *this;
}

//---------------------------------------------------------------------------
inline CP2TimeSpan CP2TimeSpan::operator- (const CP2TimeSpan& other) const
{
    CP2TimeSpan   ret;
    ret.m_span = m_span - other.m_span;
    return ret;
}

//---------------------------------------------------------------------------
inline CP2TimeSpan CP2TimeSpan::operator+ (const CP2TimeSpan& other) const
{
    CP2TimeSpan   ret;
    ret.m_span = m_span + other.m_span;
    return ret;
}

//---------------------------------------------------------------------------
inline string CP2TimeSpan::ToString() const
{
    return ToString("%D дней, %H часов, %M минут и %S секунд");
}

//---------------------------------------------------------------------------
inline long CP2TimeSpan::GetDays() const
{
    return GetTotalHours() / 24;
}

//---------------------------------------------------------------------------
inline byte CP2TimeSpan::GetHours() const
{
    return static_cast< byte >(GetTotalHours() % 24);
}

//---------------------------------------------------------------------------
inline byte CP2TimeSpan::GetMinutes() const
{
    return static_cast< byte >(GetTotalMinutes() - GetTotalHours() * 60);
}

//---------------------------------------------------------------------------
inline byte CP2TimeSpan::GetSeconds() const
{
    return static_cast< byte >(GetTotalSeconds() - GetTotalMinutes() * 60);
}

//---------------------------------------------------------------------------
inline word CP2TimeSpan::GetMilliseconds() const
{
    return static_cast< word >(m_span % 1000);
}

//---------------------------------------------------------------------------
inline long CP2TimeSpan::GetTotalHours() const
{
    return static_cast< long >(m_span / (1000 * 60 * 60));
}

//---------------------------------------------------------------------------
inline int64_t CP2TimeSpan::GetTotalMinutes() const
{
    return m_span / (1000 * 60);
}

//---------------------------------------------------------------------------
inline int64_t CP2TimeSpan::GetTotalSeconds() const
{
    return m_span / 1000;
}

//---------------------------------------------------------------------------
inline int64_t CP2TimeSpan::GetTotalFractions() const
{
    return m_span;
}

} // namespace fo2base
