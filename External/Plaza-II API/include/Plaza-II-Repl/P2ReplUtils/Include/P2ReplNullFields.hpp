#ifndef _P2REPLNULLFIELDS_H_
#define _P2REPLNULLFIELDS_H_

#include "../../../Plaza-II/P2SysParser/Include/P2SysBinParser.h"
#include "../../../Plaza-II/P2Utils/Include/stuff.h"
#include "../../../Plaza-II/P2SysLog/Include/P2SysLog.h"
#include "../../../Plaza-II/P2SysParser/Include/FastBinComposer.h"

//////////////////////////////////////////////////////////////////////////
//////////// CNullFieldsComposer
//////////////////////////////////////////////////////////////////////////

namespace nm_P2ReplUtils {


class CNullFieldsComposer
{
public:
	CNullFieldsComposer(): m_index(0), m_buf(0) { }

    P2ERR Compose(BOOL isNull, BOOL lastField);

	inline const void * GetBuffer() const { return m_composer.GetBuffer(); }

    inline size_t GetSize() const { return m_composer.GetSize(); }

    inline void Reset()
    {
        m_buf = 0;
        m_index = 0;
        m_composer.Reset();
    }

    P2ERR GrowStateBuf(size_t delta) { return m_composer.GrowStateBuf(delta); }

    inline void * Reserve(size_t reserveSz) { return m_composer.Reserve(reserveSz); }

private:
	CFastBinComposer m_composer;

    UINT32	m_index;
	UINT8	m_buf;
};

//////////////////////////////////////////////////////////////////////////
inline P2ERR CNullFieldsComposer::Compose(BOOL isNull, BOOL lastField)
{
    if( isNull ) 
    {
        m_buf = m_buf | ( 1 << m_index );
    }
    if( ++m_index == 8 || lastField ) 
    {
        RETURN_P2ERR(m_composer.Compose(static_cast< UINT8 >(m_buf)));
        //P2TRACE_INFO_EX_0(("NULL Fields", "Compose NULL fields %d", (UINT32)m_buf));
        m_index	= 0;
        m_buf	= 0;
    }
    return P2ERR_OK;
}

//////////////////////////////////////////////////////////////////////////
//////////// CNullFieldsParser
//////////////////////////////////////////////////////////////////////////
class CNullFieldsParser
{
public:
	CNullFieldsParser(): m_parser(NULL, 0), m_inited(FALSE), m_buf(0), m_index(0) { }
	P2ERR Init(const void *pData, size_t size)
	{
		RETURN_P2ERR_COND_ASS(!IsInited(), P2ERR_COMMON_ALREADY_INITIALIZED);
		m_buf	= 0;
		m_index = 0;
		if( pData )
		{
			m_parser.Init(pData, size);
			m_inited = TRUE;
		}

		return P2ERR_OK;
	}

	inline P2ERR Parse(BOOL& isNull, BOOL firstField);

	inline BOOL IsInited() const
	{
		return m_inited;
	}

	inline P2ERR SetIndex(UINT32 idx)
	{
		return m_parser.Set(idx);
	}

    inline P2ERR Skip(UINT32 sz)
    {
        return m_parser.Set(m_parser.curIndex + sz);
    }

    inline P2ERR CheckBuf(size_t sz) const
    {
        return m_parser.CheckBuf(sz);
    }

    inline const void * GetParsePtr() const { return m_parser.GetParsePtr(); }

private:
	CFastBinParser	m_parser;
	BOOL			m_inited;

	UINT8			m_buf;
	UINT32			m_index;
};

//////////////////////////////////////////////////////////////////////////
inline P2ERR CNullFieldsParser::Parse(BOOL& isNull, BOOL firstField)
{
	RETURN_P2ERR_COND_ASS(IsInited() == TRUE, P2ERR_COMMON_OBJECT_NOT_INITED);
	if( m_index == 8 || firstField ) 
	{
		RETURN_P2ERR(m_parser.Parse(&m_buf));
		P2TRACE_INFO_EX_0(("NULL Fields", "Parse NULL fields %d", (UINT32)m_buf));
		m_index = 0;
	}
	isNull = ( m_buf & 1 ) == 1;
	m_buf = m_buf >> 1;
	++m_index;
	return P2ERR_OK;
}
}; //namespace nm_P2ReplUtils

// пространство имен было сделано во избежание коллизий символов при линковке в юниксовой ветке (где so по экспортирует все символы)
using namespace nm_P2ReplUtils;

#endif	//_P2REPLNULLFIELDS_H_
