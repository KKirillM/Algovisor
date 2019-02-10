#pragma once
#include "TradeSessionData.h"
//#include ".\messages\forts_messages.h" // FutAddOrder
#include "StatisticClasses.h"

typedef int InstrumentID;

struct SOrderEx
{
    enum OrderType
    {
        FAS = 1,    // котировочная заявка (остаётся в очереди после частичного сведения)
        IOC = 2,    // встречная заявка (снимается после проведения аукциона)
        FOK = 3     // заявка Fill-or-Kill
    };

	enum Status
	{
		PENDING,
		SUBMITTED,
		FILLED,
		CANCELED,
		REJECTED_ADD,
		REJECTED_CANCLE,
		REJECTED_MOVE
	};

    OrderType type;
	Status status;
    P2Dir dir;
    t_id order_id;
    double initialPrice;						// исходная цена в приказе
    double realPrice;							// цена по которой ордер встал в стакан или начал сведение
    uint64_t remainedQuantity;					// оставшийся размер заявки
    uint64_t initialQuantity;					// исходный размер заявки
	fo2base::UnixNanosecondsTime sendTime;      // локальное время отправления заявки
    //fo2base::UnixNanosecondsTime coreTime;    // ядерное время получения заявки
	fo2base::UnixNanosecondsTime responseTime;  // локальное время получения ответа о событии с заявкой

    SOrderEx(const double _price, const uint64_t _quantity, const t_id _order_id, const SOrderEx::OrderType _type, const P2Dir _dir, const int64_t _sendTime, const Status _status = PENDING)
        : initialPrice(_price), realPrice(0), initialQuantity(_quantity), remainedQuantity(_quantity), order_id(_order_id), type(_type), dir(_dir), sendTime(_sendTime), /*coreTime(0),*/ status(_status) {}

    //void SetCoreTime(const int64_t time) { coreTime = time; }

    /*struct Comparator
    {
        bool operator()(const SOrderEx& order1, const SOrderEx& order2)
        {
            return order1.order_id < order2.order_id;
        }
    };*/
};

enum ActionType
{
    ADD_ORDER,
    CANCEL_ORDER,
    MASS_CANCEL,
    MOVE_ORDER
};

struct Action
{
    ActionType type;
    InstrumentID isin_id;
    std::set<t_id> ids;
};

struct Position
{
    int buyPos;
    int sellPos;
	double WAPrice;
	double PnL;

    Position(): PnL(0.0), buyPos(0), sellPos(0), WAPrice(0.0) {}

    void AddDeal(const bool is_long, const int qty, const double price)
    {
		const double vol = qty * price;
		if ((is_long && GetPos() >= 0) || (!is_long && GetPos() <= 0)) // долив
		{
			WAPrice = (WAPrice == 0) ? (vol / qty) : (WAPrice + vol) / (qty + 1);
		}
		else 
		{
			if (GetPos() > 0)
			{
				PnL += (price - WAPrice) * qty;
			}
			else
			{
				PnL += (WAPrice - price) * qty;
			}

			if (std::abs(GetPos()) == qty)	// полное закрытие
			{
				WAPrice = 0.0;				
			}
		}

		if (is_long)
		{
			buyPos += qty;
		}
		else
		{
			sellPos += qty;
		}
    }

    int GetPos() const // вернуть знаковую текущую позу: + для лонга и - для шорта
    {
        return buyPos - sellPos;
    }

    int GetClosedPos() const // вернуть число закрытых поз
    {
        if (buyPos > sellPos)
        {
            return sellPos; // если поза лонг, то мы закрыли по числу шортов
        }
        else
        {
            return buyPos; // если поза шорт, то закрыли по числу лонгов
        }
    }

    double GetClosedPnL() const
    {
        return PnL;
    }

    double GetWAPrice() const
    {
		return WAPrice;
    }    

    double GetUnrealisedPnL(const double price) const
    {
		double _PnL = PnL;
		if (GetPos() > 0)
		{
			_PnL += (price - WAPrice) * std::abs(GetPos());
		}
		else if (GetPos() < 0)
		{
			_PnL += (WAPrice - price) * std::abs(GetPos());
		}

        return _PnL;
    }

    std::string GetPositionInfo() const
    {
        std::string str;
        str = "TotalPnL: "       + std::to_string(GetClosedPnL()) + ",\n" +
              "WAPrice: "        + std::to_string(GetWAPrice()) + ",\n" +
              "Open position: "  + std::to_string(GetPos()) + ",\n" +
              "Total trades: "   + std::to_string(buyPos + sellPos) + ",\n" +
              "Buy trades: "     + std::to_string(buyPos) + ",\n" +
              "Sell trades: "    + std::to_string(sellPos) + '\n';
        return str;
    }

};  // Position

struct PositionEx : Position
{
    int maxPositivePos;                // максимальная позиция в лонг за сессию
    int maxNegativePos;                // максимальная позиция в шорт за сессию
    double maxRealisedPositivePnL;     // максимальный положительный зафиксированный PnL за сессию
    double maxRealisedNegativePnL;     // максимальный отрицательный зафиксированный PnL за сессию
    //double maxUnrealisedPositivePnL;   // максимальный положительный возможный PnL за сессию при ликвидации позиции в самой удачной точке рынка
    double maxUnrealisedNegativePnL;   // максимальный отрицательный возможный PnL за сессию при ликвидации позиции в самой неудачной точке рынка

    PositionEx() { memset(this, 0, sizeof(PositionEx)); }

    void AddDeal(const bool is_long, const int qty, const double price)
    {
        Position::AddDeal(is_long, qty, price);
        if (buyPos == sellPos) return;
        CalculateStatParams();
    }

    void CalculateStatParams()
    {
        const double pnl = GetClosedPnL();
        maxRealisedPositivePnL = max(maxRealisedPositivePnL, pnl);
        maxRealisedNegativePnL = min(maxRealisedNegativePnL, pnl);

        const int pos = GetPos();
        maxPositivePos = max(maxPositivePos, pos);
        maxNegativePos = min(maxNegativePos, pos);
    }

    void CalculateLiquidationPnL(const double price)
    {
        const double liqudationPnL = GetUnrealisedPnL(price);
        //maxUnrealisedPositivePnL = max(maxUnrealisedPositivePnL, liqudationPnL);
        maxUnrealisedNegativePnL = min(maxUnrealisedNegativePnL, liqudationPnL);
    }

    std::string GetPositionExInfo() const
    {
        std::string str;
        str = "maxPositivePnL: " + std::to_string(maxRealisedPositivePnL) + ",\n" +
              "maxNegativePnL: " + std::to_string(maxRealisedNegativePnL) + ",\n" +
              "maxPositivePos: " + std::to_string(maxPositivePos) + ",\n" +
              "maxNegativePos: " + std::to_string(maxNegativePos) + ",\n" +
              /*"maxPositiveLiquidationPnL: " + std::to_string(maxUnrealisedPositivePnL) + ",\n" +*/
              "maxNegativeLiquidationPnL: " + std::to_string(maxUnrealisedNegativePnL);
        return str;
    }

};  // PositionEx

class TradeSessionData;

class TradingInterface
{
public:
    TradingInterface(TradeSessionData& data) : sessionData(data) {}

    virtual void AddOrder(const InstrumentID isin_id, const t_id order_id) = 0;
    virtual void CancelOrder(const InstrumentID isin_id, const t_id order_id) = 0;
    virtual void MassCancelOrder(const InstrumentID isin_id, const std::set<t_id>& order_ids) = 0;
    virtual void MoveOrders(const InstrumentID isin_id, const std::set<t_id>& order_ids) = 0;

    virtual void OrderFilled(const InstrumentID isin_id, const t_id order_id, const fo2base::UnixNanosecondsTime coreTime) = 0;

    SOrderEx& CreateOrder(const InstrumentID isin_id, const double price, const uint64_t size, const t_id order_id, const SOrderEx::OrderType type, const P2Dir dir, const int64_t sendTime)
    { 
        // создаём новый ордер, помещаем его сразу в контейнер ордеров allOrders и возвращаем ссылку на созданный ордер
        auto& iteratorPair = allOrders[isin_id].insert(std::make_pair(order_id, SOrderEx(price, size, order_id, type, dir, sendTime)));
        return (*iteratorPair.first).second;
    }

    SOrderEx& GetOrder(const InstrumentID isin_id, const t_id order_id) { return allOrders.at(isin_id).at(order_id); }

    const Position* GetIntermediatePositionResult(const InstrumentID isin_id, const fo2base::UnixNanosecondsTime time) const;
    const SOrderEx* GetTrade(const InstrumentID isin_id, const fo2base::UnixNanosecondsTime time) const;

    void UpdatePnL(const InstrumentID isin_id, const P2Action action, const char* deal_price, const fo2base::UnixNanosecondsTime time);

protected:
    TradeSessionData& sessionData;                              // исходные данные для анализа стратегиями

    std::map<InstrumentID, std::map<t_id, SOrderEx>> allOrders; // все ордера какие были созданы в рамках стратегии
    std::map<InstrumentID, std::set<t_id>> submittedOrders;     // ордера, которые стоят в стаканах, но ещё не исполнены полностью (в порядке выставления)
    std::map<InstrumentID, std::set<t_id>> completedOrders;     // ордера, которые были полностью сведены в сделку или сняты из стакана
    std::map<InstrumentID, PositionEx> positions;               // текущие позиции по инструментам

    std::map<InstrumentID, std::map<fo2base::UnixNanosecondsTime, Position>> positionChanges;      // все значения изменения позиций по инструментам за сессию
    std::map<InstrumentID, std::map<fo2base::UnixNanosecondsTime, SOrderEx*>> allTrades;           // все сделки порождённые в рамках стратегии за сессию

    std::map<ActionType, std::set<SOrderEx*>> rejectionCounter; // подсчёт отвергнутых операций

};  // SendOrderInterface

class RecieveResultInterface
{
public:
    virtual void ConfirmAdd(const InstrumentID isin_id, const t_id order_id, const fo2base::UnixNanosecondsTime localTime) = 0;
    virtual void ConfirmCancel(const InstrumentID isin_id, const t_id order_id, const fo2base::UnixNanosecondsTime localTime) = 0;
    virtual void ConfirmMassCancel(const InstrumentID isin_id, const std::set<t_id>& order_ids, const fo2base::UnixNanosecondsTime localTime) = 0;
    virtual void ConfirmMove(const InstrumentID isin_id, const std::set<t_id>& order_ids, const fo2base::UnixNanosecondsTime localTime) = 0;

    virtual void RejectAdd(const InstrumentID isin_id, const t_id order_id, const fo2base::UnixNanosecondsTime localTime) = 0;
    virtual void RejectCancel(const InstrumentID isin_id, const t_id order_id, const fo2base::UnixNanosecondsTime localTime) = 0;
    virtual void RejectMassCancel(const InstrumentID isin_id, const std::set<t_id>& order_ids, const fo2base::UnixNanosecondsTime localTime) = 0;
    virtual void RejectMove(const InstrumentID isin_id, const std::set<t_id>& order_ids, const fo2base::UnixNanosecondsTime localTime) = 0;

};  // RecieveResultInterface

class StrategyLogger
{
protected:
	const std::string logPath;							// путь к файлам лога
	std::unique_ptr<FileLogger> strategyResultLogger;	// лог результата бэктеста
	std::unique_ptr<FileLogger> strategyOrdlogLogger;	// лог всех торговых приказов и ответов на них, порождаемых стратегией
	std::unique_ptr<FileLogger> strategyTradesLogger;	// лог всех трейдов, порождаемых стратегией

public:

#define BACKTEST_RESULT_LOG(message, ...) PRINT_TO_LOG((*strategyResultLogger.get()), LOG_TRACE, message, __VA_ARGS__)
#define BACKTEST_ORDLOG_LOG(message, ...) PRINT_TO_LOG((*strategyOrdlogLogger.get()), LOG_TRACE, message, __VA_ARGS__)
#define BACKTEST_TRADES_LOG(message, ...) PRINT_TO_LOG((*strategyTradesLogger.get()), LOG_TRACE, message, __VA_ARGS__)

	StrategyLogger(const std::string& pathToLog) : logPath(pathToLog) {}

	std::string GetPathToLog() const { return logPath; }

	void InitLogs(const fo2base::UnixNanosecondsTime localTime)
	{
		const std::string dateString = fo2base::CP2Time::FromUnixNanosecondsToDateString(localTime, '-');
		strategyResultLogger.reset(new FileLogger(logPath, "strategy_result(" + dateString + ").csv"));
		strategyOrdlogLogger.reset(new FileLogger(logPath, "strategy_ordlog(" + dateString + ").csv"));
		strategyTradesLogger.reset(new FileLogger(logPath, "strategy_trades(" + dateString + ").csv"));
	}

	void StrategyOrdlogToLog(const fo2base::UnixNanosecondsTime time, const InstrumentID isin_id, const std::string& action, const SOrderEx& order);
	void StrategyResultToLog(const std::string& str) { if (strategyResultLogger) BACKTEST_RESULT_LOG("%s", str.c_str()) }
	void StrategyTradesToLog(const fo2base::UnixNanosecondsTime time, const InstrumentID isin_id, const double price, const uint64_t quantity, const bool is_long, const int posSize, const int pnl);

};	// StrategyLogger

class TestStrategyBaseClass :  public TradingInterface, public RecieveResultInterface, public StrategyLogger
{
public:
    TestStrategyBaseClass(const fo2base::UnixNanosecondsTime& delay, TradeSessionData& data, const std::string& pathToLog) : 
		TradingInterface(data), 
		StrategyLogger(pathToLog),
		timeDelay(delay), 
		nextID(0), 
		waitActionResult(false) {}

	virtual void InitStrategy(const fo2base::UnixNanosecondsTime localTime) = 0;

    void PreEventActions(const FORTS_ORDLOG_REPL::orders_log& info);
	virtual void PreEventLogic(const FORTS_ORDLOG_REPL::orders_log& info) = 0;
    virtual void PostEventActions(const FORTS_ORDLOG_REPL::orders_log& info) = 0;
    virtual void PostEventActions(const FORTS_DEALS_REPL::deal& info) = 0;
	virtual void EndSessionAction(const std::string& date) = 0;
	virtual void EndSessionRangeAction() = 0;
    virtual std::string GetStrategyResult() const = 0;

    virtual void AddOrder(const InstrumentID isin_id, const t_id order_id);
    virtual void CancelOrder(const InstrumentID isin_id, const t_id order_id);
    virtual void MassCancelOrder(const InstrumentID isin_id, const std::set<t_id>& order_ids);
    virtual void MoveOrders(const InstrumentID isin_id, const std::set<t_id>& order_ids);

	virtual void OrderFilled(const InstrumentID isin_id, const t_id order_id, const fo2base::UnixNanosecondsTime coreTime);

    virtual void ConfirmAdd(const InstrumentID isin_id, const t_id order_id, const fo2base::UnixNanosecondsTime localTime);
    virtual void ConfirmCancel(const InstrumentID isin_id, const t_id order_id, const fo2base::UnixNanosecondsTime localTime);
    virtual void ConfirmMassCancel(const InstrumentID isin_id, const std::set<t_id>& order_ids, const fo2base::UnixNanosecondsTime localTime);
    virtual void ConfirmMove(const InstrumentID isin_id, const std::set<t_id>& order_ids, const fo2base::UnixNanosecondsTime localTime);

    virtual void RejectAdd(const InstrumentID isin_id, const t_id order_id, const fo2base::UnixNanosecondsTime localTime);
    virtual void RejectCancel(const InstrumentID isin_id, const t_id order_id, const fo2base::UnixNanosecondsTime localTime);
    virtual void RejectMassCancel(const InstrumentID isin_id, const std::set<t_id>& order_ids, const fo2base::UnixNanosecondsTime localTime);
    virtual void RejectMove(const InstrumentID isin_id, const std::set<t_id>& order_ids, const fo2base::UnixNanosecondsTime localTime);

    const t_id GetNextOrderID() { return ++nextID; }

	const double* GetIndicator1(const InstrumentID isin_id, const fo2base::UnixNanosecondsTime time) const;
	const double* GetIndicator2(const InstrumentID isin_id, const fo2base::UnixNanosecondsTime time) const;
	void SetIndicator1(const InstrumentID isin_id, const fo2base::UnixNanosecondsTime time, const double value) { indicator1Map[isin_id][lastEventTime] = value; }
	void SetIndicator2(const InstrumentID isin_id, const fo2base::UnixNanosecondsTime time, const double value) { indicator2Map[isin_id][lastEventTime] = value; }

private:
    void CheckPendingActions();
    void CheckAdd(const InstrumentID isin_id, const t_id order_id, const fo2base::UnixNanosecondsTime localTime);
    void CheckCancel(const InstrumentID isin_id, const t_id order_id, const fo2base::UnixNanosecondsTime localTime);
    void CheckMassCancel(const InstrumentID isin_id, const std::set<t_id>& order_ids, const fo2base::UnixNanosecondsTime localTime);
    void CheckMove(const InstrumentID isin_id, const std::set<t_id>& order_ids, const fo2base::UnixNanosecondsTime localTime);
    //void CheckFilling(const SOrder* order, const bool isBidOrder, const OrderType type);

protected:
	std::map<InstrumentID, std::map<fo2base::UnixNanosecondsTime, double>> indicator1Map;			// значения индикатора в момент прохождения трейда в ордерлоге
	std::map<InstrumentID, std::map<fo2base::UnixNanosecondsTime, double>> indicator2Map;			// значения индикатора в момент прохождения трейда в ордерлоге

    std::map<InstrumentID, std::map<fo2base::UnixNanosecondsTime, Action>> pendingActions;			// приказы, которые в ожидании приёма ядром биржи
    
    const fo2base::UnixNanosecondsTime& timeDelay; // задержка между посылом приказа и полученем его ядром биржи в наносекундах, а также между ответом ядра и получением клиента ответа
    fo2base::UnixNanosecondsTime lastEventTime;
    
    t_id nextID;    // id следующего ордера

	bool waitActionResult;		// флаг сигнализирующий, что было послан приказ и надо дождаться ответа о его обработке ядром

};  // TradingStrategyBase

class MyFirstStrategy : public TestStrategyBaseClass
{
  //  struct StrategyParams
  //  {
		//enum Zone
		//{
		//	ABOVE, BETWEEN, UNDER
		//};
		//Zone zone;									// текущее положение значения относительно зоны
		//bool zoneTradeFlag;							// флаг показывающий был ли уже трейд внутри зоны
		//int bigTradeSize;							// минимальный размер 5% самых крупных трейдов по данному инструменту
		//int maxPositionSize;						// максимальный размер позиции по одному инструменту для стратегии
		//uint64_t windowTimeInterval;				// интервал времени, который включает окно
		//int windowTradesCount;						// кол-во трейдов в окне
		////std::deque<AggregatedTrade> longWindow;		// окно содержащее трейды в лонг на открытие позиции
		////std::deque<AggregatedTrade> shortWindow;	// окно содержащее трейды в шорт на открытие позиции
		//StdDev<int> ordersChannel;					// содержит значения разницы суммарного кол-ва ордеров на аске и на биде для расчёта средней и отклонений
		//DeltaValues<int64_t> timeChannel;			// содержит значения разницы времени между первым и последним трейдом в окне
		//DeltaValues<int64_t> spredChangesChannel;	// содержит кол-во изменений српэда в окне
		//t_id lastDealID;							// id последней обработанной сделки
		//VPIN_Indicator_v1 vpin1;					// индикатор VPIN
		//VPIN_Indicator_v1 vpin2;					// индикатор VPIN
		//DeltaValues<double> windowPriceSpread;		// оконный индикатор спрэда, для вычисления используется интервал времени - окно, в котором определяются максимальные и минимальные значения цены
		//ExpMovingAvarage<double> vpinMoving1;		// скользящая для индикатора VPIN
		//ExpMovingAvarage<double> vpinMoving2;		// скользящая для индикатора VPIN
		//SpreadEventRatio spreadIndicator;			// индикатор направления движения спрэда
		//ExpMovingAvarage<double> spreadMoving;		// скользящая для индикатора SpreadEventRatio
		//Correlation<double> correlation;			// расчёт корреляции между двумя величинами

		//StrategyParams(const int tradeSize,
		//	const int posSize,
		//	const uint64_t timeInterval,
		//	const int tradesCount,
		//	const unsigned int vpin1Volume,
		//	const unsigned int vpin2Volume,
		//	const InterestDirection vpin1OI,
		//	const InterestDirection vpin2OI)
		//	: bigTradeSize(tradeSize)
		//	, maxPositionSize(posSize)
		//	, lastDealID(0)
		//	, windowTimeInterval(timeInterval)
		//	, ordersChannel(tradesCount)
		//	, timeChannel(tradesCount)
		//	, spredChangesChannel(tradesCount)
		//	, windowPriceSpread(tradesCount)
		//	, vpin1(vpin1Volume/*, 1000000ll * 1000*/ /** 60*//*, vpin1OI*/)
		//	, vpin2(vpin2Volume/*, vpin2OI*/)
		//	, spreadIndicator(1000/*, 1000000ll * 1000*/ /** 60*/)
		//	, vpinMoving1(100)
		//	, vpinMoving2(1000)
		//	, spreadMoving(100)
		//	, correlation(500)
		//	, prevPrice1(0), prevPrice2(0)
		//	, zone(BETWEEN)
		//	, zoneTradeFlag(false) {}
  //  };

// ***********************************************************
	//struct StrategyParams
	//{
	//	Price prevBidPrice;
	//	Price prevAskPrice;
	//	double avgPrice;
	//	double minStep;	// шаг цены инструмента

	//	// переменные для расчёта волатильности
	//	//TimeRangeDeltaValues<double> priceRange;	// разница между макс и мин ценой за временной интервал в пунктах (т.е. сколько пунктов проходит инструмент за интервал)
	//	//TimeMovingAvarage <double> timeVola;		// среднее значение кол-ва пунктов, которое проходит инструмент за временной интервал

	//	TimeVolatility<double> vola;				// волатильность

	//	StrategyParams(const double step) : avgPrice(0),  minStep(step), vola(600, 1000000ll * 1000 * 30)/*priceRange(1000000ll * 1000), timeVola(1000000ll * 1000 * 60 * 10)*/ {}
	//};

// ***********************************************************

	struct StrategyParams
	{
		t_id lastDealID;									// id последней обработанной сделки
		fo2base::UnixNanosecondsTime timeToExitAfterEnter;	// время для выхода после входа в сделку
		const int priceLevelsAbsorb = 5;					// минимальное количество поглащённых ценовых уровней, которые рассматриваем в рамках стратегии
		int maxPositionSize;								// максимальный размер позиции
		SOrderEx::OrderType orderType;						// тип ордера для заявок на открытие позиции
		double minStep;										// шаг цены инструмента
		SOrderEx* closePosOrder;							// ордер, который посылается при закрытии позиции

		StrategyParams(const double step) : 
			minStep(step), 
			closePosOrder(NULL),
			lastDealID(0), 
			timeToExitAfterEnter(1000000ll * 1000), 
			maxPositionSize(1), 
			orderType(SOrderEx::FAS) {}
	};

// ***********************************************************

	typedef int PriceLevelsCount;		// кол-во поглащённых уровней
	struct ArbitrageParams
	{
		int MIX_PriceLevelsCount;		// кол-во уровней, в том же направлении прошёл MIX
		int MXI_PriceLevelsCount;
		int RTS_OrdersAbsorbCount;		// кол-во ордеров, которые поглотил трейд на РТС
		//int MIX_OrdersAbsorbCount;	// кол-во ордеров, в том же направлении прошёл MIX
		//int MXI_OrdersAbsorbCount;
		int RTS_SizeAbsorbCount;		// кол-во контрактов, которые поглотил трейд на РТС
		//int MIX_SizeAbsorbCount;		// кол-во контрактов, в том же направлении прошёл MIX
		//int MXI_SizeAbsorbCount;
		ArbitrageParams() { memset(this, 0, sizeof(ArbitrageParams)); }
	};

	std::multimap<PriceLevelsCount, std::unique_ptr<ArbitrageParams>> backtestStatistics;

	ArbitrageParams* lastArbParams;
	const int priceLevelsAbsorb = 5;					// минимальное количество поглащённых ценовых уровней
	t_id lastDealID;
	double startMXIPrice;
	double startMIXPrice;
	char agressorSide;									// направление последней поглощающей сделки
	fo2base::UnixNanosecondsTime lastAbsorbTradeTime;	// время последней поглощающей сделки
	const fo2base::UnixNanosecondsTime startDeltaTime = 1000000ll * 5;		// начальный интервал смещения времени от поглощающей сделки
	const fo2base::UnixNanosecondsTime endDeltaTime = 1000000ll * 10000;	//  500, 1000, 5000 конечный интервал смещения времени



	const int Si_isin_id = 880556;	// Si - 12.17
	const int RTS_isin_id = 836971; // RTS - 12.17
	const int MXI_isin_id = 986770; // MXI - 12.17 - mini
	const int MIX_isin_id = 986776; // MIX - 12.17

	std::map<InstrumentID, StrategyParams> params;    // список исинов торгуемых данной стратегией и значения базовых параметров стратегии по каждому инструменту

public:
	MyFirstStrategy(const fo2base::UnixNanosecondsTime& delay, TradeSessionData& data, const std::string& pathToLog) 
		: TestStrategyBaseClass(delay, data, pathToLog) 
		, startMXIPrice(0)
		, startMIXPrice(0)
		, lastAbsorbTradeTime(0)
		, lastDealID(0)
	{}

	void InitStrategy(const fo2base::UnixNanosecondsTime localTime);

	void PreEventLogic(const FORTS_ORDLOG_REPL::orders_log& info);
    void PostEventActions(const FORTS_ORDLOG_REPL::orders_log& info);
    void PostEventActions(const FORTS_DEALS_REPL::deal& info);
	void EndSessionAction(const std::string& date);
	void EndSessionRangeAction();
    std::string GetStrategyResult() const;

private:
    void CheckFillingForSubmittedOrdersAfterNewAction(const fo2base::UnixNanosecondsTime eventTime);
    void PostORDLOGLogic(const FORTS_ORDLOG_REPL::orders_log& info);
    void PostDEALSLogic(const InstrumentID isin_id, StrategyParams& params, const t_id deal_id);

};  // MyFirstStrategy

class BackTester
{
public:
	BackTester() {}
	~BackTester() {}

    void CreateStrategies(const fo2base::UnixNanosecondsTime& timeDelay, TradeSessionData& data, const std::string& pathToLog);
	void InitStrategies(const fo2base::UnixNanosecondsTime localTime);
    void PreEventActions(const FORTS_ORDLOG_REPL::orders_log& info); 
    void PostEventActions(const FORTS_ORDLOG_REPL::orders_log& info);
    void PostEventActions(const FORTS_DEALS_REPL::deal& info);
	void EndSessionAction(const std::string& date);
	void EndSessionRangeAction();
    std::string GetStrategyResult() const;
	const TestStrategyBaseClass* GetFirstStrategy() { return strategyList.front().get(); }

	// функции для отладки одной единственной стратегии
    const Position* GetIntermediatePositionResult(const InstrumentID isin_id, const fo2base::UnixNanosecondsTime time) const;
    const SOrderEx* GetTrade(const InstrumentID isin_id, const fo2base::UnixNanosecondsTime time) const;
	const double* GetIndicator1(const InstrumentID isin_id, const fo2base::UnixNanosecondsTime time) const;
	const double* GetIndicator2(const InstrumentID isin_id, const fo2base::UnixNanosecondsTime time) const;

private:
    std::vector<std::shared_ptr<TestStrategyBaseClass>> strategyList;      // набор тестируемых стратегий

};  // BackTester

