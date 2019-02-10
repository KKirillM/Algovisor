#ifndef _QUEUEMAN_H_
#define _QUEUEMAN_H_

#include "CS.h"
#include <assert.h>
#include "../../P2SysLog/Include/P2SysLog.h"

/*
Реализация FIFO-очереди с указателями, добавляемыми непосредственно в элементы. То есть
очередь удобна для объектов, указатель на которые нужно хранить в единственной в каждый 
момент времени очереди.

Для использования нужно 
0. Включить хидер
1. Унаследовать объекты от класса CQueueElem
2. Список реализуется объектом CQueueMan<T>, где T - класс объектов. Темплейт нужен из-за того, что очередь у
    указывает на CQueueElem и требуется static_cast.

Методы у класса в целом простые: push, pop и empty. У остальных методов есть нюансы.
1. Метод swap. Потенциально может вызвать дедлок, поскольку хватает две CS. Во избежание дедлока порядок захвата секций указывается явно параметром без дефолта
2. CS класса доступна извне методом GetCS. 

Имеются также классы наследники CQueueManWithDel и CQueueManRC. Оба класса добавлюят к логике очереди логику работы с объектами. Добавился метод clear, который удаляет объекты, 
соответственно через delete p и через p->Release1(). Также происходит очистка при вызове деструктора. Также класс CQueueManRC накидывает ссылку при вызове push.

*/
template<typename T> class CQueueMan;

#define LAST_ELEM_MARKER  (reinterpret_cast<CQueueElem*>(0x1))

struct CQueueElem
{
	template<typename T> friend class CQueueMan;

	CQueueElem() : m_pNext(NULL){} 

private:
	CQueueElem *m_pNext;

protected:

	static inline bool IsLast(const CQueueElem* pElem) 
	{
		return pElem->m_pNext == LAST_ELEM_MARKER;
	}

	static inline bool inUse(const CQueueElem* pElem) 
	{ 
		return pElem->m_pNext != NULL;
	}

};



template<typename T>
class CQueueMan
{
public:
	CQueueMan(bool needCs = true) : m_pHead(NULL), m_pTail(NULL), m_size(0) { UseCS(needCs);}

	void UseCS(bool needCs) {m_cs.UseCS(needCs);}


	bool empty(bool lock = true)
	{
		if (lock) m_cs.Lock();
		bool b = m_pHead == NULL;
		if (lock) m_cs.Unlock();
		return b;
	}

	CCS& getCS(void)
	{
		return m_cs;
	}

	T* head() {return m_pHead;}

	static inline T* GetNext(T* pElement) {return static_cast<T*>(pElement->m_pNext);}

	T *pop(bool lock = true)
	{
		if (lock) m_cs.Lock();
		if (m_pHead == NULL)
		{
			assert((m_pHead == NULL) &&(m_pTail == NULL));
			if (lock) m_cs.Unlock();
			return NULL;
		}
		assert((m_pHead != NULL) && (m_pTail != NULL));
		T *p = m_pHead;
		T *next = static_cast<T *>(p->m_pNext);
		if(CQueueElem::IsLast(m_pHead))
		{
			m_pHead = m_pTail = NULL;
		}
		else
		{
			m_pHead = next;
		}
		p->m_pNext = NULL;
		--m_size;
		if (lock) m_cs.Unlock();
		return p;
	}

	size_t push(T *p, bool lock = true)
	{
		if (lock) m_cs.Lock();

        if (p == NULL)
        {
            size_t retSize = m_size;
            if (lock) m_cs.Unlock();
            return retSize;
        }

		assert((p->m_pNext == NULL));
		if(p->m_pNext != NULL)  //test version only
		{
#ifdef _MSC_VER			
			P2TRACE_ERROR_WITH_CODE_EX((P2ERR_COMMON_INTERNAL_ERROR,"CQueueMan","Push called for element already existed;0x%p",p));
			DebugBreak(); 
#endif			
		}

		if (m_pHead == NULL)
		{
			assert((m_pHead == NULL) &&(m_pTail == NULL));
			m_pHead = m_pTail = p;
		}
		else
		{
			assert((m_pHead != NULL) && (m_pTail != NULL));
			m_pTail->m_pNext = p;
			m_pTail = p;
		}

		p->m_pNext = LAST_ELEM_MARKER;
        size_t retSize = ++m_size;
		if (lock) m_cs.Unlock();
        return retSize;
	}

	bool pushIfNotExisted(T* p, bool lock = true)
	{ 
		if(p == NULL)
		{
			return false;
		}

		if(lock) m_cs.Lock(); 
	
		if( CQueueElem::inUse(p) )  //already existed in some queue; skip
		{
			if(lock) m_cs.Unlock();
			return false;
		}

		if (m_pHead == NULL)
		{
			assert((m_pHead == NULL) && (m_pTail == NULL));
			m_pHead = m_pTail = p;
		}
		else
		{ 
			assert((m_pHead != NULL) && (m_pTail != NULL));
			m_pTail->m_pNext = p;
			m_pTail = p; 
		}

		p->m_pNext = LAST_ELEM_MARKER;
		++m_size;
		if (lock) m_cs.Unlock();
		return true; 
	}

	void swap(CQueueMan& lst, bool ourCsFirst)
	{
		AutoCCS cs0(ourCsFirst ? m_cs : lst.m_cs);
		AutoCCS cs1(ourCsFirst ? lst.m_cs : m_cs);
		T *h;
		T *t;
		size_t sz;
		h = lst.m_pHead;
		t = lst.m_pTail;
		sz = lst.m_size;
		lst.m_pHead = m_pHead;
		lst.m_pTail = m_pTail;
		lst.m_size = m_size;
		m_pHead = h;
		m_pTail = t;
		m_size = sz;
	}

	void append(CQueueMan& lst, bool ourCsFirst)
	{
		AutoCCS cs0(ourCsFirst ? m_cs : lst.m_cs);
		AutoCCS cs1(ourCsFirst ? lst.m_cs : m_cs);
		if (lst.m_pHead == NULL)
		{	// новый пустой
			return;
		}
		if (m_pHead == NULL)
		{	// наш пустой, новый непустой
			m_pHead = lst.m_pHead;
		}
		else
		{	// наш непустой и новый непустой
			m_pTail->m_pNext = lst.m_pHead;
		}
		m_pTail = lst.m_pTail;
		lst.m_pHead = lst.m_pTail = NULL;
		m_size += lst.m_size;
	}

	size_t size(bool lock = true)
	{
		if(lock) m_cs.Lock(); 
		size_t sz = m_size;
		if(lock) m_cs.Unlock(); 
		return sz;
	}

protected:
	CCS m_cs;
	T *m_pHead;
	T *m_pTail;
	size_t m_size;
};

template<typename T>
class CQueueManWithDel : public CQueueMan<T>
{
public:
	CQueueManWithDel(bool needCs = true) : CQueueMan<T>(needCs) {}
	~CQueueManWithDel() { clear(); }
	void clear(void)
	{
		AutoCCS cs(CQueueMan<T>::m_cs);
		while (!CQueueMan<T>::empty())
		{
			delete CQueueMan<T>::pop();
		}
	}
};

template<typename T>
class CQueueManRC : public CQueueMan<T>
{
public:
	CQueueManRC(bool needCs = true) : CQueueMan<T>(needCs) {}
	~CQueueManRC() { clear(); }

	void push(T *p, bool lock = true)
	{
		p->AddRef1();
		CQueueMan<T>::push(p, lock);
	}

	bool pushIfNotExisted(T* p,bool lock = true)
	{
		p->AddRef1();
		bool res = CQueueMan<T>::pushIfNotExisted(p, lock);
		if (!res)
		{
			p->Release1();
		}
		return res;
	}

	void clear(bool lock = true)
	{
		if (lock) CQueueMan<T>::m_cs.Lock();
		while (!CQueueMan<T>::empty())
		{
			T* p = CQueueMan<T>::pop();
			p->Release1();
		}
		if (lock) CQueueMan<T>::m_cs.Unlock();
	}
};



#endif // _QUEUEMAN_H_
