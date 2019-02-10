#ifndef _AUTOHANDLE_H_
#define _AUTOHANDLE_H_

#include "stuff.h"

#include <assert.h>
////////////////////////////////////////////////////////////////////////////////
/* ѕример использовани€:
 * typedef AutoP2Handle<P2DB_TABLE_SCHEME_HANDLE, P2DBDeleteTableScheme> AutoTableSchemeHandle;
 */
template<typename HANDLE_TYPE, P2ERR (*CLOSE_FUNCTION)(HANDLE_TYPE)>
class AutoP2Handle
{
public:
	AutoP2Handle() : m_h(NULL){}
	explicit AutoP2Handle(HANDLE_TYPE h) : m_h(h){} // чтобы не кастил

	~AutoP2Handle()
	{
		if (m_h != NULL)
		{
			_VERIFY_P2ERR(CLOSE_FUNCTION(m_h));
		}
	}

	HANDLE_TYPE *operator&(void)
	{
		assert(m_h == NULL);
		return &m_h;
	}

	HANDLE_TYPE operator()(void) const
	{
		return m_h;
	}

	operator HANDLE_TYPE(void) const
	{
		return m_h;
	}

    void Close()
    {
		if (m_h != NULL)
		{
			_VERIFY_P2ERR(CLOSE_FUNCTION(m_h));
            m_h = NULL;
		}
    }

    void Swap(AutoP2Handle& right)
    {
        HANDLE_TYPE h = right.m_h;
        right.m_h = m_h;
        m_h = h;
    }

    void Swap(HANDLE_TYPE *pH)
    {
        HANDLE_TYPE h = *pH;
        *pH = m_h;
        m_h = h;
    }

    HANDLE_TYPE Detach()
    {
        HANDLE_TYPE h = m_h;
        m_h = NULL;
        return h;
    }

public:
	HANDLE_TYPE m_h;

private:
    AutoP2Handle(const AutoP2Handle&);
    AutoP2Handle& operator=(const AutoP2Handle&);
    AutoP2Handle& operator=(HANDLE_TYPE);
};

#endif
