#ifndef _SMARTIRC_H_
#define _SMARTIRC_H_

#include "../../P2Common/Include/P2Types.h"

#include "IRc.h"
#include <assert.h>
#include "stuff.h"

////////////////////////////////////////////////////////////////////////////////
// Смартпоинтеры. Выдраны из ATL
// CRcPtr - смартпоинтер для IRc
// CAdapt - адаптер для контейнеров CRcPtr. CRcPtr не могут храниться в контейнерах из-за перехвата operator& 
// AutoP2Handle - смартпоинтер для хэндлов с функцией освобождения типа P2ERR (*CLOSE_FUNCTION)(HANDLE)

// присваивание указателя на новое место с обсобождением предыдущего значения
inline IRc* RcPtrAssign(IRc** pp, IRc* lp)
{
	if (lp != NULL)
	{
		LONG x = lp->AddRef1();
#ifdef _DEBUG
		//printf("%lx AddRef %d\n", lp, x);
#endif
		UNUSED(x);
	}
	if (*pp)
	{
		LONG x = (*pp)->Release1();
#ifdef _DEBUG
		//printf("%lx Release %d\n", *pp, x);
#endif
        UNUSED(x);
	}
	*pp = lp;
	return lp;
}

#ifdef _MSC_VER
// класс с защищенными функциями для диагностики некорректного использования
#pragma warning( push )
#pragma warning( disable : 4624 )	// компилятор не может сгенерировать деструктор. и не нужно
#pragma warning( disable : 4510 )	// компилятор не может сгенерировать конструктор по умолчанию. и не нужно
#pragma warning( disable : 4610 )	// object 'class' can never be instantiated - user-defined constructor required
#pragma warning( disable : 4625 )	// copy constructor could not be generated because a base class copy constructor is inaccessible
#pragma warning( disable : 4626 )	// assignment operator could not be generated because a base class assignment operator is inaccessible
#endif // _MSC_VER

template <class T>
class _NoAddRefReleaseOnCRcPtr : public T
{
	private:
	    LONG AddRef1();
		LONG Release1();
};

#ifdef _MSC_VER
#pragma warning( pop )
#endif // _MSC_VER

template <class T>
class CRcPtr
{
public:
	typedef T _PtrClass;
	CRcPtr()
	{
		p=NULL;
	}

	CRcPtr(T* lp, bool makeAddRef = true)
	{
		if (((p = lp) != NULL) && makeAddRef)
		{
			 p->AddRef1();
		}
	}

	CRcPtr(const CRcPtr<T>& lp)
	{
		if ((p = lp.p) != NULL)
		{
			p->AddRef1();
		}
	}

	~CRcPtr()
	{
		if (p)
		{
			p->Release1();
		}
	}

	void Release1()
	{
		IRc* pTemp = p;
		if (pTemp)
		{
			p = NULL;
			pTemp->Release1();
		}
	}

	operator T*() const
	{
		return p;
	}


	T* Ptr(void) const
	{
		return p;
	}

	T& operator*() const
	{
		assert(p!=NULL);
		return *p;
	}

	T** operator&()
	{
		//The assert on operator& usually indicates a bug.  If this is really
		//what is needed, however, take the address of the p member explicitly.
		assert(p==NULL);
		return &p;
	}

	_NoAddRefReleaseOnCRcPtr<T>* operator->() const
	{
		assert(p!=NULL);
		return static_cast<_NoAddRefReleaseOnCRcPtr<T>*>(p);
	}

	T* operator=(T* lp)
	{

		if (lp) lp->AddRef1();
		if (p)  p->Release1();
		p = lp;
		return p; 
	}

	T* operator=(const CRcPtr<T>& lp)
	{
		if (lp.p) lp.p->AddRef1(); 
		if (p)    p->Release1();
		p = lp.p;
		return p; 
	}

	bool operator!() const
	{
		return (p == NULL);
	}

	bool operator<(T* pT) const
	{
		return p < pT;
	}

	bool operator==(T* pT) const
	{
		return p == pT;
	}

	void Attach(T* p2)
	{
		if (p)
		{
			p->Release1();
		}
		p = p2;
	}

	T* Detach()
	{
		T* pt = p;
		p = NULL;
		return pt;
	}

	P2ERR CopyTo(T** ppT)const
	{
		assert(ppT != NULL);

		if (ppT == NULL)
		{
			return P2ERR_COMMON_WRONG_ARGUMENTS;
		}

		*ppT = p;

		if (p)
		{
			p->AddRef1();
		}

		return P2ERR_OK;
	}

	T* p;
};

/////////////////////////////////////////////////////////////
// Class to Adapt CComBSTR and CComPtr for use with STL containers
// the syntax to use it is
// std::vector< CAdapt <CComBSTR> > vect;

template <class T>
class CAdapt
{
public:
	CAdapt()
	{
	}
	CAdapt(const T& rSrc)
	{
		m_T = rSrc;
	}

	CAdapt(const CAdapt& rSrCA)
	{
		m_T = rSrCA.m_T;
	}

	CAdapt& operator=(const T& rSrc)
	{
		m_T = rSrc;
		return *this;
	}
	bool operator<(const T& rSrc) const
	{
		return m_T < rSrc;
	}
	bool operator==(const T& rSrc) const
	{
		return m_T == rSrc;
	}
	operator T&()
	{
		return m_T;
	}

	operator const T&() const
	{
		return m_T;
	}

	T m_T;
};

#include "AutoHandle.hpp"

#endif
