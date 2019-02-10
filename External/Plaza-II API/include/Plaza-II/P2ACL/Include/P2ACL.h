#ifndef _P2ACL_H_
#define _P2ACL_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include "../../P2Common/Include/P2Types.h"
#include "../../P2Common/Include/P2Errors.h"
#include "../../P2SysParser/Include/P2SysParser.h"
#include "../../../Plaza-II/P2SysIni/Include/P2SysIni.h"

#ifdef P2SYSEXT_EXPORTS
#define P2ACL_API P2_API_EXPORTS
#else
#define P2ACL_API P2_API_IMPORTS
#endif

#ifdef P2SYSEXT_EXPORTS
  namespace NS_P2ACL
  {
    struct P2ACL;
    struct P2ACLAdmin;
  }
  using NS_P2ACL::P2ACL;
  using NS_P2ACL::P2ACLAdmin;
#else
  struct P2ACL;
  struct P2ACLAdmin;
#endif

typedef P2ACL* P2ACL_HANDLE;
typedef P2ACLAdmin* P2ACL_ADMIN_HANDLE;
//struct P2ACLRole;
//typedef P2ACLRole* P2ACL_ROLE_HANDLE;

//////////////////////////////////////////////////////////////////////

P2ACL_API P2ERR P2CALLSPEC P2ACLCreate(P2ACL_HANDLE* pAcl);
P2ACL_API P2ERR P2CALLSPEC P2ACLCreateFromIni(P2INI_HANDLE pIni, PCSTR sect, P2ACL_HANDLE* pAcl);
P2ACL_API P2ERR P2CALLSPEC P2ACLFree(P2ACL_HANDLE acl);

//P2ACL_API P2ERR P2CALLSPEC P2ACLSaveToBlock(P2ACL_HANDLE acl, PVOID pBlock, UINT32* pSize);
//P2ACL_API P2ERR P2CALLSPEC P2ACLCreateFromBlock(P2ACL_HANDLE* pAcl, PCVOID pBlock, UINT32 size);
P2ACL_API P2ERR P2CALLSPEC P2ACLSaveToBlock(P2ACL_HANDLE acl, P2SYS_COMPOSER_HANDLE composer);
P2ACL_API P2ERR P2CALLSPEC P2ACLCreateFromBlock(P2ACL_HANDLE* pAcl, P2SYS_PARSER_HANDLE parser);

P2ACL_API P2ERR P2CALLSPEC P2ACLCreateFromAdm(P2ACL_ADMIN_HANDLE admAcl, P2ACL_HANDLE* pAcl);

P2ACL_API P2ERR P2CALLSPEC P2ACLMakeNewPtr(P2ACL_HANDLE srcAcl, P2ACL_HANDLE* pDstAcl);

//////////////////////////////////////////////////////////////////////

P2ACL_API P2ERR P2CALLSPEC P2ACLAdmCreate(P2ACL_ADMIN_HANDLE* pAdmAcl);
P2ACL_API P2ERR P2CALLSPEC P2ACLAdmFree(P2ACL_ADMIN_HANDLE admAcl);

//P2ACL_API P2ERR P2CALLSPEC P2ACLAdmSaveToBlock(P2ACL_ADMIN_HANDLE admAcl, PVOID pBlock, UINT32* pSize);
//P2ACL_API P2ERR P2CALLSPEC P2ACLAdmCreateFromBlock(P2ACL_ADMIN_HANDLE* pAdmAcl, PCVOID pBlock, UINT32 size);
P2ACL_API P2ERR P2CALLSPEC P2ACLAdmSaveToBlock(P2ACL_ADMIN_HANDLE admAcl, P2SYS_COMPOSER_HANDLE composer);
P2ACL_API P2ERR P2CALLSPEC P2ACLAdmCreateFromBlock(P2ACL_ADMIN_HANDLE* pAdmAcl, P2SYS_PARSER_HANDLE parser);

P2ACL_API P2ERR P2CALLSPEC P2ACLAdmAddRole(P2ACL_ADMIN_HANDLE admAcl, PCSTR pRoleName);

P2ACL_API P2ERR P2CALLSPEC P2ACLAdmRoleGrantRight(P2ACL_ADMIN_HANDLE admAcl, PCSTR pRoleName, PCSTR pRight);
P2ACL_API P2ERR P2CALLSPEC P2ACLAdmRoleDenyRight(P2ACL_ADMIN_HANDLE admAcl, PCSTR pRoleName, PCSTR pRight);
P2ACL_API P2ERR P2CALLSPEC P2ACLAdmRoleRevokeRight(P2ACL_ADMIN_HANDLE admAcl, PCSTR pRoleName, PCSTR pRight);
P2ACL_API P2ERR P2CALLSPEC P2ACLAdmRoleAddRole(P2ACL_ADMIN_HANDLE admAcl, PCSTR pRoleNameTo, PCSTR pRoleNameAdd);

P2ACL_API P2ERR P2CALLSPEC P2ACLAdmAddUser(P2ACL_ADMIN_HANDLE admAcl, PCSTR pUser);

P2ACL_API P2ERR P2CALLSPEC P2ACLAdmUserGrantRight(P2ACL_ADMIN_HANDLE admAcl, PCSTR pUser, PCSTR pRight);
P2ACL_API P2ERR P2CALLSPEC P2ACLAdmUserDenyRight(P2ACL_ADMIN_HANDLE admAcl, PCSTR pUser, PCSTR pRight);
P2ACL_API P2ERR P2CALLSPEC P2ACLAdmUserRevokeRight(P2ACL_ADMIN_HANDLE admAcl, PCSTR pUser, PCSTR pRight);
P2ACL_API P2ERR P2CALLSPEC P2ACLAdmUserAddRole(P2ACL_ADMIN_HANDLE admAcl, PCSTR pUser, PCSTR pRoleNameAdd);

//////////////////////////////////////////////////////////////////////

P2ACL_API P2ERR P2CALLSPEC P2ACLCheckRight(P2ACL_HANDLE acl, PCSTR pUser, PCSTR pRight, BOOL* pRes);

enum P2ACL_ORDER
{
	P2ACLDUMP_ACL_ORDER_BY_USERS = 1,	// этот порядок работает быстрее чем BY_RIGHT
	P2ACLDUMP_ACL_ORDER_BY_RIGHTS,
	P2ACLDUMP_ACL_POOR_MAN_ORDER		// дамп выводится в формате, совместимом с P2ACLCreateFromIni
};

P2ACL_API P2ERR P2CALLSPEC P2ACLDumpACL(P2ACL_HANDLE acl, UINT32 order);
P2ACL_API P2ERR P2CALLSPEC P2ACLCopyACL(P2ACL_HANDLE * dest, P2ACL_HANDLE source);

typedef P2ERR (P2CALLSPEC *pfnEnumRightsCbk)(PCSTR user, PCSTR right, void* cbParam);
P2ACL_API P2ERR P2CALLSPEC P2ACLEnumRights(P2ACL_HANDLE acl, PCSTR min, PCSTR max, pfnEnumRightsCbk fn, void *cbParam, UINT32 flags);

//////////////////////////////////////////////////////////////////////

#ifdef  __cplusplus
}
#endif

#endif // _P2ACL_H_
