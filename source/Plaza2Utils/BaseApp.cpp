#include "stdafx.h"
#include "BaseApp.h"
#include "CommandLineParser.h"
#include <timeapi.h>

static BaseApp* g_base = NULL;

BaseApp::BaseApp() : hExitEvent(NULL)
{
    g_base = this;
}

BaseApp::~BaseApp()
{

}

bool BaseApp::CheckCommandLineArg(int argc, _TCHAR* argv[], char* usingString)
{
    if (argc < 2 )
    {
        printf("INI-file is absent\n");
        printf(usingString);
        system("pause");
        return false;
    }
    else if (strchr(argv[1], '?') != 0)
    {
        printf(usingString);
        return false;
    }

    return true;
}

bool BaseApp::OpenLogs()
{
    try
    {
        fo2base::CP2Ini* iniFile = fo2base::gGetCP2Ini();

        if (iniFile->IsOpen())
        {
            fo2base::CP2Log* logFile = fo2base::gGetLog();
            logFile->Open(*iniFile, "applog");

            fo2base::CP2DefaultLog* defaultLogFile = fo2base::gGetDefaultLog();
            defaultLogFile->Open(*iniFile, "p2syslog");
        }
        else
            throw fo2base::CP2Error("Ini-file is not opened, please open Ini-file first");
    }
    catch (fo2base::CP2Error& err)
    {
        printf("%s; errorCode=%d\n", err.GetMessageA().c_str(), err.GetCode());
        return false;
    }

    return true;
}

bool BaseApp::OpenIni()
{
    std::string iniFileName;
    utils::CCommandLineParser::CLGetArgValue("INI", ':', &iniFileName);

    try
    {
        fo2base::CP2DefaultIni* defaultIniFile = fo2base::gGetDefaultIni();
        defaultIniFile->Open(iniFileName);
        fo2base::CP2Ini* iniFile = fo2base::gGetCP2Ini();
        iniFile->Open(iniFileName);
    }
    catch (fo2base::CP2Error& err)
    {
        printf(err.GetMessageA().c_str());
		printf("\n");
        return false;
    }

    return true;
}

bool BaseApp::Init(int argc, _TCHAR* argv[], char* usingString)
{
    if (!CheckCommandLineArg(argc, argv, usingString))
        return false;

    PrintPlaza2Version();

    if (!OpenIni())
    {
        return false;
    }

    printf("INI-file opened\n");

    if (!OpenLogs())
    {
        return false;
    }

    printf("Log files opened\n");
    P2TRACECUST_INFO((fo2base::gGetLog()->GetHandle(), "********** Log to file started **********"))
    P2LogCurrentProcessInfo();

    ReadConfigParameters();

    TIMECAPS tc;
    MMRESULT mmErr;
    if (MMSYSERR_NOERROR == (mmErr = timeGetDevCaps(&tc, sizeof(tc))) &&
        MMSYSERR_NOERROR == (mmErr = timeBeginPeriod(tc.wPeriodMin)))
    {
        FO2TRACE_INFO(("Min timer resolution set to %u", tc.wPeriodMin));
    }
    else
    {
        FO2TRACE_INFO(("Can't set min timer resolution; mmerr %u", mmErr));
    }

    hExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    SetConsoleCtrlHandler(CtrlHandler, TRUE);

    AdditionalInitializations();

    return true;
}

int BaseApp::Run()
{
    WaitForSingleObject(hExitEvent, INFINITE);
    CloseHandle(hExitEvent);

    P2TRACECUST_INFO((fo2base::gGetLog()->GetHandle(), "********** Log to file stoped **********"))

    // открытые файлы инишников и файлы логов закрывать не нужно, они закроются сами в своих деструкторах

    SetConsoleCtrlHandler(CtrlHandler, FALSE);

    return 0;
}

void BaseApp::PrintPlaza2Version()
{
    P2ERR rc;
    const char* pVer = NULL;

    if(P2FAILEDV(P2GetVersion(&pVer), rc))
    {
        printf("Unknown version of Plaza2");
    }
    else
    {
        printf("Plaza2 version: %s\n", pVer);
    }
}

// From MSDN: 
// If the function handles the control signal, it should return TRUE. If it returns FALSE, the next handler function in the list of handlers for this process is used.
BOOL WINAPI BaseApp::CtrlHandler(DWORD dwCtrlType)
{
    switch (dwCtrlType)
    {
    case CTRL_C_EVENT:
        printf("Ctrl-C event\n\n");
        P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, "", "Ctrl-C event received"))
        g_base->SetEvent();
        break;

    case CTRL_CLOSE_EVENT:
        printf("Close event\n\n");
        P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, "DataCollector", "Close event received"))
        g_base->SetEvent();
        break;

    case CTRL_BREAK_EVENT:
        printf("Ctrl-Break event\n\n");
        P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, "DataCollector", "Ctrl-Break event received"))
        break;

    case CTRL_LOGOFF_EVENT: 
        printf("Ctrl-Logoff event\n\n");
        P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, "DataCollector", "Ctrl-Logoff event received"))
        break;

    case CTRL_SHUTDOWN_EVENT: 
        printf("Ctrl-Shutdown event\n\n");
        P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, "DataCollector", "Ctrl-Shutdown event received"))
        break;

    default:
        return (FALSE);
    }

    return g_base->CtrlHandlerCallback(dwCtrlType);
}
