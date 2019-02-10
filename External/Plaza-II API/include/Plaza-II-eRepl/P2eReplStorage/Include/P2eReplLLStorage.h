#ifndef _P2EREPLLLSTORAGE_H__INCLUDED_
#define _P2EREPLLLSTORAGE_H__INCLUDED_

#include "./P2eReplStorage.h"
#include "./IP2eReplLLStorage.hpp"

#ifdef P2EREPLSTORAGE_EXPORTS
#define P2EREPLSTORAGE_API P2_API_EXPORTS
#else
#define P2EREPLSTORAGE_API P2_API_EXPORTS
#endif

#ifdef  __cplusplus
extern "C" {
#endif

P2EREPLSTORAGE_API P2ERR P2CALLSPEC P2eReplStorageGetLLStorage(P2EREPL_STORAGE_HANDLE hStorage, IP2eReplLLStorage ** ppLowLevelStorage);

#ifdef  __cplusplus
}
#endif

#endif // _P2EREPLLLSTORAGE_H__INCLUDED_
