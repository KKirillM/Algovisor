#ifndef _P2DYSBCDII_H_
#define _P2DYSBCDII_H_

#ifdef P2BCD_STANDALONE

#include <string.h>

// from P2Types.h
typedef unsigned short int UINT16;
typedef unsigned int UINT32;
typedef int INT32;
typedef unsigned __int64 UINT64;
typedef __int64 INT64;
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short int WORD;

#define	PLATFORM_S64(x)		x##i64
#define	PLATFORM_U64(x)		x##ui64

#ifdef _WIN32
typedef wchar_t P2WCHAR_T;
#elif defined(__linux__) || defined(__MACH__)
typedef INT16 P2WCHAR_T;
#endif

//#define UNUSED(expr) do { (void)(expr); } while (0);

// from ��������
#define P2SYSTYPES_API
#define P2CALLSPEC
#define P2BCD_NOSQL

#else // P2BCD_STANDALONE

#include "../../P2Common/Include/P2Types.h"
#include "../../../Plaza-II/P2Common/Include/P2Windows.h"
#include "P2SysTypes_spec.h"

#endif // P2BCD_STANDALONE


// ���� ������.
// !!! �������� !!! ��������� ����� ��� ������ ����� ����� � ��� ����� ifdef'�
#ifdef P2BCD_STANDALONE

typedef UINT32 P2BCDERR;
#define P2BCDERR_OK 0
#define P2BCDERR_WRONG_ARGUMENTS 2
#define P2BCDERR_OVERFLOW 14

#else // P2BCD_STANDALONE

typedef P2ERR P2BCDERR;
#define P2BCDERR_OK P2ERR_OK
#define P2BCDERR_WRONG_ARGUMENTS P2ERR_COMMON_WRONG_ARGUMENTS
#define P2BCDERR_OVERFLOW P2ERR_COMMON_OVERFLOW // ?? P2ERR_VM_NUMERIC_OVERFLOW ??

#endif // P2BCD_STANDALONE


#define P2BCDII_MAX_DIGITS    63
#define P2BCDII_MAX_TYPE      P2BCDIIMAKETYPE( P2BCDII_MAX_DIGITS, 1 ) // ������ 1 ����� ����� �������� �����.
// P2BCDII_MAX_TYPE - ���, ���������� ������������ ����� � ������ (�.�. ��� ������ ���� �������� �� ������)
// ������������ ��� �������� P2BCDIISIZE (�� � P2BCDIITL ������). ������ �� ��� ���� �� ������������ !!!

// N - total number of digits
// M - number of digits after decimal point (fractional part of number)

// ������, ����������� ��� �������� �������� ����, ������������� P2BCDIITypeForDouble()
#define P2BCDIIFROMDOUBLESIZETL 10 // ������ � 9, �� ��� �������� ���� ����� 10

#if 1

typedef unsigned int P2BCDIIType;
typedef UINT16 P2BCDIIStoreType;

// �������� �������� ���� P2BCDIIType ��� P2BCDIIStoreType ��� N (����� ����) � M (���-�� ���� ����� ���������� �����)
#define P2BCDIIMAKETYPE( N, M ) (P2BCDIIStoreType)( ( (N) << 8 ) | ( (M) & 0xFF ) )

// �� �������� ���� P2BCDIIType ��� P2BCDIIStoreType ������ ���-�� ���� ����� (N), ����� ����� (M), ������ ������, ����������� ��� �������� �����
#define P2BCDIITYPEN( TYPE )          ( (signed char)( (TYPE) >> 8 ) )
#define P2BCDIITYPEM( TYPE )          ( (signed char)( (TYPE) & 0xFF) )
//#define P2BCDIISIZETL( TYPE )         ( (TYPE)>>9 ) + ( ( (TYPE) | ((TYPE)>>8) ) & 1 ) // �� ������� �� �������� (�������)
#define P2BCDIISIZETL( TYPE )         ( P2BCDIITYPEN( TYPE ) ? ( (TYPE)>>9 ) + ( ( (TYPE) | ((TYPE)>>8) ) & 1 ) : 0 )// �� ������� �� �������� (�������)


#else // � ��� �� ������ ���� ��������� ����� ����� ���� :-)

typedef UINT32 P2BCDIIType;
typedef UINT32 P2BCDIIStoreType;

#define P2BCDIIMAKETYPE( N, M ) ( ( (N) << 16 ) | ( (M) & 0xFFFF ) )

#define P2BCDIITYPEN( TYPE )          ( (signed short)( (TYPE) >> 16 ) )
#define P2BCDIITYPEM( TYPE )          ( (signed short)( (TYPE) & 0xFFFF) )
#define P2BCDIISIZETL( TYPE )              ( (TYPE)>>17 ) + ( ( (TYPE) | ((TYPE)>>16) ) & 1 ) // �� ������� �� �������� (�������)


#endif





//#define P2BCDIIMAXDIGIT( __TYPE__ ) ( ( (__TYPE__) >> 16 ) - ( (__TYPE__) & 0x7FFF ) - 1 )
//#define P2BCDIIMINDIGIT( __TYPE__ ) ( -(INT32)( (__TYPE__) & 0x7FFF ) )



// ����������� ������� ��� ���������� ����. ��� ��� ������:

#define P2BCDII_RESULT_OVERFLOW      0x0001
#define P2BCDII_RESULT_ZERO          0x0002
#define P2BCDII_RESULT_NEGATIVE      0x0004
//#define P2BCDII_RESULT_CARRY       0x0008
#define P2BCDII_RESULT_BAD           0x0010
#define P2BCDII_OPERAND_BAD          0x0020
#define P2BCDII_RESULT_PREC          0x0040
#define P2BCDII_OPERAND_NAN          0x0080
#define P2BCDII_NOT_STRICT_FORMAT    0x0100


// ����������� ������� TL ��� ������������� ��������� ��������� �������� ��������.
// ��� ������� ����������
typedef enum {
    ROUND_NEAREST,  // � ����������  ( 12.4=>12 12.5=>13 12.6=>13 -12.4=>-12 12.5=>-13 -12.6=>-13 )
    ROUND_UP,       // ����� ( 12.4=>13 12.6=>13 -12.4=>-12 -12.6=>-12 )
    ROUND_DOWN,     // ���� ( 12.4=>12 12.6=>12 -12.4=>-13 -12.6=>-13 )
    ROUND_FROMZERO, //  ( 12.4=>13 12.6=>13 -12.4=>-13 -12.6=>-13 )
    ROUND_TRUNC    // ��������� ������ ����� (���������� � ����) ( 12.4=>12 12.6=>12 -12.4=>-12 -12.6=>-12 )
} roundstyle;

#define ROUND_TRUNK ROUND_TRUNC


// format flags
//
// ������ ��� ������������� ����� ��� �������������� ��� � ��������� ������.
// ������ ������ ��������, ����� �������� �� ������ �������� �� ������ ������ ����������.
#define P2BCDII_FORMAT_NOPLUS           0x0000      // ���� �� �������
#define P2BCDII_FORMAT_POSPLUS          0x0001      // ���� ������ � ������ ������������� �����
#define P2BCDII_FORMAT_NONEGPLUS        0x0003      // � ������������� � ����

#define P2BCDII_FORMAT_FILLONEZER       0x0000      // ������� ���� �� ��������, �� ���� ����� ����� �������, �� ���� ���� ��������
#define P2BCDII_FORMAT_FILLZERNO        0x0004      // ������� ���� �� ��������, ���� ����� ����� �������, �� ����� ���������� � �����
#define P2BCDII_FORMAT_FILLZERALL       0x0008      // �������� ������� ������� �����
//#define P2BCDII_FORMAT_                 0x000C      // reserved

#define P2BCDII_FORMAT_FILLSPNO         0x0000      // ��� ��������
#define P2BCDII_FORMAT_FILLSPZER        0x0010      // ������� ������� ����� �������� ���������
#define P2BCDII_FORMAT_FILLSPSIGN       0x0020      // ���� ����� ���, ������ ���� ������
#define P2BCDII_FORMAT_FILLSPSIGNZER    ( P2BCDII_FORMAT_FILLSPZER | P2BCDII_FORMAT_FILLSPSIGN )

#define P2BCDII_FORMAT_NOTFORCEPOINT    0x0000      // ������� ����� ������ ���� ���� ������� �����
#define P2BCDII_FORMAT_FORCEPOINT       0x0040      // ������� ����� ���� ���� ������� ���� ���
//#define P2BCDII_FORMAT_                 0x0080      // reserved


#define P2BCDII_FORMAT_MASKPLUS         0x0003
#define P2BCDII_FORMAT_MASKFILLZER      0x000C
#define P2BCDII_FORMAT_MASKFILLSP       0x0030
#define P2BCDII_FORMAT_MASKPOINT        0x00C0


// ������ ���������� ������, � ������� �������������� ���������� ������, ������������ ��������� P2BCDIIFormatTL()
// ���-�� ���� � ����� + �� ������ ����� ��� �����, ���������� ����� � ������������ ����
// ��. �������������� - ���������� ��� ������� ��� �� ��������� ���-�� ���� ����� ������� ������ ���������
#define P2BCDII_GETSTRSIZE_BYTYPE( bcdType ) ( P2BCDIITYPEN(bcdType) + 3 )


#ifdef  __cplusplus
extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////
// typeless BCD

#define P2BCDIITL( TYPE )               \
struct                                  \
{                                       \
    BYTE data[ P2BCDIISIZETL( TYPE ) ]; \
} // ������ �����������, �������� ������� - ��� (� �� ���������� ����)

#ifndef P2CALLSPEC
#define P2CALLSPEC
#endif

P2SYSTYPES_API BOOL P2CALLSPEC P2BCDIICheckTypeTL( P2BCDIIType type);
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIICheckTL( const void* bcd, P2BCDIIType type);
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIICopyTL(void* dst, P2BCDIIType dstType, const void* src, P2BCDIIType srcType, roundstyle rs);

P2SYSTYPES_API void P2CALLSPEC P2BCDIISetZeroTL( void* bcd, P2BCDIIType type );
P2SYSTYPES_API void P2CALLSPEC P2BCDIISetNANTL( void* bcd, P2BCDIIType type );
//P2SYSTYPES_API BOOL P2CALLSPEC P2BCDIIIsZeroTL( const void* bcd, P2BCDIIType type );
//#define P2BCDIISetNULLTL P2BCDIISetZeroTL // Hey, remove this
//#define P2BCDIIIsNULLTL P2BCDIIIsZeroTL // Hey, remove this
P2SYSTYPES_API int P2CALLSPEC P2BCDIISignTL( const void* bcd, P2BCDIIType type );
#define P2BCDIIIsZeroTL( bcd, type ) ( ! P2BCDIISignTL( (bcd), (type) ) )
P2SYSTYPES_API BOOL P2CALLSPEC P2BCDIIIsNANTL( const void* bcd, P2BCDIIType type );
P2SYSTYPES_API void P2CALLSPEC P2BCDIISetMinTL( void* bcd, P2BCDIIType type );
P2SYSTYPES_API void P2CALLSPEC P2BCDIISetMaxTL( void* bcd, P2BCDIIType type );

P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIAddTL( void* ret, P2BCDIIType retType, const void* a, P2BCDIIType aType, const void* b, P2BCDIIType bType, roundstyle rs);
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIISubTL( void* ret, P2BCDIIType retType, const void* a, P2BCDIIType aType, const void* b, P2BCDIIType bType, roundstyle rs );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIMulTL( void* ret, P2BCDIIType retType, const void* a, P2BCDIIType aType, const void* b, P2BCDIIType bType, roundstyle rs );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIDivTL( void* ret, P2BCDIIType retType, const void* u, P2BCDIIType uType, const void* v, P2BCDIIType vType, roundstyle rs );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIModTL( void* ret, P2BCDIIType retType, const void* u, P2BCDIIType uType, const void* v, P2BCDIIType vType, roundstyle rs );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIINegTL( void* bcd, P2BCDIIType type );
#define P2BCDIIIncTL( ret, retType, b, bType, rs ) P2BCDIIAddTL( (ret), (retType), (ret), (retType), (b), (bType), (rs) )
#define P2BCDIIDecTL( ret, retType, b, bType, rs ) P2BCDIISubTL( (ret), (retType), (ret), (retType), (b), (bType), (rs) )

P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIRoundTL( void* bcd, P2BCDIIType type, int fd, roundstyle rs );
P2SYSTYPES_API BOOL P2CALLSPEC P2BCDIIIsRoundTL( const void* bcd, P2BCDIIType type, int fd );


P2SYSTYPES_API int P2CALLSPEC P2BCDIICmpTL( const void* a, P2BCDIIType aType, const void* b, P2BCDIIType bType );
P2SYSTYPES_API int P2CALLSPEC P2BCDIIFastCmpTL( const void* a, const void* b, P2BCDIIType type );

P2SYSTYPES_API int P2CALLSPEC P2BCDIIGetDigitTL( const void* bcd, P2BCDIIType type, int n );

P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDII2Int64TL( INT64* pi64, const void* bcd, P2BCDIIType type, int fd, roundstyle rs );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIGetFromInt64TL( void* bcd, P2BCDIIType type, INT64 i64, int fd );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDII2UInt64TL( UINT64* pi64, const void* bcd, P2BCDIIType type, int fd, roundstyle rs );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIGetFromUInt64TL( void* bcd, P2BCDIIType type, UINT64 i64, int fd );

P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIGetFromWStrTL( void* bcd, P2BCDIIType type, const P2WCHAR_T* str, roundstyle rs, const P2WCHAR_T** strend );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIGetFromStrTL( void* bcd, P2BCDIIType type, const char* str, roundstyle rs, const char** strend );
P2SYSTYPES_API P2BCDERR P2CALLSPEC P2BCDIIFormatTL( char* dst, const void* bcd, P2BCDIIType type, UINT32 flag, int prec, roundstyle rs );
#define P2BCDII2STRTL( __str__, __bcd__, __P2BCDIIType__ ) P2BCDIIFormatTL( __str__, __bcd__, __P2BCDIIType__, 0, -1, ROUND_NEAREST )
P2SYSTYPES_API P2BCDERR P2CALLSPEC P2BCDIIWFormatTL( P2WCHAR_T* dst, const void* bcd, P2BCDIIType type, UINT32 flag, int prec, roundstyle rs );
#define P2BCDII2WSTRTL( __wstr__, __bcd__, __P2BCDIIType__ ) P2BCDIIWFormatTL( __wstr__, __bcd__, __P2BCDIIType__, 0, -1, ROUND_NEAREST )

#ifdef P2BCD_STANDALONE
P2SYSTYPES_API int P2CALLSPEC P2BCDIIFormatLightTL( char* dst, const void* bcd, P2BCDIIType type, UINT32 flag );
#endif // P2BCD_STANDALONE

P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDII2DoubleTL( double* dbl, const void* bcd, P2BCDIIType type );
P2SYSTYPES_API P2BCDIIStoreType P2BCDIITypeForDouble( double d );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIGetFromDoubleTL( void* bcd, P2BCDIIType type, double dbl );

P2SYSTYPES_API P2BCDERR P2CALLSPEC P2BCDIIGetFromBCDTL( void* bcd, P2BCDIIType type, const void *oldBcd, UINT32 oldType);
P2SYSTYPES_API P2BCDERR P2CALLSPEC P2BCDII2BCDTL( void *oldBcd, UINT32 oldType, const void* bcd, P2BCDIIType type );

P2SYSTYPES_API P2BCDIIType P2CALLSPEC P2BCDIIGetValueTypeTL(const void* bcd, P2BCDIIType type);
P2SYSTYPES_API P2BCDIIType P2CALLSPEC P2BCDIIGetValueTypeExTL(const void* bcd, P2BCDIIType type);

P2SYSTYPES_API UINT64 P2CALLSPEC P2BCDIICalcHashTL( const void* bcd, P2BCDIIType type );
// ������� ���������� �����, ������� ����� ������������ ��� ���������������� ��������� ���� ����� ������ ����
// ���� �������� ����� 0, ������������ 0x80000000, ���� �������� ����� �������������, ������������ ����� ������ ������,
// ���� ������������� - ������ ������. �������� ������� �� ������ �������, � ���� ��� �� ����� �������� ���������


////////////////////////////////////////////////////////////////////////////////
// typeful BCD


#define P2BCDIISIZETF(TYPE)  ( P2BCDIITYPEN( TYPE ) ? sizeof(P2BCDIIStoreType) + P2BCDIISIZETL(TYPE) : 0 )

#define P2BCDII(TYPE)                      \
    struct                                 \
    {                                      \
        BYTE data[ P2BCDIISIZETF(TYPE) ];  \
    } // ������ �����������, �������� ������� - ��� (� �� ���������� ����)


P2SYSTYPES_API void P2CALLSPEC P2BCDIIInit( void* dst, P2BCDIIType type );

P2SYSTYPES_API P2BCDIIType P2CALLSPEC P2BCDIIGetType( const void* bcd );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIICheck( const void* bcd );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIICopy( void* dst, const void* src );

P2SYSTYPES_API void P2CALLSPEC P2BCDIISetZero( void* bcd );
//#define P2BCDIISetNULLTL P2BCDIISetZeroTL // Hey, remove this
//#define P2BCDIIIsNULLTL P2BCDIIIsZeroTL // Hey, remove this
//#define P2BCDIISetNULL P2BCDIISetZero // Hey, remove this
P2SYSTYPES_API void P2CALLSPEC P2BCDIISetNAN( void* bcd );
//P2SYSTYPES_API BOOL P2CALLSPEC P2BCDIIIsZero( const void* bcd );
#define P2BCDIIIsZero( bcd ) ( ! P2BCDIISign( bcd ) )
P2SYSTYPES_API int P2CALLSPEC P2BCDIISign( const void* bcd_ );
P2SYSTYPES_API BOOL P2CALLSPEC P2BCDIIIsNAN( const void* bcd );
P2SYSTYPES_API void P2CALLSPEC P2BCDIISetMin( void* bcd );
P2SYSTYPES_API void P2CALLSPEC P2BCDIISetMax( void* bcd );

P2SYSTYPES_API P2BCDERR P2CALLSPEC P2BCDIIFormat( char* dst, const void* bcd, int precision, UINT32 flag );
#define P2BCDII2STR( __str__, __bcd__) P2BCDIIFormat( __str__, __bcd__, -1, 0 )
P2SYSTYPES_API P2BCDERR P2CALLSPEC P2BCDIIWFormat( P2WCHAR_T* dst, const void* bcd, int precision, UINT32 flag );
#define P2BCDII2WSTR( __wstr__, __bcd__) P2BCDIIWFormat( __wstr__, __bcd__, -1, 0 )

#ifdef P2BCD_STANDALONE
P2SYSTYPES_API int P2CALLSPEC P2BCDIIFormatLight( char* dst, const void* bcd, UINT32 flag );
#endif // P2BCD_STANDALONE

// ����������
// ��������� ��� ������������� �����������
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIAdd( void* ret, const void* a, const void* b );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIISub( void* ret, const void* a, const void* b );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIMul( void* ret, const void* a, const void* b );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIMulEx( void* ret, const void* a, const void* b, roundstyle rs );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIDiv( void* ret, const void* u, const void* v );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIDivEx( void* ret_, const void* u_, const void* v_, roundstyle rs );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIMod( void* ret, const void* u, const void* v );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIINeg( void* bcd );
#define P2BCDIIInc( ret, b ) P2BCDIIAdd( (ret), (ret), (b) )
#define P2BCDIIDec( ret, b ) P2BCDIISub( (ret), (ret), (b) )


// ����������
// fd - ���������� �������� ���� ����� �������.
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIRound( void* bcd, int fd, roundstyle rs );
P2SYSTYPES_API BOOL P2CALLSPEC P2BCDIIIsRound( const void* bcd, int fd );

// ���������
P2SYSTYPES_API int P2CALLSPEC P2BCDIICmp( const void* a, const void* b );

P2SYSTYPES_API int P2CALLSPEC P2BCDIIGetDigit( const void* bcd, int );
// ��������������
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDII2Int64( INT64* pi64, const void* bcd, int fd );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIGetFromInt64( void* bcd, INT64 i64, int fd );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDII2UInt64( UINT64* pi64, const void* bcd, int fd );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIGetFromUInt64( void* bcd, UINT64 i64, int fd );

P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIGetFromStr( void* bcd, const char* str );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIGetFromStrEx( void* bcd, const char* str, roundstyle rs, const char** strend );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIGetFromWStr( void* bcd, const P2WCHAR_T* str );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIGetFromWStrEx( void* bcd, const P2WCHAR_T* str, roundstyle rs, const P2WCHAR_T** strend );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDII2Double( double* dbl, const void* bcd);
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIGetFromDouble( void* bcd, double dbl );

P2SYSTYPES_API P2BCDERR P2CALLSPEC P2BCDII2BCD( void* oldBcd, UINT32 oldType, const void* bcd );
P2SYSTYPES_API P2BCDERR P2CALLSPEC P2BCDIIGetFromBCD( void* bcd, const void *oldBcd, UINT32 oldType );

// �������� ����������� ��� ���� ������� �����
P2SYSTYPES_API P2BCDIIType P2CALLSPEC P2BCDIIGetValueTypeEx(const void* bcd);
P2SYSTYPES_API P2BCDIIType P2CALLSPEC P2BCDIIGetValueType(const void* bcd);

P2SYSTYPES_API UINT64 P2CALLSPEC P2BCDIICalcHash( const void* bcd );

#ifdef  __cplusplus
}
#endif

#define P2BCDIISetNULLTL P2BCDIISetZeroTL // Hey, remove this
#define P2BCDIIIsNULLTL P2BCDIIIsZeroTL // Hey, remove this
#define P2BCDIISetNULL P2BCDIISetZero // Hey, remove this
#define P2BCDIIIsNULL P2BCDIIIsZero // Hey, remove this
// ��� ������� ����������� :-)

#endif //_P2DYSBCDII_H_


