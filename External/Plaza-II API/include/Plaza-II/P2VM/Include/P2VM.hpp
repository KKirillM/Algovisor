#ifndef _P2VM_HPP_
#define _P2VM_HPP_

#include "../../../Plaza-II/P2Utils/Include/AutoHandle.hpp"
#include "P2VM.h"

////////////////////////////////////////////////////////////////////////////////

typedef AutoP2Handle<P2VM_BYTECODE_HANDLE, P2VMFree> P2AutoVMByteCodeHandle;
typedef AutoP2Handle<P2VM_ENVIRONMENT_HANDLE, P2VMFreeEnvironment> P2AutoVMEnvironmentHandle;
typedef AutoP2Handle<P2VMVariant*, P2VMVariantFree> P2AutoVMVariantPtr;

////////////////////////////////////////////////////////////////////////////////

#endif // _P2VM_HPP_
