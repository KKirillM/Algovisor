#ifndef AFX__IRECORD_H__9CA84E0C_24B3_4145_9A73_911BAD2D910A__INCLUDED_
#define AFX__IRECORD_H__9CA84E0C_24B3_4145_9A73_911BAD2D910A__INCLUDED_

#include "../../P2Utils/Include/IRc.h"
#include "../../P2SysScheme/Include/P2SysScheme.h"

struct P2DBRecord  : public IRc
{
	// число полей в записи
	virtual P2ERR GetFieldCount(size_t *pCount) = 0;

    // получение полей при наличии схемы
	//virtual P2ERR GetField(const char *name, void *outBuf, UINT32 *pSize) = 0;
	//virtual P2ERR GetField(int idx, void *outBuf, UINT32 *pSize) = 0;
	//дополнительно возвращают признак того, что значение поля содержит NULL 
	virtual P2ERR GetFieldEx(const char *name, void *outBuf, size_t *pSize, BOOL* isNull) = 0;
	virtual P2ERR GetFieldEx(size_t idx, void *outBuf, size_t *pSize, BOOL* isNull) = 0;
	// запись поля по имени
	virtual P2ERR SetField(const char *name, const void *inBuf, size_t len) = 0;
	virtual P2ERR SetFieldByIndex(size_t idx, const void *inBuf, size_t len) = 0;
	virtual P2ERR SetFieldNull(const char *name) = 0;
	virtual P2ERR SetFieldNullByIndex(size_t idx) = 0;

	// функции для извлечения данных из записи без схемы
	//virtual P2ERR GetField(int idx, void *outBuf, UINT32 *pSize, const char *p2type, UINT32 p2size, UINT32 prec = 0) = 0;
	virtual P2ERR GetFieldEx(size_t idx, void *outBuf, size_t *pSize, BOOL* isNull, const char *p2type, UINT32 p2size, UINT32 prec = 0) = 0;

	virtual P2ERR CopyToStruct(void *out_buf, size_t *plen) = 0;
	virtual P2ERR CopyFromStruct(const void *out_buf, size_t len) = 0;

	virtual P2ERR GetScheme(P2DB_TABLE_SCHEME_HANDLE *ptblSch) = 0;
	//virtual P2ERR GetConnection(P2DB_CONN_HANDLE *pConn) = 0;

	virtual P2ERR InsertRec(void) = 0;
	virtual P2ERR UpdateRec(void) = 0;
	virtual P2ERR DeleteRec(void) = 0;
	
	virtual P2ERR PrepareForUpdate(void) = 0;
	virtual P2ERR PrepareForInsert(void) = 0;

	virtual P2ERR Clear(void) = 0;

	virtual P2ERR PrepareForUpdateEx(size_t* whereIndexes, size_t cnt) = 0;

	virtual P2ERR GetInternalBuffers(void** ppDataBuf, UINT8** ppNullsBuf) = 0;

    // быстрая проверка на NULL
    virtual P2ERR IsFieldNullByIndex(size_t idx, BOOL* pIsNull) = 0;
};

#endif  // AFX__IRECORD_H__9CA84E0C_24B3_4145_9A73_911BAD2D910A__INCLUDED_