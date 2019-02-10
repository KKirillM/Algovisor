#ifndef _REPLLRPCQCOMPOSER_H__INCLUDED_
#define _REPLLRPCQCOMPOSER_H__INCLUDED_

#include "../../../Plaza-II/P2Utils/Include/stuff.h"
#include "../../../Plaza-II/P2SysParser/Include/P2SysBinParser.h"
#include "./ReplLrpcqLink.h"

#define POOL_INCREMENT 8192

#ifdef _MSC_VER // Microsoft specific
#define FORCE_INLINE   __forceinline
#define FORCE_NOINLINE __declspec(noinline)
#else
#define FORCE_INLINE   inline
#define FORCE_NOINLINE
#endif // _MSC_VER

class CRepl2LrpcqComposer : protected P2_ComposeState
{
public:
    CRepl2LrpcqComposer()
    {
        buf = NULL, size = 0, alloc_size = 0;
    }

    template< typename T >
    inline P2ERR ComposeSimple(UINT16 Id, T value)
    {
        size_t sz = sizeof(TReplLinkField().IdOrFlag) + sizeof(T);
        if (sz > alloc_size - size)
        {
            RETURN_P2ERR(GrowStateBuf(sz));
        }
        TReplLinkField * p = reinterpret_cast< TReplLinkField * >(reinterpret_cast< BYTE * >(buf) + size);
        p->IdOrFlag = Id;
        *reinterpret_cast< T* >(p->constSizeData) = value;
        size += sz;
        return P2ERR_OK;
    }

    inline P2ERR ComposeSimpleWithLen(UINT16 Id, const void* pValue, size_t valueSz)
    {
        size_t sz = sizeof(TReplLinkField().IdOrFlag) + valueSz;
        if (sz > alloc_size - size)
        {
            RETURN_P2ERR(GrowStateBuf(sz));
        }
        TReplLinkField * p = reinterpret_cast< TReplLinkField * >(reinterpret_cast< BYTE * >(buf) + size);
        p->IdOrFlag = Id;
        memcpy(p->constSizeData, pValue, valueSz);
        size += sz;
        return P2ERR_OK;
    }

    inline P2ERR ComposeVarLen(UINT16 Id, const void* pValue, size_t valueSz)
    {
        size_t sz = sizeof(TReplLinkField().IdOrFlag) + sizeof(TReplLinkField().variableSizeData->size) + valueSz;
        if (sz > alloc_size - size)
        {
            RETURN_P2ERR(GrowStateBuf(sz));
        }
        TReplLinkField * p = reinterpret_cast< TReplLinkField * >(reinterpret_cast< BYTE * >(buf) + size);
        p->IdOrFlag = Id;
        p->variableSizeData->size = static_cast< UINT32 >(valueSz);
        memcpy(p->variableSizeData->data, pValue, valueSz);
        size += sz;
        return P2ERR_OK;
    }

    inline P2ERR ComposeEndMarker()
    {
        size_t sz = sizeof(TReplLinkField().IdOrFlag);
        if (sz > alloc_size - size)
        {
            RETURN_P2ERR(GrowStateBuf(sz));
        }
        TReplLinkField * p = reinterpret_cast< TReplLinkField * >(reinterpret_cast< BYTE * >(buf) + size);
        p->IdOrFlag = static_cast< UINT16 >(P2ReplLinkField_EndMarker);
        size += sz;
        return P2ERR_OK;
    }

    inline P2ERR ComposeStr(UINT16 Id, const char * value)
    {
        return ComposeStr(Id, value, strlen(value) + 1);
    }

    inline P2ERR ComposeStr(UINT16 Id, const char * value, size_t maxSz)
    {
        size_t sz = sizeof(TReplLinkField().IdOrFlag) + sizeof(TReplLinkField().variableSizeData->size) + maxSz;
        if (sizeof(UINT32) + sz > alloc_size - size)
        {
            RETURN_P2ERR(GrowStateBuf(sz));
        }
        TReplLinkField * pRlf = reinterpret_cast< TReplLinkField * >(reinterpret_cast< BYTE * >(buf) + size);
        pRlf->IdOrFlag = Id;
        char * p = reinterpret_cast< char * >(pRlf->variableSizeData->data);
        char * p0 = p;
        while ( (*p++ = *value++) != 0 ) ;
        pRlf->variableSizeData->size = static_cast< UINT32 >(p - p0);
        size += sizeof(TReplLinkField().IdOrFlag) + sizeof(TReplLinkField().variableSizeData->size) + pRlf->variableSizeData->size;
        return P2ERR_OK;
    }

    inline P2ERR ComposeWithName(const char * pName, const void* pValue, size_t valueSz)
    {
        size_t nameSz = strlen(pName) + 1;
        size_t sz = sizeof(TReplLinkField().IdOrFlag) + sizeof(P2ZBLOCK().size) + nameSz +
            sizeof(TReplLinkField().variableSizeData->size) + valueSz;
        if (sz > alloc_size - size)
        {
            RETURN_P2ERR(GrowStateBuf(sz));
        }
        TReplLinkField * p = reinterpret_cast< TReplLinkField * >(reinterpret_cast< BYTE * >(buf) + size);
        p->IdOrFlag = REPL_LINK_NAMED_FIELD;
        // имя поля
        p->variableSizeData->size = static_cast< UINT32 >(nameSz);
        memcpy(p->variableSizeData->data, pName, nameSz);
        // значение поля
        P2ZBLOCK * pZData = reinterpret_cast< P2ZBLOCK * >( reinterpret_cast< BYTE * >(&(p->variableSizeData->data)) + nameSz );
        pZData->size = static_cast< UINT32 >(valueSz);
        memcpy(pZData->data, pValue, valueSz);
        size += sz;
        return P2ERR_OK;
    }

    inline const void * GetBuffer() const { return buf; }
    inline size_t GetSize() const { return size; }
    inline void Reset() { size = 0; }

    ~CRepl2LrpcqComposer()
    {
        if (NULL != buf)
        {
            free(buf);
            buf = NULL;
        }
    }

protected:
    FORCE_NOINLINE P2ERR GrowStateBuf(size_t delta)
    {
        size_t new_alloc_size = alloc_size + 
            ((delta > alloc_size * 2) ? delta + POOL_INCREMENT : alloc_size * 2);

        BYTE *b = static_cast<BYTE *>(realloc(buf, new_alloc_size));
        if (NULL == b)
        {
            return P2ERR_COMMON_NO_MEMORY;
        }
        buf = b;
        alloc_size = new_alloc_size;
        return P2ERR_OK;
    }
};

#undef FORCE_INLINE
#undef FORCE_NOINLINE

#endif // _REPLLRPCQCOMPOSER_H__INCLUDED_
