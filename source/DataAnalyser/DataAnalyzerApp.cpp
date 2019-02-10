#include "stdafx.h"
#include "DataAnalyzerApp.h"
#include "..\Plaza2Utils\EDBStorageReader.h"
#include "CommandLineParser.h"

#define MODULE_NAME "DataAnalyzerApp"
#define OPTION_SECTION "options"
#define STRATEGIES_SECTION "Strategies"


static DataAnalyzerApp* gPtr = NULL;


DataAnalyzerApp::DataAnalyzerApp() : stopFlag(false), callbacks(stopFlag)
{
    gPtr = this;
}

DataAnalyzerApp::~DataAnalyzerApp()
{
}

void DataAnalyzerApp::ReadConfigParameters()
{
    fo2base::CP2DefaultIni* defaultIniFile = fo2base::gGetDefaultIni();

    pathToDB = fo2base::gGetCP2Ini()->ReadStr(OPTION_SECTION, "pathToDB");
    if (pathToDB.empty())
    {
        P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_ERROR, MODULE_NAME, "Path to EDB database is emtpy"))
        throw std::exception("Path to EDB database is emtpy");
    }

    std::string startDateString = fo2base::gGetCP2Ini()->ReadStr(OPTION_SECTION, "startDate");
    if (startDateString.empty())
    {
        startDateString = "01-01-1900";
    }
    std::string endDateString = fo2base::gGetCP2Ini()->ReadStr(OPTION_SECTION, "endDate");
    if (endDateString.empty())
    {
        endDateString = "01-01-2100";
    }
    std::string startTimeString = fo2base::gGetCP2Ini()->ReadStr(OPTION_SECTION, "startTime");
    if (startTimeString.empty())
    {
        startTimeString = "00:00:00";
    }
    std::string endTimeString = fo2base::gGetCP2Ini()->ReadStr(OPTION_SECTION, "endTime");
    if (endTimeString.empty())
    {
        endTimeString = "23:59:59";
    }

    P2TIME timeParam;

    timeParam.day = stoi(startDateString.substr(0,2));
    timeParam.month = stoi(startDateString.substr(3,2));
    timeParam.year = stoi(startDateString.substr(6,4));
    timeParam.hour = stoi(startTimeString.substr(0,2));
    timeParam.minute = stoi(startTimeString.substr(3,2));
    timeParam.second = stoi(startTimeString.substr(6,2));
    timeParam.msec = 0;
    timeParam.nanoseconds = 0;

    startDate = timeParam;

    timeParam.day = stoi(endDateString.substr(0,2));
    timeParam.month = stoi(endDateString.substr(3,2));
    timeParam.year = stoi(endDateString.substr(6,4));
    timeParam.hour = stoi(endTimeString.substr(0,2));
    timeParam.minute = stoi(endTimeString.substr(3,2));
    timeParam.second = stoi(endTimeString.substr(6,2));
    timeParam.msec = 999;
    timeParam.nanoseconds = 999999;

    endDate = timeParam;

    if (startDate.Compare(endDate, fo2base::CP2Time::dtpDay) > 0)
    {
        P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_ERROR, MODULE_NAME, "'startDate' param is bigger than 'endDate' param"))
        throw std::exception("'startDate' param is bigger than 'endDate' param");
    }

    OptionsParams optionsParams;

    optionsParams.checkTimeInterval = fo2base::gGetCP2Ini()->ReadInt(OPTION_SECTION, "checkTimeInterval", 0) == 1 ? true : false;
    if (optionsParams.checkTimeInterval)
    {
        optionsParams.startEventTime = startDate;
        optionsParams.endEventTime = endDate;
    }

    optionsParams.pathToLogs = fo2base::gGetCP2Ini()->ReadStr(OPTION_SECTION, "pathToLogs");
    if (optionsParams.pathToLogs.empty())
    {
        P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_ERROR, MODULE_NAME, "Path to log files directory is emtpy"))
    }

    optionsParams.fileNamePrefix = fo2base::gGetCP2Ini()->ReadStr(OPTION_SECTION, "fileNamePrefix");

    std::string isinIdString = fo2base::gGetCP2Ini()->ReadStr(OPTION_SECTION, "isin_id");
    std::vector<std::string> values;
    utils::CSimpleParserA::parse(isinIdString, ',', values);
    for (std::string value : values)
    {
        optionsParams.isinIDList.insert(atoi(value.c_str()));
    }

    optionsParams.needLog_ORDLOG = fo2base::gGetCP2Ini()->ReadInt(OPTION_SECTION, "FORTS_ORDLOG_REPL", 0) == 1 ? true : false;
    optionsParams.needLog_DEALS = fo2base::gGetCP2Ini()->ReadInt(OPTION_SECTION, "FORTS_DEALS_REPL", 0) == 1 ? true : false;
    optionsParams.needLog_orderBook = fo2base::gGetCP2Ini()->ReadInt(OPTION_SECTION, "orderBook", 0) == 1 ? true : false;
    if (optionsParams.needLog_orderBook)
    {
		optionsParams.orderBookLevels = fo2base::gGetCP2Ini()->ReadInt(OPTION_SECTION, "orderBookLevels", INT_MAX);
    }
    optionsParams.needLog_aggrTrades = fo2base::gGetCP2Ini()->ReadInt(OPTION_SECTION, "aggrTrades", 0) == 1 ? true : false;
    optionsParams.needLog_splitTrades = fo2base::gGetCP2Ini()->ReadInt(OPTION_SECTION, "splitTrades", 0) == 1 ? true : false;
    optionsParams.needLog_edbContent = fo2base::gGetCP2Ini()->ReadInt(OPTION_SECTION, "edbContentLog", 0) == 1 ? true : false;
    optionsParams.rtt = fo2base::gGetCP2Ini()->ReadInt(OPTION_SECTION, "rtt", 0) == 1 ? true : false;
    optionsParams.tradesStatistics = fo2base::gGetCP2Ini()->ReadInt(OPTION_SECTION, "tradesStatistics", 0) == 1 ? true : false;
	optionsParams.dateFormat = fo2base::gGetCP2Ini()->ReadStr(OPTION_SECTION, "dateFormat");
	if (optionsParams.dateFormat.empty())
	{
		optionsParams.dateFormat = "HH:MM:SS.nnnnnnnnn";
	}
    optionsParams.sessionStatistics = fo2base::gGetCP2Ini()->ReadInt(OPTION_SECTION, "sessionStatistics", 0) == 1 ? true : false;
    optionsParams.needBacktest = fo2base::gGetCP2Ini()->ReadInt(OPTION_SECTION, "backtest", 0) == 1 ? true : false;
    if (optionsParams.needBacktest)
    {
        optionsParams.roundTripDelay = fo2base::gGetCP2Ini()->ReadInt(OPTION_SECTION, "roundTripDelay", 0);
    }

    //STRATEGIES_SECTION

	// for debug purposes
	//DateFormatParser parser(optionsParams.dateFormat, fo2base::NowUnixNanosecondsTime());
	//std::string time_string(/*(const char*)*/parser);

    callbacks.InitParams(optionsParams);
}

void DataAnalyzerApp::AdditionalInitializations()
{
    
}

std::string GetFolderNameFromCP2Time(const fo2base::CP2Time date)
{
    std::string folderName = date.ToString("%x");
    std::replace(folderName.begin(), folderName.end(), '.', '-');
    return folderName;
}

int DataAnalyzerApp::Run()
{
    if (!pathToDB.empty())
    {
		fo2base::CP2Time currentDate = startDate;
        std::string folderName = GetFolderNameFromCP2Time(currentDate);
        std::string fullPath = pathToDB + '\\' + folderName + "\\Plaza2_AllData.edb";

        callbacks.StartSessionRange();
        while(currentDate.Compare(endDate, fo2base::CP2Time::dtpDay) <= 0)
        {
            if (edbReader.Open(fullPath))
            {
                //P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_ERROR, MODULE_NAME, "Can't open EDB-file: %s", fullPath.c_str()))
                currentDate.AddDay(1);
                folderName = GetFolderNameFromCP2Time(currentDate);
                fullPath = pathToDB + '\\' + folderName + "\\Plaza2_AllData.edb";
                continue;
            }

            EDBStorabeCallback func = Plaza2Callbacks::MainCallback;
            P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, MODULE_NAME, "Start reading EDB-file: %s ...", fullPath.c_str()))
            
            callbacks.StartNewSession(folderName);
            edbReader.StartRead(func, &callbacks);
            edbReader.Close();

            if (stopFlag)
            {
                break;
            }

            currentDate.AddDay(1);
            folderName = GetFolderNameFromCP2Time(currentDate);
            fullPath = pathToDB + '\\' + folderName + "\\Plaza2_AllData.edb";
        }
        callbacks.EndSessionRange();
    }
    else
    {
        return 1;
    }

    return 0 /*BaseApp::Run()*/;
}

BOOL DataAnalyzerApp::CtrlHandlerCallback(DWORD dwCtrlType)
{
    switch (dwCtrlType)
    {
    case CTRL_C_EVENT:
        gPtr->StopApp();
        Sleep(1000);
        return (TRUE);

    case CTRL_CLOSE_EVENT:
        gPtr->StopApp();
        Sleep(1000);
        return (TRUE);

    case CTRL_BREAK_EVENT:
    case CTRL_LOGOFF_EVENT: 
    case CTRL_SHUTDOWN_EVENT: 
            return (FALSE); 

    default:
        return (FALSE);
    }
}
