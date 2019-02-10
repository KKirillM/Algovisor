#include "stdafx.h"
#include "Plaza2_defines.h"
#include "Plaza2_Connector.h"
#include "HelpTools.h"
#include "IniFileWorker.h"
#include "Plaza2_Tables.h"
#include "GetValuesHelper.h"

#include "IPlaza2_Connector.h"

volatile bool stopFlag = false;
volatile int global_int;
string global_string;
FORTS_FUTINFO_REPL::fut_sess_contents contents;
std::map<std::string, FORTS_FUTINFO_REPL::fut_sess_contents> allInstrumentContents;

void WINAPI GetPlaza2InitParametersFromINI(const std::string& pathToINI, Plaza2InitParameters& params)
{
    if (pathToINI.empty())
    {
        std::string errorString = "Path to INI-file is not set";
        log_error(errorString.c_str());
        assert(FALSE);
        throw std::exception(errorString.c_str());
    }

    IniFileWorker ini;

    try
    {
        ini.Open(pathToINI);

        params.connectionString = ini.ReadStr("Plaza2", "connectionString");
        if (params.connectionString.empty())
            throw IniFileException("Plaza2 connection string parameter is missing in INI-file", IniFileException::BAD_FILE_FORMAT);

        params.environmentString = ini.ReadStr("Plaza2", "environmentString");
        if (params.environmentString.empty())
            throw IniFileException("Plaza2 environment string parameter is missing in INI-file", IniFileException::BAD_FILE_FORMAT);

        params.replstateFolderPath = ini.ReadStr("Plaza2", "replstateFolderPath");
        if (params.replstateFolderPath.empty())
            throw IniFileException("Plaza2 path to replstate DB parameter is missing in INI-file", IniFileException::BAD_FILE_FORMAT);
    }
    catch (IniFileException& e)
    {
        char errMsg[512];
        sprintf_s(errMsg, sizeof(errMsg), "%s, error: %d", e.what(), e.GetExceptionType());
        log_error(errMsg);
        throw std::exception(errMsg);
    }

    ini.Close();
}

DLL_FUNCTION void APIENTRY GetP2ReplListParametersFromINI(const std::string& pathToINI, P2ReplList& params)
{
    if (pathToINI.empty())
    {
        assert(FALSE);
        std::string errorString = "INI-file is not transferred to the Plaza2Connector object";
        log_error(errorString.c_str());
        throw std::exception(errorString.c_str());
    }

    IniFileWorker ini;

    try
    {
        ini.Open(pathToINI);

        std::map<std::string, std::string> listOfKeys;
        ini.EnumKeys(listOfKeys, "Plaza2:p2repl");
        if (!listOfKeys.size())
        {
            throw std::exception("Section \"Plaza2:p2repl\" in INI-file is empty");
        }

        for (auto it = listOfKeys.begin(), end = listOfKeys.end(); it != end; ++it)
        {
            size_t tablesPlace = it->second.find("tables=");
            bool tablesExist = tablesPlace != std::string::npos;
            if (tablesExist)
            {
                tablesPlace += strlen("tables=");
                size_t separatorPos = it->second.find(';', tablesPlace);
                if (separatorPos == std::string::npos)
                    throw IniFileException("Separator in key value is missing in INI-file", IniFileException::BAD_FILE_FORMAT); 
                params[it->first].tables = std::string(it->second, tablesPlace, separatorPos-tablesPlace);
            }

            size_t modePlace = it->second.find("mode=");
            bool modeExist = modePlace != std::string::npos;
            if (modeExist)
            {
                modePlace += strlen("mode=");
                size_t separatorPos = it->second.find(';', modePlace);
                if (separatorPos == std::string::npos)
                    throw IniFileException("Separator in key value is missing in INI-file", IniFileException::BAD_FILE_FORMAT); 
                params[it->first].mode = std::string(it->second, modePlace, separatorPos-modePlace);
            }
            else
            {
                throw IniFileException("mode value for p2repl stream is missing in INI-file", IniFileException::BAD_FILE_FORMAT);
            }

            params[it->first].prefix = P2REPL;
        }
    }
    catch (IniFileException& e)
    {
        char errMsg[512];
        sprintf_s(errMsg, sizeof(errMsg), "%s, error: %d", e.what(), e.GetExceptionType());
        log_error(errMsg);
        throw std::exception(errMsg);
    }

    ini.Close();
}

IPlaza2ConnectorType1* WINAPI GetPlaza2ConnectorType1()
{
	return (new Plaza2ConnectorType1());
}

IPlaza2ConnectorType2* WINAPI GetPlaza2ConnectorType2()
{
    return (new Plaza2ConnectorType2());
}

int APIENTRY StartPlaza2RouterProcess(const std::string& pathToRouter, const std::string& pathToINI, bool needConsoleWindow /*= false*/)
{
    if (pathToRouter.empty())
    {
        return 2;
    }

    if (pathToINI.empty())
    {
        return 2;
    }

    std::string routerProcessName;
    routerProcessName.assign(pathToRouter, pathToRouter.rfind('\\')+1, std::string::npos);

    if (HelpTools::IsProcessNameInMemory(routerProcessName))
    {
        StopPlaza2RouterProcess(routerProcessName);
        HANDLE hProcess = HelpTools::GetProcessHandle(routerProcessName);
        WaitForSingleObject(hProcess, INFINITE);
        CloseHandle(hProcess);
    }

    STARTUPINFOA lpStartupInfo;
    PROCESS_INFORMATION lpProcessInfo;

    memset(&lpStartupInfo, 0, sizeof(lpStartupInfo));
    memset(&lpProcessInfo, 0, sizeof(lpProcessInfo));

    lpStartupInfo.cb = sizeof(lpStartupInfo);
    /*if (!needConsoleWindow)
    {
    lpStartupInfo.dwFlags = STARTF_USESHOWWINDOW;
    lpStartupInfo.wShowWindow = SW_HIDE;
    }
    else
    {
    lpStartupInfo.dwFlags = STARTF_USESHOWWINDOW;
    lpStartupInfo.wShowWindow = SW_SHOW;
    }*/

    DWORD newConsole = NULL;
    if (needConsoleWindow)
    {
        newConsole = CREATE_NEW_CONSOLE;
    }
    else
    {
        newConsole = CREATE_NO_WINDOW;
    }

    char cmdLine[256];
    sprintf_s(cmdLine, sizeof(cmdLine), "%s /ini:\"%s\"", pathToRouter.c_str(), pathToINI.c_str());
    if (!CreateProcessA(NULL, cmdLine, NULL, NULL, FALSE, newConsole, NULL, NULL, &lpStartupInfo, &lpProcessInfo))
    {
        return 1;
    }

    /*DWORD err = GetLastError();*/

    CloseHandle(lpProcessInfo.hThread);
    CloseHandle(lpProcessInfo.hProcess);

    Sleep(1000);

    if (!HelpTools::IsProcessNameInMemory(routerProcessName))
    {
        return 1;
    }

    return 0;
}

int WINAPI StopPlaza2RouterProcess(const std::string& routerProcessName)
{
    if (routerProcessName.empty())
    {
        return 2;
    }

    HANDLE plaza2RouterHandle = HelpTools::GetProcessHandle(routerProcessName);

    if (plaza2RouterHandle)
    {
        TerminateProcess(plaza2RouterHandle, 0);
        Sleep(1000);
    }

    if (!HelpTools::IsProcessNameInMemory(routerProcessName))
    {
        return 1;
    }

    return 0;
}

void WINAPI SetPlaza2RouterLoginPass(const std::string& pathToINI, const std::string& login, const std::string& password)
{
    IniFileWorker iniFile;
    iniFile.Open(pathToINI);
    iniFile.WriteStr("[AS:NS]", "USERNAME", login);
    iniFile.WriteStr("[AS:NS]", "PASSWORD", password);
}

int WINAPI GetIsinIdFromIsin(IPlaza2ConnectorType2* connector, const std::string& isin)
{
    global_int = 0;

    if (allInstrumentContents.empty() || allInstrumentContents.find(isin) == allInstrumentContents.end())
    {
        global_string = isin;
        stopFlag = false;
        P2ListnerParameters params;
        params.prefix = P2REPL;
        params.name = "FORTS_FUTINFO_REPL";
        params.mode = "snapshot"; // или snapshot+online и при переходе в онлайн стопить поток?
        params.tables = "fut_sess_contents";
        params.func1 = GetValuesHelper::SetStopFlagCallback;
        params.func2 = GetValuesHelper::GetIsinIdCallback;
        params.isin = INT_MAX;

        connector->OpenP2ReplListener(params);
        while (!stopFlag)
        {
            Sleep(1);
        }
        stopFlag = false;
        connector->CloseP2ReplListener(INT_MAX);
    }
    else
    {
        contents = (allInstrumentContents.find(isin))->second;
        global_int = contents.isin_id;
    }

    return global_int;
}

const char* WINAPI GettIsinFromIsinId(IPlaza2ConnectorType2* connector, int isin_id)
{
    global_string.clear();

    if (allInstrumentContents.empty())
    {
        global_int = isin_id;
        stopFlag = false;
        P2ListnerParameters params;
        params.prefix = P2REPL;
        params.name = "FORTS_FUTINFO_REPL";
        params.mode = "snapshot"; // или snapshot+online и при переходе в онлайн стопить поток?
        params.tables = "fut_sess_contents";
        params.func1 = GetValuesHelper::SetStopFlagCallback;
        params.func2 = GetValuesHelper::GetTickerCallback;
        params.isin = INT_MAX;

        connector->OpenP2ReplListener(params);
        while (!stopFlag)
        {
            Sleep(1);
        }
        stopFlag = false;
        connector->CloseP2ReplListener(INT_MAX);
    }
    else
    {
        for (auto it = allInstrumentContents.begin(); it != allInstrumentContents.end(); ++it)
        {
            contents = it->second;
            if (contents.isin_id == isin_id)
            {
                global_string.assign(contents.isin);
                break;
            }
        }
    }

    return global_string.length() ? global_string.c_str() : NULL;
}

void* WINAPI GetInstrumentContents(IPlaza2ConnectorType2* connector, const std::string& isin)
{
    if (allInstrumentContents.empty() || allInstrumentContents.find(isin) == allInstrumentContents.end())
    {
        global_string = isin;
        stopFlag = false;
        P2ListnerParameters params;
        params.prefix = P2REPL;
        params.name = "FORTS_FUTINFO_REPL";
        params.mode = "snapshot"; // или snapshot+online и при переходе в онлайн стопить поток?
        params.tables = "fut_sess_contents";
        params.func1 = GetValuesHelper::SetStopFlagCallback;
        params.func2 = GetValuesHelper::GetInstrumentCallback;
        params.isin = INT_MAX;

        connector->OpenP2ReplListener(params);
        while (!stopFlag)
        {
            Sleep(1);
        }
        stopFlag = false;
        connector->CloseP2ReplListener(INT_MAX);	
    }
    else
    {
        contents = allInstrumentContents.find(isin)->second;
    }

	void* data = NULL;
	data = GlobalAlloc(GPTR, sizeof(FORTS_FUTINFO_REPL::fut_sess_contents));
	memcpy(data, &contents, sizeof(FORTS_FUTINFO_REPL::fut_sess_contents));

    return data;
}

void WINAPI GetAllInstrumentsContents(IPlaza2ConnectorType2* connector, void** data, int& count)
{
    if (allInstrumentContents.empty())
    {
        stopFlag = false;
        P2ListnerParameters params;
        params.prefix = P2REPL;
        params.name = "FORTS_FUTINFO_REPL";
        params.mode = "snapshot"; // или snapshot+online и при переходе в онлайн стопить поток?
        params.tables = "fut_sess_contents";
        params.func1 = GetValuesHelper::SetStopFlagCallback;
        params.func2 = GetValuesHelper::GetAllContentsCallback;
        params.isin = INT_MAX;

        connector->OpenP2ReplListener(params);
        while (!stopFlag)
        {
            Sleep(1);
        }
        stopFlag = false;
        connector->CloseP2ReplListener(INT_MAX);
    }

	count = (int)allInstrumentContents.size();
	if (*data == NULL)
	{
		*data = GlobalAlloc(GPTR, count * sizeof(FORTS_FUTINFO_REPL::fut_sess_contents));
	}

	FORTS_FUTINFO_REPL::fut_sess_contents* result = *(FORTS_FUTINFO_REPL::fut_sess_contents**)data;

	int offset = 0;
    for (auto it = allInstrumentContents.begin(), end = allInstrumentContents.end(); it != end; ++it)
    {
		memcpy(&result[offset], &it->second, sizeof(FORTS_FUTINFO_REPL::fut_sess_contents));
		offset++;
    }
}