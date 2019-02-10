// ASTSConnector_Tester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// ASTS MteSrl API
#include "astsbridge-4.2.3.1135\demo\MSVC\mtesrl.h"
#include "astsbridge-4.2.3.1135\demo\MSVC\mteerr.h"
#include "astsbridge-4.2.3.1135\demo\MSVC\DataTypes.h"

#include "Logger.h"
#include "IniFileWorker.h"
#include "ASTS_Connector.h"

void AllTrades(TMTETable* data, TTable* tableInterface)
{
    printf("%.*s, rowsCount=%d\n", tableInterface->name.length, tableInterface->name.data, data->rowsCount);
}

void OrderBook(TMTETable* data, TTable* tableInterface)
{
    printf("%.*s, rowsCount=%d\n", tableInterface->name.length, tableInterface->name.data, data->rowsCount);
}

void ServerTime(TMTETable* data, TTable* tableInterface)
{
   printf("%.*s, rowsCount=%d\n", tableInterface->name.length, tableInterface->name.data, data->rowsCount);
}

void Test1(const _TCHAR* argv, const MarketType market_type)
{
    std::unique_ptr<ASTS_ConnectorType1> connector;
    std::string pathToRegKey = "Software\\Algovisor\\Algovisor";

    try
    {
        connector.reset(ASTS_ConnectorType1::GetASTSConnectorType1());

        // считываем данные для инициализации соединения из INI-файла
        connector->GetASTSInitParametersFromINI(std::string(argv), (market_type == MarketType::STOCK_MARKET ? std::string(ASTS_STOCK) : std::string(ASTS_CURRENCY)));
        //connector->GetASTSInitParametersFromReg(pathToRegKey);

        // для каждой таблицы устанавливаем функцию коллбэка
        //connector->SetCallbackForTable(std::string("ALL_TRADES"), AllTrades);
        //connector->SetCallbackForTable(std::string("EXT_ORDERBOOK"), OrderBook);
        //connector->SetCallbackForTable(std::string("TESYSTIME"), ServerTime);
        //connector->SetCallbackForTable(std::string("NEGDEALBOOK"), ServerTime);

        // общий коллбэка для всех таблиц
        connector->SetUniversalCallback(AllTrades);

        connector->OpenConnection(market_type);

        Sleep(10000);

        connector->Disconnect();
    }
    catch(std::exception& e)
    {
        setlocale(LC_ALL,"Russian");
        std::cout << e.what() << std::endl;
    }
}

void Test2(const _TCHAR* argv, const MarketType market_type)
{
    std::unique_ptr<ASTS_ConnectorType1> connector;
    const int passwordFiedlLength = 10;

    try
    {
        connector.reset(ASTS_ConnectorType1::GetASTSConnectorType1());
        connector->GetASTSInitParametersFromINI(std::string(argv), (market_type == MarketType::STOCK_MARKET ? std::string(ASTS_STOCK) : std::string(ASTS_CURRENCY)));
        connector->OpenConnection(market_type);

        TTransactions transactionInterface = connector->GetTransactionsInterface();
        TTransaction* changePassTransInterface = transactionInterface.GetTransationByName("CHANGE_PASSWORD");

        std::string oldPassword = connector->GetPassword();
        oldPassword += std::string(passwordFiedlLength -  oldPassword.length(), ' ');
        std::string pwd = "7777";
        std::string newPassword =  pwd + std::string(passwordFiedlLength -  pwd.length(), ' ');
        connector->ExecTransaction("CHANGE_PASSWORD", oldPassword + newPassword);
        
        IniFileWorker ini;
        ini.Open(argv);
        ini.WriteStr("ASTS:currency", "PASSWORD", pwd);
        ini.Close();

        connector->Disconnect();
    }
    catch(std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}

int _tmain(int argc, _TCHAR* argv[])
{
    if (argc < 2)
    {
        printf("Second parameter is empty, please specify path to INI-file");
        return 1;
    } 

    // Logger test
    // FileLogger logger("", "logFile.txt");
    // logger.print(LOG_TRACE, "", "%s\n", "Test string");

    printf("ASTS_STOCK market test START:\n");
    //Test1(argv[1], MarketType::STOCK_MARKET);
    printf("ASTS_STOCK market test END.\n\n");

    printf("ASTS_CURRENCY market test START:\n");
    //Test1(argv[1], MarketType::CURRENCY_MARKET);
    printf("ASTS_CURRENCY market test END.\n\n");

    printf("ASTS_CURRENCY market transaction test START:\n");
    Test2(argv[1], MarketType::CURRENCY_MARKET);
    printf("ASTS_CURRENCY market transaction test END.\n\n");

	return 0;
}

