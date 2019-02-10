#include "StdAfx.h"
#include "MatchingData.h"
#include <chrono>

void* SOrder::free_list=0;
int SOrder::counter = 0;
std::vector<void*> SOrder::newPointerList;


COrdersBook::COrdersBook() : COrdersBookBase()
{
    askSet.OrderBookSet<BestSell>::SetOrdersBook( this );
    bidSet.OrderBookSet<BestBuy>::SetOrdersBook( this );
}

COrdersBookBase::~COrdersBookBase()
{
    for( AllIdSet::iterator it = idSet.begin(); it != idSet.end(); it++ )
    {
        delete it->Order;
    }
}

SOrder* COrdersBookBase::DelOrder(t_id ordNum)
{
    AllIdSet::iterator it = idSet.find(OrderIDType(ordNum));
    if(it != idSet.end())
    {
        SOrder* order = it->Order;
        idSet.Erase(order);
        GetAskSet()->Erase(order);
        GetBidSet()->Erase(order);      
        return order;
    }
    return 0;
}

SOrder* COrdersBookBase::DelBidOrder(t_id ordNum)
{
    AllIdSet::iterator it = idSet.find(OrderIDType(ordNum));
    if(it != idSet.end())
    {
        SOrder* order = it->Order;
        idSet.Erase(order);
        GetBidSet()->Erase(order);              
        return order;
    }
    return 0;
}

SOrder* COrdersBookBase::DelAskOrder(t_id ordNum)
{
    AllIdSet::iterator it = idSet.find(OrderIDType(ordNum));
    if(it != idSet.end())
    {
        SOrder* order = it->Order;
        idSet.Erase(order);
        GetAskSet()->Erase(order);
        return order;
    }
    return 0;
}

//-------------------------------------------------------
void COrdersBookBase::InsertOrderIntoIndexes(SOrder* order)
{
    idSet.Insert(order);
}

void COrdersBookBase::ClearBook()
{
    for (AllIdSet::iterator it = idSet.begin(), end = idSet.end(); it != end; ++it)
    {
        SOrder* ret = it->Order;
        delete it->Order;
    }

    idSet.clear();
    GetBidSet()->Clear();
    GetAskSet()->Clear();
}

void SOrder::reduce(unsigned int volume )
{
    if( mRemainedQuantity > 0 )
    {
        mRemainedQuantity -= volume;	
    } 
    else 
    {
        mRemainedQuantity += volume;
    }
}

void SOrder::SetReducedResult(unsigned int volume)
{
    mRemainedQuantity = volume; // <=> mRemainedQuantity := mRemainedQuantity +/- volume 
};

unsigned int SOrder::getReducedResult(unsigned int volume) const
{
    if(mRemainedQuantity > 0)
    {
        return mRemainedQuantity - volume;	
    }
    else
    {
        return mRemainedQuantity + volume;
    }
}

CMatchingData::CMatchingData()
{
    m_Iter = m_ordersBookMap.end();
}

CMatchingData::~CMatchingData()
{
}
//----------------------------------------------------------------------------
COrdersBookBase* CMatchingData::getOrdersBook(InstrumentIDType InstrumentID)
{ 
    OrderBooksMap::iterator it = m_ordersBookMap.find(InstrumentID);

    if(it == m_ordersBookMap.end())
    {
        throw std::exception(("Invalid InstrumentID:" + to_string(InstrumentID)).c_str()); 
    }

    return it->second.get();
}
//-------------------------------------------------------------------------------
void CMatchingData::PushOrdersBook(InstrumentIDType InstrumentID, COrdersBookBase* orderBookBase)
{
    m_ordersBookMap[InstrumentID] = std::shared_ptr<COrdersBookBase>(orderBookBase);
}

void CMatchingData::PopOrdersBook(InstrumentIDType InstrumentID)
{
    OrderBooksMap::iterator it = m_ordersBookMap.find(InstrumentID);
    
    if(it == m_ordersBookMap.end())
    {
        throw std::exception("Invalid InstrumentID. Can't find orders book."); 
    }

    m_ordersBookMap.erase(it);
}

void CMatchingData::ClearAllBooks()
{
    for (auto& it : m_ordersBookMap)
    {
        it.second->ClearBook();
    }

    SOrder::DeleteAllSOrder();
}

//-------------------------------------------------------------------------------
SOrder* createSOrder(t_id _orderID, const Price& _price, unsigned int _quantity, int64_t _time)
{
    return (new SOrder(_orderID, _price, _quantity, _time));
}

void AggregatedBook::setPriceSets(COrdersSetBase* bidSet, COrdersSetBase* askSet, int numberOfPriceLevels, __int64 moment)
{
    timestamp = moment;

    bidPriceSet.clear();
    askPriceSet.clear();

    bidSet->GetOrdersRange(bidPriceSet, numberOfPriceLevels);
    askSet->GetOrdersRange(askPriceSet, numberOfPriceLevels);
}

std::string AggregatedBook::GetBidString() const
{
    char str[128] = {0};
    std::string logString;

    for (auto it = bidPriceSet.rbegin(), end = bidPriceSet.rend(); it != end; ++it)
    {
        sprintf_s(str, sizeof(str), "%s(%d/%d) ", it->price.GetString().c_str(), it->orders_size, it->orders_count);
        logString += str;
    }

    return logString;
}

std::string AggregatedBook::GetAskString() const
{
    char str[128] = {0};
    std::string logString;

    for (auto it = askPriceSet.begin(), end = askPriceSet.end(); it != end; ++it)
    {
        sprintf_s(str, sizeof(str), "%s(%d/%d) ", it->price.GetString().c_str(), it->orders_size, it->orders_count);
        logString += str;
    }

    return logString;
}

std::string AggregatedBook::GetTradeString() const
{
    std::string logString;
    
    if (tradeQuantity > 0)
    {
        logString = tradePrice.GetString() + '(' + std::to_string(tradeQuantity) + ')' + '\'' + agressorSide + '\'';
    }

    return logString;
}

std::string AggregatedBook::GetInfoString() const
{
    std::string logString;

    char buf[128];

    std::chrono::minutes timezoneShift = std::chrono::minutes(0);
    TIME_ZONE_INFORMATION tz_info;
    GetTimeZoneInformation(&tz_info);
    timezoneShift = std::chrono::minutes(-tz_info.Bias);

    std::time_t ttt = (timestamp + std::chrono::duration_cast<std::chrono::nanoseconds>(timezoneShift).count())
        / std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();
    std::tm tm;
    gmtime_s(&tm, &ttt);
    /*msec = static_cast<UINT16>
    ((value % std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count())
    / std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::milliseconds(1)).count());
    second = static_cast<UINT8>(tm.tm_sec);
    minute = static_cast<UINT8>(tm.tm_min);
    hour = static_cast<UINT8>(tm.tm_hour);
    day = static_cast<UINT8>(tm.tm_mday);
    month = static_cast<UINT8>(tm.tm_mon) + 1;
    year = static_cast<UINT16>(tm.tm_year) + 1900;*/

    sprintf_s(buf, sizeof(buf), "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
    logString += buf;

    std::time_t tt = timestamp / std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();
    gmtime_s(&tm, &tt);

    int64_t nmec = timestamp % std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();
    /*int64_t datatime =
    ((uint64_t)(tm.tm_sec)
    + (uint64_t)(tm.tm_min)         * 100
    + (uint64_t)(tm.tm_hour)        * 100 * 100
    + (uint64_t)(tm.tm_mday)        * 100 * 100 * 100
    + (uint64_t)(tm.tm_mon + 1)     * 100 * 100 * 100 * 100
    + (uint64_t)(tm.tm_year + 1900) * 100 * 100 * 100 * 100 * 100);
    sprintf_s(buf, sizeof(buf), "%14.14I64u.%09.9I64u", datatime, nmec);*/

    sprintf_s(buf, sizeof(buf), ".%09lld", nmec);
    logString += buf;

    if (bidPriceLevelEvent > 0)
    {
        logString += " B:" + std::to_string(bidPriceLevelEvent);
    }

    if (askPriceLevelEvent > 0)
    {
        logString += " A:" + std::to_string(askPriceLevelEvent);
    }

    return logString;
}
