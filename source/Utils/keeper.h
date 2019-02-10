//----------------------------------------------------------------------------
// Copyright: (c) 2006 RTS, Russia
//----------------------------------------------------------------------------
// Created:	    08 Jun 2006   16:19
// File:        keeper
// Author:		Mikhail_Navernyuk
//----------------------------------------------------------------------------
// Description: declaration of the handler keeper class
//----------------------------------------------------------------------------

#pragma once

namespace fo2base
{

//----------------------------------------------------------------------------
/// Темплейтный класс для управления хендлами Plaza II. В отличии от
/// CHandle не удаляет объект автоматически. Тоесть CKeeper предназначен, для 
/// ссылок на объекты временем жизни которых управляют CHandle. Тем не менее
/// в CKeeper определена функция Release, которая уничтожает управляемый объект.
/// Вызывать эту функцию можно только в том случае, когда CKeeper является 
/// ЕДИНСТВЕННЫМ хранителем, ссылающимся на хендл.
/// \example typedef CKeeper<P2DB_TABLE_HANDLE, P2DBCloseTable> CP2TableKeeper;
//----------------------------------------------------------------------------
template<class H, P2ERR (*Fn)(H)>
class CKeeper
{
public:
    CKeeper() : m_h(NULL) {};
    explicit CKeeper(H h) : m_h(h) {};
    virtual ~CKeeper();

    void Release();

    H    GetHandle() const;
    bool IsInitialized() const;

    void operator=(H h);

    virtual H    Detach();
    virtual void Attach(H h);
    virtual void Swap(CKeeper& other);

protected:
    //void SetHandle(H h);

private:
    CKeeper(const CKeeper&){ assert(false); }
    CKeeper& operator=(const CKeeper&){ assert(false); }

private:
    H  m_h;

};  // CKeeper

//----------------------------------------------------------------------------
template<class H, P2ERR (*Fn)(H)>
CKeeper<H, Fn>::~CKeeper()
{
}

//----------------------------------------------------------------------------
template<class H, P2ERR (*Fn)(H)>
void CKeeper<H, Fn>::Release()
{
    if(IsInitialized())
    {
        Fn(m_h);
        m_h = NULL;
    }
}

//----------------------------------------------------------------------------
template<class H, P2ERR (*Fn)(H)>
H CKeeper<H, Fn>::GetHandle() const
{
    return m_h;
}

//----------------------------------------------------------------------------
template<class H, P2ERR (*Fn)(H)>
bool CKeeper<H, Fn>::IsInitialized() const
{
    return (m_h != NULL);
}

//----------------------------------------------------------------------------
template<class H, P2ERR (*Fn)(H)>
H CKeeper<H, Fn>::Detach()
{
    H h = m_h;
    m_h = NULL;
    return h;
}

//----------------------------------------------------------------------------
template<class H, P2ERR (*Fn)(H)>
void CKeeper<H, Fn>::Attach(H h)
{
    m_h = h;
}

//----------------------------------------------------------------------------
template<class H, P2ERR (*Fn)(H)>
void CKeeper<H, Fn>::operator=(H h)
{
    Attach(h);
}

//----------------------------------------------------------------------------
template<class H, P2ERR (*Fn)(H)>
void CKeeper<H, Fn>::Swap(CKeeper& other)
{
    H h = other.m_h;
    other.m_h = m_h;
    m_h = h;
}

} // namespace fo2base

