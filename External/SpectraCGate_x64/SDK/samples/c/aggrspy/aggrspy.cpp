/*
		Copyright (c) 2014, MOEX. All rights reserved.
		
		Plaza-2 Client Gate API Usage Sample.
		Process orders log steram.
		
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
	Настоящий пример показывает получение агрегированного стакана для определенного инструмента,
	чей isin_id указан в аргументах командной строки.
	Также указываются максимальное количество выводимых заявок (глубина стакана, 
	0 - для вывода без ограничений), название выходного файла и признак реверсивной сортировки (r).

	This sample represents getting an aggregate order book for specific instrument 
	whose isin_id is set in command line arguments.
	The other arguments are: order book depth (max quantity of elements in output, 0 for unlimited output),
	output file name and reverse sort flag (r).
*/

#ifndef _WIN32
#include "conio.h"
#else
#include <conio.h>
#include <memory>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <functional>
#include <memory>
#include <iostream>
#include "compilers.h"
#ifdef CGUTILS_HAS_CXX_UNIQUE_PTR
	using std::unique_ptr;
	#define smart_ptr unique_ptr
#else
	using std::auto_ptr;
	#define smart_ptr auto_ptr
#endif

#ifdef _WIN32
#include <windows.h>
#endif//_WIN32

#define CG_SHORT_NAMES
#include <cgate.h>

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
	size_t offset_repl_id;
	size_t offset_repl_revision;
	size_t offset_repl_act;
	size_t offset_isin_id;
	size_t offset_price;
	size_t offset_amount;
	size_t offset_moment;
	size_t offset_direction;
	size_t offset_timestamp;
	size_t offset_session_id;
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
		long orders_aggr;

		TableIndices()
		{
			orders_aggr = -1L;
		}
	} table_indices;

	// названия таблиц в схеме
	//
	// names of tables in scheme
	static struct TableNames
	{
		const char* orders_aggr;
		
		TableNames() : orders_aggr("orders_aggr")	{	}
	} table_names;

	// названия полей в таблицах
	//
	// names of fields in tables
	static struct FieldNames
	{
		const char* repl_id;
		const char* repl_revision;
		const char* repl_act;
		const char* isin_id;
		const char* price;
		const char* amount;
		const char* moment;
		const char* direction;
		const char* timestamp;
		const char* session_id;

		FieldNames()
			: repl_id("replID")
			, repl_revision("replRev")
			, repl_act("replAct")
			, isin_id("isin_id")
			, price("price")
			, amount("volume")	// The database table has a field containing amount data but called as "volume"
			, moment("moment")
			, direction("dir")
			, timestamp("timestamp")
			, session_id("sess_id")
		{}
	} field_names;

	static StreamOffsetInfo data_orders_aggr;

	static bool ready;
};

OrdLogSchemeInfo::TableIndices OrdLogSchemeInfo::table_indices;
OrdLogSchemeInfo::TableNames OrdLogSchemeInfo::table_names;
OrdLogSchemeInfo::FieldNames OrdLogSchemeInfo::field_names;
StreamOffsetInfo OrdLogSchemeInfo::data_orders_aggr;

// Flag false until scheme isn't inited
bool OrdLogSchemeInfo::ready = false;

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
			_msg_names[CG_MSG_OPEN] = "MsgOpen";
			_msg_names[CG_MSG_CLOSE] = "MsgClose";
			_msg_names[CG_MSG_TIME] = "MsgTime";
			_msg_names[CG_MSG_DATA] = "MsgData";
			_msg_names[CG_MSG_STREAM_DATA] = "MsgStreamData";
			_msg_names[CG_MSG_TN_BEGIN] = "MsgTnBegin";
			_msg_names[CG_MSG_TN_COMMIT] = "MsgTnCommit";
			_msg_names[CG_MSG_P2MQ_TIMEOUT] = "MsgP2MQTimeout";
			_msg_names[CG_MSG_P2REPL_LIFENUM] = "MsgP2ReplLifeNum";
			_msg_names[CG_MSG_P2REPL_CLEARDELETED] = "MsgP2ReplClearDeleted";
			_msg_names[CG_MSG_P2REPL_ONLINE] = "MsgP2ReplOnline";
			_msg_names[CG_MSG_P2REPL_REPLSTATE] = "MsgP2ReplReplState";
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

class DisconnectException : public std::exception {};

// Даннные агрегированной заявки
//
// aggregate order data
struct Order
{
	double price;
	int32_t amount;
	RevisionInfo revision_info;

	Order(double price_, int32_t amount_, const RevisionInfo& revInfo_) : 
		price(price_), amount(amount_), revision_info(revInfo_){}

	// чтение данных заявки из stream_data message
	//
	// reading order data from stream_data message
	Order(const cg_msg_streamdata_t* stream_msg)
	{
		const char* msg_data = (const char*) stream_msg->data;
		StreamOffsetInfo* table_info = NULL;
		if(stream_msg->msg_index != OrdLogSchemeInfo::data_orders_aggr.msg_index)
		{
			EXIT_WITH_ERROR("Incorrect table index");
		}
		table_info = &OrdLogSchemeInfo::data_orders_aggr;

		const char* price_ptr = msg_data + table_info->offset_price;
		int64_t price_int = 0;
		int8_t price_scale = 0;
		// convert price from p2 bcd to double
		WARN_FAIL(cg_bcd_get(price_ptr, &price_int, &price_scale));
		price = ((double)price_int)/(pow(10.0, price_scale));
		amount = *(int32_t*)(msg_data + table_info->offset_amount);
		
		// save table idx and order revison
		revision_info.table_idx = (int32_t)stream_msg->msg_index;
		revision_info.table_rev = *(int64_t*)(msg_data + table_info->offset_repl_revision);
	}

	virtual int toString(char* str) const
	{
		return sprintf(str, "Price = %.2lf, amount = %d, table = %d, rev = %lld,",
			price, amount, revision_info.table_idx, (long long int)revision_info.table_rev);
	}
	
	virtual ~Order() { }
};

enum ACTION
{
	ACTION_ADD = 1
};

// расширенные данные агрегированной заявки
// копия содержимого потокового сообщения
// используется для упрощения обработки заявок
//
// extended aggregate order data
// copy of stream_data message contents
// used to simplify orders handling
struct OrderDataEx : public Order
{
	Dir direction;
	int32_t isin_id;
	int64_t repl_id;
	int64_t repl_act;

	// чтение данных заявки из stream_data message
	//
	// reading order data from stream_data message
	OrderDataEx(const cg_msg_streamdata_t* stream_msg)
		: Order(stream_msg)
	{
		const char* msg_data = (const char*) stream_msg->data;

		StreamOffsetInfo* table_info = NULL;
		if(stream_msg->msg_index == OrdLogSchemeInfo::data_orders_aggr.msg_index)
		{
			table_info = &OrdLogSchemeInfo::data_orders_aggr;
		}
		else
		{
			CHECK_FAIL(!"Incorrect table index");
		}

		int8_t dir_int = *(msg_data + table_info->offset_direction);
		direction = (dir_int == 1) ? DirBuy : DirSell;
		isin_id = *(int32_t*)(msg_data + table_info->offset_isin_id);
		repl_id = *(int64_t*)(msg_data + table_info->offset_repl_id);
		repl_act = *(int64_t*)(msg_data + table_info->offset_repl_act);
	}

	virtual int toString(char* str) const
	{
		int orders_str_end = Order::toString(str);
		return orders_str_end + sprintf(
			str + orders_str_end,
			" dir = %s, isinId  = %d, replAct = %lld"
			, (direction == DirBuy) ? "Buy" : "Sell"
			, isin_id
			, (long long int)repl_act);
	}
};

// вспомогательная функция для сравнения цены покупки. лучшая цена - самая большая
//
// helper function object for buying price comparison. bigger price is better
struct OrderComparatorBuy
{
	bool operator() (const Order* o1, const Order* o2) const
	{
		return o1->price > o2->price;
	}
};

// вспомогательная функция для сравнения цены продажи. лучшая цена - самая маленькая
//
// helper function object for selling price comparison. less price is better
struct OrderComparatorSell
{
	bool operator() (const Order* o1, const Order* o2) const
	{
		return o1->price < o2->price;
	}
};

// вспомогательная функция для поиска заявок с указанной ценой
//
// helper function object for searching order with specified price
struct price_equal : public std::binary_function<Order*, double, bool>
{
	bool operator()(const Order* _Order, const double& price) const
	{
		return _Order->price == price;
	}
};

// сортированный в указанном направлении контейнер для агрегированных заявок
//
// sorted in specified direction container for aggregate orders 
template <class TComparator> class Orders
{
	typedef std::set<Order*, TComparator> TOrdersByPrice;
	typedef std::map<int64_t, OrderDataEx> TOrdersByReplId;
	// содержит агрегированные заявки (суммарный объем заявок с одинаковой ценой)
	// отсортирован по цене
	//
	// contains aggregate amount of all equally priced active orders
	// sorted by price
	TOrdersByReplId _orders_aggregate;
	TComparator _compare;

public:
	Orders(){}

	// добавление агрегированной заявки или изменение существующей, 
	// если найдется заявка с равной ценой
	//
	// place new aggregate order or alter existing if found equal price order
	void add(const OrderDataEx& order)
	{
		log_trace("New order record. Table = %d, revision = %d. price = %lf, amount = %d",
			order.revision_info.table_idx, order.revision_info.table_rev, order.price, order.amount);
		typename TOrdersByReplId::iterator it = _orders_aggregate.find(order.repl_id);

		if(it == _orders_aggregate.end())
		{
			_orders_aggregate.insert(TOrdersByReplId::value_type(order.repl_id, order));
		}
		else
		{
			it->second.amount = order.amount;
			it->second.revision_info = order.revision_info;
		}
	}

	// удалить все заявки
	//
	// delete all orders
	void clear()
	{
		_orders_aggregate.clear();
	}

	// обработчик CG_MSG_P2REPL_CLEARDELETED
	// удалить все заявки с ревизией, старшей, чем указанная
	//
	// CG_MSG_P2REPL_CLEARDELETED handler
	// delete all orders with revision older than specified
	void onClearDeleted(const RevisionInfo* pRevInfo)
	{
		for(typename TOrdersByReplId::iterator it = _orders_aggregate.begin(); 
			it != _orders_aggregate.end(); )
		{
			if ((it->second.revision_info.table_idx == pRevInfo->table_idx) &&
				(it->second.revision_info.table_rev < pRevInfo->table_rev))
			{
				_orders_aggregate.erase(it++);
				log_trace(
					"Erasing deleted order with table_idx = %d and table_rev = %lld. Price = %lf, amount = %d",
					it->second.revision_info.table_idx, 
					(long long int)it->second.revision_info.table_rev, 
					it->second.price, 
					it->second.amount);
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
		TOrdersByPrice orders_by_price;
		for(InputIterator it = first; it != last; ++it)
		{
			if ((it->second.price > 0) && (it->second.amount > 0))
			{
				orders_by_price.insert(&(it->second));
			}
		}
		int cntr = 0;
		for (typename TOrdersByPrice::iterator it = orders_by_price.begin();
			it != orders_by_price.end() && cntr < max_count; ++it, ++cntr)
		{
			fprintf(file, "\t\tprice = %.2lf\tamount = %d\n", (*it)->price, (*it)->amount);
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
	OrderBook(int32_t isin_id):_isin_id(isin_id){}
	
	void add(const OrderDataEx& order)
	{
		if (order.direction == DirBuy)
		{
			_orders_buy.add(order);
		}
		else
		{
			_orders_sell.add(order);
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

// основной объект - приложение AggrSpyApp
// реализует логику обработки сообщений
//
// main aggrspy application
// implements message callback and other message handling functions
class AggrSpyApp
{
	cg_conn_t* _connection;
	cg_listener_t *_listener;

	OrderBook _book;

	int _session_id;
	uint32_t _target_isin_id;
	uint32_t _depth;
	std::string _outfile;
	bool _reverse;
	int64_t _last_revision;

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
			if (strcmp(message_desc->name, OrdLogSchemeInfo::table_names.orders_aggr) == 0)
			{
				cg_field_desc_t* field_desc = message_desc->fields;
				OrdLogSchemeInfo::data_orders_aggr.msg_index = index;
				while (field_desc)
				{
					if (strcmp(field_desc->name, OrdLogSchemeInfo::field_names.repl_id) == 0)
					{
						OrdLogSchemeInfo::data_orders_aggr.offset_repl_id = field_desc->offset;
						assert (field_desc->size == sizeof(int64_t));
					} 
					else if (strcmp(field_desc->name, OrdLogSchemeInfo::field_names.repl_revision) == 0)
					{
						OrdLogSchemeInfo::data_orders_aggr.offset_repl_revision = field_desc->offset;
						assert (field_desc->size == sizeof(int64_t));
					} 
					else if (strcmp(field_desc->name, OrdLogSchemeInfo::field_names.repl_act) == 0)
					{
						OrdLogSchemeInfo::data_orders_aggr.offset_repl_act = field_desc->offset;
						assert (field_desc->size == sizeof(int64_t));
					} 
					else if (strcmp(field_desc->name, OrdLogSchemeInfo::field_names.isin_id) == 0)
					{
						OrdLogSchemeInfo::data_orders_aggr.offset_isin_id = field_desc->offset;
						assert (field_desc->size == sizeof(int32_t));
					} 
					else if (strcmp(field_desc->name, OrdLogSchemeInfo::field_names.price) == 0)
					{
						OrdLogSchemeInfo::data_orders_aggr.offset_price = field_desc->offset;
					} 
					else if (strcmp(field_desc->name, OrdLogSchemeInfo::field_names.amount) == 0)
					{
						OrdLogSchemeInfo::data_orders_aggr.offset_amount = field_desc->offset;
						assert (field_desc->size == sizeof(int64_t));
					} 
					else if (strcmp(field_desc->name, OrdLogSchemeInfo::field_names.moment) == 0)
					{
						OrdLogSchemeInfo::data_orders_aggr.offset_moment = field_desc->offset;
					} 
					else if (strcmp(field_desc->name, OrdLogSchemeInfo::field_names.direction) == 0)
					{
						OrdLogSchemeInfo::data_orders_aggr.offset_direction = field_desc->offset;
						assert (field_desc->size == sizeof(int8_t));
					} 
					else if (strcmp(field_desc->name, OrdLogSchemeInfo::field_names.timestamp) == 0)
					{
						OrdLogSchemeInfo::data_orders_aggr.offset_timestamp = field_desc->offset;
					} 
					else if (strcmp(field_desc->name, OrdLogSchemeInfo::field_names.session_id) == 0)
					{
						OrdLogSchemeInfo::data_orders_aggr.offset_session_id = field_desc->offset;
						assert (field_desc->size == sizeof(int32_t));
					} 

					field_desc = field_desc->next;
				}
			}
		}

		OrdLogSchemeInfo::ready = true;
	}

	// основной обработчик сообщений
	//
	// main message callback function
	static CG_RESULT messageCallback(cg_conn_t* conn, cg_listener_t* listener, struct cg_msg_t* message, void* data)
	{
		AggrSpyApp* _this = (AggrSpyApp*)data;
		if (_this == NULL)
			return CG_ERR_INVALIDARGUMENT;

		// открытие нового соединения с сервером
		// инициализировать схему
		//
		// opening new server connection
		// initialize scheme description
		switch (message->type)
		{
		case CG_MSG_OPEN:
			{
				_this->logMessage(message);
				_this->cleanup(); // in case of reconnect
				_this->initializeScheme(listener);
				break;
			}

			// обрабатывать только таблицу FUTAGGR50.orders_aggr
			// пропускать остальные таблицы
			// пропускать сообщения для инструментов, отличных от _target_isin_id
			//
			// process only FUTAGGR50.orders_aggrtable
			// skip other tables
			// skip messages for instruments other than _target_isin_id
		case CG_MSG_STREAM_DATA:
			{
				struct cg_msg_streamdata_t* stream_msg = (struct cg_msg_streamdata_t* )message;
				StreamOffsetInfo* table_info = &OrdLogSchemeInfo::data_orders_aggr;

				// игнорировать остальные таблицы
				// ignore all other tables
				if(stream_msg->msg_index != table_info->msg_index)
				{
					break;
				}
				const char* msg_data = (const char*)stream_msg->data;

				int32_t isin_id = *(int32_t*)(msg_data + table_info->offset_isin_id);
				// пропуск сообщений для других инструментов
				// skip messages for other instruments
				if (isin_id != (int32_t)_this->_target_isin_id)
				{
					return CG_ERR_OK;
				}

				int32_t sess_id = *(int32_t*)(msg_data + table_info->offset_session_id);
				// пропуск сообщений с предыдущих сессий
				// ignore messages from older sessions
				if(sess_id < _this->_session_id)
				{
					break;
				}
				// сравнение полученного sess_id с текущим
				// изменение текущего номера сессии при обновлении
				// compare session id with current
				// update if session_id from message data is newer than current
				_this->updateSessionId(sess_id);

				// заполнение заявки из потокового сообщения
				// filling order data from stream message
				OrderDataEx order(stream_msg);
				_this->logMessage(&order);

				_this->_book.add(order);

				// запоминание текущего номера ревизии
				// remembering current revision
				_this->_last_revision = order.revision_info.table_rev;
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
			// очистить все данные при смене номера жизни
			//
			// clear all data when life number is changed
		case CG_MSG_P2REPL_LIFENUM:
			{
				_this->logMessage(message);
				log_info("Life number is changed");
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
				
				if (rev_info->table_idx == OrdLogSchemeInfo::data_orders_aggr.msg_index)
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
				break;
			}
		};

		return CG_ERR_OK;
	}

public:
	AggrSpyApp(cg_conn_t* conn, const char* settings, 
		uint32_t target_isin_id, uint32_t depth, const std::string& outfile, bool reverse)
		: _connection(conn)
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

		_msg_log_file = fopen("aggrspy msg.log", "w");
		if(_msg_log_file == NULL)
		{
			log_error("Unable to create msglog file");
		}

		log_info("OrderBook application for isin_id = %d created, settings = %s", target_isin_id, settings);
	}

	~AggrSpyApp()
	{
		log_info("Closing OrderBook application");
		if (_listener)
			cg_lsn_destroy(_listener);
				
		cleanup();
		if (_msg_log_file)
			fclose(_msg_log_file);
	}
	
	void cleanup()
	{
		log_trace("Performing cleanup");
		_book.clear();
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
				char file_name[256];
				sprintf(file_name, "aggrspy session %d.log", _session_id);
				print(file_name);
				
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
	bool print(const char* name)
	{
		FILE* file = fopen(name, "w");
		if(file == NULL)
		{
			return false;
		}

		fprintf(file, "BUY:\n");
		_book.print(DirBuy, file, _depth, _reverse);
		fprintf(file, "SELL:\n");
		_book.print(DirSell, file, _depth, _reverse);
		fprintf(file, "\n");
		fclose(file);
		return true;
	}

	// записать содержимое стакана в текстовый файл
	// добавить в название файла номер текущей ревизии
	//
	// write current order books content to text file
	// add current revision number to file name
	bool printLastRevision()
	{
		char filename[256];
		sprintf(filename, "aggrspy %d.%lld.log", _target_isin_id, (long long int)_last_revision);
		
		return print(filename);
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
		else
		if (state == CG_STATE_CLOSED)
		{
			WARN_FAIL(cg_conn_open(_connection, 0));
		}
		else
		if (state == CG_STATE_ACTIVE)
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
	conn_t* conn = 0;
	// listener pointer
	listener_t* listener = 0;

	// connection init string
	const char* conn_str = "p2tcp://127.0.0.1:4001;app_name=aggrspy";

#ifdef _WIN32
	SetConsoleCtrlHandler(InterruptHandler, 1); 
#endif//_WIN32

	if (argc < 4) 
	{
		printf("Usage:\n\taggrspy ISIN_ID depth outfile r(optional)\n\n"
			"where\n\tISIN_ID is unique instrument ID (e.g. 167566)\n"
			"\tdepth is max orderbook size (0 for unlimited size)\n"
			"\toutfile is output file path"
			"\tr for reverse sorting");
		return 2;
	}
	
	uint32_t isin_id = atoi(argv[1]);
	uint32_t depth = atoi(argv[2]);
	std::string outfile(argv[3]);
	bool reverse = false;
	if((argc >= 5) && (strcmp(argv[4], "r") == 0))
	{
		reverse = true;
	}

	// Environment is initialized using provided INI-file
	CHECK_FAIL(env_open("ini=aggrspy.ini;key=11111111;minloglevel=trace"));
	// Connection is initialized using connection init string
	CHECK_FAIL(conn_new(conn_str, &conn));

	try
	{
		AggrSpyApp app(conn, "p2repl://FORTS_FUTAGGR50_REPL", isin_id, depth, outfile, reverse);
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
				char ch = getchar();
				if(ch == 0x0a)
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
	if (conn != NULL)
		conn_destroy(conn);


	// close environment
	CHECK_FAIL(env_close());
	return 0;
}
