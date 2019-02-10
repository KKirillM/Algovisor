#ifndef _P2SYSRECORD_H_
#define _P2SYSRECORD_H_

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef P2SYS_EXPORTS
#define P2SYSRECORD_API P2_API_EXPORTS
#else
#define P2SYSRECORD_API P2_API_IMPORTS
#endif

#include "../../P2Common/Include/P2Errors.h"
#include "../../P2SysScheme/Include/P2SysScheme.h"

//////////////////////////////////////////////////////////////////////

#ifdef P2SYS_EXPORTS
  namespace NS_P2SysRecord
  {
    class CRecord;
  }
  using NS_P2SysRecord::CRecord;
#else
  class CRecord;
#endif

struct P2DBRecord;
typedef P2DBRecord* P2DB_RECORD_HANDLE;

//////////////////////////////////////////////////////////////////////

P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordFree(P2DB_RECORD_HANDLE rec);

P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetSchemeFromRecord(P2DB_RECORD_HANDLE rec, P2DB_TABLE_SCHEME_HANDLE *ptblSch);

P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldsCount(P2DB_RECORD_HANDLE rec, size_t *pCount);

P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetField(P2DB_RECORD_HANDLE rec, const char *name, void *outBuf, size_t *pSize);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldINT8(P2DB_RECORD_HANDLE rec, const char *name, INT8 *val);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldINT16(P2DB_RECORD_HANDLE rec, const char *name, INT16 *val);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldINT32(P2DB_RECORD_HANDLE rec, const char *name, INT32 *val);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldINT64(P2DB_RECORD_HANDLE rec, const char *name, INT64 *val);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldUINT8(P2DB_RECORD_HANDLE rec, const char *name, UINT8 *val);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldUINT16(P2DB_RECORD_HANDLE rec, const char *name, UINT16 *val);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldUINT32(P2DB_RECORD_HANDLE rec, const char *name, UINT32 *val);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldUINT64(P2DB_RECORD_HANDLE rec, const char *name, UINT64 *val);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldP2TIME(P2DB_RECORD_HANDLE rec, const char *name, P2TIME *val);

P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldByIndex(P2DB_RECORD_HANDLE rec, size_t idx, void *out_buf, size_t *pSize);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldByIndexINT8(P2DB_RECORD_HANDLE rec, size_t idx, INT8 *val);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldByIndexINT16(P2DB_RECORD_HANDLE rec, size_t idx, INT16 *val);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldByIndexINT32(P2DB_RECORD_HANDLE rec, size_t idx, INT32 *val);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldByIndexINT64(P2DB_RECORD_HANDLE rec, size_t idx, INT64 *val);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldByIndexUINT8(P2DB_RECORD_HANDLE rec, size_t idx, UINT8 *val);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldByIndexUINT16(P2DB_RECORD_HANDLE rec, size_t idx, UINT16 *val);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldByIndexUINT32(P2DB_RECORD_HANDLE rec, size_t idx, UINT32 *val);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldByIndexUINT64(P2DB_RECORD_HANDLE rec, size_t idx, UINT64 *val);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldByIndexP2TIME(P2DB_RECORD_HANDLE rec, size_t idx, P2TIME *val);

P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldByIndexWithoutScheme(P2DB_RECORD_HANDLE rs, size_t idx, void *out_buf, size_t *pSize, const char *p2type, UINT32 p2size, UINT32 p2prec);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldByIndexWithoutSchemeINT8(P2DB_RECORD_HANDLE rec, size_t idx, INT8 *val);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldByIndexWithoutSchemeINT16(P2DB_RECORD_HANDLE rec, size_t idx, INT16 *val);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldByIndexWithoutSchemeINT32(P2DB_RECORD_HANDLE rec, size_t idx, INT32 *val);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldByIndexWithoutSchemeINT64(P2DB_RECORD_HANDLE rec, size_t idx, INT64 *val);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldByIndexWithoutSchemeUINT8(P2DB_RECORD_HANDLE rec, size_t idx, UINT8 *val);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldByIndexWithoutSchemeUINT16(P2DB_RECORD_HANDLE rec, size_t idx, UINT16 *val);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldByIndexWithoutSchemeUINT32(P2DB_RECORD_HANDLE rec, size_t idx, UINT32 *val);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldByIndexWithoutSchemeUINT64(P2DB_RECORD_HANDLE rec, size_t idx, UINT64 *val);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldByIndexWithoutSchemeP2TIME(P2DB_RECORD_HANDLE rec, size_t idx, P2TIME *val);

P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordSetField(P2DB_RECORD_HANDLE rs, const char *name, const void *buf, size_t len);
/*#define P2RecordSetFieldTypeVal__(rec, name, val) P2RecordSetField(rec, name, &(val), sizeof(val))
#define P2RecordSetFieldTypePtr__(rec, name, pVal) P2RecordSetField(rec, name, pVal, sizeof(*(pVal)))
#define P2RecordSetFieldINT8(rec, name, val) P2RecordSetFieldTypeVal__(rec, name, (INT8)(val))
#define P2RecordSetFieldINT16(rec, name, val) P2RecordSetFieldTypeVal__(rec, name, (INT16)(val))
#define P2RecordSetFieldINT32(rec, name, val) P2RecordSetFieldTypeVal__(rec, name, (INT32)(val))
#define P2RecordSetFieldINT64(rec, name, val) P2RecordSetFieldTypeVal__(rec, name, (INT64)(val))
#define P2RecordSetFieldUINT8(rec, name, val) P2RecordSetFieldTypeVal__(rec, name, (UINT8)(val))
#define P2RecordSetFieldUINT16(rec, name, val) P2RecordSetFieldTypeVal__(rec, name, (UINT16)(val))
#define P2RecordSetFieldUINT32(rec, name, val) P2RecordSetFieldTypeVal__(rec, name, (UINT32)(val))
#define P2RecordSetFieldUINT64(rec, name, val) P2RecordSetFieldTypeVal__(rec, name, (UINT64)(val))
#define P2RecordSetFieldP2TIME(rec, name, pVal) P2RecordSetFieldTypePtr__(rec, name, (const P2TIME*)(pVal))*/

P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordSetFieldByIndex(P2DB_RECORD_HANDLE rs, size_t idx, const void *buf, size_t len);
/*#define P2RecordSetFieldByIndexTypeVal__(rec, idx, val) P2RecordSetFieldByIndex(rec, idx, &val, sizeof(val))
#define P2RecordSetFieldByIndexTypePtr__(rec, idx, pVal) P2RecordSetFieldByIndex(rec, idx, pVal, sizeof(*(pVal)))
#define P2RecordSetFieldByIndexINT8(rec, idx, val) P2RecordSetFieldByIndexTypeVal__(rec, idx, (INT8)(val))
#define P2RecordSetFieldByIndexINT16(rec, idx, val) P2RecordSetFieldByIndexTypeVal__(rec, idx, (INT16)(val))
#define P2RecordSetFieldByIndexINT32(rec, idx, val) P2RecordSetFieldByIndexTypeVal__(rec, idx, (INT32)(val))
#define P2RecordSetFieldByIndexINT64(rec, idx, val) P2RecordSetFieldByIndexTypeVal__(rec, idx, (INT64)(val))
#define P2RecordSetFieldByIndexUINT8(rec, idx, val) P2RecordSetFieldByIndexTypeVal__(rec, idx, (UINT8)(val))
#define P2RecordSetFieldByIndexUINT16(rec, idx, val) P2RecordSetFieldByIndexTypeVal__(rec, idx, (UINT16)(val))
#define P2RecordSetFieldByIndexUINT32(rec, idx, val) P2RecordSetFieldByIndexTypeVal__(rec, idx, (UINT32)(val))
#define P2RecordSetFieldByIndexUINT64(rec, idx, val) P2RecordSetFieldByIndexTypeVal__(rec, idx, (UINT64)(val))
#define P2RecordSetFieldByIndexP2TIME(rec, idx, pVal) P2RecordSetFieldByIndexTypePtr__(rec, idx, (const P2TIME*)(pVal))*/

P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetStructSize(P2DB_TABLE_SCHEME_HANDLE sch, size_t* pStructSize);

P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordCopyToStruct(P2DB_RECORD_HANDLE rec, void *out_buf, size_t *plen);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordCopyFromStruct(P2DB_RECORD_HANDLE rec, const void *out_buf, size_t len);

P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordCopyRecord(P2DB_RECORD_HANDLE dstRec, P2DB_RECORD_HANDLE srcRec);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordBlindCopyRecord(P2DB_RECORD_HANDLE dstRec, P2DB_RECORD_HANDLE srcRec);

//////////////////////////////////////////////////////////////////////

P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordCreateInMemory(P2DB_RECORD_HANDLE* pRec, P2DB_TABLE_SCHEME_HANDLE sch);
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordCreateInMemoryEx(P2DB_RECORD_HANDLE* pRec, P2DB_TABLE_SCHEME_HANDLE sch,
                                                          void** pRecFields, size_t* pRecBufSize, void* pExtRecBuf);

///////////////////////////////////////////////////////////////////////
//Функции, возвращающие признак isNull, сигнализирующий о том, что в поле содержится значение NULL
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldEx(P2DB_RECORD_HANDLE rec, const char *name, void *outBuf, size_t *pSize, BOOL* isNull);

P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldByIndexEx(P2DB_RECORD_HANDLE rec, size_t idx, void *out_buf, size_t *pSize, BOOL* isNull);

P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetFieldByIndexWithoutSchemeEx(P2DB_RECORD_HANDLE rs, size_t idx, void *out_buf, size_t *plen, BOOL* isNull, const char *p2type, UINT32 p2size, UINT32 p2prec);

P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordIsFieldNullByIndex(P2DB_RECORD_HANDLE rec, size_t idx, BOOL* pIsNull);

P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordSetFieldNull(P2DB_RECORD_HANDLE rs, const char *name);

P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordSetFieldNullByIndex(P2DB_RECORD_HANDLE rs, size_t idx);

P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordReloadBufferInMemory(P2DB_RECORD_HANDLE pRec, size_t* pRecBufSize, void* pExtRecBuf);

/**
* P2RecordGetBuffers - Получает указатели на внутренние буферы записи.
*
* Функция имеет смысл только для записей-в-памяти, для других типов записи вернет P2ERR_DB_NO_NATIVE_BUFFER.
*
* @param rec [req] Хендл записи.
*
* @param ppRecBuf [opt] Указатель на место в памяти, куда будет помещен указатель на внутренний буфер данных записи.
*
* @param ppNulls [opt] Указатель на место в памяти, куда будет помещен указатель на массив признаков Null/NotNull для всех полей записи.
*              0 - поле Not Null, 1 - поле Null. Длина массива соответствует количеству полей записи.
*
* @returns Код ошибки.
*/
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordGetBuffers(P2DB_RECORD_HANDLE rec, void** ppDataBuf, UINT8** ppNullsBuf);

///////////////////////////////////////////////////////////////////////
//Очистка полей записи
P2SYSRECORD_API P2ERR P2CALLSPEC P2RecordClear(P2DB_RECORD_HANDLE rec);

#ifdef  __cplusplus
}
#endif

#endif // _P2SYSRECORD_H_
