//#include "P2SysBCDII.h"

#ifndef _P2DYSBCDII_HPP_
#define _P2DYSBCDII_HPP_


#ifdef  __cplusplus

#include "P2SysBCDII.h"


class p2bcdii_error
{
public:
	UINT32 p2bcd_retflag;
    p2bcdii_error( UINT32 ui32 ) { p2bcd_retflag = ui32; }
    static void throwIf( UINT32 flag ) { if( flag & P2BCDII_RESULT_BAD ) throw p2bcdii_error( flag ); }
};


#ifdef __GNUC__
#pragma pack(push, 1)
#else
#include <pshpack1.h>
#endif

class P2BCDIIPP_base
{

protected:
    P2BCDIIStoreType type;
    typedef BYTE dataType[1];
    dataType data;

    P2BCDIIPP_base( P2BCDIIStoreType type_ ) { type = type_; }

public:

	inline bool operator != (const P2BCDIIPP_base& bcd) const;
	inline bool operator == (const P2BCDIIPP_base& bcd) const;
	inline bool operator < (const P2BCDIIPP_base& bcd) const;
	inline bool operator <= (const P2BCDIIPP_base& bcd) const;
	inline bool operator > (const P2BCDIIPP_base& bcd) const;
	inline bool operator >= (const P2BCDIIPP_base& bcd) const;

	inline P2BCDIIPP_base& operator=( INT64 i64 );
	inline P2BCDIIPP_base& operator=( UINT64 ui64 );
	inline P2BCDIIPP_base& operator=( int i );
	inline P2BCDIIPP_base& operator=( unsigned int i );

	inline P2BCDIIPP_base& operator=(const P2BCDIIPP_base& bcd);
	inline P2BCDIIPP_base& operator=(const char *s);
	inline P2BCDIIPP_base& operator=(const P2WCHAR_T *ws);
	inline P2BCDIIPP_base& operator=(double d);
	inline P2BCDIIPP_base& Assign(const void* bcd);
	// inline P2BCDIIPP_base& Assign(const void* bcdtl, P2BCDIIType type);

	inline UINT32 Check(void) const;

	inline void SetZero(void);
	inline bool IsZero(void) const;
	inline void SetNAN(void);
	inline bool IsNAN(void) const;
	inline void SetMin(void);
	inline void SetMax(void);


	// арифметика
	// результат при необходимости округляется
	//inline UINT32 Add( void* ret, const void* b);
	//inline UINT32 Sub( void* ret, const void* b);
	//inline UINT32 Mul( void* ret, const void* b);
	//inline UINT32 Div( void* ret, const void* v);
	inline UINT32 Neg(void);

    inline P2BCDIIPP_base& operator+=(const P2BCDIIPP_base& bcd);
    inline P2BCDIIPP_base& operator-=(const P2BCDIIPP_base& bcd);
    inline P2BCDIIPP_base& operator*=(const P2BCDIIPP_base& bcd);
    inline P2BCDIIPP_base& operator/=(const P2BCDIIPP_base& bcd);

	// округления
	// fd - количество значащих цифр после запятой.
	inline UINT32 Round( int fd, roundstyle rs );
	inline bool IsRound( int fd ) const;
    inline int Sign() const;

	// сравнения
	inline int Cmp( const P2BCDIIPP_base& other ) const;

	inline int GetDigit(int dig) const;

	// преобразования
	inline UINT32 CastToInt64( INT64* pi64, int fd = 0 ) const;
	inline UINT32 CastToUInt64( UINT64* pui64, int fd = 0 ) const;
	inline UINT32 CastToDouble(double* dbl) const;
	inline P2BCDERR CastToString( char* dst, int precision = -1, UINT32 flag = 0 ) const;
	inline P2BCDERR CastToString( P2WCHAR_T* dst, int precision = -1, UINT32 flag = 0 ) const;

#ifdef P2SYSTYPES_OLDBCD
	inline P2BCDERR CastToOldBCD( void* oldBcd, UINT32 oldType ) const;
    inline UINT32 GetFromBCD( const void* oldBcd, UINT32 oldType );
#endif //P2SYSTYPES_OLDBCD

	// получить минимальный тип куда влезает число
	inline P2BCDIIType GetValueType(void) const;

//private:
//	P2BCDII(digits, frac) m_val;
};

#ifdef __GNUC__
#pragma pack(pop)
#else
#include <poppack.h>
#endif

bool P2BCDIIPP_base::operator != (const P2BCDIIPP_base& other) const
{
    return !! P2BCDIICmp( this, &other );
}
bool P2BCDIIPP_base::operator == (const P2BCDIIPP_base& other) const
{
    return ! P2BCDIICmp( this, &other );
}
bool P2BCDIIPP_base::operator < (const P2BCDIIPP_base& other) const
{
    return P2BCDIICmp( this, &other ) < 0;
}
bool P2BCDIIPP_base::operator <= (const P2BCDIIPP_base& other) const
{
    return P2BCDIICmp( this, &other ) <= 0;
}
bool P2BCDIIPP_base::operator > (const P2BCDIIPP_base& other) const
{
    return P2BCDIICmp( this, &other ) > 0;
}
bool P2BCDIIPP_base::operator >= (const P2BCDIIPP_base& other) const
{
    return P2BCDIICmp( this, &other ) >= 0;
}


P2BCDIIPP_base& P2BCDIIPP_base::operator=( INT64 i64 )
{
    p2bcdii_error::throwIf( P2BCDIIGetFromInt64( this, i64, 0 ) );
    return *this;
}

P2BCDIIPP_base& P2BCDIIPP_base::operator=( UINT64 ui64 )
{
    p2bcdii_error::throwIf( P2BCDIIGetFromUInt64( this, ui64, 0 ) );
    return *this;
}

P2BCDIIPP_base& P2BCDIIPP_base::operator=( int i )
{
    p2bcdii_error::throwIf( P2BCDIIGetFromInt64( this, (INT64)i, 0 ) );
    return *this;
}

P2BCDIIPP_base& P2BCDIIPP_base::operator=( unsigned int i )
{
    p2bcdii_error::throwIf( P2BCDIIGetFromUInt64( this, (unsigned int)i, 0 ) );
    return *this;
}


P2BCDIIPP_base& P2BCDIIPP_base::operator=( const P2BCDIIPP_base& other )
{
    if( other.type == type )
    {
		if( P2BCDIIIsNAN( &other ) )
	        p2bcdii_error::throwIf( P2BCDII_OPERAND_BAD );
        memcpy( data, other.data, (size_t)P2BCDIISIZETL( type ) );
    }
    else
    {
        p2bcdii_error::throwIf( P2BCDIICopy( this, &other ) );
    }
    return *this;
}

P2BCDIIPP_base& P2BCDIIPP_base::operator=(const char *s)
{
	const char* strend;
	UINT32 flag = P2BCDIIGetFromStrEx( this, s, ROUND_TRUNC, &strend );
	if( flag & P2BCDII_RESULT_BAD || *strend )
		throw p2bcdii_error( flag );
    return *this;
}

P2BCDIIPP_base& P2BCDIIPP_base::operator=(const P2WCHAR_T *s)
{
	const P2WCHAR_T* strend;
	UINT32 flag = P2BCDIIGetFromWStrEx( this, s, ROUND_TRUNC, &strend );
	if( flag & P2BCDII_RESULT_BAD || *strend )
		throw p2bcdii_error( flag );
    return *this;
}

P2BCDIIPP_base& P2BCDIIPP_base::operator=(double d)
{
    p2bcdii_error::throwIf( P2BCDIIGetFromDouble( this, d ) );
    return *this;
}

P2BCDIIPP_base& P2BCDIIPP_base::Assign( const void* src )
{
    p2bcdii_error::throwIf( P2BCDIICopy( this, src ) );
    return *this;
}

UINT32 P2BCDIIPP_base::Check(void) const
{
    return P2BCDIICheck( this );
}

void P2BCDIIPP_base::SetZero(void)
{
    P2BCDIISetZero( this );
}

bool P2BCDIIPP_base::IsZero(void) const
{
    return !!P2BCDIIIsZero( this );
}

void P2BCDIIPP_base::SetNAN(void)
{
    P2BCDIISetNAN( this );
}

bool P2BCDIIPP_base::IsNAN(void) const
{
    return !!P2BCDIIIsNAN( this );
}

void P2BCDIIPP_base::SetMin(void)
{
    P2BCDIISetMin( this );
}

void P2BCDIIPP_base::SetMax(void)
{
    P2BCDIISetMax( this );
}


UINT32 P2BCDIIPP_base::Neg(void)
{
    return P2BCDIINeg( this );
}


P2BCDIIPP_base& P2BCDIIPP_base::operator+=(const P2BCDIIPP_base& bcd)
{
    p2bcdii_error::throwIf( P2BCDIIAdd( this, this, &bcd ) );
    return *this;
}

P2BCDIIPP_base& P2BCDIIPP_base::operator-=(const P2BCDIIPP_base& bcd)
{
    p2bcdii_error::throwIf( P2BCDIISub( this, this, &bcd ) );
    return *this;
}

P2BCDIIPP_base& P2BCDIIPP_base::operator*=(const P2BCDIIPP_base& bcd)
{
    p2bcdii_error::throwIf( P2BCDIIMul( this, this, &bcd ) );
    return *this;
}

P2BCDIIPP_base& P2BCDIIPP_base::operator/=(const P2BCDIIPP_base& bcd)
{
    p2bcdii_error::throwIf( P2BCDIIDiv( this, this, &bcd ) );
    return *this;
}

UINT32 P2BCDIIPP_base::Round( int fd, roundstyle rs )
{
    return P2BCDIIRound( this, fd, rs );
}

bool P2BCDIIPP_base::IsRound( int fd ) const
{
    return !!P2BCDIIIsRound( this, fd );
}

int P2BCDIIPP_base::Sign() const
{
    return P2BCDIISign( this );
}

int P2BCDIIPP_base::Cmp( const P2BCDIIPP_base& other ) const
{
    return P2BCDIICmp( this, &other );
}


int P2BCDIIPP_base::GetDigit( int dig ) const
{
    return P2BCDIIGetDigit( this, dig );
}


UINT32 P2BCDIIPP_base::CastToInt64( INT64* pi64, int fd ) const
{
    return P2BCDII2Int64( pi64, this, fd );
}

UINT32 P2BCDIIPP_base::CastToUInt64( UINT64* pui64, int fd ) const
{
    return P2BCDII2UInt64( pui64, this, fd );
}

UINT32 P2BCDIIPP_base::CastToDouble(double* dbl) const
{
    return P2BCDII2Double( dbl, this );
}

#ifdef P2SYSTYPES_OLDBCD

P2BCDERR P2BCDIIPP_base::CastToOldBCD( void* oldBcd, UINT32 oldType ) const
{
    return P2BCDII2BCD( oldBcd, oldType, this );
}

UINT32 P2BCDIIPP_base::GetFromBCD( const void* oldBcd, UINT32 oldType )
{
    return P2BCDIIGetFromBCD( this, oldBcd, oldType );
}

#endif //P2SYSTYPES_OLDBCD

P2BCDERR P2BCDIIPP_base::CastToString( char* dst, int precision, UINT32 flag ) const
{
    return P2BCDIIFormat( dst, this, precision, flag );
}

P2BCDERR P2BCDIIPP_base::CastToString( P2WCHAR_T* dst, int precision, UINT32 flag ) const
{
    return P2BCDIIWFormat( dst, this, precision, flag );
}



P2BCDIIType P2BCDIIPP_base::GetValueType(void) const
{
    return P2BCDIIGetValueType( this );
}




template <P2BCDIIType tptype>
class P2BCDIIPP : public P2BCDIIPP_base
{
    private:
    BYTE _[ P2BCDIISIZETL(tptype) <= sizeof(dataType) ? (P2BCDIISIZETL(tptype)?1:0) : P2BCDIISIZETL(tptype) - sizeof(dataType) ];
    // Будьте внимательны. Параметр темплейта ТИП (а не кол-во цифр)

    public:
    inline P2BCDIIPP();

    inline P2BCDIIPP( const char* str );
    inline P2BCDIIPP( const P2WCHAR_T* str );
    inline P2BCDIIPP( double f);
    inline P2BCDIIPP( const P2BCDIIPP_base& other );

    inline P2BCDIIPP( INT64 i64, int nfrac = 0 );
    inline P2BCDIIPP( UINT64 ui64, int nfrac = 0 );
    inline P2BCDIIPP( int i, int nfrac = 0 );
    inline P2BCDIIPP( unsigned int i, int nfrac = 0 );

    ~P2BCDIIPP() {}

	P2BCDIIPP_base& operator=( INT64 i64 ) { return P2BCDIIPP_base::operator= ( i64 ); }
	P2BCDIIPP_base& operator=( UINT64 ui64 ) { return P2BCDIIPP_base::operator= ( ui64 ); }
	P2BCDIIPP_base& operator=( int i ) { return P2BCDIIPP_base::operator= ( i ); }
	P2BCDIIPP_base& operator=( unsigned int i ) { return P2BCDIIPP_base::operator= ( i ); }

    P2BCDIIPP_base& operator= ( const P2BCDIIPP_base& other ) { return P2BCDIIPP_base::operator= ( other ); }
    P2BCDIIPP_base& operator= ( const char* str ) { return P2BCDIIPP_base::operator= ( str ); }
	P2BCDIIPP_base& operator= (const P2WCHAR_T *ws) { return P2BCDIIPP_base::operator= ( ws ); }
	P2BCDIIPP_base& operator=( double d )  { return P2BCDIIPP_base::operator= ( d ); }
	//P2BCDIIPP_base& operator=(void *oldBcd)  { return P2BCDIIPP_base::operator= ( i64 ); }


//	explicit P2BCDIIPP(bool initialize = true) {}; // !!! Implementation required
};

template <P2BCDIIType tptype>
P2BCDIIPP<tptype>::P2BCDIIPP() : P2BCDIIPP_base( tptype )
{
    P2BCDIISetNANTL( data, tptype );
}

template <P2BCDIIType tptype>
P2BCDIIPP<tptype>::P2BCDIIPP( const char* str ) : P2BCDIIPP_base( tptype )
{
    p2bcdii_error::throwIf( P2BCDIIGetFromStr( this, str ) );
}

template <P2BCDIIType tptype>
P2BCDIIPP<tptype>::P2BCDIIPP( const P2WCHAR_T* str ) : P2BCDIIPP_base( tptype )
{
    p2bcdii_error::throwIf( P2BCDIIGetFromWStr( this, str ) );
}

template <P2BCDIIType tptype>
P2BCDIIPP<tptype>::P2BCDIIPP( double f ) : P2BCDIIPP_base( tptype )
{
    p2bcdii_error::throwIf( P2BCDIIGetFromDouble( this, f ) );
}

template <P2BCDIIType tptype>
P2BCDIIPP<tptype>::P2BCDIIPP( const P2BCDIIPP_base& other ) : P2BCDIIPP_base( tptype )
{
    p2bcdii_error::throwIf( P2BCDIICopy( this, &other ) );
}

template <P2BCDIIType tptype>
P2BCDIIPP<tptype>::P2BCDIIPP( INT64 i64, int nfrac ) : P2BCDIIPP_base( tptype )
{
    p2bcdii_error::throwIf( P2BCDIIGetFromInt64( this, i64, nfrac ) );
}

template <P2BCDIIType tptype>
P2BCDIIPP<tptype>::P2BCDIIPP( UINT64 ui64, int nfrac ) : P2BCDIIPP_base( tptype )
{
    p2bcdii_error::throwIf( P2BCDIIGetFromUInt64( this, ui64, nfrac ) );
}

template <P2BCDIIType tptype>
P2BCDIIPP<tptype>::P2BCDIIPP( int i, int nfrac ) : P2BCDIIPP_base( tptype )
{
    p2bcdii_error::throwIf( P2BCDIIGetFromInt64( this, (INT64)i, nfrac ) );
}

template <P2BCDIIType tptype>
P2BCDIIPP<tptype>::P2BCDIIPP( unsigned int i, int nfrac ) : P2BCDIIPP_base( tptype )
{
    p2bcdii_error::throwIf( P2BCDIIGetFromUInt64( this, (UINT64)i, nfrac ) );
}




typedef P2BCDIIPP< P2BCDIIMAKETYPE(P2BCDII_MAX_DIGITS-3,(P2BCDII_MAX_DIGITS-3)/2) > P2BCDIIPP_MAX_TYPE;

typedef P2BCDIIPP< P2BCDIIMAKETYPE(10,0) > P2BCDIIPP_MAXINT32_TYPE; // сюда всегда поместится инт, знаковый или беззнаковый
typedef P2BCDIIPP< P2BCDIIMAKETYPE(20,0) > P2BCDIIPP_MAXINT64_TYPE; // а сюда обязательно должен поместиться INT64 или UINT64

#ifdef __GNUC__
#pragma pack(push, 1)
#else
#include <pshpack1.h>
#endif
struct fromDoubleType
{
    // должна быть побитно совместимой с P2BCDIIPP (из этого файла) и InternalBCDType (из P2SysBCDII.cpp)
    P2BCDIIStoreType type;
    BYTE data[P2BCDIIFROMDOUBLESIZETL];
	fromDoubleType( double d ) { type = P2BCDIITypeForDouble(d); p2bcdii_error::throwIf(P2BCDIIGetFromDouble(this,d)); }
};
#ifdef __GNUC__
#pragma pack(pop)
#else
#include <poppack.h>
#endif



#define P2BCDPP_OP1( op, func, type ) inline P2BCDIIPP_base& operator op##=( P2BCDIIPP_base& a, type b ) { \
        P2BCDIIPP_MAX_TYPE tmp( b ); p2bcdii_error::throwIf( P2BCDII##func( &a, &a, &tmp ) ); return a; }
#define P2BCDPP_OP1d( op, func ) inline P2BCDIIPP_base& operator op##=( P2BCDIIPP_base& a, double d ) { \
        fromDoubleType tmp( d ); p2bcdii_error::throwIf( P2BCDII##func( &a, &a, &tmp ) ); return a; }
#define P2BCDPP_OP2( op, func, type, tmptype ) inline P2BCDIIPP_base& operator op##=( P2BCDIIPP_base& a, type b ) { \
        tmptype tmp( b, 0 ); p2bcdii_error::throwIf( P2BCDII##func( &a, &a, &tmp ) ); return a; }
#define P2BCDPP_OP3( op, func, type ) inline P2BCDIIPP_MAX_TYPE operator op( const P2BCDIIPP_base& a, type b ) { \
    P2BCDIIPP_MAX_TYPE tmp( b ); P2BCDIIPP_MAX_TYPE ret; p2bcdii_error::throwIf( P2BCDII##func( &ret, &a, &tmp ) ); return ret; } \
    inline P2BCDIIPP_MAX_TYPE operator op(  type b, const P2BCDIIPP_base& a ) { \
    P2BCDIIPP_MAX_TYPE tmp( b ); P2BCDIIPP_MAX_TYPE ret; p2bcdii_error::throwIf( P2BCDII##func( &ret, &tmp, &a ) ); return ret; }
#define P2BCDPP_OP3d( op, func ) inline P2BCDIIPP_MAX_TYPE operator op( const P2BCDIIPP_base& a, double d ) { \
    fromDoubleType tmp( d ); P2BCDIIPP_MAX_TYPE ret; p2bcdii_error::throwIf( P2BCDII##func( &ret, &a, &tmp ) ); return ret; } \
    inline P2BCDIIPP_MAX_TYPE operator op(  double d, const P2BCDIIPP_base& a ) { \
    fromDoubleType tmp( d ); P2BCDIIPP_MAX_TYPE ret; p2bcdii_error::throwIf( P2BCDII##func( &ret, &tmp, &a ) ); return ret; }
#define P2BCDPP_OP4( op, func, type, tmptype ) inline P2BCDIIPP_MAX_TYPE operator op( const P2BCDIIPP_base& a, type b ) { \
    tmptype tmp( b, 0 ); P2BCDIIPP_MAX_TYPE ret; p2bcdii_error::throwIf( P2BCDII##func( &ret, &a, &tmp ) ); return ret; } \
    inline P2BCDIIPP_MAX_TYPE operator op(  type b, const P2BCDIIPP_base& a ) { \
    tmptype tmp( b, 0 ); P2BCDIIPP_MAX_TYPE ret; p2bcdii_error::throwIf( P2BCDII##func( &ret, &tmp, &a ) ); return ret; }

#define P2BCDPP_OP_ALLTYPES( op, func ) \
P2BCDPP_OP2( op, func, int, P2BCDIIPP_MAXINT32_TYPE ) \
P2BCDPP_OP2( op, func, unsigned int, P2BCDIIPP_MAXINT32_TYPE ) \
P2BCDPP_OP2( op, func, INT64, P2BCDIIPP_MAXINT64_TYPE ) \
P2BCDPP_OP2( op, func, UINT64, P2BCDIIPP_MAXINT64_TYPE ) \
/*P2BCDPP_OP1( op, func, double )*/ P2BCDPP_OP1d( op, func ) \
P2BCDPP_OP1( op, func, const char* ) \
P2BCDPP_OP1( op, func, const P2WCHAR_T* ) \
P2BCDPP_OP4( op, func, int, P2BCDIIPP_MAXINT32_TYPE ) \
P2BCDPP_OP4( op, func, unsigned int, P2BCDIIPP_MAXINT32_TYPE ) \
P2BCDPP_OP4( op, func, INT64, P2BCDIIPP_MAXINT64_TYPE ) \
P2BCDPP_OP4( op, func, UINT64, P2BCDIIPP_MAXINT64_TYPE ) \
/*P2BCDPP_OP3( op, func, double )*/ P2BCDPP_OP3d( op, func ) \
P2BCDPP_OP3( op, func, const char* ) \
P2BCDPP_OP3( op, func, const P2WCHAR_T* )

P2BCDPP_OP_ALLTYPES( +, Add )
P2BCDPP_OP_ALLTYPES( -, Sub )
P2BCDPP_OP_ALLTYPES( *, Mul )
P2BCDPP_OP_ALLTYPES( /, Div )
P2BCDPP_OP_ALLTYPES( %, Mod )


#define P2BCDPP_OP5( op, type ) inline bool operator op( const P2BCDIIPP_base& a, type b ) { \
    P2BCDIIPP_MAX_TYPE tmp( b ); return P2BCDIICmp( &a, &tmp ) op 0; } \
    inline bool operator op( type b, const P2BCDIIPP_base& a ) { \
    P2BCDIIPP_MAX_TYPE tmp( b ); return P2BCDIICmp( &tmp, &a ) op 0; }
#define P2BCDPP_OP5d( op ) inline bool operator op( const P2BCDIIPP_base& a, double d ) { \
    fromDoubleType tmp( d ); return P2BCDIICmp( &a, &tmp ) op 0; } \
    inline bool operator op( double d, const P2BCDIIPP_base& a ) { \
    fromDoubleType tmp( d ); return P2BCDIICmp( &tmp, &a ) op 0; }
#define P2BCDPP_OP6( op, type, tmptype ) inline bool operator op( const P2BCDIIPP_base& a, type b ) { \
    tmptype tmp( b, 0 );  return P2BCDIICmp( &a, &tmp ) op 0; } \
    inline bool operator op(  type b, const P2BCDIIPP_base& a ) { \
    tmptype tmp( b, 0 );  return P2BCDIICmp( &tmp, &a ) op 0; }

#define P2BCDPP_CMP_ALLTYPES( op ) \
/*P2BCDPP_OP5( op, double )*/ P2BCDPP_OP5d( op )\
P2BCDPP_OP5( op, const char* ) \
P2BCDPP_OP5( op, const P2WCHAR_T* ) \
P2BCDPP_OP6( op, int, P2BCDIIPP_MAXINT32_TYPE ) \
P2BCDPP_OP6( op, unsigned int, P2BCDIIPP_MAXINT32_TYPE ) \
P2BCDPP_OP6( op, INT64, P2BCDIIPP_MAXINT64_TYPE ) \
P2BCDPP_OP6( op, UINT64, P2BCDIIPP_MAXINT64_TYPE )

P2BCDPP_CMP_ALLTYPES( < )
P2BCDPP_CMP_ALLTYPES( <= )
P2BCDPP_CMP_ALLTYPES( > )
P2BCDPP_CMP_ALLTYPES( >= )
P2BCDPP_CMP_ALLTYPES( == )
P2BCDPP_CMP_ALLTYPES( != )


inline P2BCDIIPP_MAX_TYPE operator+( const P2BCDIIPP_base& a, const P2BCDIIPP_base& b )
{
    P2BCDIIPP_MAX_TYPE ret;
    p2bcdii_error::throwIf( P2BCDIIAdd( &ret, &a, &b ) );
    return ret;
}

inline P2BCDIIPP_MAX_TYPE operator-( const P2BCDIIPP_base& a, const P2BCDIIPP_base& b )
{
    P2BCDIIPP_MAX_TYPE ret;
    p2bcdii_error::throwIf( P2BCDIISub( &ret, &a, &b ) );
    return ret;
}

inline P2BCDIIPP_MAX_TYPE operator*( const P2BCDIIPP_base& a, const P2BCDIIPP_base& b )
{
    P2BCDIIPP_MAX_TYPE ret;
    p2bcdii_error::throwIf( P2BCDIIMul( &ret, &a, &b ) );
    return ret;
}

inline P2BCDIIPP_MAX_TYPE operator/( const P2BCDIIPP_base& a, const P2BCDIIPP_base& b )
{
    P2BCDIIPP_MAX_TYPE ret;
    p2bcdii_error::throwIf( P2BCDIIDiv( &ret, &a, &b ) );
    return ret;
}

inline P2BCDIIPP_MAX_TYPE operator%( const P2BCDIIPP_base& a, const P2BCDIIPP_base& b )
{
    P2BCDIIPP_MAX_TYPE ret;
    p2bcdii_error::throwIf( P2BCDIIMod( &ret, &a, &b ) );
    return ret;
}



#define BCD( n, m ) P2BCDIIPP< P2BCDIIMAKETYPE(n,m) >

#endif  //__cplusplus

#endif //_P2DYSBCDII_HPP_
