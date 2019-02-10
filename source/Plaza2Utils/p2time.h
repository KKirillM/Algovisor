//----------------------------------------------------------------------------
// Copyright: (c) 2006 RTS, Russia
//----------------------------------------------------------------------------
// Created:	    10 Feb 2006   18:08
// File:        p2time
// Author:		Mikhail_Navernyuk
//----------------------------------------------------------------------------
// Description: Interface of the CP2Time class
//----------------------------------------------------------------------------

#pragma once

#include "global.h"
#include "exept.h"

#include "p2systypes.h"

#if !defined(_WIN32)
#include <boost/date_time.hpp>
typedef boost::posix_time::ptime SYSTEMTIME;
#endif

#include <chrono>

namespace fo2base
{

typedef uint64_t UnixNanosecondsTime;
UnixNanosecondsTime NowUnixNanosecondsTime();
UnixNanosecondsTime NowHighResolutionUnixNanosecondsTime();

//---------------------------------------------------------------------------
// class TimeZoneShift
//---------------------------------------------------------------------------
class TimeZoneShift
{
    std::chrono::minutes shift;
public:

    TimeZoneShift()
    {
        TIME_ZONE_INFORMATION tz_info;
        GetTimeZoneInformation(&tz_info);
        shift = std::chrono::minutes(-tz_info.Bias);
    }

    std::chrono::minutes getMinute() const
    {
        return shift;
    }

    __int64 get() const
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(shift).count();
    }
};

static TimeZoneShift timezoneShift;

class CP2TimeSpan;

//---------------------------------------------------------------------------
// class CP2Time
//---------------------------------------------------------------------------
class CP2Time : public P2TIME
{
public:
    enum Precision
    {
        dtpDay,
        dtpHour,
        dtpMinute,
        dtpSecond,
        dtpMillisecond,
        dtpNanosecond
    };

    time_t GetCTime(const CP2Time* p2t = NULL)
    {
        if (p2t == NULL)
            p2t = this;
        time_t r = 0;
        if (p2t->year > 2000)
        {
            struct tm t;
            t.tm_sec = p2t->second;
            t.tm_min = p2t->minute;
            t.tm_hour = p2t->hour;
            t.tm_mday = p2t->day;
            t.tm_mon = p2t->month - 1;
            t.tm_year = p2t->year - 1900;
            r = mktime(&t);
        }
        return r;
	};
	void   SetTimeOfADay(const CP2Time& o)
	{
		hour = o.hour;
		minute = o.minute;
		second = o.second;
		msec = o.msec;
	}


    CP2Time();
    CP2Time(word year, byte month, byte day, 
        byte hour = 0, byte minute = 0, byte sec = 0, 
        word msec = 0, dword nanosec = 0);
    CP2Time(const CP2Time& other);
    explicit CP2Time(const P2TIME& other);

    CP2Time& operator= (const CP2Time& other);
    CP2Time& operator= (const P2TIME& other);

    static const CP2Time& Today();
    static const CP2Time& Now();

    void Copy(const CP2Time& other);
    void Copy(const P2TIME& other);

    bool IsNull() const;
    void SetNull();

    word GetYear() const;
    byte GetMonth() const;
    byte GetDay() const;
    byte GetHour() const;
    byte GetMinute() const;
    byte GetSecond() const;
    word GetMillisecond() const;
    dword GetNanosecond() const;

    void SetDateTime(word year, byte month, byte day, byte hour, byte minute, byte sec, word msec, dword nsec);

    int  Compare(const CP2Time& other, Precision p = dtpMillisecond) const;
    int  CompareTime(const CP2Time& other, Precision p = dtpNanosecond) const;
    int  CmpDay(const CP2Time& other) const;
    int  CmpHour(const CP2Time& other) const;
    int  CmpMinute(const CP2Time& other) const;
    int  CmpSecond(const CP2Time& other) const;

    void Add(Precision p, int val);
    void AddDay(int val);
    void AddHour(int val);
    void AddMinute(int val);
    void AddSecond(int val);

    void Add(const CP2TimeSpan& span);
    void Subtract(const CP2TimeSpan& span);

    CP2Time& operator+= (const CP2TimeSpan& span);
    CP2Time& operator-= (const CP2TimeSpan& span);

    CP2TimeSpan operator- (const CP2Time& other) const;

    string ToString(const string& fmt) const; // for format see doc on strftime
    string ToString() const;
    void Parse(const string& fmt, const string& str);

    void Clear();

    int ToInt32() const
    {
        return (unsigned int)day + (unsigned int)month * 100  + (unsigned int)year * 100 * 100;

    };
    static P2TIME FromInt32(unsigned int value)
    {
        uint64_t value_ = (uint64_t)value;
        P2TIME res;
        res.padding = 0;
        res.msec = 0;
        res.second = 0;
        res.minute = 0;
        res.hour = 0;
        res.day = value_ % 100;
        value_ /= 100;
        res.month = value_ % 100;
        value_ /= 100;
        res.year = (short)value_;
        return res;
    }

    static int64_t ToInt64(const P2TIME& value)
    {
        return (int64_t)((uint64_t)value.msec + (uint64_t)value.second * 1000 + (uint64_t)value.minute * 1000 * 100 +
            (uint64_t)value.hour * 1000 * 100 * 100 + (uint64_t)value.day *  1000 * 100 * 100 * 100 +
            (uint64_t)value.month * 1000 * 100 * 100 * 100 * 100 + (uint64_t)value.year * 1000 * 100 * 100 * 100 * 100 * 100);
    }
    static P2TIME FromInt64(uint64_t value)
    {
        uint64_t value_ = (uint64_t)value;
        P2TIME res;
        res.padding = 0;
        res.msec = ((uint64_t)value_) % 1000;
        value_ /= 1000;
        res.second = value_ % 100;
        value_ /= 100;
        res.minute = value_ % 100;
        value_ /= 100;
        res.hour = value_ % 100;
        value_ /= 100;
        res.day = value_ % 100;
        value_ /= 100;
        res.month = value_ % 100;
        value_ /= 100;
        res.year = (short)value_;
        return res;
    }

    int64_t ToInt64() const
    {
        return ToInt64(*this);
    }

    static UnixNanosecondsTime ToUnixNanosecondsTime(const P2TIME& value);

    static P2TIME FromUnixNanosecondsTime(const UnixNanosecondsTime value);
    static void FromUnixNanosecondsToString(UnixNanosecondsTime value, char* res, size_t buflen);
    static void FromUnixNanosecondsToString2(UnixNanosecondsTime value, std::string& output);
	static void FromUnixNanosecondsToString3(UnixNanosecondsTime value, std::string& output);
    static std::string FromUnixNanosecondsToDateString(const UnixNanosecondsTime value, const char separator);

    UnixNanosecondsTime ToUnixNanosecondsTime() const
    {
        return ToUnixNanosecondsTime(*this);
    }

private:
    enum width_action
    {
        waSET,
        waRESET
    };

private:
    void InitToday();
    void InitNow();

    void SetYear(word y);
    void SetMonth(byte m);
    void SetDay(byte d);
    void SetHour(byte h);
    void SetMinute(byte m);
    void SetSecond(byte s);
    void SetMillisecond(word m);
    void SetNanosecond(dword n);

    void DecYear();
    void DecMonth();
    void IncYear();
    void IncMonth();

    void   CopyToSystemtime(SYSTEMTIME& sys) const;
    string ToDateTimeString() const;
    string ToLongDateString() const;
    string ToShortDateString() const;
    string ToTimeString() const;

    void  SetOutputWidth( width_action a, stringstream& s, int width ) const;

private:

    friend bool operator== (const CP2Time& a, const CP2Time& b);
    friend bool operator!= (const CP2Time& a, const CP2Time& b);
    friend bool operator< (const CP2Time& a, const CP2Time& b);
    friend bool operator<= (const CP2Time& a, const CP2Time& b);
    friend bool operator> (const CP2Time& a, const CP2Time& b);
    friend bool operator>= (const CP2Time& a, const CP2Time& b);

};  // CP2Time

//---------------------------------------------------------------------------
inline bool operator== (const CP2Time& a, const CP2Time& b)
{
    return (a.Compare(b) == 0);
}

//---------------------------------------------------------------------------
inline bool operator!= (const CP2Time& a, const CP2Time& b)
{
    return (a.Compare(b) != 0);
}

//---------------------------------------------------------------------------
inline bool operator< (const CP2Time& a, const CP2Time& b)
{
    return (a.Compare(b) < 0);
}

//---------------------------------------------------------------------------
inline bool operator<= (const CP2Time& a, const CP2Time& b)
{
    return (a.Compare(b) <= 0);
}

//---------------------------------------------------------------------------
inline bool operator> (const CP2Time& a, const CP2Time& b)
{
    return (a.Compare(b) > 0);
}

//---------------------------------------------------------------------------
inline bool operator>= (const CP2Time& a, const CP2Time& b)
{
    return (a.Compare(b) >= 0);
}


//---------------------------------------------------------------------------
// inline functions
//---------------------------------------------------------------------------
inline int CP2Time::CmpDay(const CP2Time& other) const
{
    return Compare(other, dtpDay);
}

//---------------------------------------------------------------------------
inline int CP2Time::CmpHour(const CP2Time& other) const
{
    return Compare(other, dtpHour);
}

//---------------------------------------------------------------------------
inline int CP2Time::CmpMinute(const CP2Time& other) const
{
    return Compare(other, dtpMinute);
}

//---------------------------------------------------------------------------
inline int CP2Time::CmpSecond(const CP2Time& other) const
{
    return Compare(other, dtpSecond);
}
/*
//---------------------------------------------------------------------------
inline bool CP2Time::operator== (const CP2Time& other) const
{
    return (Compare(other) == 0);
}

//---------------------------------------------------------------------------
inline bool CP2Time::operator!= (const CP2Time& other) const
{
    return (Compare(other) != 0);
}

//---------------------------------------------------------------------------
inline bool CP2Time::operator< (const CP2Time& other) const
{
    return (Compare(other) < 0);
}

//---------------------------------------------------------------------------
inline bool CP2Time::operator<= (const CP2Time& other) const
{
    return (Compare(other) <= 0);
}

//---------------------------------------------------------------------------
inline bool CP2Time::operator> (const CP2Time& other) const
{
    return (Compare(other) > 0);
}

//---------------------------------------------------------------------------
inline bool CP2Time::operator>= (const CP2Time& other) const
{
    return (Compare(other) >= 0);
}
*/
//---------------------------------------------------------------------------
inline word CP2Time::GetYear() const
{
    return year;
}

//---------------------------------------------------------------------------
inline byte CP2Time::GetMonth() const
{
    return month;
}

//---------------------------------------------------------------------------
inline byte CP2Time::GetDay() const
{
    return day;
}

//---------------------------------------------------------------------------
inline byte CP2Time::GetHour() const
{
    return hour;
}

//---------------------------------------------------------------------------
inline byte CP2Time::GetMinute() const
{
    return minute;
}

//---------------------------------------------------------------------------
inline byte CP2Time::GetSecond() const
{
    return second;
}

//---------------------------------------------------------------------------
inline word CP2Time::GetMillisecond() const
{
    return msec;
}

//---------------------------------------------------------------------------
inline dword CP2Time::GetNanosecond() const
{
    return nanoseconds;
}

//---------------------------------------------------------------------------
inline void CP2Time::SetYear(word y)
{
    year = y;
}

//---------------------------------------------------------------------------
inline void CP2Time::SetMonth(byte m)
{
    assert(m <= 12);
    if(m > 12) 
        throw CArgumentException("month");
    month = m;
}

//---------------------------------------------------------------------------
inline void CP2Time::SetDay(byte d)
{
    assert(d <= 31);
    if(d > 31) 
        throw CArgumentException("day");
    day = d;
}

//---------------------------------------------------------------------------
inline void CP2Time::SetHour(byte h)
{
    assert(h < 24);
    if(h >= 24)
        throw CArgumentException("hour");
    hour = h;
}

//---------------------------------------------------------------------------
inline void CP2Time::SetMinute(byte m)
{
    assert(m < 60);
    if(m >= 60)
        throw CArgumentException("min");
    minute = m;
}

//---------------------------------------------------------------------------
inline void CP2Time::SetSecond(byte s)
{
    assert(s < 60);
    if(s >= 60)
        throw CArgumentException("sec");
    second = s;
}

//---------------------------------------------------------------------------
inline void CP2Time::SetMillisecond(word m)
{
    assert(m < 1000);
    if(m >= 1000)
        throw CArgumentException("csec");
    msec = m;
}

//---------------------------------------------------------------------------
inline void CP2Time::SetNanosecond(dword n)
{
    assert(n < 1000000000);
    if(n >= 1000000000)
        throw CArgumentException("nsec");
    nanoseconds = n;
}

//---------------------------------------------------------------------------
inline void CP2Time::SetDateTime(word year, byte month, byte day, byte hour, byte minute, byte sec, word msec, dword nsec)
{
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
inline void CP2Time::Clear()
{
    SetDateTime(0, 1, 1, 0, 0, 0, 0, 0);
}

//---------------------------------------------------------------------------
inline void CP2Time::SetNull()
{
    SetDateTime(0, 0, 0, 0, 0, 0, 0, 0);
}

//---------------------------------------------------------------------------
inline bool CP2Time::IsNull() const
{
    if(GetYear() != 0)        return false;
    if(GetMonth() != 0)       return false;
    if(GetDay() != 0)         return false;
    if(GetHour() != 0)        return false;
    if(GetMinute() != 0)      return false;
    if(GetSecond() != 0)      return false;
    if(GetMillisecond() != 0) return false;
    return true;
}

//---------------------------------------------------------------------------
inline void CP2Time::AddDay(int val)
{
    Add(dtpDay, val);
}

//---------------------------------------------------------------------------
inline void CP2Time::AddHour(int val)
{
    Add(dtpHour, val);
}

//---------------------------------------------------------------------------
inline void CP2Time::AddMinute(int val)
{
    Add(dtpMinute, val);
}

//---------------------------------------------------------------------------
inline void CP2Time::AddSecond(int val)
{
    Add(dtpSecond, val);
}

//---------------------------------------------------------------------------
inline void CP2Time::DecYear()
{
    word aYear;
	aYear = GetYear();
	aYear--;
    SetYear(aYear);
}

//---------------------------------------------------------------------------
inline void CP2Time::DecMonth()
{
    byte aMonth;
	aMonth = GetMonth();
	aMonth--;
    SetMonth(aMonth);
}

//---------------------------------------------------------------------------
inline void CP2Time::IncYear()
{
    word aYear;
	aYear = GetYear();
	aYear++;
    SetYear(aYear);
}

//---------------------------------------------------------------------------
inline void CP2Time::IncMonth()
{
    byte aMonth;
	aMonth = GetMonth();
	aMonth++;
    SetMonth(aMonth);
}

//---------------------------------------------------------------------------
inline CP2Time& CP2Time::operator+= (const CP2TimeSpan& span)
{
    Add(span);
    return *this;
}

//---------------------------------------------------------------------------
inline CP2Time& CP2Time::operator-= (const CP2TimeSpan& span)
{
    Subtract(span);
    return *this;
}

//---------------------------------------------------------------------------
inline string CP2Time::ToString() const
{
    return ToString("%c");
}

//---------------------------------------------------------------------------
inline void CP2Time::SetOutputWidth( width_action a, stringstream& s, int width ) const
{
    if( a == waSET )
    {
        s.width( width );
        s.fill( '0' );
    }
    else
    {
        s.width( 1 );
    }
}


} // namespace fo2base

class DateFormatParser
{
	enum ResolutionSinceEpoch { SECOND=9, MILLISECOND=6, MICROSECOND=3, NANOSECOND=0 };
public:
	DateFormatParser(std::string formatString, fo2base::UnixNanosecondsTime timestamp) : bYear(false), bMonth(false)
	{
		std::time_t tt = timestamp / std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();
		std::tm tm;
		gmtime_s(&tm, &tt);

		int64_t nmec = timestamp % std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();

		//std::transform(formatString.begin(), formatString.end(), formatString.begin(), ::tolower);

		if (formatString == "TS") {
			PrintTimeEpoch(timestamp, SECOND);
			return;
		}
		if (formatString == "TMS") {
			PrintTimeEpoch(timestamp, MILLISECOND);
			return;
		}
		if (formatString == "TMC") {
			PrintTimeEpoch(timestamp, MICROSECOND);
			return;
		}
		if (formatString == "TN") {
			PrintTimeEpoch(timestamp, NANOSECOND);
			return;
		}

		while (!formatString.empty()) {
			switch (formatString[0]) {
			case 'Y':	PrintYear(formatString, tm.tm_year + 1900); break;
			case 'M':	PrintMonthOrMinute(formatString, tm.tm_mon + 1, tm.tm_min); break;
			case 'D':	PrintDay(formatString, tm.tm_mday); break;
			case 'H':	PrintHour(formatString, tm.tm_hour); break;
			case 'S':	PrintSecond(formatString, tm.tm_sec); break;
			case 'm':   PrintMilliOrMicrosecond(formatString, nmec); break;
			case 'n':	PrintNanosecond(formatString, nmec); break;
			default:
				break;
			}
		}
	}

	operator const char*() const
	{
		return output.c_str();
	}

private:
	void PrintTimeEpoch(fo2base::UnixNanosecondsTime timestamp, const ResolutionSinceEpoch resolution)
	{
		std::string res(std::to_string(timestamp));
		switch (resolution) {
		case SECOND:		res.erase(res.size() - SECOND); break;
		case MILLISECOND:	res.erase(res.size() - MILLISECOND); break;
		case MICROSECOND:	res.erase(res.size() - MICROSECOND); break;
		case NANOSECOND:	res.erase(res.size() - NANOSECOND); break;
		}

		output = res;
	}

	void PrintYear(std::string& formatString, int year)
	{
		char buf[100];
		std::string format;
		std::string::size_type Y_pos = formatString.find_last_of('Y');
		if (Y_pos == 1) {
			format = "%02u";
		}
		else if (Y_pos == 3) {
			format = "%4u";
		}
		else {
			return;
		}
		sprintf_s(buf, sizeof(buf), format.c_str(), year);
		output.append(buf);
		output.append(1, formatString[Y_pos + 1]);
		formatString.erase(0, Y_pos + 2);
		bYear = true;
	}

	void PrintMonthOrMinute(std::string& formatString, int month, int minute)
	{
		char buf[100];
		std::string format;
		std::string::size_type M_pos = formatString.find_last_of('M');
		format = "%02u";
		if (!bYear && M_pos != 1) {
			return;
		}
		M_pos = 1;
		if (bYear && !bMonth)
		{
			sprintf_s(buf, sizeof(buf), format.c_str(), month);
			bMonth = true;
		}
		else
		{
			sprintf_s(buf, sizeof(buf), format.c_str(), minute);
		}
		output.append(buf);
		output.append(1, formatString[M_pos + 1]);
		formatString.erase(0, M_pos + 2);
	}

	void PrintDay(std::string& formatString, int day)
	{
		char buf[100];
		std::string format;
		std::string::size_type D_pos = formatString.find_last_of('D');
		if (D_pos == 1) {
			format = "%02u";
		}
		else {
			return;
		}
		sprintf_s(buf, sizeof(buf), format.c_str(), day);
		output.append(buf);
		output.append(1, formatString[D_pos + 1]);
		formatString.erase(0, D_pos + 2);
		bYear = true;
	}

	void PrintHour(std::string& formatString, int hour)
	{
		char buf[100];
		std::string format;
		std::string::size_type H_pos = formatString.find_last_of('H');
		if (H_pos == 1) {
			format = "%02u";
		}
		else {
			return;
		}
		sprintf_s(buf, sizeof(buf), format.c_str(), hour);
		output.append(buf);
		output.append(1, formatString[H_pos + 1]);
		formatString.erase(0, H_pos + 2);
	}

	void PrintSecond(std::string& formatString, int second)
	{
		char buf[100];
		std::string format;
		std::string::size_type S_pos = formatString.find_last_of('S');
		if (S_pos == 1) {
			format = "%02u";
		}
		else {
			return;
		}
		sprintf_s(buf, sizeof(buf), format.c_str(), second);
		output.append(buf);
		output.append(1, formatString[S_pos + 1]);
		formatString.erase(0, S_pos + 2);
	}

	void PrintMilliOrMicrosecond(std::string& formatString, int64_t nmec)
	{
		char buf[100];
		std::string format;
		std::string::size_type pos = formatString.find_last_of('m');
		int64_t msec;
		if (pos == 2) {
			format = "%03.3I64u";
			msec = nmec / 1000000;
		}
		else if (pos == 5) {
			format = "%06.6I64u";
			msec = nmec / 1000;
		}
		else {
			return;
		}
		sprintf_s(buf, sizeof(buf), format.c_str(), msec);
		output.append(buf);
		output.append(1, formatString[pos + 1]);
		formatString.erase(0, pos + 2);
	}

	void PrintNanosecond(std::string& formatString, int64_t nmec)
	{
		char buf[100];
		std::string format;
		std::string::size_type pos = formatString.find_last_of('n');
		if (pos == 8) {
			format = "%09.9I64u";
		}
		else {
			return;
		}
		sprintf_s(buf, sizeof(buf), format.c_str(), nmec);
		output.append(buf);
		formatString.erase(0, pos+1);
	}

private:
	std::string output;
	bool bYear;
	bool bMonth;

};	// DateFormatParser
