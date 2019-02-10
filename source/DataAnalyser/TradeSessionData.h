#pragma once
#include "..\Utils\MatchingData.h"
#include "..\Utils\singleton.h"
#include <cstdint>
#include "..\Plaza2_Connector\Plaza2_Tables.h"
#include "..\Plaza2_Connector\Plaza2Objects.h"
//#include "DataAnalyzerApp.h"
#include "Logger.h"

// ����������� ��������� ��������� ��������
enum InterestDirection
{
    UNDEFINED,          // ��������� �� ���������� (�������� ����������� �� ��� �� ���������)
    UNKNOWN_POSITION,   // �������� ����������� ��������� ������� �� �������
    OPEN_POSITION,      // ������� �������
    CLOSE_POSITION      // ������� �������
};

// ������������� �����
struct SplitTrade
{
   Price price;                      // ���� ������
   int64_t size;                     // ������ ������
   int64_t tradeID;                  // id ������
   int64_t ordID;                    // id ������
   int64_t time;                     // ���������
   InterestDirection IODir;          // ����������� ��������� ��������� �������� ��� ����� ������
   char agressorSide;                // ������� ��������� 'S' : 'B'
   int cumulativeOI;                 // �������� ������������� �� ����� ������

   SplitTrade() : size(0), tradeID(0), ordID(0), time(0), cumulativeOI(0), IODir(UNDEFINED), agressorSide('?') {}
};

// �������������� �����
struct AggregatedTrade
{
    std::vector<SplitTrade> components;     // ������ ������������� �������, �� ������� ������� ������ �������������� �����
    Price initialPrice;                     // ����, �� ������� ��� ��������� ����� �������� � ������
    Price startPrice;                       // ����, � ������� �������� �������� ������
    Price endPrice;                         // ����, �� ������� ����������� �������� ������
    int matchSize;		                    // ������ ������
    int initialSize;						// �������� ������ ������, ������� ������� � ������
    int64_t time;                           // ���������
    int64_t ordID;                          // id ������
    InterestDirection IODir;                // ����������� ��������� ��������� �������� ��� ����� ������
    int executedPriceLevelsCount;           // ���������� ��������� "���������" ������� �������
    char agressorSide;                      // ������� ��������� 'S' : 'B'
	unsigned int oppositeOrdersAbsorb;		// ���������� ����������� ������� �� ��������������� �������
    int64_t oppositeOrderSize;              // ������ ������, ������� ���� �� ��������������� ������� � � ������� �������� ��������
    int64_t priceLevelCumulativeSize;       // ��������� ������ ���� ������ �� ������� ������, c �������� �������� ��������
    int64_t priceLevelOrdersCount;          // ���������� ������ �� ������� ������, c �������� �������� ��������
    int64_t maxPriceLevelOrderSize;         // ������������ ������ ������ ��������������� ������� �� ������� ������, �� �������� ������ �������������� �����
    int64_t oppositeMaxOrderDeltaTime;      // ���������� ����������, ������� �������� ����� ������� �� ������� ������ ������ �� �������� � �������������� �������
    int64_t totalBidSize;                   // ��������� ���������� �������� ������ �� ���� ������� ������� �����
    int64_t totalAskSize;                   // ��������� ���������� �������� ������ �� ���� ������� ������� �����
    int64_t totalBidOrdersCount;            // ��������� ���������� ������ �� ���� ������� ������� �����
    int64_t totalAskOrdersCount;            // ��������� ���������� ������ �� ���� ������� ������� �����
    int64_t totalEventsCount;               // ��������� ���������� ������� ������������ �� ������ ����������� ������ (��������� ������ �������)
    int64_t spreadChangesCount;             // ��������� ���������� ��������� ��� ������
    int status;                             // ������ ������, ������� ������� � ������ (���� ����� ��� ����������� ���� ������, ������� �������� �����)

    AggregatedTrade() : matchSize(0), initialSize(0), time(0), IODir(UNDEFINED), executedPriceLevelsCount(0), agressorSide('?'),
                        oppositeOrderSize(0), priceLevelCumulativeSize(0), maxPriceLevelOrderSize(0), oppositeMaxOrderDeltaTime(0), oppositeOrdersAbsorb(0),
                        totalBidSize(0), totalAskSize(0), totalBidOrdersCount(0), totalAskOrdersCount(0), spreadChangesCount(0), status(0) {}

    // �������� ��� ��� �������� ������������� �� � ������������� ������� ���������
    bool isAllIOFilled()
    {
        for (auto& trade : components)
        {
            if (trade.cumulativeOI == 0)
                return false;
        }
        return true;
    }

    // ��������� ������� ������� �������� tradeID
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


	FutInstrumentInfoMap futInfoMap;			// ������ ������ ���������� ������������
	OptInstrumentInfoMap optInfoMap;			// ������ ������ ��������� ������������

	CMatchingData ordersBooks;					// ��������� ����� �������� ������ �� ���� ������������
	OppositeOrdersMap temporaryOrders;			// ���� ����������� �������, ������� ������� � ������� � �������� ������� �� ��� ����� � ������

	DealsSynchronizationMap dealsCache;			// ������������ ����� ������ ������ FORTS_DEALS_REPL, ������� �������� ����� FORTS_ORDLOG_REPL
	SplitTradeMap splitTradesMap;				// ������ ������������� �������
	AggrTradeMap aggrTradesMap;					// ������ �������������� �������
	AggrTradeStatusMap lastAggrTradeComplete;	// ���� ������, ������������ ��� ����� ��������� ������ �� �������� ������, ������� �� ���� �������� ��������������� � ����, �.�. ���������� ��� ��������� action=TRADE ��� ������� ������				

	std::vector<SplitTrade>::iterator splitIterator;
	std::vector<AggregatedTrade>::iterator aggrIterator;

	int currInstrumentID;               // ID ����������� � ������� ������ ��������

public:    

    typedef std::pair<Price, Price> PriceLimits;

    ~TradeSessionData()  {}

    // ���������� ������������ � ������
    void PushFutInstrument(FORTS_FUTINFO_REPL::fut_sess_settl& info);
    void PushOptInstrument(FORTS_OPTINFO_REPL::opt_sess_settl& info);
    void PushFutSessionContents(FORTS_FUTINFO_REPL::fut_sess_contents& info);
    void PushOptSessionContents(FORTS_OPTINFO_REPL::opt_sess_contents& info);

    // ��������� ������ ������ ������ ������� ���������
    void PushOrderLogAction(const FORTS_ORDLOG_REPL::orders_log& info);

    // ��������� ������ ������ ������ ������
    void PushDealAction(const FORTS_DEALS_REPL::deal& info);

    // ��������� ������� ��� ����������� ������: �������, ������...
    void ClearAllData();

    // ��������� ����� �� id �����������
    std::string GetIsinFromId(const InstrumentIDType ID) const { return isFutures(ID) ? futInfoMap.at(ID).isin : (isOption(ID) ? optInfoMap.at(ID).isin : ""); }

    PriceLimits GetPriceLimits(const InstrumentIDType ID);

    void GetAllIsins(std::set<InstrumentIDType>& isin_ids);

	const AggregatedTrade* GetLastTrade(const InstrumentIDType ID);
	bool isLastAggrTradeReady(const InstrumentIDType ID) const { return (lastAggrTradeComplete.find(ID) != lastAggrTradeComplete.end()) ? lastAggrTradeComplete.at(ID) : false; }

    bool isFutures(const InstrumentIDType ID) const { return futInfoMap.find(ID) != futInfoMap.end(); }
    bool isOption(const InstrumentIDType ID) const { return optInfoMap.find(ID) != optInfoMap.end(); }
	
	// �������� ��� ���� �� �����������
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

    // ��������� �������� ����� ������� � ���� ������
    std::string DumpBookToString(int currInstrumentID, int orderBookLevels);

    // ������� ��� �������� �������� ������� ������������� �������
    void StartEnumSplitTrades(const int currInstrumentID);
    bool GetNextSplitTrade(SplitTrade& trade, const int currInstrumentID);

    // ������� ��� �������� �������� ������� �������������� �������
    void StartEnumAggrTrades(const int currInstrumentID);
    bool GetNextAggrTrade(AggregatedTrade& trade, const int currInstrumentID);

    size_t GetAggrTradesCount(const int currInstrumentID) { return aggrTradesMap[currInstrumentID].size(); }

    void GetBestPrices(const InstrumentIDType ID, Price& bestBidPrice, Price& bestAskPrice);

private:

    TradeSessionData() : currInstrumentID(0) {}

    // 3 ������� ��� ��������� ���������� �������
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