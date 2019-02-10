#ifndef __P2_AUTHSERVER_NS__
#define __P2_AUTHSERVER_NS__

#include "../../../Plaza-II/P2SysTypes/Include/P2SysTypes.h"
#include "../../../Plaza-II/P2SysMsg/Include/P2SysMsg.h"

#pragma pack(push, 4)
// Scheme: 'Uzr', table: 'Uzr' ///////////////////////////

typedef struct
{
    INT64 id;
    INT64 rev;
    INT64 act;
    char uzr[256];
    BYTE pwd[288];
    char status[2];
    char info[128];
    char data[256];
	char salt[9];
	char hash[129];
} Uzr_Uzr;

#define UZR_DEF "i8i8i8c256b288c2c128c256"
// Scheme: 'Uzr', table: 'UzrAddr' ///////////////////////////

typedef struct
{
    INT64 id;
    INT64 rev;
    INT64 act;
    INT64 uzrId;
    INT32 ip;
    INT32 mask;
} Uzr_UzrAddr;

enum  P2_CTL_NS_CMD_REC
{
	P2_CTL_NS_insert,
	P2_CTL_NS_update,
	P2_CTL_NS_delete,
	P2_CTL_NS_insertEx,
#define P2_CTL_NS_UZRADDR_BEGIN  (0xff)
	P2_CTL_NS_UzrAddr_insert = P2_CTL_NS_UZRADDR_BEGIN,
	P2_CTL_NS_UzrAddr_update,
	P2_CTL_NS_UzrAddr_delete,
#define P2_CTL_NS_UZER_ROLES_BEGIN  P2_CTL_NS_UZER_ROLES_INSERT
	P2_CTL_NS_UZER_ROLES_INSERT,
	P2_CTL_NS_UZER_ROLES_UPDATE,
	P2_CTL_NS_UZER_ROLES_DELETE,
#define P2_CTL_NS_CONN_RULES_BEGIN  P2_CTL_NS_CONN_RULES_INSERT
	P2_CTL_NS_CONN_RULES_INSERT,
	P2_CTL_NS_CONN_RULES_UPDATE,
	P2_CTL_NS_CONN_RULES_DELETE
};

struct UserRoles
{
	INT64 id;
    INT64 rev;
    INT64 act;
	char  uzr[256];
	char  group[256];
};

struct ConnPolicy
{
	INT64 id;
    INT64 rev;
    INT64 act;
	char  group_from[256];
	char  group_to[256];	
};







#pragma pack(pop)


#define ROLE_ANY			"ANY"
#define ROLE_ACCEPTOR_NONE  "ACCEPTOR_NONE"
#define ROLE_INITIATOR_NONE "INITIATOR_NONE"

// Определение является инициатор доверенным узлом. 
// для того, чтобы узел был доверенным поле 'data' должно содержать подстроку  $trusted$.
#define NODE_ATTRIB_TRUSTED		  "$trusted$"
#define NODE_ATTRIB_UNTRUSTED 	  "$untrusted$"

inline bool IsNodeDataHasTrustedAttribute(const char* pNodeData)
{
	return strstr(pNodeData, NODE_ATTRIB_TRUSTED) != NULL;
}

inline bool IsNodeDataHasUnTrustedAttribute(const char* pNodeData)
{
	return strstr(pNodeData, NODE_ATTRIB_UNTRUSTED) != NULL;
}

#define UZRADDR_DEF "u8u8u8u8u4u4"


#define	P2_AUTH_NS_PROTOCOL_VERSION				MAKELONG(0, MAKEWORD(0, 1))

#define	P2_AUTH_NS_PROP_CATEGORY_VAL			"P2_AUTH_NS"
#define	P2_CTL_NS_PROP_CATEGORY_VAL				"P2_CTL_NS"

// u4 - protocol version
// u4 - cryptographic algorithm, u4 - cryptographic mode
// c - Acceptor's name, u8 - Acceptor random number, b - encrypted data of P2_AUTH_NS_AUTH_REQUEST_B_FMT "type"
#define P2_AUTH_NS_AUTH_REQUEST_FMT				"u4u4u4cu8b"

// u4 - protocol version between Initiator and Acceptor
// u4 - ip address of initiator
// c - Initiator's name, u8 - Initiator random number, b - timemark of Acceptor
#define P2_AUTH_NS_AUTH_REQUEST_B_FMT			"u4u4cu8b"

// b - encrypted data of P2_AUTH_NS_INITIATOR_TOKEN_FMT "type",
// b - encrypted data of P2_AUTH_NS_ACCEPTOR_TOKEN_FMT "type"
#define P2_AUTH_NS_POSITIVE_REPLY_FMT			"bb"

// u4 - protocol version between Initiator and Acceptor
// c - Acceptor's name, u8 - Initiator random number, b - Acceptor's timemark, b - session key,
// b - side information
#define P2_AUTH_NS_INITIATOR_TOKEN_FMT			"u4cu8bbbc"

// // u4 - protocol version (for more proof against cheating)
// c - Initiator's name, b - Acceptor's timemark, b - session key,
// b - side information
#define P2_AUTH_NS_ACCEPTOR_TOKEN_FMT			"u4cbbb"

#define P2_AUTH_ALGO_BLOWFISH   8
#define P2_AUTH_ALGO_MODE_CFB   3


inline bool P2IsAuthNSMesage(P2MSG_HANDLE msg, P2ERR &err)
{
	const char *category;
	if (P2FAILEDV(P2MsgGetStrField(msg, P2MSG_PROP_CATEGORY, &category), err))
		return false;

	return strcmp(category, P2_AUTH_NS_PROP_CATEGORY_VAL) == 0;
}
inline bool P2IsCtlNSMesage(P2MSG_HANDLE msg, P2ERR &err)
{
	const char *category;
	if (P2FAILEDV(P2MsgGetStrField(msg, P2MSG_PROP_CATEGORY, &category), err))
		return false;

	return strcmp(category, P2_CTL_NS_PROP_CATEGORY_VAL) == 0;
}
inline bool P2IsSystem(P2MSG_HANDLE msg, P2ERR &err)
{
	const char *category;
	if (P2FAILEDV(P2MsgGetStrField(msg, P2MSG_PROP_CATEGORY, &category), err))
		return false;

	return strcmp(category, P2MSG_CATEGORY_SYSTEM) == 0;
}


#endif // __P2_AUTHSERVER_NS__
