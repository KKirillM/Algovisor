#ifndef _NATIVEFILTERSUPPORT_H_INCLUDED_
#define _NATIVEFILTERSUPPORT_H_INCLUDED_

#include "../../../Plaza-II/P2Common/Include/P2Windows.h"
#include "../../../Plaza-II/P2SysTypes/Include/P2SysTypes.hpp"
#include "../../../Plaza-II/P2SysRecord/Include/P2SysRecord.hpp"
#include "../../../Plaza-II/P2ACL/Include/P2ACL.hpp"
#include "./IComposable.h"

#define NATIVE_FILTER_BLOCK_SIGNATURE 0x544C464E // "NFLT"
#define NATIVE_FILTER_BLOCK_FORMAT "u4ccc" // signature, category, dll_name, filter_func_name
#define NATIVE_FILTER_PROPERTIES_FORMAT "u4" // filter properties

#define NATIVE_FILTER_INITFUNC   "NativeFilter_Init"    // init function name of native filter
#define NATIVE_FILTER_INITFUNCV2 "NativeFilter_InitV2"  // init function name of native filter (version 2)
#define NATIVE_FILTER_DEINITFUNC "NativeFilter_Deinit"  // deinit function name of native filter

#define NATIVE_FILTER_ADDCHANNELFUNC "NativeFilter_AddChannel" // function is called when new channel connected
#define NATIVE_FILTER_DELETECHANNELFUNC "NativeFilter_DeleteChannel" // function is called when channel will be disconnected
#define NATIVE_FILTER_CHANGEACLFUNC "NativeFilter_ChangeACL" // function is called when server ACL was changed
#define NATIVE_FILTER_ENUMERATEFUNC "NativeFilter_Enumerate" // enumerating function


#define NATIVE_FILTER_CATEGORY "P2Repl_0"

typedef P2ERR (__stdcall *FnNativeFilterExec) (P2DB_RECORD_HANDLE rec, P2ACL_HANDLE acl, const char * pFrom, /*OUT*/BOOL * pResult);
typedef P2ERR (__stdcall *FnNativeFilterInit) (void);
typedef P2ERR (__stdcall *FnNativeFilterInitV2) (P2DB_SCHEME_HANDLE filterScheme, void * hSrv, const char * serverName, const void * pReserved);
typedef P2ERR (__stdcall *FnNativeFilterDeinit) (void);

// функции поддержки фильтров с собственным перечислителем
typedef void (__stdcall *FnNativeFilterEnumerate) (P2DB_RECORD_HANDLE rec, size_t tableNo, const void *);
typedef void (__stdcall *FnNativeFilterAddChannel) (IComposable* pChannel);
typedef void (__stdcall *FnNativeFilterDeleteChannel) (IComposable* pChannel);
typedef P2ERR (__stdcall *FnNativeFilterChangeACL) (P2ACL_HANDLE acl);

// filter properties
#define NATIVE_FILTER_PROP__CLIENT_INDEPENDENT 0x0001 // фильтр не зависит от клиента
#define NATIVE_FILTER_PROP__ENUMERATIVE        0x0002 // поддерживает функции для оптимизированного перечисления клиентов
#define NATIVE_FILTER_PROP__NON_MODIFYING      0x0004 // не модифицирует запись

#endif // _NATIVEFILTERSUPPORT_H_INCLUDED_
