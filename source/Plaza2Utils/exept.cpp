//----------------------------------------------------------------------------
// Copyright: (c) 2006 RTS, Russia
//----------------------------------------------------------------------------
// Created:	    10 Feb 2006   16:36
// File:        exept
// Author:		Mikhail_Navernyuk
//----------------------------------------------------------------------------
// Description: implementation of the base exception class
//----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef FASTCALL
#undef FASTCALL
#endif
#include <P2SysTypes.h>

#include "exept.h"
#include "p2errors.h"



//----------------------------------------------------------------------------
/// Подменяет exceptions возбуждаемые boost на CP2Error.
/// \param[in] e - boost exception
//----------------------------------------------------------------------------
void throw_exception(std::exception const& e) // user defined
{
    throw fo2base::CP2Error(e);
}


namespace fo2base
{

//----------------------------------------------------------------------------
// CP2Error
//----------------------------------------------------------------------------
CP2Error::CP2Error(P2ERR code)
{
    PCSTR msg;
	m_code = code;
    if(code < P2ERR_COMMON_ERROR)
    {
        msg = P2ErrGetDescrStr(code);
    }
    else
    {
        msg = Fo2ErrGetDescrStr(code);
    }
    if(msg)
        m_str = msg;
}

//----------------------------------------------------------------------------
CP2Error::CP2Error(std::exception const& e)
{
    m_code = P2ERR_COMMON_INTERNAL_ERROR;
    m_str  = e.what();
}

//----------------------------------------------------------------------------
// CArgumentException
//----------------------------------------------------------------------------
CArgumentException::CArgumentException() : 
    CP2Error(P2ERR_COMMON_WRONG_ARGUMENTS), m_agr("")
{
}

//----------------------------------------------------------------------------
CArgumentException::CArgumentException(const std::string& txt) : 
    CP2Error(P2ERR_COMMON_WRONG_ARGUMENTS), m_agr(txt)
{
    m_str += " - ";
    m_str += m_agr;
}

CArgumentException::CArgumentException(LPCTSTR lpszFormat, ...) :
    CP2Error(P2ERR_COMMON_WRONG_ARGUMENTS), m_agr("")
{
    int nBuf;
    TCHAR szBuffer[1024];
    va_list args;
    va_start(args, lpszFormat);
#pragma warning(disable : 4996)
#if defined(_WIN32)
    nBuf = _vsnprintf(szBuffer, sizeof(szBuffer)/sizeof(szBuffer[0]), lpszFormat, args);
#else
    nBuf = vsnprintf(szBuffer, sizeof(szBuffer)/sizeof(szBuffer[0]), lpszFormat, args);
#endif
    m_str = szBuffer;
    va_end(args);

    m_str += " - ";
    m_str += m_agr;
};

//----------------------------------------------------------------------------
// CArgumentNullException
//----------------------------------------------------------------------------
CArgumentNullException::CArgumentNullException() : 
    CArgumentException("")
{
}

//----------------------------------------------------------------------------
CArgumentNullException::CArgumentNullException(const std::string& txt) : 
    CArgumentException(txt)
{
}


//----------------------------------------------------------------------------
// CReplyException
//----------------------------------------------------------------------------
CReplyException::CReplyException() : 
    CP2Error(P2ERR_COMMON_WRONG_ARGUMENTS)
{
    m_str  = "";
}

//----------------------------------------------------------------------------
CReplyException::CReplyException(const std::string& txt) : 
    CP2Error(P2ERR_COMMON_WRONG_ARGUMENTS)
{
    m_str  = txt;
}

} // namespace fo2base

