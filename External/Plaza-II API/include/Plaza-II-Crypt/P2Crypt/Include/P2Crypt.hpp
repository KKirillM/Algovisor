#ifndef _P2CRYPT_HPP_
#define _P2CRYPT_HPP_

#include "../../../Plaza-II/P2Utils/Include/AutoHandle.hpp"
#include "P2Crypt.h"

////////////////////////////////////////////////////////////////////////////////
typedef AutoP2Handle<P2CRYPT_CTX_HANDLE, P2CryptContextFree> P2AutoCryptCtx;

#endif //_P2CRYPT_HPP_