#include "stdafx.h"
#include "IniFileWorker.h"
#include "HelpTools.h"
#include "CommandLineParser.h"
#include "global.h"
#include "ASTS_Connector.h"

    MTE_VERSION_NEW

void checkErrorCode(int32_t errorCode, MTEMSG* mteMsg)
{
    char errMsg[512];
    if (errorCode == MTE_TSMR)
    {
        sprintf_s(errMsg, sizeof(errMsg), "MTERefresh() FAILED, return_value=%d, error_message=%.*s", errorCode, mteMsg->DataLen, mteMsg->Data);
    }
    else
    {
        sprintf_s(errMsg, sizeof(errMsg), "MTERefresh() FAILED, return_value=%d, error_message=%.*s", errorCode, MTEErrorMsg(errorCode));
    }
    throw std::exception(errMsg);
}

void checkErrorCode(int32_t errorCode, char* message)
{
    char errMsg[512];
    
    sprintf_s(errMsg, sizeof(errMsg), "MTERefresh() FAILED, return_value=%d, error_message=%.*s", errorCode, message);
    throw std::exception(errMsg);
}


void ASTS_Initializer::GetASTSInitParametersFromINI(const std::string& pathToINI, const std::string& section)
{
    if (pathToINI.empty())
    {
        std::string errorString = "Path to INI-file is not set";
        assert(FALSE);
        throw std::exception(errorString.c_str());
    }

    IniFileWorker ini;

    try
    {
        ini.Open(pathToINI);

        if (section == ASTS_STOCK)
        {
            m_params.marketType = MarketType::STOCK_MARKET;
        }
        else if (section == ASTS_CURRENCY)
        {
            m_params.marketType = MarketType::CURRENCY_MARKET;
        }

        m_params.host += ini.ReadStr(section, "HOST");
        m_params.server += ini.ReadStr(section, "SERVER");
        m_params.userid += ini.ReadStr(section, "USERID");
        m_params.password += ini.ReadStr(section, "PASSWORD");
        m_params.iface += ini.ReadStr(section, "INTERFACE");
        m_params.logfolder += ini.ReadStr(section, "LOGFOLDER");
        m_params.pathToDB = ini.ReadStr("options", "pathToDB");
        m_params.mteUpdateRefreshInterval = ini.ReadInt(section, "refreshInterval");
    }
    catch (IniFileException& e)
    {
        char errMsg[512];
        sprintf_s(errMsg, sizeof(errMsg), "%s, error: %d", e.what(), e.GetExceptionType());
        throw std::exception(errMsg);
    }

    ini.Close();
}

void ASTS_Initializer::GetASTSInitParametersFromReg(const std::string& pathToRegKey)
{
    HKEY key;
    LSTATUS res;
    if (ERROR_SUCCESS == (res = ::RegOpenKeyEx(HKEY_CURRENT_USER, pathToRegKey.c_str(), NULL, KEY_QUERY_VALUE/*|KEY_WOW64_64KEY*/, &key)))
    {
        BYTE param[512];
        DWORD paramSize = sizeof(param);

        if (ERROR_SUCCESS == ::RegQueryValueEx(key, "HOST", NULL, NULL, param, &paramSize))
        {
            m_params.host += (char*)param;
        }
        if (ERROR_SUCCESS == ::RegQueryValueEx(key, "SERVER", NULL, NULL, param, &paramSize))
        {
            m_params.server += (char*)param;
        }
        if (ERROR_SUCCESS == ::RegQueryValueEx(key, "USERID", NULL, NULL, param, &paramSize))
        {
            m_params.userid += (char*)param;
        }
        if (ERROR_SUCCESS == ::RegQueryValueEx(key, "PASSWORD", NULL, NULL, param, &paramSize))
        {
            m_params.password += (char*)param;
        }
        if (ERROR_SUCCESS == ::RegQueryValueEx(key, "INTERFACE", NULL, NULL, param, &paramSize))
        {
            m_params.iface += (char*)param;
        }
        if (ERROR_SUCCESS == ::RegQueryValueEx(key, "LOGFOLDER", NULL, NULL, param, &paramSize))
        {
            m_params.logfolder += (char*)param;
        }

        if (ERROR_SUCCESS == ::RegQueryValueEx(key, "refreshInterval", NULL, NULL, param, &paramSize))
        {
            m_params.mteUpdateRefreshInterval = *(unsigned int*)param;
        }
        else m_params.mteUpdateRefreshInterval = 100;

        if (ERROR_SUCCESS == ::RegQueryValueEx(key, "MarketType", NULL, NULL, param, &paramSize))
        {
            if (strcmp((char*)param, ASTS_STOCK) == 0)
            {
                m_params.marketType = MarketType::STOCK_MARKET;
            }
            else if (strcmp((char*)param, ASTS_CURRENCY) == 0)
            {
                m_params.marketType = MarketType::CURRENCY_MARKET;
            }
        }

        RegCloseKey(key);
    }
    else if (res == ERROR_FILE_NOT_FOUND)
    {
        TCHAR errMsg[512] = {0};
        sprintf_s(errMsg, sizeof(errMsg), "The specified registry path: HKEY_CURRENT_USER%s is invalid", pathToRegKey.c_str());
        throw std::exception(errMsg);
    }
    else if (res == ERROR_BAD_PATHNAME)
    {
        TCHAR errMsg[512] = {0};
        sprintf_s(errMsg, sizeof(errMsg), "The specified registry path: HKEY_CURRENT_USER%s not found", pathToRegKey.c_str());
        throw std::exception(errMsg);
    }
    else
    {
        TCHAR errMsg[512] = {0};
        TCHAR win_error[512] = {0};
        HelpTools::GetLastErrorString(win_error);
        sprintf_s(errMsg, sizeof(errMsg), "Can't open registry path: HKEY_CURRENT_USER%s, Error: %s", pathToRegKey.c_str(), win_error);
        throw std::exception(errMsg);
    }
}

void ASTS_Initializer::SetASTSInitParameters(const ASTSInitParameters& _params)
{
    m_params = _params;
}

//void ASTS_Initializer::GetTableListFromINI(const std::string& pathToINI)
//{
//    if (pathToINI.empty())
//    {
//        std::string errorString = "Path to INI-file is not set";
//        assert(FALSE);
//        throw std::exception(errorString.c_str());
//    }
//
//    IniFileWorker ini;
//
//    try
//    {
//        ini.Open(pathToINI);
//
//        std::string tablesString = ini.ReadStr("ASTS:tables", "tables");
//        std::vector<std::string> tablesName;
//        utils::CSimpleParserA parser;
//        parser.parse(tablesString, ';', tablesName);
//
//        for (auto it = tablesName.begin(), end = tablesName.end(); it != end; ++it)
//        {
//            TableParams params;
//            params.name = (*it);
//            StockTableType type = m_stockTables[(*it)];
//            m_tableParams[type] = params;
//        }
//    }
//    catch (IniFileException& e)
//    {
//        char errMsg[512];
//        sprintf_s(errMsg, sizeof(errMsg), "%s, error: %d", e.what(), e.GetExceptionType());
//        throw std::exception(errMsg);
//    }
//
//    ini.Close();
//}

void ASTS_Initializer::SetTableList(TablesParams& list)
{
    m_tableList.swap(list);
}

void ASTS_Initializer::SetEmptyParamsForTables(TInterface& interFace)
{
    for (auto it = m_tableList.begin(), end = m_tableList.end(); it != end; ++it)
    {
        TTable table = interFace.tables.GetTableByName((*it).second.name);
        char* ptr = (char*)table.inputFiedls.fields;

        for (int start = 0, end = table.inputFiedls.numberOfFields; start != end; ++start)
        {
            TFieldIn field;
            field.SetValues(ptr);
            (*it).second.params += std::string(field.size, ' ');
        }
    }
}


void ASTS_Initializer::InitAllMarketTables()
{
    switch (m_params.marketType)
    {
    case MarketType::STOCK_MARKET:
        m_allMarketTables.reset(new ASTSStockTableList());
        break;
    case MarketType::CURRENCY_MARKET:
        m_allMarketTables.reset(new ASTSCurrencyTableList());
        break;
    default:
        throw std::exception("MarketType is not initialized");
        break;
    }
}

std::string ASTS_Connector::GetVersion() const
{
    return std::string(MTEGetVersion());
}

ASTSConnectionStatus ASTS_Connector::GetConnectionStatus() const
{
    return (ASTSConnectionStatus)MTEConnectionStatus(connectionHandle);
}

void ASTS_Connector::SaveSnapshotToFile(const std::string& snapshotFilePath)
{
    HelpTools::SaveBinaryToFile(snapshotFilePath, snapshotBuf, snapshotLen);
}

void ASTS_Connector::ReadSnapshotFile(const std::string& snapshotFilePath)
{
    HANDLE fH;
    FILETIME creationTime;
    SYSTEMTIME sysFileTime;

    fH = CreateFile(snapshotFilePath.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if(fH != INVALID_HANDLE_VALUE)
    {
        GetFileTime(fH, &creationTime, 0, 0);
        FileTimeToSystemTime(&creationTime, &sysFileTime);
        CloseHandle(fH);
    }
    else
    {
        return;
    }

    SYSTEMTIME sysTime;
    GetSystemTime(&sysTime);
    if (sysTime.wDay == sysFileTime.wDay)
    {
        snapshotLen = HelpTools::ReadFileInBuffer(snapshotFilePath, snapshotBuf);
    }
    else
    {
        DeleteFile(snapshotFilePath.c_str());
    }
}


ASTS_ConnectorType1* ASTS_ConnectorType1::GetASTSConnectorType1()
{
    return (new ASTS_ConnectorType1());
}

void ASTS_ConnectorType1::OpenConnection(const MarketType type)
{
    marketType = type;

    char errMsg[512];
    std::string connectionString;
    connectionString = m_params.host + '\n' + m_params.server + '\n' + m_params.userid + '\n' + m_params.password + '\n' + m_params.iface + '\n' + m_params.logfolder + '\n';

    if (MTE_OK > (connectionHandle = MTEConnect(connectionString.c_str(), errMsg)))
    {
        char msg[512];
         CharToOemA(errMsg, errMsg); // перевод кодировки из 1251 в 866
        // или
        // setlocale(LC_ALL,"Russian"); // перевод кодировки из 866 в 1251 (866 по-умолчанию у консоли стоит)
        sprintf_s(msg, sizeof(msg), "OpenConnection() error %d: '%s'\n", connectionHandle, errMsg);
        throw std::exception(msg);
    }

    // проверка на отсутствие в мапе таблиц, дл€ которых не установлены коллбэки
    for (auto it = m_tableList.begin(), end = m_tableList.end(); it != end; ++it)
    {
        if ((*it).second.func == NULL)
        {
            char msg[512];
            sprintf_s(msg, sizeof(msg), "Callback function is not set for table %s\n", (*it).second.name.c_str());
            Disconnect();
            throw std::exception(msg);
        }
    }

    GetInterfaceData();

    TCHAR currDir[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, currDir);
    std::string path = currDir + m_params.pathToDB + '\\' + "ASTS_Snapshot\\" + (marketType == MarketType::STOCK_MARKET ? SNAPSHOT_FILE_STOCK : SNAPSHOT_FILE_CURRENCY);
    ReadSnapshotFile(path);

    SetEmptyParamsForTables(interFace);  // установим дл€ всех таблиц требующих входные параметры пустые значени€

    m_callbackExit = true;
    Fo2Thread::startThread("ASTS_CALLBACK");
}

void ASTS_ConnectorType1::Disconnect()
{
    m_callbackExit = false;
    if (!Fo2Thread::join())
    {
        char msg[512];
        sprintf_s(msg, sizeof(msg), "Callback function close error\n");
        throw std::exception(msg);
    }
    for (auto it = m_tableList.begin(), end = m_tableList.end(); it != end; ++it)
    { 
        if ((*it).second.handle >= MTE_OK)
        {
            MTECloseTable(connectionHandle, (*it).second.handle);
        }
    }
    MTEDisconnect(connectionHandle);
    connectionHandle = 0;
}

void ASTS_ConnectorType1::run()
{
    try
    {
        if (snapshotLen == 0)
        {
            OpenTables();
        }
        else
        {
            OpenTabelsFromSnapshot();
        }

        TablesLoop();

        TCHAR currDir[MAX_PATH];
        GetCurrentDirectory(MAX_PATH, currDir);
        std::string path = std::string(currDir) + m_params.pathToDB + '\\' + "ASTS_Snapshot\\" + (marketType == MarketType::STOCK_MARKET ? SNAPSHOT_FILE_STOCK : SNAPSHOT_FILE_CURRENCY);
        SaveSnapshotToFile(path);
    }
    catch(std::exception& e)
    {
        // здесь надо логировать сообщение внешним логгером

        fo2base::chFail((PSTR)e.what());
    }
}

void ASTS_ConnectorType1::SetCallbackForTable(std::string& tableName, const TableCallback func)
{
    if (!m_allMarketTables)
    {
        InitAllMarketTables();
    }
    ASTSTableType type = m_allMarketTables->at(tableName);
    m_tableList[type].name = tableName;
    m_tableList[type].func = func;
}

void ASTS_ConnectorType1::SetUniversalCallback(const TableCallback func)
{
    if (!m_allMarketTables)
    {
        InitAllMarketTables();
    }
    for (auto it = m_allMarketTables->begin(), end = m_allMarketTables->end(); it != end; ++it)
    {
        ASTSTableType type = (*it).second;
        m_tableList[type].name = (*it).first;
        m_tableList[type].func = func;
    }
}

void ASTS_ConnectorType1::GetInterfaceData()
{
    int32_t err;
    char errMsg[512];
    MTEMSG* mteMsg;

    if (!g_isVersion2)
    {
        err = MTEStructure(connectionHandle, &mteMsg);
    }
    else
    {
        err = MTEStructureEx(connectionHandle, 3, &mteMsg);
    }

    if (MTE_OK != err) 
    {
        //CharToOemA(MTEErrorMsg(err), ErrMsg);
        char *msg = MTEErrorMsg(err);
        sprintf_s(errMsg, sizeof(errMsg), "MTEStructureEx() Failed. Error code=%d, error message='%s'\n", err, msg);
        throw std::exception(errMsg);
    }

    interFace.setData(mteMsg->Data, mteMsg->DataLen);
}

void ASTS_ConnectorType1::OpenTables()
{
    MTEMSG* mteMsg;
    TMTETable table;
    char* ptr;

    for (auto it = m_tableList.begin(), end = m_tableList.end(); it != end; ++it)
    {            
        if (MTE_OK > ((*it).second.handle = MTEOpenTable(connectionHandle, (*it).second.name.c_str(), (*it).second.params.c_str(), FALSE, &mteMsg)))
        {
            checkErrorCode((*it).second.handle, mteMsg);
        }

        TTable tableInterface = interFace.tables.GetTableByName((*it).second.name);
        if (mteMsg->DataLen > 0)
        {
            ptr = mteMsg->Data;
            table.SetValues(ptr);
            (*it).second.func(&table, &tableInterface);
        }
        (*it).second.flag = tableInterface.attributes;   // зададим флаги дл€ каждой из таблиц: tfNonUpdateable, tfUpdateable, tfClearOnUpdate, tfOrderbook
    }
}

void ASTS_ConnectorType1::OpenTabelsFromSnapshot()
{
    int32_t err;

    //if (MTE_OK > ((*it).second.handle = MTEOpenTableAtSnapshot(connectionHandle, (*it).second.name.c_str(), (*it).second.param.c_str(), snapshotBuf, snapshotLen, &mteMsg)))

    MteSnapTable* snapTables;
    if (MTE_OK > (err = MTEGetTablesFromSnapshot(connectionHandle, snapshotBuf, snapshotLen, &snapTables)))
    {
        checkErrorCode(err, "");
    }

    for (int i = 0; i < err; ++i)
    {
        for (auto it = m_tableList.begin(), end = m_tableList.end(); it != end; ++it)
        { 
            if (snapTables[i].name == (*it).second.name)
            {
                (*it).second.handle = snapTables[i].handle;
                (*it).second.params = snapTables[i].params;
                TTable tableInterface = interFace.tables.GetTableByName((*it).second.name);
                (*it).second.flag = tableInterface.attributes;
                break;
            }
        }
    }

    char errMsg[512];
    if (MTE_OK != (err = MTESetSnapshot(connectionHandle, snapshotBuf, snapshotLen, errMsg)))
    {
        checkErrorCode(err, errMsg);
    }
}

void ASTS_ConnectorType1::TablesLoop()
{
    int32_t err;
    MTEMSG* mteMsg;

    if (!m_tableList.empty())
    {
        while(m_callbackExit)
        {
            for (auto it = m_tableList.begin(), end = m_tableList.end(); it != end; ++it)
            {
                if ((*it).second.flag != TTableFlags::tfNonUpdateable)
                {
                    if (MTE_OK != (err = MTEAddTable(connectionHandle, (*it).second.handle, (int32_t)(*it).first)))
                    {
                        checkErrorCode(err, "");
                    }
                }
            }

            if (MTE_OK != (err = MTERefresh(connectionHandle, &mteMsg)))
            {
                //if (MTE_OK != (err = MTESetSnapshot(connectionHandle, snapshotBuf, snapshotLen, errMsg)))
                checkErrorCode(err, mteMsg); 
            }

            TMTETables tables;
            char* ptr = mteMsg->Data;
            tables.SetValues(ptr);

            for (int i = 0; i < tables.tablesCount; ++i)
            {
                TMTETable table;
                ptr = tables.SetPointerToTable(i);
                table.SetValues(ptr);

                auto it = m_tableList.find(table.ref);

                TTable tableInterface = interFace.tables.GetTableByName((*it).second.name);
                (*it).second.func(&table, &tableInterface);
            }

            // если дошли сюда, то уже точно обработали текущие полученные данные от MTERefresh и можно сохранить состо€ние снашота
            if (MTE_OK != (err = MTEGetSnapshot(connectionHandle, &snapshotBuf, &snapshotLen)))
            {
                checkErrorCode(err, mteMsg);
            }

            if (m_params.mteUpdateRefreshInterval > 0 && mteMsg->DataLen < 30720) // если объем полученного пакета данных превышает 30 кбайт, немедленно отправить новый запрос на обновление
            {
                /* ≈сли размер ответа на запрос не превышает 30 килобайт, то об€зательна пауза не менее
                    100 миллисекунд перед отправкой следующего запроса. ѕауза может отсчитыватьс€ от
                    времени отправки предыдущего запроса, а не от времени получени€ ответа. */

                Sleep(m_params.mteUpdateRefreshInterval);
            }
        }
    }
}

void ASTS_Connector::ExecTransaction(const std::string& transationName, const std::string& params)
{
    int32_t err;
    char resultMsg[512];

    err = MTEExecTrans(connectionHandle, transationName.c_str(), params.c_str(), resultMsg);
    if( err != MTE_OK )
    {
        checkErrorCode(err, resultMsg);
    }
}
