#ifndef _P2EREPLTYPES_H_
#define _P2EREPLTYPES_H_

#include <stddef.h> // for offsetof
#include "../../../Plaza-II/P2Common/Include/P2Types.h"

typedef INT64 P2EREPL_REVISION_TYPE;

#define P2EREPL_REVISION_FIELD_FORMAT   "i8"

#define P2EREPL_REVISION_PRINTF_FORMAT  "%I64d"

// Константа ревижена возвращаемая при запросе первого ревижина пустой таблицы
// используется для удаления всех записей с меньшим ревиженом на клиенте в случае
// пустой таблицы на сервере (ClearDeleted)
#define P2EREPL_MIN_REVISION        PLATFORM_S64(0)
#define P2EREPL_MAX_REVISION        PLATFORM_S64(0x7FFFFFFFFFFFFFFF)

#define P2EREPL_MAX_SERVER_NAME_SIZE  64


enum P2EREPL_COMPRESSION_TYPE
{
	P2EREPL_COMPRESSION_NONE   = 0,
	P2EREPL_COMPRESSION_LZO1X  = 1, // deprecated
	P2EREPL_COMPRESSION_SNAPPY = 2,
    P2EREPL_COMPRESSION_LZ4    = 3
};

#pragma pack(push, 4)
struct TEReplRec
{
    P2EREPL_REVISION_TYPE   rev;
    UINT32                  eventType;
	UINT32                  dataSize;  // длина данных эвента без учёта заголовка

    inline size_t getSize() const { return sizeof(TEReplRec) + dataSize; }
    static size_t getSize(size_t eventDataSize) { return sizeof(TEReplRec) + eventDataSize; }
}; 
#pragma pack(pop)

#endif // _P2EREPLTYPES_H_
