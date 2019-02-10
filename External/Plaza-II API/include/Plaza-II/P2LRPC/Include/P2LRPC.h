#ifndef _P2LRPC_H_
#define _P2LRPC_H_

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef P2SYSEXT_EXPORTS
#define P2LRPC_API __declspec(dllexport)
#else
#define P2LRPC_API __declspec(dllimport)
#endif

#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II/P2SysScheme/Include/P2SysScheme.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define DEFAULT_CALL_EXEC_TIMEOUT        10
#define DEFAULT_WAIT_TIMEOUT             1000
#define MAX_CONCURRENT_CALL_COUNT        (MAXIMUM_WAIT_OBJECTS-10)
#define DEFAULT_CONCURRENT_CALL_COUNT    10


////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace NS_P2LRPC
{
class CClientArea;
class CServerArea;
class CCall;
class CCallPool;
}
using NS_P2LRPC::CClientArea;
using NS_P2LRPC::CServerArea;
using NS_P2LRPC::CCall;
using NS_P2LRPC::CCallPool;

typedef CClientArea* P2LRPC_AREA_HANDLE;
typedef CServerArea* P2LRPC_SERVER_AREA_HANDLE;
typedef CCall* P2LRPC_CALL_HANDLE;
typedef CCallPool* P2LRPC_CALL_POOL_HANDLE;

typedef UINT32    P2LRPC_SIZE_TYPE;

////////////////////////////////////////////////////////////////////////////////////////////////////////////

//client API
P2LRPC_API P2ERR  P2CALLSPEC P2LRPCStartup(void);
P2LRPC_API void  P2CALLSPEC P2LRPCCleanup(void);

P2LRPC_API P2ERR  P2CALLSPEC P2LRPCOpenArea(P2LRPC_SIZE_TYPE areaSize, P2DB_SCHEME_HANDLE hDbScheme, unsigned callExecTimeout, P2LRPC_AREA_HANDLE* ppArea);
P2LRPC_API P2ERR  P2CALLSPEC P2LRPCOpenAreaEx(P2DB_SCHEME_HANDLE hDbScheme, unsigned callExecTimeout, P2LRPC_AREA_HANDLE* ppArea);

P2LRPC_API P2ERR  P2CALLSPEC P2LRPCCloseArea(P2LRPC_AREA_HANDLE hArea);

P2LRPC_API P2ERR  P2CALLSPEC P2LRPCCreateCall(P2LRPC_AREA_HANDLE hArea, size_t index, P2LRPC_CALL_HANDLE* ppCall);
P2LRPC_API P2ERR  P2CALLSPEC P2LRPCCloseCall(P2LRPC_CALL_HANDLE hCall);

P2LRPC_API P2ERR  P2CALLSPEC P2LRPCGetCallParametersPointer(P2LRPC_CALL_HANDLE hCall, void** ppMem);

P2LRPC_API P2ERR  P2CALLSPEC P2LRPCSyncCall(P2LRPC_CALL_HANDLE hCall);

P2LRPC_API P2ERR  P2CALLSPEC P2LRPCAsyncCall(P2LRPC_CALL_HANDLE hCall, HANDLE* ppWaitEvent);

P2LRPC_API P2ERR  P2CALLSPEC P2LRPCAllocBuffer(P2LRPC_AREA_HANDLE hArea, P2LRPC_SIZE_TYPE memSize, void** pPtr);

P2LRPC_API P2ERR  P2CALLSPEC P2LRPCFreeBuffer(P2LRPC_AREA_HANDLE hArea, void* ptr);

P2LRPC_API void*  P2CALLSPEC P2LRPCGetBaseAddress(P2LRPC_AREA_HANDLE hArea);

P2LRPC_API P2ERR  P2CALLSPEC P2LRPCGetFreeMemoryInfo(P2LRPC_AREA_HANDLE hArea, P2LRPC_SIZE_TYPE* pTotalFreeMem, P2LRPC_SIZE_TYPE* pMaxFreeBlockSize);

P2LRPC_API P2ERR  P2CALLSPEC P2LRPCCreateCallPool(P2LRPC_AREA_HANDLE hArea, size_t index, size_t poolCapacity, P2LRPC_CALL_POOL_HANDLE* ppCallPool);

P2LRPC_API P2ERR  P2CALLSPEC P2LRPCCloseCallPool(P2LRPC_CALL_POOL_HANDLE hCallPool);

P2LRPC_API P2ERR  P2CALLSPEC P2LRPCGetCallFromPool(P2LRPC_CALL_POOL_HANDLE hCallPool, P2LRPC_CALL_HANDLE* ppCall);

P2LRPC_API P2ERR  P2CALLSPEC P2LRPCGetCallFromPoolEx(P2LRPC_CALL_POOL_HANDLE hCallPool, UINT32 timeout, P2LRPC_CALL_HANDLE* ppCall);

P2LRPC_API P2ERR  P2CALLSPEC P2LRPCReturnCallToPool(P2LRPC_CALL_HANDLE hCall);

//server API

typedef P2ERR (P2CALLSPEC *P2LRPC_CALL_HANDLER)(void* pMem, void* cbParams);

P2LRPC_API P2ERR  P2CALLSPEC P2LRPCServerStartup(void); 
P2LRPC_API void   P2CALLSPEC P2LRPCServerCleanup(void);

P2LRPC_API P2ERR  P2CALLSPEC P2LRPCServerCreateArea(P2LRPC_SIZE_TYPE areaSize, P2DB_SCHEME_HANDLE hDbScheme, P2LRPC_SERVER_AREA_HANDLE* ppServerArea);
P2LRPC_API P2ERR  P2CALLSPEC P2LRPCServerCreateAreaEx(P2LRPC_SIZE_TYPE areaSize, P2DB_SCHEME_HANDLE hDbScheme, unsigned concurrentCallCount, P2LRPC_SERVER_AREA_HANDLE* ppServerArea);

P2LRPC_API P2ERR  P2CALLSPEC P2LRPCServerCloseArea(P2LRPC_SERVER_AREA_HANDLE hServerArea);

P2LRPC_API P2ERR  P2CALLSPEC P2LRPCServerRegisterHandlerByIndex(P2LRPC_SERVER_AREA_HANDLE hServerArea, size_t idx, P2LRPC_CALL_HANDLER cbFunc, void* cbParam);

P2LRPC_API P2ERR  P2CALLSPEC P2LRPCServerGetWaitHandle(P2LRPC_SERVER_AREA_HANDLE hServerArea, HANDLE* pEvent); 

P2LRPC_API P2ERR  P2CALLSPEC P2LRPCServerProcessIncomingCall(P2LRPC_SERVER_AREA_HANDLE hServerArea);

P2LRPC_API P2ERR  P2CALLSPEC P2LRPCServerAllocBuffer(P2LRPC_SERVER_AREA_HANDLE hServerArea, P2LRPC_SIZE_TYPE memSize, void** pPtr);

P2LRPC_API P2ERR  P2CALLSPEC P2LRPCServerFreeBuffer(P2LRPC_SERVER_AREA_HANDLE hServerArea, void* ptr);

P2LRPC_API void*  P2CALLSPEC P2LRPCServerGetBaseAddress(P2LRPC_SERVER_AREA_HANDLE hServerArea);

P2LRPC_API P2ERR  P2CALLSPEC P2LRPCServerGetFreeMemoryInfo(P2LRPC_SERVER_AREA_HANDLE hServerArea, P2LRPC_SIZE_TYPE* pTotalFreeMem, P2LRPC_SIZE_TYPE* pMaxFreeBlockSize);


////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG

	inline void* P2OffsetToPtr(P2LRPC_AREA_HANDLE hArea, P2LRPC_SIZE_TYPE offset)
	{
		void* basePtr = P2LRPCGetBaseAddress(hArea);
        return (static_cast<BYTE*>(basePtr) + offset);
	}

	inline P2LRPC_SIZE_TYPE P2PtrToOffset(P2LRPC_AREA_HANDLE hArea, void* ptr)
	{
		void* basePtr = P2LRPCGetBaseAddress(hArea);
        return static_cast<P2LRPC_SIZE_TYPE>(static_cast<BYTE*>(ptr) - static_cast<BYTE*>(basePtr));
	}

	inline void* P2ServerOffsetToPtr(P2LRPC_SERVER_AREA_HANDLE hServerArea, P2LRPC_SIZE_TYPE offset)
	{
		void* basePtr = P2LRPCServerGetBaseAddress(hServerArea);
        return (static_cast<BYTE*>(basePtr) + offset);
	}

	inline P2LRPC_SIZE_TYPE P2ServerPtrToOffset(P2LRPC_SERVER_AREA_HANDLE hServerArea, void* ptr)
	{
		void* basePtr = P2LRPCServerGetBaseAddress(hServerArea);
        return static_cast<P2LRPC_SIZE_TYPE>(static_cast<BYTE*>(ptr) - static_cast<BYTE*>(basePtr));
	}

	#define P2_OFFSET_TO_PTR(hArea, offset) (P2OffsetToPtr(hArea, offset))
	#define P2_PTR_TO_OFFSET(hArea, ptr) (P2PtrToOffset(hArea, ptr))

	#define P2_SERVER_OFFSET_TO_PTR(hServerArea, offset) (P2ServerOffsetToPtr(hServerArea, offset))
	#define P2_SERVER_PTR_TO_OFFSET(hServerArea, ptr) (P2ServerPtrToOffset(hServerArea, ptr))

#else

	#define P2_OFFSET_TO_PTR(hArea, offset) (static_cast<BYTE*>(P2LRPCGetBaseAddress(hArea)) + offset)
	#define P2_PTR_TO_OFFSET(hArea, ptr) (static_cast<BYTE*>(ptr) - static_cast<BYTE*>(P2LRPCGetBaseAddress(hArea)))

	#define P2_SERVER_OFFSET_TO_PTR(hServerArea, offset) (static_cast<BYTE*>(P2LRPCServerGetBaseAddress(hServerArea)) + offset)
	#define P2_SERVER_PTR_TO_OFFSET(hServerArea, ptr) (static_cast<BYTE*>(ptr) - static_cast<BYTE*>(P2LRPCServerGetBaseAddress(hServerArea)))

#endif //_DEBUG


#ifdef  __cplusplus
}
#endif

#endif //_P2LRPC_H_
