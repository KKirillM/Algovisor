//----------------------------------------------------------------------------
// Copyright: (c) 2006 RTS, Russia
//----------------------------------------------------------------------------
// Created:	    10 Feb 2006   16:19
// File:        handle
// Author:		Mikhail_Navernyuk
//----------------------------------------------------------------------------
// Description: declaration of the base handle class
//----------------------------------------------------------------------------

#pragma once

namespace fo2base
{

//----------------------------------------------------------------------------
/// Темплейтный класс для управления хендлами Plaza II. В отличии от
/// AutoP2Handle делает хендл - закрытым.
/// \example typedef CHandle<P2DB_TABLE_HANDLE, P2DBCloseTable> CP2TableHandle;
//----------------------------------------------------------------------------
template<class H, P2ERR (*Fn)(H)>
class CHandle
{
public:
    CHandle() : m_h(NULL) {};
    explicit CHandle(H h) : m_h(h) {};
    virtual ~CHandle();

    void Release();

    H    GetHandle() const;
    bool IsInitialized() const;

    void operator=(H h);

    virtual H    Detach();
    virtual void Attach(H h);
    virtual void Swap(CHandle& other);

protected:

private:
    CHandle(const CHandle&){ assert(false); }
    void operator=(const CHandle&){ assert(false); }

private:
    H  m_h;

};  // CHandle

//----------------------------------------------------------------------------
template<class H, P2ERR (*Fn)(H)>
CHandle<H, Fn>::~CHandle()
{
    Release();
}

//----------------------------------------------------------------------------
template<class H, P2ERR (*Fn)(H)>
void CHandle<H, Fn>::Release()
{
    if(IsInitialized())
    {
        Fn(m_h);
        m_h = NULL;
    }
}

//----------------------------------------------------------------------------
template<class H, P2ERR (*Fn)(H)>
H CHandle<H, Fn>::GetHandle() const
{
    return m_h;
}

//----------------------------------------------------------------------------
template<class H, P2ERR (*Fn)(H)>
bool CHandle<H, Fn>::IsInitialized() const
{
    return (m_h != NULL);
}

//----------------------------------------------------------------------------
template<class H, P2ERR (*Fn)(H)>
H CHandle<H, Fn>::Detach()
{
    H h = m_h;
    m_h = NULL;
    return h;
}

//----------------------------------------------------------------------------
template<class H, P2ERR (*Fn)(H)>
void CHandle<H, Fn>::Attach(H h)
{
    Release();
    m_h = h;
}

//----------------------------------------------------------------------------
template<class H, P2ERR (*Fn)(H)>
void CHandle<H, Fn>::operator=(H h)
{
    Attach(h);
}

//----------------------------------------------------------------------------
template<class H, P2ERR (*Fn)(H)>
void CHandle<H, Fn>::Swap(CHandle& other)
{
    H h = other.m_h;
    other.m_h = m_h;
    m_h = h;
}

} // namespace fo2base

