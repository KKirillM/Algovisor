#ifndef _P2ACL_HPP_
#define _P2ACL_HPP_

#include "../../../Plaza-II/P2Utils/Include/AutoHandle.hpp"
#include "P2ACL.h"

////////////////////////////////////////////////////////////////////////////////

typedef AutoP2Handle<P2ACL_HANDLE, P2ACLFree> P2AutoACLHandle;
typedef AutoP2Handle<P2ACL_ADMIN_HANDLE, P2ACLAdmFree> P2AutoACLAdmHandle;
//typedef AutoP2Handle<P2ACL_ROLE_HANDLE, P2ACLAdmFreeRole> P2AutoACLRoleHandle;

////////////////////////////////////////////////////////////////////////////////

#endif // _P2ACL_HPP_
