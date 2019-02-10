//----------------------------------------------------------------------------
// Copyright: (c) 2006 RTS, Russia
//----------------------------------------------------------------------------
// Created:	    13 Mar 2006   11:46
// File:        lock
// Author:		Mikhail_Navernyuk
//----------------------------------------------------------------------------
// Description: declaration of the multithread lock classes
//----------------------------------------------------------------------------

#pragma once
#include "global.h"

namespace fo2base
{

//---------------------------------------------------------------------------
//  class CSyncObj
/// ������� ����� �������� �������������� ������������� ������� � 
/// ������������ �����.
//---------------------------------------------------------------------------
class CSyncObj
{
public:
    CSyncObj() {};
    virtual ~CSyncObj() {};

    virtual void Lock() = 0;
    virtual void Unlock() = 0;
};

//---------------------------------------------------------------------------
//  class CGuard
/// ����������� ������. 
/// � ������������ ���������� ������� ����������, � � ����������� ����������
/// ���������. ��������:
/// \p
/// <code>
///     \n
///     void fn()\n
///     {\n
///         CGuard(m_lock);\n
///         ...\n
///         some guarded code\n
///         ...\n
///     }\n
/// </code>
//---------------------------------------------------------------------------
class CGuard
{
public:
    CGuard(CSyncObj& lock) : m_lock(lock) 
    {
        m_lock.Lock();
    }

    virtual ~CGuard() 
    {
        m_lock.Unlock();
    };

private:
    BAN_COPY_CONSTRUCTOR(CGuard);

private:
    CSyncObj& m_lock;

};  // CGuard

//---------------------------------------------------------------------------
//  class CNoLock
/// ������ ���������������� ������. ������������ ��� ������������ ����.
//---------------------------------------------------------------------------
class CNoLock : public CSyncObj
{
public:
    CNoLock() {};
    virtual ~CNoLock() {};

    virtual void Lock() {};
    virtual void Unlock() {};
};

} // namespace fo2base

//---------------------------------------------------------------------------
//  class CCriticalSectionLock
/// C��������������� ������ �� ���� ����������� ������. ������������ ��� 
/// ������������� ����. 
//---------------------------------------------------------------------------
class CCriticalSectionLock : public fo2base::CSyncObj
{
public:
    CCriticalSectionLock() { ::InitializeCriticalSection(&cs); }
    virtual ~CCriticalSectionLock() { ::DeleteCriticalSection(&cs); }

    virtual void Lock() { ::EnterCriticalSection(&cs); }
    virtual void Unlock() { ::LeaveCriticalSection(&cs); }

private:
    CRITICAL_SECTION cs;
};

//---------------------------------------------------------------------------
//  class CSRWLockObject
/// ������ ������������� ����������������� ��������� SRWLOCK. ������������   
/// ��� ������������� ����, ��� ��������� ������ ������ ������, � ��������� 
/// �����.
//---------------------------------------------------------------------------
class CSRWLockObject
{
public:
    CSRWLockObject() 
    { 
        ::InitializeSRWLock(&lock); 
    }
    ~CSRWLockObject() {}

    SRWLOCK& getLock() { return lock; }

    operator SRWLOCK&()
    {
        return lock;
    }

private:
    SRWLOCK lock;
};

//---------------------------------------------------------------------------
//  class CSRWReadLock
/// C��������������� ������ ��� ������. ������������ ��� ������������� ����, 
/// ��� ��������� ������ ������ ������, � ��������� ������ �����.
//---------------------------------------------------------------------------
class CSRWReadLock : public fo2base::CSyncObj
{
public:
    CSRWReadLock(SRWLOCK& _lock) : lock(_lock) {}
    virtual ~CSRWReadLock() {}

    virtual void Lock() { ::AcquireSRWLockShared(&lock); }
    virtual void Unlock() { ::ReleaseSRWLockShared(&lock); }

private:
    SRWLOCK& lock;
};

//---------------------------------------------------------------------------
//  class CSRWWriteLock
/// C��������������� ������ ��� ������. ������������ ��� ������������� ����, 
/// ��� ��������� ������ ������ ������, � ��������� ������ �����.
//---------------------------------------------------------------------------
class CSRWWriteLock : public fo2base::CSyncObj
{
public:
    CSRWWriteLock(SRWLOCK& _lock) : lock(_lock) {}
    virtual ~CSRWWriteLock() {}

    virtual void Lock() { ::AcquireSRWLockExclusive(&lock); }
    virtual void Unlock() { ::ReleaseSRWLockExclusive(&lock); }

private:
    SRWLOCK& lock;
};