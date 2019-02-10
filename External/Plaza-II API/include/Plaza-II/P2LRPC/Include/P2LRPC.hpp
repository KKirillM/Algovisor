#pragma once 

#include "../../../Plaza-II/P2Utils/Include/AutoHandle.hpp"
#include "P2LRPC.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef AutoP2Handle<P2LRPC_AREA_HANDLE, P2LRPCCloseArea> P2AutoLRPCAreaHandle;
typedef AutoP2Handle<P2LRPC_SERVER_AREA_HANDLE, P2LRPCServerCloseArea> P2AutoLRPCServerAreaHandle;
typedef AutoP2Handle<P2LRPC_CALL_HANDLE, P2LRPCCloseCall> P2AutoLRPCCallHandle;
typedef AutoP2Handle<P2LRPC_CALL_POOL_HANDLE, P2LRPCCloseCallPool> P2AutoLRPCCallPoolHandle;
