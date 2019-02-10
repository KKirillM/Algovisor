#include "stdafx.h"
#include "CommandLineParser.h"
#include "ConnectionManager.h"


#define USING_STRING "\nUsing: DataCollector /INI:[path_to_ini_file]\n\n"

HANDLE g_hExitEvent = NULL;

// From MSDN: 
// If the function handles the control signal, it should return TRUE. If it returns FALSE, the next handler function in the list of handlers for this process is used.
BOOL WINAPI CtrlHandler(DWORD dwCtrlType)
{
	ConnectionManager* connMngr = ConnectionManagerHolder::GetInstance();

	switch (dwCtrlType)
	{
	case CTRL_C_EVENT:
		printf("Ctrl-C event\n\n");
		P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, "DataCollector", "Ctrl-C event received"))
		connMngr->Disconnect();
		connMngr->DeInit();
		SetEvent(g_hExitEvent);
		Sleep(1000);
		return (TRUE);

	case CTRL_CLOSE_EVENT:
		printf("Close event\n\n");
		P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, "DataCollector", "Close event received"))
		connMngr->Disconnect();
		connMngr->DeInit();
		SetEvent(g_hExitEvent);
		Sleep(1000);
		return (TRUE);

	case CTRL_BREAK_EVENT:
		printf("Ctrl-Break event\n\n");
		P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, "DataCollector", "Ctrl-Break event received"))
		return (FALSE);

	case CTRL_LOGOFF_EVENT: 
		printf("Ctrl-Logoff event\n\n");
		P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, "DataCollector", "Ctrl-Logoff event received"))
		return (FALSE);

	case CTRL_SHUTDOWN_EVENT: 
		printf("Ctrl-Shutdown event\n\n");
		P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, "DataCollector", "Ctrl-Shutdown event received"))
		return(FALSE); 

	default:
		return (FALSE);
	}
}

bool CheckCommandLineArg(int argc, _TCHAR* argv[], char* usingString)
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

bool InitLogs()
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
        {
			throw fo2base::CP2Error("Ini-file is not opened, please open Ini-file first");
        }
	}
	catch (fo2base::CP2Error& err)
	{
		printf("%s; errorCode=%d\n", err.GetMessageA().c_str(), err.GetCode());
		return false;
	}

	return true;
}

bool TryToOpenIniFile()
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
		return false;
	}

	return true;
}

void PrintPlaza2Version()
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

int _tmain(int argc, _TCHAR* argv[])
{	
	if (!CheckCommandLineArg(argc, argv, USING_STRING))
    {
		return 1;
    }

	PrintPlaza2Version();
	
	if (!TryToOpenIniFile())
	{
		printf("INI-file open failed");
		return 1;
	}
	
	printf("INI-file opened\n");

	if (!InitLogs())
	{
		printf("Log files open failed");
		return 1;
	}

	printf("Log files opened\n");
	P2TRACECUST_INFO((fo2base::gGetLog()->GetHandle(), "********** Log to file started **********"))

	P2LogCurrentProcessInfo(); // информация о текущем процессе пишется в системный лог

	g_hExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	ConnectionManager* connMngr = ConnectionManagerHolder::GetInstance();
    
    SetConsoleCtrlHandler(CtrlHandler, TRUE);

    //setlocale(LC_ALL,"Russian");

	try
	{
        connMngr->InitFlags();
        connMngr->Init();
        connMngr->ConnectToDataProviders();
	}
	catch (fo2base::CP2Error& p2err)
	{
		P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_ERROR, "CP2Error", "Unhalted exception: %s, P2ERR: %d", p2err.GetMessageA(), p2err.GetCode()))
		SetEvent(g_hExitEvent);
	}
	catch (std::exception& err)
	{
		P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_ERROR, "std::exception", "Unhalted exception: %s", err.what()))
		SetEvent(g_hExitEvent);
	}
		
	WaitForSingleObject(g_hExitEvent, INFINITE);
	CloseHandle(g_hExitEvent);

	P2TRACECUST_INFO((fo2base::gGetLog()->GetHandle(), "********** Log to file stoped **********"))

    // открытые файлы инишников и файлы логов закрывать не нужно, они закроются сами в своих деструкторах

	SetConsoleCtrlHandler(CtrlHandler, FALSE);

	return 0;
}

	/*MSG msg;
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	while (GetMessage(&msg, NULL, 0, 0))
	{
	TranslateMessage(&msg);
	DispatchMessage(&msg);

	if( msg.message == WM_QUIT )
	break;
	}*/

