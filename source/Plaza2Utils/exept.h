//----------------------------------------------------------------------------
// Copyright: (c) 2006 RTS, Russia
//----------------------------------------------------------------------------
// Created:	    10 Feb 2006   16:19
// File:        except
// Author:		Mikhail_Navernyuk
//----------------------------------------------------------------------------
// Description: declaration of the base exception class
//----------------------------------------------------------------------------

#pragma once

#include "cexept.h"
#include "global.h"
#ifdef FASTCALL
#undef FASTCALL
#endif
#ifndef EIF_LEAN
#include <P2Errors.h>
#endif
#include <cstdarg>

namespace fo2base
{

//----------------------------------------------------------------------------
/// Инкапсулирует коды ошибок Plaza II
//----------------------------------------------------------------------------
class CP2Error : public CExcept
{
public:
    explicit CP2Error() : 
        CExcept(), m_code(P2ERR_OK) 
    {
    }
    
    explicit CP2Error(P2ERR code);
    explicit CP2Error(std::exception const& e);
    explicit CP2Error(const std::string& s, P2ERR r) : CExcept(s), m_code(r) {}
    explicit CP2Error(const char* s, P2ERR r=P2ERR_COMMON_INTERNAL_ERROR) : CExcept(s), m_code(r)  {}
    explicit CP2Error(P2ERR code, const char* lpszFormat, ...) : CExcept(), m_code(code)
    {
        int nBuf;
        char szBuffer[1024];
        va_list args;
        va_start(args, lpszFormat);
#if defined(_WIN32)
        nBuf = _vsnprintf_s(szBuffer, 1024, sizeof(szBuffer)/sizeof(szBuffer[0]), lpszFormat, args);
#else
        nBuf = vsnprintf(szBuffer, sizeof(szBuffer)/sizeof(szBuffer[0]), lpszFormat, args);
#endif
        m_str = szBuffer;
        findAndReplace(m_str,"%","%%");
        va_end(args);
    }

    virtual ~CP2Error() throw()
    {
    }

    P2ERR GetCode() const 
    { 
        return m_code; 
    }
    void ReplaceWithCode(P2ERR newCode) 
    { 
        m_code=newCode; 
    }

protected:
    P2ERR      m_code;
};

//----------------------------------------------------------------------------
/// Инкапсулирует ошибки клиента еРеплики
//----------------------------------------------------------------------------
class CEReplError : public CP2Error
{
public:
    explicit CEReplError() : CP2Error(P2ERR_OK) {}
    explicit CEReplError(const std::string& s, P2ERR r) : CP2Error(s, r) {}
    explicit CEReplError(const char* s, P2ERR r=P2ERR_COMMON_INTERNAL_ERROR) : CP2Error(s, r) {}

    virtual ~CEReplError() throw()
    {
    };
};

//----------------------------------------------------------------------------
/// Wrong argument exception
//----------------------------------------------------------------------------
class CArgumentException : public CP2Error
{
public:
    explicit CArgumentException();
    explicit CArgumentException(const std::string& txt);
	explicit CArgumentException(LPCTSTR lpszFormat, ...);
    virtual ~CArgumentException() throw() {}

    std::string GetParamName() 
    {
        return m_agr;
    }

protected:
    std::string m_agr;
};

//----------------------------------------------------------------------------
/// Null argument exception
//----------------------------------------------------------------------------
class CArgumentNullException : public CArgumentException
{
public:
    explicit CArgumentNullException();
    explicit CArgumentNullException(const std::string& txt);
    virtual ~CArgumentNullException() throw() {}
};

//----------------------------------------------------------------------------
/// Reply exception
/// Возбуждается в случае, возникновения ошибки на уровне бизнес логики. 
/// Приводит к формированию ответа на исходный запрос, с описанием возникшей
/// ошибки.
//----------------------------------------------------------------------------
class CReplyException : public CP2Error
{
public:
    explicit CReplyException();
    explicit CReplyException(const std::string& txt);
    virtual ~CReplyException() throw() {}
};


} // namespace fo2base

