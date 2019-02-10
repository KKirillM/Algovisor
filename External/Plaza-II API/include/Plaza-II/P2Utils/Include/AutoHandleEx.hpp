#ifndef _AUTOHANDLE_EX__H_
#define _AUTOHANDLE_EX__H_

#include "stuff.h"

#include <assert.h>
////////////////////////////////////////////////////////////////////////////////

// AutoHandle для нужд юниксовой ветки в которой handle может не только
// закрываться (close), но и удаляться из системы (unlink)

#define MAX_HANDLE_NAME 256

template<typename T>
class AutoCloseAndUnlink
{
	typedef typename T::handle_type HANDLE_TYPE; 
public:

	explicit AutoCloseAndUnlink( HANDLE_TYPE h, const char* name): m_h(h)
	{ strncpy(m_name, name, MAX_HANDLE_NAME);
		m_name[NAME_MAX-1] = 0; 
		_ASSERT(m_name[0] != 0);
	}

	~AutoCloseAndUnlink()
	{
		if (m_h != T::invalid_handle() ) { T::close_handle(m_h); T::unlink_handle(m_name); }
	}

	HANDLE_TYPE operator()(void) const
	{ return m_h; }

    HANDLE_TYPE Detach()
    {
        HANDLE_TYPE h = m_h;
        m_h = T::invalid_handle() ;
		m_name[0] = 0;
        return h;
    }

public:
	HANDLE_TYPE m_h;
	char 		m_name[MAX_HANDLE_NAME];

private:
    AutoCloseAndUnlink(const AutoCloseAndUnlink&){ assert(false); }
    AutoCloseAndUnlink& operator=(const AutoCloseAndUnlink&){ assert(false); }
    AutoCloseAndUnlink& operator=(HANDLE_TYPE){ assert(false); }
};


template<typename T>
class AutoClose
{
	typedef typename T::handle_type HANDLE_TYPE; 
public:

	AutoClose():m_h( T::invalid_handle() )
	{ }

	explicit AutoClose( HANDLE_TYPE h): m_h(h) 
	{ }

	~AutoClose()
	{
		if (m_h != T::invalid_handle() ) { T::close_handle(m_h); }
	}

	HANDLE_TYPE *operator&(void)
	{
		_ASSERT(m_h == T::invalid_handle());
		return &m_h;
	}

	HANDLE_TYPE operator()(void) const { return m_h; }

	operator HANDLE_TYPE(void) const
	{
		return m_h;
	}

    void Close()
    {
		if (m_h != T::invalid_handle())
		{
			T::close_handle(m_h);
            m_h = T::invalid_handle();
		}
    }

    void Swap(AutoClose& right)
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
        m_h = T::invalid_handle();
        return h;
    }

public:
	HANDLE_TYPE m_h;

private:
    AutoClose(const AutoClose&){ assert(false); }
    AutoClose& operator=(const AutoClose&){ assert(false); }
    AutoClose& operator=(HANDLE_TYPE){ assert(false); }
};

#endif //_AUTOHANDLE_EX__H_
