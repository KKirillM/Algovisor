#ifndef _AUTOMEMHANDLE_H_
#define _AUTOMEMHANDLE_H_

#include "stuff.h"

#include <assert.h>
////////////////////////////////////////////////////////////////////////////////
template<size_t MAX_BUF_SIZE, typename CAST_TYPE>
class AutoSmartMem
{
public:
	AutoSmartMem() : m_ptr(NULL){}
	AutoSmartMem(size_t sz) : m_ptr(NULL)
	{
		Set(sz);
	}

	void Set(size_t sz)
	{
		Close();
		if (sz <= MAX_BUF_SIZE)
		{
			m_ptr = reinterpret_cast<CAST_TYPE *>(m_buf);
		}
		else
		{
			m_ptr = static_cast<CAST_TYPE *>(malloc(sz));
		}
	}

	~AutoSmartMem()
	{
		Close();
	}

	CAST_TYPE *operator()(void) const
	{
		return m_ptr;
	}

	operator CAST_TYPE *(void) const
	{
		return reinterpret_cast<CAST_TYPE *>(m_ptr);
	}

	CAST_TYPE * operator ->(void) const
	{
		return reinterpret_cast<CAST_TYPE *>(m_ptr);
	}

    void Close()
    {
		if ((m_ptr != NULL) && (static_cast<void *>(m_ptr) != m_buf))
		{
			free(m_ptr);
		}
		m_ptr = NULL;
    }

public:
	CAST_TYPE *m_ptr;
	UINT8 m_buf[MAX_BUF_SIZE];

private:
    AutoSmartMem(const AutoSmartMem&){ assert(false); }
    AutoSmartMem& operator=(const AutoSmartMem&){ assert(false); }
};


#endif
