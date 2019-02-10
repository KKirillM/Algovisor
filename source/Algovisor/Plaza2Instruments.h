#pragma once
#include "singleton.h"
#include "Instrument.h"
#include "Plaza2_Tables.h"
#include "Price.h"
#include "MatchingData.h"
#include <chrono>

#define AGGR_BOOK_SIZE 65536 // 16384, 32768, 65536, 131072, 262144, 524288, 1048576
#define AGGR_TRADES_SIZE 16384
#define NON_AGGR_TRADES_SIZE 32768
#define PLAZA2_BOOK_SIZE 10

// Шаблонный класс хранилище для инструментов
// Позволяет доступ к данным инструментов в многопоточной среде
// Предполагается что инструмент может содержать следующие данные:
// - кольцевой массив агрегированных стаканов
// - кольцевой массив неагрегированных трейдов
// - кольцевой массив агрегированных трейдов
// - информацию о параметарх текущего инструмента (набор различных параметорв: шаг цены, кол-во знаков после запятой в цене, ...)
// В реализации не предусмотрена проверка выхода за границы размеров массива инструментов при добавлении новго инструмента,
// т.е. предполагается, что так много инструментов пользователь не задействует никогда
template <typename keyType, typename InstrumentType, unsigned int SIZE = 100>
class InstrumentStore
{
    size_t instruments_count;
    InstrumentType** instruments;
    CSRWLockObject lock;
    CSRWReadLock read_lock;
    CSRWWriteLock write_lock;

    InstrumentStore() : read_lock(lock.getLock()), write_lock(lock.getLock())
    {
        instruments_count = 0;
        instruments = new InstrumentType* [SIZE];
        for (size_t pos = 0; pos < SIZE; ++pos)
        {
            instruments[pos] = NULL;
        }
    }

    friend class CSingletonHolder<InstrumentStore<keyType, InstrumentType, SIZE>>;

public:
    ~InstrumentStore()
    {
        std::for_each(instruments, instruments + instruments_count, 
            [](InstrumentType* instrument) 
            {
                if (instrument)
                {
                    delete instrument;
                    instrument = NULL;
                }
            });

        delete[] instruments;
        instruments = NULL;
    }

    InstrumentType* GetInstrumentPtrByKey(const keyType& key)
    {
        size_t index = GetInstrumentIndex(key);
        return instruments[index];
    }

    InstrumentType* GetInstrumentPtrByName(const std::string& name)
    {
        size_t pos = 0;
        for (; pos < SIZE; ++pos)
        {
            InstrumentType* instrument = instruments[pos];
            if (instrument && instrument->GetName() == name)
            {
                return instruments[pos];
            }
        }

        return NULL;
    }

    bool IsInstrumentExist_ST(const keyType& key) const	//однопоточная проверка наличия инструмента
    { 
        return GetInstrumentIndex(key) != SIZE;
    }

    bool IsInstrumentExist_MT(const keyType& key) const	//многопоточная (безопасная) проверка наличия инструмента
    { 
        CGuard guard(read_lock);
        return IsInstrumentExist_ST(key);
    }

    //кол-во инструментов
    size_t Size_ST() const	
    { 
        return instruments_count;
    }

    size_t Size_MT() const
    { 
        size_t size = 0;
        {
            CGuard guard(read_lock);
            size = Size_ST();
        }
        return size;
    }

    void AddNewInstrument_ST(InstrumentType* element)
    {
        instruments[GetFirstNull()] = element;
        ++instruments_count;
    }

    void AddNewInstrument_MT(InstrumentType* element)
    {
        CGuard guard(write_lock); 
        AddNewInstrument_ST(element); 
    }

    void RemoveInstrument_ST(const keyType& key)
    { 
        size_t index = GetInstrumentIndex(key);
        delete instruments[index];
        instruments[index] = NULL;
        --instruments_count;
    }

    void RemoveInstrument_MT(const keyType& key)
    { 
        CGuard guard(write_lock);
        RemoveInstrument_ST(key);
    }

private:
    // вспомогательная функция для поиска первого пустого указателя в массиве инструментов
    size_t GetFirstNull() const
    {
        size_t pos = 0;
        for (; pos < SIZE; ++pos)
        {
            if (instruments[pos] == NULL)
            {
                return pos;
            }
        }
        return pos;
    }

    // вспомогательная функция для поиска инструментов в массиве
    size_t GetInstrumentIndex(const keyType& key) const
    {
        size_t pos = 0;
        for (; pos < SIZE; ++pos)
        {
            InstrumentType* instrument = instruments[pos];
            if (instrument && instrument->GetKey() == key)
            {
                return pos;
            }
        }
        return pos;
    }

    BAN_COPY_CONSTRUCTOR(InstrumentStore)

   friend class CNewStorage<InstrumentStore>;

}; //InstrumentStore

// Шаблон класса кольцевого буфера
// После заполнения буфер начинает перезаписывать начальные элементы новыми значениями
// !!! возможные длины буфера ограничить степенями двойки, это нужно для вычисления относительного индекса с помощью побитового И
// если происходит попытка чтения за пределами размера буфера возвращается пустое значение
// за основу бралась эта инфа: http://we.easyelectronics.ru/Soft/kolcevoy-bufer-na-s-dlya-mk.html
// P.S. позже эта логика была переделана на следующую:
//  буфер заполняется полностью, затем он разбивается на 2 половины,
//  вторая половина буфера копируется на место первой, и начинается заполняться с середины далее
template <typename DATA_T, const size_t SIZE>
class RingBuffer
{
    // static const size_t mask = SIZE - 1;        // маска для индексов
    DATA_T data[SIZE];    
    size_t index;           // указывает на элемент, который будет заполнен следующим, элемент на который указывает переменная index не доступен для чтения!!!
    size_t findIterator;
    
public:
    RingBuffer() : index(0), findIterator(0) {}

    // пуст ли буфер
    inline bool IsEmpty() const
    {
        return index == 0;
    }

    // очистить буфер
    inline void Clear()
    {
        index = 0;
		data[0] = DATA_T();
    }

    // полный размер буфера
    inline size_t MaxSize() const
    {
        return SIZE;
    }

    // размер буфера
    inline size_t Size() const
    {
        /*if (index == 0)
            return 0;
        else if (index < SIZE)
            return index;
        else
            return SIZE/2 + (index & mask) - 1;*/
        return index;
    }

    // запись в конец буфера
    inline void Write(const DATA_T& value)
    {
        //data[index++ & mask] = value;
        if (index == SIZE)
        {
            for (size_t start1 = 0, start2 = SIZE/2; start2 != SIZE; ++start1, ++start2)
            {
                data[start1] = data[start2];
            }

            index = SIZE/2;
        }

        data[index++] = value;
    }

    // чтение из буфера
    inline DATA_T& Read(size_t indx)
    {
        return data[indx];
        /*if (index < SIZE)
        {
            return data[indx & mask];
        }
        else
        {
            size_t endLength = mask - ((index+1) & mask);
            if (indx <= endLength)
                return data[((index+1) & mask) + indx];
            else
                return data[indx - endLength - 1];
        }*/
    }
    
    // возвращает последний элемент из буфера
    inline DATA_T& Last()
    {
        if (index > 0)
            return data[(index-1)/* & mask*/];
        else
            return data[0];
    }

    // возвращает первый элемент из буфера
    inline DATA_T& First()
    {
        //if (index < SIZE)
            return data[0];
        //else
        //    return data[(index+1) & mask];            
    } 

    // поиск нужного элемента по ключу в прямом направлении
    inline void Begin()
    {
        //if (index < SIZE)
            findIterator = 0;
        //else
        //    findIterator = (index+1) & mask;            
    }

    inline DATA_T& Next()
    {
        //if (findIterator == SIZE)
        //    findIterator = 0;

        return data[findIterator++];
    }

    inline bool ISEnd() const
    {
        return findIterator == (index/* & mask*/);
    }

    // поиск нужного элемента по ключу в обратном направлении
    inline void rBegin()
    {
        findIterator = (index-1)/* & mask*/;
    }

    inline DATA_T& rNext()
    {
        //if (findIterator == SIZE_MAX)
        //    findIterator = mask;

        return data[findIterator--];
    }

    inline bool ISrEnd() const
    {
        return findIterator == (index/* & mask*/);
    }

    BAN_COPY_CONSTRUCTOR(RingBuffer)

}; //RingBuffer

struct TimeZone
{
    std::chrono::minutes minutesShift;

    TimeZone()
    {        
        TIME_ZONE_INFORMATION tz_info;
        GetTimeZoneInformation(&tz_info);
        minutesShift = std::chrono::minutes(-tz_info.Bias);
    }

}; // TimeZone

struct Plaza2Timestamp
{
    static TimeZone timezone;

    unsigned char hour;
    unsigned char minute;
    unsigned char second;
    unsigned int nanoseconds;

    Plaza2Timestamp() 
    { 
        memset(this, 0, sizeof(Plaza2Timestamp));
    }

    Plaza2Timestamp(const __int64 timestamp)
    {
        SetTimestamp(timestamp);
    }

    void SetTimestamp(const __int64 timestamp)
    {
        std::time_t ttt = (timestamp + std::chrono::duration_cast<std::chrono::nanoseconds>(timezone.minutesShift).count())
            / std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();
        std::tm tm;
        gmtime_s(&tm, &ttt);

        hour = static_cast<unsigned char>(tm.tm_hour);
        minute = static_cast<unsigned char>(tm.tm_min);
        second = static_cast<unsigned char>(tm.tm_sec);

        std::time_t tt = timestamp / std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();
        gmtime_s(&tm, &tt);

        nanoseconds = unsigned int(timestamp % std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count());
    }

};	// Plaza2Timestamp

enum InterestDirection
{
    UNDEFINED, OPEN_POSITION, CLOSE_POSITION
};

struct Plaza2Trade
{
    typedef long long key;

	long long ord_id;                   // id оредера, который породил сделку
	long long deal_id;                  // id сделки, которую породил выставленный ордер
    Price price;						// цена текущего трейда
    Plaza2Timestamp timestamp;
    unsigned int size;					// размер текущего трейда
    unsigned int sizeBeforeTrade;		// размер лимита до трейда (т.е. размер трейда плюс размер лимита по которому он ударил)
    char agressorSide;					// B - Buy, S - Sell, N - Unknown
    InterestDirection interestDir;      // сторона изменения открытого интереса в сделке
    int interest;                       // Кол-во позиций по инструменту на рынке после сделки
    
    Plaza2Trade() 
    {
        size = 0; 
        sizeBeforeTrade = 0; 
        agressorSide = 'N'; 
        interestDir = UNDEFINED;
        interest = 0;
        ord_id = 0LL;
        deal_id = 0LL;
    }

    Plaza2Trade(const Plaza2Trade& copy) 
    {
        price = copy.price;
        size = copy.size;
        sizeBeforeTrade = copy.sizeBeforeTrade;
        ord_id = copy.ord_id;
		deal_id = copy.deal_id;
        agressorSide = copy.agressorSide;
        timestamp = copy.timestamp;	
        interest = copy.interest;
        interestDir = copy.interestDir;
    }

    void SetTrade(const Price& _price, 
                  const unsigned int _size, 
                  const unsigned int _sizeBeforeTrade, 
                  const long long _ord_id, 
                  const long long _deal_id, 
                  const char _agressorSide, 
                  const __int64 _timestamp,
                  const InterestDirection _interestDir = UNDEFINED)
    {
        price = _price;
        size = _size;
        sizeBeforeTrade = _sizeBeforeTrade;
        ord_id = _ord_id;
        deal_id = _deal_id;
        agressorSide = _agressorSide;
        timestamp.SetTimestamp(_timestamp);
        interestDir = _interestDir;
    }

    std::string GetString() const
    {
        char buf[128] = {0};
        sprintf_s(buf, sizeof(buf), "%s(%d) '%c' OI_size:%d, OI_change:%c", price.GetString().c_str(), size, agressorSide, interest, interestDir == OPEN_POSITION ? '+' : (interestDir == CLOSE_POSITION ? '-' : '?'));
        return std::string(buf);
    }

};	// Plaza2Trade

struct Plaza2AggregatedTrade : public Plaza2Trade
{
    //int pieces;                       // кол-во кусочков, из которых состоит данный агрегированный трейд
    int initialQuantity;                // исходный размер трейда (может отличаться от сведённого, т.к. остаток может попасть в книгу или отброситься)
    std::vector<int> posArray;          // все значения изменеий открытого интереса, которые сделал этот ордер
    std::vector<unsigned int> dealIDArray;       // все значения id сделок, которые породил этот ордер
    
    Plaza2AggregatedTrade() : Plaza2Trade(), initialQuantity(0)/*, pieces(0)*/ {}

    Plaza2AggregatedTrade(Plaza2Trade trade) : Plaza2Trade(trade), initialQuantity(0) {}

    Plaza2AggregatedTrade(const Plaza2AggregatedTrade& copy)
    {
        *this = copy;
    }

};  // Plaza2AggregatedTrade

struct Plaza2OrdersSet
{
    Price price;
	unsigned int size;		// если здесь изменить тип на long long, то вылезает бага
	unsigned int ordersCount;

    Plaza2OrdersSet() : size(0), ordersCount(0) {}

    bool operator<(const Plaza2OrdersSet& element) const { return size < element.size; }

    static bool OrdersCountComparator(const Plaza2OrdersSet& element1, const Plaza2OrdersSet& element2) 
    { 
        return element1.ordersCount < element2.ordersCount; 
    }

};	// Plaza2OrdersInBook

struct Plaza2Book
{
    static const int dom_size = PLAZA2_BOOK_SIZE;

    Plaza2Trade* trade;
    Plaza2OrdersSet bids[dom_size];
    Plaza2OrdersSet asks[dom_size];
    int lastOrderTransactionSize;	    // размер добавленного или убранного ордера на данной итерации, если > 0, то добавили, если < 0, то убрали
    Plaza2Timestamp timestamp;
    int eventBidLevel;				    // 32 бита, устанавливает номер того бита, который соотвествует номеру уровня в стакане
    int eventAskLevel;
    char agressorSide;					// types of agressor side: B - Buy trade, S - Sell trade, ? - implied trade, N - not defined, a - ask event, b - bid event

    Plaza2Book() : eventBidLevel(0), eventAskLevel(0), agressorSide('N'), lastOrderTransactionSize(0), trade(nullptr)
    {
    }

    Plaza2Book(const Plaza2Book& copy) 
    {
        trade = copy.trade; 
        timestamp = copy.timestamp;
        agressorSide = copy.agressorSide;
        lastOrderTransactionSize = copy.lastOrderTransactionSize;
        std::copy(copy.bids, copy.bids+dom_size, bids);
        std::copy(copy.asks, copy.asks+dom_size, asks);
        eventBidLevel = copy.eventBidLevel;
        eventAskLevel = copy.eventAskLevel;
    }

    Plaza2Book(AggregatedBook& book)
    {
        trade = NULL;
        timestamp.SetTimestamp(book.timestamp);
        agressorSide = book.agressorSide;
        lastOrderTransactionSize = book.lastOrderTransactionSize;
        int i = 0;
        for (PriceSet::iterator it = book.bidPriceSet.begin(), end = book.bidPriceSet.end(); it != end && i < dom_size; ++it, ++i)
        {
            bids[i].price = Price(it->price); bids[i].size = it->orders_size; bids[i].ordersCount = it->orders_count;
        }
        i = 0;
        for (PriceSet::iterator it = book.askPriceSet.begin(), end = book.askPriceSet.end(); it != end && i < dom_size; ++it, ++i)
        {
            asks[i].price = it->price; asks[i].size = it->orders_size; asks[i].ordersCount = it->orders_count;
        }
        eventBidLevel = book.bidPriceLevelEvent > 0 ? 1 << (book.bidPriceLevelEvent - 1) : 0;
        eventAskLevel = book.askPriceLevelEvent > 0 ? 1 << (book.askPriceLevelEvent - 1) : 0;
    }

    void operator=(const Plaza2Book& copy)
    {
        if (&copy == this)
        {
            return;
        }

        trade = copy.trade;
        timestamp = copy.timestamp;
        agressorSide = copy.agressorSide;
        lastOrderTransactionSize = copy.lastOrderTransactionSize;
        std::copy(copy.bids, copy.bids+dom_size, bids);
        std::copy(copy.asks, copy.asks+dom_size, asks);
        eventBidLevel = copy.eventBidLevel;
        eventAskLevel = copy.eventAskLevel;
    }

    unsigned int MaxSize() const
    {
        unsigned int ask_max = 0;
        unsigned int bid_max = 0;
        ask_max = (*std::max_element(asks, asks+dom_size)).size;
        bid_max = (*std::max_element(bids, bids+dom_size)).size;

        return max(ask_max, bid_max);
    }

    unsigned int MinSize() const
    {
        unsigned int ask_min = 0;
        unsigned int bid_min = 0;
        ask_min = (*std::min_element(asks, asks+dom_size)).size;
        bid_min = (*std::min_element(bids, bids+dom_size)).size;

        return min(ask_min, bid_min);
    }

    unsigned int MaxOrderCount() const
    {
        unsigned int ask_max = 0;
        unsigned int bid_max = 0;
        ask_max = (*std::max_element(asks, asks+dom_size, Plaza2OrdersSet::OrdersCountComparator)).ordersCount;
        bid_max = (*std::max_element(bids, bids+dom_size, Plaza2OrdersSet::OrdersCountComparator)).ordersCount;

        return max(ask_max, bid_max);
    }

    unsigned int MinOrderCount() const
    {
        unsigned int ask_min = 0;
        unsigned int bid_min = 0;
        ask_min = (*std::min_element(asks, asks+dom_size, Plaza2OrdersSet::OrdersCountComparator)).ordersCount;
        bid_min = (*std::min_element(bids, bids+dom_size, Plaza2OrdersSet::OrdersCountComparator)).ordersCount;

        return min(ask_min, bid_min);
    }

    std::string GetAskString() const
    {
        char str[128] = {0};
        std::string logString;

        for (int i = 0; i != dom_size; ++i)
        {
            if (asks[i].price.IsPrice())
            {
                sprintf_s(str, sizeof(str), "%s(%d/%d) ", asks[i].price.GetString().c_str(), asks[i].size, asks[i].ordersCount);
                logString += str;
            }
        }

        return logString;
    }

    std::string GetBidString() const
    {
        char str[128] = {0};
        std::string logString;

        for (int i = dom_size; i != 0; --i)
        {
            if (bids[i-1].price.IsPrice())
            {
                sprintf_s(str, sizeof(str), "%s(%d/%d) ", bids[i-1].price.GetString().c_str(),bids[i-1].size, bids[i-1].ordersCount);
                logString += str;
            }
        }

        return logString;
    }

    std::string GetTradeString() const
    {
        std::string logString;
    
        if (trade)
        {
            logString = trade->GetString();
        }

        return logString;
    }

    std::string GetInfoString() const
    {
        char buf[128];

        sprintf_s(buf, sizeof(buf), "%02d:%02d:%02d.%09d", timestamp.hour, timestamp.minute, timestamp.second, timestamp.nanoseconds);

        std::string logString(buf);

        if (eventBidLevel > 0)
        {
            for(int i = 0; i < 10; ++i)
            {
                if(eventBidLevel & (1 << i))
                {
                    logString += " B:" + std::to_string(i+1);
                }
            }
            
        }

        if (eventAskLevel > 0)
        {
            for(int i = 0; i < 10; ++i)
            {
                if(eventAskLevel & (1 << i))
                {
                    logString += " A:" + std::to_string(i+1);
                }
            }
        }

        return logString;
    }

};  // Plaza2Book

class Plaza2Instrument
{
public:    // костыль
    int precision;		// количество знаков после запятой
    int minTickInc;		// минимальный шаг цены инструмента

private:
    typedef RingBuffer<Plaza2Book, AGGR_BOOK_SIZE> AccumulatedBook;
    typedef RingBuffer<Plaza2Trade, NON_AGGR_TRADES_SIZE> Plaza2NonAggregatedTrades;
    typedef RingBuffer<Plaza2AggregatedTrade, AGGR_TRADES_SIZE> Plaza2AggregatedTrades;

	FORTS_FUTINFO_REPL::fut_sess_contents info;
    AccumulatedBook book;
    Plaza2AggregatedTrades aggrTradesArray;
	Plaza2NonAggregatedTrades tradesArray;
	

	Plaza2Instrument() : precision(0), minTickInc(0) { }

public:
	static Plaza2Instrument* CreateNewInstrument() 
	{ 
		return new Plaza2Instrument();
	}

    void Info(const FORTS_FUTINFO_REPL::fut_sess_contents& _info);
    
    const FORTS_FUTINFO_REPL::fut_sess_contents& Info() const { return info; }

    std::string GetName() const { return info.isin; }
    
    int GetKey() const { return info.isin_id; }

    void AddBook(const Plaza2Book& val)
    {
        book.Write(val);
    }

    /*void SetBook(const size_t index, const Plaza2Book& _book)
    {
        book.WriteByIndex(index, _book);
    }*/

    Plaza2Book& GetBook(const size_t index)
    {
        return book.Read(index);
    }

    size_t BookSize() const
    {
        return book.Size();
    }

    void AddNonAggregatedTrade(const Plaza2Trade& trade)
    { 
        tradesArray.Write(trade);
    }

    Plaza2Trade& GetNonAggregatedTrade(const size_t index)
    { 
        return tradesArray.Read(index);
    }

    Plaza2Trade& GetLastNonAggrTrade()
    {
        return tradesArray.Last();
    }

    size_t NonAggregatedTradesArraySize() const
    { 
        return tradesArray.Size();
    }

    size_t NonAggregatedTradesArrayMaxSize() const
    { 
        return tradesArray.MaxSize();
    }

    void ClearNonAggregatedTrades() 
    { 
        tradesArray.Clear(); 
    }    

    void AddAggregatedTrade(const Plaza2AggregatedTrade& trade)
    { 
        aggrTradesArray.Write(trade);
    }

    Plaza2AggregatedTrade& GetAggregatedTrade(const size_t index) 
    { 
        return aggrTradesArray.Read(index);
    }

    Plaza2AggregatedTrade& GetLastAggregatedTrade()
    {
        return aggrTradesArray.Last();
    }

    size_t AggregatedTradesArraySize() const
    { 
        return aggrTradesArray.Size();
    }

    size_t AggregatedTradesArrayMaxSize() const
    { 
        return aggrTradesArray.MaxSize();
    }

    void ClearAggregatedTrades()
    { 
        aggrTradesArray.Clear(); 
    }

	void Clear()
	{
		book.Clear();
		aggrTradesArray.Clear();
		tradesArray.Clear();
	}

    Plaza2Trade* FindNonAggrTrade(const Plaza2Trade::key id)
    {
        if (tradesArray.IsEmpty())
        {
            return NULL;
        }

        tradesArray.rBegin();
        Plaza2Trade* trade = &tradesArray.rNext();

        // у каждого последующего объекта буффера значение поля key будет больше чем у предыдущего
        // поэтому эта проверка имеет место быть
        if (trade->deal_id < id)
        {
            return NULL;
        }

        while (trade->deal_id != id)
        {
            if (tradesArray.ISrEnd())
            {
                return NULL;
            }
            trade = &tradesArray.rNext();
        }

        return trade;
    }

    // поиск наличия deal_id в буфере не агрегированных трейдов
    bool FindNonAggrTradeID(const Plaza2Trade::key id)
    {
        if (tradesArray.IsEmpty())
        {
            return false;
        }

        // у каждого последующего объекта буффера значение поля key будет больше чем у предыдущего
        // поэтому эта проверка имеет место быть
        const Plaza2Trade::key deal_id = tradesArray.Last().deal_id;
        if (deal_id < id)
        {
            return false;
        }

        return true;
    }

    Plaza2AggregatedTrade* FindAggrTrade(const Plaza2AggregatedTrade::key id)
    {
        if (aggrTradesArray.IsEmpty())
        {
            return NULL;
        }

        aggrTradesArray.rBegin();
        Plaza2AggregatedTrade* trade = &aggrTradesArray.rNext();

        // у каждого последующего объекта буффера значение поля key будет больше чем у предыдущего
        // поэтому эта проверка имеет место быть
        if (trade->dealIDArray.back() < id)
        {
            return NULL;
        }

        while (std::find(trade->dealIDArray.begin(), trade->dealIDArray.end(), id) == trade->dealIDArray.end())
        {
            if (aggrTradesArray.ISrEnd())
            {
                return NULL;
            }
            trade = &aggrTradesArray.rNext();
        }

        return trade;
    }

}; //Plaza2Instrument

typedef int Plaza2IsinId;
typedef InstrumentStore<Plaza2IsinId, Plaza2Instrument> Plaza2InstrumentDB;
typedef CSingletonHolder<Plaza2InstrumentDB> Plaza2DB;

class Plaza2TickerAdapterClass : public IInstrument
{
    int key;								//ключ для поиска инструмента в массиве инструментов

    std::string exchange;
    std::string expiration;
    int type;
    bool tradeOnlyFlag;
    Plaza2Instrument* instrumentPtr;

public:
    Plaza2TickerAdapterClass(int _key, bool _tradeOnlyFlag = false);
    ~Plaza2TickerAdapterClass();

    virtual void CopyAllParameters(void* x);
    virtual void TradePrint(void* p);
    virtual void TradePrintNinja(void* p);
    virtual size_t GetAggregatedOrderbookSize() const;
    virtual size_t GetNonAggregationTradesDataSize() const;
    virtual size_t GetAggregationTradesDataSize() const;
    virtual int GetHistoryFlag() const;

    virtual Price Get_tradePrice(size_t i) const;
    virtual UINT Get_tradeSize(size_t i) const;
    virtual UINT GetInterestAfterTrade(size_t i) const;
    virtual void Add_tradeSize(size_t i, UINT size);
    virtual BYTE Get_hour(size_t i) const;
    virtual BYTE Get_minute(size_t i) const;
    virtual BYTE Get_second(size_t i) const;
    virtual UINT Get_microseconds(size_t i) const;
    virtual UINT Get_microdelta(size_t i) const;
    virtual UINT Get_buyVolume(size_t i) const;
    virtual UINT Get_sellVolume(size_t i) const;
    virtual char Get_agressorSide(size_t i) const;
    virtual char Get_impliedSide(size_t i) const;
    virtual char Get_condition(size_t i) const;
    virtual bool Get_yAxis(size_t i) const;
    virtual void Set_yAxis(size_t i, bool y);

    virtual Price Get_bestBidPrice(size_t i) const;
    virtual Price Get_bestAskPrice(size_t i) const;
    virtual UINT Get_bestBidSize(size_t i) const;
    virtual UINT Get_bestAskSize(size_t i) const;
    virtual UINT Get_bestBidOrders(size_t i) const;
    virtual UINT Get_bestAskOrders(size_t i) const;
    virtual UINT Get_tradeSizeQuant(size_t i) const;

    virtual Price Get_bidPrice(size_t i, int y) const;
    virtual Price Get_askPrice(size_t i, int y) const;
    virtual UINT Get_bidSize(size_t i, int y) const;
    virtual UINT Get_askSize(size_t i, int y) const;
    virtual UINT Get_bidOrders(size_t i, int y) const;
    virtual UINT Get_askOrders(size_t i, int y) const;
    virtual int Get_eventBidLevel(size_t i) const;
    virtual int Get_eventAskLevel(size_t i) const;
    virtual int Get_deltaBidLimitSize(size_t i, int y = 0) const;
    virtual int Get_deltaAskLimitSize(size_t i, int y = 0) const;
    virtual int Get_deltaBidLimitSum(size_t i) const;
    virtual int Get_deltaAskLimitSum(size_t i) const;
    virtual UINT Get_maxDomLimit(size_t i) const;
    virtual UINT Get_minDomLimit(size_t i) const;
    virtual UINT Get_maxDomOrder(size_t i) const;
    virtual UINT Get_minDomOrder(size_t i) const;
    virtual char Get_updateType(size_t i) const;
    virtual char Get_orderType(size_t i) const;

    virtual void GetDOM(void* d);
    virtual void GetDOMNinja(void* d);

    virtual void push_back(void* i);
    virtual void* get_data(size_t i);
    virtual void erase_data(size_t i);
    virtual void* create_object();
    virtual size_t get_size();

    virtual Plaza2Instrument* GetInstrument() { return instrumentPtr; }

}; //Plaza2TickerAdapterClass