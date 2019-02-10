#ifndef _P2DLL_HPP_
#define _P2DLL_HPP_
 
#include "P2Dll.h"
#include "../../P2Utils/Include/AutoHandle.hpp"
 
typedef AutoP2Handle< P2DLL_HANDLE, P2DLLFreeLibrary > P2AutoFreeLibrary;
 
#endif // _P2DLL_HPP_
