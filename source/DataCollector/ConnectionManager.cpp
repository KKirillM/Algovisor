#include "stdafx.h"
#include "Plaza2Callbacks.h"
#include "Plaza2_Connector.h"
#include "ASTSData.h"
#include "p2execlog.h"
#include "CommandLineParser.h"
#include "auxiliary_methods.h"

#include "ConnectionManager.h"

const char* plaza2_section = "Plaza2";
const char* options_section = "options";
const char* asts_stock_section = "ASTS:stock";
const char* asts_currency_section = "ASTS:currency";
const char* rithmic_section = "Rithmic";
const char* ignoreProviderKey = "ignoreThisProvider";
const char* plaza2_eshmem_trades_section = "p2esharedmem:Plaza2_Trades";
const char* plaza2_eshmem_alldata_section = "p2esharedmem:Plaza2_AllData";
const char* plaza2_eshmem_info_section = "p2esharedmem:Plaza2_Info";
const char* asts_eshmem_stock_section = "p2esharedmem:ASTS_stock";
const char* asts_eshmem_currency_section = "p2esharedmem:ASTS_currency";
const char* rithmic_eshmem_section = "p2esharedmem:Rithmic";



ConnectionManager::ConnectionManager() : 
	ignorePlaza2(1),
    ignoreASTSStock(1),
    ignoreASTSCurrency(1),
    ignoreRithmic(1),
    startNewRouterProcess(-1)
{
}

ConnectionManager::~ConnectionManager()
{
}

void ConnectionManager::InitFlags()
{
    ignorePlaza2 = fo2base::gGetCP2Ini()->ReadInt(plaza2_section, ignoreProviderKey, -1);
    ignoreASTSStock = fo2base::gGetCP2Ini()->ReadInt(asts_stock_section, ignoreProviderKey, -1);
    ignoreASTSCurrency = fo2base::gGetCP2Ini()->ReadInt(asts_currency_section, ignoreProviderKey, -1);
    ignoreRithmic = fo2base::gGetCP2Ini()->ReadInt(rithmic_section, ignoreProviderKey, -1);

    if (ignorePlaza2 == (-1))
    {
        char msg[512];
        sprintf_s(msg, sizeof(msg), "Plaza2 parameter is missing in INI file. section='%s', key='%s'", plaza2_section, ignoreProviderKey);
        throw std::exception(msg);
    }
    if (ignoreASTSStock == (-1))
    {
        char msg[512];
        sprintf_s(msg, sizeof(msg), "ASTS parameter is missing in INI file. section='%s', key='%s'", asts_stock_section, ignoreProviderKey);
        throw std::exception(msg);
    }
    if (ignoreASTSCurrency == (-1))
    {
        char msg[512];
        sprintf_s(msg, sizeof(msg), "ASTS parameter is missing in INI file. section='%s', key='%s'", asts_currency_section, ignoreProviderKey);
        throw std::exception(msg);
    }
    if (ignoreRithmic == (-1))
    {
        char msg[512];
        sprintf_s(msg, sizeof(msg), "Rithmic parameter is missing in INI file. section='%s', key='%s'", rithmic_section, ignoreProviderKey);
        throw std::exception(msg);
    }
}

void ConnectionManager::Init()
{
    if (ignorePlaza2 == 0)
    {
        startNewRouterProcess = fo2base::gGetCP2Ini()->ReadInt(plaza2_section, "startNewRouterProcess", -1);
        if (startNewRouterProcess == -1)
        {
            throw std::exception("Parameter 'startNewRouterProcess' is missing in INI file in section='option'");
        }

        if (startNewRouterProcess == 1)
        {
            std::string pathToRouterExe = fo2base::gGetCP2Ini()->ReadStr(plaza2_section, "pathToRouterExe");
            std::string pathToRouterIni = fo2base::gGetCP2Ini()->ReadStr(plaza2_section, "pathToRouterIni");
            if (!pathToRouterExe.empty() && !pathToRouterIni.empty())
            {
                int ret = StartPlaza2RouterProcess(pathToRouterExe, pathToRouterIni, true);
                if (ret == 1)
                {
                    throw std::exception("Start Plaza2 router failed");
                }
            }
            else
            {
                throw std::exception("Plaza2 path to router or router INI-file is empty, but \"startNewRouterProcess\" parameter is enabled");
            }
        }  

        //externalStorages[Plaza2_Trades].Create(plaza2_eshmem_trades_section);
        externalStorages[Plaza2_AllData].Create(plaza2_eshmem_alldata_section);
        //externalStorages[Plaza2_Info].Create(plaza2_eshmem_info_section);
    }
    if (ignoreASTSStock == 0)
    {
        externalStorages[ASTS_Stock].Create(asts_eshmem_stock_section);
    }
    if (ignoreASTSCurrency == 0)
    {
        externalStorages[ASTS_Currency].Create(asts_eshmem_currency_section);
    }
    if (ignoreRithmic == 0)
    {
        externalStorages[Rithmic].Create(rithmic_eshmem_section);
    }

    if (ignorePlaza2 == 1 && ignoreASTSStock == 1 && ignoreASTSCurrency == 1 && ignoreRithmic == 1)
    {
        throw std::exception("All datafeed are ignored, all parameters \"ignoreThisProvider\" = 1 in INI-file");
    }

	P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, "ConnectionManager", "E-replica storages opened"))
}

void ConnectionManager::DeInit()
{
	for (int i = 0; i < ExternalStoragesCount; ++i)
	{
		if (externalStorages[i].IsInitialized())
			externalStorages[i].Disconnect();
	}

	P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, "ConnectionManager", "E-replica storages closed"))
}

void ConnectionManager::ConnectToDataProviders()
{
	//------------------------------------------
	// Plaza2
	//------------------------------------------
	if (ignorePlaza2 == 0)
	{
        GetPlaza2InitParametersFromINI(fo2base::gGetCP2Ini()->GetName(), params);

        if (!params.replstateFolderPath.empty())
        {
            CreateFolder(params.replstateFolderPath);
        }
        else
        {
            throw std::exception("Plaza2 path to Database parameter - dbPath, is empty");
        }

        P2ReplList streamParams;
        GetP2ReplListParametersFromINI(fo2base::gGetCP2Ini()->GetName(), streamParams);

        ConnectToPlaza2(streamParams);
	}
	else
    {
		P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, "ConnectionManager", "Plaza2 data provider ignored"))
    }
	//------------------------------------------
	// ASTS
	//------------------------------------------
    if (ignoreASTSStock == 0)
    {
        ConnectToASTS(MarketType::STOCK_MARKET);
    }
    else
    {
        P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, "ConnectionManager", "ASTS_Stock data provider ignored"))
    }
    if (ignoreASTSCurrency == 0)
    {
        ConnectToASTS(MarketType::CURRENCY_MARKET);
    }
    else
    {
        P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, "ConnectionManager", "ASTS_Currency data provider ignored"))
    }
	//------------------------------------------
	// Rithmic
	//------------------------------------------




}

void ConnectionManager::Disconnect()
{
	if (ignorePlaza2 == 0)
    {
		DisconnectFromPlaza2();

        if (startNewRouterProcess == 1)
        {
            std::string pathToRouter = fo2base::gGetCP2Ini()->ReadStr(plaza2_section, "pathToRouterExe");
            std::string routerProcessName;
            routerProcessName.assign(pathToRouter, pathToRouter.rfind('\\')+1, std::string::npos);
            StopPlaza2RouterProcess(routerProcessName);
        }
    }
    if (ignoreASTSStock == 0)
    {
        DisconnectFromASTS(MarketType::STOCK_MARKET);
    }
    if (ignoreASTSCurrency == 0)
    {
        DisconnectFromASTS(MarketType::CURRENCY_MARKET);
    }
}

void ConnectionManager::ConnectToPlaza2(P2ReplList& streamParams)
{
    try
    {
        for (auto it = streamParams.begin(), end = streamParams.end(); it != end; ++it)
        {
            it->second.name = it->first;
            it->second.name.erase(0, 7); // удаляем приставку 'p2repl_'
            it->second.func1 = Plaza2Callbacks::MsgTypeCallback;
            it->second.func2 = Plaza2Callbacks::StreamDataCallback;
        }

        plaza2Connector.reset(GetPlaza2ConnectorType1());
        plaza2Connector->OpenConnection(params, streamParams);
        P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, "ConnectionManager", "Connection opened"))
    }
    catch(Plaza2ConnectorException& e)
    {
        std::string errorString = "Plaza2ConnectorException occurred!";
        errorString += "Error code: " + e.ErrorCodeToStr();
        errorString += ". Error message : ";
        errorString += e.what();
        throw std::exception(errorString.c_str());
    }
}

void ConnectionManager::DisconnectFromPlaza2()
{
	if (plaza2Connector)
	{
        plaza2Connector->Disconnect();
	}
}

void ConnectionManager::ConnectToASTS(const MarketType marketType)
{
    fo2base::CP2DefaultIni* iniFile = fo2base::gGetDefaultIni();

    if(marketType == MarketType::STOCK_MARKET)
    {
        astsStockConnector.reset(ASTS_ConnectorType1::GetASTSConnectorType1());
        astsStockConnector->GetASTSInitParametersFromINI(iniFile->GetName(), std::string(ASTS_STOCK));
        astsStockConnector->SetUniversalCallback(ASTSData::ASTS_CallbackStock);
        astsStockConnector->OpenConnection(marketType);
    }
    else if(marketType == MarketType::CURRENCY_MARKET)
    {
        astsCurrencyConnector.reset(ASTS_ConnectorType1::GetASTSConnectorType1());
        astsCurrencyConnector->GetASTSInitParametersFromINI(iniFile->GetName(), std::string(ASTS_CURRENCY));
        astsCurrencyConnector->SetUniversalCallback(ASTSData::ASTS_CallbackCurrency);
        astsCurrencyConnector->OpenConnection(marketType);
    }
}

void ConnectionManager::DisconnectFromASTS(const MarketType marketType)
{
    if (marketType == MarketType::STOCK_MARKET && astsStockConnector)
    {
        astsStockConnector->Disconnect();
    }
    if (marketType == MarketType::CURRENCY_MARKET && astsCurrencyConnector)
    {
        astsCurrencyConnector->Disconnect();
    }
}