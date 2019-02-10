#ifndef _P2UGLYBCD_H_
#define _P2UGLYBCD_H_

#include <typeinfo> // for std::bad_cast
#include "P2SysTypes.h"
////////////////////////////////////////////////////////////////////////////////
struct BCDX
{
	BCDX(UINT32 flags, const char *error = "") : m_flags(flags), m_opnum(0), m_error(error) { }
	BCDX(UINT32 flags, const void *bcd1, const char *error = "") : m_flags(flags), m_opnum(1), m_error(error)
	{
		P2BCDIIInit(&m_op1, P2BCDIIGetType(bcd1));
		P2BCDIICopy(&m_op1, bcd1); //ignore error
	}

	BCDX(UINT32 flags, const void *bcd1, const void *bcd2, const char *error = "") : m_flags(flags), m_opnum(2), m_error(error)
	{
		P2BCDIIInit(&m_op1, P2BCDIIGetType(bcd1));
		P2BCDIICopy(&m_op1, bcd1); //ignore error
		P2BCDIIInit(&m_op2, P2BCDIIGetType(bcd2));
		P2BCDIICopy(&m_op2, bcd2); //ignore error
	}

	UINT32 m_flags;
	UINT32 m_opnum;
	const char *m_error;
	P2BCDII(P2BCDII_MAX_TYPE) m_op1;
	P2BCDII(P2BCDII_MAX_TYPE) m_op2;
};

////////////////////////////////////////////////////////////////////////////////
template<int N, int M>
class CUglyBCD
{
public:
	//TODO: ctors
	enum 
	{
		bcdN = N,
		bcdM = M,
		bcdType = P2BCDIIMAKETYPE(N, M),
        bcdSize = P2BCDIISIZETF(bcdType),
		bcdErrorFlagsMask = ~(P2BCDII_RESULT_ZERO | P2BCDII_RESULT_NEGATIVE | P2BCDII_RESULT_PREC)
	};

	CUglyBCD(){ P2BCDIIInit(data, bcdType); }

    CUglyBCD<N, M>& Assign(const void * p)
    {
        UINT32 res = P2BCDIICopy(data, p);
        if ((res & bcdErrorFlagsMask) != 0)
        {
            throw BCDX(res);
        }
        return *this;
    }

    static const CUglyBCD<N, M>& Interpret(const void * p)
    {
        if (P2BCDIIGetType(p) != bcdType)
        {
#if defined(_WIN32)
            P2BCDIIType type_from = P2BCDIIGetType(p);
            char buf[256];
            P2SysSNPrintf(buf, sizeof(buf), "CUglyBCD bad cast from (%d,%d) to (%d,%d)",
                P2BCDIITYPEN(type_from), P2BCDIITYPEM(type_from), bcdN, bcdM);
            throw std::bad_cast(buf);
#else
            throw std::bad_cast();
#endif // _WIN32
        }
        return InterpretUnsafe(p);
    }

    // небезопасно, зато быстро
    static inline CUglyBCD<N, M>& InterpretUnsafe(void * p)
    {
        return *(static_cast< CUglyBCD<N, M>* >(p));
    }
    // небезопасно, зато быстро
    static inline const CUglyBCD<N, M>& InterpretUnsafe(const void * p)
    {
        return *(static_cast< const CUglyBCD<N, M>* >(p));
    }

	typedef UINT32 (*BINARY_FUNC)( void* ret, const void* a, const void* b );

	template<int N1, int M1, int N2, int M2, BINARY_FUNC fn>
	CUglyBCD<N, M>& BinaryOp(const CUglyBCD<N1, M1>& op1, const CUglyBCD<N2, M2>& op2, const char *errorText)
	{
		UINT32 res = fn(data, op1.data, op2.data);
		if ((res & bcdErrorFlagsMask) != 0)
		{
			throw BCDX(res, op1.data, op2.data, errorText);
		}
		return *this;
	}

	template<int N1, int M1, int N2, int M2>
	CUglyBCD<N, M>& Add(const CUglyBCD<N1, M1>& op1, const CUglyBCD<N2, M2>& op2)
	{
		return BinaryOp<N1, M1, N2, M2, P2BCDIIAdd>(op1, op2, "Addition error");
	}

	template<int N1, int M1, int N2, int M2>
	CUglyBCD<N, M>& Mul(const CUglyBCD<N1, M1>& op1, const CUglyBCD<N2, M2>& op2)
	{
		return BinaryOp<N1, M1, N2, M2, P2BCDIIMul>(op1, op2, "Multiply error");
	}

	template<int N1, int M1, int N2, int M2>
	CUglyBCD<N, M>& Sub(const CUglyBCD<N1, M1>& op1, const CUglyBCD<N2, M2>& op2)
	{
		return BinaryOp<N1, M1, N2, M2, P2BCDIISub>(op1, op2, "Substraction error");
	}

	template<int N1, int M1, int N2, int M2>
	CUglyBCD<N, M>& Div(const CUglyBCD<N1, M1>& op1, const CUglyBCD<N2, M2>& op2)
	{
		return BinaryOp<N1, M1, N2, M2, P2BCDIIDiv>(op1, op2, "Division error");
	}

	template<int N1, int M1, int N2, int M2>
	CUglyBCD<N, M>& Mod(const CUglyBCD<N1, M1>& op1, const CUglyBCD<N2, M2>& op2)
	{
		return BinaryOp<N1, M1, N2, M2, P2BCDIIMod>(op1, op2, "Modulo error");
	}
	
	template<int N1, int M1>
	CUglyBCD<N, M>& operator = (const CUglyBCD<N1, M1>& bcd2)
	{
		UINT32 res = P2BCDIICopy(data, bcd2.data);
		if ((res & bcdErrorFlagsMask) != 0)
		{
			throw BCDX(res);
		}
		return *this;
	}


	CUglyBCD<N, M>& operator = (double f)
	{
		UINT32 res = P2BCDIIGetFromDouble(data, f);
		if ((res & bcdErrorFlagsMask) != 0)
		{
			throw BCDX(res);
		}
		return *this;
	}

	CUglyBCD<N, M>& operator = (const char *str)
	{
		UINT32 res = P2BCDIIGetFromStr(data, str);
		if ((res & bcdErrorFlagsMask) != 0)
		{
			throw BCDX(res);
		}
		return *this;
	}
	CUglyBCD<N, M>& operator = (const wchar_t *str)
	{
		UINT32 res = P2BCDIIGetFromWStr(data, str);
		if ((res & bcdErrorFlagsMask) != 0)
		{
			throw BCDX(res);
		}
		return *this;
	}

	CUglyBCD<N, M>& operator = (int i) // для типа int заведен отдельный оператор, дабы литералы (типа int) присваивались без проблем
	{
		UINT32 res = P2BCDIIGetFromInt64(data, i, 0);
		if ((res & bcdErrorFlagsMask) != 0)
		{
			throw BCDX(res);
		}
		return *this;
	}

	CUglyBCD<N, M>& operator = (INT64 i)
	{
		UINT32 res = P2BCDIIGetFromInt64(data, i, 0);
		if ((res & bcdErrorFlagsMask) != 0)
		{
			throw BCDX(res);
		}
		return *this;
	}
	CUglyBCD<N, M>& operator = (UINT64 u)
	{
		UINT32 res = P2BCDIIGetFromUInt64(data, u, 0);
		if ((res & bcdErrorFlagsMask) != 0)
		{
			throw BCDX(res);
		}
		return *this;
	}
	
	double toDouble(void)const
	{
		double f;
		UINT32 res = P2BCDII2Double(&f, data);
		if ((res & bcdErrorFlagsMask) != 0)
		{
			throw BCDX(res);
		}
		return f;
	}

	INT64 toInt64(void)const
	{
		INT64 i;
		UINT32 res = P2BCDII2Int64(&i, data);
		if ((res & bcdErrorFlagsMask) != 0)
		{
			throw BCDX(res);
		}
		return i;
	}

	UINT64 toUInt64(void)const
	{
		INT64 i;
		UINT32 res = P2BCDII2UInt64(&i, data);
		if ((res & bcdErrorFlagsMask) != 0)
		{
			throw BCDX(res);
		}
		return i;
	}

	CUglyBCD<N, M>& Min(void)
	{
		P2BCDIISetMin(data);
		return *this;
	}

	CUglyBCD<N, M>& Max(void)
	{
		P2BCDIISetMax(data);
		return *this;
	}

	template<int N1, int M1>
	bool operator <(const CUglyBCD<N1, M1>& bcd2) const
	{
		return P2BCDIICmp(data, bcd2.data) < 0;
	}

	template<int N1, int M1>
	bool operator ==(const CUglyBCD<N1, M1>& bcd2) const
	{
		return P2BCDIICmp(data, bcd2.data) == 0;
	}

	template<int N1, int M1>
	bool operator >(const CUglyBCD<N1, M1>& bcd2) const
	{
		return P2BCDIICmp(data, bcd2.data) > 0;
	}

	template<int N1, int M1>
	bool operator !=(const CUglyBCD<N1, M1>& bcd2) const
	{
		return P2BCDIICmp(data, bcd2.data) != 0;
	}

	template<int N1, int M1, int N2, int M2>
	int Cmp(const CUglyBCD<N1, M1>& op1, const CUglyBCD<N2, M2>& op2) const
	{
		return P2BCDIICmp(op1.data, op2.data);
	}

	CUglyBCD<N, M>& SetZero(void)
	{
		P2BCDIISetZero(data);
		return *this;
	}

	bool IsZero() const
	{
		return (P2BCDIIIsZero(data) == TRUE);
	}

	/*
		typedef enum {
		ROUND_NEAREST,  // к ближайшему  ( 12.4=>12 12.5=>13 12.6=>13 -12.4=>-12 12.5=>-13 -12.6=>-13 )
		ROUND_UP,       // вверх ( 12.4=>13 12.6=>13 -12.4=>-12 -12.6=>-12 )
		ROUND_DOWN,     // вниз ( 12.4=>12 12.6=>12 -12.4=>-13 -12.6=>-13 )
		ROUND_FROMZERO, //  ( 12.4=>13 12.6=>13 -12.4=>-13 -12.6=>-13 )
		ROUND_TRUNC    // отбросить лишние цифры (округление к нулю) ( 12.4=>12 12.6=>12 -12.4=>-12 -12.6=>-12 )
		} roundstyle;
	*/
	CUglyBCD<N, M>& Round(int digitsNumAfterComma, roundstyle rs) 
	{
		UINT32 res = P2BCDIIRound(data, digitsNumAfterComma, rs);
		if ((res & bcdErrorFlagsMask) != 0)
		{
			throw BCDX(res);
		}
		return *this;
	}

	int Sign() const
	{
        return P2BCDIISign(data);
	}

	CUglyBCD<N, M>& Neg(void)
	{
		UINT32 res = P2BCDIINeg(data);
		if ((res & bcdErrorFlagsMask) != 0)
		{
			throw BCDX(res);
		}
		return *this;
	}

	BYTE data[ bcdSize ];
};

/*
P2SYSTYPES_API P2BCDIIType P2CALLSPEC P2BCDIIGetType( const void* bcd );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIICheck( const void* bcd );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIICopy( void* dst, const void* src );

P2SYSTYPES_API void P2CALLSPEC P2BCDIISetZero( void* bcd );
P2SYSTYPES_API void P2CALLSPEC P2BCDIISetNAN( void* bcd );
#define P2BCDIIIsZero( bcd ) ( ! P2BCDIISign( bcd ) )
P2SYSTYPES_API BOOL P2CALLSPEC P2BCDIISign( const void* bcd_ );
P2SYSTYPES_API BOOL P2CALLSPEC P2BCDIIIsNAN( const void* bcd );

P2SYSTYPES_API P2ERR P2CALLSPEC P2BCDIIFormat( char* dst, const void* bcd, int precision, UINT32 flag );
#define P2BCDII2STR( __str__, __bcd__) P2BCDIIFormat( __str__, __bcd__, -1, 0 )

P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIINeg( void* bcd );
#define P2BCDIIInc( ret, b ) P2BCDIIAdd( (ret), (ret), (b) )
#define P2BCDIIDec( ret, b ) P2BCDIISub( (ret), (ret), (b) )


// округления
// fd - количество значащих цифр после запятой.
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIRound( void* bcd, int fd, roundstyle rs );
P2SYSTYPES_API BOOL P2CALLSPEC P2BCDIIIsRound( const void* bcd, int fd );

// сравнения
P2SYSTYPES_API int P2CALLSPEC P2BCDIICmp( const void* a, const void* b );

P2SYSTYPES_API int P2CALLSPEC P2BCDIIGetDigit( const void* bcd, int );

P2SYSTYPES_API P2ERR P2CALLSPEC P2BCDII2BCD( void* oldBcd, UINT32 oldType, const void* bcd );
P2SYSTYPES_API P2ERR P2CALLSPEC P2BCDIIGetFromBCD( void* bcd, const void *oldBcd, UINT32 oldType );

// получить минимальный тип куда влезает число
P2SYSTYPES_API P2BCDIIType P2CALLSPEC P2BCDIIGetValueTypeEx(const void* bcd);
P2SYSTYPES_API P2BCDIIType P2CALLSPEC P2BCDIIGetValueType(const void* bcd);

P2SYSTYPES_API UINT64 P2CALLSPEC P2BCDIICalcHash( const void* bcd );

*/


#endif
