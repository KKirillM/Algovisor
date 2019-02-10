//----------------------------------------------------------------------------
// Copyright: (c) 2006 RTS, Russia
//----------------------------------------------------------------------------
// Created:	    10 Feb 2006   18:10
// File:        p2time
// Author:		Mikhail_Navernyuk
//----------------------------------------------------------------------------
// Description: Implementattion of the CP2Time class
//----------------------------------------------------------------------------

#include "stdafx.h"

#include "p2time.h"
#include "p2timespan.h"

#include <ctime>
#include <time.h>
#include <windows.h>

#if !defined(_WIN32)
#include <time.h>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/date_facet.hpp>
#include <boost/date_time/time_facet.hpp>
#include <sstream>
#include <string>
#endif

#include <stdint.h>

namespace fo2base
{

//---------------------------------------------------------------------------
CP2Time::CP2Time()
{
    padding = 0;
    Clear();
}

//---------------------------------------------------------------------------
CP2Time::CP2Time(word year, byte month, byte day, 
                 byte hour, byte minute, byte sec, word msec, dword nsec)
{
    assert(month <= 12 && month > 0);
    assert(day <= 31 && day > 0);
    assert(hour < 24);
    assert(minute < 60);
    assert(sec < 60);
    assert(msec < 100);
    assert(nsec < 1000000000);

    if(month > 12 || month <= 0)  throw CArgumentException("month");
    if(day > 31 || day <= 0)      throw CArgumentException("day");
    if(hour >= 24)                throw CArgumentException("hour");
    if(minute >= 60)              throw CArgumentException("min");
    if(sec >= 60)                 throw CArgumentException("sec");
    if(msec >= 100)               throw CArgumentException("msec");
    if(nsec >= 1000000000)        throw CArgumentException("nsec");

    padding = 0;
    SetYear(year);
    SetMonth(month);
    SetDay(day);
    SetHour(hour);
    SetMinute(minute);
    SetSecond(sec);
    SetMillisecond(msec);
    SetNanosecond(nsec);
}

//---------------------------------------------------------------------------
CP2Time::CP2Time(const CP2Time& other)
{
    padding = 0;
    Copy(other);
}

//---------------------------------------------------------------------------
CP2Time::CP2Time(const P2TIME& other)
{
    padding = 0;
    Copy(other);
}

//---------------------------------------------------------------------------
CP2Time& CP2Time::operator= (const CP2Time& other)
{
    Copy(other);
    return *this;
}

//---------------------------------------------------------------------------
CP2Time& CP2Time::operator= (const P2TIME& other)
{
    Copy(other);
    return *this;
}

//---------------------------------------------------------------------------
void CP2Time::Copy(const CP2Time& other)
{
    if(other.IsNull())
    {
        SetNull();
    }
    else
    {
        SetDateTime(other.GetYear(), other.GetMonth(), other.GetDay(), 
            other.GetHour(), other.GetMinute(), other.GetSecond(), 
            other.GetMillisecond(), other.GetNanosecond());
    }
}

//---------------------------------------------------------------------------
void CP2Time::Copy(const P2TIME& other)
{
    SetDateTime(other.year, other.month, other.day, 
        other.hour, other.minute, other.second, other.msec, other.nanoseconds);
}

/*
//---------------------------------------------------------------------------
void CP2Time::Attach(P2TIME* data)
{
    assert(data != NULL);
    if(data == NULL) throw CArgumentNullException();

    if((data->year != 0) ||
       (data->month != 0) ||
       (data->day != 0) ||
       (data->hour != 0) ||
       (data->minute != 0) ||
       (data->second != 0) ||
       (data->msec != 0))
    {
        assert(data->month <= 12 || data->month > 0);
        assert(data->day <= 31 || data->day > 0);
        assert(data->hour < 24);
        assert(data->minute < 60);
        assert(data->second < 60);

        if(data->month > 12 || data->month <= 0) throw CArgumentException("month");
        if(data->day > 31 || data->day <= 0)     throw CArgumentException("day");
        if(data->hour >= 24)                     throw CArgumentException("hour");
        if(data->minute >= 60)                   throw CArgumentException("min");
        if(data->second >= 60)                   throw CArgumentException("sec");
        if(data->msec >= 1000)                   throw CArgumentException("csec");
    }

    m_tm = data;
}

//---------------------------------------------------------------------------
P2TIME* CP2Time::Detach()
{
    P2TIME* ret;

    if(m_tm == &m_data)
    {
        ret = null;
    }
    else
    {
        ret = m_tm;
    }

    m_tm = &m_data;
    Clear();
    return ret;
}
*/

//---------------------------------------------------------------------------
const CP2Time& CP2Time::Today()
{
    static CP2Time  today;

    today.InitToday();
    return today;
}

//---------------------------------------------------------------------------
const CP2Time& CP2Time::Now()
{
    static CP2Time  now;

    now.InitNow();
    return now;
}

//---------------------------------------------------------------------------
void CP2Time::InitToday()
{
#if defined(_WIN32)
    SYSTEMTIME  sys;
    GetLocalTime(&sys);
    SetDateTime(sys.wYear, byte(sys.wMonth), byte(sys.wDay), 0, 0, 0, 0, 0);
#else
    SYSTEMTIME sys(boost::posix_time::microsec_clock::local_time());
    tm timef = boost::posix_time::to_tm(sys);
    SetDateTime(timef.tm_year,timef.tm_mon+1,timef.tm_mday,0,0,0,0,0);
#endif
}

//---------------------------------------------------------------------------
void CP2Time::InitNow()
{
#if defined(_WIN32)
    SYSTEMTIME  sys;
    GetLocalTime(&sys);
    SetDateTime(sys.wYear, byte(sys.wMonth), byte(sys.wDay), 
        byte(sys.wHour), byte(sys.wMinute), byte(sys.wSecond), 
        sys.wMilliseconds, 0);
#else
    SYSTEMTIME sys(boost::posix_time::microsec_clock::local_time());
    tm timef = boost::posix_time::to_tm(sys);
    SetDateTime(timef.tm_year,timef.tm_mon+1,timef.tm_mday,timef.tm_hour,
                timef.tm_min,timef.tm_sec,sys.time_of_day().total_milliseconds()%1000, 0);
#endif
}

//---------------------------------------------------------------------------
int CP2Time::Compare(const CP2Time& other, Precision p) const
{
    int cmp;

    cmp = (int)GetYear() - (int)other.GetYear();
    if(cmp)
        return cmp;

    cmp = (int)GetMonth() - (int)other.GetMonth();
    if(cmp)
        return cmp;

    cmp = (int)GetDay() - (int)other.GetDay();
    if(cmp)
        return cmp;

    if(p == dtpDay)
        return 0;

    cmp = (int)GetHour() - (int)other.GetHour();
    if(cmp)
        return cmp;

    if(p == dtpHour)
        return 0;

    cmp = (int)GetMinute() - (int)other.GetMinute();
    if(cmp)
        return cmp;

    if(p == dtpMinute)
        return 0;

    cmp = (int)GetSecond() - (int)other.GetSecond();
    if(cmp)
        return cmp;

    if(p == dtpSecond)
        return 0;

    cmp = (int)GetMillisecond() - (int)other.GetMillisecond();
    return cmp;
}

int CP2Time::CompareTime(const CP2Time& other, Precision p /*= dtpNanosecond*/) const
{
    int cmp;

    cmp = (int)GetHour() - (int)other.GetHour();
    if(cmp)
        return cmp;

    if(p == dtpHour)
        return 0;

    cmp = (int)GetMinute() - (int)other.GetMinute();
    if(cmp)
        return cmp;

    if(p == dtpMinute)
        return 0;

    cmp = (int)GetSecond() - (int)other.GetSecond();
    if(cmp)
        return cmp;

    if(p == dtpSecond)
        return 0;

    cmp = (int)GetMillisecond() - (int)other.GetMillisecond();
    if(cmp)
        return cmp;

    if(p == dtpMillisecond)
        return 0;

    cmp = (int)GetNanosecond() - (int)other.GetNanosecond();
    return cmp;
}

//---------------------------------------------------------------------------
static int rts_IsLeapYear(int year)
{
    if(year % 4 == 0)
    {
        if(year % 100 == 0)
        {
            if(year % 400 == 0)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 0;
    }
}

static const int _nDays[ 12 ] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
#define rts_MonthDays(__month__, __year__) (_nDays[ (__month__) - 1 ] + ((__month__) == 2 && rts_IsLeapYear(__year__)))

//---------------------------------------------------------------------------
CP2TimeSpan CP2Time::operator- (const CP2Time& other) const
{
    const CP2Time& a = *this;
    const CP2Time& b = other;
    word a_year  = a.GetYear();
    word b_year  = b.GetYear();
    byte a_month = a.GetMonth();
    byte b_month = b.GetMonth();
    int64_t diff = 0;

    while(a_year > b_year)
    {
        diff += 365;
        if(b_month <= 2)
            diff += rts_IsLeapYear(b_year);
        else
            diff += rts_IsLeapYear(b_year + 1);
        ++b_year;
    }

    while(a_year < b_year)
    {
        diff -= 365;
        if(a_month <= 2)
            diff -= rts_IsLeapYear(a_year);
        else
            diff -= rts_IsLeapYear(a_year + 1);
        ++a_year;
    }
    assert(a_year == b_year);

    while(a_month > b_month)
    {
        diff += rts_MonthDays(b_month, b_year);
        ++b_month;
    }

    while(a_month < b_month)
    {
        diff -= rts_MonthDays(a_month, a_year);
        ++a_month;
    }
    assert(a_month == b_month);

    diff += a.GetDay() - b.GetDay();
    diff *= 24;

    diff += a.GetHour() - b.GetHour();
    diff *= 60;

    diff += a.GetMinute() - b.GetMinute();
    diff *= 60;

    diff += a.GetSecond() - b.GetSecond();
    diff *= 1000;

    diff += a.GetMillisecond() - b.GetMillisecond();

    return CP2TimeSpan(diff);
}


//---------------------------------------------------------------------------
void CP2Time::Add(Precision p, int val)
{
    int i;
    int carry = val;

    switch(p)
    {
    case dtpMillisecond:
        i = GetMillisecond() + carry;
        if(i < 0)
        {
            carry = - (-i + 999) / 1000;
            i -= carry * 1000;
        }
        else if(i >= 1000)
        {
            carry = i / 1000;
            i -= carry * 1000;
        }
        else
        {
            SetMillisecond((word)i);
            return;
        }
        SetMillisecond((word)i);
        //no break
    case dtpSecond:
        i = GetSecond() + carry;
        if(i < 0)
        {
            carry = - (-i + 59) / 60;
            i -= carry * 60;
        }
        else if(i >= 60)
        {
            carry = i / 60;
            i -= carry * 60;
        }
        else
        {
            SetSecond((byte)i);
            return;
        }
        SetSecond((byte)i);
        //no break
    case dtpMinute:
        i = GetMinute() + carry;
        if(i < 0)
        {
            carry = - (-i + 59) / 60;
            i -= carry * 60;
        }
        else if(i >= 60)
        {
            carry = i / 60;
            i -= carry * 60;
        }
        else
        {
            SetMinute((byte)i);
            return;
        }
        SetMinute((byte)i);
        //no break
    case dtpHour:
        i = GetHour() + carry;
        if(i < 0)
        {
            carry = - (-i + 23) / 24;
            i -= carry * 24;
        }
        else if(i >= 24)
        {
            carry = i / 24;
            i -= carry * 24;
        }
        else
        {
            SetHour((byte)i);
            return;
        }
        SetHour((byte)i);
        //no break
    case dtpDay:
        i = GetDay() + carry;
        while(i < 1)
        {
            if(GetMonth() == 1)
            {
                SetMonth(12);
                DecYear();
            }
            else
            {
                DecMonth();
            }

            i += rts_MonthDays(GetMonth(), GetYear());
        }

        {   
            int mDays = rts_MonthDays(GetMonth(), GetYear());
            while(i > mDays)
            {
                i -= mDays;

                if(GetMonth() == 12)
                {
                    SetMonth(1);
                    IncYear();
                }
                else
                {
                    IncMonth();
                }

                mDays = rts_MonthDays(GetMonth(), GetYear());
            }
        }

        SetDay((byte)i);
        break;
    }
}

//---------------------------------------------------------------------------
void CP2Time::Add(const CP2TimeSpan& span)
{
    Add(dtpDay,         int(span.GetDays()));
    Add(dtpHour,        span.GetHours());
    Add(dtpMinute,      span.GetMinutes());
    Add(dtpSecond,      span.GetSeconds());
    Add(dtpMillisecond, span.GetMilliseconds());
}

//---------------------------------------------------------------------------
void CP2Time::Subtract(const CP2TimeSpan& span)
{
    Add(dtpDay,         -int(span.GetDays()));
    Add(dtpHour,        -span.GetHours());
    Add(dtpMinute,      -span.GetMinutes());
    Add(dtpSecond,      -span.GetSeconds());
    Add(dtpMillisecond, -span.GetMilliseconds());
}

//---------------------------------------------------------------------------
string CP2Time::ToString(const string& fmt) const
{
    stringstream    s;
    size_t          i;
    size_t          n;
    width_action    action;
    int             val;

    n = fmt.length();
    for(i = 0; i < n; i++)
    {
        if(fmt[ i ] == '%')
        {
            i++;
            if(fmt[ i ] != '#')
            {
                action = waSET;
            }
            else
            {
                i++;
                action = waRESET;
            }

            switch(fmt[ i ])
            {
            case 'c': 
                if(action == waSET)
                {
                    s << ToDateTimeString();   
                }
                else
                {
                    s << ToLongDateString();   
                    s << " ";   
                    s << ToTimeString();   
                }
                break;
            case 'd': 
                SetOutputWidth(action, s, 2);
                s << (int)GetDay();   
                break;
            case 'H': 
                SetOutputWidth(action, s, 2);
                s << (int)GetHour();  
                break;
            case 'I':
                SetOutputWidth(action, s, 2);
                val = (int)(GetHour() % 12);
                if(val == 0) val = 12;
                s << val;
                break;
            case 'm': 
                SetOutputWidth(action, s, 2);
                s << (int)GetMonth();  
                break;
            case 'M': 
                SetOutputWidth(action, s, 2);
                s << (int)GetMinute(); 
                break;
            case 'S': 
                SetOutputWidth(action, s, 2);
                s << (int)GetSecond(); 
                break;
            case 'x': 
                if(action == waSET)
                {
                    s << ToShortDateString();   
                }
                else
                {
                    s << ToLongDateString();   
                }
                break;
            case 'X': 
                s << ToTimeString();   
                break;
            case 'y': 
                SetOutputWidth(action, s, 2);
                s << (int)GetYear() % 100; 
                break;
            case 'Y': 
                SetOutputWidth(action, s, 4);
                s << (int)GetYear();   
                break;
            case '%': 
                SetOutputWidth(action, s, 1);
                s << '%';
                break;
            default: 
                assert(false);          
                throw CArgumentException("Unsupported format string.");
                //break;
            }
        }
        else
        {
            s << fmt[ i ];
        }
    }

    return s.str();
}

//---------------------------------------------------------------------------
//    exapmle: st2.parse("%Y/%m/%d %H:%M:%S", "200/5/6 1:2:3");
//---------------------------------------------------------------------------
void CP2Time::Parse(const string& fmt, const string& str)
{
    char        buf[5];
    const char* p;
    size_t      i;
    size_t      s;
    size_t      b;
    size_t      lim;    // limit
    int         val;

    word aYear  = 0; 
    byte aMonth = 1; 
    byte aDay   = 1; 
    byte aHour  = 0; 
    byte aMin   = 0; 
    byte aSec   = 0; 
    byte aCsec  = 0;
    dword aNsec = 0;

    Clear();

    for(i = 0, s = 0; i < fmt.length() && s < str.length(); i++, s++)
    {
        if(fmt[ i ] == '%')
        {
            i++;
            lim = 2;
            if(fmt[ i ] == '%')
            {
                break;
            }
            else if(fmt[ i ] == 'Y')
            {
                lim = 4;
            }

            b = 0;
            p = str.c_str() + s;
            while(isdigit(*p) && b < lim)
            {
                buf[ b ] = *p;
                p++;
                b++;
            }
            buf[ b ] = '\0';

            val = atoi(buf);
            s += b - 1;

            switch(fmt[ i ])
            {
            case 'd': 
                aDay = static_cast< byte >(val);   
                break;
            case 'H': 
                aHour = static_cast< byte >(val);
                break;
            case 'm': 
                aMonth = static_cast< byte >(val);
                break;
            case 'M': 
                aMin = static_cast< byte >(val);
                break;
            case 'S': 
                aSec = static_cast< byte >(val);
                break;
            case 'y': 
                aYear = static_cast< word >((val > 70) ? 1900 + val : 2000 + val);
                break;
            case 'Y': 
                aYear = static_cast< word >(val);
                break;
            default: 
                assert(false);       
                throw CArgumentException("Unsupported format string.");
                //break;
            }
        }
        else
        {
            if(fmt[ i ] != str[ s ])
            {
                assert(false);          
                throw CArgumentException("Parameter mismatch (wrong format of input string).");
            }
        }
    }

    if(aYear == 0 && aMonth == 0 && aDay == 0 && aHour == 0 && aMin == 0 && aSec == 0 && aCsec == 0 && aNsec == 0)
    {
        SetNull();
    }
    else
    {
        SetDateTime(aYear, aMonth, aDay, aHour, aMin, aSec, aCsec, aNsec);
    }
}

//---------------------------------------------------------------------------
void CP2Time::CopyToSystemtime(SYSTEMTIME& sys) const
{
#if defined(_WIN32)
    sys.wYear         = GetYear();
    sys.wMonth        = GetMonth();
    sys.wDayOfWeek    = 0;
    sys.wDay          = GetDay();
    sys.wHour         = GetHour();
    sys.wMinute       = GetMinute();
    sys.wSecond       = GetSecond();
    sys.wMilliseconds = GetMillisecond();
#else
    using namespace boost::posix_time;
    using namespace boost::gregorian;
    sys = ptime(date(GetYear(),GetMonth(),GetDay()),
                hours(GetHour())+minutes(GetMinute())+
                seconds(GetSecond())+milliseconds(GetMillisecond()));
#endif
}

//---------------------------------------------------------------------------
string CP2Time::ToDateTimeString() const
{
    return ToShortDateString() + " " + ToTimeString();
}

//---------------------------------------------------------------------------
string CP2Time::ToLongDateString() const
{
    SYSTEMTIME  sys;
    TCHAR       buf[128];

    CopyToSystemtime(sys);
#if defined(_WIN32)
    if(GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, 
        &sys, NULL, buf, 128))
    {
        return buf;
    }
    else
    {
        assert(false);
        return "";
    }
#else
    static std::locale special_locale(std::locale(),new boost::gregorian::date_facet("%A %B %d, %Y"));
    std::stringstream ss;
    ss.imbue(special_locale);
    ss<<sys.date();
    return ss.str();
#endif
}

//---------------------------------------------------------------------------
string CP2Time::ToShortDateString() const
{
#if defined(_WIN32)
    SYSTEMTIME  sys;
    TCHAR       buf[128];

    CopyToSystemtime(sys);
    if(GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, 
        &sys, NULL, buf, 128))
    {
        return buf;
    }
    else
    {
        assert(false);
        return "";
    }
#else
    return std::to_string(GetYear())+'-'+std::to_string(GetMonth())+'-'+std::to_string(GetDay());
#endif
}

//---------------------------------------------------------------------------
string CP2Time::ToTimeString() const
{
#if defined(_WIN32)
    SYSTEMTIME  sys;
    TCHAR       buf[128];

    CopyToSystemtime(sys);
    if(GetTimeFormat(LOCALE_USER_DEFAULT, 0, 
        &sys, NULL, buf, 128))
    {
        return buf;
    }
    else
    {
        assert(false);
        return "";
    }
#else
    return std::to_string(GetHour())+':'+std::to_string(GetMinute())+':'+
            std::to_string(GetSecond())+'.'+std::to_string(GetMillisecond());
#endif
}

/// @brief преобразовать UNC Unix Nanosec время в CP2Time Local Time с учетом часового пояса.
P2TIME CP2Time::FromUnixNanosecondsTime(const UnixNanosecondsTime value)
{
    P2TIME res;
    uint64_t nmec = value % std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();
    std::time_t tt = value / std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();
    std::tm tm;
    gmtime_s(&tm, &tt);
    res.msec = static_cast<UINT16>
        ((value % std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count())
            / std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::milliseconds(1)).count());
    res.second = static_cast<UINT8>(tm.tm_sec);
    res.minute = static_cast<UINT8>(tm.tm_min);
    res.hour = static_cast<UINT8>(tm.tm_hour);
    res.day = static_cast<UINT8>(tm.tm_mday);
    res.month = static_cast<UINT8>(tm.tm_mon) + 1;
    res.year = static_cast<UINT16>(tm.tm_year) + 1900;
    res.padding = 0;
    res.nanoseconds = static_cast<UINT32>(nmec);
    return res;
}

/// @brief печать в строку UNC Unix Nanosec время (log_serializer)
void CP2Time::FromUnixNanosecondsToString(UnixNanosecondsTime value, char* res, size_t buflen)
{
    std::time_t tt = value / std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();
    std::tm tm;
    gmtime_s(&tm, &tt);

    int64_t nmec = value % std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();
    int64_t datatime =
         ((uint64_t)(tm.tm_sec)
        + (uint64_t)(tm.tm_min)         * 100
        + (uint64_t)(tm.tm_hour)        * 100 * 100
        + (uint64_t)(tm.tm_mday)        * 100 * 100 * 100
        + (uint64_t)(tm.tm_mon + 1)     * 100 * 100 * 100 * 100
        + (uint64_t)(tm.tm_year + 1900) * 100 * 100 * 100 * 100 * 100);
    sprintf_s(res, buflen, "%14.14I64u.%09.9I64u", datatime, nmec);
}

/// @brief печать в строку UNC Unix Nanosec время (log_serializer)
void CP2Time::FromUnixNanosecondsToString2(UnixNanosecondsTime value, std::string& output)
{
    std::time_t tt = value / std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();
    std::tm tm;
    gmtime_s(&tm, &tt);

    int64_t nmec = value % std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();

    char buf[100];
    sprintf_s(buf, sizeof(buf), "%4u-%02u-%02u %02u:%02u:%02u.%09.9I64u", 
        (tm.tm_year + 1900), (tm.tm_mon + 1), (tm.tm_mday),
        (tm.tm_hour), (tm.tm_min), (tm.tm_sec), nmec);

    output.append(buf);
}

void CP2Time::FromUnixNanosecondsToString3(UnixNanosecondsTime value, std::string& output)
{
	std::time_t tt = value / std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();
	std::tm tm;
	gmtime_s(&tm, &tt);

	int64_t nmec = value % std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();

	char buf[100];
	sprintf_s(buf, sizeof(buf), "%02u:%02u:%02u.%09.9I64u",	(tm.tm_hour), (tm.tm_min), (tm.tm_sec), nmec);

	output.append(buf);
}

std::string CP2Time::FromUnixNanosecondsToDateString(const UnixNanosecondsTime value, const char separator)
{
    fo2base::CP2Time dateTime(fo2base::CP2Time::FromUnixNanosecondsTime(value));
    std::string dateString = dateTime.ToString("%x");
    std::replace(dateString.begin(), dateString.end(), '.', separator);
    return dateString;
}

// @brief преобразовать P2Time локальное время в UNC Unix Nanosecond
UnixNanosecondsTime CP2Time::ToUnixNanosecondsTime(const P2TIME& value)
{
    std::tm tm;
    tm.tm_sec = value.second;     // seconds after the minute - [0, 60] including leap second
    tm.tm_min = value.minute;     // minutes after the hour - [0, 59]
    tm.tm_hour = value.hour;      // hours since midnight - [0, 23]
    tm.tm_mday = value.day;       // day of the month - [1, 31]
    tm.tm_mon = value.month - 1;      // months since January - [0, 11]
    tm.tm_year = value.year - 1900; // years since 1900
    tm.tm_wday = 0;  // days since Sunday - [0, 6]
    tm.tm_yday = 0;  // days since January 1 - [0, 365]
    tm.tm_isdst = -1;

    std::time_t tt = mktime(&tm);
    std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(tt);

    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        tp.time_since_epoch()
        + timezoneShift.getMinute()
        + std::chrono::milliseconds(value.msec)
        ).count()
        - std::chrono::duration_cast<std::chrono::nanoseconds>(timezoneShift.getMinute()).count();
}


UnixNanosecondsTime NowUnixNanosecondsTime()
{
    auto tmp = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(tmp.time_since_epoch()).count();
    //return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch() + timezoneShift.get()).count();
}

UnixNanosecondsTime NowHighResolutionUnixNanosecondsTime()
{
	std::chrono::steady_clock::time_point tmp = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(tmp.time_since_epoch()).count();
}

} // namespace fo2base

