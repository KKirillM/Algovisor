#ifndef _P2SYSBCDIIODBC_H_
#define _P2SYSBCDIIODBC_H_

#include "P2SysBCDII.h"
// Do not use ODBC in unix branch at all.
#ifdef _WIN32
#include <SQLTYPES.H>
#include <oleauto.h>

#ifdef  __cplusplus
extern "C" {
#endif

P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDII2SqlNumericStructTL( SQL_NUMERIC_STRUCT* data, const void* bcd, P2BCDIIType type );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIGetFromSqlNumericStructTL( void* bcd, P2BCDIIType type, const SQL_NUMERIC_STRUCT* data );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDII2DECIMALTL( DECIMAL* data, const void* bcd, P2BCDIIType type );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIGetFromDECIMALTL( void* bcd, P2BCDIIType type, const DECIMAL* data );

P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDII2SqlNumericStruct( SQL_NUMERIC_STRUCT* data, const void* bcd_ );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIGetFromSqlNumericStruct( void* bcd_, const SQL_NUMERIC_STRUCT* data );
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDII2DECIMAL(DECIMAL* data, const void* bcd_);
P2SYSTYPES_API UINT32 P2CALLSPEC P2BCDIIGetFromDECIMAL(void* bcd_, const DECIMAL* d);

#ifdef  __cplusplus
}
#endif
#endif // _WIN32

#endif // _P2SYSBCDIIODBC_H_
