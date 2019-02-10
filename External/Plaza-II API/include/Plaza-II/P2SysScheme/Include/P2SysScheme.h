#ifndef _P2SYSSCHEME_H_
#define _P2SYSSCHEME_H_

#include <string.h>

#ifdef __cplusplus
extern "C"{
#endif

#include "../../P2Common/Include/P2Errors.h"
#include "../../P2Common/Include/P2Types.h"
#include "../../../Plaza-II-MQ/P2MQ/Include/P2MQ.h"

#ifndef P2SCHEME_API
#ifdef P2SYS_EXPORTS
#define P2SCHEME_API P2_API_EXPORTS
#else
#define P2SCHEME_API P2_API_IMPORTS
#endif
#endif // #ifndef P2SCHEME_API

// Максимальные размеры с учетом завершающего нуля!
#define P2DB_MAX_DB_NAME_SIZE	        64
#define P2DB_MAX_SCHEME_NAME_SIZE	    64
#define P2DB_MAX_TABLE_NAME_SIZE	    64
#define P2DB_MAX_FIELD_NAME_SIZE	    64
#define P2DB_MAX_TYPE_NAME_SIZE	        64
#define P2DB_MAX_DEFAULT_VALUE_SIZE		128

// для работы с Item'ами схемы базы данных
#define P2SCHEME_ITEM_TYPE_ITEM			0
#define P2SCHEME_ITEM_TYPE_TABLE		1
#define P2SCHEME_ITEM_TYPE_VIEW			2

// флаг замены представления на таблицу
#define P2SCHEME_VIEW_TO_TABLE			1

//флаг для ф-ции P2SchemeSaveSchemeToBlock2
#define P2SCHEME_WITH_OTHER_FIELDS		2

// сохранять в блок только первые два индекса (точнее, P2SCHEME_N_REPL_INDEXES)
#define P2SCHEME_ONLY_REPL_INDICES		4

// флаги для ф-ции P2SchemeEqualSchemesEx
#define P2SCHEME_COMPARE_INDEXES                0x1
#define P2SCHEME_COMPARE_IGNORE_LIFENUM         0x2
#define P2SCHEME_COMPARE_CUSTOM_FIELDS          0x4
#define P2SCHEME_COMPARE_ALL			        0x8
#define P2SCHEME_COMPARE_IGNORE_SCHEME_NAME     0x10

#define P2SCHEME_N_REPL_INDEXES 		 2

#define P2SCHEME_URL_SEPARATOR '|'
#define P2SCHEME_URL_PREFIX_CUSTOMINI "FILE"
#define P2SCHEME_URL_PREFIX_SERVICE "MQ"
#define P2SCHEME_URL_DEFAULT_TIMEOUT 1000
#define P2SCHEME_URL_MAX_SIZE (MAX_PATH) + (P2DB_MAX_SCHEME_NAME_SIZE) + 10 // 10 (with reserve) for source type ("FILE" or "MQ") and 3 separators ('|')

// описание хэндлов объектов схемы
#ifdef P2SYS_EXPORTS
  namespace NS_P2SysScheme
  {
    class P2DBScheme;
    struct P2DBTableScheme;
    struct P2DBIndex;
    class P2DBField;
  }
  using NS_P2SysScheme::P2DBScheme;
  using NS_P2SysScheme::P2DBTableScheme;
  using NS_P2SysScheme::P2DBIndex;
  using NS_P2SysScheme::P2DBField;
  struct P2DBConnection;
#else
  struct P2DBScheme;
  typedef struct P2DBScheme P2DBScheme;
  struct P2DBTableScheme;
  typedef struct P2DBTableScheme P2DBTableScheme;
  struct P2DBIndex;
  typedef struct P2DBIndex P2DBIndex;
  struct P2DBConnection;
  typedef struct P2DBConnection P2DBConnection;
  struct P2DBField;
  typedef struct P2DBField P2DBField;
#endif

typedef P2DBScheme*       P2DB_SCHEME_HANDLE;
typedef P2DBTableScheme*  P2DB_TABLE_SCHEME_HANDLE;
typedef P2DBIndex*	    	P2DB_TABLE_INDEX_HANDLE;
typedef P2DBConnection*   P2DB_CONN_HANDLE;
////////////////////////////////////////////////////////////////////////////////

typedef P2ERR (P2CALLSPEC *P2DB_ENUMTABLES_CALLBACK)(P2DB_TABLE_SCHEME_HANDLE tblSch, void* cbParams);
typedef void (P2CALLSPEC *P2SCHEME_SCHEMEURLASYNC_CALLBACK)(P2ERR err, P2DB_SCHEME_HANDLE scheme, void* cbParams);
typedef P2ERR (*P2DB_ENUMCUSTOMFIELDS_CALLBACK)(const char* pKey, const char* pValue, void* cbParams);

typedef struct
{
	int index;
	char name[P2DB_MAX_FIELD_NAME_SIZE];
	char type[P2DB_MAX_TYPE_NAME_SIZE];
	UINT32 size;
	UINT32 precision;
} P2DBFieldInfo;

enum P2DBIndexFieldType
{
	Ascending,
	Descending
};

typedef enum P2DBIndexFieldType P2DBIndexFieldType;

struct P2DBIndexFieldInfo 
{
	char name[P2DB_MAX_FIELD_NAME_SIZE];
	P2DBIndexFieldType type;

#ifdef __cplusplus
	bool operator==(const P2DBIndexFieldInfo& oper2) const
	{
		return ((strcmp(name, oper2.name) == 0) && (type == oper2.type));
	};
#endif
};

typedef struct P2DBIndexFieldInfo P2DBIndexFieldInfo;


////////////////////////////////////////////////////////////////////////////////
// структуры, описывающие схему в памяти. pairs и sections заканчиваются записью с name == NULL
typedef struct P2DBIniPair
{
	const char * name;
	const char * value;
}
P2DBIniPair;

typedef struct P2DBIniSection
{
	const char * name;
	const P2DBIniPair * pairs;
}
P2DBIniSection;

typedef struct P2DBIniStruct
{
	const P2DBIniSection * sections;
}
P2DBIniStruct;



P2SCHEME_API P2ERR P2SchemeGetSchemeName(P2DB_SCHEME_HANDLE sch, char *schemeName, size_t *pSize);

P2SCHEME_API P2ERR P2SchemeEnumTablesFromScheme(P2DB_SCHEME_HANDLE sch, P2DB_ENUMTABLES_CALLBACK fncbk, void* cbParams);
P2SCHEME_API P2ERR P2SchemeCreateSchemeFromIni(P2DB_SCHEME_HANDLE *pScheme, const char *section);

P2SCHEME_API P2ERR P2SchemeCreateSchemeFromBlock(const void *block, size_t size, P2DB_SCHEME_HANDLE *pScheme);
P2SCHEME_API P2ERR P2SchemeSaveSchemeToBlock(P2DB_SCHEME_HANDLE scheme, void * block, size_t *pSize);

P2SCHEME_API P2ERR P2SchemeGetSchemeScheme(P2DB_SCHEME_HANDLE *pSch, const char *dbName, const char *tableName);
P2SCHEME_API P2ERR P2SchemeGetSchemeSchemeV2(P2DB_SCHEME_HANDLE *pSch, const char *dbName, const char *tableName);

P2SCHEME_API P2ERR P2SchemeCreateSchemeFromStruct(P2DB_SCHEME_HANDLE *pScheme, const P2DBIniStruct *ini, const char *section);

P2SCHEME_API P2ERR P2SchemeCreateReplScheme(P2DB_SCHEME_HANDLE src, P2DB_SCHEME_HANDLE *pDst);



P2SCHEME_API P2ERR P2SchemeGetTableCount(P2DB_SCHEME_HANDLE sch, size_t *pCount);
P2SCHEME_API P2ERR P2SchemeGetTableByName(P2DB_SCHEME_HANDLE sch, const char *tableName, P2DB_TABLE_SCHEME_HANDLE *ptbl);
P2SCHEME_API P2ERR P2SchemeGetTableByIndex(P2DB_SCHEME_HANDLE sch, size_t idx, P2DB_TABLE_SCHEME_HANDLE *ptbl);

P2SCHEME_API P2ERR P2SchemeGetTableName(P2DB_TABLE_SCHEME_HANDLE tblSch, char *tableName, size_t *pSize);
P2SCHEME_API P2ERR P2SchemeGetPrimaryTableName(P2DB_TABLE_SCHEME_HANDLE tblSch, char *tableName, size_t *pSize);
P2SCHEME_API P2ERR P2SchemeGetPrimaryKey(P2DB_TABLE_SCHEME_HANDLE tblSch, size_t *pPrimaryKeyField);

P2SCHEME_API P2ERR P2SchemeGetFieldCount(P2DB_TABLE_SCHEME_HANDLE tblSch, size_t *pCount);
P2SCHEME_API P2ERR P2SchemeGetFieldInfoByIndex(P2DB_TABLE_SCHEME_HANDLE tblSch, size_t idx, P2DBFieldInfo *pInfo);
P2SCHEME_API P2ERR P2SchemeGetFieldInfoByName(P2DB_TABLE_SCHEME_HANDLE tblSch, const char *name, P2DBFieldInfo *pInfo);
P2SCHEME_API P2ERR P2SchemeGetFieldIndexByName(P2DB_TABLE_SCHEME_HANDLE tblSch, const char * name, size_t *pIdx);

P2SCHEME_API P2ERR P2SchemeCopyTableScheme(P2DB_TABLE_SCHEME_HANDLE srcTblSch, P2DB_TABLE_SCHEME_HANDLE *pDstTblSch);
P2SCHEME_API P2ERR P2SchemeFreeTableScheme(P2DB_TABLE_SCHEME_HANDLE tblSch);
P2SCHEME_API P2ERR P2SchemeCopyScheme(P2DB_SCHEME_HANDLE srcSch, P2DB_SCHEME_HANDLE *pDstSch);
P2SCHEME_API P2ERR P2SchemeFreeScheme(P2DB_SCHEME_HANDLE dbSch);

P2SCHEME_API P2ERR P2SchemeGetFieldSource(P2DB_TABLE_SCHEME_HANDLE tblSch, size_t idx, char * buf, size_t *psize, BOOL withQuotes);

P2SCHEME_API BOOL P2SchemeEqualSchemes(P2DB_SCHEME_HANDLE sch1, P2DB_SCHEME_HANDLE sch2);
P2SCHEME_API BOOL P2SchemeEqualSchemesEx(P2DB_SCHEME_HANDLE sch1, P2DB_SCHEME_HANDLE sch2, UINT32 flag);

P2SCHEME_API P2ERR P2SchemeGetFieldMaxSizeByIndex(P2DB_TABLE_SCHEME_HANDLE tblSch, size_t idx, size_t *pSize);
P2SCHEME_API P2ERR P2SchemeGetTypeSizeInfo(P2DB_TABLE_SCHEME_HANDLE tblSch, size_t idx, UINT32 *pSize, UINT32 *pPrec, char *pType);

P2SCHEME_API P2ERR P2SchemeGetCustomField(P2DB_TABLE_SCHEME_HANDLE tblSch, const char *name, char * buf, size_t *pSize);
P2SCHEME_API P2ERR P2SchemeEnumCustomFields(P2DB_TABLE_SCHEME_HANDLE tblSch, P2DB_ENUMCUSTOMFIELDS_CALLBACK cbFunc, void* cbParams);

P2SCHEME_API P2ERR P2SchemeCreateRecordsetSchemeFromIni(const char *section, P2DB_TABLE_SCHEME_HANDLE *pRecScheme);

P2SCHEME_API P2ERR P2SchemeGetItemCount(P2DB_SCHEME_HANDLE sch, size_t *pCount, UINT32 item);
P2SCHEME_API P2ERR P2SchemeGetItemByName(P2DB_SCHEME_HANDLE sch, const char *tableName, P2DB_TABLE_SCHEME_HANDLE *ptbl, UINT32 item);
P2SCHEME_API P2ERR P2SchemeGetItemByIndex(P2DB_SCHEME_HANDLE sch, size_t idx, P2DB_TABLE_SCHEME_HANDLE *ptbl, UINT32 item);
P2SCHEME_API P2ERR P2SchemeEnumItemsFromScheme(P2DB_SCHEME_HANDLE sch, P2DB_ENUMTABLES_CALLBACK fncbk, void* cbParams, UINT32 item);

// internal functions
P2SCHEME_API P2ERR P2DBGetTableCreationSQL(P2DB_TABLE_SCHEME_HANDLE pScheme, char * buf, size_t *psize, P2DB_CONN_HANDLE sch);
P2SCHEME_API P2ERR P2DBGetTableDeleteSQL(P2DB_TABLE_SCHEME_HANDLE pScheme, char * buf, size_t *psize, P2DB_CONN_HANDLE sch);
P2SCHEME_API P2ERR P2DBMakeNewPtrTableScheme(P2DB_TABLE_SCHEME_HANDLE srcTblSch, P2DB_TABLE_SCHEME_HANDLE *pDstTblSch);
P2SCHEME_API P2ERR P2DBMakeNewPtrScheme(P2DB_SCHEME_HANDLE srcSch, P2DB_SCHEME_HANDLE *pDstSch);
P2SCHEME_API void DumpScheme(P2DB_SCHEME_HANDLE sch);

// Индексы схемы 
P2SCHEME_API P2ERR P2SchemeGetIndexCount(P2DB_TABLE_SCHEME_HANDLE tblSch, size_t *pCount);
P2SCHEME_API P2ERR P2SchemeGetIndexByIndex(P2DB_TABLE_SCHEME_HANDLE tblSch, size_t idx, P2DB_TABLE_INDEX_HANDLE *pTblIdx);
P2SCHEME_API P2ERR P2SchemeGetIndexName(P2DB_TABLE_INDEX_HANDLE tblIdx, char *indexName, size_t *pSize);
P2SCHEME_API P2ERR P2SchemeGetIndexTableName(P2DB_TABLE_INDEX_HANDLE tblIdx, char *indexTblName, size_t *pSize);
P2SCHEME_API P2ERR P2SchemeGetIndexInfo(P2DB_TABLE_INDEX_HANDLE tblIdx, BOOL *pIsUnique);
P2SCHEME_API P2ERR P2SchemeGetIndexFieldCount(P2DB_TABLE_INDEX_HANDLE tblIdx, size_t *pCount);
P2SCHEME_API P2ERR P2SchemeGetIndexFieldInfo(P2DB_TABLE_INDEX_HANDLE tblIdx, size_t idx, P2DBIndexFieldInfo *pInfo);
P2SCHEME_API P2ERR P2SchemeFreeIndex(P2DB_TABLE_INDEX_HANDLE tblIdx);

// Номер жизни схемы
P2SCHEME_API P2ERR P2SchemeGetLifeNum(P2DB_SCHEME_HANDLE sch, UINT32 *lifeNum);
P2SCHEME_API P2ERR P2SchemeSetLifeNum(P2DB_SCHEME_HANDLE sch, UINT32 lifeNum);
P2SCHEME_API P2ERR P2SchemeSetLifeNumToIni(P2DB_SCHEME_HANDLE sch, UINT32 lifeNum);
P2SCHEME_API P2ERR P2SchemeSetLifeNumToLocalUrl(const char* url, P2DB_SCHEME_HANDLE sch, UINT32 lifeNum);
P2SCHEME_API P2ERR P2SchemeCopySchemeWithNewLifeNum(P2DB_SCHEME_HANDLE src, P2DB_SCHEME_HANDLE* pDst, UINT32 lifeNum);
////////////////////////////////////////////////////////////////////////////////
P2SCHEME_API P2ERR P2SchemeSaveSchemeToBlock2(P2DB_SCHEME_HANDLE pScheme, void * block, size_t *pSize, UINT32 flag);

P2SCHEME_API P2ERR P2SchemeCreateSchemeFromIni2(P2DB_SCHEME_HANDLE *pScheme, const char* name, const char *section);
P2SCHEME_API P2ERR P2SchemeCreateSchemeFromIni3(P2DB_SCHEME_HANDLE *pScheme, P2INI_HANDLE ini, const char *section);
P2SCHEME_API P2ERR P2SchemeCreateRecordsetSchemeFromIni2(const char* name, const char *section, P2DB_TABLE_SCHEME_HANDLE *pRecScheme);
P2SCHEME_API P2ERR P2SchemeSetLifeNumToIni2(const char* name, P2DB_SCHEME_HANDLE sch, UINT32 lifeNum);

P2SCHEME_API P2ERR P2SchemeGetFieldDefValueByIndex(P2DB_TABLE_SCHEME_HANDLE tblSch, size_t idx, char * buf, size_t *pSize);
P2SCHEME_API P2ERR P2SchemeGetFieldDefValueByName(P2DB_TABLE_SCHEME_HANDLE tblSch, const char *name, char * buf, size_t *pSize);

P2SCHEME_API P2ERR P2SchemeCreateSchemeFromUrl(P2DB_SCHEME_HANDLE *pScheme, const char* url, P2MQ_CONN_HANDLE conn,
    UINT32 timeout, UINT32 flags);
P2SCHEME_API P2ERR P2SchemeCreateSchemeFromUrlAsync(const char *url, P2MQ_CONN_HANDLE conn, UINT32 timeout, UINT32 flags,
    P2SCHEME_SCHEMEURLASYNC_CALLBACK callback, void *cbParams);


#ifdef __cplusplus
};
#endif

#endif // _P2SYSSCHEME_H_
