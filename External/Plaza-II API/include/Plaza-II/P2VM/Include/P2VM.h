#ifndef _P2VM_H_
#define _P2VM_H_

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef P2SYSEXT_EXPORTS
#define P2VM_API P2_API_EXPORTS
#else
#define P2VM_API P2_API_IMPORTS
#endif

#include "../../../Plaza-II/P2Common/Include/P2Types.h"
#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II/P2SysTypes/Include/P2SysTypes.h"
#include "../../P2SysRecord/Include/P2SysRecord.h"
#include "../../../Plaza-II/P2ACL/Include/P2ACL.h"

#ifdef P2SYSEXT_EXPORTS
  namespace NS_P2VM
  {
    struct P2VMByteCode;
    struct P2VMEnvironment;
    class CVariant;
  }
  using NS_P2VM::P2VMByteCode;
  using NS_P2VM::P2VMEnvironment;
  using NS_P2VM::CVariant;
#else
  struct P2VMByteCode;
  struct P2VMEnvironment;
  class CVariant;
#endif

typedef P2VMByteCode* P2VM_BYTECODE_HANDLE;
typedef P2VMEnvironment* P2VM_ENVIRONMENT_HANDLE;

//////////////////////////////////////////////////////////////////////
// P2VMVariant

typedef enum//TType
{
    P2VM_TP_NA_
    ,
    P2VM_TP_INT32,
    P2VM_TP_INT64,
    P2VM_TP_UINT32,
    P2VM_TP_UINT64,
    P2VM_TP_DOUBLE,
    P2VM_TP_STR_PTR,
    P2VM_TP_STR_COPY,
    P2VM_TP_BCD,
	P2VM_TP_DB_REC,
	P2VM_TP_ACL
} P2VMVariantType;

typedef struct //TStrPtr
{
    size_t    	len;
    PCSTR       pStr;
} P2VMVariantStrPtr;

typedef struct //TStrCopy
{
    size_t    len;
    PSTR        pStr;
} P2VMVariantStrCopy;

typedef struct
{
	P2BCDII(P2BCDII_MAX_TYPE) value;
} P2VMVariantBCD;

typedef union //TValue
{
    INT32               i32;
    INT64               i64;
    UINT32              u32;
    UINT64              u64;
    double              dbl;
    P2VMVariantStrPtr   str_ptr;
    P2VMVariantStrCopy  str_copy;
    P2VMVariantBCD      bcd;
	P2DB_RECORD_HANDLE	rec;
	P2ACL_HANDLE		acl;
    // DATE/TIME
} P2VMVariantValue;

typedef struct
{
    P2VMVariantType     m_type;
    P2VMVariantValue    m_value;
} P2VMVariant;

//P2VM_API P2ERR P2CALLSPEC P2VMVariantCreate(P2VMVariant** ppVar);
P2VM_API P2ERR P2CALLSPEC P2VMVariantFree(P2VMVariant* pVar);
// AllocStr()
// FreeStr()
// Copy() - ?

//////////////////////////////////////////////////////////////////////

typedef P2ERR (*FUNC_EXEC_EXTERN) (P2VM_BYTECODE_HANDLE pByteCode, P2VM_ENVIRONMENT_HANDLE env);
typedef P2ERR (*FUNC_INIT_EXTERN) (P2VM_BYTECODE_HANDLE pByteCode, const char* param);
typedef P2ERR (*FUNC_DEINIT_EXTERN) (P2VM_BYTECODE_HANDLE pByteCode);

P2VM_API P2ERR P2CALLSPEC P2VMCreateFromSource(P2VM_BYTECODE_HANDLE* pByteCode, PCSTR pSrc);
P2VM_API P2ERR P2CALLSPEC P2VMCreateFromBlock(P2VM_BYTECODE_HANDLE* pByteCode, const void* pBlock, size_t blockSize);
P2VM_API P2ERR P2CALLSPEC P2VMFree(P2VM_BYTECODE_HANDLE byteCode);
P2VM_API P2ERR P2CALLSPEC P2VMSaveToBlock(P2VM_BYTECODE_HANDLE byteCode, void* pBlock, /*in,out*/ size_t* pBlockSize);
P2VM_API P2ERR P2CALLSPEC P2VMExecute(P2VM_BYTECODE_HANDLE byteCode, P2VM_ENVIRONMENT_HANDLE env,
                                      P2VMVariant** ppRetVar /* = 0 */);


P2VM_API P2ERR P2CALLSPEC P2VMPushVariable(P2VM_BYTECODE_HANDLE pByteCode, P2VMVariantType type, const void* pValue);
P2VM_API P2ERR P2CALLSPEC P2VMPopVariable(P2VM_BYTECODE_HANDLE pByteCode, P2VMVariant** ppVar);
P2VM_API P2ERR P2CALLSPEC P2VMTopVariable(P2VM_BYTECODE_HANDLE pByteCode, P2VMVariant** ppVar);

#ifdef _DEBUG
P2VM_API P2ERR P2CALLSPEC P2VMDisAsm_(P2VM_BYTECODE_HANDLE byteCode);
#endif //_DEBUG

//////////////////////////////////////////////////////////////////////

P2VM_API P2ERR P2CALLSPEC P2VMCreateEnvironment(P2VM_ENVIRONMENT_HANDLE* pEnv);
P2VM_API P2ERR P2CALLSPEC P2VMFreeEnvironment(P2VM_ENVIRONMENT_HANDLE env);

P2VM_API P2ERR P2CALLSPEC P2VMEnvironmentSetVar(P2VM_ENVIRONMENT_HANDLE env, PCSTR pVarName,
                                                PCVOID pData, size_t dataLen, PCSTR p2type, UINT32 p2size, UINT32 p2scale);
//P2VM_API P2ERR P2CALLSPEC P2VMEnvironmentSetVar(P2VM_ENVIRONMENT_HANDLE env, PCSTR pVarName, P2VMVariant* pVar);
P2VM_API P2ERR P2CALLSPEC P2VMEnvironmentSetVarINT32(P2VM_ENVIRONMENT_HANDLE env, PCSTR pVarName, INT32 var);
P2VM_API P2ERR P2CALLSPEC P2VMEnvironmentSetVarINT64(P2VM_ENVIRONMENT_HANDLE env, PCSTR pVarName, INT64 var);
P2VM_API P2ERR P2CALLSPEC P2VMEnvironmentSetVarUINT32(P2VM_ENVIRONMENT_HANDLE env, PCSTR pVarName, UINT32 var);
P2VM_API P2ERR P2CALLSPEC P2VMEnvironmentSetVarUINT64(P2VM_ENVIRONMENT_HANDLE env, PCSTR pVarName, UINT64 var);
P2VM_API P2ERR P2CALLSPEC P2VMEnvironmentSetVarDOUBLE(P2VM_ENVIRONMENT_HANDLE env, PCSTR pVarName, double var);
P2VM_API P2ERR P2CALLSPEC P2VMEnvironmentSetVarSTR(P2VM_ENVIRONMENT_HANDLE env, PCSTR pVarName, PCSTR var);
P2VM_API P2ERR P2CALLSPEC P2VMEnvironmentSetVarBCD(P2VM_ENVIRONMENT_HANDLE env, PCSTR pVarName, const void* bcd, UINT32 type);
//P2VM_API P2ERR P2CALLSPEC P2VMEnvironmentSetVarTIME(P2VM_ENVIRONMENT_HANDLE env, PCSTR pVarName, const P2TIME* pVar);
P2VM_API P2ERR P2CALLSPEC P2VMEnvironmentGetVar(P2VM_ENVIRONMENT_HANDLE env, PCSTR pVarName, P2VMVariant** ppVar);

P2VM_API P2ERR P2CALLSPEC P2VMEnvironmentSetVarREC(P2VM_ENVIRONMENT_HANDLE env, PCSTR pVarName, P2DB_RECORD_HANDLE rec);
P2VM_API P2ERR P2CALLSPEC P2VMEnvironmentSetVarACL(P2VM_ENVIRONMENT_HANDLE env, PCSTR pVarName, P2ACL_HANDLE acl);

#ifdef  __cplusplus
}
#endif

#endif // _P2VM_H_
