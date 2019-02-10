#ifndef _P2SYSLOG_H_
#define _P2SYSLOG_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include <stdarg.h>

#include "../../P2Common/Include/P2Types.h"

#ifdef P2SYS_EXPORTS
#define P2SYSLOG_API P2_API_EXPORTS
#else
#define P2SYSLOG_API P2_API_IMPORTS
#endif


#if (defined(__linux__) || defined(__MACH__)) && defined(_P2_PLATFORM_32_)
static inline INT64 PtrToLong(const void *p)
{
    return reinterpret_cast<INT64>(p);
}
#elif defined(_WIN32)

#include "../../../Plaza-II/P2Common/Include/P2Windows.h"

#include <basetsd.h> // for PtrToLong()
#endif //(__linux__ || __MACH__) && _P2_PLATFORM_32_


#include "../../P2Common/Include/P2Errors.h"
#include "../../P2SysIni/Include/P2SysIni.h"

#ifdef P2SYS_EXPORTS
  namespace NS_P2SysLog
  {
    class CLogMan;
    class CTraceShmemStorageManager;
  }
  using NS_P2SysLog::CLogMan;
  using NS_P2SysLog::CTraceShmemStorageManager;
#else
  struct CLogMan;
  struct CTraceShmemStorageManager;
#endif

// Severities
enum Severity
{
    P2LOG_SEVERITY_INFO = 0,
    P2LOG_SEVERITY_WARNING,
    P2LOG_SEVERITY_ERROR,
    P2LOG_SEVERITY_FATAL
};

#define P2LOG_COL_SEP           ";"     // Columns separator
//#define P2LOG_FILE_EXT          "log"   // Log file extention



//  INI parameter names
#define P2LOG_INI_PARAM_LOGFILE			"logfile"
#define P2LOG_INI_PARAM_LOGFILENAMETYPE	"logfilenametype"
#define P2LOG_INI_PARAM_LOGFILEPERDAY	"logfileperday"
#define P2LOG_INI_PARAM_LOGFILEDEPTH	"logfiledepth"
#define P2LOG_INI_PARAM_LOGFILECACHE	"logfilecache"
#define P2LOG_INI_PARAM_LOGTIME			"logtime"
#define P2LOG_INI_PARAM_LOGTOCONSOLE	"logtoconsole"
#define P2LOG_INI_PARAM_LOGTRACEINI		"traceini"
#define P2LOG_INI_PARAM_THREADID		"addthreadid"
#define P2LOG_INI_PARAM_PRELOADTRACES	"preload_traces"
#define P2LOG_INI_PARAM_LOGASYNC		"logasync"
#define P2LOG_INI_PARAM_AFFINITY		"logthread_affinity"
#define P2LOG_INI_PARAM_MSG_POOL_SIZE	"msg_pool_size"

#define P2LOG_INI_PARAM_LOGREMOTE		"remote_log"
#define P2LOG_INI_PARAM_REMOTEIP		"remote_ip"
#define P2LOG_INI_PARAM_REMOTEPORT		"remote_port"
#define P2LOG_INI_PARAM_DEFREMOTEPORT	514
#define P2LOG_INI_PARAM_REMOTEBUFSZ     "remote_log_snd_buf"
#define P2LOG_INI_PARAM_APPNAME			"remote_log_appname"
#define P2LOG_INI_PARAM_REMOTELOG_USECOMMANDLINE ":command_line:"
#define P2LOG_INI_PARAM_NOREMOTELOG     "no"
#define P2LOG_INI_PARAM_ONLYREMOTELOG   "only"
#define P2LOG_INI_PARAM_LOGWITHFILE		"both"
#define P2LOG_INI_PARAM_LOG_FILTER		"remote_log_filter"

#define P2LOG_INI_PARAM_ERRORLOGSECTION "err_log_section"
 


// Possible INI parameters
#define P2LOG_LOGFILENAMETYPE_RIGHT     0
#define P2LOG_LOGFILENAMETYPE_MIDDLE    1

#define P2LOG_LOGFILEPERDAY_OFF         0
#define P2LOG_LOGFILEPERDAY_ON          1
#define P2LOG_LOGFILEPERDAY_PERHOUR		2

#define P2LOG_LOGFILECACHE_DISABLE      0
#define P2LOG_LOGFILECACHE_MIN          64
#define P2LOG_LOGFILECACHE_MAX          32767

#define P2LOG_LOGTIME_NONE                      0
#define P2LOG_LOGTIME_PER_SECOND                1
#define P2LOG_LOGTIME_PER_LINE                  2
#define P2LOG_LOGTIME_PER_LINE_NO_DATE          3
#define P2LOG_LOGTIME_PER_LINE_HI_RES           4
#define P2LOG_LOGTIME_PER_LINE_HI_RES_0         5
#define P2LOG_LOGTIME_PER_LINE_NO_DATE_AND_HR0  6

#define P2LOG_LOGTOCONSOLE_OFF          0
#define P2LOG_LOGTOCONSOLE_ON           1

#define P2LOG_THREADID_OFF		      0
#define P2LOG_THREADID_ON		      1

#define P2LOG_PRELOADTRACES_OFF       0
#define P2LOG_PRELOADTRACES_ON        1

// Some INI defaults
#define P2LOG_DEF_INI_SECTION           "p2syslog"   // Default section name for default log
#define P2LOG_DEF_INI_LOGFILENAMETYPE   P2LOG_LOGFILENAMETYPE_MIDDLE
#define P2LOG_DEF_INI_LOGFILEPERDAY     P2LOG_LOGFILEPERDAY_OFF
#define P2LOG_DEF_INI_LOGFILEDEPTH      3
#define P2LOG_DEF_INI_LOGFILECACHE      -1 // use default allocated buffer (fully buffered)
#define P2LOG_DEF_INI_LOGTIME           P2LOG_LOGTIME_PER_LINE
#define P2LOG_DEF_INI_LOGTOCONSOLE      P2LOG_LOGTOCONSOLE_ON
#define P2LOG_DEF_INI_THREADID			P2LOG_THREADID_ON
#define P2LOG_DEF_INI_PRELOADTRACES     P2LOG_PRELOADTRACES_ON

// Some default functions parameters
#define P2LOG_DEF_SEVERITY      P2LOG_SEVERITY_INFO
#define P2LOG_DEF_CATEGORY      ""

//////////////////////////////////////////////////////////////////////
//  Возможность устанавливать опции лога при открытии лога.

#define P2LOG_OPTION_OVERRIDE_INI_VALUE	 0x1	// значение опции в ини файле игнорируется
#define P2LOG_OPTION_DEFAULT_INI_VALUE	 0x0	// используется, если в ини файле значение опции не задано

struct P2LOG_OPTION 
{
	const char* m_iniOptionName;     	// имя опции
	const char* m_iniOptionValue;		// значение опции
	int			m_overrideIniValue;		// одна из констант P2LOG_OPTION_*
};

//////////////////////////////////////////////////////////////////////

#ifdef P2SYS_EXPORTS
  namespace NS_P2SysLog
  {
    struct P2LogImpl;
  }
  using NS_P2SysLog::P2LogImpl;
#else
  struct P2LogImpl;
#endif

typedef struct P2LogImpl* P2LOG_HANDLE;

//////////////////////////////////////////////////////////////////////

#define P2LOG_DEF_HANDLE    ((P2LOG_HANDLE)~0)
#define P2LogGetDefHandle() P2LOG_DEF_HANDLE

//////////////////////////////////////////////////////////////////////
// Default log

// Open log with default INI (must be previously opened)
// (if iniSection == 0 or "", then P2LOG_DEF_INI_SECTION will be used)
P2SYSLOG_API P2ERR P2CALLSPEC P2LogOpen(PCSTR iniSection);

// Open log with given INI
// (if iniSection == 0 or "", then P2LOG_DEF_INI_SECTION will be used)
P2SYSLOG_API P2ERR P2CALLSPEC P2LogOpenEx(PCSTR iniSection, P2INI_HANDLE ini);

// pUserDefinedOption - массив опций, заканчивающийся NULL-именем
P2SYSLOG_API P2ERR P2CALLSPEC P2LogOpenWithOption(PCSTR iniSection, P2INI_HANDLE ini, const P2LOG_OPTION* pUserDefinedOptions);

P2SYSLOG_API BOOL P2CALLSPEC P2LogIsOpened();

// Close log
P2SYSLOG_API void P2CALLSPEC P2LogClose();

// General form
P2SYSLOG_API void P2CALLSPEC P2LogEx(UINT32 severity, PCSTR category, PCSTR fmt, ...);
//P2SYSLOG_API void P2CALLSPEC P2LogExv(UINT32 severity, PCSTR category, PCSTR fmt, va_list args);
#define P2LogExv(severity, category, fmt, args) \
    P2LogCustExv(P2LOG_DEF_HANDLE, severity, category, fmt, args)
//P2SYSLOG_API void P2CALLSPEC P2LogWithCodeExv(P2ERR err, UINT32 severity, PCSTR category, PCSTR fmt, va_list args);
#define P2LogWithCodeExv(err, severity, category, fmt, args) \
    P2LogCustWithCodeExv(P2LOG_DEF_HANDLE, err, severity, category, fmt, args)

// Simple form (severity = P2LOG_SEVERITY_INFO, category=<empty>)
P2SYSLOG_API void P2CALLSPEC P2Log(PCSTR fmt, ...);

// With fixed severity & without category (<empty>)
P2SYSLOG_API void P2CALLSPEC P2LogInfo(PCSTR fmt, ...);
P2SYSLOG_API void P2CALLSPEC P2LogWarning(PCSTR fmt, ...);
P2SYSLOG_API void P2CALLSPEC P2LogError(PCSTR fmt, ...);
P2SYSLOG_API void P2CALLSPEC P2LogFatal(PCSTR fmt, ...);

P2SYSLOG_API void P2CALLSPEC P2LogCurrentProcessInfo();

// With fixed severity
P2SYSLOG_API void P2CALLSPEC P2LogInfoEx(PCSTR category, PCSTR fmt, ...);
P2SYSLOG_API void P2CALLSPEC P2LogWarningEx(PCSTR category, PCSTR fmt, ...);
P2SYSLOG_API void P2CALLSPEC P2LogErrorEx(PCSTR category, PCSTR fmt, ...);
P2SYSLOG_API void P2CALLSPEC P2LogFatalEx(PCSTR category, PCSTR fmt, ...);

P2SYSLOG_API void P2CALLSPEC P2LogInfoSimple(PCSTR category, PCSTR msg);

// With P2ERR code
P2SYSLOG_API void P2CALLSPEC P2LogErrorWithCode(P2ERR err, PCSTR fmt, ...);
P2SYSLOG_API void P2CALLSPEC P2LogFatalWithCode(P2ERR err, PCSTR fmt, ...);
P2SYSLOG_API void P2CALLSPEC P2LogErrorWithCodeEx(P2ERR err, PCSTR category, PCSTR fmt, ...);
P2SYSLOG_API void P2CALLSPEC P2LogFatalWithCodeEx(P2ERR err, PCSTR category, PCSTR fmt, ...);

// Flush log
//P2SYSLOG_API P2ERR P2CALLSPEC P2LogFlush();
#define P2LogFlush()    P2LogCustFlush(P2LogGetDefHandle())

//////////////////////////////////////////////////////////////////////
// Custom log(s)

// Open log with default INI (must be previously opened)
// (if iniSection == 0 or "", then P2LOG_DEF_INI_SECTION will be used)
P2SYSLOG_API P2ERR P2CALLSPEC P2LogCustOpen(P2LOG_HANDLE* pLog, PCSTR iniSection);

// Open log with given INI
// (if iniSection == 0 or "", then P2LOG_DEF_INI_SECTION will be used)
P2SYSLOG_API P2ERR P2CALLSPEC P2LogCustOpenEx(P2LOG_HANDLE* pLog, PCSTR iniSection, P2INI_HANDLE ini);

// Close log
P2SYSLOG_API void P2CALLSPEC P2LogCustClose(P2LOG_HANDLE log);

// General form
P2SYSLOG_API void P2CALLSPEC P2LogCustEx(P2LOG_HANDLE log, UINT32 severity, PCSTR category, PCSTR fmt, ...);
P2SYSLOG_API void P2CALLSPEC P2LogCustExv(P2LOG_HANDLE log, UINT32 severity, PCSTR category, PCSTR fmt, va_list args);
P2SYSLOG_API void P2CALLSPEC P2LogCustWithCodeExv(P2LOG_HANDLE log, P2ERR err, UINT32 severity, PCSTR category, PCSTR fmt, va_list args);

// Simple form (severity = P2LOG_SEVERITY_INFO, category=<empty>)
P2SYSLOG_API void P2CALLSPEC P2LogCust(P2LOG_HANDLE log, PCSTR fmt, ...);

// With fixed severity & without category (<empty>)
P2SYSLOG_API void P2CALLSPEC P2LogCustInfo(P2LOG_HANDLE log, PCSTR fmt, ...);
P2SYSLOG_API void P2CALLSPEC P2LogCustWarning(P2LOG_HANDLE log, PCSTR fmt, ...);
P2SYSLOG_API void P2CALLSPEC P2LogCustError(P2LOG_HANDLE log, PCSTR fmt, ...);
P2SYSLOG_API void P2CALLSPEC P2LogCustFatal(P2LOG_HANDLE log, PCSTR fmt, ...);

// With fixed severity
P2SYSLOG_API void P2CALLSPEC P2LogCustInfoEx(P2LOG_HANDLE log, PCSTR category, PCSTR fmt, ...);
P2SYSLOG_API void P2CALLSPEC P2LogCustWarningEx(P2LOG_HANDLE log, PCSTR category, PCSTR fmt, ...);
P2SYSLOG_API void P2CALLSPEC P2LogCustErrorEx(P2LOG_HANDLE log, PCSTR category, PCSTR fmt, ...);
P2SYSLOG_API void P2CALLSPEC P2LogCustFatalEx(P2LOG_HANDLE log, PCSTR category, PCSTR fmt, ...);

// With P2ERR code
P2SYSLOG_API void P2CALLSPEC P2LogCustErrorWithCode(P2LOG_HANDLE log, P2ERR err, PCSTR fmt, ...);
P2SYSLOG_API void P2CALLSPEC P2LogCustFatalWithCode(P2LOG_HANDLE log, P2ERR err, PCSTR fmt, ...);
P2SYSLOG_API void P2CALLSPEC P2LogCustErrorWithCodeEx(P2LOG_HANDLE log, P2ERR err, PCSTR category, PCSTR fmt, ...);
P2SYSLOG_API void P2CALLSPEC P2LogCustFatalWithCodeEx(P2LOG_HANDLE log, P2ERR err, PCSTR category, PCSTR fmt, ...);

// Flush log
P2SYSLOG_API P2ERR P2CALLSPEC P2LogCustFlush(P2LOG_HANDLE log);

//////////////////////////////////////////////////////////////////////
// Overriding standard log writer
//
//  Using:
//
//  // Override current writer
//  m_oldWriter = P2LogCustSetWriter(log, MyLogWriter);
//  ...
//
//  // Implement new writer
//  P2ERR MyLogWriter(P2LOG_HANDLE log, const P2TIME* pTm, UINT32 severity, const char* pStr)
//  {
//      ...
//
//      // Call old log writer (or don't call, if you want)
//      return m_oldWriter(log, pTm, severity, pStr);
//  }
//

// размер строки должен быть большим для печати кошмарных SQL-запросов
#define P2LOG_MAX_MSG_LEN   16384

typedef P2ERR (P2CALLSPEC *P2LOG_WRITE_FUNC)(P2LOG_HANDLE log,
                                             const P2TIME* pTm, UINT32 severity, const char* pStr);

// Return: oldWriter address or 0 on any error
P2SYSLOG_API P2LOG_WRITE_FUNC P2CALLSPEC P2LogCustSetWriter(P2LOG_HANDLE log, P2LOG_WRITE_FUNC newWriter);

//////////////////////////////////////////////////////////////////////
// Overriding standard log flusher

typedef P2ERR (P2CALLSPEC *P2LOG_FLUSH_FUNC)(P2LOG_HANDLE log);

// Return: oldFlusher address or 0 on any error
P2SYSLOG_API P2LOG_FLUSH_FUNC P2CALLSPEC P2LogCustSetFlusher(P2LOG_HANDLE log, P2LOG_FLUSH_FUNC newFlusher);

//////////////////////////////////////////////////////////////////////
// Operating system log

#define P2LOG_SYSTEMLOG_SOURCE_NAME     "P2SysLog"

P2SYSLOG_API void P2CALLSPEC P2SystemLogEx(UINT32 severity, PCSTR category, BOOL copyToConsole, PCSTR fmt, ...);
P2SYSLOG_API void P2CALLSPEC P2SystemLogExv(UINT32 severity, PCSTR category, BOOL copyToConsole, PCSTR fmt, va_list args);

//////////////////////////////////////////////////////////////////////
// P2TRACE
//
//  Using:
//  P2TRACExxx( ([..., ]fmt_string[, args]) )
//
//      - all arguments in the double brackets
//      - without semicolon
//

// состояния строк в секции TRACE
enum
{
	P2TRACE_INI_STATE_UNKNOWN = -1,	// это значение не должно появляться в ini-файле
	P2TRACE_INI_STATE_OFF = 0,
	P2TRACE_INI_STATE_AS_GLOBAL = 1,
	P2TRACE_INI_STATE_ON = 2,
};

enum
{
	P2TRACE_STATE_UNKNOWN = -1,		// неопределено. статус должен быть считан из ini-файла 
	P2TRACE_STATE_OFF = 0,			// строка не выводится в лог
	P2TRACE_STATE_ON = 1			// строка выводится в лог. может быть переобпределено в глобальной настройке 
	//P2TRACE_STATE_ALWAYS_ON = 2	// строка всегда выводится в лог 
};


#define P2TRACE_GEN_(func, args)                                \
    {                                                           \
        static volatile INT32 * tstatePtr = NULL;               \
        if (NULL == tstatePtr) tstatePtr = (trcheckEx_##func##_ args); \
        else if (*tstatePtr) func args;                         \
    }

#define P2TRACE_GEN_0(func, args)                               \
    {                                                           \
        static volatile INT32 * tstatePtr = NULL;               \
        if (NULL == tstatePtr) tstatePtr = (trcheckEx_##func##_0 args); \
        else if (*tstatePtr) func args;                         \
    }

// Check current state of given trace & writes to log if state != 0
// Returns:
//  pointer to 'trace state' var
//  NULL - on any error
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogEx_(UINT32 severity, PCSTR category, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2Log_(PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogInfo_(PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogWarning_(PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogError_(PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogInfoEx_(PCSTR category, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogWarningEx_(PCSTR category, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogErrorEx_(PCSTR category, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogErrorWithCode_(P2ERR err, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogErrorWithCodeEx_(P2ERR err, PCSTR category, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogCustEx_(P2LOG_HANDLE log, UINT32 severity, PCSTR category, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogCust_(P2LOG_HANDLE log, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogCustInfo_(P2LOG_HANDLE log, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogCustWarning_(P2LOG_HANDLE log, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogCustError_(P2LOG_HANDLE log, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogCustInfoEx_(P2LOG_HANDLE log, PCSTR category, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogCustWarningEx_(P2LOG_HANDLE log, PCSTR category, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogCustErrorEx_(P2LOG_HANDLE log, PCSTR category, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogCustErrorWithCode_(P2LOG_HANDLE log, P2ERR err, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogCustErrorWithCodeEx_(P2LOG_HANDLE log, P2ERR err, PCSTR category, PCSTR fmt, ...);

P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogEx_0(UINT32 severity, PCSTR category, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2Log_0(PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogInfo_0(PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogWarning_0(PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogError_0(PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogInfoEx_0(PCSTR category, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogWarningEx_0(PCSTR category, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogErrorEx_0(PCSTR category, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogErrorWithCode_0(P2ERR err, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogErrorWithCodeEx_0(P2ERR err, PCSTR category, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogCustEx_0(P2LOG_HANDLE log, UINT32 severity, PCSTR category, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogCust_0(P2LOG_HANDLE log, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogCustInfo_0(P2LOG_HANDLE log, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogCustWarning_0(P2LOG_HANDLE log, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogCustError_0(P2LOG_HANDLE log, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogCustInfoEx_0(P2LOG_HANDLE log, PCSTR category, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogCustWarningEx_0(P2LOG_HANDLE log, PCSTR category, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogCustErrorEx_0(P2LOG_HANDLE log, PCSTR category, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogCustErrorWithCode_0(P2LOG_HANDLE log, P2ERR err, PCSTR fmt, ...);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogCustErrorWithCodeEx_0(P2LOG_HANDLE log, P2ERR err, PCSTR category, PCSTR fmt, ...);

#define P2TRACE(args) P2TRACE_GEN_(P2Log, args)
#define P2TRACE_INFO(args) P2TRACE_GEN_(P2LogInfo, args)
#define P2TRACE_WARNING(args) P2TRACE_GEN_(P2LogWarning, args)
#define P2TRACE_ERROR(args) P2TRACE_GEN_(P2LogError, args)

#define P2TRACE_EX(args) P2TRACE_GEN_(P2LogEx, args)
#define P2TRACE_INFO_EX(args) P2TRACE_GEN_(P2LogInfoEx, args)
#define P2TRACE_WARNING_EX(args) P2TRACE_GEN_(P2LogWarningEx, args)
#define P2TRACE_ERROR_EX(args) P2TRACE_GEN_(P2LogErrorEx, args)
#define P2TRACE_ERROR_WITH_CODE(args) P2TRACE_GEN_(P2LogErrorWithCode, args)
#define P2TRACE_ERROR_WITH_CODE_EX(args) P2TRACE_GEN_(P2LogErrorWithCodeEx, args)

#define P2TRACECUST_EX(args) P2TRACE_GEN_(P2LogCustEx, args)
#define P2TRACECUST(args) P2TRACE_GEN_(P2LogCust, args)
#define P2TRACECUST_INFO(args) P2TRACE_GEN_(P2LogCustInfo, args)
#define P2TRACECUST_WARNING(args) P2TRACE_GEN_(P2LogCustWarning, args)
#define P2TRACECUST_ERROR(args) P2TRACE_GEN_(P2LogCustError, args)
#define P2TRACECUST_INFO_EX(args) P2TRACE_GEN_(P2LogCustInfoEx, args)
#define P2TRACECUST_WARNING_EX(args) P2TRACE_GEN_(P2LogCustWarningEx, args)
#define P2TRACECUST_ERROR_EX(args) P2TRACE_GEN_(P2LogCustErrorEx, args)
#define P2TRACECUST_ERROR_WITH_CODE(args) P2TRACE_GEN_(P2LogCustErrorWithCode, args)
#define P2TRACECUST_ERROR_WITH_CODE_EX(args) P2TRACE_GEN_(P2LogCustErrorWithCodeEx, args)

#define P2TRACE_0(args) P2TRACE_GEN_0(P2Log, args)
#define P2TRACE_INFO_0(args) P2TRACE_GEN_0(P2LogInfo, args)
#define P2TRACE_WARNING_0(args) P2TRACE_GEN_0(P2LogWarning, args)
#define P2TRACE_ERROR_0(args) P2TRACE_GEN_0(P2LogError, args)

#define P2TRACE_EX_0(args) P2TRACE_GEN_0(P2LogEx, args)
#define P2TRACE_INFO_EX_0(args) P2TRACE_GEN_0(P2LogInfoEx, args)
#define P2TRACE_WARNING_EX_0(args) P2TRACE_GEN_0(P2LogWarningEx, args)
#define P2TRACE_ERROR_EX_0(args) P2TRACE_GEN_0(P2LogErrorEx, args)
#define P2TRACE_ERROR_WITH_CODE_0(args) P2TRACE_GEN_0(P2LogErrorWithCode, args)
#define P2TRACE_ERROR_WITH_CODE_EX_0(args) P2TRACE_GEN_0(P2LogErrorWithCodeEx, args)

#define P2TRACECUST_EX_0(args) P2TRACE_GEN_0(P2LogCustEx, args)
#define P2TRACECUST_0(args) P2TRACE_GEN_0(P2LogCust, args)
#define P2TRACECUST_INFO_0(args) P2TRACE_GEN_0(P2LogCustInfo, args)
#define P2TRACECUST_WARNING_0(args) P2TRACE_GEN_0(P2LogCustWarning, args)
#define P2TRACECUST_ERROR_0(args) P2TRACE_GEN_0(P2LogCustError, args)
#define P2TRACECUST_INFO_EX_0(args) P2TRACE_GEN_0(P2LogCustInfoEx, args)
#define P2TRACECUST_WARNING_EX_0(args) P2TRACE_GEN_0(P2LogCustWarningEx, args)
#define P2TRACECUST_ERROR_EX_0(args) P2TRACE_GEN_0(P2LogCustErrorEx, args)
#define P2TRACECUST_ERROR_WITH_CODE_0(args) P2TRACE_GEN_0(P2LogCustErrorWithCode, args)
#define P2TRACECUST_ERROR_WITH_CODE_EX_0(args) P2TRACE_GEN_0(P2LogCustErrorWithCodeEx, args)

//////////////////////////////////////////////////////////////////////

// Check current state of given trace (don't write to log!)
// Return:
//  pointer to 'trace state' var
//  NULL - state:unknown (on any error)
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogCustExv_(P2LOG_HANDLE log, UINT32 severity, PCSTR category, PCSTR fmt);
P2SYSLOG_API INT32 * P2CALLSPEC trcheckEx_P2LogCustExv_0(P2LOG_HANDLE log, UINT32 severity, PCSTR category, PCSTR fmt);

//////////////////////////////////////////////////////////////////////

// <- moved to project P2SysTypes
//P2SYSLOG_API PCSTR P2CALLSPEC P2ErrGetDescrStr(P2ERR err);

void P2CALLSPEC P2SysLogStartup();
void P2CALLSPEC P2SysLogCleanup();

P2SYSLOG_API int P2CALLSPEC trread_int_P2LogCust(P2LOG_HANDLE log, PCSTR section, PCSTR fmt);
P2SYSLOG_API P2ERR P2CALLSPEC trwrite_int_P2LogCust(P2LOG_HANDLE log, PCSTR section, PCSTR fmt, int value);

#ifdef  __cplusplus
}
#endif

#endif // _P2SYSLOG_H_
