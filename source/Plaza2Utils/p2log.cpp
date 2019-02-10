//----------------------------------------------------------------------------
// Copyright: (c) 2006 RTS, Russia
//----------------------------------------------------------------------------
// Created:	    10 Feb 2006   18:10
// File:        p2log
// Author:		Mikhail_Navernyuk
//----------------------------------------------------------------------------
// Description: Implementattion of the CP2Log class
//----------------------------------------------------------------------------

#include "stdafx.h"

#include "p2ini.h"
#include "p2log.h"

//----------------------------------------------------------------------------
// Эта функция должна быть реализована в основном приложении, для того, чтобы
// можно было воспользоваться механизмом тейсов.
//----------------------------------------------------------------------------
// Функция возвращает форматную строку по идентификатору.
//----------------------------------------------------------------------------
const char* gGetFormatString(int msg_id);

namespace fo2base
{

int FO2DEBUG_TRACE_FLAG = 0;

//----------------------------------------------------------------------------
P2ERR P2CALLSPEC P2LogCustCloseHelper(P2LOG_HANDLE log)
{
    P2LogCustClose(log);
    return P2ERR_OK;
}

//----------------------------------------------------------------------------
CP2Log::CP2Log() : CP2LogHandle()
{
}

//----------------------------------------------------------------------------
CP2Log::~CP2Log()
{
}

//----------------------------------------------------------------------------
void CP2Log::Open(const CP2Ini& ini, const string& section)
{
    OpenImp(ini, section);
}

//----------------------------------------------------------------------------
void CP2Log::OpenImp(const CP2Ini& ini, const string& section)
{
    P2LOG_HANDLE    h;
    P2ERR           rc;
    if(P2FAILEDV(P2LogCustOpenEx(&h, section.c_str(), ini.GetHandle()), rc))
    {
		if (rc!=P2ERR_LOG_ALREADY_OPENED)
		{
			switch (rc)
			{
			case P2ERR_LOG_ALREADY_OPENED:
				{
					std::string s = "Log file is already open";
					throw CP2Error(s, rc);
				}
			case P2ERR_INI_NOT_STARTED:
				{
					std::string s = "Ini filename is not started";
					throw CP2Error(s, rc);
				}
			case P2ERR_LOG_INVALID_INI_VALUE:
				{
					std::string s = "Bad value argument in Ini file. Check keys: 'logfilenametype', 'logfileperday', 'logtoconsole', 'logtimе', 'logfiledepth'";
					throw CP2Error(s, rc);
				}
			}
		}
    }
    Attach(h);
}

//----------------------------------------------------------------------------
void CP2Log::Flush()
{
    P2ERR           rc;
    if(P2FAILEDV(P2LogCustFlush(GetHandle()), rc))
    {
        throw CP2Error(rc);
    }
}

//----------------------------------------------------------------------------
void CP2Log::Log(PCSTR fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    P2LogCustExv(GetHandle(), P2LOG_DEF_SEVERITY, P2LOG_DEF_CATEGORY, fmt, args);
    va_end(args);
}

//----------------------------------------------------------------------------
void CP2Log::LogEx(UINT32 severity, PCSTR category, PCSTR fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    P2LogCustExv(GetHandle(), severity, category, fmt, args);
    va_end(args);
}

//----------------------------------------------------------------------------
void CP2Log::LogInfo(PCSTR fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    P2LogCustExv(GetHandle(), P2LOG_SEVERITY_INFO, P2LOG_DEF_CATEGORY, fmt, args);
    va_end(args);
}

//----------------------------------------------------------------------------
void CP2Log::LogWarning(PCSTR fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    P2LogCustExv(GetHandle(), P2LOG_SEVERITY_WARNING, P2LOG_DEF_CATEGORY, fmt, args);
    va_end(args);
}

//----------------------------------------------------------------------------
void CP2Log::LogError(PCSTR fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    P2LogCustExv(GetHandle(), P2LOG_SEVERITY_ERROR, P2LOG_DEF_CATEGORY, fmt, args);
    va_end(args);
}

//----------------------------------------------------------------------------
void CP2Log::LogErrorV( PCSTR fmt, va_list vargs )
{
    P2LogCustExv(GetHandle(), P2LOG_SEVERITY_ERROR, P2LOG_DEF_CATEGORY, fmt, vargs);
}                                                                             

//----------------------------------------------------------------------------
void CP2Log::LogFatal(PCSTR fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    P2LogCustExv(GetHandle(), P2LOG_SEVERITY_FATAL, P2LOG_DEF_CATEGORY, fmt, args);
    va_end(args);
}

//----------------------------------------------------------------------------
void CP2Log::LogInfoEx(PCSTR category, PCSTR fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    P2LogCustExv(GetHandle(), P2LOG_SEVERITY_INFO, category, fmt, args);
    va_end(args);
}

//----------------------------------------------------------------------------
void CP2Log::LogWarningEx(PCSTR category, PCSTR fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    P2LogCustExv(GetHandle(), P2LOG_SEVERITY_WARNING, category, fmt, args);
    va_end(args);
}

//----------------------------------------------------------------------------
void CP2Log::LogErrorEx(PCSTR category, PCSTR fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    P2LogCustExv(GetHandle(), P2LOG_SEVERITY_ERROR, category, fmt, args);
    va_end(args);
}

//----------------------------------------------------------------------------
void CP2Log::LogFatalEx(PCSTR category, PCSTR fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    P2LogCustExv(GetHandle(), P2LOG_SEVERITY_FATAL, category, fmt, args);
    va_end(args);
}

//----------------------------------------------------------------------------
void CP2Log::LogErrorWithCode(P2ERR err, PCSTR fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    P2LogCustWithCodeExv(GetHandle(), err, P2LOG_SEVERITY_ERROR, P2LOG_DEF_CATEGORY, fmt, args);
    va_end(args);
}

//----------------------------------------------------------------------------
void CP2Log::LogFatalWithCode(P2ERR err, PCSTR fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    P2LogCustWithCodeExv(GetHandle(), err, P2LOG_SEVERITY_FATAL, P2LOG_DEF_CATEGORY, fmt, args);
    va_end(args);
}

//----------------------------------------------------------------------------
void CP2Log::LogErrorWithCodeEx(P2ERR err, PCSTR category, PCSTR fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    P2LogCustWithCodeExv(GetHandle(), err, P2LOG_SEVERITY_ERROR, category, fmt, args);
    va_end(args);
}

//----------------------------------------------------------------------------
void CP2Log::LogFatalWithCodeEx(P2ERR err, PCSTR category, PCSTR fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    P2LogCustWithCodeExv(GetHandle(), err, P2LOG_SEVERITY_FATAL, category, fmt, args);
    va_end(args);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

#define FO2TRACE_CHECK_(log, severity, category)                    \
{                                                                   \
    INT32* res = NULL;												\
	res = trcheckEx_P2LogCustExv_(log, severity, category, fmt);    \
	if(res == NULL) return 1;										\
    INT32 local_res = *res;                                         \
    if(local_res != 0)                                                        \
    {                                                               \
        va_list args;                                               \
        va_start(args, fmt);                                        \
        P2LogCustExv(log, severity, category, fmt, args);           \
        local_res = 1;                                                    \
        va_end(args);                                               \
    }                                                               \
    return local_res;                                                     \
}

#define FO2TRACE_CHECK_CODE_(log, severity, category)                    \
{                                                                       \
	INT32* res = NULL;													\
    res = trcheckEx_P2LogCustExv_(log, severity, category, fmt);        \
	if(res == NULL) return 1;											\
    INT32 local_res = *res;                                         \
    if (local_res != 0)                                                           \
    {                                                                   \
        va_list args;                                                   \
        va_start(args, fmt);                                            \
        P2LogCustWithCodeExv(log, err, severity, category, fmt, args);  \
        local_res = 1;                                                        \
        va_end(args);                                                   \
    }                                                                   \
    return local_res;                                                         \
}

//----------------------------------------------------------------------------
#define FO2TRACE_CHECK_0(log, severity, category)                   \
{                                                                   \
	INT32* res = NULL;												\
    res = trcheckEx_P2LogCustExv_0(log, severity, category, fmt);  \
	if(res == NULL) return 0;										\
    INT32 local_res = *res;                                         \
    if (local_res != 0)                                                       \
    {                                                               \
        va_list args;                                               \
        va_start(args, fmt);                                        \
        P2LogCustExv(log, severity, category, fmt, args);           \
        local_res = 1;                                                    \
        va_end(args);                                               \
    }                                                               \
    return local_res;                                                     \
}

//----------------------------------------------------------------------------
#define FO2TRACE_CHECK_CODE_0(log, severity, category)                  \
{                                                                       \
	INT32* res = NULL;													\
    res = trcheckEx_P2LogCustExv_0(log, severity, category, fmt);      \
	if(res == NULL) return 0;										\
    INT32 local_res = *res;                                         \
    if (local_res != 0)						                                    \
    {                                                                   \
        va_list args;                                                   \
        va_start(args, fmt);                                            \
        P2LogCustWithCodeExv(log, err, severity, category, fmt, args);  \
        local_res = 1;                                                        \
        va_end(args);                                                   \
    }                                                                   \
    return local_res;                                                         \
}


//----------------------------------------------------------------------------
int CP2Log::trcheck_Log_(PCSTR fmt, ...)
    FO2TRACE_CHECK_(GetHandle(), P2LOG_DEF_SEVERITY, P2LOG_DEF_CATEGORY)

int CP2Log::trcheck_LogEx_(UINT32 severity, PCSTR category, PCSTR fmt, ...)
    FO2TRACE_CHECK_(GetHandle(), severity, category)

int CP2Log::trcheck_LogInfo_(PCSTR fmt, ...)
    FO2TRACE_CHECK_(GetHandle(), P2LOG_SEVERITY_INFO, P2LOG_DEF_CATEGORY)

int CP2Log::trcheck_LogWarning_(PCSTR fmt, ...)
    FO2TRACE_CHECK_(GetHandle(), P2LOG_SEVERITY_WARNING, P2LOG_DEF_CATEGORY)

int CP2Log::trcheck_LogError_(PCSTR fmt, ...)
    FO2TRACE_CHECK_(GetHandle(), P2LOG_SEVERITY_ERROR, P2LOG_DEF_CATEGORY)

//----------------------------------------------------------------------------
int CP2Log::trcheck_LogErrorV_( PCSTR fmt, va_list vargs )
{
    INT32* res = NULL;
	res = trcheckEx_P2LogCustExv_(GetHandle(), P2LOG_SEVERITY_ERROR, P2LOG_DEF_CATEGORY, fmt);
	if(res == NULL) return 1;
    INT32 local_res = *res;                                         
    if(local_res != 0)
    {
        P2LogCustExv(GetHandle(), P2LOG_SEVERITY_ERROR, P2LOG_DEF_CATEGORY, fmt, vargs);
        local_res = 1;
    }
    return local_res;
}

int CP2Log::trcheck_LogInfoEx_(PCSTR category, PCSTR fmt, ...)
    FO2TRACE_CHECK_(GetHandle(), P2LOG_SEVERITY_INFO, category)

int CP2Log::trcheck_LogWarningEx_(PCSTR category, PCSTR fmt, ...)
    FO2TRACE_CHECK_(GetHandle(), P2LOG_SEVERITY_WARNING, category)

int CP2Log::trcheck_LogErrorEx_(PCSTR category, PCSTR fmt, ...)
    FO2TRACE_CHECK_(GetHandle(), P2LOG_SEVERITY_ERROR, category)

int CP2Log::trcheck_LogErrorWithCode_(P2ERR err, PCSTR fmt, ...)
    FO2TRACE_CHECK_CODE_(GetHandle(), P2LOG_SEVERITY_ERROR, P2LOG_DEF_CATEGORY)

int CP2Log::trcheck_LogErrorWithCodeEx_(P2ERR err, PCSTR category, PCSTR fmt, ...)
    FO2TRACE_CHECK_CODE_(GetHandle(), P2LOG_SEVERITY_ERROR, category)



//----------------------------------------------------------------------------
int CP2Log::trcheck_Log_0(PCSTR fmt, ...)
    FO2TRACE_CHECK_0(GetHandle(), P2LOG_DEF_SEVERITY, P2LOG_DEF_CATEGORY)

int CP2Log::trcheck_LogEx_0(UINT32 severity, PCSTR category, PCSTR fmt, ...)
    FO2TRACE_CHECK_0(GetHandle(), severity, category)

int CP2Log::trcheck_LogInfo_0(PCSTR fmt, ...)
    FO2TRACE_CHECK_0(GetHandle(), P2LOG_SEVERITY_INFO, P2LOG_DEF_CATEGORY)

int CP2Log::trcheck_LogWarning_0(PCSTR fmt, ...)
    FO2TRACE_CHECK_0(GetHandle(), P2LOG_SEVERITY_WARNING, P2LOG_DEF_CATEGORY)

int CP2Log::trcheck_LogError_0(PCSTR fmt, ...)
    FO2TRACE_CHECK_0(GetHandle(), P2LOG_SEVERITY_ERROR, P2LOG_DEF_CATEGORY)

int CP2Log::trcheck_LogInfoEx_0(PCSTR category, PCSTR fmt, ...)
    FO2TRACE_CHECK_0(GetHandle(), P2LOG_SEVERITY_INFO, category)

int CP2Log::trcheck_LogWarningEx_0(PCSTR category, PCSTR fmt, ...)
    FO2TRACE_CHECK_0(GetHandle(), P2LOG_SEVERITY_WARNING, category)

int CP2Log::trcheck_LogErrorEx_0(PCSTR category, PCSTR fmt, ...)
    FO2TRACE_CHECK_0(GetHandle(), P2LOG_SEVERITY_ERROR, category)

int CP2Log::trcheck_LogErrorWithCode_0(P2ERR err, PCSTR fmt, ...)
    FO2TRACE_CHECK_CODE_0(GetHandle(), P2LOG_SEVERITY_ERROR, P2LOG_DEF_CATEGORY)

int CP2Log::trcheck_LogErrorWithCodeEx_0(P2ERR err, PCSTR category, PCSTR fmt, ...)
    FO2TRACE_CHECK_CODE_0(GetHandle(), P2LOG_SEVERITY_ERROR, category)


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#define FO2TRACE_M_CHECK_(log, severity, category)                  \
{                                                                   \
	INT32* res = NULL;													\
    res = trcheckEx_P2LogCustExv_(log, severity, category,        \
        gGetFormatString(msg_id));                                  \
	if(res == NULL) return 1;										\
    INT32 local_res = *res;                                         \
    if(local_res != 0)                                                    \
    {                                                               \
        va_list args;                                               \
        va_start(args, msg_id);                                     \
        P2LogCustExv(log, severity, category,                       \
            gGetFormatString(msg_id), args);                        \
        local_res = 1;                                                    \
        va_end(args);                                               \
    }                                                               \
    return local_res;                                                     \
}

#define FO2TRACE_M_CHECK_CODE_(log, severity, category)                 \
{                                                                       \
	INT32* res = NULL;													\
    res = trcheckEx_P2LogCustExv_(log, severity, category,            \
        gGetFormatString(msg_id));                                      \
	if(res == NULL) return 1;										\
    INT32 local_res = *res;                                         \
    if (local_res != 0)                                                       \
    {                                                                   \
        va_list args;                                                   \
        va_start(args, msg_id);                                         \
        P2LogCustWithCodeExv(log, err, severity, category,              \
            gGetFormatString(msg_id), args);                            \
        local_res = 1;                                                        \
        va_end(args);                                                   \
    }                                                                   \
    return local_res;                                                         \
}

//----------------------------------------------------------------------------
#define FO2TRACE_M_CHECK_0(log, severity, category)                 \
{                                                                   \
	INT32* res = NULL;													\
    res = trcheckEx_P2LogCustExv_0(log, severity, category,      \
        gGetFormatString(msg_id));                                  \
	if(res == NULL) return 0;										\
    INT32 local_res = *res;                                         \
    if (local_res != 0)                                                   \
    {                                                               \
        va_list args;                                               \
        va_start(args, msg_id);                                     \
        P2LogCustExv(log, severity, category,                       \
            gGetFormatString(msg_id), args);                        \
        local_res = 1;                                                    \
        va_end(args);                                               \
    }                                                               \
    return local_res;                                                     \
}

//----------------------------------------------------------------------------
#define FO2TRACE_M_CHECK_CODE_0(log, severity, category)                \
{                                                                       \
	INT32* res = NULL;													\
    res = trcheckEx_P2LogCustExv_0(log, severity, category,          \
        gGetFormatString(msg_id));                                     \
	if(res == NULL) return 0;										\
    INT32 local_res = *res;                                         \
    if (local_res != 0)                                                       \
    {                                                                   \
        va_list args;                                                   \
        va_start(args, msg_id);                                         \
        P2LogCustWithCodeExv(log, err, severity, category,              \
            gGetFormatString(msg_id), args);                            \
        local_res = 1;                                                        \
        va_end(args);                                                   \
    }                                                                   \
    return local_res;                                                         \
}

//----------------------------------------------------------------------------
// class CP2DefaultLog
//----------------------------------------------------------------------------
/// 
//----------------------------------------------------------------------------
void CP2DefaultLog::OpenImp(const CP2Ini& ini, const string& section)
{
    P2ERR           rc;
    if(P2FAILEDV(P2LogOpenEx(section.c_str(), ini.GetHandle()), rc))
    {
		if (rc!=P2ERR_LOG_ALREADY_OPENED)
		{
	        throw CP2Error(rc);
		}
    }
#pragma warning (disable: 4312)
    Attach(P2LogGetDefHandle());
#pragma warning (default: 4312)
}

} // namespace fo2base

