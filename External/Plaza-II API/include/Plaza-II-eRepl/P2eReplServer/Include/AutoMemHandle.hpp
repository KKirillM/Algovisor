#ifndef _AUTOMEMHANDLE_HPP_
#define _AUTOMEMHANDLE_HPP_

#include <stdlib.h>
#include "../../../Plaza-II/P2Utils/Include/AutoHandle.hpp"

typedef char * P2MemHandle;

inline P2ERR P2MemFree(P2MemHandle pMem)
{
    if (NULL != pMem)
    {
        free(pMem);
    }
    return P2ERR_OK;
}

typedef AutoP2Handle< P2MemHandle, P2MemFree > AutoMemHandle;

#endif // _AUTOMEMHANDLE_HPP_
