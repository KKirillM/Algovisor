#pragma once
#pragma warning(disable: 4251)

#include "Plaza2_defines.h"
#include "Plaza2_Tables.h"
#include "IPlaza2_Connector.h"

enum P2Side
{
	UNKNOWN, BUY, SELL
};

enum P2Dir
{
	BID=1, ASK
};

enum P2Action
{
	REMOVE,     // заявка удалена
    ADD,        // заявка добавлена
    TRADE       // заявка сведена в сделку
};

enum /*class*/ P2Status /*: __int64*/
{
    QUOTE               = 0x01,         // Котировочная (заявка, которая ставится в аукцион)
    OPPOSITE            = 0x02,         // Встречная (заявка, которая сводится сколько может, остаток удаляется)
    NON_SYSTEMIC        = 0x04,         // Внесистемная
    MONEY_DONT_CHECK    = 0x10,         // При выставлении заявки не проверялись деньги по клиенту
    LAST_IN_RANSACTION  = 0x1000,       // Запись является последней в транзакции
    ORDER_MOVE          = 0x100000,     // Запись является результатом операции перемещения заявки
    ORDER_DELETE        = 0x200000,     // Запись является результатом операции удаления заявки
    GROUP_DELETE        = 0x400000,     // Запись является результатом группового удаления
    CROSS_TRADE_DELETE  = 0x20000000,   // Признак удаления остатка заявки по причине кросс-сделки
    FILL_OR_KILL        = 0x00080000    // Заявка Fill-or-kill
    //CANCEL_ON_DISCONNECT = 0x100000000  // Запись является результатом операции удаления заявки сервисом Cancel On Disconnect
};

#include "P2SysBCDII.h" // for P2BCDII2Double
struct P2Price
{
	double price_double;		// цена double
	int64_t price_int;			// все значащие цифры цены в виде целого
    int8_t price_scale;			// положение десятичной точки относительно конца числа price_int, т.е. насколько влево нужно сдвинуть точку в числе price_int

	P2Price() : price_double(0), price_int(0), price_scale(0) {}
	P2Price(const double _price_double, const int64_t _price_int = 0, const int8_t _price_scale = 0) : price_double(_price_double), price_int(_price_int), price_scale(_price_scale) {}
	P2Price(const P2Price& _price) { price_double = _price.price_double; price_int = _price.price_int; price_scale = _price.price_scale; }
    P2Price(const char* bcd) : price_double(0), price_int(0), price_scale(0)
    {
		P2BCDII2Double(&price_double, bcd);
		//CG_RESULT result = P2BCDIICheck(bcd);
		//P2BCDIIType bcd_type = P2BCDIIGetType(bcd);
		//result = P2BCDII2Int64(&price_int, bcd, price_scale);
  //      //result = cg_bcd_get(bcd, &price_int, &price_scale);	//эта функция работает только когда подключение к плазе2 активно
  //      if (result != CG_ERR_OK)
  //      {
  //          throw std::exception("Failed to convert decimal: 0x%X\n", result);
  //      }
  //      price_double = (double)price_int / pow(10., price_scale);
    }    

	bool operator<(const P2Price& _price) const 
	{
		return price_double < _price.price_double;
	}

	bool operator!=(const P2Price& _price) const
	{
		return price_double != _price.price_double;
	}
};

struct P2Order
{
    P2Price deal_price;         // цена сведенного ордера
    P2Price price;				// цена данного ордера
    P2Dir direction;			// сторона на которую выставляется ордер
    P2Action action;            // действие с ордером
    long long id_ord;           // айди ордера
    int quantity;               // исходный сайз ордера
    int quantity_rest;          // сколько осталось у ордера после сведения
    int status;                 // статус ордера
    __int64 timestamp;          // время ордера
    int precision;              // кол-во знаков после запятой в цене
    
    P2Order() : id_ord(0), quantity(0), quantity_rest(0), status(0), timestamp(0), precision(0) {}
    P2Order(const FORTS_ORDLOG_REPL::orders_log& info, const int _precision, const P2Price& _price, const P2Price& _deal_price) : price(_price), deal_price(_deal_price)
    {
        Init(info, _precision);
    }
    P2Order(const FORTS_ORDLOG_REPL::orders_log& info, const int _precision) : price(info.price)
    {
        Init(info, _precision);
        if (action == TRADE)
        {
            deal_price = P2Price(info.deal_price);
        }
    }

    void Init(const FORTS_ORDLOG_REPL::orders_log& info, const int _precision)
    {
        id_ord = info.id_ord;
        direction = (P2Dir)info.dir;
        action = (P2Action)info.action;
        quantity = action == REMOVE ? -info.amount : info.amount;
        quantity_rest = info.amount_rest;
        status = info.status;
        timestamp = info.moment_ns;
        precision = _precision;
    }

    bool operator!=(const P2Order& order) const
    {
        return id_ord != order.id_ord;
    }
};

struct P2Trade
{
	P2Price tradePrice;
	int tradeSize;
	P2Side agressorSide;
	long long id;

	P2Trade() : tradeSize(0), agressorSide(UNKNOWN), id(0) {}
	
	void SetId(const long long id_deal)		{ id = id_deal; }
	bool isNewId(const long long id_deal)	{ return id_deal != id ? true : false; }
};

struct Order
{
	int tradeSize;				//размер куска трейда сведённого с данным лимитом
	P2Price price;				//цена данного лимитного уровня
	int32_t volume;				//кол-во лимитов на данном ценовом уровне
	int32_t ordersCount;		//кол-во ордеров на данном ценовом уровне
	P2Dir	direction;			//сторона на которую выставляется ордер
	bool changeStatus;			//статус изменения данного уровня, true - изменение было именно на этом уровне, false - изменение было на каком-то другом уровне
	long long id_ord;


	Order(const P2Price& _price, const int32_t _volume, const P2Dir _dir, const long long _id_ord, const bool _changeStatus = false) : 
		price(_price),
		volume(_volume),
		ordersCount(0),
		direction(_dir),
		id_ord(_id_ord),
		changeStatus(_changeStatus),
		tradeSize(0)/*,
		tradeInitSize(0)*/ {}

	Order() : 
		volume(0),
		ordersCount(0),
		changeStatus(false),
		tradeSize(0),
		id_ord(0) {}

	// вспомогательная функция для сравнения цены покупки. лучшая цена - самая большая
	struct OrderComparatorBuy
	{
		bool operator() (const Order* o1, const Order* o2) const
		{
			return o1->price.price_double > o2->price.price_double;
		}
	};

	//вспомогательная функция для сравнения цены продажи. лучшая цена - самая маленькая
	struct OrderComparatorSell
	{
		bool operator() (const Order* o1, const Order* o2) const
		{
			return o1->price.price_double < o2->price.price_double;
		}
	};
};

//вспомогательная функция для поиска заявок с указанной ценой
struct price_equal : public std::binary_function<Order*, P2Price, bool>
{
	bool operator()(const Order* _order, const P2Price& _price) const
	{
		return _order->price.price_double == _price.price_double;
	}
};

template <class TComparator> 
class Orders
{
	typedef typename std::set<Order*, TComparator> TOrdersByPrice;
	typedef typename TOrdersByPrice::iterator iterator;
	
	//содержит агрегированные заявки (суммарный объем заявок с одинаковой ценой)
	//отсортирован по цене
	TOrdersByPrice _orders_aggregate;

	iterator findOrderByPrice(const P2Price& _price)
	{
		return std::find_if(_orders_aggregate.begin(), 
			_orders_aggregate.end(), 
			std::bind2nd(price_equal(), _price));
	}

public:
	typedef typename TOrdersByPrice::size_type size_type;
	typedef typename TOrdersByPrice::difference_type difference_type;

	Orders(){}
	~Orders() 
	{
		for (auto it = _orders_aggregate.begin(); it != _orders_aggregate.end();)
		{
			Order* order = *it;
			delete order;
			_orders_aggregate.erase(it++);
		}
	}

	void Add(const Order* order);
	void SetPrice(const Order* order);
	void Trade(const P2Trade& trade);
	void Remove(Order* const value);
	void RemovePrice(Order* const value);
	Order* const Get(size_t index) const;
	size_t Size() const;
	void ClearInfo();
	void Clear();
	Order* GetOrderByPrice(const P2Price& price);
};

class P2OrderBook
{
public:
	typedef Orders<Order::OrderComparatorBuy>::size_type size_type;
	typedef Orders<Order::OrderComparatorBuy>::difference_type difference_type;

	P2OrderBook();
	~P2OrderBook();

	void AddToPrice(Order* const order);		//добавление к уже существующей цене
	void SetPriceLevel(Order* const order);		//общее кол-во лимитных ордеров на текущей цене
	void RemoveFromPrice(Order* const order);	//снятие какого-то кол-ва ордеров с текущей цены
	void RemovePriceLevel(Order* const order);	//полное снятие всех ордеров с ценового уровня
	void SetTrade(const P2Trade& trade);
	void SetTime(const cg_time_t& t);
	void ClearPreviousInfo();
	void Clear();

	bool GetBestBidOrder(Order& order);
	bool GetBestAskOrder(Order& order);

	bool GetBestBidOrderWithVolume(Order& order);
	bool GetBestAskOrderWithVolume(Order& order);

	Order* GetNextBuyOrder(Orders<Order::OrderComparatorBuy>::size_type index) { return _orders_buy.Get(index); }
	Order* GetNextSellOrder(Orders<Order::OrderComparatorSell>::size_type index) { return _orders_sell.Get(index); }

	size_t BuyBookSize() { return _orders_buy.Size(); }
	size_t SellBookSize() { return _orders_sell.Size(); }

	cg_time_t& GetTime() { return time; }

	int32_t GetCurrOrderSize() { return currOrderSize; }
	
	void PrintToFile(std::string& fileName);	

	Order* GetOrderByPrice(const P2Price& price, P2Dir direction);

private:
	Orders<Order::OrderComparatorBuy> _orders_buy;
	Orders<Order::OrderComparatorSell> _orders_sell;
	int32_t currOrderSize;										//размер добавленного или убранного ордера на данной итерации
	cg_time_t time;
};






template <class TComparator>
void Orders<TComparator>::Add(const Order* order)
{
	//ищем пришедшую цену в нашем стакане
	typename TOrdersByPrice::iterator it = findOrderByPrice(order->price);

	if (it != _orders_aggregate.end())
	{
		(*it)->volume += order->volume;
		(*it)->changeStatus = true;
		(*it)->ordersCount += 1;
	}
	else
	{
		Order* new_order = new Order(order->price, order->volume, order->direction, order->changeStatus);
		new_order->ordersCount = 1;
		_orders_aggregate.insert(new_order);
	}
}

template <class TComparator>
void Orders<TComparator>::SetPrice(const Order* order)
{
	//ищем пришедшую цену в нашем стакане
	typename TOrdersByPrice::iterator it = findOrderByPrice(order->price);

	if (it == _orders_aggregate.end())	//цены не оказалось
	{
		//Order* new_order = new Order(order->price, order->volume, order->changeStatus, order->status);
		//_orders_aggregate.insert(new_order);	
		assert("Price is not available in Book");
	}
	else								//такая цена уже есть
	{
		(*it)->volume -= order->volume;
		(*it)->changeStatus = true;
		if (((*it)->volume && (*it)->ordersCount > 1) ||
			(!(*it)->volume && (*it)->ordersCount == 1))
			(*it)->ordersCount -= 1;
	}
}

template <class TComparator>
void Orders<TComparator>::Trade(const P2Trade& trade)
{
	//ищем пришедшую цену в нашем стакане
	typename TOrdersByPrice::iterator it = findOrderByPrice(trade.tradePrice);

	if(it != _orders_aggregate.end())	//цена есть
	{
		(*it)->tradeSize = trade.tradeSize;
	}
}

template <class TComparator>
Order* const Orders<TComparator>::Get(size_t index) const
{
	if (_orders_aggregate.size() > index)
	{
		typename TOrdersByPrice::const_iterator it = _orders_aggregate.begin();
		advance(it, index);
		return *it;
	}
	
	return NULL;
}

template <class TComparator>
void Orders<TComparator>::Remove(Order* const value)
{
	typename TOrdersByPrice::iterator it = _orders_aggregate.find(value);
	
	if (it != _orders_aggregate.end())
	{
		(*it)->volume = max(0, (*it)->volume - value->volume);
		(*it)->changeStatus = true;
		if (((*it)->volume && (*it)->ordersCount > 1) || 
			(!(*it)->volume && (*it)->ordersCount == 1))
			(*it)->ordersCount -= 1;
		
		if ((*it)->volume == 0)
			RemovePrice(value);
	}
}

template <class TComparator>
void Orders<TComparator>::RemovePrice(Order* const value)
{
	typename TOrdersByPrice::iterator it = _orders_aggregate.find(value);
	
	if (it != _orders_aggregate.end())
	{
		std::auto_ptr<Order> ptr(*it);		//здесь выполняется скрытый delete value
		_orders_aggregate.erase(it);
	}
}

template <class TComparator>
size_t Orders<TComparator>::Size() const
{
	return _orders_aggregate.size();
}

template <class TComparator>
void Orders<TComparator>::ClearInfo()
{
	//удаляем цены с нулевыми сайзами
	for (typename TOrdersByPrice::iterator it = _orders_aggregate.begin(); it != _orders_aggregate.end(); )
	{
		if (!(*it)->volume)
			RemovePrice(*it++);
		else
			++it;
	}

	for_each(_orders_aggregate.begin(), _orders_aggregate.end(), [](Order* it) {
		(*it).tradeSize = 0;
		/*(*it).tradeInitSize = 0;*/
		/*(*it).tradeInitPrice.price_double = 0;*/
		(*it).changeStatus = false;
	});
}

template <class TComparator>
void Orders<TComparator>::Clear()
{
	for(typename TOrdersByPrice::iterator it = _orders_aggregate.begin(); it != _orders_aggregate.end(); ++it)
	{
		delete *it;
	}
	_orders_aggregate.clear();	
}

template <class TComparator>
Order* Orders<TComparator>::GetOrderByPrice(const P2Price& price)
{
	//ищем пришедшую цену в нашем стакане
	typename TOrdersByPrice::iterator it = findOrderByPrice(price);

	if (it != _orders_aggregate.end())
		return *it;

	return NULL;
}