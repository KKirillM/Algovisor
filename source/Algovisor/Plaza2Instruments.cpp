#include "stdafx.h"
#include "Plaza2Objects.h"
#include "GetValuesHelper.h"
#include "Plaza2_Tables.h"
#include "Instrument.h"
#include "Auxiliary_methods.h"
#include "Plaza2Instruments.h"

TimeZone Plaza2Timestamp::timezone;

Plaza2TickerAdapterClass::Plaza2TickerAdapterClass(int _key, bool _tradeOnlyFlag /* = false */) : key(_key), tradeOnlyFlag(_tradeOnlyFlag)
{		
    if (tradeOnlyFlag)
    {
        instrumentPtr = Plaza2Instrument::CreateNewInstrument();
    }
    else
    {
        instrumentPtr = Plaza2DB::GetInstance()->GetInstrumentPtrByKey(_key);
    }
    
    tInfo.ordersNumber = true;	//кол-во ордеров на ценовых уровнях доступно в плазе2 с услугой полный ордер лог
    domLevelsCount = 10;
    name = instrumentPtr->Info().isin;
    description = instrumentPtr->Info().name;
    precision = instrumentPtr->Info().roundto;
    MinTickInc = instrumentPtr->minTickInc;
    //mode = "Disconnect"; //"Connect"
    tInfo.level2 = true;
    exchange = "MOEX";
    char char_expiration[15];
    sprintf_s(char_expiration, sizeof(char_expiration), "%02d.%02d.%d", instrumentPtr->Info().d_exp.day, instrumentPtr->Info().d_exp.month, instrumentPtr->Info().d_exp.year);
    expiration = char_expiration;
}

Plaza2TickerAdapterClass::~Plaza2TickerAdapterClass()
{
    if (tradeOnlyFlag)
    {
        delete instrumentPtr;
    }
}

void Plaza2TickerAdapterClass::CopyAllParameters(void* x) 
{
    Plaza2TickerAdapterClass* i = (Plaza2TickerAdapterClass*)x;

    name			= i->name;
    description		= i->description;
    MinTickInc		= i->MinTickInc;						
    precision		= i->precision;
    mode			= i->mode;
    domLevelsCount	= i->domLevelsCount;
    exchange		= i->exchange;
    expiration		= i->expiration;
}
void Plaza2TickerAdapterClass::TradePrint(void* p)					{}
void Plaza2TickerAdapterClass::TradePrintNinja(void* p)				{}
size_t Plaza2TickerAdapterClass::GetAggregatedOrderbookSize() const		 { return instrumentPtr->BookSize(); }
size_t Plaza2TickerAdapterClass::GetNonAggregationTradesDataSize() const { return instrumentPtr->NonAggregatedTradesArraySize(); }
size_t Plaza2TickerAdapterClass::GetAggregationTradesDataSize() const    { return instrumentPtr->AggregatedTradesArraySize(); }

int Plaza2TickerAdapterClass::GetHistoryFlag() const				{ return 0; }

Price Plaza2TickerAdapterClass::Get_tradePrice(size_t i) const		
{ 
    Plaza2Trade* trade = instrumentPtr->GetBook(i).trade;
    if (trade)
    {
        return trade->price;
    }
    return Price();
}
UINT Plaza2TickerAdapterClass::Get_tradeSize(size_t i) const		
{ 
    Plaza2Trade* trade = instrumentPtr->GetBook(i).trade;
    if (trade)
    {
        return trade->size;
    }
    return 0;
}
UINT Plaza2TickerAdapterClass::GetInterestAfterTrade(size_t i) const
{ 
    Plaza2Trade* trade = instrumentPtr->GetBook(i).trade;
    if (trade)
    {
        return trade->interest;
    }
    return 0;
}
void Plaza2TickerAdapterClass::Add_tradeSize(size_t i, UINT size) 
{ 
    //Plaza2Book book = instrument->GetBook(i);
    //book.trade->size += size;
    //instrument->SetBook(i, book);
}
BYTE Plaza2TickerAdapterClass::Get_hour(size_t i) const				{ return instrumentPtr->GetBook(i).timestamp.hour; }
BYTE Plaza2TickerAdapterClass::Get_minute(size_t i) const			{ return instrumentPtr->GetBook(i).timestamp.minute; }
BYTE Plaza2TickerAdapterClass::Get_second(size_t i) const			{ return instrumentPtr->GetBook(i).timestamp.second; }
UINT Plaza2TickerAdapterClass::Get_microseconds(size_t i) const		{ return instrumentPtr->GetBook(i).timestamp.nanoseconds / 1000; }
UINT Plaza2TickerAdapterClass::Get_microdelta(size_t i) const 
{ 
    UINT deltamilliseconds = 0;
    if (i > 0)
    {
        deltamilliseconds = instrumentPtr->GetBook(i).timestamp.nanoseconds / 1000 - instrumentPtr->GetBook(i-1).timestamp.nanoseconds / 1000;
    }
    return deltamilliseconds;
}
UINT Plaza2TickerAdapterClass::Get_buyVolume(size_t i) const		{ return 0; }
UINT Plaza2TickerAdapterClass::Get_sellVolume(size_t i) const		{ return 0; }
char Plaza2TickerAdapterClass::Get_agressorSide(size_t i) const		{ return instrumentPtr->GetBook(i).agressorSide; }
char Plaza2TickerAdapterClass::Get_impliedSide(size_t i) const		{ return 0; }
char Plaza2TickerAdapterClass::Get_condition(size_t i) const		{ return 0; }
bool Plaza2TickerAdapterClass::Get_yAxis(size_t i) const 
{ 
    bool needAxis = false;
    if (i && i % 100 == 0)
    {
        needAxis = true;
    }
    return needAxis; 
}
void Plaza2TickerAdapterClass::Set_yAxis(size_t i, bool y) {}

Price Plaza2TickerAdapterClass::Get_bestBidPrice(size_t i) const	{ return instrumentPtr->GetBook(i).bids[0].price; }
Price Plaza2TickerAdapterClass::Get_bestAskPrice(size_t i) const	{ return instrumentPtr->GetBook(i).asks[0].price; }
UINT Plaza2TickerAdapterClass::Get_bestBidSize(size_t i) const		{ return instrumentPtr->GetBook(i).bids[0].size; }
UINT Plaza2TickerAdapterClass::Get_bestAskSize(size_t i) const		{ return instrumentPtr->GetBook(i).asks[0].size; }
UINT Plaza2TickerAdapterClass::Get_bestBidOrders(size_t i) const	{ return instrumentPtr->GetBook(i).bids[0].ordersCount; }
UINT Plaza2TickerAdapterClass::Get_bestAskOrders(size_t i) const	{ return instrumentPtr->GetBook(i).asks[0].ordersCount; }

UINT Plaza2TickerAdapterClass::Get_tradeSizeQuant(size_t i) const	{ return instrumentPtr->GetBook(i).trade->sizeBeforeTrade; }

Price Plaza2TickerAdapterClass::Get_bidPrice(size_t i, int y) const { return instrumentPtr->GetBook(i).bids[y].price; }
Price Plaza2TickerAdapterClass::Get_askPrice(size_t i, int y) const { return instrumentPtr->GetBook(i).asks[y].price; }
UINT Plaza2TickerAdapterClass::Get_bidSize(size_t i, int y) const	{ return instrumentPtr->GetBook(i).bids[y].size; }
UINT Plaza2TickerAdapterClass::Get_askSize(size_t i, int y) const	{ return instrumentPtr->GetBook(i).asks[y].size; }
UINT Plaza2TickerAdapterClass::Get_bidOrders(size_t i, int y) const { return instrumentPtr->GetBook(i).bids[y].ordersCount; }
UINT Plaza2TickerAdapterClass::Get_askOrders(size_t i, int y) const { return instrumentPtr->GetBook(i).asks[y].ordersCount; }

int Plaza2TickerAdapterClass::Get_eventBidLevel(size_t i) const	{ return instrumentPtr->GetBook(i).eventBidLevel; }
int Plaza2TickerAdapterClass::Get_eventAskLevel(size_t i) const	{ return instrumentPtr->GetBook(i).eventAskLevel; }

int Plaza2TickerAdapterClass::Get_deltaBidLimitSize(size_t i, int y) const
{ 
    Plaza2Book& book = instrumentPtr->GetBook(i);
    if (book.eventBidLevel & (1 << y))
        return book.lastOrderTransactionSize;
    return 0; 
}
int Plaza2TickerAdapterClass::Get_deltaAskLimitSize(size_t i, int y) const
{ 
    Plaza2Book& book = instrumentPtr->GetBook(i);
    if (book.eventAskLevel & (1 << y))
        return book.lastOrderTransactionSize;
    return 0; 
}
int Plaza2TickerAdapterClass::Get_deltaBidLimitSum(size_t i) const	
{ 
    Plaza2Book& book = instrumentPtr->GetBook(i);
    if (book.eventBidLevel > 0)
    {
        return book.lastOrderTransactionSize;
    }
    return 0;
}
int Plaza2TickerAdapterClass::Get_deltaAskLimitSum(size_t i) const	
{ 
    Plaza2Book& book = instrumentPtr->GetBook(i);
    if (book.eventAskLevel > 0)
    {
        return book.lastOrderTransactionSize;
    }
    return 0; 
}
UINT Plaza2TickerAdapterClass::Get_maxDomLimit(size_t i) const		{ return instrumentPtr->GetBook(i).MaxSize(); }
UINT Plaza2TickerAdapterClass::Get_minDomLimit(size_t i) const		{ return instrumentPtr->GetBook(i).MinSize(); }
UINT Plaza2TickerAdapterClass::Get_maxDomOrder(size_t i) const		{ return instrumentPtr->GetBook(i).MaxOrderCount(); }
UINT Plaza2TickerAdapterClass::Get_minDomOrder(size_t i) const		{ return instrumentPtr->GetBook(i).MinOrderCount(); }
char Plaza2TickerAdapterClass::Get_updateType(size_t i) const		{ return 0; }
char Plaza2TickerAdapterClass::Get_orderType(size_t i) const		{ return 0; }

void Plaza2TickerAdapterClass::GetDOM(void* d)						{}
void Plaza2TickerAdapterClass::GetDOMNinja(void* d)					{}

void Plaza2TickerAdapterClass::push_back(void* i)					{ instrumentPtr->AddBook(*(Plaza2Book*)i); }
void* Plaza2TickerAdapterClass::get_data(size_t i)					{ return &instrumentPtr->GetBook(i); }
void Plaza2TickerAdapterClass::erase_data(size_t i)					{}
void* Plaza2TickerAdapterClass::create_object()						{ return (new Plaza2TickerAdapterClass(key, true)); }
size_t Plaza2TickerAdapterClass::get_size()							{ return instrumentPtr->BookSize(); }

void Plaza2Instrument::Info(const FORTS_FUTINFO_REPL::fut_sess_contents& _info)
{ 
    info = _info;
    P2Price price(_info.min_step);
    precision = _info.roundto;
    minTickInc = (int)(price.price_double * (pow(10.0, precision)));
}