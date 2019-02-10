#ifndef _REPLLRPCQDATAREADER_H__INCLUDED_
#define _REPLLRPCQDATAREADER_H__INCLUDED_

#include "./ReplLrpcqLink.h"

extern const TReplKnownField knownFieldsInfo[];

class CReplLrpcqDataReader
{
	bool canRead(const void* pFrom, size_t size) const
	{ return static_cast< const UINT8* >(pFrom) + size <= pDataEnd; } 

	const TReplLinkField* p;
	const UINT8*          pDataEnd;
public:
    CReplLrpcqDataReader(const void * pData, const void* pDataEnd_): p(static_cast< const TReplLinkField * >(pData)), pDataEnd(static_cast< const UINT8* >(pDataEnd_)) {}

	bool checkField() const 
	{ 
		if (!canRead(p, sizeof(p->IdOrFlag)))
		{
			return false; 
		} 
		else if (REPL_LINK_NAMED_FIELD == p->IdOrFlag)
		{
			const P2ZBLOCK* name = reinterpret_cast< const P2ZBLOCK* >(p->variableSizeData);
			if (!canRead(name, sizeof(name->size)) || !name->size || !canRead(name->data, name->size) || name->data[name->size - 1] != '\0') return false;  	

			const P2ZBLOCK* data = reinterpret_cast< const P2ZBLOCK* >(name->data + name->size);
			if (!canRead(data, sizeof(data->size)) || !canRead(data->data, data->size)) return false;
		} 
		else if (p->IdOrFlag >= (sizeof(knownFieldsInfo) / sizeof(knownFieldsInfo[0]))) 
		{
			return false; 
		}
        else if (FIELD_LEN_IS_VARIABLE == knownFieldsInfo[p->IdOrFlag].fieldDataSize)
		{
			const P2ZBLOCK* data = reinterpret_cast< const P2ZBLOCK* >(p->variableSizeData);
			if (!canRead(data, sizeof(data->size)) || !canRead(data->data, data->size)) return false;
		}
		else if (!canRead(p->constSizeData, knownFieldsInfo[p->IdOrFlag].fieldDataSize))
		{
			return false;
		} 
		return true;
	} 

    inline UINT16 getIdOrFlag() const { return p->IdOrFlag; }
    inline bool isEnd() const 		  { return P2ReplLinkField_EndMarker == p->IdOrFlag; }
	inline bool isAddrTo() const	  { return P2ReplLinkField_AddrTo ==    p->IdOrFlag; }
	inline bool isType()  const       { return P2ReplLinkField_Type == p->IdOrFlag; }
    inline const char * getFieldName() const
    { 
        return REPL_LINK_NAMED_FIELD == p->IdOrFlag ? reinterpret_cast< const char * >(p->variableSizeData->data) : knownFieldsInfo[p->IdOrFlag].fieldName;
    }

    inline UINT32 getFieldNameSize() const
    {
        return REPL_LINK_NAMED_FIELD == p->IdOrFlag ? p->variableSizeData->size :  static_cast< UINT32 >(knownFieldsInfo[p->IdOrFlag].fieldNameSize);
    }

    inline const UINT8 * getFieldData() const
    {
        if (REPL_LINK_NAMED_FIELD == p->IdOrFlag)
            return (reinterpret_cast< const P2ZBLOCK* >(p->variableSizeData->data + p->variableSizeData->size))->data;
        else if (FIELD_LEN_IS_VARIABLE == knownFieldsInfo[p->IdOrFlag].fieldDataSize)
            return p->variableSizeData->data;
        else
            return p->constSizeData;
    }

    inline UINT32 getFieldDataSize() const
    {
        if (REPL_LINK_NAMED_FIELD == p->IdOrFlag)
            return (reinterpret_cast< const P2ZBLOCK* >(p->variableSizeData->data + p->variableSizeData->size))->size;
        else if (FIELD_LEN_IS_VARIABLE == knownFieldsInfo[p->IdOrFlag].fieldDataSize)
            return p->variableSizeData->size;
        else
            return knownFieldsInfo[p->IdOrFlag].fieldDataSize;
    }

    inline void MoveNext()
    {
        if (isEnd())
            return;
        if (REPL_LINK_NAMED_FIELD == p->IdOrFlag)
        {
            const P2ZBLOCK* data = reinterpret_cast< const P2ZBLOCK* >(p->variableSizeData->data + p->variableSizeData->size);
            p = reinterpret_cast< const TReplLinkField* >( data->data + data->size );
        }
        else if (FIELD_LEN_IS_VARIABLE == knownFieldsInfo[p->IdOrFlag].fieldDataSize)
            p = reinterpret_cast< const TReplLinkField* >(p->variableSizeData->data + p->variableSizeData->size);
        else
            p = reinterpret_cast< const TReplLinkField* >(p->constSizeData + knownFieldsInfo[p->IdOrFlag].fieldDataSize);
    }

    inline const void * GetCurrentPtr() const { return p; }
   
};

#endif // _REPLLRPCQDATAREADER_H__INCLUDED_
