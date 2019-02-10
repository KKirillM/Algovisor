#include "stdafx.h"
#include "TradeSessionData.h"

#define MODULE_NAME "TradeSessionData"


char getPriceLevelEventNumber(const P2Order& p2order, COrdersSetBase* orderSet)
{
    char priceLevelEvent = 1;

    std::unique_ptr<SOrder> incomeOrder(createSOrder(p2order.id_ord, Price(p2order.price.price_double, p2order.precision), p2order.quantity, p2order.timestamp));

    orderSet->Begin();
    SOrder* orderInBook = orderSet->GetOrder();
    Price bookPrice = orderInBook ? orderInBook->price : Price();

    if (!bookPrice.IsPrice())
    {
        return priceLevelEvent; 
    }

    if (orderSet->LowerPricesFirst())
    {
        for( ; !orderSet->Eof(); orderSet->Next())
        {
            if (bookPrice != orderSet->GetOrder()->price)
            {
                bookPrice = orderSet->GetOrder()->price;
                ++priceLevelEvent;
            }
            if (incomeOrder->price <= orderSet->GetOrder()->price)
            {
                return priceLevelEvent;
            }
        }
    }
    else
    {
        for( ; !orderSet->Eof(); orderSet->Next())
        {
            if (bookPrice != orderSet->GetOrder()->price)
            {
                bookPrice = orderSet->GetOrder()->price;
                ++priceLevelEvent;
            }
            if (incomeOrder->price >= orderSet->GetOrder()->price)
            {
                return priceLevelEvent;
            }
        }
    }

    return ++priceLevelEvent;
}

void TradeSessionData::AddOrder(const P2Order& p2order, COrdersBookBase* ordersBook)
{
    P2Order& oppositeOrder = temporaryOrders[currInstrumentID];

    if (oppositeOrder.quantity_rest != 0 && p2order != oppositeOrder) // добавл€ем в стакан остаток не сведЄнного ордера
    {
        AddOrder(oppositeOrder, ordersBook);
        oppositeOrder.quantity_rest = 0;
    }

    std::unique_ptr<SOrder> incomeOrder(createSOrder(p2order.id_ord, Price(p2order.price.price_double, p2order.precision), p2order.quantity_rest, p2order.timestamp));
    COrdersSetBase* askMatchSet = ordersBook->GetAskSet();
    COrdersSetBase* bidMatchSet = ordersBook->GetBidSet();
    COrdersSetBase* matchSet = NULL, *remainSet = NULL;

    if (p2order.direction == P2Dir::ASK)
    {
        matchSet = bidMatchSet;
        remainSet = askMatchSet;
    }
    else
    {
        matchSet = askMatchSet;
        remainSet = bidMatchSet;
    }

    matchSet->Begin();
    if (!matchSet->Eof() && matchSet->CheckPrice(incomeOrder.get())) // если ордер приводит к трейдам, то не ставим его в книгу, а прихраниваем
    {
        temporaryOrders[currInstrumentID] = p2order;
    }
    else
    {
        remainSet->Add(incomeOrder.release());
    }
}

void TradeSessionData::RemoveOrder(const P2Order& p2order, COrdersBookBase* ordersBook, const bool isCrossRemove)
{
    P2Order& oppositeOrder = temporaryOrders[currInstrumentID];

    if (isCrossRemove)
    {
        oppositeOrder.quantity_rest = 0;
        return;
    }

    if (oppositeOrder.quantity_rest != 0)   // добавл€ем в стакан остаток не сведЄнного ордера
    {
        AddOrder(oppositeOrder, ordersBook);
        oppositeOrder.quantity_rest = 0;
    }

    SOrder* order = NULL;
    if (p2order.direction == P2Dir::ASK)
    {
        order = ordersBook->DelAskOrder(p2order.id_ord);        
    }
    else
    {
        order = ordersBook->DelBidOrder(p2order.id_ord);
    }

    assert(order && order->mRemainedQuantity == abs(p2order.quantity));
    if (order == NULL)
    {
        std::string errorString;
        errorString = "Can't find order to remove, isin_id: " + std::to_string(currInstrumentID) + ", id_ord: " + std::to_string(p2order.id_ord);
        throw std::exception(errorString.c_str());
    }
    else if (order && order->mRemainedQuantity != abs(p2order.quantity))
    {
        std::string errorString;
        errorString = "Matching algorithm error: order size to remove not equal order size in book, isin_id: " + std::to_string(currInstrumentID) + 
                      ", id_ord: " + std::to_string(p2order.id_ord) + 
                      ", size to remove: " + std::to_string(abs(p2order.quantity)) + 
                      ", size in book: " + std::to_string(order->mRemainedQuantity);
        throw std::exception(errorString.c_str());
    }
}

void TradeSessionData::TradeOrder(const FORTS_ORDLOG_REPL::orders_log& info, const P2Order& p2order, COrdersBookBase* ordersBook)
{
    COrdersSetBase* askMatchSet = ordersBook->GetAskSet();
    COrdersSetBase* bidMatchSet = ordersBook->GetBidSet();
    COrdersSetBase* matchSet = NULL;   

    P2Order& oppositeOrder = temporaryOrders[currInstrumentID];
    
    if (oppositeOrder.id_ord == info.id_ord)
    {
		oppositeOrder.quantity_rest = info.amount_rest;	

        if (info.amount_rest == 0)
        {
            oppositeOrder.id_ord = 0;
        }

        AggregatedTrade* lastAggrTrade = aggrTradesMap[currInstrumentID].empty() ? NULL : &aggrTradesMap[currInstrumentID].back();

        if (lastAggrTrade == NULL || lastAggrTrade->ordID != info.id_ord)
        {
            matchSet = (info.dir == P2Dir::ASK) ? bidMatchSet : askMatchSet;

            matchSet->Begin();
            SOrder* matchSetOrder = matchSet->GetOrder();
            AggregatedTrade aggrTrade;
            aggrTrade.initialPrice = Price(p2order.price.price_double, p2order.precision);
            aggrTrade.startPrice = matchSetOrder->price;
            aggrTrade.endPrice = matchSetOrder->price;
            aggrTrade.initialSize = oppositeOrder.quantity;
            aggrTrade.matchSize = info.amount;
            aggrTrade.ordID = info.id_ord;
            aggrTrade.time = info.moment_ns;
            aggrTrade.agressorSide = (info.dir == P2Dir::ASK) ? 'S' : 'B';
            aggrTrade.oppositeOrderSize = matchSetOrder->mRemainedQuantity;

            Price firstPrice;
            int64_t cumulativeSize = 0;
            uint64_t maxPriceLevelOrderSize = 0;
            int64_t ordersCount = 0;
            int64_t biggestOrderTime = 0;
            firstPrice = matchSetOrder->price;
            while (matchSetOrder != NULL && firstPrice == matchSetOrder->price)
            {
                ++ordersCount;
                cumulativeSize += matchSetOrder->mRemainedQuantity;
                if (maxPriceLevelOrderSize < matchSetOrder->mRemainedQuantity)
                {
                    maxPriceLevelOrderSize = matchSetOrder->mRemainedQuantity;
                    biggestOrderTime = matchSetOrder->time;
                }
                matchSet->Next();
                matchSetOrder = matchSet->GetOrder();
            }
            aggrTrade.oppositeMaxOrderDeltaTime = info.moment_ns - biggestOrderTime;
            aggrTrade.priceLevelOrdersCount = ordersCount;
            aggrTrade.priceLevelCumulativeSize = cumulativeSize;
            aggrTrade.maxPriceLevelOrderSize = maxPriceLevelOrderSize;

            aggrTrade.totalBidSize = bidMatchSet->OrdersSize();      
            aggrTrade.totalAskSize = askMatchSet->OrdersSize();      
            aggrTrade.totalBidOrdersCount = bidMatchSet->OrdersCount();
            aggrTrade.totalAskOrdersCount = askMatchSet->OrdersCount();

            //aggrTrade.totalEventsCount = booksStat.GetData()[info.isin_id].eventsCountPerSession;
            //aggrTrade.spreadChangesCount = booksStat.GetData()[info.isin_id].spreadChangesCount; 
            aggrTrade.status = info.status;

            aggrTradesMap[currInstrumentID].push_back(aggrTrade);
			lastAggrTradeComplete[info.isin_id] = false;
        }
        else
        {
            lastAggrTrade->matchSize += info.amount;
            Price currentPrice(p2order.deal_price.price_double, p2order.precision);
            if (lastAggrTrade->endPrice != currentPrice)
            {
                ++lastAggrTrade->executedPriceLevelsCount;
                lastAggrTrade->endPrice = currentPrice;
            }
        }
    }
    else
    {
		AggregatedTrade& lastAggrTrade = aggrTradesMap[currInstrumentID].back();

        if (info.amount_rest == 0)
        {
            ordersBook->DelOrder(info.id_ord);
			++lastAggrTrade.oppositeOrdersAbsorb;
        }
        else
        {
            matchSet = (info.dir == P2Dir::ASK) ? askMatchSet : bidMatchSet;
            matchSet->Begin();
            SOrder* matchOrder = matchSet->GetOrder();
            assert(matchOrder && matchOrder->price == Price(p2order.price.price_double, p2order.precision));
            assert(matchOrder->mRemainedQuantity - info.amount == info.amount_rest);
            matchOrder->SetReducedResult(info.amount_rest);
        }

        SplitTrade splittedTrade;
        splittedTrade.price = Price(p2order.price.price_double, p2order.precision);
        splittedTrade.size = p2order.quantity;
        splittedTrade.time = info.moment_ns;
        splittedTrade.tradeID = info.id_deal;
        splittedTrade.ordID = info.id_ord;
        splittedTrade.agressorSide = (info.dir == P2Dir::ASK) ? 'B' : 'S';
        splitTradesMap[currInstrumentID].push_back(splittedTrade);

        lastAggrTrade.components.push_back(splittedTrade);
		if (oppositeOrder.id_ord == 0)
		{
			lastAggrTradeComplete[info.isin_id] = true;
		}
    }
}

void TradeSessionData::PushFutSessionContents(FORTS_FUTINFO_REPL::fut_sess_contents& info)
{
    futInfoMap[info.isin_id] = info;
    if (futInfoMap.find(info.isin_id) == futInfoMap.end())
    {   
        ordersBooks.PushOrdersBook(info.isin_id, (new COrdersBook())); 
    }

    // костыль
    optInfoMap.erase(info.isin_id);
}

void TradeSessionData::PushOptSessionContents(FORTS_OPTINFO_REPL::opt_sess_contents& info)
{
    optInfoMap[info.isin_id] = info;
    if (optInfoMap.find(info.isin_id) == optInfoMap.end())
    {
        ordersBooks.PushOrdersBook(info.isin_id, (new COrdersBook()));
    }    

    // костыль
    futInfoMap.erase(info.isin_id);
}


void TradeSessionData::PushFutInstrument(FORTS_FUTINFO_REPL::fut_sess_settl& info)
{
    if (futInfoMap.find(info.isin_id) == futInfoMap.end())
    {   
        futInfoMap[info.isin_id];
		ordersBooks.PushOrdersBook(info.isin_id, (new COrdersBook()));
    }

    // костыль
    optInfoMap.erase(info.isin_id);
}

void TradeSessionData::PushOptInstrument(FORTS_OPTINFO_REPL::opt_sess_settl& info)
{
    if (optInfoMap.find(info.isin_id) == optInfoMap.end())
    {
        optInfoMap[info.isin_id];
		ordersBooks.PushOrdersBook(info.isin_id, (new COrdersBook()));
    }

    // костыль
    futInfoMap.erase(info.isin_id);
}

void TradeSessionData::PushOrderLogAction(const FORTS_ORDLOG_REPL::orders_log& info)
{
    currInstrumentID = info.isin_id;

    double dPrice = 0, dDealPrice = 0;
    P2BCDII2Double(&dPrice, info.price);
    P2BCDII2Double(&dDealPrice, info.deal_price);

    /*bool isFut = isFutures(currInstrumentID);*/
    P2Order order(info, 4 /*isFut ? futInfoMap[currInstrumentID].roundto : optInfoMap[currInstrumentID].roundto*/, dPrice, dDealPrice);
    COrdersBookBase* ordersBook = NULL;

    // костыль
    try
    {
        ordersBook = ordersBooks.getOrdersBook(currInstrumentID);
    }
    catch (std::exception& e)
    {
        std::string what(e.what());
        if (what.find("Invalid InstrumentID") != std::string::npos)
        {
            futInfoMap[currInstrumentID];   // не можем узнать фьючер это или опцион поэтому добавл€ем сразу и туда и туда
            optInfoMap[currInstrumentID];
            ordersBooks.PushOrdersBook(currInstrumentID, (new COrdersBook()));
            ordersBook = ordersBooks.getOrdersBook(currInstrumentID);
        }
        else
        {
            throw e;
        }
    }
    
    if (info.action == P2Action::ADD)
    {
        AddOrder(order, ordersBook);
    }
    else if (info.action == P2Action::REMOVE)
    {
        RemoveOrder(order, ordersBook, (info.status & CROSS_TRADE_DELETE) != 0 ? true : false);
    }
    else if (info.action == P2Action::TRADE)
    {
        TradeOrder(info, order, ordersBook);
    }
}

void TradeSessionData::PushDealAction(const FORTS_DEALS_REPL::deal& info)
{
    currInstrumentID = info.isin_id;

    dealsCache[currInstrumentID].push_back(info);

    if (splitTradesMap[currInstrumentID].empty() || splitTradesMap[currInstrumentID].back().tradeID < info.id_deal)
    {
        return;
    }

    // устанавливаем значение кумул€тивного ќ» у всех раздробленных трейдов, данные о которых накопились, пока поток FORTS_DEALS_REPL обгон€л полный ордер лог
    ptrdiff_t indx = (ptrdiff_t)splitTradesMap[currInstrumentID].size() - 1;
    for (std::vector<FORTS_DEALS_REPL::deal>::reverse_iterator& it = dealsCache[currInstrumentID].rbegin(), end = dealsCache[currInstrumentID].rend(); it != end; --indx)
    {
        if (indx < 0) // indx может быть меньше 0, если массив storedDealsList содержит трейды за прошлую сессию
        {
            break;
        }

        SplitTrade& trade = splitTradesMap[currInstrumentID].at(indx);

        if (trade.tradeID == (*it).id_deal)
        {
            trade.cumulativeOI = it->pos;
            //booksStat.SetCummOI(currInstrumentID, it->pos);
            ++it;
        }
    }

    std::vector<FORTS_DEALS_REPL::deal>::iterator& it = dealsCache[currInstrumentID].begin(), end = dealsCache[currInstrumentID].end();
    const long long tradeID = aggrTradesMap[currInstrumentID].front().components.front().tradeID;
    while (it->id_deal < tradeID)
    {
        ++it;
        if (it == end)
        {
            --it;
            break;
        }
    }

    indx = (ptrdiff_t)aggrTradesMap[currInstrumentID].size() - 1;
    AggregatedTrade* aggrTrade = &aggrTradesMap[currInstrumentID].at(indx);
    while (it->id_deal < aggrTrade->components.front().tradeID)
    {
        aggrTrade = &aggrTradesMap[currInstrumentID].at(--indx);
    }

    // определ€ем направление изменени€ ќ» в трейде
    for (; it != end; ++it)
    {
        while (!aggrTrade->isTradeIDExist((*it).id_deal))
        {
            aggrTrade = &aggrTradesMap[currInstrumentID].at(++indx);
        }

        for (SplitTrade& splitTrade : aggrTrade->components)
        {
            if (splitTrade.tradeID == it->id_deal)
            {
                splitTrade.cumulativeOI = it->pos;
            }
        }

        if (aggrTrade->isAllIOFilled())
        {
            const int lastPos = aggrTrade->components.back().cumulativeOI;
            int prevPos = aggrTrade->components.front().cumulativeOI;
            if (prevPos == lastPos)
            {
                if (indx > 0 && !aggrTradesMap[currInstrumentID].at(indx-1).components.empty())
                {
                    prevPos = aggrTradesMap[currInstrumentID].at(indx-1).components.back().cumulativeOI;
                }
                else
                {
                    continue;
                }
            }

            const int deltaPos = lastPos - prevPos;

            if (deltaPos > 0) // определ€ем направление изменени€ ќ» дл€ агрегированного трейда
            {
                aggrTrade->IODir = OPEN_POSITION;
            }
            else if (deltaPos < 0)
            {
                aggrTrade->IODir = CLOSE_POSITION;
            }
            else if (deltaPos == 0)
            {
                aggrTrade->IODir = UNKNOWN_POSITION;
            }

            // определ€ем направление изменени€ ќ» дл€ не агрегированных трейдов
            for (SplitTrade& splitTrade : aggrTrade->components)
            {
                splitTrade.IODir = aggrTrade->IODir;
            }
        }  
    }

    dealsCache[currInstrumentID].clear();
    dealsCache[currInstrumentID].shrink_to_fit();
}

void TradeSessionData::ClearAllData()
{
    ordersBooks.ClearAllBooks();
    temporaryOrders.clear();
    splitTradesMap.clear();
    aggrTradesMap.clear();
	lastAggrTradeComplete.clear();
    dealsCache.clear();
    //futInfoMap.clear();
    //optInfoMap.clear();
}

TradeSessionData::PriceLimits TradeSessionData::GetPriceLimits(const InstrumentIDType ID)
{
    double limitUp = 0, limitDown = 0, old_kotir = 0;

    if (isFutures(ID))
    {
        P2BCDII2Double(&old_kotir, futInfoMap[ID].old_kotir);
        P2BCDII2Double(&limitUp, futInfoMap[ID].limit_up);
        P2BCDII2Double(&limitDown, futInfoMap[ID].limit_down);
    }
    else if (isOption(ID))
    {
        P2BCDII2Double(&old_kotir, futInfoMap[ID].old_kotir);
        P2BCDII2Double(&limitUp, optInfoMap[ID].limit_up);
        P2BCDII2Double(&limitDown, optInfoMap[ID].limit_down);
    }
    else
    {
        throw std::exception(("GetPriceLimits(): Invalid InstrumentID: " + to_string(ID)).c_str()); 
    }
    
    return std::make_pair(Price(old_kotir + limitUp, 4), Price(old_kotir - limitDown, 4));
}

void TradeSessionData::GetAllIsins(std::set<InstrumentIDType>& isin_ids)
{
    for (const auto& pair : futInfoMap)
    {
        isin_ids.insert(pair.first);
    }
    for (const auto& pair : optInfoMap)
    {
        isin_ids.insert(pair.first);
    }
}

const AggregatedTrade* TradeSessionData::GetLastTrade(const InstrumentIDType ID)
{
    return (aggrTradesMap[ID].size() != 0) ? &aggrTradesMap[ID].back() : NULL;
}

std::string TradeSessionData::DumpBookToString(int currInstrumentID, int orderBookLevels)
{
    COrdersBookBase* ordersBook = ordersBooks.getOrdersBook(currInstrumentID);
    COrdersSetBase* askSet = ordersBook->GetAskSet();
    COrdersSetBase* bidSet = ordersBook->GetBidSet();
    std::string askString = askSet->GetLogString(orderBookLevels);
    std::string bidString = bidSet->GetLogString(orderBookLevels);

    std::string str = "ORDER_BOOK; isin_id=" + std::to_string(currInstrumentID) + 
                      "; BID(" + std::to_string(bidSet->OrdersCount()) + "): " + bidString + 
                  ";     ASK(" + std::to_string(askSet->OrdersCount()) + "): " + askString; 

    return str;
}

void TradeSessionData::StartEnumSplitTrades(const int currInstrumentID)
{
    splitIterator = splitTradesMap[currInstrumentID].begin();
}

bool TradeSessionData::GetNextSplitTrade(SplitTrade& trade, const int currInstrumentID)
{
    if (splitIterator == splitTradesMap[currInstrumentID].end())
    {
        return false;
    }

    trade = *splitIterator;
    ++splitIterator;

    return true;
}

void TradeSessionData::StartEnumAggrTrades(const int currInstrumentID)
{
    aggrIterator = aggrTradesMap[currInstrumentID].begin();
}

bool TradeSessionData::GetNextAggrTrade(AggregatedTrade& trade, const int currInstrumentID)
{
    if (aggrIterator == aggrTradesMap[currInstrumentID].end())
    {
        return false;
    }

    trade = *aggrIterator;
    ++aggrIterator;

    return true;
}

void TradeSessionData::GetBestPrices(const InstrumentIDType ID, Price& bestBidPrice, Price& bestAskPrice)
{
    COrdersBookBase* ordersBook = ordersBooks.getOrdersBook(ID);
    COrdersSetBase* askMatchSet = ordersBook->GetAskSet();
    COrdersSetBase* bidMatchSet = ordersBook->GetBidSet();
    SOrder* order = NULL;

    askMatchSet->Begin(); bidMatchSet->Begin();
    
    order = bidMatchSet->GetOrder();
	if (order)
	{
		bestBidPrice = order->price;
	}

    order = askMatchSet->GetOrder();
	if (order)
	{
		bestAskPrice = order->price;
	}
}
