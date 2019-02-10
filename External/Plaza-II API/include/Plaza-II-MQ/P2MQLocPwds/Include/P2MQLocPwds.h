// P2MQLocPwds.h: interface for the P2MQLocPwds library
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_P2MQLOCPWDS_H__AACD2624_21D8_4EAC_845E_4398815E89E9__INCLUDED_)
#define AFX_P2MQLOCPWDS_H__AACD2624_21D8_4EAC_845E_4398815E89E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../Plaza-II/P2Common/Include/P2Errors.h"

//////////////////////////////////////////////////////////////////////
//
// For replace decryption code with stubs define macro 
// P2MQLOCPWDS_EXCLUDE_DECRYPT
// before this include file
//
#ifdef P2MQLOCPWDS_EXCLUDE_DECRYPT

#define Decrypt             DecryptStub__
#define DecryptFromHexStr   DecryptFromHexStrStub__

#else // !P2MQLOCPWDS_EXCLUDE_DECRYPT

#define Decrypt             Decrypt__
#define DecryptFromHexStr   DecryptFromHexStr__

#endif // P2MQLOCPWDS_EXCLUDE_DECRYPT

//////////////////////////////////////////////////////////////////////

namespace P2MqLocPwds
{

// Get max possible length of encrypted data
inline size_t GetMaxEncrLen(size_t data_len){ return data_len + 18; }

P2ERR Encrypt(const void* pSrc, size_t srcSize, void* pDest, size_t* /* in, out */ pDestSize);
// pSrc - may be not encrypted
P2ERR Decrypt__(const void* pSrc, size_t srcSize, void* pDest, size_t* /* in, out */ pDestSize);

P2ERR EncryptToHexStr(const void* pSrc, size_t srcSize, char* pDest, size_t* /* in, out */ pDestSize);
// pSrc - may be not in hex & not encrypted
P2ERR DecryptFromHexStr__(const char* pSrc, void* pDest, size_t* /* in, out */ pDestSize);

// Stubs for exclude decrypt code
P2ERR DecryptStub__(const void* pSrc, size_t srcSize, void* pDest, size_t* /* in, out */ pDestSize);
P2ERR DecryptFromHexStrStub__(const char* pSrc, void* pDest, size_t* /* in, out */ pDestSize);

};  // namespace P2MqLocPwds

#endif // !defined(AFX_P2MQLOCPWDS_H__AACD2624_21D8_4EAC_845E_4398815E89E9__INCLUDED_)
