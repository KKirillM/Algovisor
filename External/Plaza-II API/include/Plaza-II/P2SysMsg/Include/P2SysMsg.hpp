#ifndef _P2SYSMSG_HPP_
#define _P2SYSMSG_HPP_

#include "../../../Plaza-II/P2Utils/Include/AutoHandle.hpp"
#include "P2SysMsg.h"
////////////////////////////////////////////////////////////////////////////////
typedef AutoP2Handle<P2MSG_HANDLE, P2MsgFreeMessage> P2AutoMessage;
typedef AutoP2Handle<P2_ParseState *, P2MsgFreeParse> P2AutoParse;
typedef AutoP2Handle<P2_ComposeState *, P2MsgFreeCompose> P2AutoCompose;

#endif
