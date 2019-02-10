/*
		Copyright (c) 2014, MOEX. All rights reserved.
		
		Plaza-2 Client Gate API Usage Sample.
		Replication DataStream Client.
		
		All the software and documentation included in this and any
		other MOEX CGate Releasese is copyrighted by MOEX.
		
		Redistribution and use in source and binary forms, with or without
		modification, are permitted only by the terms of a valid
		software license agreement with MOEX.
		
		THIS SOFTWARE IS PROVIDED "AS IS" AND MICEX-RTS DISCLAIMS ALL WARRANTIES
		EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY IMPLIED WARRANTIES OF
		NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR
		PURPOSE.  MICEX-RTS DOES NOT WARRANT THAT USE OF THE SOFTWARE WILL BE
		UNINTERRUPTED OR ERROR-FREE.  MICEX-RTS SHALL NOT, UNDER ANY CIRCUMSTANCES, BE
		LIABLE TO LICENSEE FOR LOST PROFITS, CONSEQUENTIAL, INCIDENTAL, SPECIAL OR
		INDIRECT DAMAGES ARISING OUT OF OR RELATED TO THIS AGREEMENT OR THE
		TRANSACTIONS CONTEMPLATED HEREUNDER, EVEN IF MICEX-RTS HAS BEEN APPRISED OF
		THE LIKELIHOOD OF SUCH DAMAGES.
*/

/*
	Настоящий пример показывает сборку биржевого стакана для определенного инструмента,
	чей isin_id указан в аргументах командной строки.
	Также указываются максимальное количество выводимых заявок (глубина стакана, 
	0 - для вывода без ограничений), название выходного файла и признак реверсивной сортировки (r).

	This sample represents working with order book for specific instrument 
	whose isin_id is set in command line arguments.
	The other arguments are: order book depth (max quantity of elements in output, 0 for unlimited output),
	output file name and reverse sort flag (r).
*/

#ifndef _WIN32
#include "conio.h"
#else
#include <conio.h>
#include <memory.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <functional>
#include "compilers.h"
#include <memory>
#include <iostream>
#ifdef CGUTILS_HAS_CXX_UNIQUE_PTR
	using std::unique_ptr;
	#define smart_ptr unique_ptr
#else
	using std::auto_ptr;
	#define smart_ptr auto_ptr
#endif

#ifdef _WIN32
#include <windows.h>
#undef max
#else//_WIN32
#include <memory>
#endif//_WIN32

#define CG_SHORT_NAMES
#include ".\..\..\..\include\cgate.h"

// вспомогательный макрос для вызова функций CGate
// и выхода в случае ошибки
//
// helper macro to call CGate function
// and exit if there was error
#define CHECK_FAIL(f) \
	{ \
		uint32_t res = f; \
		if (res != CG_ERR_OK) \
		{ \
			log_info("Client gate error: %x", res); \
			exit(1); \
		} \
	} 

#define EXIT_WITH_ERROR(msg) \
	{ \
		log_info("Client gate error: %s", msg); \
		exit(1); \
	} \

// вспомогательный макрос для вызова функций CGate
// и вывода предупреждения в случае ошибки
//
// helper macro to call CGate function
// and print warning if there was error
#define WARN_FAIL(f) \
	{ \
		uint32_t res = f; \
		if (res != CG_ERR_OK) \
			log_info("Error: %x", res); \
	} 

// флаг выхода из программы
//
// helper variable used as a notification flag to exit
static volatile int bExit = 0;

#ifdef _WIN32
BOOL WINAPI InterruptHandler(DWORD reason)
{
	log_info("----BREAK----");
	bExit = 1;
	return 1;
}
#endif//_WIN32

enum Dir
{
	DirBuy,
	DirSell
};

typedef cg_data_cleardeleted_t RevisionInfo;

// отступы полей таблиц stream_data
// заполняются в обработчике CG_MSG_OPEN
//
// stream_data tables fields offsets
// initialized in CG_MSG_OPEN handler
struct StreamOffsetInfo
{
	size_t msg_index;
	size_t offset_replact;
	size_t offset_action;
	size_t offset_id;
	size_t offset_isin_id;
	size_t offset_price;
	size_t offset_amount;
	size_t offset_status;
	size_t offset_session_id;
	size_t offset_dir;
	size_t offset_repl_revision;
	size_t msg_data_size;
};

// описание используемых таблиц и полей из серверной схемы
//
// description of used tables and fields from server scheme
struct OrdLogSchemeInfo
{
	// индексы таблиц в схеме
	// 
	// indices of tables in scheme
	static struct TableIndices
	{
		long orders;
		long orders_log;
		long sys_events;

		TableIndices()
		{
			orders = orders_log = sys_events = -1L;
		}
	} table_indices;

	// названия таблиц в схеме
	//
	// names of tables in scheme
	static struct TableNames
	{
		const char* orders;
		const char* orders_log;
		const char* sys_events;
		
		TableNames()
			: orders("orders")
			, orders_log("orders_log")
			, sys_events("sys_events")
		{}
	} table_names;

	// названия полей в таблицах
	//
	// names of fields in tables
	static struct FieldNames
	{
		const char* repl_act;
		const char* action;
		const char* order_id;
		const char* isin_id;
		const char* price;
		const char* amount;
		const char* status;
		const char* direction;
		const char* session_id;
		const char* repl_revision;

		FieldNames()
			: repl_act("replAct")
			, action("action")
			, order_id("id_ord")
			, isin_id("isin_id")
			, price("price")
			, amount("amount_rest")
			, status("status")
			, direction("dir")
			, session_id("sess_id")
			, repl_revision("replRev")
		{}
	} field_names;

	static StreamOffsetInfo data_orders;
	static StreamOffsetInfo data_orders_log;
	static StreamOffsetInfo data_sys_events;

	static bool ready;
};

OrdLogSchemeInfo::TableIndices OrdLogSchemeInfo::table_indices;
OrdLogSchemeInfo::TableNames OrdLogSchemeInfo::table_names;
OrdLogSchemeInfo::FieldNames OrdLogSchemeInfo::field_names;
StreamOffsetInfo OrdLogSchemeInfo::data_orders;
StreamOffsetInfo OrdLogSchemeInfo::data_orders_log;
StreamOffsetInfo OrdLogSchemeInfo::data_sys_events;
bool OrdLogSchemeInfo::ready = false;

// вспомогательная функция для удаления динамически выделенных объектов при помощи алгоритма std::for_each
//
// helper function object for deleting dynamic allocated elements in containers using std::for_each algorithm
struct DeleteObjectByPtr
{
	template <typename T> void operator()(const T* ptr) const
	{
		delete ptr;
	}
};

class DisconnectException: public std::exception{};

// вспомогательный класс для создания логов
//
// helper class used for log messages creation
class MessageLogHelper
{
	static std::map<int, std::string> _msg_names;

public:
	static const unsigned int MaxMessageSize = 1024;

	static bool isInitialized()
	{
		return (!_msg_names.empty());
	}

	static void initialize()
	{
		if (!isInitialized())
		{
			_msg_names[CG_MSG_OPEN] = std::string("MsgOpen");
			_msg_names[CG_MSG_CLOSE] = std::string("MsgClose");
			_msg_names[CG_MSG_TIME] = std::string("MsgTime");
			_msg_names[CG_MSG_DATA] = std::string("MsgData");
			_msg_names[CG_MSG_STREAM_DATA] = std::string("MsgStreamData");
			_msg_names[CG_MSG_TN_BEGIN] = std::string("MsgTnBegin");
			_msg_names[CG_MSG_TN_COMMIT] = std::string("MsgTnCommit");
			_msg_names[CG_MSG_P2MQ_TIMEOUT] = std::string("MsgP2MQTimeout");
			_msg_names[CG_MSG_P2REPL_LIFENUM] = std::string("MsgP2ReplLifeNum");
			_msg_names[CG_MSG_P2REPL_CLEARDELETED] = std::string("MsgP2ReplClearDeleted");
			_msg_names[CG_MSG_P2REPL_ONLINE] = std::string("MsgP2ReplOnline");
			_msg_names[CG_MSG_P2REPL_REPLSTATE] = std::string("MsgP2ReplReplState");
		}
	}

	static int messagePrefix(int msg_type, char* prefix)
	{
		if(!isInitialized())
		{
			initialize();
		}
		return sprintf(prefix, "%s. ", _msg_names[msg_type].c_str());
	}
};
std::map<int, std::string> MessageLogHelper::_msg_names;

// данные заявки (активной или агрегированной), 
// сохраненные в стакане
//
// active or aggregate order data
// stored in order books
struct Order
{
	uint64_t id;
	double price;
	int32_t amount;
	RevisionInfo revision_info; // ignored for aggregate orders

	Order(uint64_t id_, double price_, int32_t vol_, const RevisionInfo& revInfo_)	:
		id(id_), price(price_), amount(vol_), revision_info(revInfo_){}

	// чтение данных заявки из stream_data message
	//
	// reading order data from stream_data message
	Order(const cg_msg_streamdata_t* stream_msg)
	{
		const char* msg_data = (const char*) stream_msg->data;
		StreamOffsetInfo* table_info = NULL;
		if(stream_msg->msg_index == OrdLogSchemeInfo::data_orders.msg_index)
		{
			table_info = &OrdLogSchemeInfo::data_orders;
		}
		else if(stream_msg->msg_index == OrdLogSchemeInfo::data_orders_log.msg_index)
		{
			table_info = &OrdLogSchemeInfo::data_orders_log;
		}
		else
		{
			EXIT_WITH_ERROR("Incorrect table index");
		}

		id = *(int64_t*)(msg_data + table_info->offset_id);
		const char* price_ptr = msg_data + table_info->offset_price;
		int64_t price_int = 0;
		int8_t price_scale = 0;
		WARN_FAIL(cg_bcd_get(price_ptr, &price_int, &price_scale));
		price = ((double)price_int)/(pow(10.0, price_scale));
		amount = *(int32_t*)(msg_data + table_info->offset_amount);
		revision_info.table_idx = (int32_t)stream_msg->msg_index;
		revision_info.table_rev = *(int64_t*)(msg_data + table_info->offset_repl_revision);
	}
	
	virtual int toString(char* str) const
	{
		return sprintf(str, "Id = %lld, price = %.2lf, amount = %d, table = %d, rev = %lld,",
			(long long int)id, price, amount, revision_info.table_idx, (long long int)revision_info.table_rev);
	}

	virtual ~Order() {}
};

enum ACTION
{
	ACTION_CANCEL,
	ACTION_ADD,
	ACTION_EXEC
};

// расширенные данные заявки
// используются для хранения частей составной бизнес-транзакции
//
// extended order data
// used for storing parts of composite business transaction
struct OrderDataEx : public Order
{
	Dir direction;
	uint32_t isin_id;
	int64_t repl_act;
	int8_t action;

	// чтение данных заявки из stream_data message
	//
	// reading order data from stream_data message
	OrderDataEx(const cg_msg_streamdata_t* stream_msg)
		: Order(stream_msg)
	{
		const char* msg_data = (const char*) stream_msg->data;

		StreamOffsetInfo* table_info = NULL;
		if(stream_msg->msg_index == OrdLogSchemeInfo::data_orders.msg_index)
		{
			table_info = &OrdLogSchemeInfo::data_orders;
		}
		else if(stream_msg->msg_index == OrdLogSchemeInfo::data_orders_log.msg_index)
		{
			table_info = &OrdLogSchemeInfo::data_orders_log;
		}
		else
		{
			EXIT_WITH_ERROR("Incorrect table index");
		}

		int8_t dir_int = *(msg_data + table_info->offset_dir);
		direction = (dir_int == 1) ? DirBuy : DirSell;
		isin_id = *(int32_t*)(msg_data + table_info->offset_isin_id);
		repl_act = *(int64_t*)(msg_data + table_info->offset_replact);
		action = (stream_msg->msg_index == OrdLogSchemeInfo::data_orders.msg_index) ? 
			ACTION_ADD : *(msg_data + table_info->offset_action);
	}

	virtual int toString(char* str) const
	{
		int orders_str_end = Order::toString(str);
		return orders_str_end + sprintf(
			str + orders_str_end,
			" dir = %s, isinId  = %d, replAct = %lld, action = %d"
			, (direction == DirBuy) ? "Buy" : "Sell"
			, isin_id
			, (long long int)repl_act
			, action);
	}
};

// вспомогательная функция для сравнения цены покупки. лучшая цена - самая большая
//
// helper function object for buying price comparison. bigger price is better
struct OrderComparatorBuy
{
	bool operator() (const Order* o1, const Order* o2) const
	{
		return (o1->price == o2->price) ? o1->id < o2->id : o1->price > o2->price;
	}
};

// вспомогательная функция для сравнения цены продажи. лучшая цена - самая маленькая
//
// helper function object for selling price comparison. less price is better
struct OrderComparatorSell
{
	bool operator() (const Order* o1, const Order* o2) const
	{
		return (o1->price == o2->price) ? o1->id < o2->id : o1->price < o2->price;
	}
};

// вспомогательная функция для поиска заявок с указанной ценой
//
// helper function object for searching order with specified price
struct price_equal	: public std::binary_function<Order*, double, bool>
{
	bool operator()(const Order* _Order, const double& price) const
	{
		return _Order->price == price;
	}
};

// сортированный в указанном направлении контейнер для активных и агрегированных заявок
//
// sorted in specified direction container for active and aggregate orders 
template <typename TComparator> class Orders
{
	typedef std::map<int64_t, Order*> TOrdersById;
	typedef std::set<Order*, TComparator> TOrdersByPrice;
	// содержит агрегированные заявки (суммарный объем заявок с одинаковой ценой)
	// отсортирован по цене
	//
	// contains aggregate amount of all equally priced active orders
	// sorted by price
	TOrdersByPrice _orders_aggregate;
	// активные заявки, отсортированные по ord_id
	//
	// active orders sorted dy ord_id
	TOrdersById _orders_active;

	TComparator _compare;

	typename TOrdersByPrice::iterator findOrderByPrice(const double& price)
	{
		return std::find_if(
			_orders_aggregate.begin()
			, _orders_aggregate.end()
			, std::bind2nd(price_equal(), price));
	}

	// вычесть объем заявки из агрегированного объема по данной цене
	// удалить заявку, если агрегированный объем стал равен 0
	//
	// subtract order amount from the aggregate amount. 
	// delete record when modified aggregate amount = 0
	void modifyAggregateAmount(const double& price, int32_t amount)
	{
		log_trace("Modifying aggregate amount, price = %10.2lf", price);
		typename TOrdersByPrice::iterator itPriceOrder = findOrderByPrice(price);
		if (itPriceOrder == _orders_aggregate.end())
		{
			fprintf(stderr, "\nGot remove operator for non-existing order price=%.2lf\n", price);
			return;
		}

		if((*itPriceOrder)->amount == amount)
		{
			smart_ptr<Order> apOrder(*itPriceOrder); // delete helper 
			_orders_aggregate.erase(itPriceOrder);
			log_trace("Deleting aggregate record");
		}
		else
		{
			(*itPriceOrder)->amount -= amount;
			assert((*itPriceOrder)->amount > 0);
			log_trace("Aggregate amount %d -> %d", (*itPriceOrder)->amount + amount, (*itPriceOrder)->amount);
		}
	}

public:
	Orders() { }

	// добавление активной заявки.
	// создание новой агрегированной завки, если нет заявки с данной ценой
	//
	// place new active order. 
	// alter aggregate record corresponding to it (if exist) or create new
	void add(const OrderDataEx* order)
	{
		log_trace(
			"New order record. Table = %d, revision = %lld. ord_id = %lld, price = %10.2lf, amount = %d",
			order->revision_info.table_idx, 
			(long long int)order->revision_info.table_rev, 
			(long long int)order->id, 
			order->price, 
			order->amount);
		Order* active_order = new Order(
			order->id, order->price, order->amount, order->revision_info);
		
		TOrdersById::iterator it_active = _orders_active.find(order->id);
		if(it_active != _orders_active.end())
		{
			log_error("Duplicate order %d\n", order->id);
			delete order;
			return;
		}
		_orders_active[order->id] = active_order;

		typename TOrdersByPrice::iterator it = findOrderByPrice(order->price);

		if(it == _orders_aggregate.end())
		{
			Order* aggr_order = new Order(
				order->id, order->price, order->amount, order->revision_info);
			_orders_aggregate.insert(aggr_order);
		}
		else
		{
			log_trace("Found existing record. amount %d -> %d", 
				(*it)->amount, (*it)->amount + order->amount);
			(*it)->amount += order->amount;
		}
	}

	// удалить активную заявку. вычесть ее объем из агрегированной
	//
	// delete active record. subtract it from aggregate amount
	void remove(int64_t id)
	{
		log_trace("Deleting active order record with id = %lld", (long long int)id);
		typename TOrdersById::iterator it_id_order = _orders_active.find(id);
		if (it_id_order == _orders_active.end())
		{
			fprintf(stderr, "\nGot remove operator for non-existing order id=%lld\n", (long long int)id);
			return;
		}
		smart_ptr<Order> ap_order(it_id_order->second); // delete helper
		_orders_active.erase(it_id_order);

		modifyAggregateAmount(ap_order->price, ap_order->amount);
	}

	// изменить объем активной заявки. поправить агрегированную запись
	//
	// change amount of active record. alter aggregate amount
	void modify(int64_t id, int32_t amount)
	{
		log_trace("Modifying order with ord_id = %lld, new amount = %d", (long long int)id, amount);
		if (amount == 0)
		{
			remove(id);
		}
		else
		{
			typename TOrdersById::iterator it_order = _orders_active.find(id);
			if (it_order == _orders_active.end())
			{
				fprintf(stderr, "\nGot modify operator for non-existing order id=%lld, amount=%d\n", 
					(long long int)id, amount);
				return;
			}
			
			Order* order = it_order->second;
			int32_t delta_amount = order->amount - amount;
			order->amount = amount;
			modifyAggregateAmount(order->price, delta_amount);
		}
	}

	// удалить все заявки (активные и агрегированные)
	//
	// delete all orders (active and aggregate)
	void clear()
	{
		log_trace("Deleting %d aggregate orders", _orders_aggregate.size());
		for(typename TOrdersByPrice::iterator it = _orders_aggregate.begin();
			it != _orders_aggregate.end(); ++it)
		{
			smart_ptr<Order> ap_order(*it);	// delete helper
			log_trace("Deleting order with price = %lf and aggregate amount = %d",
				ap_order->price, ap_order->amount);
		}
		_orders_aggregate.clear();

		log_trace("Deleting %d active orders", _orders_active.size());
		for(typename TOrdersById::iterator it = _orders_active.begin();
			it != _orders_active.end(); ++it)
		{
			smart_ptr<Order> ap_order(it->second);	// delete helper
			log_debug("Deleting active order with price = %lf and amount = %d",
				ap_order->price, ap_order->amount);
		}
		_orders_active.clear();
	}

	// обработчик CG_MSG_P2REPL_CLEARDELETED
	// удалить все заявки с ревизией, старшей, чем указанная
	//
	// CG_MSG_P2REPL_CLEARDELETED handler
	// delete all orders with revision older than specified
	void onClearDeleted(const RevisionInfo* rev_info)
	{
		for(typename TOrdersById::iterator it = _orders_active.begin();
			it != _orders_active.end(); )
		{
			Order* order = it->second;
			if ((order->revision_info.table_idx == rev_info->table_idx) &&
				(order->revision_info.table_rev < rev_info->table_rev))
			{
				smart_ptr<Order> ap_order(order);	// delete helper
				_orders_active.erase(it++);
				log_trace("Erasing deleted order with table_idx = %d and table_rev = %lld. Price = %lf, amount = %d",
					order->revision_info.table_idx, (long long int)order->revision_info.table_rev, ap_order->price, ap_order->amount);
				modifyAggregateAmount(order->price, order->amount);
			}
			else
			{
				++it;
			}
		}
	}

	// STL-алгоритм для вывода на печать элементов типа 'Order'
	//
	// STL-algorithm for printing 'Order' elements
	template <typename InputIterator> void print_elements(
		InputIterator first, InputIterator last, FILE* file, int max_count)
	{
		int cntr = 0;
		for(InputIterator it = first; (it != last) && (cntr < max_count); ++it, ++cntr)
		{
			if (((*it)->price > 0) && ((*it)->amount > 0))
			{
				fprintf(file, "\t\tprice = %.2lf\tamount = %d\n", (*it)->price, (*it)->amount);
			}
		}
	}

	// записать содержимое агрегированных заявок в текстовый файл
	//
	// write aggregate contents to text file
	void print(FILE* file, int depth, bool reverse)
	{
		if (!_orders_aggregate.size())
			return;
		assert(file != NULL);
		if (depth == 0)
		{
			depth = (int)_orders_aggregate.size();
		}

		if (!reverse)
		{
			print_elements(_orders_aggregate.begin(), _orders_aggregate.end(), file, depth);
		}
		else
		{
			print_elements(_orders_aggregate.rbegin(), _orders_aggregate.rend(), file, depth);
		}
	}
};

// стакан для определенного инструмента
// содержит по одному контейнеру 'Orders' для заявок на покупку и продажу
//
// order book for single instrument
// contains one 'Orders' container for buy orders and one for sell orders
class OrderBook
{
	int32_t _isin_id;
	Orders<OrderComparatorBuy> _orders_buy;
	Orders<OrderComparatorSell> _orders_sell;

public:
	OrderBook(int32_t isin_id) : _isin_id(isin_id) { }

	void add(const OrderDataEx* order)
	{
		if (order->direction == DirBuy)
			_orders_buy.add(order);
		else
			_orders_sell.add(order);
	}

	void modify(const OrderDataEx* order)
	{
		if (order->direction == DirBuy)
		{
			_orders_buy.modify(order->id, order->amount);
		}
		else
		{
			_orders_sell.modify(order->id, order->amount);
		}
	}

	void remove(const OrderDataEx* order)
	{
		if (order->direction == DirBuy)
		{
			_orders_buy.remove(order->id);
		}
		else
		{
			_orders_sell.remove(order->id);
		}
	}

	void clear()
	{
		_orders_buy.clear();
		_orders_sell.clear();
	}

	void onClearDeleted(const RevisionInfo* rev_info)
	{
		_orders_buy.onClearDeleted(rev_info);
		_orders_sell.onClearDeleted(rev_info);
	}

	void print(Dir dir, FILE* file, int depth, bool reverse)
	{
		if (dir == DirBuy)
		{
			_orders_buy.print(file, depth, reverse);
		}
		else
		{
			_orders_sell.print(file, depth, reverse);
		}
	}
};

// признаки системы Spectra
//
// Spectra message status 
enum MESSAGE_STATUS
{
	MESSAGE_STATUS_NONSYSTEM = 0x04,
	MESSAGE_STATUS_END_BUSINESS_TRANSACTION = 0x1000
};

// данные таблицы 'sys_events'
// на данный момент используется только поле 'sess_id'
//
// 'sys_events' table data
// now we use only 'sesion_id' field
struct SysEvents
{
	int32_t session_id;
};

// основной объект - приложение OrderBookApp
// реализует логику обработки сообщений
//
// main order book application
// implements message callback and other message handling functions
class OrderBookApp
{
	cg_conn_t* _connection;
	cg_listener_t *_listener;

	OrderBook _book;

	int32_t _session_id;
	uint32_t _target_isin_id;
	uint32_t _depth;
	std::string _outfile;
	bool _reverse;
	int64_t _last_revision;

	typedef std::vector<OrderDataEx*> TMsgDataContainer;

	// контейнер для заявок из составной бизнес-транзакции
	// содержит заявки по текущей транзакции
	// очищается по MESSAGE_STATUS_END_BUSINESS_TRANSACTION
	//
	// container for composite business transaction orders
	// collects orders for current transaction
	// cleared by MESSAGE_STATUS_END_BUSINESS_TRANSACTION
	TMsgDataContainer _business_transaction_orders;

	FILE* _msg_log_file;
	
	void logMessageToFile(const char* message)
	{
		if(_msg_log_file != NULL)
		{
			fprintf(_msg_log_file, "%s\n", message);
		}
	}

	void logMessage(cg_msg_t* message)
	{
		char message_str[MessageLogHelper::MaxMessageSize];
		int prefixSize = MessageLogHelper::messagePrefix(message->type, message_str);
		
		log_trace(message_str);
		logMessageToFile(message_str);
	}

	void logMessage(const OrderDataEx* order)
	{
		char message_str[MessageLogHelper::MaxMessageSize];
		int prefix_size = MessageLogHelper::messagePrefix(CG_MSG_STREAM_DATA, message_str);
		int message_size = prefix_size + order->toString(message_str + prefix_size);
		
		log_trace(message_str);
		logMessageToFile(message_str);
	}

	void logMessage(const RevisionInfo* rev_info)
	{
		char message_str[MessageLogHelper::MaxMessageSize];
		int prefix_size = MessageLogHelper::messagePrefix(CG_MSG_P2REPL_CLEARDELETED, message_str);
		int message_size = prefix_size + sprintf(
			message_str + prefix_size
			, "table = %d, rev = %lld"
			, rev_info->table_idx
			, (long long int)rev_info->table_rev);

		log_trace(message_str);
		logMessageToFile(message_str);
	}

	void logMessage(const SysEvents* sys_events)
	{
		char message_str[MessageLogHelper::MaxMessageSize];
		int prefix_size = MessageLogHelper::messagePrefix(CG_MSG_STREAM_DATA, message_str);
		int message_size = prefix_size + sprintf(message_str + prefix_size,
			"session id %d", sys_events->session_id);
		
		log_trace(message_str);
		logMessageToFile(message_str);
	}
	

	
	// save Order in vector and process all vector after end of business transaction 
	void addBusinessTransaction(OrderDataEx* order)
	{
		_business_transaction_orders.push_back(order);
	}

	// заполнение значений отступов полей в таблицах
	// вызывается из обработчика CG_MSG_OPEN
	// при изменении серверной схемы следует поменять значения имен полей в SchemeInfo
	//
	// filling table fields offset values
	// called from CG_MSG_OPEN handler
	// in case of server scheme change it is necessary to update field names values in SchemeInfo
	void initializeScheme(cg_listener_t* listener)
	{
		cg_scheme_desc_t* scheme_desc = 0;
		WARN_FAIL(cg_lsn_getscheme(listener, &scheme_desc));
		assert(scheme_desc);

		cg_message_desc_t* message_desc = scheme_desc->messages;

		for (size_t index = 0; message_desc; index ++, message_desc = message_desc->next)
		{
			cg_field_desc_t* field_desc = message_desc->fields;
			StreamOffsetInfo* table_info = 0;
			if (strcmp(message_desc->name, OrdLogSchemeInfo::table_names.orders) == 0)
			{
				table_info = &OrdLogSchemeInfo::data_orders;
			}
			else
			if (strcmp(message_desc->name, OrdLogSchemeInfo::table_names.orders_log) == 0)
			{
				table_info = &OrdLogSchemeInfo::data_orders_log;
			}
			else
			if (strcmp(message_desc->name, OrdLogSchemeInfo::table_names.sys_events) == 0)
			{
				table_info = &OrdLogSchemeInfo::data_sys_events;
			}
			else
			{
				continue;
			}

			table_info->msg_index = index;
			table_info->msg_data_size = message_desc->size;

			while (field_desc != NULL)
			{
				if (strcmp(field_desc->name, OrdLogSchemeInfo::field_names.repl_act) == 0)
				{
					table_info->offset_replact = field_desc->offset;
					assert (field_desc->size == sizeof(int64_t));
				}
				else if (strcmp(field_desc->name, OrdLogSchemeInfo::field_names.action) == 0)
				{
					table_info->offset_action = field_desc->offset;
					assert (field_desc->size == sizeof(int8_t));
				}
				else if (strcmp(field_desc->name, OrdLogSchemeInfo::field_names.order_id) == 0)
				{
					table_info->offset_id = field_desc->offset;
					assert (field_desc->size == sizeof(int64_t));
				}
				else if (strcmp(field_desc->name, OrdLogSchemeInfo::field_names.isin_id) == 0)
				{
					table_info->offset_isin_id = field_desc->offset;
					assert (field_desc->size == sizeof(int32_t));
				}
				else if (strcmp(field_desc->name, OrdLogSchemeInfo::field_names.price) == 0)
				{
					table_info->offset_price = field_desc->offset;
				}
				else if (strcmp(field_desc->name, OrdLogSchemeInfo::field_names.amount) == 0)
				{
					table_info->offset_amount = field_desc->offset;
					assert (field_desc->size == sizeof(int32_t));
				}
				else if (strcmp(field_desc->name, OrdLogSchemeInfo::field_names.status) == 0)
				{
					table_info->offset_status = field_desc->offset;
					assert (field_desc->size == sizeof(int32_t));
				}
				else if (strcmp(field_desc->name, OrdLogSchemeInfo::field_names.direction) == 0)
				{
					table_info->offset_dir = field_desc->offset;
					assert (field_desc->size == sizeof(int8_t));
				}
				else if (strcmp(field_desc->name, OrdLogSchemeInfo::field_names.session_id) == 0)
				{
					table_info->offset_session_id = field_desc->offset;
					assert (field_desc->size == sizeof(int32_t));
				}
				else if (strcmp(field_desc->name, OrdLogSchemeInfo::field_names.repl_revision) == 0)
				{
					table_info->offset_repl_revision = field_desc->offset;
					assert (field_desc->size == sizeof(int64_t));
				}

				field_desc = field_desc->next;
			}
		}

		OrdLogSchemeInfo::ready = true;
	}

	// основной обработчик сообщений
	//
	// main message callback function
	static CG_RESULT messageCallback(cg_conn_t* connection, cg_listener_t* listener, struct cg_msg_t* message, void* data)
	{
		OrderBookApp* _this = (OrderBookApp*)data;
		if (_this == NULL)
			return CG_ERR_INVALIDARGUMENT;

		switch (message->type)
		{
			// открытие нового соединения с сервером
			// инициализировать схему
			//
			// opening new server connection
			// initialize scheme description
		case CG_MSG_OPEN:
			{
			_this->logMessage(message);
			_this->cleanup(); // in case of reconnect
			_this->initializeScheme(listener);
				break;
			}

			// обрабатывать только таблицы 'orders', 'orders_log' и 'sys_events'
			// получать снимок базы данных по FORTS_FUTORDERBOOK_REPL.orders
			// и обновления по FORTS_ORDLOG_REPL.orders_log
			// использовать 'sys_events' для определения текущего sess_id
			// пропускать остальные таблицы
			// пропускать сообщения для инструментов, отличных от _target_isin_id
			//
			// process only 'orders', 'orders_log' and 'sys_events' tables
			// receive database snapshot from FORTS_FUTORDERBOOK_REPL.orders 
			// and updates from FORTS_ORDLOG_REPL.orders_log
			// use 'sys_events' for determining current sess_id
			// skip other tables
			// skip messages for instruments other than _target_isin_id
		case CG_MSG_STREAM_DATA:
			{
				if(!OrdLogSchemeInfo::ready)
				{
					break;
				}
				struct cg_msg_streamdata_t* stream_msg = (struct cg_msg_streamdata_t* )message;

				StreamOffsetInfo* table_info = NULL;
				// выбор описателя таблицы
				// choosing message description
				if (stream_msg->msg_index == OrdLogSchemeInfo::data_orders.msg_index)
				{ 
					table_info = &OrdLogSchemeInfo::data_orders;
				}
				else if (stream_msg->msg_index == OrdLogSchemeInfo::data_orders_log.msg_index)
				{
					table_info = &OrdLogSchemeInfo::data_orders_log;
				}
				// обработка 'sys_events'
				// 'sys_events' handling
				else if (stream_msg->msg_index == OrdLogSchemeInfo::data_sys_events.msg_index)
				{
					table_info = &OrdLogSchemeInfo::data_sys_events;
					const char *msg_data = (const char *)stream_msg->data;
					SysEvents sys_events;
					// определения номера сессии
					// determining session id
					sys_events.session_id = *(int32_t*)(msg_data + table_info->offset_session_id);
					_this->logMessage(&sys_events);
					_this->updateSessionId(sys_events.session_id);
					break;
				}
				else
				{
					break;
				}
				
				const char *msg_data = (const char *)stream_msg->data;
				
				int32_t isin_id = *(int32_t*)(msg_data + table_info->offset_isin_id);
				// пропуск сообщений для других инструментов
				// skip messages for other instruments
				if (isin_id != (int32_t)_this->_target_isin_id)
				{
					return CG_ERR_OK;
				}

				int32_t status = *(int32_t*)(msg_data + table_info->offset_status);
				// пропуск внесистемных заявок
				// ignore non-system orders
				if ((status & MESSAGE_STATUS_NONSYSTEM) != 0)
				{
					break;
				}

				int32_t session_id = *(int32_t*)(msg_data + table_info->offset_session_id);
				// пропуск сообщений с предыдущих сессий
				// ignore messages from older sessions
				if(session_id < _this->_session_id)
				{
					break;
				}

				// сравнение полученного sess_id с текущим
				// изменение текущего номера сессии при обновлении
				// compare session id with current
				// update if session_id from message data is newer than current
				_this->updateSessionId(session_id);

				// заполнение заявки из потокового сообщения
				// filling order data from stream message
				OrderDataEx* order = new OrderDataEx(stream_msg);
				_this->logMessage(order);

				// запоминание текущего номера ревизии (для 'orders_log')
				// remembering current revision (for 'orders_log')
				if (stream_msg->msg_index == OrdLogSchemeInfo::data_orders_log.msg_index)
				{
					if (order->revision_info.table_rev <= _this->_last_revision)
					{
						delete order;
						return CG_ERR_INCORRECTSTATE;
					}
					_this->_last_revision = order->revision_info.table_rev;
				}

				// данная заявка является не последней в составной бизнес-транзакции
				// сохраняем ее без обработки. 
				// current order is not last in composite business-transaction
				// save it without processing
				if((status & MESSAGE_STATUS_END_BUSINESS_TRANSACTION) == 0)
				{
					_this->addBusinessTransaction(order);
				}
				else
				// обработка полной бизнес-транзакции
				// processing complete business transaction
				{
					// если бизнес-транзакция состоит из одной заявки
					// in case of single-ordered business-transaction
					if (_this->_business_transaction_orders.empty())
					{
						_this->processOrder(order);
						smart_ptr<OrderDataEx> ap_order(order);
					}
					// обработка всех заявок транзакции по очереди
					// processing all transaction orders one after another
					else
					{
						_this->addBusinessTransaction(order);
						for (TMsgDataContainer::iterator it = _this->_business_transaction_orders.begin();
							it != _this->_business_transaction_orders.end(); 
							++it)
						{
							_this->processOrder(*it);
						}
						// очистка текущей очереди заявок
						// clear current order queue
						_this->clearBusinessTransaction();
					}
				}
				break;
			}

			// очистить все данные при смене номера жизни
			//
			// clear all data when life number is changed
		case CG_MSG_P2REPL_LIFENUM:
			{
				_this->logMessage(message);
				_this->cleanup();
				break;
			}

			// удалить все заявки с ревизией, старшей, чем указанная
			//
			// clear older orders
		case CG_MSG_P2REPL_CLEARDELETED:
			{
				const RevisionInfo* rev_info = (RevisionInfo*)message->data;
				_this->logMessage(rev_info);
				if ((rev_info->table_idx == OrdLogSchemeInfo::data_orders.msg_index) ||
					(rev_info->table_idx == OrdLogSchemeInfo::data_orders_log.msg_index))
				{
					_this->_book.onClearDeleted(rev_info);
					if (rev_info->table_rev == CG_MAX_REVISON)
					{
						_this->_last_revision = 0;
					}
				}
				break;
			}

			// закрытие соединения 
			// записать содержимое стакана в файл
			// произвести очистку всех данных
			//
			// closing current connection
			// write order book contents into file
			// perform complete cleanup
		case CG_MSG_CLOSE:
			{
				_this->logMessage(message);
				_this->print(_this->_outfile.c_str());
				_this->cleanup();
				_this->_last_revision = 0;
				break;
			}

			// данное сообщение говорит о получении полной реплики базы данных
			// с этого момента мы будем получать обновления
			//
			// this message tells that we have completed database replica
			// since this moment we shall receive updates only
		case CG_MSG_P2REPL_ONLINE:
			{
				_this->logMessage(message);
				break;
			}
		};

		return CG_ERR_OK;
	}

	// обработка активной заявки
	//
	// processing active order
	CG_RESULT processOrder(OrderDataEx* order)
	{
		assert(order != NULL);
		assert(order->isin_id == _target_isin_id);

		char log_str[MessageLogHelper::MaxMessageSize] = "Processing order: ";
		order->toString(log_str + strlen(log_str));
		log_trace(log_str);

		// если repl_act != 0, заявка удаляется
		// при этом в поле repl_act записан предыдущий номер ревизии заявки
		// in case of repl_act != 0 current order is deleted
		// at that repl_act contains previous revision number of this order
		if(order->repl_act != 0)
		{
			_book.remove(order);
			return CG_ERR_OK;
		}

		switch (order->action)
		{
		case ACTION_ADD:
			{
				_book.add(order);
				break;
			}

		case ACTION_EXEC:
			{
				_book.modify(order);
				break;
			}

		case ACTION_CANCEL:
			{
				_book.remove(order);
				break;
			}
		default:
			fprintf(stderr, "\nUnknown action %d for order %lld\n", order->action, (unsigned long long)order->id);
		}

		return CG_ERR_OK;
	}
	
public:
	OrderBookApp(cg_conn_t* connection, const char* settings, 
		uint32_t target_isin_id, uint32_t depth, const std::string& outfile, bool reverse)
		: _connection(connection)
		, _listener(NULL)
		, _book(target_isin_id)
		, _session_id(0)
		, _target_isin_id(target_isin_id)
		, _depth(depth)
		, _outfile(outfile)
		, _reverse(reverse)
		, _last_revision(0)
	{
		if (cg_lsn_new(_connection, settings, &messageCallback, this, &_listener) != CG_ERR_OK)
			throw std::exception();

		_msg_log_file = fopen("ordbook msg.log", "w");
		if(_msg_log_file == NULL)
		{
			EXIT_WITH_ERROR("Unable to create msglog file");
		}
		log_info("OrderBook application for isin_id = %d created, settings = %s", target_isin_id, settings);
	}

	~OrderBookApp()
	{
		log_info("Closing OrderBook application");
		if (_listener)
			cg_lsn_destroy(_listener);

		cleanup();

		fclose(_msg_log_file);
	}

	void clearBusinessTransaction()
	{
		std::for_each(
			_business_transaction_orders.begin(), 
			_business_transaction_orders.end(), 
			DeleteObjectByPtr());
		_business_transaction_orders.clear();
	}

	void cleanup()
	{
		_book.clear();
		
		clearBusinessTransaction();
	}

	bool updateSessionId(int32_t id)
	{
		// если номер сессии сменился на следующий
		// if sess_id is changed to next
		if(id > _session_id)
		{
			// исключаем случай, когда sess_id еще не был инициализирован
			// удаляем все данные с предыдущей сессии
			// avoiding an occurence when sess_id not being initialized yet
			// deleting all data from previous session
			if(_session_id > 0)
			{
				cleanup();
			}
			log_info("Session ID is changed %d -> %d", _session_id, id);
			_session_id = id;
			return true;
		}
		else
		{
			return false;
		}
	}

	// записать содержимое стакана в текстовый файл
	//
	// write current order books content to text file
	void print(const char* name)
	{
		FILE* file = fopen(name, "w");
		if(file == NULL)
		{
			EXIT_WITH_ERROR("Unable to create output file");
		}

		fprintf(file, "BUY:\n");
		_book.print(DirBuy, file, _depth, _reverse);
		fprintf(file, "SELL:\n");
		_book.print(DirSell, file, _depth, _reverse);
		fprintf(file, "\n");
		fclose(file);
	}

	// записать содержимое стакана в текстовый файл
	// добавить в название файла номер текущей ревизии
	//
	// write current order books content to text file
	// add current revision number to file name
	void printLastRevision()
	{
		char filename[256];
		sprintf(filename, "ordbook %d.%lld.log", _target_isin_id, (long long int)_last_revision);
		
		print(filename);
	}

	// убедиться в том, что listener находится в корректном состоянии
	//
	// ensure that listener is in correct state
	void checkListener(uint32_t &state)
	{
		WARN_FAIL(cg_lsn_getstate(_listener, &state));
	}

	// основной цикл приложения
	//
	// main application loop body
	void run(uint32_t &state)
	{
		WARN_FAIL(cg_conn_getstate(_connection, &state));

		if (state == CG_STATE_ERROR)
		{
			WARN_FAIL(cg_conn_close(_connection));
		}
		else if (state == CG_STATE_CLOSED)
		{
			WARN_FAIL(cg_conn_open(_connection, 0));
		}
		else if (state == CG_STATE_ACTIVE)
		{
			uint32_t result = cg_conn_process(_connection, 1, 0);
			if (result != CG_ERR_OK && result != CG_ERR_TIMEOUT) 
			{
				log_info("Warning: connection state request failed: %X", result);
			}

			WARN_FAIL(cg_lsn_getstate(_listener, &state));

			switch (state)
			{
			case CG_STATE_CLOSED:
					WARN_FAIL(cg_lsn_open(_listener, 0));
					break;
			case CG_STATE_ERROR:
					WARN_FAIL(cg_lsn_close(_listener));
					throw DisconnectException();
					break;
			}
		}
	}
};

int main(int argc, char* argv[])
{
	// connection pointer
	conn_t* connection = 0;
	// listener pointer
	listener_t* listener = 0;

	// connection init string
	const char* conn_str = "p2tcp://127.0.0.1:4001;app_name=test_ordbook";

#ifdef _WIN32
	SetConsoleCtrlHandler(InterruptHandler, 1);
#endif//_WIN32

	if (argc < 4) 
	{
		printf("Usage:\n\tordbook ISIN_ID depth outfile r(optional)\n\n"
			"where\n\tISIN_ID is unique instrument ID (e.g. 167566)\n"
			"\tdepth is max orderbook size (0 for unlimited size)\n"
			"\toutfile is output file path\n"
			"\tr for reverse sorting");
		return 2;
	}

	uint32_t isin_id = atoi(argv[1]);
	uint32_t depth = atoi(argv[2]);
	std::string outfile(argv[3]);
	bool reverse = false;
	if((argc == 5) && (strcmp(argv[4], "r") == 0))
	{
		reverse = true;
	}

	// Environment is initialized using provided INI-file
	CHECK_FAIL(env_open("ini=ordbook.ini;key=11111111;minloglevel=trace"));
	// Connection is initialized using connection init string
	CHECK_FAIL(conn_new(conn_str, &connection));

	try
	{
		const char* ordbook_str = "p2ordbook://FORTS_ORDLOG_REPL;snapshot=FORTS_ORDBOOK_REPL";
		OrderBookApp app(connection, ordbook_str, isin_id, depth, outfile, reverse);
		uint32_t state;
		app.checkListener(state);
		while (!bExit) // check global interrupt flag
		{
			try
			{
				app.run(state);
			}
			catch(DisconnectException&)
			{
				log_info("Disconnection");
			}

			if(_kbhit())
			{
				if(getchar() == 0x0a)
				{
					app.printLastRevision();
				}
			}
		}
	}
	catch (std::exception& e)
	{
		fprintf(stderr, "Exception: %s", e.what());
	}

	// destroy connection
	if (connection != NULL)
	{
		conn_destroy(connection);
	}

	// close environment
	CHECK_FAIL(env_close());
	return 0;
}
