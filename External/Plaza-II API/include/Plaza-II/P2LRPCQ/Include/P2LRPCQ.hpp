#ifndef _P2LRPCQ_HPP_
#define _P2LRPCQ_HPP_

#ifndef __MACH__

#include "../../../Plaza-II/P2Utils/Include/AutoHandle.hpp"
#include "P2LRPCQ.h"

typedef AutoP2Handle<P2LRPCQ_SERVER_HANDLE, P2LRPCQFreeServer> 	P2AutoLRPCQServer;
typedef AutoP2Handle<P2LRPCQ_HANDLE, P2LRPCQFree>				P2AutoLRPCQChannel;

#endif //__MACH__

#endif //< _P2LRPCQ_HPP_>