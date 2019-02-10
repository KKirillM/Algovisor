#pragma once
#pragma warning (disable : 4100)
#pragma warning (disable : 4480)

#include "DEFINE.h"
#include "Price.h"


class RTicker;
class CQGTicker;
struct INSTRUMENT_PROPERTIES;
struct CQG_INSTRUMENT_TRADE;
struct CQG_INSTRUMENT_BBO_ASK;
struct CQG_INSTRUMENT_BBO_BID;
struct CQG_INSTRUMENT_DOM;

extern std::vector<RTicker*> rithmic_data;
extern std::vector<CQGTicker*> cqg_data;
extern std::vector<CQGTicker*> ninja_data;
extern SRWLOCK SRWLockRithmic;

typedef struct _DATA {	//28 байт
	Price tradePrice;
	UINT tradeSize;
	BYTE hour;
	BYTE minute;
	BYTE second;
} DATA;

typedef struct _RITHMICTRADE { //80 байт
	DATA Data;
	UINT microseconds;
	//UINT s_microseconds;
	UINT microDelta;
	UINT buyVolume;		//накопленная дельта покупок с начала сессии
	UINT sellVolume;	//накопленная дельта продаж с начала сессии
	bool yAxis;			//там где не равен нулю нужна вертикальная метка с временем	
	char agressorSide;
	char impliedSide;
	char condition;
} RITHMICTRADE;

//typedef struct _RITHMICBBO {	//120 байт
//	RITHMICTRADE Data;
//	Price bidPrice;
//	Price askPrice;
//	UINT bidSize;
//	UINT askSize;
//	UINT bidOrders;
//	UINT askOrders;
//	UINT tradeSizeQuant;
//	int deltaBidLimitSize;			//на сколько увеличился или уменьшился бид
//	int deltaAskLimitSize;			//на сколько увеличился или уменьшился аск
//} RITHMICBBO;

typedef struct _RITHMICDOM {	//448 байт
	RITHMICTRADE Data;
	Price bidPrice[11];
	Price askPrice[11];
	UINT bidSize[11];
	UINT askSize[11];
	UINT bidOrders[11];
	UINT askOrders[11];
	int deltaBidLimitSize[10];	//на сколько увеличился или уменьшился бид
	int deltaAskLimitSize[10];	//на сколько увеличился или уменьшился аск
	int eventBidLevel;		    //16 бит, устанавливает номер того бита, который соотвествует номеру уровня в стакане
	int eventAskLevel;
	UINT tradeSizeQuant;
	//int deltaLimitSize;		
	UINT maxDomLimit;		//максимальное значение в стакане среди текущих(это нужно для визуализации хит меп)
	UINT minDomLimit;		//минимальное значение в стакане среди текущих
	UINT maxDomOrder;
	UINT minDomOrder;
	char updateType;		//тип ордера
	char orderType;			//тип ордера определяемый нашим алгоритмом: 1 - простой добавили, 2 - простой убрали, 3 - смещение добавили, 4 - смещение убрали, 5 - алго-ордер добавили, 6 - алго-ордер убрали
} RITHMICDOM;

// структура содержит флаги, информирующие о наличии той или иной информации у конкретного инструмента
struct Instrument_INFO
{
	Instrument_INFO::Instrument_INFO() 
	{
		level2 = tradeCondition = ordersNumber = cumulativeVolume = false;
	}

	bool level2;
	bool tradeCondition;
	bool ordersNumber;      // количество ордеров на ценовом уровне
	bool cumulativeVolume;
};

class IInstrument		//базовый класс для всех инструментов
{
public:
	IInstrument(std::string n = "") : name(n), MinTickInc(0), precision(0), domLevelsCount(0) {}
	virtual ~IInstrument() {}
	
	//Price PriceMinusPrice(const Price& price1, const Price& price2);
	UINT PriceDifference(const Price& price1, const Price& price2) const;
	void IncPrice(Price& price);
	void DecPrice(Price& price);
	int ToInt(double x) const;

	void SetInstrumentINFO(Instrument_INFO tInf) { tInfo = tInf; }
	void SetName(const char* s) { name = s; }
	void SetDescription(const char* s) { description = s; }
	void SetMinTick(int tick) { MinTickInc = tick; }
	void SetPrecision(int p) { precision = p; }
	void SetDomLevelsCount(int i) { domLevelsCount = i; }
	void SetMode(const char* s) { mode = s; }

	Instrument_INFO GetInstrumentINFO() const { return tInfo; }
	std::string GetName() const { return name; }
	std::string GetDescription() const { return description; }
	int GetMinTick() const { return MinTickInc; }
	int GetPrecision() const { return precision; }
	int GetDomLevelsCount() const { return domLevelsCount; }
	std::string GetMode() const { return mode; }

	virtual bool IsLevel2() { return tInfo.level2; }
	virtual bool IsTradeCondition() { return tInfo.tradeCondition; }
	virtual bool IsOrdersNumber() { return tInfo.ordersNumber; }
	virtual bool IsCumulativeVolume() { return tInfo.cumulativeVolume; }

	virtual void CopyAllParameters(void* i) = 0;
	virtual void TradePrint(void* p) = 0;
	virtual void TradePrintNinja(void* p) = 0;
	virtual size_t GetAggregatedOrderbookSize() const = 0;
    virtual size_t GetNonAggregationTradesDataSize() const = 0;
    virtual size_t GetAggregationTradesDataSize() const = 0;
	virtual int GetHistoryFlag() const = 0;

	virtual Price Get_tradePrice(size_t i) const = 0;
	virtual UINT Get_tradeSize(size_t i) const = 0;
    virtual UINT GetInterestAfterTrade(size_t i) const = 0;
	virtual void Add_tradeSize(size_t i, UINT size) = 0;
	virtual BYTE Get_hour(size_t i) const = 0;
	virtual BYTE Get_minute(size_t i) const = 0;
	virtual BYTE Get_second(size_t i) const = 0;
	virtual UINT Get_microseconds(size_t i) const = 0;
	virtual UINT Get_microdelta(size_t i) const = 0;
	virtual UINT Get_buyVolume(size_t i) const = 0;
	virtual UINT Get_sellVolume(size_t i) const = 0;
	virtual char Get_agressorSide(size_t i) const = 0;
	virtual char Get_impliedSide(size_t i) const = 0;
	virtual char Get_condition(size_t i) const = 0;
	virtual bool Get_yAxis(size_t i) const = 0;
	virtual void Set_yAxis(size_t i, bool y) = 0;

	virtual Price Get_bestBidPrice(size_t i) const = 0;
	virtual Price Get_bestAskPrice(size_t i) const = 0;
	virtual UINT Get_bestBidSize(size_t i) const = 0;
	virtual UINT Get_bestAskSize(size_t i) const = 0;
	virtual UINT Get_bestBidOrders(size_t i) const = 0;
	virtual UINT Get_bestAskOrders(size_t i) const = 0;
	virtual UINT Get_tradeSizeQuant(size_t i) const = 0;

	virtual Price Get_bidPrice(size_t i, int y) const = 0;
	virtual Price Get_askPrice(size_t i, int y) const = 0;
	virtual UINT Get_bidSize(size_t i, int y) const = 0;
	virtual UINT Get_askSize(size_t i, int y) const = 0;
	virtual UINT Get_bidOrders(size_t i, int y) const = 0;
	virtual UINT Get_askOrders(size_t i, int y) const = 0;
	virtual int Get_eventBidLevel(size_t i) const = 0;
	virtual int Get_eventAskLevel(size_t i) const = 0;
	virtual int Get_deltaBidLimitSize(size_t i, int y = 0) const = 0;
	virtual int Get_deltaAskLimitSize(size_t i, int y = 0) const = 0;
	virtual int Get_deltaBidLimitSum(size_t i) const = 0;
	virtual int Get_deltaAskLimitSum(size_t i) const = 0;
	virtual UINT Get_maxDomLimit(size_t i) const = 0;
	virtual UINT Get_minDomLimit(size_t i) const = 0;
	virtual UINT Get_maxDomOrder(size_t i) const = 0;
	virtual UINT Get_minDomOrder(size_t i) const = 0;
	virtual char Get_updateType(size_t i) const = 0;
	virtual char Get_orderType(size_t i) const = 0;

	virtual void GetDOM(void* d) = 0;
	virtual void GetDOMNinja(void* d) = 0;

	virtual void push_back(void* i) = 0;
	virtual void* get_data(size_t i) = 0;
	virtual void erase_data(size_t i) = 0;
	virtual void* create_object() = 0;
	virtual size_t get_size() = 0;

	Instrument_INFO tInfo;

protected:	
	std::string name;
	std::string description;
	int MinTickInc;						//минимальный шаг цены по инструменту
	int precision;						//кол-во значащих знаков после запятой
	int domLevelsCount;					//количество уровней в стакане
	std::string mode;
};

template <class T> 
class TickerData		//шаблонный класс данных
{
public:

#define DATA_SIZE 100000

	TickerData() 
	{
		Data.reserve(DATA_SIZE);
	}
	virtual ~TickerData() {}
	
	void push_back(T i) 
	{ 
		//::AcquireSRWLockExclusive(&SRWLock);
		Data.push_back(i);
		if (Data.size() == DATA_SIZE)
		{
			Data.erase(Data.begin(), Data.begin() + DATA_SIZE/2);
		}
		//::ReleaseSRWLockExclusive(&SRWLock);
	}

	void push_back_history(T i) 
	{ 
		HData.push_back(i);
	}

	//const T operator[](int i) const { return Data[i]; }
	//T& operator[](int i) { return Data[i]; }

	void erase(size_t i) 
	{ 
		//::AcquireSRWLockExclusive(&SRWLock);
		Data.erase(Data.begin()+i);
		//::ReleaseSRWLockExclusive(&SRWLock);
	}

	void insert(size_t i, T x) 
	{ 
		//::AcquireSRWLockExclusive(&SRWLock);
		Data.insert(Data.begin()+i, x);
		//::ReleaseSRWLockExclusive(&SRWLock);
	}

	void CombineData() 
	{
		//сортируем исторические данные
		for (ptrdiff_t i = HData.size()-1, j = 0; j < i; --i, ++j)
		{
			T x = HData[i];
			HData[i] = HData[j];
			HData[j] = x;
		}
		//добавляем данные реальные
		for (size_t i = 0; i < Data.size(); ++i)
			HData.push_back(Data[i]);
		//обмениваем содержимое
		//::AcquireSRWLockExclusive(&SRWLock);
		HData.swap(Data);
		//::ReleaseSRWLockExclusive(&SRWLock);
		//очищаем исторический вектор
		HData.clear();
	}

	size_t get_size() { return Data.size(); }

protected:
	std::vector<T> Data;
	std::vector<T> HData;
};

class RTicker : public IInstrument		//абстрактный базовый класс для всех инструментов от фида Rithmic
{
public:
	RTicker(std::string n = "") : IInstrument(n) 
	{ 
		historyFlag = 0; counter = 100; hour = minute = second = 0; iFlags = 0; 
		tInfo.tradeCondition = true; 
		tInfo.cumulativeVolume = true; 
		type = 0;
	}

	virtual ~RTicker() {};

	void CopyAllParameters(void* x)
	{
		RTicker* i = (RTicker*)x;

		name = i->name;
		description = i->description;
		MinTickInc = i->MinTickInc;						
		precision = i->precision;
		iFlags = i->iFlags;
		exchange = i->exchange;
		expiration = i->expiration;
		mode = i->mode;
		type = i->type;
		historyFlag = i->historyFlag;
		counter = i->counter;
		domLevelsCount = i->domLevelsCount;
	}

	void SetExpiration(const char* s) { expiration = s; }
	//void SetExpTime(const char* s) { expTime = s; }
	void SetExchange(const char* s) { exchange = s; }
	void SetHistoryFlag(int i) { historyFlag = i; }

	std::string GetExpiration() const { return expiration; }
	//std::string GetExpTime() const { return expTime; }
	std::string GetExchange() const { return exchange; }
	int GetHistoryFlag() const { return historyFlag; }

	void GetDOM(void*) {}
	void GetDOMNinja(void*) {}

	void TradePrintNinja(void*) {}

	int iFlags;
	int historyFlag;	//флаг показывающий была ли подкачка по инструменту или нет 0 - история не подкачивалась, 1 - идет подкачка истории, 2 - подкачка завершена
	BYTE counter;		//счетчик для вертикальных надписей для квант чарта
	BYTE hour;			//предыдущее значение времени для сравнения и получения осей на графике квант чарт
	BYTE minute;
	BYTE second;	

protected:
	std::string exchange;
	std::string expiration;
	//std::string expTime;
	int type;
};
class RTradeTicker : public RTicker, public TickerData<RITHMICTRADE>		//класс для всех инструментов от фида Rithmic, которые содержат только трейды
{
public:
	RTradeTicker(std::string n = "") : RTicker(n) 
	{
	}
	virtual ~RTradeTicker() {}

	virtual size_t GetAggregatedOrderbookSize() const { return Data.size(); }
    virtual size_t GetNonAggregationTradesDataSize() const { return 0; }
    virtual size_t GetAggregationTradesDataSize() const { return 0; }
	void TradePrint(void* p);
	
	Price Get_tradePrice(size_t i) const	{ return Data[i].Data.tradePrice; }
	UINT Get_tradeSize(size_t i) const   	{ return Data[i].Data.tradeSize; }
    virtual UINT GetInterestAfterTrade(size_t i) const { return 0; }
	void Add_tradeSize(size_t i, UINT size)	{ Data[i].Data.tradeSize += size; }
	BYTE Get_hour(size_t i) const			{ return Data[i].Data.hour; }
	BYTE Get_minute(size_t i) const		{ return Data[i].Data.minute; }
	BYTE Get_second(size_t i) const		{ return Data[i].Data.second; }
	UINT Get_microseconds(size_t i) const	{ return Data[i].microseconds; }
	UINT Get_microdelta(size_t i) const	{ return Data[i].microDelta; }
	UINT Get_buyVolume(size_t i) const		{ return Data[i].buyVolume; }
	UINT Get_sellVolume(size_t i) const	{ return Data[i].sellVolume; }
	char Get_agressorSide(size_t i) const	{ return Data[i].agressorSide; }
	char Get_impliedSide(size_t i) const	{ return Data[i].impliedSide; }
	char Get_condition(size_t i) const		{ return Data[i].condition; }
	bool Get_yAxis(size_t i) const			{ return Data[i].yAxis; }
	void Set_yAxis(size_t i, bool y)		{  Data[i].yAxis = y; }

	Price Get_bestBidPrice(size_t) const { return 0; }
	Price Get_bestAskPrice(size_t) const { return 0; }
	UINT Get_bestBidSize(size_t) const	{ return 0; }
	UINT Get_bestAskSize(size_t) const	{ return 0; }
	UINT Get_bestBidOrders(size_t) const { return 0; }
	UINT Get_bestAskOrders(size_t) const { return 0; }
	UINT Get_tradeSizeQuant(size_t) const { return 0; }

	Price Get_bidPrice(size_t, int) const { return 0; }
	Price Get_askPrice(size_t, int) const { return 0; }
	UINT Get_bidSize(size_t, int) const { return 0; }
	UINT Get_askSize(size_t, int) const { return 0; }
	UINT Get_bidOrders(size_t, int) const { return 0; }
	UINT Get_askOrders(size_t, int ) const { return 0; }
	int Get_eventBidLevel(size_t ) const { return 0; }
	int Get_eventAskLevel(size_t ) const { return 0; }
	int Get_deltaBidLimitSize(size_t , int  = 0) const { return 0; }
	int Get_deltaAskLimitSize(size_t , int  = 0) const { return 0; }
	int Get_deltaBidLimitSum(size_t ) const { return 0; }
	int Get_deltaAskLimitSum(size_t ) const { return 0; }
	UINT Get_maxDomLimit(size_t ) const { return 0; }
	UINT Get_minDomLimit(size_t ) const { return 0; }
	UINT Get_maxDomOrder(size_t ) const { return 0; }
	UINT Get_minDomOrder(size_t ) const { return 0; }
	char Get_updateType(size_t ) const { return 0; }
	char Get_orderType(size_t ) const { return 0; }

	void push_back(void* i)		{ TickerData<RITHMICTRADE>::push_back(*((_RITHMICTRADE*)i)); }
	void* get_data(size_t i)	{ return (void*)&Data[i]; }
	void erase_data(size_t i)	{ TickerData<RITHMICTRADE>::erase(i); }
	void* create_object()		{ return (new RTradeTicker); }
	size_t get_size()			{ return TickerData<RITHMICTRADE>::get_size(); }
};

class RDOMTicker : public RTicker, public TickerData<RITHMICDOM>			//класс для всех инструментов от фида Rithmic, которые содержат трейды и все уровни стакана
{
	bool skipChanges;		// флаг для пропуска значений изменения стакана после трейда

public:
	RDOMTicker(std::string n = "") : RTicker(n), skipChanges(false)
	{
		tInfo.level2 = true;
		tInfo.ordersNumber = true;
	}
	virtual ~RDOMTicker() {};

	virtual size_t GetAggregatedOrderbookSize() const { return Data.size(); }
    virtual size_t GetNonAggregationTradesDataSize() const { return 0; }
    virtual size_t GetAggregationTradesDataSize() const { return 0; }
	void TradePrint(void* p);
	void LimitOrderBook(RApi::LimitOrderBookInfo* pInfo);
	void AskQuote(RApi::AskInfo* pInfo);
	void BidQuote(RApi::BidInfo* pInfo);

	void WritePreviousBidValue(RITHMICDOM& _d);
	void WritePreviousAskValue(RITHMICDOM& _d);
	void WriteNewData(RITHMICDOM& _d, char Type, const Price& price, int limits, int traders);
	void TradesToLimits(RITHMICDOM& _d);
	void OrderTypeSpec(RITHMICDOM& _d, int type, int size);
	int GetDeltaLimitSize(RITHMICDOM& rd);

	Price Get_tradePrice(size_t i) const	{ return Data[i].Data.Data.tradePrice; }
	UINT Get_tradeSize(size_t i) const		{ return Data[i].Data.Data.tradeSize; }
    virtual UINT GetInterestAfterTrade(size_t i) const { return 0; }
	void Add_tradeSize(size_t i, UINT size)	{ Data[i].Data.Data.tradeSize += size; }
	BYTE Get_hour(size_t i) const			{ return Data[i].Data.Data.hour; }
	BYTE Get_minute(size_t i) const			{ return Data[i].Data.Data.minute; }
	BYTE Get_second(size_t i) const			{ return Data[i].Data.Data.second; }
	UINT Get_microseconds(size_t i) const	{ return Data[i].Data.microseconds; }
	UINT Get_microdelta(size_t i) const		{ return Data[i].Data.microDelta; }
	UINT Get_buyVolume(size_t i) const		{ return Data[i].Data.buyVolume; }
	UINT Get_sellVolume(size_t i) const		{ return Data[i].Data.sellVolume; }
	char Get_agressorSide(size_t i) const	{ return Data[i].Data.agressorSide; }
	char Get_impliedSide(size_t i) const	{ return Data[i].Data.impliedSide; }
	char Get_condition(size_t i) const		{ return Data[i].Data.condition; }
	bool Get_yAxis(size_t i) const			{ return Data[i].Data.yAxis; }
	void Set_yAxis(size_t i, bool y)		{ Data[i].Data.yAxis = y; }

	Price Get_bestBidPrice(size_t i) const	{ return Data[i].bidPrice[0]; }
	Price Get_bestAskPrice(size_t i) const	{ return Data[i].askPrice[0]; }
	UINT Get_bestBidSize(size_t i) const	{ return Data[i].bidSize[0]; }
	UINT Get_bestAskSize(size_t i) const	{ return Data[i].askSize[0]; }
	UINT Get_bestBidOrders(size_t i) const	{ return Data[i].bidOrders[0]; }
	UINT Get_bestAskOrders(size_t i) const	{ return Data[i].askOrders[0]; }
	UINT Get_tradeSizeQuant(size_t i) const { return Data[i].tradeSizeQuant; }

	Price Get_bidPrice(size_t i, int y) const	{ return Data[i].bidPrice[y]; }
	Price Get_askPrice(size_t i, int y) const	{ return Data[i].askPrice[y]; }
	UINT Get_bidSize(size_t i, int y) const		{ return Data[i].bidSize[y]; }
	UINT Get_askSize(size_t i, int y) const		{ return Data[i].askSize[y]; }
	UINT Get_bidOrders(size_t i, int y) const	{ return Data[i].bidOrders[y]; }
	UINT Get_askOrders(size_t i, int y) const	{ return Data[i].askOrders[y]; }
	int Get_eventBidLevel(size_t i) const		{ return Data[i].eventBidLevel; }
	int Get_eventAskLevel(size_t i) const		{ return Data[i].eventAskLevel; }
	int Get_deltaBidLimitSize(size_t i, int y = 0) const { return Data[i].deltaBidLimitSize[y]; }
	int Get_deltaAskLimitSize(size_t i, int y = 0) const { return Data[i].deltaAskLimitSize[y]; }
	int Get_deltaBidLimitSum(size_t i) const;
	int Get_deltaAskLimitSum(size_t i) const;
	UINT Get_maxDomLimit(size_t i) const		{ return Data[i].maxDomLimit; }
	UINT Get_minDomLimit(size_t i) const		{ return Data[i].minDomLimit; }
	UINT Get_maxDomOrder(size_t i) const		{ return Data[i].maxDomOrder; }
	UINT Get_minDomOrder(size_t i) const		{ return Data[i].minDomOrder; }
	char Get_updateType(size_t i) const			{ return Data[i].updateType; }
	char Get_orderType(size_t i) const			{ return Data[i].orderType; }

	void push_back(void* i)						{ TickerData<RITHMICDOM>::push_back(*((_RITHMICDOM*)i)); }
	void* get_data(size_t i)					{ return (void*)&Data[i]; }
	void erase_data(size_t i)					{ TickerData<RITHMICDOM>::erase(i); }
	void* create_object()						{ return (new RDOMTicker); }
	size_t get_size()							{ return TickerData<RITHMICDOM>::get_size(); }

	bool& SkipChanges()							{ return skipChanges; }
};


typedef struct _CQGTRADE { //80 байт
	DATA Data;
	UINT microseconds;
	UINT microDelta;
	bool yAxis;			//там где не равен нулю нужна вертикальная метка с временем	
	char agressorSide;	
} CQGTRADE;

typedef struct _CQGDOM {	//448 байт
	CQGTRADE Data;
	Price bidPrice[11];
	Price askPrice[11];
	UINT bidSize[11];
	UINT askSize[11];
	int deltaBidLimitSize[11];
	int deltaAskLimitSize[11];
	int eventBidLevel;
	int eventAskLevel;
	UINT tradeSizeQuant;
	//int deltaLimitSize;		//на сколько увеличился или уменьшился аск/бид
	UINT maxDomLimit;			//максимальное значение в стакане среди текущих(это нужно для визуализации хит меп)
	UINT minDomLimit;			//минимальное значение в стакане среди текущих
	char orderType;				//тип ордера определяемый нашим алгоритмом: 1 - простой 2 - смещение, 3 - алго-ордер 
} CQGDOM;


class CQGTicker : public IInstrument
{
public:
	CQGTicker(std::string n = "") : IInstrument(n) 
	{ 
		/*historyFlag = 0;*/ counter = 100; hour = minute = second = 0; 
		instrumentID = 0;
		exchID = 0;
		type = 0;
	}
	virtual ~CQGTicker() {};

	virtual void TradePrint(void* p) = 0;

	int GetHistoryFlag() const { return 0; }

	void CopyAllParameters(void* x)
	{
		CQGTicker* i = (CQGTicker*)x;

		name = i->name;
		description = i->description;
		MinTickInc = i->MinTickInc;						
		precision = i->precision;
		exchange = i->exchange;
		expiration = i->expiration;
		mode = i->mode;
		type = i->type;
		//historyFlag = i->historyFlag;
		counter = i->counter;
	}

	//void SetFullName(wchar_t* str) { fullName.append(str); }
	void SetExpiration(const char* s) { expiration = s; }
	void SetExchangeID(UINT s) { exchID = s; }
	void SetExchange(const std::string& s) { exchange = s; }
	void SetMode(const char* s) { mode = s; }
	void SetID(long i) { instrumentID = i; }
	void SetStrID(const std::string& s) { instrumentStrID = s; }

	//std::wstring GetFullName() const { return fullName; }
	std::string GetMode() const { return mode; }
	std::string GetExpiration() const { return expiration; }
	UINT GetExchangeID() const { return exchID; }
	std::string GetExchange() const { return exchange; }
	long GetID() const { return instrumentID; }
	std::string GetStrID() const { return instrumentStrID; }

	void SetLastAsk(double price) { lastAsk.SetPrice(price, GetPrecision()); }
	void SetLastBid(double price) { lastBid.SetPrice(price, GetPrecision()); }

	//int historyFlag;	//флаг показывающий была ли подкачка по инструменту или нет 0 - история не подкачивалась, 1 - идет подкачка истории, 2 - подкачка завершена
	BYTE counter;		//счетчик для вертикальных надписей для квант чарта
	BYTE hour;			//предыдущее значение времени для сравнения и получения осей на графике квант чарт
	BYTE minute;
	BYTE second;	

	Price lastBid;
	Price lastAsk;

protected:
	//std::wstring fullName;
	long instrumentID;
	std::string instrumentStrID;
	UINT exchID;
	std::string exchange;
	std::string expiration;
	std::string mode;
	int type;
};

class CQGTradeTicker : public CQGTicker, public TickerData<CQGTRADE>		//класс для всех инструментов от фида CQG, которые содержат только трейды
{
public:
	CQGTradeTicker(std::string n = "") : CQGTicker(n)
	{
	}
	virtual ~CQGTradeTicker() {}

	virtual size_t GetAggregatedOrderbookSize() const { return Data.size(); }
    virtual size_t GetNonAggregationTradesDataSize() const { return 0; }
    virtual size_t GetAggregationTradesDataSize() const { return 0; }
	void TradePrint(void* p);
	void TradePrintNinja(void* p);

	Price Get_tradePrice(size_t i) const	{ return Data[i].Data.tradePrice; }
	UINT Get_tradeSize(size_t i) const		{ return Data[i].Data.tradeSize; }
    virtual UINT GetInterestAfterTrade(size_t i) const { return 0; }
	void Add_tradeSize(size_t i, UINT size)	{ Data[i].Data.tradeSize += size; }
	BYTE Get_hour(size_t i) const			{ return Data[i].Data.hour; }
	BYTE Get_minute(size_t i) const		{ return Data[i].Data.minute; }
	BYTE Get_second(size_t i) const		{ return Data[i].Data.second; }
	UINT Get_microseconds(size_t i) const	{ return Data[i].microseconds; }
	UINT Get_microdelta(size_t i) const	{ return Data[i].microDelta; }
	UINT Get_buyVolume(size_t ) const		{ return 0; }
	UINT Get_sellVolume(size_t ) const	{ return 0; }
	char Get_agressorSide(size_t i) const	{ return Data[i].agressorSide; }
	char Get_impliedSide(size_t ) const	{ return 0; }
	char Get_condition(size_t ) const		{ return 0; }
	bool Get_yAxis(size_t i) const			{ return Data[i].yAxis; }
	void Set_yAxis(size_t i, bool y)		{  Data[i].yAxis = y; }

	Price Get_bestBidPrice(size_t ) const { return 0; }
	Price Get_bestAskPrice(size_t ) const { return 0; }
	UINT Get_bestBidSize(size_t ) const	{ return 0; }
	UINT Get_bestAskSize(size_t ) const	{ return 0; }
	UINT Get_bestBidOrders(size_t ) const { return 0; }
	UINT Get_bestAskOrders(size_t ) const { return 0; }
	UINT Get_tradeSizeQuant(size_t ) const { return 0; }

	Price Get_bidPrice(size_t , int ) const { return 0; }
	Price Get_askPrice(size_t , int ) const { return 0; }
	UINT Get_bidSize(size_t , int ) const { return 0; }
	UINT Get_askSize(size_t , int ) const { return 0; }
	UINT Get_bidOrders(size_t , int ) const { return 0; }
	UINT Get_askOrders(size_t , int ) const { return 0; }
	int Get_eventBidLevel(size_t ) const { return 0; }
	int Get_eventAskLevel(size_t ) const { return 0; }
	int Get_deltaBidLimitSize(size_t , int  = 0) const { return 0; }
	int Get_deltaAskLimitSize(size_t , int  = 0) const { return 0; }
	int Get_deltaBidLimitSum(size_t ) const { return 0; }
	int Get_deltaAskLimitSum(size_t ) const { return 0; }
	UINT Get_maxDomLimit(size_t ) const { return 0; }
	UINT Get_minDomLimit(size_t ) const { return 0; }
	UINT Get_maxDomOrder(size_t ) const { return 0; }
	UINT Get_minDomOrder(size_t ) const { return 0; }
	char Get_updateType(size_t ) const { return 0; }
	char Get_orderType(size_t ) const { return 0; }

	void GetDOM(void* ) { return; }
	void GetDOMNinja(void* ) { return; }

	void push_back(void* i) { TickerData<CQGTRADE>::push_back(*((_CQGTRADE*)i)); }
	void* get_data(size_t i) { return (void*)&Data[i]; }
	void erase_data(size_t i) { TickerData<CQGTRADE>::erase(i); }
	void* create_object() { return (new CQGTradeTicker); }
	size_t get_size() { return TickerData<CQGTRADE>::get_size(); }
};

class CQGDOMTicker : public CQGTicker, public TickerData<CQGDOM>			//класс для всех инструментов от фида CQG, которые содержат трейды и все уровни стакана
{
public:
	CQGDOMTicker(std::string n = "") : CQGTicker(n) 
	{
		tInfo.level2 = true;
		lastSyncTradeIndx = 0;
	}
	virtual ~CQGDOMTicker() {}

	virtual size_t GetAggregatedOrderbookSize() const { return Data.size(); }
    virtual size_t GetNonAggregationTradesDataSize() const { return 0; }
    virtual size_t GetAggregationTradesDataSize() const { return 0; }
	void TradePrint(void* p);
	void TradePrintNinja(void* p);

	void WritePreviousBidValue(CQGDOM& _d);
	void WritePreviousAskValue(CQGDOM& _d);
	void WriteNewData(CQGDOM& _d, char Type, const Price& price, UINT limits, int nPos, wchar_t type);
	void TradesToLimits(CQGDOM& _d);

	Price Get_tradePrice(size_t i) const	{ return Data[i].Data.Data.tradePrice; }
	UINT Get_tradeSize(size_t i) const      { return Data[i].Data.Data.tradeSize; }
    virtual UINT GetInterestAfterTrade(size_t i) const { return 0; }
	void Add_tradeSize(size_t i, UINT size)	{ Data[i].Data.Data.tradeSize += size; }
	BYTE Get_hour(size_t i) const			{ return Data[i].Data.Data.hour; }
	BYTE Get_minute(size_t i) const			{ return Data[i].Data.Data.minute; }
	BYTE Get_second(size_t i) const			{ return Data[i].Data.Data.second; }
	UINT Get_microseconds(size_t i) const	{ return Data[i].Data.microseconds; }
	UINT Get_microdelta(size_t i) const		{ return Data[i].Data.microDelta; }
	UINT Get_buyVolume(size_t ) const		{ return 0; }
	UINT Get_sellVolume(size_t ) const		{ return 0; }
	char Get_agressorSide(size_t i) const	{ return Data[i].Data.agressorSide; }
	char Get_impliedSide(size_t ) const		{ return 0; }
	char Get_condition(size_t ) const		{ return 0; }
	bool Get_yAxis(size_t i) const			{ return Data[i].Data.yAxis; }
	void Set_yAxis(size_t i, bool y)		{  Data[i].Data.yAxis = y; }

	Price Get_bestBidPrice(size_t ) const { return 0; }
	Price Get_bestAskPrice(size_t ) const { return 0; }
	UINT Get_bestBidSize(size_t ) const	{ return 0; }
	UINT Get_bestAskSize(size_t ) const	{ return 0; }
	UINT Get_bestBidOrders(size_t ) const { return 0; }
	UINT Get_bestAskOrders(size_t ) const { return 0; }
	UINT Get_tradeSizeQuant(size_t i) const { return Data[i].tradeSizeQuant; }

	Price Get_bidPrice(size_t i, int y) const	{ return Data[i].bidPrice[y]; }
	Price Get_askPrice(size_t i, int y) const	{ return Data[i].askPrice[y]; }
	UINT Get_bidSize(size_t i, int y) const	{ return Data[i].bidSize[y]; }
	UINT Get_askSize(size_t i, int y) const	{ return Data[i].askSize[y]; }
	UINT Get_bidOrders(size_t , int ) const	{ return 0; }
	UINT Get_askOrders(size_t , int ) const	{ return 0; }
	int Get_eventBidLevel(size_t i) const	{ return Data[i].eventBidLevel; }
	int Get_eventAskLevel(size_t i) const	{ return Data[i].eventAskLevel; }
	int Get_deltaBidLimitSize(size_t i, int y = 0) const { return Data[i].deltaBidLimitSize[y]; }
	int Get_deltaAskLimitSize(size_t i, int y = 0) const { return Data[i].deltaAskLimitSize[y]; }
	int Get_deltaBidLimitSum(size_t i) const;
	int Get_deltaAskLimitSum(size_t i) const;
	UINT Get_maxDomLimit(size_t i) const		{ return Data[i].maxDomLimit; }
	UINT Get_minDomLimit(size_t i) const		{ return Data[i].minDomLimit; }
	UINT Get_maxDomOrder(size_t ) const		{ return 0; }
	UINT Get_minDomOrder(size_t ) const		{ return 0; }
	char Get_updateType(size_t ) const		{ return 0; }
	char Get_orderType(size_t i) const			{ return Data[i].orderType; }

	void GetDOM(void* d);
	void GetDOMNinja(void* d);

	void push_back(void* i) { TickerData<CQGDOM>::push_back(*((_CQGDOM*)i)); }
	void* get_data(size_t i) { return (void*)&Data[i]; }
	void erase_data(size_t i) { TickerData<CQGDOM>::erase(i); }
	void* create_object() { return (new CQGDOMTicker); }
	size_t get_size() { return TickerData<CQGDOM>::get_size(); }

	size_t lastSyncTradeIndx;	//индекс последнего синхронизированного трейда в векторе
};