#include "stdafx.h"
#include "Plaza2Objects.h"

P2OrderBook::P2OrderBook() : currOrderSize(0)
{
}

P2OrderBook::~P2OrderBook()
{
}

void P2OrderBook::AddToPrice(Order* const order)
{
	if (order->direction == P2Dir::BID)
	{
		/*if (_orders_sell.Size() && order->price < _orders_sell.Get(0)->price)
			_orders_buy.Add(order);
		else if (!_orders_sell.Size())*/
			_orders_buy.Add(order);
	}
	else
	{
		/*if (_orders_buy.Size() && _orders_buy.Get(0)->price < order->price)
			_orders_sell.Add(order);
		else if (!_orders_buy.Size())*/
			_orders_sell.Add(order);
	}

	currOrderSize = order->volume;
}

void P2OrderBook::SetPriceLevel(Order* const order)
{
	if (order->direction == P2Dir::BID)
	{
		/*if (_orders_sell.Size() && order->price < _orders_sell.Get(0)->price)
			_orders_buy.SetPrice(order);
		else if (!_orders_sell.Size())*/
			_orders_buy.SetPrice(order);
	}
	else
	{
		/*if (_orders_buy.Size() && _orders_buy.Get(0)->price < order->price)
			_orders_sell.SetPrice(order);
		else if (!_orders_buy.Size())*/
			_orders_sell.SetPrice(order);
	}
}

void P2OrderBook::SetTrade(const P2Trade& trade)
{
	if (trade.agressorSide == SELL)
	{
		_orders_buy.Trade(trade);
	}
	else
	{
		_orders_sell.Trade(trade);
	}
}

void P2OrderBook::SetTime(const cg_time_t& t)
{
	time = t;
}

void P2OrderBook::RemoveFromPrice(Order* const order)
{
	if (order->direction == P2Dir::BID)
	{
		_orders_buy.Remove(order);
	}
	else
	{
		_orders_sell.Remove(order);
	}
	
	currOrderSize -= order->volume;
}

void P2OrderBook::RemovePriceLevel(Order* const order)
{
	if (order->direction == P2Dir::BID)
	{
		_orders_buy.RemovePrice(order);
	}
	else
	{
		_orders_sell.RemovePrice(order);
	}
}

void P2OrderBook::ClearPreviousInfo()
{
	_orders_buy.ClearInfo();
	_orders_sell.ClearInfo();
	currOrderSize = 0;
}

void P2OrderBook::Clear()
{
	_orders_buy.Clear();
	_orders_sell.Clear();
}

bool P2OrderBook::GetBestBidOrder(Order& order)
{
	const Order* ord = _orders_buy.Get(0);

	if (ord)
	{
		order = *ord;
		return true;
	}
	else
		return false;
}

bool P2OrderBook::GetBestAskOrder(Order& order)
{
	const Order* ord = _orders_sell.Get(0);

	if (ord)
	{
		order = *ord;
		return true;
	}
	else
		return false;
}

bool P2OrderBook::GetBestBidOrderWithVolume(Order& order)
{
	size_t indx = 0;
	const Order* ord = _orders_buy.Get(indx);

	if (ord)
	{
		size_t size = _orders_buy.Size();
		while (!ord->volume)
		{
			if (indx < size-1)
			{
				++indx;
				ord = _orders_buy.Get(indx);
			}
			else 
				return false;
		}
		order = *ord;
		return true;
	}
	else
		return false;
}

bool P2OrderBook::GetBestAskOrderWithVolume(Order& order)
{
	size_t indx = 0;
	const Order* ord = _orders_sell.Get(indx);

	if (ord)
	{
		size_t size = _orders_sell.Size();
		while (!ord->volume)
		{
			if (indx < size-1)
			{
				++indx;
				ord = _orders_sell.Get(indx);
			}
			else 
				return false;
		}
		order = *ord;
		return true;
	}
	else
		return false;
}

Order* P2OrderBook::GetOrderByPrice(const P2Price& price, P2Dir direction)
{
	Order* order = NULL;

	if (direction == P2Dir::BID)
		order = _orders_buy.GetOrderByPrice(price);
	else
		order = _orders_sell.GetOrderByPrice(price);

	return order ? order : NULL;
}

void P2OrderBook::PrintToFile(std::string& fileName)
{	
	std::fstream m_fs;
	m_fs.open(fileName, std::ios::in | std::ios::out | std::ios::app);
		
	if (!m_fs.is_open())
	{
		std::string err;
		err.append("Error opening file: %s\n", fileName.c_str());
		m_fs.close();
		throw std::exception(err.c_str());	
	}
	
	//Time
	char str[100];
	sprintf_s(str, 100, "\t%02d:%02d:%02d.%03d\t", time.hour, time.minute, time.second, time.msec);
	m_fs << str;

	//Bids
	for (size_t i = _orders_buy.Size(); i != 0; --i)
	{
		m_fs << _orders_buy.Get(i-1)->price.price_double << " (" << _orders_buy.Get(i-1)->volume << "/" << _orders_buy.Get(i-1)->tradeSize << ")\t";
	}
	
	m_fs << "\t|\t\t";

	//Asks
	for (size_t i = 0; i != _orders_sell.Size(); ++i)
	{
		m_fs << _orders_sell.Get(i)->price.price_double << " (" << _orders_sell.Get(i)->volume << "/" << _orders_sell.Get(i)->tradeSize << ")\t";
	}
		
	m_fs << '\n';
}
