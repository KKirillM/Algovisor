#pragma once
#include "..\Utils\MatchingData.h"
#include "..\Utils\singleton.h"
#include <cstdint>
#include "..\Plaza2_Connector\Plaza2_Tables.h"
#include "..\Plaza2_Connector\Plaza2Objects.h"
//#include "DataAnalyzerApp.h"
#include "Logger.h"

// направление изменения открытого интереса
enum InterestDirection
{
    UNDEFINED,          // состояние не определено (алгоритм определения ОИ ещё не обработал)
    UNKNOWN_POSITION,   // выяснить направление изменения позиции не удалось
    OPEN_POSITION,      // позиция открыта
    CLOSE_POSITION      // позиция закрыта
};

// раздробленный трейд
struct SplitTrade
{
   Price price;                      // цена трейда
   int64_t size;                     // размер трейда
   int64_t tradeID;                  // id трейда
   int64_t ordID;                    // id ордера
   int64_t time;                     // таймштемп
   InterestDirection IODir;          // направление изменения открытого интереса для этого трейда
   char agressorSide;                // сторона агрессора 'S' : 'B'
   int cumulativeOI;                 // значение кумулятивного ОИ после трейда

   SplitTrade() : size(0), tradeID(0), ordID(0), time(0), cumulativeOI(0), IODir(UNDEFINED), agressorSide('?') {}
};

// агрегированный трейд
struct AggregatedTrade
{
    std::vector<SplitTrade> components;     // массив раздробленных трейдов, из которых состоит данный агрегированный трейд
    Price initialPrice;                     // цена, по которой был выставлен ордер привёдший к трейду
    Price startPrice;                       // цена, с которой началось сведение трейда
    Price endPrice;                         // цена, на которой закончилось сведение трейда
    int matchSize;		                    // размер трейда
    int initialSize;						// исходный размер заявки, которая привела к трейду
    int64_t time;                           // таймштемп
    int64_t ordID;                          // id ордера
    InterestDirection IODir;                // направление изменения открытого интереса для этого трейда
    int executedPriceLevelsCount;           // количество полностью "съеденных" ценовых уровней
    char agressorSide;                      // сторона агрессора 'S' : 'B'
	unsigned int oppositeOrdersAbsorb;		// количество поглощённых ордеров на противоположной стороне
    int64_t oppositeOrderSize;              // размер заявки, которая была на противоположной стороне и с которой началось сведение
    int64_t priceLevelCumulativeSize;       // суммарный размер всех заявок на ценовом уровне, c которого началось сведение
    int64_t priceLevelOrdersCount;          // количество заявок на ценовом уровне, c которого началось сведение
    int64_t maxPriceLevelOrderSize;         // максимальный размер заявки противоположной стороны на ценовом уровне, по которому прошёл агрегированный трейд
    int64_t oppositeMaxOrderDeltaTime;      // количество наносекунд, которое отстояла самая крупная на ценовом уровне заявка до сведения с агрегированным трейдом
    int64_t totalBidSize;                   // суммарное количество размеров заявок на всех ценовых уровнях бидов
    int64_t totalAskSize;                   // суммарное количество размеров заявок на всех ценовых уровнях асков
    int64_t totalBidOrdersCount;            // суммарное количество заявок на всех ценовых уровнях бидов
    int64_t totalAskOrdersCount;            // суммарное количество заявок на всех ценовых уровнях асков
    int64_t totalEventsCount;               // суммарное количество событий произошедших на момент прохождения трейда (плотность потока ордеров)
    int64_t spreadChangesCount;             // суммарное количество изменений цен спрэда
    int status;                             // статус заявки, которая привела к трейду (поле нужно для определения вида заявки, которая породила трейд)

    AggregatedTrade() : matchSize(0), initialSize(0), time(0), IODir(UNDEFINED), executedPriceLevelsCount(0), agressorSide('?'),
                        oppositeOrderSize(0), priceLevelCumulativeSize(0), maxPriceLevelOrderSize(0), oppositeMaxOrderDeltaTime(0), oppositeOrdersAbsorb(0),
                        totalBidSize(0), totalAskSize(0), totalBidOrdersCount(0), totalAskOrdersCount(0), spreadChangesCount(0), status(0) {}

    // проверка что все значения кумулятивного ОИ у раздробленных трейдов заполнены
    bool isAllIOFilled()
    {
        for (auto& trade : components)
        {
            if (trade.cumulativeOI == 0)
                return false;
        }
        return true;
    }

    // проверяет наличие нужного значения tradeID
    bool isTradeIDExist(const int64_t id) const
    {
        for (auto& it : components)
        {
            if (it.tradeID == id)
                return true;
        }
        return false;
    }
};

class TradeSessionData
{
    friend class StatisticsWrapper;
    friend class Plaza2Callbacks;
	friend class CNewStorage<TradeSessionData>;

	struct TradeReadyFlag
	{
		bool flag;
		TradeReadyFlag() : flag(false) {}
		TradeReadyFlag(const bool f) : flag(f) {}
		operator bool() const
		{
			return flag;
		}
	};

    typedef CMatchingData::InstrumentIDType InstrumentIDType;
    typedef std::map<InstrumentIDType, P2Order> OppositeOrdersMap;
    typedef std::map<InstrumentIDType, std::vector<FORTS_DEALS_REPL::deal>> DealsSynchronizationMap;
    typedef std::map<InstrumentIDType, std::vector<SplitTrade>> SplitTradeMap;
    typedef std::map<InstrumentIDType, std::vector<AggregatedTrade>> AggrTradeMap;
	typedef std::map<InstrumentIDType, TradeReadyFlag> AggrTradeStatusMap;
    typedef std::map<InstrumentIDType, FORTS_FUTINFO_REPL::fut_sess_contents> FutInstrumentInfoMap;
    typedef std::map<InstrumentIDType, FORTS_OPTINFO_REPL::opt_sess_contents> OptInstrumentInfoMap;


	FutInstrumentInfoMap futInfoMap;			// массив данных фьючерсных инструментов
	OptInstrumentInfoMap optInfoMap;			// массив данных опционных инструментов

	CMatchingData ordersBooks;					// последние срезы стаканов заявок по всем инструментам
	OppositeOrdersMap temporaryOrders;			// мапа прихраненых ордеров, которые привели к трейдам и возможно остаток от них пойдёт в стакан

	DealsSynchronizationMap dealsCache;			// прихраниваем здесь трейды потока FORTS_DEALS_REPL, которые обгоняют поток FORTS_ORDLOG_REPL
	SplitTradeMap splitTradesMap;				// массив раздробленных трейдов
	AggrTradeMap aggrTradesMap;					// массив агрегированных трейдов
	AggrTradeStatusMap lastAggrTradeComplete;	// мапа флагов, показывающих что трейд полностью собран из кусочков данных, которые по нему приходят последовательно в логе, т.е. обработаны все сообщения action=TRADE для данного трейда				

	std::vector<SplitTrade>::iterator splitIterator;
	std::vector<AggregatedTrade>::iterator aggrIterator;

	int currInstrumentID;               // ID инструмента с которым сейчас работаем

public:    

    typedef std::pair<Price, Price> PriceLimits;

    ~TradeSessionData()  {}

    // добавление инструментов в список
    void PushFutInstrument(FORTS_FUTINFO_REPL::fut_sess_settl& info);
    void PushOptInstrument(FORTS_OPTINFO_REPL::opt_sess_settl& info);
    void PushFutSessionContents(FORTS_FUTINFO_REPL::fut_sess_contents& info);
    void PushOptSessionContents(FORTS_OPTINFO_REPL::opt_sess_contents& info);

    // обработка нового пакета данных полного ордерлога
    void PushOrderLogAction(const FORTS_ORDLOG_REPL::orders_log& info);

    // обработка нового пакета данных сделки
    void PushDealAction(const FORTS_DEALS_REPL::deal& info);

    // полностью очищает все накопленные данные: стаканы, трейды...
    void ClearAllData();

    // получение исина по id инструмента
    std::string GetIsinFromId(const InstrumentIDType ID) const { return isFutures(ID) ? futInfoMap.at(ID).isin : (isOption(ID) ? optInfoMap.at(ID).isin : ""); }

    PriceLimits GetPriceLimits(const InstrumentIDType ID);

    void GetAllIsins(std::set<InstrumentIDType>& isin_ids);

	const AggregatedTrade* GetLastTrade(const InstrumentIDType ID);
	bool isLastAggrTradeReady(const InstrumentIDType ID) const { return (lastAggrTradeComplete.find(ID) != lastAggrTradeComplete.end()) ? lastAggrTradeComplete.at(ID) : false; }

    bool isFutures(const InstrumentIDType ID) const { return futInfoMap.find(ID) != futInfoMap.end(); }
    bool isOption(const InstrumentIDType ID) const { return optInfoMap.find(ID) != optInfoMap.end(); }
	
	// получить шаг цены по инструменту
	double GetMinStep(const InstrumentIDType ID) const 
	{
		const char* minStepStr = isFutures(ID) ? futInfoMap.at(ID).min_step : (isOption(ID) ? optInfoMap.at(ID).min_step : NULL);
		double dDealPrice = 0;
		if (minStepStr)
		{
			P2BCDII2Double(&dDealPrice, minStepStr);
		}
		return dDealPrice;
	}

    // получение текущего среза стакана в виде строки
    std::string DumpBookToString(int currInstrumentID, int orderBookLevels);

    // функции для перебора значений массива раздробленных трейдов
    void StartEnumSplitTrades(const int currInstrumentID);
    bool GetNextSplitTrade(SplitTrade& trade, const int currInstrumentID);

    // функции для перебора значений массива агрегированных трейдов
    void StartEnumAggrTrades(const int currInstrumentID);
    bool GetNextAggrTrade(AggregatedTrade& trade, const int currInstrumentID);

    size_t GetAggrTradesCount(const int currInstrumentID) { return aggrTradesMap[currInstrumentID].size(); }

    void GetBestPrices(const InstrumentIDType ID, Price& bestBidPrice, Price& bestAskPrice);

private:

    TradeSessionData() : currInstrumentID(0) {}

    // 3 функции для обработки приходящих ордеров
    void AddOrder(const P2Order& p2order, COrdersBookBase* ordersBook);
    void RemoveOrder(const P2Order& p2order, COrdersBookBase* ordersBook, const bool isCrossRemove);
    void TradeOrder(const FORTS_ORDLOG_REPL::orders_log& info, const P2Order& p2order, COrdersBookBase* ordersBook);
    
};  // TradeSessionData

typedef CSingletonHolder<TradeSessionData>  TradeSessionDataHolder;

inline TradeSessionData* gGetTradeSessionData()
{ 
    return TradeSessionDataHolder::GetInstance(); 
}

inline void gDelTradeSessionData()
{ 
    return TradeSessionDataHolder::DelInstance();
}