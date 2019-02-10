//----------------------------------------------------------------------------
// Copyright: (c) 2006 RTS, Russia
//----------------------------------------------------------------------------
// Created:	    10 Feb 2006   18:08
// File:        p2log
// Author:		Mikhail_Navernyuk
//----------------------------------------------------------------------------
// Description: Interface of the CP2Log class
//----------------------------------------------------------------------------

#pragma once

#include "global.h"
#include "exept.h"
#include "handle.h"
#include "singleton.h"

#include <P2SysLog.h>

namespace fo2base
{

class CP2Ini;

P2ERR P2CALLSPEC P2LogCustCloseHelper(P2LOG_HANDLE log);

typedef CHandle<P2LOG_HANDLE, P2LogCustCloseHelper> CP2LogHandle;

//---------------------------------------------------------------------------
// class CP2Log
//---------------------------------------------------------------------------
class CP2Log : public CP2LogHandle
{
public:
    void Open(const CP2Ini& ini, const string& section = "");
    void Close();

    bool IsOpen();

    void Flush();

    void Log(PCSTR fmt, ...);
    void LogEx(UINT32 severity, PCSTR category, PCSTR fmt, ...);
    void LogInfo(PCSTR fmt, ...);
    void LogWarning(PCSTR fmt, ...);
    void LogError(PCSTR fmt, ...);
    void LogErrorV(PCSTR fmt, va_list vargs);
    void LogFatal(PCSTR fmt, ...);
    void LogInfoEx(PCSTR category, PCSTR fmt, ...);
    void LogWarningEx(PCSTR category, PCSTR fmt, ...);
    void LogErrorEx(PCSTR category, PCSTR fmt, ...);
    void LogFatalEx(PCSTR category, PCSTR fmt, ...);
    void LogErrorWithCode(P2ERR err, PCSTR fmt, ...);
    void LogFatalWithCode(P2ERR err, PCSTR fmt, ...);
    void LogErrorWithCodeEx(P2ERR err, PCSTR category, PCSTR fmt, ...);
    void LogFatalWithCodeEx(P2ERR err, PCSTR category, PCSTR fmt, ...);

public:
    int trcheck_Log_(PCSTR fmt, ...);
    int trcheck_LogEx_(UINT32 severity, PCSTR category, PCSTR fmt, ...);
    int trcheck_LogInfo_(PCSTR fmt, ...);
    int trcheck_LogWarning_(PCSTR fmt, ...);
    int trcheck_LogError_(PCSTR fmt, ...);    
    int trcheck_LogErrorV_(PCSTR fmt, va_list vargs);
    int trcheck_LogInfoEx_(PCSTR category, PCSTR fmt, ...);
    int trcheck_LogWarningEx_(PCSTR category, PCSTR fmt, ...);
    int trcheck_LogErrorEx_(PCSTR category, PCSTR fmt, ...);
    int trcheck_LogErrorWithCode_(P2ERR err, PCSTR fmt, ...);
    int trcheck_LogErrorWithCodeEx_(P2ERR err, PCSTR category, PCSTR fmt, ...);

    int trcheck_Log_0(PCSTR fmt, ...);
    int trcheck_LogEx_0(UINT32 severity, PCSTR category, PCSTR fmt, ...);
    int trcheck_LogInfo_0(PCSTR fmt, ...);
    int trcheck_LogWarning_0(PCSTR fmt, ...);
    int trcheck_LogError_0(PCSTR fmt, ...);
    int trcheck_LogInfoEx_0(PCSTR category, PCSTR fmt, ...);
    int trcheck_LogWarningEx_0(PCSTR category, PCSTR fmt, ...);
    int trcheck_LogErrorEx_0(PCSTR category, PCSTR fmt, ...);
    int trcheck_LogErrorWithCode_0(P2ERR err, PCSTR fmt, ...);
    int trcheck_LogErrorWithCodeEx_0(P2ERR err, PCSTR category, PCSTR fmt, ...);
protected:
    virtual void OpenImp(const CP2Ini& ini, const string& section = "");

public:
    CP2Log();
    virtual ~CP2Log();

private:
    BAN_COPY_CONSTRUCTOR(CP2Log)

};  // CP2Log

typedef CSingletonHolder<CP2Log>  CP2LogHolder;

//----------------------------------------------------------------------------
// inline functions
//----------------------------------------------------------------------------
inline bool CP2Log::IsOpen()
{
   return IsInitialized();
}

//---------------------------------------------------------------------------
inline void CP2Log::Close()
{
    Release();
}



//----------------------------------------------------------------------------
inline CP2Log* gGetLog()
{ 
    return CP2LogHolder::GetInstance();
}

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
/// class CP2DefaultLog
/// Инкапсулирует работу с дефолтным логом. Библиотеки Plaza II работают 
/// именно с дефолтным логом, поэтому приложение должно отрыть этот лог.
//----------------------------------------------------------------------------
class CP2DefaultLog : public CP2Log
{
public:
    CP2DefaultLog() : CP2Log() {}
    virtual ~CP2DefaultLog() {}

protected:
    virtual void OpenImp(const CP2Ini& ini, const string& section = "");

private:
    BAN_COPY_CONSTRUCTOR(CP2DefaultLog)

};  // CP2DefaultLog

typedef CSingletonHolder<CP2DefaultLog>  CP2DefaultLogHolder;


//----------------------------------------------------------------------------
inline CP2DefaultLog* gGetDefaultLog() 
{ 
    return CP2DefaultLogHolder::GetInstance(); 
}

#ifndef _MT
#define FO2TRACE_GEN_(func, args)                                   \
{                                                                   \
    static int tstate = -1;                                         \
    if (tstate == -1) tstate = (fo2base::gGetLog()->trcheck_##func##_ args); \
        else if (tstate) fo2base::gGetLog().func args;                      \
}

#define FO2TRACE_GEN_0(func, args)                                  \
{                                                                   \
    static int tstate = -1;                                         \
    if (tstate == -1) tstate = (fo2base::gGetLog()->trcheck_##func##_0 args);\
        else if (tstate) fo2base::gGetLog().func args;                      \
}

#else // _MT

#define FO2TRACE_GEN_(func, args)                                               \
{                                                                               \
    static LONG volatile tstate = -1;                                           \
    if (tstate == -1)                                                           \
    InterlockedExchange((LPLONG)&tstate, (fo2base::gGetLog()->trcheck_##func##_ args));  \
        else if (tstate) fo2base::gGetLog()->func args;                                  \
}

#define FO2TRACE_GEN_0(func, args)                                              \
{                                                                               \
    static LONG volatile tstate = -1;                                           \
    if (tstate == -1)                                                           \
    InterlockedExchange((LPLONG)&tstate, (fo2base::gGetLog()->trcheck_##func##_0 args)); \
        else if (tstate) fo2base::gGetLog()->func args;                                  \
}

#endif // _MT

/*
#define FO2TRACE_GEN_(func, args)                               \
    {                                                           \
        static int tstate = -1;                                 \
        if (tstate == -1) tstate = (gGetLog()->trcheck_##func##_ args);    \
        else if (tstate) gGetLog()->func args;                             \
    }
*/

#ifndef FO2TRACE_ALREADY_DEFINED
#define FO2TRACE_ALREADY_DEFINED
#define FO2TRACE(args) FO2TRACE_GEN_(Log, args)
#define FO2TRACE_EX(args) FO2TRACE_GEN_(LogEx, args)
#define FO2TRACE_INFO(args) FO2TRACE_GEN_(LogInfo, args)
#define FO2TRACE_WARNING(args) FO2TRACE_GEN_(LogWarning, args)
#define FO2TRACE_ERROR(args) FO2TRACE_GEN_(LogError, args)
#define FO2TRACE_FATAL(args) fo2base::gGetLog()->LogFatal args;

#define FO2TRACE_ERROR_VA(fmt,vargs)                                                                     \
    {                                                                                                    \
		INT32* res = NULL;													\
        res = trcheckEx_P2LogCustExv_(gGetLog()->GetHandle(), P2LOG_SEVERITY_ERROR, P2LOG_DEF_CATEGORY, fmt);            \
        if(res != NULL && *res != 0)                                                                                     \
        {                                                                                                \
            P2LogCustExv(gGetLog()->GetHandle(), P2LOG_SEVERITY_ERROR, P2LOG_DEF_CATEGORY, fmt, vargs ); \
        };                                                                                               \
    }

#define FO2TRACE_INFO_EX(args) FO2TRACE_GEN_(LogInfoEx, args)
#define FO2TRACE_WARNING_EX(args) FO2TRACE_GEN_(LogWarningEx, args)
#define FO2TRACE_ERROR_EX(args) FO2TRACE_GEN_(LogErrorEx, args)
#define FO2TRACE_ERROR_WITH_CODE(args) FO2TRACE_GEN_(LogErrorWithCode, args)
#define FO2TRACE_ERROR_WITH_CODE_EX(args) FO2TRACE_GEN_(LogErrorWithCodeEx, args)

#define FO2TRACE_0(args) FO2TRACE_GEN_0(Log, args)
#define FO2TRACE_EX_0(args) FO2TRACE_GEN_0(LogEx, args)
#define FO2TRACE_INFO_0(args) FO2TRACE_GEN_0(LogInfo, args)
#define FO2TRACE_WARNING_0(args) FO2TRACE_GEN_0(LogWarning, args)
#define FO2TRACE_ERROR_0(args) FO2TRACE_GEN_0(LogErrorV, args)
#define FO2TRACE_INFO_EX_0(args) FO2TRACE_GEN_0(LogInfoEx, args)
#define FO2TRACE_WARNING_EX_0(args) FO2TRACE_GEN_0(LogWarningEx, args)
#define FO2TRACE_ERROR_EX_0(args) FO2TRACE_GEN_0(LogErrorEx, args)
#define FO2TRACE_ERROR_WITH_CODE_0(args) FO2TRACE_GEN_0(LogErrorWithCode, args)
#define FO2TRACE_ERROR_WITH_CODE_EX_0(args) FO2TRACE_GEN_0(LogErrorWithCodeEx, args)
#endif 

extern int FO2DEBUG_TRACE_FLAG;

#ifdef _DEBUG
#define FO2DEBUG_TRACE(title, o) if (FO2DEBUG_TRACE_FLAG) {o.Trace(title);}
#endif
#ifndef _DEBUG
#define FO2DEBUG_TRACE(title, o) 
#endif

#ifdef _DEBUG
#define FO2DEBUG_TRACE2(args) if (FO2DEBUG_TRACE_FLAG) {FO2TRACE_INFO(args)}
#endif
#ifndef _DEBUG
#define FO2DEBUG_TRACE2(args) 
#endif

} // namespace fo2base
