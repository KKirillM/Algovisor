#include "stdafx.h"
#include "Logger.h"
#include "..\Plaza2_Connector\Plaza2Objects.h"
#include "Plaza2Instruments.h"
#include <minwindef.h>
#include "Plaza2Callbacks.h"

#define NUMBER_OF_BOOK_LEVELS 10
#define BOOK_LEVELS_TO_LOG 2
#define NEED_LOG

#ifdef DEBUG && NEED_LOG

static FileLogger logger(".\\log", "Plaza2_Callbacks.txt");
#define DEBUG_LOG(message, ...) PRINT_TO_LOG(logger, LOG_DEBUG, message, __VA_ARGS__)

// ������ ������ ������ �� ��������������
#define NON_AGGR_ORDERBOOK_LOG(bookLevels) \
    COrdersSetBase* askSet = ordersBook->GetAskSet(); \
    COrdersSetBase* bidSet = ordersBook->GetBidSet(); \
    std::string askString = askSet->GetLogString(bookLevels); \
    std::string bidString = bidSet->GetLogString(bookLevels); \
    DEBUG_LOG("ORDERBOOK: BID(%d): %s; ASK(%d): %s", bidSet->OrdersCount(), bidString.c_str(), askSet->OrdersCount(), askString.c_str())

// �������������� �� ����� ������ ������ � ������
#define AGGR_ORDER_BOOK_LOG \
    size_t book_size = instrument.BookSize(); \
    Plaza2Book& aggr_book = instrument.GetBook(book_size-1); \
    std::string aggrAskString = aggr_book.GetAskString(); \
    std::string aggrBidString = aggr_book.GetBidString(); \
    std::string tradeString = aggr_book.GetTradeString(); \
    std::string infoString = aggr_book.GetInfoString(); \
    DEBUG_LOG("AGGR_ORDERBOOK: INFO: %s; BID: %s; T: %s; ASK: %s", infoString.c_str(), aggrBidString.c_str(), tradeString.c_str(), aggrAskString.c_str())


// ���������� �������� ������� ��������� � �������� �������, �.�. ������ ��������� ��������� ����� (������� ��������� 10 �������)
#define TRADES_ARRAY_LOG \
    for (auto size = instrument.NonAggregatedTradesArraySize(), indx = size-1, i = 0ULL; size > 0 && indx < size && i < 10; --indx, ++i) \
    { \
        std::string trade = instrument.GetNonAggregatedTrade(indx).GetString(); \
        DEBUG_LOG("NONAGGR_TRADE: %s", trade.c_str()) \
    } \
    for (auto size = instrument.AggregatedTradesArraySize(), indx = size-1, i = 0ULL; indx < size && i < 10; --indx, ++i) \
    { \
        std::string trade = instrument.GetAggregatedTrade(indx).GetString(); \
        DEBUG_LOG("AGGR_TRADE: %s", trade.c_str()) \
    }

#else

#define DEBUG_LOG(message, ...)
#define NON_AGGR_ORDERBOOK_LOG(bookLevels)
#define AGGR_ORDER_BOOK_LOG
#define TRADES_ARRAY_LOG

#endif


inline const string getOrderStatusString(int status)
{
    string res;

    if (status & QUOTE) res += "QUOTE ";
    else if (status & OPPOSITE) res += "OPPOSITE ";
    else if (status & FILL_OR_KILL) res += "FILL_OR_KILL ";
    
    if (status & NON_SYSTEMIC) res += "NON_SYSTEMIC ";
    if (status & LAST_IN_RANSACTION) res += "LAST_IN_RANSACTION ";
    if (status & CROSS_TRADE_DELETE) res += "CROSS_TRADE_DELETE ";
    if (status & MONEY_DONT_CHECK) res += "MONEY_DONT_CHECK ";

    if (status & ORDER_MOVE) res += "ORDER_MOVE ";
    if (status & ORDER_DELETE) res += "ORDER_DELETE ";
    if (status & GROUP_DELETE) res += "GROUP_DELETE ";

    return res;
}

CMatchingData Plaza2Callbacks::mMatchingData;
Plaza2Callbacks::DealsMap Plaza2Callbacks::mTradeList;
Plaza2Callbacks::OppositeOrderMap Plaza2Callbacks::mOppositeOrders;
//Plaza2Callbacks::StateMap Plaza2Callbacks::isOnlineFlags;

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

CG_RESULT AddOrder(const P2Order& p2order, COrdersBookBase* ordersBook, Plaza2Instrument& instrument)
{
    P2Order& oppositeOrder = Plaza2Callbacks::mOppositeOrders[instrument.Info().isin_id];

    if (oppositeOrder.quantity_rest != 0 && p2order != oppositeOrder) // ��������� � ������ ������� �� ��������� ������
    {
        AddOrder(oppositeOrder, ordersBook, instrument);
        oppositeOrder.quantity_rest = 0;
    }

    AggregatedBook aggrBook;
    std::unique_ptr<SOrder> incomeOrder(createSOrder(p2order.id_ord, Price(p2order.price.price_double, p2order.precision), p2order.quantity_rest, p2order.timestamp));
    COrdersSetBase* askMatchSet = ordersBook->GetAskSet();
    COrdersSetBase* bidMatchSet = ordersBook->GetBidSet();
    COrdersSetBase* matchSet = NULL, *remainSet = NULL;

    if (p2order.direction == P2Dir::ASK)
    {
        matchSet = bidMatchSet;
        remainSet = askMatchSet;
        char priceLevelEvent = getPriceLevelEventNumber(p2order, remainSet);
        aggrBook.setEventInfo(0, priceLevelEvent, 'a', p2order.quantity_rest);
    }
    else
    {
        matchSet = askMatchSet;
        remainSet = bidMatchSet;
        char priceLevelEvent = getPriceLevelEventNumber(p2order, remainSet);
        aggrBook.setEventInfo(priceLevelEvent, 0, 'b', p2order.quantity_rest);
    }

	matchSet->Begin();
    if (!matchSet->Eof() && matchSet->CheckPrice(incomeOrder.get())) // ���� ����� �������� � �������, �� �� ������ ��� � �����, � ������������
    {
        Plaza2Callbacks::mOppositeOrders[instrument.Info().isin_id] = p2order;
        return CG_ERR_OK;
    }
    else
    {
        remainSet->Add(incomeOrder.release());
    }

    askMatchSet->Begin();
    bidMatchSet->Begin();
    aggrBook.setPriceSets(bidMatchSet, askMatchSet, NUMBER_OF_BOOK_LEVELS, p2order.timestamp);
    instrument.AddBook(aggrBook);

	NON_AGGR_ORDERBOOK_LOG(BOOK_LEVELS_TO_LOG)
	//AGGR_ORDER_BOOK_LOG
		
	return CG_ERR_OK;
}

CG_RESULT RemoveOrder(const P2Order& p2order, COrdersBookBase* ordersBook, Plaza2Instrument& instrument, const bool isCrossRemove)
{
    P2Order& oppositeOrder = Plaza2Callbacks::mOppositeOrders[instrument.Info().isin_id];

    if (isCrossRemove)
    {
        oppositeOrder.quantity_rest = 0;
		return CG_ERR_OK;
    }

    if (oppositeOrder.quantity_rest != 0)   // ��������� � ������ ������� �� ��������� ������
    {
        AddOrder(oppositeOrder, ordersBook, instrument);
        oppositeOrder.quantity_rest = 0;
    }

    AggregatedBook aggrBook;
    COrdersSetBase* askMatchSet = ordersBook->GetAskSet();
    COrdersSetBase* bidMatchSet = ordersBook->GetBidSet();

    if (p2order.direction == P2Dir::ASK)
    {
        ordersBook->DelAskOrder(p2order.id_ord);
        char priceLevelEvent = getPriceLevelEventNumber(p2order, askMatchSet);
        aggrBook.setEventInfo(0, priceLevelEvent, 'a', p2order.quantity);
    }
    else
    {
        ordersBook->DelBidOrder(p2order.id_ord);
        char priceLevelEvent = getPriceLevelEventNumber(p2order, bidMatchSet);
        aggrBook.setEventInfo(priceLevelEvent, 0, 'b', p2order.quantity);
    }

    askMatchSet->Begin();
    bidMatchSet->Begin();
    aggrBook.setPriceSets(bidMatchSet, askMatchSet, NUMBER_OF_BOOK_LEVELS, p2order.timestamp);
    instrument.AddBook(aggrBook);

    NON_AGGR_ORDERBOOK_LOG(BOOK_LEVELS_TO_LOG)
    //AGGR_ORDER_BOOK_LOG

	return CG_ERR_OK;
}

CG_RESULT TradeOrder(const FORTS_ORDLOG_REPL::orders_log& info, const P2Order& p2order, COrdersBookBase* ordersBook, Plaza2Instrument& instrument)
{
    COrdersSetBase* askMatchSet = ordersBook->GetAskSet();
    COrdersSetBase* bidMatchSet = ordersBook->GetBidSet();
    COrdersSetBase* matchSet = NULL;   
    
    P2Order& oppositeOrder = Plaza2Callbacks::mOppositeOrders[info.isin_id];
    if (oppositeOrder.id_ord == info.id_ord)
    {
        oppositeOrder.quantity_rest = info.amount_rest;
        
        Plaza2AggregatedTrade& lastAggrTrade = instrument.GetLastAggregatedTrade();

        if (lastAggrTrade.ord_id != info.id_ord)
        {
            if (info.dir == P2Dir::ASK)
            {
                matchSet = bidMatchSet;
            }
            else
            {
                matchSet = askMatchSet;
            }

            matchSet->Begin();

            Plaza2AggregatedTrade aggrTrade;
            aggrTrade.initialQuantity = oppositeOrder.quantity;
            aggrTrade.ord_id = oppositeOrder.id_ord;
            aggrTrade.agressorSide = (info.dir == P2Dir::ASK) ? 'S' : 'B';
            aggrTrade.timestamp = info.moment_ns;
            aggrTrade.deal_id = info.id_deal;
            aggrTrade.dealIDArray.push_back(info.id_deal);
            aggrTrade.price = matchSet->GetOrder()->price;
            aggrTrade.size = info.amount;
            instrument.AddAggregatedTrade(aggrTrade);
        }
        else
        {
            lastAggrTrade.size += info.amount;
            lastAggrTrade.dealIDArray.push_back(info.id_deal);
        }

        if (info.amount_rest == 0)
        {
            oppositeOrder.id_ord = 0;
        }

        //TRADES_ARRAY_LOG

		return CG_ERR_OK;
    }

    AggregatedBook aggrBook;
	Plaza2Book& p2book = instrument.GetBook(instrument.BookSize() - 1);
	unsigned int sizeBeforeTrade = 0;
    if (info.dir == P2Dir::ASK)
    {
        matchSet = askMatchSet;
        aggrBook.agressorSide = 'B';
		sizeBeforeTrade = p2book.asks[0].size;
    }
    else
    {
        matchSet = bidMatchSet;
        aggrBook.agressorSide = 'S';
		sizeBeforeTrade = p2book.bids[0].size;
    }

    matchSet->Begin();
    SOrder* matchOrder = matchSet->GetOrder();
	if (matchOrder && matchOrder->price != Price(p2order.price.price_double, p2order.precision))
	{
		assert(matchOrder->price == Price(p2order.price.price_double, p2order.precision));
		return CG_ERR_INTERNAL;
	}

    aggrBook.setTrade(Price(p2order.price.price_double, p2order.precision), p2order.quantity, sizeBeforeTrade);
    if (info.amount_rest == 0)
    {
        ordersBook->DelOrder(info.id_ord);
    }
    else
    {
        matchOrder->SetReducedResult(info.amount_rest);
    }

    askMatchSet->Begin();
    bidMatchSet->Begin();
    aggrBook.setPriceSets(bidMatchSet, askMatchSet, NUMBER_OF_BOOK_LEVELS, p2order.timestamp);

    Plaza2Trade trade;
    trade.SetTrade(aggrBook.tradePrice, aggrBook.tradeQuantity, aggrBook.sizeBeforeTrade, info.id_ord, info.id_deal, aggrBook.agressorSide, aggrBook.timestamp);
    instrument.AddNonAggregatedTrade(trade);

	if (instrument.GetLastAggregatedTrade().dealIDArray.back() != info.id_deal) // �������� �� ������
	{
		assert(instrument.GetLastAggregatedTrade().dealIDArray.back() == info.id_deal);
		return CG_ERR_INTERNAL;
	}

    Plaza2Book book(aggrBook);
    book.trade = &instrument.GetLastNonAggrTrade();
    instrument.AddBook(book);

    NON_AGGR_ORDERBOOK_LOG(BOOK_LEVELS_TO_LOG)
    //AGGR_ORDER_BOOK_LOG
    //TRADES_ARRAY_LOG

	return CG_ERR_OK;
}

const FORTS_ORDLOG_REPL::orders_log ResolveMessageType(const msg_streamdata_t* data, const int isin)
{
    if (data->msg_index == 2)
    {
        return *reinterpret_cast<FORTS_ORDLOG_REPL::orders_log*>(data->data);
    }
    else if (data->msg_index == 0)
    {
        const FORTS_ORDBOOK_REPL::orders& info = *reinterpret_cast<FORTS_ORDBOOK_REPL::orders*>(data->data);
        FORTS_ORDLOG_REPL::orders_log message;
        message.isin_id = info.isin_id;
       
        if (isin == info.isin_id)
        {
			message.replRev = info.replRev;
            message.id_ord = info.id_ord;
            message.sess_id = info.sess_id;
            message.amount = info.amount;
            message.amount_rest = info.amount_rest;
            message.id_deal = info.id_deal;
            message.status = info.status;
            memcpy(message.price, info.price, 11);
            message.moment = info.moment;
            message.moment_ns = info.moment_ns;
            message.dir = info.dir;
			message.action = P2Action::ADD;
            memcpy(message.deal_price, info.deal_price, 11);
        }
        
        return message;
    }
    else
    {
        FORTS_ORDLOG_REPL::orders_log message;
        message.isin_id = 0;

        return message;
    }
}

CG_RESULT Plaza2Callbacks::OrderBook(msg_streamdata_t* data, IListener* listener)
{
	using namespace FORTS_ORDLOG_REPL;

	const orders_log& info = ResolveMessageType(data, listener->GetIsin());

	if (listener->GetIsin() != info.isin_id)    // �� ��������� ������ �������� ������ ��� ���� ������������ �����, � ��� ����� ������ �� ����, ������� � �������� ��������
	{
        return CG_ERR_OK;
    }

    const int instrumentKey = info.isin_id;

    P2Order order(info, listener->GetPrecision());
	DEBUG_LOG("ORDLOG; rev: %lld; date=%02d-%02d-%04d; time=%02d:%02d:%02d.%03d; nanosec=%llu; sess_id=%d; isin_id=%d; id_ord=%lld; action=%6s; dir=%s; "
                "price=%f; amount=%d; deal_price=%f; amount_rest=%d; id_deal=%lld; xstatus=0x%llx; status=0x%x; Status=%s",
				info.replRev,
				info.moment.day,                // ����
                info.moment.month,
                info.moment.year,
                info.moment.hour,               // �����
                info.moment.minute,
                info.moment.second,
                info.moment.msec,
                info.moment_ns,
                info.sess_id,                   // ������������� �������� ������
                info.isin_id,                   // ���������� �������� ������������� �����������
                info.id_ord,                    // ����� ������
                ((info.action == P2Action::REMOVE) ? "REMOVE" : (info.action == P2Action::ADD) ? "ADD" : "TRADE"),  // �������� � �������
                (info.dir == P2Dir::ASK ? "Ask" : "Bid"),   // �����������
                order.price.price_double,       // ����
                info.amount,                    // ���������� � ��������
                order.deal_price.price_double,  // ���� ����������� ������
                info.amount_rest,               // ���������� ���������� � ������ (� ����� ������� �����)
                info.id_deal,                   // ������������� ������ �� ������ ������ ������� ������
                info.xstatus,                   // ����������� ������ ������
                info.status,                    // ������ ������
                getOrderStatusString(info.status).c_str())

    if (info.status & NON_SYSTEMIC)   // ������������(��������) ������ ���� �� �������
    {
        return CG_ERR_OK;
    }

	Plaza2Instrument& instrument = *Plaza2DB::GetInstance()->GetInstrumentPtrByKey(instrumentKey);
	COrdersBookBase* ordersBook = mMatchingData.getOrdersBook(instrumentKey);
            
    // ���� action=ADD, �� ���� amount � amount_rest �������� ���������� ��������
    // ���� action=REMOVE, �� ���� amount ��� ����� ���� ���������� ������, � amount_rest ����� 0 ������
    // ���� action=TRADE, �� amount_rest �������� ���������� ���� ���������� ������, ������� �������� �� ������ ��������,
    //  �������������� �� �� ����� ����� ����, ������ ���� ����� �������� � ������ ������� ����
        
    if (!listener->IsCurrentSession(info.sess_id))    // � ������������ ����� ������ ������ ����������� ������
    {
		DEBUG_LOG("Orders count in book at the end of session: %d, Isin_id: %d", ordersBook->Size(), instrumentKey)
		DEBUG_LOG("Non aggregation trades count at the end of session: %d, Isin_id: %d", instrument.NonAggregatedTradesArraySize(), instrumentKey)
		DEBUG_LOG("Aggregation trades count at the end of session: %d, Isin_id: %d", instrument.AggregatedTradesArraySize(), instrumentKey)

		Plaza2Callbacks::ClearOrderBookBuffers(instrumentKey);
		Plaza2Callbacks::ClearDealsBuffers(instrumentKey);
        //instrumentDB.ClearNonAggrTradesArray(instrumentKey);
        //instrumentDB.ClearAggrTradesArray(instrumentKey);
    }

	CG_RESULT res = CG_ERR_OK;

    if (info.action == P2Action::ADD)
    {
		res = AddOrder(order, ordersBook, instrument);
    }
    else if (info.action == P2Action::REMOVE)
    {
		res = RemoveOrder(order, ordersBook, instrument, (info.status & CROSS_TRADE_DELETE) != 0 ? true : false);
    }
    else if (info.action == P2Action::TRADE)
    {
		res = TradeOrder(info, order, ordersBook, instrument);
    }

	return res;
}

// ����� ������ ������ ����� �������� ����� ���������, �.�. � ������ ������ ����� ��������� �����, �������� ��� ���� � ������ ���������
// � ����� � ����, ���������� ����������� ������ ������ ������ � ���������������� �� � ���������� � ������� ����� ����
//
// ����� ������ ������� ������������ � ������ �������� ������� ������, � ������ � �������� ������� ��� ��������
// ��� �������������� ������ �� ��������� �������������� �������, �.�. �������� �������� ����� ������ ������ ������
// ����� ������ �� �������� � ��������� ��������� ��������
CG_RESULT Plaza2Callbacks::Trade(msg_streamdata_t* data, IListener* listener)
{
	using FORTS_DEALS_REPL::deal;

	deal& info = *(deal*)data->data;

    if (info.nosystem != 0)   // ������������(��������) ������ ���� �� �������
    {
        return CG_ERR_OK;
    }
	
	if (listener->GetIsin() != info.isin_id)
	{
        return CG_ERR_OK;
    }

#ifdef DEBUG
    Plaza2Trade log_trade;
    log_trade.SetTrade(Price(P2Price(info.price).price_double, listener->GetPrecision()), info.amount, 0, 0, 0, '?', info.moment_ns);
    DEBUG_LOG("DEAL; rev: %lld; time=%02d:%02d:%02d.%03d; nanosec=%llu; sess_id=%d; isin_id=%d; id_deal=%lld; pos=%d; price=%s; amount=%d; id_ord_buy=%lld; id_ord_sell=%lld; nosystem=%d",
                info.replRev,
				info.moment.hour,               // �����
                info.moment.minute,
                info.moment.second,
                info.moment.msec,
                info.moment_ns,
                info.sess_id,                   // ������������� �������� ������
                info.isin_id,                   // ���������� �������� ������������� �����������
                info.id_deal,                   // ����� ������
                info.pos,                       // ���-�� ������� �� ����������� �� ����� ����� ������
                log_trade.price.GetString().c_str(),// ����
                info.amount,                    // �����, ���-�� ������ �����������
                info.id_ord_buy,                // ����� ������ ����������
                info.id_ord_sell,               // ����� ������ ��������
                info.nosystem)                  // ������� ������������ ������
#endif

	if (!listener->IsCurrentSession(info.sess_id))    // � ������������ ����� ������ ������ ����������� ������
    {
		mTradeList[info.isin_id].clear();
    }

    // �������� ������ ������ ����������� ������ ������:
    // - � ������� ��������������� ������� � ����������� ��� �������� �������� ��������� �������� ��� ������� ������,
    // ������ ���������� ���������� �� ��������� id ������;
    // - ����� ���� ��� ������ ��������� �������� ��������� �������� ���������� ����������� ������ ��� ��������������� ������ 
    // � �� �������������� �������

    mTradeList[info.isin_id].push_back(info);

    const int instrumentKey = info.isin_id;
    Plaza2Instrument& instrument = *Plaza2DB::GetInstance()->GetInstrumentPtrByKey(instrumentKey);

    if (!instrument.FindNonAggrTradeID(info.id_deal))
    {
        return CG_ERR_OK;
    }

    //TRADES_ARRAY_LOG

    size_t indx = instrument.NonAggregatedTradesArraySize() - 1;
    for (auto it = mTradeList[info.isin_id].rbegin(), end = mTradeList[info.isin_id].rend(); it != end; --indx)
    {
        Plaza2Trade& trade = instrument.GetNonAggregatedTrade(indx);

        if (trade.deal_id == (*it).id_deal)
        {
            trade.interest = it->pos;
            ++it;
        }

		if (indx == 0)
		{
			break;
		}
    }

	auto tmp = instrument.AggregatedTradesArraySize();
    indx = tmp == 0 ? 0 : tmp - 1;
    Plaza2AggregatedTrade* aggrTrade = &instrument.GetAggregatedTrade(indx);
    long long id_deal = mTradeList[info.isin_id].begin()->id_deal;
    while (std::find(aggrTrade->dealIDArray.begin(), aggrTrade->dealIDArray.end(), id_deal) == aggrTrade->dealIDArray.end())
    {
        aggrTrade = &instrument.GetAggregatedTrade(--indx);
    }

    for (auto it = mTradeList[info.isin_id].begin(), end = mTradeList[info.isin_id].end(); it != end; ++it)
    {
        while (std::find(aggrTrade->dealIDArray.begin(), aggrTrade->dealIDArray.end(), (*it).id_deal) == aggrTrade->dealIDArray.end())
        {
            aggrTrade = &instrument.GetAggregatedTrade(++indx);
        }

        aggrTrade->posArray.push_back(it->pos);

        if (aggrTrade->posArray.size() == aggrTrade->dealIDArray.size())
        {
            aggrTrade->interest = aggrTrade->posArray.back();
            int lastPos = aggrTrade->posArray.back();
            int prevPos = *aggrTrade->posArray.begin();
            if (prevPos == lastPos)
            {
                std::vector<int>& posArray = instrument.GetAggregatedTrade(indx != 0 ? indx-1 : 0).posArray;
                if (!posArray.empty())
                {
                    prevPos = posArray.back();
                }
                else
                {
                    continue;
                }
            }
                
            int deltaPos = lastPos - prevPos;

            if (deltaPos > 0) // ���������� ������� ������ ��� ��������������� ������
            {
                aggrTrade->interestDir = OPEN_POSITION;
            }
            else if (deltaPos < 0)
            {
                aggrTrade->interestDir = CLOSE_POSITION;
            }

            // ���������� ������� ������ ��� �� �������������� �������
            if (aggrTrade->interestDir != UNDEFINED)
            {
                for (const auto id : aggrTrade->dealIDArray)
                {
                    Plaza2Trade* trade = instrument.FindNonAggrTrade(id);
                    if (trade)
                    {
                        trade->interestDir = aggrTrade->interestDir;
                    }
                }
            }
        }  
    }

    mTradeList[info.isin_id].clear();

    //TRADES_ARRAY_LOG
        
	return CG_ERR_OK;
}

CG_RESULT Plaza2Callbacks::BBO(msg_streamdata_t* data, IListener* listener)
{
    using FORTS_FUTCOMMON_REPL::common;

    common& info = *(common*)data->data;

    if (listener->GetIsin() == info.isin_id)
    {	
        Price bestBid(P2Price(info.best_buy).price_double, listener->GetPrecision());
        Price bestAsk(P2Price(info.best_sell).price_double, listener->GetPrecision());

        DEBUG_LOG("BBO; time=%02d:%02d:%02d.%03d; sess_id=%d; isin_id=%d; best_buy=%s; amount_buy=%d, best_sell=%s, amount_sell=%d",
            info.mod_time.hour,             // �����
            info.mod_time.minute,
            info.mod_time.second,
            info.mod_time.msec,
            info.sess_id,                   // ������������� �������� ������
            info.isin_id,                   // ���������� �������� ������������� �����������
            bestBid.GetString().c_str(),    // ���� ������ ������ �� �������
            info.amount_buy,                // i4 ���������� � ������� �� ������� � ������ �����
            bestAsk.GetString().c_str(),    // d16.5 ���� ������ ������ �� �������
            info.amount_sell)               // i4 ���������� � ������� �� ������� � ������ �����
    }

    /*price d16.5 ���� ��������� ������
    trend d16.5 ����� ��������� ���� (������� ����� ������ ����
    ��������� ������)
    amount i4 ���������� � ��������� ������
    deal_time t ���� � ����� ��������� ������
    min_price d16.5 ����������� ����
    max_price d16.5 ������������ ����
    avr_price d16.5 ���������������� ����
    old_kotir d16.5 ��������� ���� ���������� ������
    deal_count i4 ���������� ������
    contr_count i4 ����� ���������� ���������� � �������
    capital d26.2 ��������� ����� ������ � ������
    pos i4 ������� ���-�� �������� �������
    mod_time t ���� � ����� ��������� ������
    cur_kotir d16.5 ������� ���������
    cur_kotir_real d16.5 �������� ���������
    orders_sell_qty i4 ���������� ������ �� �������
    orders_sell_amount i4 ����� � ���������� � ������� �� �������
    orders_buy_qty i4 ���������� ������ �� �������
    orders_buy_amount i4 ����� � ���������� � ������� �� �������
    open_price d16.5 ���� ��������
    close_price d16.5 ���� ��������
    local_time t ���� ��� ����������� ���������� common*/

	return CG_ERR_OK;
}

CG_RESULT Plaza2Callbacks::SystemCallback(const int type, msg_t* data, IListener* listener)
{
    switch(type)
    {
        /*case CG_MSG_P2REPL_ONLINE:

        break;*/
    case CG_MSG_P2REPL_REPLSTATE:
        {
            // � ������ ������ �������� � �������� ��������� ��������� � �������� (�� � �����) � ��� ������������� ��������
            // ��� ������������� �������� �������������� ������� � ������ ���� � ����� ������� ��������
			
			// ���� �� �������� ������ ������ FORTS_ORDLOG_REPL �� ��������� p2ordbook, �� ���������� ���������� �� �����
			// �� ������� ����, ��� ����� ����������� �������� ����� �������� ������ ��� ������ �����
			// �� �������� �� ���� ����� �� ������� �������� ��� �������� ��������� ��� ���
			//DEBUG_LOG("Close data stream: %s and save REPLSTATE", listener->GetStreamName().c_str())
			//listener->SetReplState(std::string((char*)data->data, data->data_size));

			// ���� ��� ���������� ����� �������� ��������� �� ��������� ACTIVE � CLOSE ��� � ERROR, 
			// ��� ����� ����������� ��� Unsubscribe �� ���������� ��� ��� �����������
			// � ����� ������� ��������� �������� ������ �� ������� �����������
			// ��� ������� �������� ��������� ������ ��� ����
			int isin_id = listener->GetIsin();
			const std::string& name = listener->GetStreamName();
			if (name == "FORTS_DEALS_REPL")
			{
				Plaza2Callbacks::ClearDealsBuffers(isin_id);
			}
			if (name == "FORTS_ORDLOG_REPL")
			{
				Plaza2Callbacks::ClearOrderBookBuffers(isin_id);
			}

            // ���� ����� ������ ������ ���������� � ����, ����������������� ��� ������ � ��������� ���� � �����
            // listener->SaveReplstateToFile("Plaza2Replstate.repl", (char*)data->data, data->data_size);
        }
        break;
    }

    return CG_ERR_OK;
}

void Plaza2Callbacks::ClearOrderBookBuffers(int isin_id)
{
	DEBUG_LOG("Clean orderbook for Isin_id: %d", isin_id)

	COrdersBookBase* ordersBook = mMatchingData.getOrdersBook(isin_id);
	ordersBook->ClearBook();

	if (mOppositeOrders.find(isin_id) != mOppositeOrders.end())
	{
		mOppositeOrders.erase(isin_id);
	}	
}

void Plaza2Callbacks::ClearDealsBuffers(int isin_id)
{
	if (mTradeList.find(isin_id) != mTradeList.end())
	{
		mTradeList.erase(isin_id);
	}
}