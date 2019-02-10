#pragma once

#include <list>
#include <map>
#include "fo2thread.h"

// ASTS MteSrl API
#include "astsbridge-4.2.3.1135\demo\MSVC\mtesrl.h"
#include "astsbridge-4.2.3.1135\demo\MSVC\mteerr.h"
#include "astsbridge-4.2.3.1135\demo\MSVC\DataTypes.h"


#define SNAPSHOT_FILE_STOCK "ASTS_STOCK_Snapshot.snp"
#define SNAPSHOT_FILE_CURRENCY "ASTS_CURRENCY_Snapshot.snp"

#define ASTS_STOCK "ASTS:stock"          // название секции в INI-файле для настроек подключения к фондовому рынку
#define ASTS_CURRENCY "ASTS:currency"    // название секции в INI-файле для настроек подключения к валютному рынку

#define STOCK_TABLE_LIST \
    STOCK_TABLE_NAME(ACCOUNT_BALANCE) \
    STOCK_TABLE_NAME(ALL_TRADES) \
    STOCK_TABLE_NAME(ASSETS) \
    STOCK_TABLE_NAME(AUCTSTATS) \
    STOCK_TABLE_NAME(BANK) \
    STOCK_TABLE_NAME(BANKACC) \
    STOCK_TABLE_NAME(BANKUSE) \
    STOCK_TABLE_NAME(BCMESSAGES) \
    STOCK_TABLE_NAME(BOARDS) \
    STOCK_TABLE_NAME(CLIENTCODES) \
    STOCK_TABLE_NAME(CURRENCY) \
    STOCK_TABLE_NAME(DPAUCTIONS) \
    STOCK_TABLE_NAME(EXT_MMSTATS) \
    STOCK_TABLE_NAME(EXT_ORDERBOOK) \
    STOCK_TABLE_NAME(EXT_TRADES) \
    STOCK_TABLE_NAME(FIRMS) \
    STOCK_TABLE_NAME(FIRM_HOLDING_TOTAL) \
    STOCK_TABLE_NAME(GCPOOL) \
    STOCK_TABLE_NAME(GCPOOLASSET) \
    STOCK_TABLE_NAME(INDEXES) \
    STOCK_TABLE_NAME(INSTRUMENTS) \
    STOCK_TABLE_NAME(MARKETS) \
    STOCK_TABLE_NAME(MM_STATS) \
    /*STOCK_TABLE_NAME(NEGDEALBOOK)*/ \
    STOCK_TABLE_NAME(NEGDEALS) \
    STOCK_TABLE_NAME(ONENEGDEAL) \
    STOCK_TABLE_NAME(ONEORDER) \
    STOCK_TABLE_NAME(ORDERS) \
    STOCK_TABLE_NAME(POSITIONS) \
    /*STOCK_TABLE_NAME(PRICEMOVELIMIT)*/ \
    /*STOCK_TABLE_NAME(QUOTEBOOK)*/ \
    STOCK_TABLE_NAME(QUOTES) \
    STOCK_TABLE_NAME(REPORTS) \
    /*STOCK_TABLE_NAME(REPO_NEGDEALBOOK)*/ \
    /*STOCK_TABLE_NAME(REPO_QUOTEBOOK)*/ \
    STOCK_TABLE_NAME(RM_HOLD) \
    STOCK_TABLE_NAME(RM_HOLD_DETL) \
    /*STOCK_TABLE_NAME(RM_INDICATIVE)*/ \
    STOCK_TABLE_NAME(RM_POSN) \
    STOCK_TABLE_NAME(RM_PRICERANGE) \
    STOCK_TABLE_NAME(RM_PRICERANGE_FIRM) \
    /*STOCK_TABLE_NAME(SECACCINT)*/ \
    STOCK_TABLE_NAME(SECTRANSFERS) \
    STOCK_TABLE_NAME(SECURITIES) \
    STOCK_TABLE_NAME(SEC_SETTLECODE) \
    STOCK_TABLE_NAME(SETTLECODES) \
    STOCK_TABLE_NAME(STATS) \
    /*STOCK_TABLE_NAME(TESYSTIME)*/ \
    STOCK_TABLE_NAME(TRADES) \
    STOCK_TABLE_NAME(TRADETIME) \
    STOCK_TABLE_NAME(TRDACC) \
    STOCK_TABLE_NAME(TRDTIMEGROUPS) \
    STOCK_TABLE_NAME(TRDTIMETYPES) \
    STOCK_TABLE_NAME(USERS) \
    STOCK_TABLE_NAME(USER_TRDACC) \
    STOCK_TABLE_NAME(USTRADES)

#define CURRENCY_TABLE_LIST \
    CURRENCY_TABLE_NAME(ALL_TRADES) \
    CURRENCY_TABLE_NAME(ASSETS) \
    CURRENCY_TABLE_NAME(AUCTSTATS) \
    CURRENCY_TABLE_NAME(BANK) \
    CURRENCY_TABLE_NAME(BANKACC) \
    CURRENCY_TABLE_NAME(BANKUSE) \
    CURRENCY_TABLE_NAME(BCMESSAGES) \
    CURRENCY_TABLE_NAME(BOARDS) \
    CURRENCY_TABLE_NAME(CLIENTCODES) \
    CURRENCY_TABLE_NAME(CURRENCY) \
    CURRENCY_TABLE_NAME(EXT_MMSTATS) \
    CURRENCY_TABLE_NAME(EXT_ORDERBOOK) \
    CURRENCY_TABLE_NAME(FIRMS) \
    CURRENCY_TABLE_NAME(INDEXES) \
    CURRENCY_TABLE_NAME(INSTRS) \
    CURRENCY_TABLE_NAME(MARKETS) \
    /*CURRENCY_TABLE_NAME(NEGDEALBOOK)*/ \
    CURRENCY_TABLE_NAME(NEGDEALS) \
    CURRENCY_TABLE_NAME(ONENEGDEAL) \
    CURRENCY_TABLE_NAME(ONEORDER) \
    CURRENCY_TABLE_NAME(ORDERS) \
    CURRENCY_TABLE_NAME(POSITIONS) \
    CURRENCY_TABLE_NAME(POSTYPE) \
    CURRENCY_TABLE_NAME(PRICEMOVELIMIT) \
    CURRENCY_TABLE_NAME(RM_HOLD) \
    /*CURRENCY_TABLE_NAME(RM_INDICATIVE)*/ \
    CURRENCY_TABLE_NAME(RM_PRICERANGE) \
    CURRENCY_TABLE_NAME(RM_PRICERANGE_FIRM) \
    CURRENCY_TABLE_NAME(RM_PRODUCT_SPREAD) \
    CURRENCY_TABLE_NAME(SECTRANSFERS) \
    CURRENCY_TABLE_NAME(SECURITIES) \
    CURRENCY_TABLE_NAME(SEC_SETTLECODE) \
    CURRENCY_TABLE_NAME(SEC_SETTLECODE_STATS) \
    CURRENCY_TABLE_NAME(SETTLECODES) \
    CURRENCY_TABLE_NAME(STATS) \
    /*CURRENCY_TABLE_NAME(TESYSTIME)*/ \
    CURRENCY_TABLE_NAME(TRADES) \
    CURRENCY_TABLE_NAME(TRADETIME) \
    CURRENCY_TABLE_NAME(TRDACC) \
    CURRENCY_TABLE_NAME(TRDTIMEGROUPS) \
    CURRENCY_TABLE_NAME(TRDTIMETYPES) \
    CURRENCY_TABLE_NAME(USERS) \
    CURRENCY_TABLE_NAME(USER_TRDACC)

enum class MarketType : unsigned
{
    STOCK_MARKET,
    CURRENCY_MARKET
};

//enum class StockTableType : unsigned
//    {
//#undef CURRENCY_TABLE_NAME
//#define CURRENCY_TABLE_NAME(x) x,
//        STOCK_TABLE_LIST
//        TABLES_COUNT
//};
//
//enum class CurrencyTableType : unsigned
//{
//#undef STOCK_TABLE_NAME
//#define STOCK_TABLE_NAME(x) x,
//        CURRENCY_TABLE_LIST
//        TABLES_COUNT
//};

typedef unsigned int ASTSTableType;
typedef std::map<std::string, ASTSTableType> TableList;

class ASTSTableList
{
    TableList m_astsTables;
public:
    typedef TableList::key_type key_type;
    typedef TableList::value_type value_type;
    typedef TableList::iterator iterator;

    iterator begin() { return m_astsTables.begin(); }
    iterator end() { return m_astsTables.end(); }
    ASTSTableType at(const key_type& key) 
    { 
        ASTSTableType type;
        try
        {
            type = m_astsTables.at(key);
        }
        catch (std::exception&)
        {
            char errMsg[512];
            sprintf_s(errMsg, sizeof(errMsg), "в мапе исходных таблиц m_astsTables отсутствует таблица с именем %s", key.c_str());
            throw std::exception(errMsg);
        }

        return type;
    }
    ASTSTableType& operator[](const key_type& key) { return m_astsTables[key]; }
};

class ASTSStockTableList : public ASTSTableList
{
public:
    ASTSStockTableList()
    {
        ASTSTableType type = 0;

#undef STOCK_TABLE_NAME
#define STOCK_TABLE_NAME(x) \
        operator[](#x)=type; \
        ++type;

        STOCK_TABLE_LIST
    }
};

class ASTSCurrencyTableList : public ASTSTableList
{
public:
    ASTSCurrencyTableList()
    {
        ASTSTableType type = 0;

#undef CURRENCY_TABLE_NAME
#define CURRENCY_TABLE_NAME(x) \
        operator[](#x)=type; \
        ++type;

        CURRENCY_TABLE_LIST       
    }
};

enum class ASTSConnectionStatus : unsigned
{
    UNKNOWN,                    // не известно
    OK,                         // соединение установлено
    INVALID,                    // указан недопустимый дескриптор соединения
    SRV_UNAVAILABLE,            // недоступен сревре ASTSBridge
    TRADE_ENGINE_UNAVAILABLE    // недоступна торговая система
};


struct ASTSInitParameters
{
    std::string host;
    std::string server;
    std::string userid;
    std::string password;
    std::string iface;
    std::string logfolder;
    std::string pathToDB;
    unsigned int mteUpdateRefreshInterval;
    MarketType marketType;

    ASTSInitParameters()  { Reset(); }

    void Reset() { host = "HOST="; 
                   server = "SERVER="; 
                   userid = "USERID="; 
                   password = "PASSWORD="; 
                   iface = "INTERFACE="; 
                   logfolder = "LOGFOLDER=";
                   mteUpdateRefreshInterval = 0;
                 }

}; // ASTSInitParameters

typedef void (*TableCallback)(TMTETable*, TTable*);

struct TableParams
{
    std::string name;
    std::string params;
    TableCallback func;
    int32_t handle;
    TTableFlags flag;

    TableParams() : func(NULL), handle(0), flag(TTableFlags::tfNonUpdateable) {}
};

typedef std::map<ASTSTableType, TableParams> TablesParams;

class ASTS_Initializer
{
public:
    ASTS_Initializer() {}

    virtual ~ASTS_Initializer() {}

    // инициализировать структуру ASTSInitParameters из INI-файла
    /* Пример содержания секции INI-файла:
        [ASTS]
        HOST=127.0.0.1:15005
        SERVER=UAT_GATEWAY
        USERID=MU1001000001
        PASSWORD=1111
        INTERFACE=IFCBroker_26
        LOGFOLDER=.\log
        refreshInterval=100
    */
    void GetASTSInitParametersFromINI(const std::string& pathToINI, const std::string& section);

    // инициализировать структуру ASTSInitParameters из реестра
    void GetASTSInitParametersFromReg(const std::string& pathToRegKey);

    // задать значения для структуры ASTSInitParameters
    void SetASTSInitParameters(const ASTSInitParameters& _params);

    // получить список таблиц из INI-файла
    /* Пример содержания секции INI-файла:
        [ASTS:tables]
        tables=ALL_TRADES;EXT_ORDERBOOK;TESYSTIME 
    */
    //void GetTableListFromINI(const std::string& pathToINI);

    // задать список необходимых таблиц
    void SetTableList(TablesParams& list);

    std::string GetPassword() { return std::string(m_params.password.c_str() + m_params.password.find('=') + 1); }

protected:
    void SetEmptyParamsForTables(TInterface& interFace);
    void InitAllMarketTables();

protected:
    ASTSInitParameters m_params;
    std::shared_ptr<ASTSTableList> m_allMarketTables;
    TablesParams m_tableList;

}; // ASTS_Initializer

class ASTS_Connector: public ASTS_Initializer
{
public:
    ASTS_Connector() : connectionHandle(0), snapshotBuf(NULL), snapshotLen(0) {}
    virtual ~ASTS_Connector() {}

    std::string GetVersion() const;
    ASTSConnectionStatus GetConnectionStatus() const;

    void SaveSnapshotToFile(const std::string& snapshotFilePath);
    void ReadSnapshotFile(const std::string& snapshotFilePath);

    void ExecTransaction(const std::string& transationName, const std::string& params);

protected:
    int32_t connectionHandle;
    char* snapshotBuf;  // указатель на буфер для резервного сохранения
    int snapshotLen;    // длина буфера для резервного сохранения

}; // ASTS_Connector

class ASTS_ConnectorType1: public ASTS_Connector, public Fo2Thread
{
public:

    static ASTS_ConnectorType1* GetASTSConnectorType1();

    ASTS_ConnectorType1() : m_callbackExit(false) {}
    virtual ~ASTS_ConnectorType1() {}

    void OpenConnection(const MarketType type);
    void Disconnect();

    virtual void run();

    // установка коллбэка для конкретной таблицы
    void SetCallbackForTable(std::string& tableName, const TableCallback func);

    // установка единого коллбэка для всех таблиц
    void SetUniversalCallback(const TableCallback func);

    TEnumTypes GetEnumsInterface() { return interFace.enumTypes; }
    TTables GetTablesInterface() { return interFace.tables; }
    TTransactions GetTransactionsInterface() { return interFace.transactions; }

private:
    void GetInterfaceData();
    void OpenTables();
    void OpenTabelsFromSnapshot();
    void TablesLoop();

private:
    volatile bool m_callbackExit;
    TInterface interFace;
    MarketType marketType;

};  // ASTS_ConnectorType1

//class ASTS_ConnectorType2: public ASTS_Connector
//{
//public:
//    typedef std::list<ASTS_Reciever*> RecieverList;
//
//    static ASTS_ConnectorType2* GetASTSConnectorType2();
//
//    ASTS_ConnectorType1();
//    ~ASTS_ConnectorType1();
//
//    void OpenConnection();
//    void Disconnect();
//
//    std::string GetVersion();
//
//    ASTSConnectionStatus GetConnectionStatus() const;
//
//    // запустить поток получения данных для указанных таблиц по конкретному инструменту и режиму торгов
//    void StartDataCallbackThread(std::vector<std::string> tablesList, ASTSCallback func, , );
//
//    void StopAllCallbacks();
//
//    //void GetInstrumentInfo(const std::string& instrumentName = "");
//
//private:
//    int32_t connectionHandle;
//    RecieverList 
//
//};  // ASTS_ConnectorType1
//
