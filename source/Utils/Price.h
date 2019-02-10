#pragma once
#include <strsafe.h>
#include <assert.h>

struct Price
{
	enum Separator { POINT, COMMA };

    int _integer;
    UINT _fraction;
    bool minus;
    unsigned char _precision; // кол-во цифр после запятой

    Price(int intPart = INT_MAX, unsigned int fraqPart = 0, bool znak = false, char precisionPart = 0) : _integer(intPart), _fraction(fraqPart), minus(znak), _precision(precisionPart) {}
    Price(double price, int precision);
    Price(const Price& price)
    {
        _integer = price._integer;
        _fraction = price._fraction;
        minus = price.minus;
        _precision = price._precision;
    }

    double ToDouble() const;

    // пересчёт значения цены
    // бывает нужно если fraction часть имеет лишние нули
    void Recalc(char precision)
    {
        int count = getCountsOfDigits(_fraction);
        _fraction /= (UINT)std::pow(10., count-precision);
    }

    std::string GetString(const Separator sep = POINT) const
    {
        char str[50] = {0};
        if (_integer != INT_MAX)
        {
            if (_precision)
			{
				char sepSymbol = (sep == POINT) ? '.' : ',';
                StringCchPrintfA(str, sizeof(str), minus ? "-%d%c%0.*u" : "%d%c%0.*u", _integer, sepSymbol, _precision, _fraction);
			}
            else StringCchPrintfA(str, sizeof(str), "%d", _integer);
        }
        else StringCchPrintfA(str, sizeof(str), "-");

        return std::string(str);
    }

    const char* GetString(int precision, const Separator sep = POINT) const
    {
        static char str[50];
        if (_integer != INT_MAX)
        {
            if (precision) 
			{
				char sepSymbol = (sep == POINT) ? '.' : ',';
				StringCchPrintfA(str, 50, minus ? "-%d%c%0.*u" : "%d%c%0.*u", _integer, sepSymbol, _precision, _fraction);
			}
            else StringCchPrintfA(str, 50, "%d", _integer);
        }
        else StringCchPrintfA(str, 50, "-");
        return str; 
    }

    const wchar_t* GetWString(int precision) const 
    {
        static wchar_t str[50];
        if (_integer != INT_MAX)
        {
            if (precision) StringCchPrintfW(str, 50,  minus ? L"-%d.%0.*u" : L"%d.%0.*u", _integer, precision, _fraction);
            else StringCchPrintfW(str, 50, L"%d", _integer);
        }
        else StringCchPrintfW(str, 50, L"-");
        return str; 
    }

    void SetPrice(const Price& p) { operator=(p); }
    void SetPrice(const int& i, const unsigned int& f, bool y = false) { _integer = i; _fraction = f; minus = y; }
    void SetPrice(double price, int precision);
    void SetInt(const int& i) { _integer = i; }
    void SetFrac(const unsigned int& f) { _fraction = f; }
    void SetZnak(bool i) { minus = i; }
    void IncFrac() { ++_fraction; }
    void IncInt() { ++_integer; }
    void DecFrac() { --_fraction; }
    void DecInt() { --_integer; }
    int GetInt() const { return _integer; }
    UINT GetFrac() const { return _fraction; }
    bool GetZnak() const { return minus; }
    bool IsPrice() const { return _integer != INT_MAX; }
    bool IsZero() const { return (_integer == 0 && _fraction == 0); }

    bool operator<(const int& value) const { return _integer < value; }
    bool operator<=(const int& value) const { return _integer < value || (_integer == value && !_fraction); }
    bool operator==(const int& value) const { return _integer == value && !_fraction; }
    bool operator!=(const int& value) const { return !operator==(value); }
    bool operator>=(const int& value) const { return !operator<(value); }
    bool operator>(const int& value) const { return !operator<=(value); }

    bool operator<(const Price& other) const 
    { 
        assert(_precision == other._precision || other._precision == 0 || _precision == 0);
        if (_precision != other._precision && other._precision != 0 && _precision != 0)
        {
            throw std::exception("Precision is different between two prices");
        }

        if (!minus && minus == other.minus)
            return _integer < other._integer || (_integer == other._integer && _fraction < other._fraction); 
        else if (!minus && other.minus)
            return false;
        else if (minus && !other.minus)
            return true;
        else if (minus && minus == other.minus)
            return _integer > other._integer || (_integer == other._integer && _fraction > other._fraction); 
        return 0;
    }

    bool operator<=(const Price& other) const 
    { 
        assert(_precision == other._precision || other._precision == 0 || _precision == 0);
        if (_precision != other._precision && other._precision != 0 && _precision != 0)
        {
            throw std::exception("Precision is different between two prices");
        }

        if (!minus && minus == other.minus)
            return _integer < other._integer || (_integer == other._integer && _fraction <= other._fraction); 
        else if (!minus && other.minus)
            return false;
        else if (minus && !other.minus)
            return true;
        else if (minus && minus == other.minus)
            return _integer > other._integer || (_integer == other._integer && _fraction >= other._fraction); 
        return 0;
    }

    bool operator==(const Price& other) const 
    { 
        assert(_precision == other._precision || other._precision == 0 || _precision == 0);
        if (_precision != other._precision && other._precision != 0 && _precision != 0)
        {
            throw std::exception("Precision is different between two prices");
        }

        if (IsZero() && other.IsZero())
            return true; 
        return (_integer == other._integer && _fraction == other._fraction && minus == other.minus); 
    }

    bool operator!=(const Price& other) const { return !operator==(other); }
    bool operator>=(const Price& other) const { return !operator<(other); }
    bool operator>(const Price& other) const { return !operator<=(other); }

    Price& operator++() { ++_integer; return *this; }
    Price& operator--() { --_integer; return *this; }
    Price operator-() const { return Price(-_integer, _fraction); }

    static int getCountsOfDigits(int number)
    {
        int count = (number == 0) ? 1 : 0;
        while (number != 0) 
        {
            count++;
            number /= 10;
        }
        return count;
    }
};