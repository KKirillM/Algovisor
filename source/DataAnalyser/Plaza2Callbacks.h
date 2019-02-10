#pragma once
#include "..\Plaza2Utils\p2time.h"
#include "..\Plaza2_Connector\Plaza2_Tables.h"
#include "..\Utils\MatchingData.h"
#include "TradeSessionData.h"
#include "BackTester.h"

class FileLogger;

struct OptionsParams
{
    bool checkTimeInterval;
    fo2base::CP2Time startEventTime;
    fo2base::CP2Time endEventTime;
    std::string pathToLogs;
    std::string fileNamePrefix;
	std::string dateFormat;
    std::set<int> isinIDList;
    bool needLog_ORDLOG;
    bool needLog_DEALS;
    bool needLog_orderBook;
    bool needLog_aggrTrades;
    bool needLog_splitTrades;
    bool needLog_edbContent;
    bool needBacktest;
    int orderBookLevels;
    bool rtt;
    bool tradesStatistics;
    bool sessionStatistics;
    unsigned long long roundTripDelay;
};

class RTTCounter
{
    uint64_t counter;           // количество событий в полном ордер логе по всем инструментам произошедшее за 1 секунду
    uint64_t cumDeltaValue;     // накопленное время
    uint64_t timer;             // начальное время последнего интервала

public:
    RTTCounter()
    {
        counter = 0;            
        cumDeltaValue = 0;      
        timer = 0;              
    }

    bool isNewTimeInterval(const fo2base::UnixNanosecondsTime currentTime) const
    {
        const long long oneMinuteInNanosec = 60000000000;
        int64_t delta = currentTime - timer;
        return (delta > oneMinuteInNanosec); // проверяем прошла минута или нет
    }

    uint64_t calc() const
    {
        return counter != 0 ? (cumDeltaValue / counter) : 0;
    }

    void setNewIntervalValues(const int64_t time, const int64_t delta)
    {
        counter = 1;
        timer = time;
        cumDeltaValue = delta;
    }

    void addNewValue(const int64_t delta)
    {
        ++counter;
        cumDeltaValue += delta;
    }

    uint64_t getCounter() { return counter; }
};

struct HourlyStatistics 
{
    uint64_t tradesCount;            // кол-во трейдов
    uint64_t spreadChangesCount;     // кол-во изменений спрэда
    uint64_t eventCount;             // кол-во событий
};

typedef int InstrumentID, TradeSize, Frequency, PriceLevelCount, Hour;

// дополнительные счётчики
struct SessionStatistics
{
    //int64_t bidOrdersCount;         // суммарное количество заявок на покупку в стакане на всех ценовых уровнях
    //int64_t askOrdersCount;         // суммарное количество заявок на продажу в стакане на всех ценовых уровнях
    //int64_t bidOrdersSize;          // суммарный объём всех заявок на покупку в стакане на всех ценовых уровнях
    //int64_t askOrdersSize;          // суммарный объём всех заявок на продажу в стакане на всех ценовых уровнях

    uint64_t addOrderEventCount;     // суммарное количество добавленных заявок в стакан за сессию
    uint64_t delOrderEventCount;     // суммарное количество удалённых заявок из стакана за сессию
    uint64_t moveOrderEventCount;    // суммарное количество передвинутых заявок в стакане за сессию
    uint64_t eventsCountPerSession;  // суммарное количество событий в ядре по инструменту за сессию
    uint64_t totalTradesCount;            // суммарное количество агрегированных трейдов за сессию

    uint64_t cummIO;                     // кумулятивный открытый интерес

    uint64_t spreadChangesCount;     // количество изменений цены спрэда, за изменение считается изменение либо цены бида либо аска

    std::map<uint64_t, uint64_t> ordersSizeCounter;   // мапа значений: <размер добавленной заявки, количество заявок данного размера>
    std::set<uint64_t> ordersSizeOnBBO;          // множество размеров ордеров, которые встречались на ценовых уровнях BBO
    //std::set<int> cumulativeSizeOnBBO;      // сумма всех размеров ордеров, которые встречались на ценовых уровнях BBO 
    std::vector<std::pair<Hour, HourlyStatistics>> hourlyStatistics;  // статистика часовых значений по инструментам 

    Price lastBestBidPrice, lastBestAskPrice;   // последние цены бида и аска

    SessionStatistics() 
    { 
        memset(this, 0, offsetof(SessionStatistics, ordersSizeCounter));
    }
};

class StatisticsWrapper
{
    typedef size_t StatParameter;
    typedef std::map<InstrumentID, SessionStatistics> SessionStatisticsMap;
    typedef std::map<InstrumentID, std::map<fo2base::UnixNanosecondsTime, StatParameter>> MultiSessionStatisticsMap;

    SessionStatisticsMap sessionStatistics;
    MultiSessionStatisticsMap tradesMultiSessionStatistics;
    MultiSessionStatisticsMap spreadMultiSessionStatistics;

	TradeSessionData* pSessionData;

public:
    StatisticsWrapper();

    void Init(TradeSessionData* ptr);

    /*void CalcSizeCounters(const InstrumentIDType ID)
    {
        assert(matchingData != NULL);
        statData[ID].bidOrdersCount = matchingData->getOrdersBook(ID)->GetBidSet()->OrdersCount();
        statData[ID].askOrdersCount = matchingData->getOrdersBook(ID)->GetAskSet()->OrdersCount();
        statData[ID].bidOrdersSize = matchingData->getOrdersBook(ID)->GetBidSet()->OrdersSize();
        statData[ID].askOrdersSize = matchingData->getOrdersBook(ID)->GetAskSet()->OrdersSize();
    }*/

    void IncAddOrderCounter(const InstrumentID& ID) { ++sessionStatistics[ID].addOrderEventCount; ++sessionStatistics[ID].eventsCountPerSession; }
    void IncDelOrderCounter(const InstrumentID& ID) { ++sessionStatistics[ID].delOrderEventCount; ++sessionStatistics[ID].eventsCountPerSession; }   
    void IncMoveOrderCounter(const InstrumentID& ID) { ++sessionStatistics[ID].moveOrderEventCount; ++sessionStatistics[ID].eventsCountPerSession; }    

    void SetTotalTradesCount(const InstrumentID& ID, const StatParameter count)    { sessionStatistics[ID].totalTradesCount = count; }
    void AddOrderStatistic(const InstrumentID& ID, const StatParameter orderSize);
    void SetCummOI(const InstrumentID& ID, const StatParameter IOSize) { sessionStatistics[ID].cummIO = IOSize; }
    void AddBBOOrderSize(const InstrumentID& ID);
    void CalculateHourlyStatistics(const InstrumentID& currInstrumentID, const Hour hour);
    void CalculateSpreadChangesCount(const InstrumentID& ID);
    void PrepareHourlyStatistics();
    void CalculateMultiSessionStatistics(const fo2base::UnixNanosecondsTime localTime);

    void ResetSessionStatistics() { sessionStatistics.clear(); }

    const SessionStatisticsMap& GetSessionStatisticsData() const { return sessionStatistics; }
    const MultiSessionStatisticsMap& GetTradesMultiSessionStatisticsData() const { return tradesMultiSessionStatistics; }
    const MultiSessionStatisticsMap& GetSpreadMultiSessionStatisticsData() const { return spreadMultiSessionStatistics; }

    
};  // OrdersBookStatistic

class BackTester;

class Plaza2Callbacks
{
    class RevisionChecker
    {
        int64_t lastRevision;
        int64_t prevRevision;
        std::string replName;

    public:
        RevisionChecker(const std::string& name) : replName(name), lastRevision(0), prevRevision(0) {}

        void Reset() 
        {
            lastRevision = prevRevision = 0;
        }

        bool check(const int64_t incomeRevision)
        {
            if (incomeRevision == 1) // если читаем новый edb-файл, ревизии с единицы начинаются
            {
                lastRevision = incomeRevision;
                return true;
            }

            if (incomeRevision <= lastRevision)
            {
                if (prevRevision > incomeRevision)
                {
                    P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_WARNING, "RevisionChecker", "Wrong revision sequence in %s, lastRevesion: %lld, incomeRevision: %lld", replName.c_str(), lastRevision, incomeRevision))
                        prevRevision = incomeRevision;
                }

                if (incomeRevision == lastRevision)
                {
                    P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_WARNING, "RevisionChecker", "Revision sequence restored in %s, lastRevesion: %lld, incomeRevision: %lld", replName.c_str(), lastRevision, incomeRevision))
                }

                return false;
            }

            lastRevision = incomeRevision;
            prevRevision = incomeRevision;

            return true;
        }

    };  // RevisionChecker

    const bool& bAbortOperation;
    bool isTradingPeriod;                           // true - можно посылать ордера и торговать, false - не торговый период (клиринг...)
    OptionsParams params;

    std::unique_ptr<FileLogger> edbContentLogger;   // логер для оглавления содержимого edb-файла
    std::unique_ptr<FileLogger> orderlogLogger;     // логер для входящих данных потока ордерлога, потока сделок, собранных стаканов и трейдов
    std::unique_ptr<FileLogger> roundTripLogger;    // логер для времени задержки получения маркет даты

    RTTCounter orderRTT;                            // счётчик пар значений [текущее время : значение дельты задержки в наносеках между ядерным временем и временем получения заявки в маркетдате]

    StatisticsWrapper statistics;                   // статистика посчитанная по книгам заявок
    TradeSessionData* sessionData;
    std::unique_ptr<BackTester> strategyTester;

    RevisionChecker revChecherForEDBFile;
    RevisionChecker revChecherForORDLOG;
    RevisionChecker revChecherForDEALS;

	int64_t lastCoreTime;	// последнее время ядра, которое было зафиксировано в потоках репликации

public:
	
	using callbackPtr = void (Plaza2Callbacks::*)(void*, const fo2base::UnixNanosecondsTime);
	//typedef void (Plaza2Callbacks::*callbackPtr)(void*, const fo2base::UnixNanosecondsTime);

    Plaza2Callbacks(const bool& abortFlag);
    ~Plaza2Callbacks();

    void InitParams(const OptionsParams& _params);

    void StartSessionRange();
    void EndSessionRange();
    void StartNewSession(const std::string& sessionName);
    void EndSession();

    bool AbortCheck() const { return bAbortOperation; }

    void ResetRevisionCheckers();

    static P2ERR MainCallback(const TEReplRec* pRec, void* param);

    void EdbContentToLog(const TEReplRec* pRec);

private:
	void sys_events(void* data, const fo2base::UnixNanosecondsTime localTime);
	void orders_log(void* data, const fo2base::UnixNanosecondsTime localTime);
	void deal(void* data, const fo2base::UnixNanosecondsTime localTime);
	void fut_sess_contents(void* data, const fo2base::UnixNanosecondsTime localTime);
	void opt_sess_contents(void* data, const fo2base::UnixNanosecondsTime localTime);
	void fut_sess_settl(void* data, const fo2base::UnixNanosecondsTime localTime);
	void opt_sess_settl(void* data, const fo2base::UnixNanosecondsTime localTime);

    // функции для проверки необходимости логирования различных данных
    bool logORDLOG() const { return params.needLog_ORDLOG; }
    bool logDEALS() const { return params.needLog_DEALS; }
    bool logOrderBook() const { return params.needLog_orderBook; }
    bool logAggrTrades() const { return params.needLog_aggrTrades; }
    bool logSplitTrades() const { return params.needLog_splitTrades; }
    bool logIsinID(const int ID) const { return params.isinIDList.empty() || params.isinIDList.find(ID) != params.isinIDList.end(); }
    bool logRTT() const { return params.rtt; }
    bool logTradesStatistics() const { return params.tradesStatistics; }
    bool logSessionStatistics() const { return params.sessionStatistics; }
    bool logEdbContent() const { return params.needLog_edbContent; }
    bool NeedBacktest() const { return params.needBacktest; }

    // запись задержки в файл в формате: RTT; ВРЕМЯ_ЯДРА; ВРЕМЯ_СБОРЩИКА_ДАННЫХ; КОЛИЧЕСТВО_СОБЫТИЙ_ЗА_ПОСЛЕДНЮЮ_СЕКУНДУ;
    void RTTToFile(const FORTS_ORDLOG_REPL::orders_log& info, const fo2base::UnixNanosecondsTime time);

    // запись cтатистических данных о раздробленных и агрегированных трейдах за всю сессию
    void TradesStatisticsToFile(const int isin_id, const std::string& date);

    // запись cтатистических данных за одну торговую сессию об ордерах и агрегированных трейдах в файл
    // данные включают название инструмента, количество ордеров каждого размера, который встречался в торгах,
    // количество трейдов каждого размера, количество ценовых уровней, которые пробил трейд
    void PerSessionStatisticsToFile(const std::string& date);

    // запись ститстических данных за несколько торговых сессий в файл
    void MultiSessionStatisticsToFile();

	// это две вспомогательные функции используются для PerSessionStatisticsToFile
    void GetFrequencyTradeStatistic(std::map<InstrumentID, std::map<TradeSize, Frequency>>& massiv);
    void GetDepthOfPenetrationStatistic(std::map<InstrumentID, std::map<PriceLevelCount, Frequency>>& massiv);
    
};  // Plaza2Callbacks
