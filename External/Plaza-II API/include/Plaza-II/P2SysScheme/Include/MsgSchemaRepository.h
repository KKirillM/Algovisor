#if !defined(MSGSCHEMAREPOSITORY_H__03650F83_EB4A_4D54_8D0B_DA6A44B45CA5__INCLUDED_)
#define MSGSCHEMAREPOSITORY_H__03650F83_EB4A_4D54_8D0B_DA6A44B45CA5__INCLUDED_

#define P2MSG_CATEGORY_SCHEMAREPOSITORY "_SchemaRepository_"

#define P2MSG_SCHEMANAME_FIELD "schema_name"
#define P2MSG_SCHEMA_FIELD "schema_data"
#define P2MSG_SCHEMA_ERRORCODE "error_code"

enum
{
    P2MSG_TYPE_SCHEMAREPOSITORY_GET_SCHEMA = 0,
    P2MSG_TYPE_SCHEMAREPOSITORY_ADD_SCHEMA = 1,
    P2MSG_TYPE_SCHEMAREPOSITORY_UPDATE_SCHEMA = 2,
    P2MSG_TYPE_SCHEMAREPOSITORY_REMOVE_SCHEMA = 3,
    P2MSG_TYPE_SCHEMAREPOSITORY_REPLY = 10
};

enum
{
    P2MSG_CODE_REPLY_SCHEMAREPOSITORY_OK = 0,
    P2MSG_CODE_REPLY_SCHEMAREPOSITORY_ERROR_NOT_FOUND = 1,
    P2MSG_CODE_REPLY_SCHEMAREPOSITORY_ERROR_ALREADY_EXISTS = 2,
    P2MSG_CODE_REPLY_SCHEMAREPOSITORY_ERROR_WRONGARGS = 3,
    P2MSG_CODE_REPLY_SCHEMAREPOSITORY_ERROR_UNKNOWNERROR = 5
};

#endif // !defined(MSGSCHEMAREPOSITORY_H__03650F83_EB4A_4D54_8D0B_DA6A44B45CA5__INCLUDED_)

