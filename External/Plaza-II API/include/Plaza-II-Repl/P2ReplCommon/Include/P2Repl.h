#ifndef _P2REPL_H_
#define _P2REPL_H_

#include "../../../Plaza-II/P2Common/Include/P2Types.h"
#include "../../../Plaza-II-DB/P2DB/Include/P2DB.h"

// Required replication fields
#define P2REPL_ID_FIELD_INDEX           0
#define P2REPL_REVISION_FIELD_INDEX     1
#define P2REPL_DELETED_FIELD_INDEX      2
#define P2REPL_LAST_FIELD_INDEX         P2REPL_DELETED_FIELD_INDEX

// Required replication indexes
#define P2REPL_ID_INDEX_INDEX			0
#define P2REPL_REVISION_INDEX_INDEX		1

typedef INT64 P2REPL_ID_TYPE;
typedef INT64 P2REPL_REVISION_TYPE;
typedef P2REPL_ID_TYPE P2REPL_DELETED_TYPE;

#define P2REPL_ID_FIELD_SIZE sizeof(P2REPL_ID_TYPE)
#define P2REPL_REVISION_FIELD_SIZE sizeof(P2REPL_REVISION_TYPE)
#define P2REPL_DELETED_FIELD_SIZE sizeof(P2REPL_DELETED_TYPE)


#define P2REPL_ID_FIELD_FORMAT "i8"
#define P2REPL_REVISION_FIELD_FORMAT "i8"
#define P2REPL_DELETED_FIELD_FORMAT P2REPL_ID_FIELD_FORMAT

//TODO: lld or I64d???
#if defined(__linux__) || defined (__MACH__)
#define P2REPL_ID_PRINTF_FORMAT "%lld"
#define P2REPL_REVISION_PRINTF_FORMAT "%lld"
#else
#define P2REPL_ID_PRINTF_FORMAT "%I64d"
#define P2REPL_REVISION_PRINTF_FORMAT "%I64d"
#endif
#define P2REPL_DELETED_PRINTF_FORMAT P2REPL_ID_PRINTF_FORMAT

#define P2REPL_CHANNEL_HANDLE_PRINTF "%I64x"

#define P2DBGetIdFieldByIndex(rec, pval) P2RecordGetFieldByIndexINT64((rec), P2REPL_ID_FIELD_INDEX, (pval))
#define P2DBGetRevisionFieldByIndex(rec, pval) P2RecordGetFieldByIndexINT64((rec), P2REPL_REVISION_FIELD_INDEX, (pval))

// возможные предопределенные значения поля deleted
// существующая неудаленная запись
#define P2REPL_DELETED_EXIST PLATFORM_S64(0x0)
// добавляется в клиентские базы вместо i/u, если удаленные записи удаляются
// отказались от такого алгоритма
//#define P2REPL_DELETED_USELESS 0x7FFFFFFFFFFFFFFFL
// раньше константв были в enum, но VS 2003 отказывается делать 64-битовые енумы 

// право в ACL на открытие потока данных
#define P2REPL_ACL_OPEN_DATASTREAM "open"

// Константа ревижена возвращаемая при запросе первого ревижина пустой таблицы
// используется для удаления всех записей с меньшим ревиженом на клиенте в случае
// пустой таблицы на сервере (ClearDeleted)
#define P2REPL_MIN_REVISION		PLATFORM_S64(0x0)
#define P2REPL_MAX_REVISION		PLATFORM_S64(0x7FFFFFFFFFFFFFFF)

#define P2REPL_MAX_ADDRESS_SIZE		256

#define P2REPL_MAX_SERVER_NAME_SIZE 64

#define P2REPL_MAX_ACL_SIZE			        131072
#define P2REPL_MAX_FILTER_CODE_SIZE         7000

enum P2REPL_COMPRESSION_TYPE
{
	P2REPL_COMPRESSION_NONE,
	P2REPL_COMPRESSION_LZO1X  = 1, // deprecated
	P2REPL_COMPRESSION_SNAPPY,
    P2REPL_COMPRESSION_LZ4
};



#pragma pack(push, 4)

struct TReplRecHdr
{
    P2REPL_ID_TYPE			id;
	P2REPL_REVISION_TYPE	rev;
	P2REPL_DELETED_TYPE		deleted;
};

struct TACLRec
{
    P2REPL_ID_TYPE          id;
    P2REPL_REVISION_TYPE	rev;
    P2REPL_DELETED_TYPE		deleted;
    char                    server[P2REPL_MAX_ADDRESS_SIZE+1];
    char                    stream[P2REPL_MAX_SERVER_NAME_SIZE+1];
    BYTE                    ACL[P2REPL_MAX_ACL_SIZE];
};

//Отличие в том, что ACL имеет тип z
struct TACLRec2         
{
    P2REPL_ID_TYPE                  id;
    P2REPL_REVISION_TYPE	        rev;
    P2REPL_DELETED_TYPE		        deleted;
    char                            server[P2REPL_MAX_ADDRESS_SIZE+1];
    char                            stream[P2REPL_MAX_SERVER_NAME_SIZE+1];
    P2ZBLOCKN(P2REPL_MAX_ACL_SIZE)  ACL;
};

struct TFilterRec
{
	P2REPL_ID_TYPE          id;
	P2REPL_REVISION_TYPE	rev;
	P2REPL_DELETED_TYPE		deleted;
	char                    svr_name[P2REPL_MAX_SERVER_NAME_SIZE+1];
	char                    tbl_name[P2DB_MAX_TABLE_NAME_SIZE+1];
	BYTE                    code[P2REPL_MAX_FILTER_CODE_SIZE];
};

#pragma pack(pop)

//#define P2REPL_ACL_SERVER_FIELD_INDEX       2

//#define P2REPL_FILTER_SERVICE_FIELD_INDEX    2

#define P2REPL_REV_SCHEME_FIELD    "repl_rev"

#endif // _P2REPL_H_
