//----------------------------------------------------------------------------
// Copyright: (c) 2006 RTS, Russia
//----------------------------------------------------------------------------
// Created:	    10 Feb 2006   16:19
// File:        singleton
// Author:		Mikhail_Navernyuk
//----------------------------------------------------------------------------
// Description: declaration of the base exception class
//----------------------------------------------------------------------------

#pragma once

#include "lock.h"

//----------------------------------------------------------------------------
/// ��������� ��������� � ������������ ��������.
/// \param T - ����������� ����� 
//----------------------------------------------------------------------------
template<class T>
class CNewStorage
{
public:
    explicit CNewStorage(T* s = NULL) : m_inst(s) {};
    ~CNewStorage() 
    { 
        delete m_inst;
        m_inst = NULL;
    };

    void Create()
    { 
        assert(m_inst == NULL);
        m_inst = new T();
    };

    void Delete()
    {
        delete m_inst;
        m_inst = NULL;
    }

    T* GetInst() const
    { 
        return m_inst; 
    }

private:
    T* m_inst;

};  // CNewStorage

//----------------------------------------------------------------------------
/// ��������� ���������. 
/// 1. ����������� ������������� ������ ������ ���������� ������ Type � 
///    ����������. \n
/// 2. ������������� ������� ������ � ����� ����������. \n
/// 3. � ������� �� ���������� ���������� ���� ������ ��������� ���������
///    ����������� ������.
/// \note ����� Type ������ ��������� ��������������� CSingletonHolder ��� 
///       friend.
/// \param Type    - ��� ��������
/// \param Lock    - �������� ���������� � ������������ �����
/// \see CNoLock

//----------------------------------------------------------------------------
template<class Type, 
         class Storage = CNewStorage<Type>,
         class Lock = fo2base::CNoLock>
class CSingletonHolder
{
public:
    typedef CSingletonHolder<Type,Lock> holder_type;
    typedef Type                        singleton_type;
    typedef Storage                     storage_type;
    typedef Lock                        lock_type;

    static singleton_type* GetInstance();
    static void DelInstance();

protected:
	CSingletonHolder() /*: s_inst(NULL)*/ {}
    CSingletonHolder(const CSingletonHolder&);
	virtual ~CSingletonHolder() {}

private:
    static lock_type	    s_lock;     ///< ���������������� ������
    static storage_type     s_storage;

};  // CSingletonHolder

//template<class Type, class Storage, class Lock>
//typename CSingletonHolder<Type, Storage, Lock>::singleton_type* 
//    CSingletonHolder<Type, Storage, Lock>::s_inst = NULL;

template<class Type, class Storage, class Lock>
typename CSingletonHolder<Type, Storage, Lock>::lock_type       
    CSingletonHolder<Type, Storage, Lock>::s_lock;

template<class Type, class Storage, class Lock>
Storage CSingletonHolder<Type, Storage, Lock>::s_storage;

//----------------------------------------------------------------------------
/// ���������� ������ �� ������������ ����������� ��������� ������. 
/// ������� Singleton �������
//----------------------------------------------------------------------------
template<class Type, class Storage, class Lock>
Type* CSingletonHolder<Type, Storage, Lock>::GetInstance()
{
	//double checked locking pattern
    /*if( s_inst == NULL )
    {
        CGuard  guard(s_lock);
        if( s_inst == NULL )
        {
            s_inst = new Type();
        }
    }*/

    if( s_storage.GetInst() == NULL )
    {
        fo2base::CGuard  guard(s_lock);
        if( s_storage.GetInst() == NULL )
        {
            s_storage.Create();
        }
    }

	//static Type s_inst;

    return s_storage.GetInst(); //&s_inst;
}

template<class Type, class Storage, class Lock>
void CSingletonHolder<Type, Storage, Lock>::DelInstance()
{
    s_storage.Delete();
}

