#include "stdafx.h"
#include <stuff.h>
#include <P2Console.hpp>
#include <P2SysTypes.hpp>
#include <P2SysLog.h>
#include "./App.h"

bool ParseCommandLine(int argc, char* argv[], /*OUT*/ TInputParams& parsedParams);
P2ERR IniOpenOrCreate();
void printUsage();

#define MODULE_NAME "edbexport"
#define INI_FOLDER ".\\ini\\"
#define DEF_INI_FILE_NAME INI_FOLDER MODULE_NAME ".ini"
#define LOG_FOLDER ".\\log\\"
#define DEF_LOG_FILE_NAME LOG_FOLDER MODULE_NAME ".log"

CApp* g_App = NULL;
////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
BOOL WINAPI CtrlHandlerRoutine(DWORD dwCtrlType)
{
    if (CTRL_LOGOFF_EVENT != dwCtrlType) 
    {
        P2TRACE_INFO_EX((LOG_CAT_, "Request for stop received (%u)", dwCtrlType))
        g_App->Abort();
    }
    return TRUE;
}
#elif  __unix__
void CtrlHandlerRoutine(int signum)
{ 
    const char* str = "\nRequest for stop received\n"; 
    write(STDERR_FILENO, str, strlen(str));
    g_App->Abort();
}
#endif //__unix__

int main(int argc, char* argv[])
{
    P2ERR res;

    TInputParams ip;
    if (!ParseCommandLine(argc, argv, ip))
    {
        printUsage();
        return -1;
    }

    if (P2SUCCEEDEDV(IniOpenOrCreate(), res))
    {
        if (P2SUCCEEDEDV(P2LogOpen(NULL), res))
        {
            P2TRACE_INFO(("********** Log started **********"))
            P2LogCurrentProcessInfo();

            std::auto_ptr< CApp > pApp;
            try
            {
                pApp.reset(new CApp);
            }
            catch (std::bad_alloc&)
            {
                P2SysPrintf("Couldn't run %R", P2ERR_COMMON_NO_MEMORY);
                return P2ERR_COMMON_NO_MEMORY;
            }

            g_App = pApp.get();
            P2SetConsoleCtrlHandler(CtrlHandlerRoutine);
            if (P2FAILEDV(pApp->Init(ip), res))
            {
                if (P2ERR_NOT_OK == res) // ошибка в параметрах командной строки
                {
                    printUsage();
                    res = static_cast< P2ERR >(-1);
                }
            }
            if (P2SUCCEEDED(res))
            {
                P2TRACE_INFO_EX(( LOG_CAT_, "App finished with error code %R", pApp->Run()))
            }
            P2ResetConsoleCtrlHandler(CtrlHandlerRoutine);
            pApp.reset();
            P2TRACE_INFO(("********** Log stopped **********"))
            P2LogClose();
        }
        else
        {
            P2SysPrintf("Couldn't open log: %R", res);
        }
        P2IniClose();
    }
    else
    {
        P2SysPrintf("Couldn't open or create settings file '%s': %R", DEF_INI_FILE_NAME, res);
    }
    return res;
}

bool ParseCommandLine(int argc, char* argv[], /*OUT*/ TInputParams& parsedParams)
{
    // Example: -o BASENAME --replace-non-printing --split 5000 --range 1000:2000;3000: file.edb schema.ini SchemaName
    enum ERequiredParam
    {
        rpNone          = 0,
        rpInputFile     = 1,
        rpSchemaIni     = 2,
        rpSchemaName    = 3
    } requiredParam = rpNone;
    enum EExpectedParam
    {
        epNone,
        epRanges,
        epSplit,
        epOutputFile,
        epOutAsDump,
        epFastMode,
        epIgnoreUnknown
    } expectedParam = epNone;
    for (int i = 1; i < argc; i++)
    {
        const char * pp = argv[i];
        if (rpNone == requiredParam) // ищем опциональные параметры
        {
            if (pp[0] == '-')
            {
                if (epNone != expectedParam)
                {
                    return false;
                }
                if (0 == P2StrIgnoreCaseCompare("-s", pp) || 0 == P2StrIgnoreCaseCompare("--split", pp))
                {
                    expectedParam = epSplit; continue;         
                }
                else if (0 == P2StrIgnoreCaseCompare("-r", pp) || 0 == P2StrIgnoreCaseCompare("--range", pp))
                {
                    expectedParam = epRanges; continue;
                }
                else if (0 == P2StrIgnoreCaseCompare("-o", pp))
                {
                    expectedParam = epOutputFile; continue;
                }
                else if (0 == P2StrIgnoreCaseCompare("-n", pp) || 0 == P2StrIgnoreCaseCompare("--replace-non-printing", pp))
                {
                    parsedParams.ReplaceNonPrintingChars = true; continue;
                }
                else if (0 == P2StrIgnoreCaseCompare("-d", pp) || 0 == P2StrIgnoreCaseCompare("--dump", pp))
                {
                    parsedParams.AsDump = true; continue;
                }
                else if (0 == P2StrIgnoreCaseCompare("-f", pp) || 0 == P2StrIgnoreCaseCompare("--fast", pp))
                {
                    parsedParams.FastMode = true; continue;
                }
                else if (0 == P2StrIgnoreCaseCompare("-u", pp) || 0 == P2StrIgnoreCaseCompare("--include-unknown", pp))
                {
                    parsedParams.IgnoreUnknownEvents = true; continue;
                }
				else if (0 == P2StrIgnoreCaseCompare("-t", pp) || 0 == P2StrIgnoreCaseCompare("--time-header", pp))
				{
					parsedParams.TimeHeader = true; continue;
				}
            }
        }
        if (epNone != expectedParam)
        {
            switch (expectedParam)
            {
                case epSplit: parsedParams.SplitBy.assign(pp); break;
                case epRanges: parsedParams.Ranges.assign(pp); break;
                case epOutputFile: parsedParams.OutputFile.assign(pp); break;
            }
            expectedParam = epNone;
            continue;
        }
        else
        {
            requiredParam = (ERequiredParam)(((int)requiredParam) + 1);
            switch (requiredParam)
            {
                case rpInputFile: parsedParams.InputFile.assign(pp); continue;
                case rpSchemaIni: parsedParams.SchemaIni.assign(pp); continue;
                case rpSchemaName: parsedParams.SchemaName.assign(pp); return true;
            }
        }
    }
    return true;
}

void printUsage()
{
    // show usage
    P2SysPrintf("%s",
        "\n"
        "Usage: " MODULE_NAME ".exe [OPTIONS] EDB_FILE SCHEMA_FILE SCHEMA_NAME\n"
        "  Exports edb file (or its parts) to csv or human-readable format.\n"
        "\n"
        "Options:\n"
        "  -o BASENAME\t\tOutput file name (without extension).\n"
        "  -r, --range RANGES\tRevision ranges to export, where RANGES is:\n"
        "\t\t\t(start1:)|(start1:end1)|(:end1)][;(start2:end2)[;...]]\n"
        "  -s, --split COUNT\tLimit every output file to COUNT events.\n"
        "  -n, --replace-non-printing\tReplace non-printing chars with spaces.\n"
        "  -d, --dump\t\tOutput in human-readable dump format.\n"
        "  -f, --fast\tFast positioning to range(s) (using EnumRecs).\n"
        "  -u, --include-unknown\tNo error on event types without scheme.\n"
        "\n"
        "Examples:\n"
        "\t" MODULE_NAME " input.edb schema.ini MySchema\n"
        "\t" MODULE_NAME " -r 500:1000;5000: -o exported input.edb schema.ini MySchema\n"
        "\t" MODULE_NAME " -r :50000 --split 10000 input.edb schema.ini MySchema\n"
        "\t" MODULE_NAME " input.edb -d schema.ini MySchema\n"
        "\n"
        "See http://jira.moex.com:8090/pages/viewpage.action?pageId=20054396 for details.\n"
   );
}

P2ERR IniOpenOrCreate()
{
    P2ERR res;
    if (P2FAILEDV(P2IniOpen(DEF_INI_FILE_NAME), res))
    {
        RETURN_P2ERR(P2IniSetOpt(P2INI_OPT_CREATE_IF_ABSENT));
        if (P2SUCCEEDEDV(P2IniOpen(DEF_INI_FILE_NAME), res))
        {
            const char * logSection = 
                "logfile="DEF_LOG_FILE_NAME"\n"
                "logfileperday=0\n"
                "logtoconsole=1\n";
            RETURN_P2ERR(P2IniWriteSection("p2syslog", logSection));
        }
    }
    return res;
}
