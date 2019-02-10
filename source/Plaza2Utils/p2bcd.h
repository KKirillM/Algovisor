#pragma once

#include "exept.h"

#ifndef _ASSERT
#include <cassert>
#define _ASSERT(x) assert(x)
#endif

#pragma warning(push, 3)
#pragma warning(disable: 4267)
#include "p2systypes.hpp"
#pragma warning(default: 4267)
#pragma warning(pop)
#include "p2systypes.h"
#include "p2errors.h"

extern "C" void * __cdecl memset(void * const s, int c, size_t n);
extern "C" void * __cdecl memcpy(void * dst, const void * src, size_t count);
extern "C" int    __cdecl memcmp(const void * buf1, const void * buf2, size_t count);

#pragma intrinsic(memset, memcpy, memcmp)

namespace fo2base
{

template <byte N, byte M>
class CP2BCD;

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M> operator-(const CP2BCD<N, M>& a, const CP2BCD<N, M>& b)
{
    CP2BCD<N, M> p;
    P2ERR res;

    if (P2FAILEDV(P2BCDIIErr(P2BCDIISub(&p.m_val, &a.m_val, &b.m_val)), res))
        throw CP2Error(res);
    return p;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M> operator-(long i, const CP2BCD<N, M>& b)
{
    CP2BCD<N, M> a(i);
    return a - b;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M> operator-(const CP2BCD<N, M>& a, long i)
{
    CP2BCD<N, M> b(i);
    return a - b;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M> operator+(const CP2BCD<N, M>& a, const CP2BCD<N, M>& b)
{
    CP2BCD<N, M> p;
    P2ERR res;
    if (P2FAILEDV(P2BCDIIErr(P2BCDIIAdd(&p.m_val, &a.m_val, &b.m_val)), res))
        throw CP2Error(res);
    return p;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M> operator+(long i, const CP2BCD<N, M>& b)
{
    CP2BCD<N, M> a(i);
    return a + b;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M> operator+(const CP2BCD<N, M>& a, long i)
{
    CP2BCD<N, M> b(i);
    return a + b;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M> operator*(const CP2BCD<N, M>& a, const CP2BCD<N, M>& b)
{
    CP2BCD<N, M> p;
    P2ERR res;
    if (P2FAILEDV(P2BCDIIErr(P2BCDIIMul(&p.m_val, &a.m_val, &b.m_val)), res))
        throw CP2Error(res);
    return p;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M> operator*(long i, const CP2BCD<N, M>& b)
{
    CP2BCD<N, M> a(i);
    return a * b;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M> operator*(const CP2BCD<N, M>& a, long i)
{
    CP2BCD<N, M> b(static_cast<double>(i));
    return a * b;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M> operator/(const CP2BCD<N, M>& a, const CP2BCD<N, M>& b)
{
    CP2BCD<N, M> p;
    P2ERR res;
    if (P2FAILEDV(P2BCDIIErr(P2BCDIIDiv(&p.m_val, &a.m_val, &b.m_val)), res))
        throw CP2Error(res);
    return p;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M> operator/(long i, const CP2BCD<N, M>& b)
{
    CP2BCD<N, M> a(i);
    return a / b;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M> operator/(const CP2BCD<N, M>& a, long i)
{
    CP2BCD<N, M> b(i);
    return a / b;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
bool operator==(const CP2BCD<N, M>& a, const CP2BCD<N, M>& b)
{
    return (P2BCDIICmp(&a.m_val, &b.m_val)==0);
}

template <byte N, byte M>
bool operator!=(const CP2BCD<N, M>& a, const CP2BCD<N, M>& b)
{
    return !operator==(a,b);
}

//----------------------------------------------------------------------------
template <byte N, byte M>
bool operator<(const CP2BCD<N, M>& a, const CP2BCD<N, M>& b)
{
    return (P2BCDIICmp(&a.m_val, &b.m_val ) == -1);
}

//----------------------------------------------------------------------------
template <byte N, byte M>
bool operator<=(const CP2BCD<N, M>& a, const CP2BCD<N, M>& b)
{
    int ret = P2BCDIICmp(&a.m_val, &b.m_val);
    return (ret<=0);
}

//----------------------------------------------------------------------------
template <byte N, byte M>
bool operator>(const CP2BCD<N, M>& a, const CP2BCD<N, M>& b)
{
    return (P2BCDIICmp(&a.m_val, &b.m_val) == 1);
}

//----------------------------------------------------------------------------
template <byte N, byte M>
bool operator>=(const CP2BCD<N, M>& a, const CP2BCD<N, M>& b)
{
    int ret = P2BCDIICmp(&a.m_val, &b.m_val);
    return (ret>=0);
}

//----------------------------------------------------------------------------
// class CP2BCD
//----------------------------------------------------------------------------
enum SpecialConstructionMode
{
    eSkipInit  = 1
};

template <byte N, byte M>
class CP2BCD
{
public:
    static const P2BCDIIStoreType   m_type = P2BCDIIMAKETYPE(N,M);
    static const CP2BCD             Zero;

private:
    P2BCDII( m_type ) m_val;

public:
    CP2BCD(void);
    CP2BCD(const SpecialConstructionMode) {};
    explicit CP2BCD(const std::string& str);
    
    explicit CP2BCD(double d);
    CP2BCD(INT32 d);
    CP2BCD(UINT32 d);
    explicit CP2BCD(int64_t i64);

    template <byte X, byte Y>
    explicit CP2BCD(const CP2BCD<X,Y>& d);

    CP2BCD(const CP2BCD<N,M>& BCD);
    ~CP2BCD(void);

    void Init();
    void InitNAN();

    const void* GetValue() const {return &m_val; }
    void SetNAN();
    CP2BCD<N, M>& SetMin();
    CP2BCD<N, M>& SetMax();
    void SetZero();
    bool IsZero() const;
    bool IsNAN() const;
    bool IsNeg() const;
    bool IsPositive() const;
    void Round(int fd, roundstyle rs = ROUND_NEAREST);
    int Digit(int pos);
    string AsString(int precision) const;
    string AsString() const;
    double AsDouble() const;
    int     AsInteger() const;
    int64_t AsInt64() const;
    void Set(const string& a);
    void Set(const double a);
    void FromInt64(const int64_t a);

    CP2BCD<N, M>& Neg();
    template <byte X, byte Y>
    CP2BCD<N, M> Mod(const CP2BCD<X, Y>& a) const;

    CP2BCD<N, M> Mod(const double a) const;

    template <byte X, byte Y>
    CP2BCD<N, M>& operator=(const CP2BCD<X, Y>& a);
    template <byte X, byte Y>
    CP2BCD<N, M>& operator+=(const CP2BCD<X, Y>& a);
    template <byte X, byte Y>
    CP2BCD<N, M>& operator-=(const CP2BCD<X, Y>& a);
    template <byte X, byte Y>
    CP2BCD<N, M>& operator*=(const CP2BCD<X, Y>& a);
    template <byte X, byte Y>
    CP2BCD<N, M>& operator/=(const CP2BCD<X, Y>& a);

    CP2BCD<N, M>& operator=(const CP2BCD<N, M>& a);
    CP2BCD<N, M>& operator+=(const CP2BCD<N, M>& a);
    CP2BCD<N, M>& operator-=(const CP2BCD<N, M>& a);
    CP2BCD<N, M>& operator*=(const CP2BCD<N, M>& a);
    CP2BCD<N, M>& operator/=(const CP2BCD<N, M>& a);

public:
    friend bool operator== <N,M>(const CP2BCD<N, M>& a, const CP2BCD<N, M>& b);
    friend CP2BCD<N, M> operator- <N,M>(const CP2BCD<N, M>& a, const CP2BCD<N, M>& b);
    friend CP2BCD<N, M> operator+ <N,M>(const CP2BCD<N, M>& a, const CP2BCD<N, M>& b);
    friend CP2BCD<N, M> operator* <N,M>(const CP2BCD<N, M>& a, const CP2BCD<N, M>& b);
    friend CP2BCD<N, M> operator/ <N,M>(const CP2BCD<N, M>& a, const CP2BCD<N, M>& b);
    friend bool operator< <N,M>(const CP2BCD<N, M>& a, const CP2BCD<N, M>& b);
    friend bool operator<= <N,M>(const CP2BCD<N, M>& a, const CP2BCD<N, M>& b);
    friend bool operator> <N,M>(const CP2BCD<N, M>& a, const CP2BCD<N, M>& b);
    friend bool operator>= <N,M>(const CP2BCD<N, M>& a, const CP2BCD<N, M>& b);

public:
    CP2BCD<N, M> operator-() const;
};

//----------------------------------------------------------------------------
template<byte N, byte M>
const CP2BCD<N, M> CP2BCD<N, M>::Zero = CP2BCD<N, M>(0);

//----------------------------------------------------------------------------
template <byte N, byte M>
void CP2BCD<N, M>::Init()
{
    P2BCDIIInit(&m_val, m_type);
    P2BCDIISetZero(&m_val);
}

//----------------------------------------------------------------------------
template <byte N, byte M>
void CP2BCD<N, M>::InitNAN()
{
    P2BCDIIInit(&m_val, m_type);
    P2BCDIISetNAN(&m_val);
}

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M> CP2BCD<N, M>::operator-() const
{ 
    CP2BCD<N, M> tmp(*this);
    return tmp.Neg(); 
};

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M>::CP2BCD(void)
{
    Init();
}

//----------------------------------------------------------------------------
template <byte N, byte M>
template <byte X, byte Y>
CP2BCD<N, M>::CP2BCD(const CP2BCD<X,Y>& d)
{
    Init();
    P2ERR res;
    UINT32  rc;

    rc = P2BCDIICopy(&m_val, d.GetValue());
    if(P2BCDII_OPERAND_NAN & rc)
    {
        SetNAN();
    }
    if(P2FAILEDV(P2BCDIIErr(rc), res))
    {
        throw CP2Error(res);
    }
}

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M>::CP2BCD(const std::string& str)
{
    Init();
    P2ERR res;
    if (str.empty()) return;
    if (P2FAILEDV(P2BCDIIErr(P2BCDIIGetFromStr(&m_val, str.c_str())), res))
        throw CP2Error(res);
}

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M>::CP2BCD(double d)
{
    Init();
    P2ERR res;
    if (P2FAILEDV(P2BCDIIErr(P2BCDIIGetFromDouble(&m_val, d)), res))
        throw CP2Error(res);
}

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M>::CP2BCD(INT32 d)
{
    Init();
    P2ERR res;
    if (P2FAILEDV(P2BCDIIErr(P2BCDIIGetFromInt64(&m_val, d, 0)), res))
        throw CP2Error(res);
}

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M>::CP2BCD(UINT32 d)
{
    Init();
    P2ERR res;
    if (P2FAILEDV(P2BCDIIErr(P2BCDIIGetFromUInt64(&m_val, d, 0)), res))
        throw CP2Error(res);
}

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M>::CP2BCD(int64_t i64)
{
    Init();
    P2ERR res;
    if (P2FAILEDV(P2BCDIIErr(P2BCDIIGetFromInt64(&m_val, i64, P2BCDIITYPEM(m_type))), res))
        throw CP2Error(res);
}

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M>::CP2BCD(const CP2BCD<N,M>& BCD)
{
    //Init();
    //m_val = BCD.m_val;
    memcpy(this,&BCD,sizeof(*this));
}

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M>::~CP2BCD(void)
{
}


//----------------------------------------------------------------------------
template <byte N, byte M>
template <byte X, byte Y>
CP2BCD<N, M>& CP2BCD<N, M>::operator=(const CP2BCD<X,Y>& d)
{
    P2ERR res;
    UINT32  rc;

    rc = P2BCDIICopy(&m_val, d.GetValue());
    if(P2BCDII_OPERAND_NAN & rc)
    {
        SetNAN();
        return *this;
    }
    if(P2FAILEDV(P2BCDIIErr(rc), res))
    {
        throw CP2Error(res);
    }
    return *this;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
template <byte X, byte Y>
CP2BCD<N, M>& CP2BCD<N, M>::operator+=(const CP2BCD<X, Y>& a)
{
    P2ERR res;
    if (P2FAILEDV(P2BCDIIErr(P2BCDIIAdd(&m_val, &m_val, &a.GetValue())), res))
        throw CP2Error(res);
    return *this;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
template <byte X, byte Y>
CP2BCD<N, M>& CP2BCD<N, M>::operator-=(const CP2BCD<X, Y>& a)
{
    P2ERR res;
    if (P2FAILEDV(P2BCDIIErr(P2BCDIISub(&m_val, &m_val, &a.GetValue())), res))
        throw CP2Error(res);
    return *this;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
template <byte X, byte Y>
CP2BCD<N, M>& CP2BCD<N, M>::operator*=(const CP2BCD<X, Y>& a)
{
    P2ERR res;
    if (P2FAILEDV(P2BCDIIErr(P2BCDIIMul(&m_val, &m_val, &a.GetValue())), res))
        throw CP2Error(res);
    return *this;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
template <byte X, byte Y>
CP2BCD<N, M>& CP2BCD<N, M>::operator/=(const CP2BCD<X, Y>& a)
{
    P2ERR res;
    if (P2FAILEDV(P2BCDIIErr(P2BCDIIDiv(&m_val, &m_val, &a.GetValue())), res))
        throw CP2Error(res);
    return *this;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M>& CP2BCD<N, M>::operator=(const CP2BCD<N, M>& a)
{
//     P2ERR   res;
//     UINT32  rc;
// 
//     if(&m_val == &a.m_val) return *this;
// 
//     rc = P2BCDIICopy(&m_val, &a.m_val);
//     if(P2BCDII_OPERAND_NAN & rc)
//     {
//         SetNAN();
//         return *this;
//     }
//     if(P2FAILEDV(P2BCDIIErr(rc), res))
//     {
//         throw CP2Error(res);
//     }
    memcpy(this,&a,sizeof(*this));
    return *this;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M>& CP2BCD<N, M>::Neg()
{
    P2ERR res;
    if (P2FAILEDV(P2BCDIIErr(P2BCDIINeg(&m_val)), res))
        throw CP2Error(res);
    return *this;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
template <byte X, byte Y>
CP2BCD<N, M> CP2BCD<N, M>::Mod(const CP2BCD<X, Y>& a) const
{
    P2ERR res;
    CP2BCD<N, M> ret;
    if (P2FAILEDV(P2BCDIIErr(P2BCDIIMod(&ret, &m_val, &a)), res))
        throw CP2Error(res);
    return ret;
}

template <byte N, byte M>
CP2BCD<N, M> CP2BCD<N, M>::Mod(const double a) const
{
    P2ERR res;
    CP2BCD<N, M> _a, ret;
    _a.Set(a);
    if (P2FAILEDV(P2BCDIIErr(P2BCDIIMod(&ret, &m_val, &_a)), res))
        throw CP2Error(res);
    return ret;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M>& CP2BCD<N, M>::operator+=(const CP2BCD<N, M>& a)
{
    P2ERR res;
    if (P2FAILEDV(P2BCDIIErr(P2BCDIIAdd(&m_val, &m_val, &a.m_val)), res))
        throw CP2Error(res);
    return *this;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M>& CP2BCD<N, M>::operator-=(const CP2BCD<N, M>& a)
{
    P2ERR res;
    if (P2FAILEDV(P2BCDIIErr(P2BCDIISub(&m_val, &m_val, &a.m_val)), res))
        throw CP2Error(res);
    return *this;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M>& CP2BCD<N, M>::operator*=(const CP2BCD<N, M>& a)
{
    P2ERR res;
    if (P2FAILEDV(P2BCDIIErr(P2BCDIIMul(&m_val, &m_val, &a.m_val)), res))
        throw CP2Error(res);
    return *this;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M>& CP2BCD<N, M>::operator/=(const CP2BCD<N, M>& a)
{
    P2ERR res;
    if (P2FAILEDV(P2BCDIIErr(P2BCDIIDiv(&m_val, &m_val, &a.m_val)), res))
        throw CP2Error(res);
    return *this;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
string CP2BCD<N, M>::AsString(int precision) const
{
    char buf[1024]; memset(&buf, 0, sizeof(buf));
    P2ERR res;
    if (P2FAILEDV(P2BCDIIFormat(buf, &m_val, precision, 0), res))
        throw CP2Error(res);
    return buf;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
string CP2BCD<N, M>::AsString() const
{
    char buf[1024]; memset(&buf, 0, sizeof(buf));
    P2ERR res;
    if (P2FAILEDV(P2BCDIIFormat(buf, &m_val, P2BCDIITYPEM(m_type), 0), res))
        throw CP2Error(res);
    return buf;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
void CP2BCD<N, M>::SetNAN()
{
    P2BCDIISetNAN(&m_val);
}

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M>& CP2BCD<N, M>::SetMin()
{
    P2BCDIISetMin(&m_val);
    return *this;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
CP2BCD<N, M>& CP2BCD<N, M>::SetMax()
{
    P2BCDIISetMax(&m_val);
    return *this;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
void CP2BCD<N, M>::SetZero()
{
    P2BCDIISetZero(&m_val);
}

//----------------------------------------------------------------------------
template <byte N, byte M>
bool CP2BCD<N, M>::IsZero() const
{
    return P2BCDIIIsZero(&m_val);
}

//----------------------------------------------------------------------------
template <byte N, byte M>
bool CP2BCD<N, M>::IsNeg() const
{
    return P2BCDIISign(&m_val) < 0;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
bool CP2BCD<N, M>::IsPositive() const
{
    return P2BCDIISign(&m_val) > 0;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
bool CP2BCD<N, M>::IsNAN() const
{
    return (TRUE == P2BCDIIIsNAN(&m_val));
}

//----------------------------------------------------------------------------
template <byte N, byte M>
void CP2BCD<N, M>::Round(int fd, roundstyle rs)
{
    P2ERR res;
    if (P2FAILEDV(P2BCDIIErr(P2BCDIIRound(&m_val, fd, rs)), res))
        throw CP2Error(res);
}

//----------------------------------------------------------------------------
template <byte N, byte M>
int CP2BCD<N, M>::Digit(int pos)
{
    return P2BCDIIGetDigit(&m_val, pos);
}

//----------------------------------------------------------------------------
template <byte N, byte M>
void CP2BCD<N, M>::Set(const string& a)
{
    P2ERR res;
    if (P2FAILEDV(P2BCDIIErr(P2BCDIIGetFromStr(&m_val, a.c_str())), res))
        throw CP2Error(res);
}

//----------------------------------------------------------------------------
template <byte N, byte M>
void CP2BCD<N, M>::Set(const double a)
{
    P2ERR res;
    if (P2FAILEDV(P2BCDIIErr(P2BCDIIGetFromDouble(&m_val, a)), res))
        throw CP2Error(res);
}

//----------------------------------------------------------------------------
template <byte N, byte M>
void CP2BCD<N, M>::FromInt64(const int64_t a)
{
    P2ERR res;
    if (P2FAILEDV(P2BCDIIErr(P2BCDIIGetFromInt64(&m_val, a, P2BCDIITYPEM(m_type))), res))
        throw CP2Error(res);
}
//----------------------------------------------------------------------------
template <byte N, byte M>
double CP2BCD<N, M>::AsDouble() const
{
    P2ERR   res;
    double  ret;
    if (P2FAILEDV(P2BCDIIErr(P2BCDII2Double(&ret, &m_val)), res))
        throw CP2Error(res);
    return ret;
}

//----------------------------------------------------------------------------
template <byte N, byte M>
int CP2BCD<N, M>::AsInteger() const
{
    return int( AsInt64() );
}

//----------------------------------------------------------------------------
template <byte N, byte M>
int64_t CP2BCD<N, M>::AsInt64() const
{
    P2ERR   res;
    INT64   ret;
    if (P2FAILEDV(P2BCDIIErr(P2BCDII2Int64(&ret, &m_val, 0)), res))
        throw CP2Error(res);
    return ret;
}

void fn();

template <byte N, byte M>
inline void bcdcpy(CP2BCD<N, M>& dst, const CP2BCD<N, M>& src)
{
    memcpy(&dst,&src,sizeof(CP2BCD<N, M>));
}

template <int MaxPower, int MaxCoeff>
class TenPowerDataChache
{
public:
    int64_t* longTenPowersPlusArray;

    TenPowerDataChache()
    {
        int64_t power = 1;
        longTenPowersPlusArray = new int64_t[MaxPower*MaxCoeff];
        for (size_t i = 0; i < MaxPower; ++i)
        {
            for (size_t j = 0; j < MaxCoeff; ++j)
            {
                longTenPowersPlusArray[i*MaxCoeff+j] = power*j;
            }
            power *= 100;
        }
    }

    int64_t get(int power,int coeff)
    {
        return longTenPowersPlusArray[power*MaxCoeff+coeff];
    }

    ~TenPowerDataChache()
    {
        delete[] longTenPowersPlusArray;
    }
};

template <typename TP2BCD>
int64_t bcdToInt64(const TP2BCD& other)
{
    int64_t value(0);
    static TenPowerDataChache<20, 100> tenPowerDataChache;
    static int total(P2BCDIISIZETF(other.m_type) - 2);
    char* buf((char *)const_cast<void*>(other.GetValue()));
    buf+=2;
    int tenPower = total-1;
    bool isNeg = false;
    if ( (*buf & -128) == -128)
    {
        isNeg=true;
    }
    char coeff = (*buf & 127);
    if (coeff != 0)
    {
        value +=  tenPowerDataChache.get(tenPower,coeff);
    }
    tenPower--;
    ++buf;
    while (tenPower > 0)
    {
        if (*buf != 0)
        {
            value +=  tenPowerDataChache.get(tenPower,*buf);
        }
        tenPower--;
        ++buf;
    }
    value+=*buf;
    if (isNeg)
    {
        value *=  -1;
    }
    #pragma warning(suppress: 4127)
    if ( P2BCDIITYPEM(other.m_type) % 2 != 0 )
    {
        value =  value/10;
    }
    return value;
}

}  //namespace fo2base
