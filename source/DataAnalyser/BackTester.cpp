#include "stdafx.h"
#include "BackTester.h"


void BackTester::CreateStrategies(const fo2base::UnixNanosecondsTime& timeDelay, TradeSessionData& data, const std::string& pathToLog)
{
    strategyList.push_back(std::shared_ptr<TestStrategyBaseClass>(new MyFirstStrategy(timeDelay, data, pathToLog)));
}

void BackTester::InitStrategies(const fo2base::UnixNanosecondsTime localTime)
{
	for (auto& val : strategyList)
	{
		val.get()->InitStrategy(localTime);
	}
}

void BackTester::PreEventActions(const FORTS_ORDLOG_REPL::orders_log& info)
{
    for (auto& strategy_ptr : strategyList)
    {
        strategy_ptr->PreEventActions(info);
    }
}

void BackTester::PostEventActions(const FORTS_ORDLOG_REPL::orders_log& info)
{
    for (auto& strategy_ptr : strategyList)
    {
        strategy_ptr->PostEventActions(info);
    }
}

void BackTester::PostEventActions(const FORTS_DEALS_REPL::deal& info)
{
    for (auto& strategy_ptr : strategyList)
    {
        strategy_ptr->PostEventActions(info);
    }
}

void BackTester::EndSessionAction(const std::string& date)
{
	for (auto& strategy_ptr : strategyList)
	{
		strategy_ptr->EndSessionAction(date);
	}
}

void BackTester::EndSessionRangeAction()
{
	for (auto& strategy_ptr : strategyList)
	{
		strategy_ptr->EndSessionRangeAction();
	}
}

std::string BackTester::GetStrategyResult() const
{
    std::string result;
    int count = 0;
    for (const auto& strategy_ptr : strategyList)
    {
        result += "Backtest strategy #" + std::to_string(++count) + " result:\n";
        result += strategy_ptr->GetStrategyResult() + '\n';

		strategy_ptr->StrategyResultToLog("\n" + strategy_ptr->GetStrategyResult());
    }
    return result;
}

const Position* BackTester::GetIntermediatePositionResult(const InstrumentID isin_id, const fo2base::UnixNanosecondsTime time) const
{
    const TestStrategyBaseClass& strategy = **strategyList.begin(); // эта функция используется только когда отлаживается конкретная стратегия, поэтому всегда берётся первая стратегия
    return strategy.GetIntermediatePositionResult(isin_id, time);
}

const SOrderEx* BackTester::GetTrade(const InstrumentID isin_id, const fo2base::UnixNanosecondsTime time) const
{
    const TestStrategyBaseClass& strategy = **strategyList.begin(); // эта функция используется только когда отлаживается конкретная стратегия, поэтому всегда берётся первая стратегия
    return strategy.GetTrade(isin_id, time);
}

const double* BackTester::GetIndicator1(const InstrumentID isin_id, const fo2base::UnixNanosecondsTime time) const
{
	const TestStrategyBaseClass& strategy = **strategyList.begin(); // эта функция используется только когда отлаживается конкретная стратегия, поэтому всегда берётся первая стратегия
	return strategy.GetIndicator1(isin_id, time);
}

const double* BackTester::GetIndicator2(const InstrumentID isin_id, const fo2base::UnixNanosecondsTime time) const
{
	const TestStrategyBaseClass& strategy = **strategyList.begin(); // эта функция используется только когда отлаживается конкретная стратегия, поэтому всегда берётся первая стратегия
	return strategy.GetIndicator2(isin_id, time);
}

void TestStrategyBaseClass::AddOrder(const InstrumentID isin_id, const t_id order_id)
{
    Action action;
    action.type = ADD_ORDER;
    action.isin_id = isin_id;
    action.ids.insert(order_id);
    pendingActions[isin_id].insert(std::make_pair(lastEventTime + timeDelay, action));
	waitActionResult = true;
	StrategyOrdlogToLog(lastEventTime, isin_id,
		action.type == ADD_ORDER ? "ADD_ORDER" :
		action.type == CANCEL_ORDER ? "CANCEL_ORDER" :
		action.type == MASS_CANCEL ? "MASS_CANCEL" : "MOVE_ORDER",
		GetOrder(isin_id, order_id));
}

void TestStrategyBaseClass::CancelOrder(const InstrumentID isin_id, const t_id order_id)
{
    Action action;
    action.type = CANCEL_ORDER;
    action.isin_id = isin_id;
    action.ids.insert(order_id);
    pendingActions[isin_id].insert(std::make_pair(lastEventTime + timeDelay, action));
	waitActionResult = true;
	StrategyOrdlogToLog(lastEventTime, isin_id,
		action.type == ADD_ORDER ? "ADD_ORDER" :
		action.type == CANCEL_ORDER ? "CANCEL_ORDER" :
		action.type == MASS_CANCEL ? "MASS_CANCEL" : "MOVE_ORDER",
		GetOrder(isin_id, order_id));
}

void TestStrategyBaseClass::MassCancelOrder(const InstrumentID isin_id, const std::set<t_id>& order_ids)
{
    Action action;
    action.type = MASS_CANCEL;
    action.isin_id = isin_id;
    action.ids = order_ids;
    pendingActions[isin_id].insert(std::make_pair(lastEventTime + timeDelay, action));
	waitActionResult = true;
	
	for (const t_id id : order_ids)
	{
		StrategyOrdlogToLog(lastEventTime, isin_id,
			action.type == ADD_ORDER ? "ADD_ORDER" :
			action.type == CANCEL_ORDER ? "CANCEL_ORDER" :
			action.type == MASS_CANCEL ? "MASS_CANCEL" : "MOVE_ORDER",
			GetOrder(isin_id, id));
	}
}

void TestStrategyBaseClass::MoveOrders(const InstrumentID isin_id, const std::set<t_id>& order_ids)
{
    Action action;
    action.type = MOVE_ORDER;
    action.isin_id = isin_id;
    action.ids = order_ids;
    pendingActions[isin_id].insert(std::make_pair(lastEventTime + timeDelay, action));
	waitActionResult = true;

	for (const t_id id : order_ids)
	{
		StrategyOrdlogToLog(lastEventTime, isin_id,
			action.type == ADD_ORDER ? "ADD_ORDER" :
			action.type == CANCEL_ORDER ? "CANCEL_ORDER" :
			action.type == MASS_CANCEL ? "MASS_CANCEL" : "MOVE_ORDER",
			GetOrder(isin_id, id));
	}
}

void TestStrategyBaseClass::OrderFilled(const InstrumentID isin_id, const t_id order_id, const fo2base::UnixNanosecondsTime coreTime)
{
    // TODO: изменяем позицию и размеры ордера если он зафилин не полностью, а если полностью то убираем его

    submittedOrders.at(isin_id).erase(order_id);
    completedOrders[isin_id].insert(order_id);

    // пока считаем что ордер был зафилен полностью на весь объём
    SOrderEx& order = GetOrder(isin_id, order_id);
	order.status = SOrderEx::FILLED;
    order.remainedQuantity = 0;

    // добавим в массив всех трейдов
    allTrades[isin_id].insert(std::make_pair(coreTime, &order));

    // обновим позиции
    const double price = order.realPrice;
    const uint64_t quantity = order.initialQuantity;
    const bool is_long = (order.dir == BID) ? true : false;
    PositionEx& position = positions[isin_id];
    position.AddDeal(is_long, static_cast<int>(quantity), price);

	StrategyOrdlogToLog(coreTime, isin_id, "OrderFilled", order);
	StrategyTradesToLog(coreTime, isin_id, price, quantity, is_long, position.GetPos(), int(position.GetClosedPnL() / sessionData.GetMinStep(isin_id)));
}

void TestStrategyBaseClass::PreEventActions(const FORTS_ORDLOG_REPL::orders_log& info)
{
    lastEventTime = info.moment_ns;
    CheckPendingActions();
	PreEventLogic(info);
}

const Position* TradingInterface::GetIntermediatePositionResult(const InstrumentID isin_id, const fo2base::UnixNanosecondsTime time) const
{
    if (positionChanges.find(isin_id) == positionChanges.end() || positionChanges.at(isin_id).size() == 0)
    {
        return NULL;
    }

	return &positionChanges.at(isin_id).at(time);
}

const SOrderEx* TradingInterface::GetTrade(const InstrumentID isin_id, const fo2base::UnixNanosecondsTime time) const
{
    if (allTrades.find(isin_id) == allTrades.end() || allTrades.at(isin_id).size() == 0)
    {
        return NULL;
    }

    const SOrderEx* order = NULL;
    for (const auto& mapValue : allTrades.at(isin_id))
    {
        const fo2base::UnixNanosecondsTime eventTime = mapValue.first;

        if (time >= eventTime)
        {
            order = mapValue.second;
        }
        else
        {
            break;
        }
    }
    return order;
}

const double* TestStrategyBaseClass::GetIndicator1(const InstrumentID isin_id, const fo2base::UnixNanosecondsTime time) const
{
	if (indicator1Map.find(isin_id) == indicator1Map.end() || 
		indicator1Map.at(isin_id).size() == 0 || 
		indicator1Map.at(isin_id).find(time) == indicator1Map.at(isin_id).end())
	{
		return NULL;
	}

	return &indicator1Map.at(isin_id).at(time);

	/*const double* indicatorValue = NULL;
	for (const auto& pair : indicator1Map.at(isin_id))
	{
		const fo2base::UnixNanosecondsTime eventTime = pair.first;

		if (time == eventTime)
		{
			return &pair.second;
		}
		else if (time > eventTime)
		{
			indicatorValue = &pair.second;
		}
		else if (time < eventTime)
		{
			break;
		}
	}

	return indicatorValue;*/
}

const double* TestStrategyBaseClass::GetIndicator2(const InstrumentID isin_id, const fo2base::UnixNanosecondsTime time) const
{
	if (indicator2Map.find(isin_id) == indicator2Map.end() || 
		indicator2Map.at(isin_id).size() == 0 ||
		indicator2Map.at(isin_id).find(time) == indicator2Map.at(isin_id).end())
	{
		return NULL;
	}

	return &indicator2Map.at(isin_id).at(time);

	/*const double* indicatorValue = NULL;
	for (const auto& pair : indicator2Map.at(isin_id))
	{
		const fo2base::UnixNanosecondsTime eventTime = pair.first;

		if (time == eventTime)
		{
			return &pair.second;
		}
		else if (time > eventTime)
		{
			indicatorValue = &pair.second;
		}
		else if (time < eventTime)
		{
			break;
		}
	}

	return indicatorValue;*/
}


void TradingInterface::UpdatePnL(const InstrumentID isin_id, const P2Action action, const char* deal_price, const fo2base::UnixNanosecondsTime time)
{
    if (action != P2Action::TRADE)
    {
        return;
    }

	if (positions.find(isin_id) != positions.end())
	{
		double dDealPrice = 0;
		P2BCDII2Double(&dDealPrice, deal_price);
		PositionEx& position = positions.at(isin_id);
		position.CalculateLiquidationPnL(dDealPrice);
		positionChanges[isin_id].insert(std::make_pair(time, static_cast<Position>(position)));
	}
}

void TestStrategyBaseClass::CheckPendingActions()
{
    for (const std::pair<InstrumentID, std::map<fo2base::UnixNanosecondsTime, Action>>& pair : pendingActions)
    {
        const InstrumentID isin_id = pair.first;

        for (const auto& _pair : pair.second)
        {
            const fo2base::UnixNanosecondsTime actionTime = _pair.first;
            const fo2base::UnixNanosecondsTime goodTime = actionTime + timeDelay;
            const Action& action = _pair.second;

            if (goodTime <= lastEventTime)
            {
                switch (action.type)
                {
                case ADD_ORDER:
                    CheckAdd(action.isin_id, *(action.ids.begin()), lastEventTime);
                    break;

                case CANCEL_ORDER:
                    CheckCancel(action.isin_id, *(action.ids.begin()), lastEventTime);
                    break;

                case MASS_CANCEL:
                    CheckMassCancel(action.isin_id, action.ids, lastEventTime);
                    break;

                case MOVE_ORDER:
                    CheckMove(action.isin_id, action.ids, lastEventTime);
                    break;
                }

                pendingActions.at(isin_id).erase(actionTime);   // возможно этот цикл надо через итераторы делать, ато Access violation будет
				waitActionResult = false;
            }
        }
    }
}

void TestStrategyBaseClass::CheckAdd(const InstrumentID isin_id, const t_id order_id, const fo2base::UnixNanosecondsTime localTime)
{
    SOrderEx& order = GetOrder(isin_id, order_id);
    const TradeSessionData::PriceLimits& priceLimits = sessionData.GetPriceLimits(isin_id);
    const Price& upLimit = priceLimits.first;
    const Price& downLimit = priceLimits.second;

    if (order.initialPrice < downLimit.ToDouble() || order.initialPrice > upLimit.ToDouble())   // проверка на выход за лимиты колебания цен
    {
        RejectAdd(isin_id, order_id, localTime);
		return;
    }

    Price bestBidPrice, bestAskPrice;
    sessionData.GetBestPrices(isin_id, bestBidPrice, bestAskPrice);
    
    switch (order.type)
    {
    case SOrderEx::FAS:
		ConfirmAdd(isin_id, order_id, localTime); // сперва добавляем ордер в стакан, а потом уже филим его
        if ((order.dir == BID && order.initialPrice >= bestAskPrice.ToDouble()) || (order.dir == ASK && order.initialPrice <= bestBidPrice.ToDouble()))
        {
            order.realPrice = (order.dir == BID) ? bestAskPrice.ToDouble(): bestBidPrice.ToDouble();  // добавить сюда нормальный алгоритм расчёта WAPrice для сделки и перенести это внутрь функции OrderFilled
            OrderFilled(isin_id, order_id, localTime);
            //CheckFilling(isin_id, order_id);
        }
        break;

    case SOrderEx::IOC:
    case SOrderEx::FOK:
        if ((order.dir == BID && order.initialPrice >= bestAskPrice.ToDouble()) || (order.dir == ASK && order.initialPrice <= bestBidPrice.ToDouble()))
        {
            order.realPrice = (order.dir == BID) ? bestAskPrice.ToDouble(): bestBidPrice.ToDouble();
            OrderFilled(isin_id, order_id, localTime);
            //CheckFilling(isin_id, order_id);
        }
        else
        {
            RejectAdd(isin_id, order_id, localTime);
        }
        break;
    }
}

void TestStrategyBaseClass::CheckCancel(const InstrumentID isin_id, const t_id order_id, const fo2base::UnixNanosecondsTime localTime)
{
    if (submittedOrders.at(isin_id).find(order_id) != submittedOrders.at(isin_id).end())
    {
        ConfirmCancel(isin_id ,order_id, localTime);
    }
    else
    {
        RejectCancel(isin_id, order_id, localTime);
    }
}

void TestStrategyBaseClass::CheckMassCancel(const InstrumentID isin_id, const std::set<t_id>& order_ids, const fo2base::UnixNanosecondsTime coreTime)
{
    if (submittedOrders.at(isin_id).find(*order_ids.begin()) != submittedOrders.at(isin_id).end())
    {
        ConfirmMassCancel(isin_id , order_ids, coreTime);
    }
    else
    {
        RejectMassCancel(isin_id, order_ids, coreTime);
    }
}

void TestStrategyBaseClass::CheckMove(const InstrumentID isin_id, const std::set<t_id>& order_ids, const fo2base::UnixNanosecondsTime coreTime)
{
    // TODO
}

//void MyFirstStrategy::CheckFilling(const SOrder* order, const bool isBidOrder, const OrderType type)
//{
//    Price bestBidPrice, bestAskPrice;
//    sessionData.GetBestPrices(isin_id, bestBidPrice, bestAskPrice);
//
//
//    const int64_t quantity;
//    OrderFilled(isin_id, order_id, quantity);
//}

void TestStrategyBaseClass::ConfirmAdd(const InstrumentID isin_id, const t_id order_id, const fo2base::UnixNanosecondsTime localTime)
{
    submittedOrders[isin_id].insert(order_id);
    SOrderEx& order = GetOrder(isin_id, order_id);
	order.status = SOrderEx::SUBMITTED;
    order.responseTime = localTime;

	StrategyOrdlogToLog(lastEventTime, isin_id, __func__, order);
}

void TestStrategyBaseClass::ConfirmCancel(const InstrumentID isin_id, const t_id order_id, const fo2base::UnixNanosecondsTime localTime)
{
    submittedOrders.at(isin_id).erase(order_id);
    completedOrders[isin_id].insert(order_id);
    SOrderEx& order = GetOrder(isin_id, order_id);
	order.status = SOrderEx::CANCELED;
    order.responseTime = localTime;

	StrategyOrdlogToLog(lastEventTime, isin_id, __func__, order);
}

void TestStrategyBaseClass::ConfirmMassCancel(const InstrumentID isin_id, const std::set<t_id>& order_ids, const fo2base::UnixNanosecondsTime localTime)
{
    for (const t_id order_id : order_ids)
    {
        submittedOrders.at(isin_id).erase(order_id);
        completedOrders[isin_id].insert(order_id);
        SOrderEx& order = GetOrder(isin_id, order_id);
		order.status = SOrderEx::CANCELED;
        order.responseTime = localTime;

		StrategyOrdlogToLog(lastEventTime, isin_id, __func__, order);
    }
}

void TestStrategyBaseClass::ConfirmMove(const InstrumentID isin_id, const std::set<t_id>& order_ids, const fo2base::UnixNanosecondsTime localTime)
{
    // TODO

}

void TestStrategyBaseClass::RejectAdd(const InstrumentID isin_id, const t_id order_id, const fo2base::UnixNanosecondsTime localTime)
{
    SOrderEx& order = GetOrder(isin_id, order_id);
	order.status = SOrderEx::REJECTED_ADD;
    order.responseTime = localTime;
    
    rejectionCounter[ADD_ORDER].insert(&order);

	StrategyOrdlogToLog(lastEventTime, isin_id, __func__, order);
}

void TestStrategyBaseClass::RejectCancel(const InstrumentID isin_id, const t_id order_id, const fo2base::UnixNanosecondsTime localTime)
{
    SOrderEx& order = GetOrder(isin_id, order_id);
	order.status = SOrderEx::REJECTED_CANCLE;
    order.responseTime = localTime;

    rejectionCounter[CANCEL_ORDER].insert(&order);

	StrategyOrdlogToLog(lastEventTime, isin_id, __func__, order);
}

void TestStrategyBaseClass::RejectMassCancel(const InstrumentID isin_id, const std::set<t_id>& order_ids, const fo2base::UnixNanosecondsTime localTime)
{
    for (const t_id order_id : order_ids)
    {
		SOrderEx& order = GetOrder(isin_id, order_id);
		order.status = SOrderEx::REJECTED_CANCLE;
        order.responseTime = localTime;
        
        rejectionCounter[MASS_CANCEL].insert(&order);

		StrategyOrdlogToLog(lastEventTime, isin_id, __func__, order);
    }	
}

void TestStrategyBaseClass::RejectMove(const InstrumentID isin_id, const std::set<t_id>& order_ids, const fo2base::UnixNanosecondsTime localTime)
{
    for (const t_id order_id : order_ids)
    {
        SOrderEx& order = GetOrder(isin_id, order_id);
		order.status = SOrderEx::REJECTED_MOVE;
        order.responseTime = localTime;

        rejectionCounter[MOVE_ORDER].insert(&order);

		StrategyOrdlogToLog(lastEventTime, isin_id, __func__, order);
    }
}

void MyFirstStrategy::InitStrategy(const fo2base::UnixNanosecondsTime localTime)
{
	InitLogs(localTime);

	params.insert(std::make_pair(Si_isin_id, StrategyParams(sessionData.GetMinStep(Si_isin_id))));
	params.insert(std::make_pair(RTS_isin_id, StrategyParams(sessionData.GetMinStep(RTS_isin_id))));
	params.insert(std::make_pair(MIX_isin_id, StrategyParams(sessionData.GetMinStep(MIX_isin_id))));
	params.insert(std::make_pair(MIX_isin_id, StrategyParams(sessionData.GetMinStep(MXI_isin_id))));

	//const int seconds = 60;
	//const int minutes = 5;
	//const uint64_t windowTimeInterval = 1000000ll * 1000 * seconds * minutes;
	//const unsigned int vpin1Volume = 1000;	// 100, 500, 1000, 5000
	//const unsigned int vpin2Volume = 500;	// 100, 500, 1000, 5000
	//const unsigned int tradesCount = 500;
	
	// /Si-9.17 Si_isin_id/ /RTS-9.17 RTS_isin_id/ /SBRF-9.17 935386/ /BR-9.17 971878/ /MXI-9.17 MIX_isin_id/
	//params.insert(std::make_pair(Si_isin_id, StrategyParams(50, 1, windowTimeInterval, tradesCount, vpin1Volume, vpin2Volume, OPEN_POSITION, CLOSE_POSITION)));  // 40
	//params.insert(std::make_pair(RTS_isin_id, StrategyParams(50, 1, windowTimeInterval, tradesCount, vpin1Volume, vpin2Volume, OPEN_POSITION, CLOSE_POSITION)));  // 20
	//params.insert(std::make_pair(935386, StrategyParams(40, 1)));  // 35
	//params.insert(std::make_pair(MIX_isin_id, StrategyParams(8, 1, windowTimeInterval, tradesCount, vpin1Volume, vpin2Volume, OPEN_POSITION, CLOSE_POSITION)));   // 6
}

void MyFirstStrategy::PostEventActions(const FORTS_ORDLOG_REPL::orders_log& info)
{
    // проверка на сведение перед торговой логикой
    CheckFillingForSubmittedOrdersAfterNewAction(info.moment_ns);

    UpdatePnL(info.isin_id, static_cast<P2Action>(info.action), info.deal_price, info.moment_ns);

	if (params.find(info.isin_id) != params.end())
	{
		PostORDLOGLogic(info);
	}	

  //  for (std::pair<const InstrumentID, StrategyParams>& pair : params)
  //  {
		//if (info.isin_id == pair.first)	// инструмента может и не быть в данной стратегии, т.к. он торгуется в другой стратегии
		//{
		//	PostORDLOGLogic(pair.first, pair.second);
		//}
//  }
}

void MyFirstStrategy::PostEventActions(const FORTS_DEALS_REPL::deal& info)
{
	// проверка на сведение перед торговой логикой
	CheckFillingForSubmittedOrdersAfterNewAction(info.moment_ns);

	for (std::pair<const InstrumentID, StrategyParams>& pair : params)
	{
		PostDEALSLogic(pair.first, pair.second, info.id_deal);
	}
}

void MyFirstStrategy::CheckFillingForSubmittedOrdersAfterNewAction(const fo2base::UnixNanosecondsTime eventTime)
{
	// TODO: реализовать раздельную проверку на филл для разных типов ордеров и учитывать объём ордеров

	for (const std::pair<InstrumentID, std::set<t_id>> pair : submittedOrders)
	{
		const InstrumentID isin_id = pair.first;
		const std::set<t_id>& idSet = pair.second;
		Price bestBidPrice, bestAskPrice;
		sessionData.GetBestPrices(isin_id, bestBidPrice, bestAskPrice);

		for (const t_id order_id : idSet)
		{
			SOrderEx& order = GetOrder(isin_id, order_id);
			bool isFilled = ((order.dir == BID && order.initialPrice >= bestAskPrice.ToDouble()) || (order.dir == ASK && order.initialPrice <= bestBidPrice.ToDouble()));
			if (isFilled)
			{
				order.realPrice = (order.dir == BID) ? bestAskPrice.ToDouble() : bestBidPrice.ToDouble();  // добавить сюда нормальный алгоритм расчёта WAPrice для сделки и перенести это внутрь функции OrderFilled
				OrderFilled(isin_id, order_id, eventTime);
			}
		}
	}
}

void MyFirstStrategy::PreEventLogic(const FORTS_ORDLOG_REPL::orders_log& info)
{
	return;

	//if (info.action == P2Action::TRADE || !(info.status & QUOTE))	// учитывает только котировочные заявки, IOC и FAK в стакан не встанут, поэтому их пропускаем
	//{
	//	return;
	//}

	//StrategyParams* parameters = NULL;

	//for (std::pair<const InstrumentID, StrategyParams>& pair : params)
	//{
	//	if (info.isin_id == pair.first)	// инструмента может и не быть в данной стратегии, т.к. он торгуется в другой стратегии, стратегий ведь несколько одновременно можно тестировать
	//	{
//		parameters = &pair.second;
//	}
//}

//Price bestBidPrice, bestAskPrice;
//sessionData.GetBestPrices(info.isin_id, bestBidPrice, bestAskPrice);

//double orderPrice = 0;
//P2BCDII2Double(&orderPrice, info.price);

//char value = 0;

//if (orderPrice >= bestBidPrice.ToDouble() && orderPrice < bestAskPrice.ToDouble())
//{
//	if (info.action == P2Action::ADD)
//	{
//		++value;
//	}
//	else if (info.action == P2Action::REMOVE)
//	{
//		--value;
//	}
//}
//else if (orderPrice <= bastAskPrice.ToDouble() && orderPrice > bestBidPrice.ToDouble())
//{
//	if (info.action == P2Action::ADD)
//	{
//		--value;
//	}
//	else if (info.action == P2Action::REMOVE)
//	{
//		++value;
//	}
//}
//else
//{
//	return;
//}

//parameters->spreadIndicator.AddValue(value, info.moment_ns);
}

void MyFirstStrategy::PostORDLOGLogic(const FORTS_ORDLOG_REPL::orders_log& info)
{
	//const AggregatedTrade* tradePtr = sessionData.GetLastTrade(info.isin_id);
	//if (info.isin_id == RTS_isin_id &&
	//	sessionData.isLastAggrTradeReady(RTS_isin_id) &&
	//	tradePtr->ordID != lastDealID &&
	//	tradePtr->executedPriceLevelsCount >= priceLevelsAbsorb)
	//{
	//	lastAbsorbTradeTime = tradePtr->time;
	//	agressorSide = tradePtr->agressorSide;
	//	ArbitrageParams* paramsPtr = new ArbitrageParams();
	//	lastArbParams = paramsPtr;
	//	paramsPtr->RTS_OrdersAbsorbCount = tradePtr->oppositeOrdersAbsorb;
	//	paramsPtr->RTS_SizeAbsorbCount = tradePtr->matchSize;
	//	backtestStatistics.insert(std::make_pair(tradePtr->executedPriceLevelsCount, paramsPtr));
	//	lastDealID = tradePtr->ordID;   // помечаем что данный трейд уже обработан
	//	startMXIPrice = 0; startMIXPrice = 0;
	//}

	//if (lastAbsorbTradeTime + startDeltaTime < info.moment_ns && info.moment_ns < lastAbsorbTradeTime + endDeltaTime)
	//{
	//	Price bestBidPriceMIX, bestAskPriceMIX;
	//	sessionData.GetBestPrices(MIX_isin_id, bestBidPriceMIX, bestAskPriceMIX);
	//	Price bestBidPriceMXI, bestAskPriceMXI;
	//	sessionData.GetBestPrices(MXI_isin_id, bestBidPriceMXI, bestAskPriceMXI);
	//	double MIX_min_step = sessionData.GetMinStep(MIX_isin_id);
	//	double MXI_min_step = sessionData.GetMinStep(MXI_isin_id);

	//	if (bestBidPriceMIX.IsPrice() && bestAskPriceMIX.IsPrice() && bestBidPriceMXI.IsPrice() && bestAskPriceMXI.IsPrice())
	//	{
	//		if (startMXIPrice == 0 && startMIXPrice == 0)
	//		{
	//			startMIXPrice = agressorSide == 'B' ? bestAskPriceMIX.ToDouble() : bestBidPriceMIX.ToDouble();
	//			startMXIPrice = agressorSide == 'B' ? bestAskPriceMXI.ToDouble() : bestBidPriceMXI.ToDouble();
	//		}

	//		if (agressorSide == 'B')
	//		{
	//			int MIX_delta = int((bestAskPriceMIX.ToDouble() - startMIXPrice) / MIX_min_step);
	//			if (MIX_delta > lastArbParams->MIX_PriceLevelsCount)
	//			{
	//				lastArbParams->MIX_PriceLevelsCount = MIX_delta;
	//			}
	//			int MXI_delta = int((bestAskPriceMXI.ToDouble() - startMXIPrice) / MXI_min_step);
	//			if (MXI_delta > lastArbParams->MXI_PriceLevelsCount)
	//			{
	//				lastArbParams->MXI_PriceLevelsCount = MXI_delta;
	//			}
	//		}
	//		else // agressorSide == 'S'
	//		{
	//			int MIX_delta = int((startMIXPrice - bestBidPriceMIX.ToDouble()) / MIX_min_step);
	//			if (MIX_delta > lastArbParams->MIX_PriceLevelsCount)
	//			{
	//				lastArbParams->MIX_PriceLevelsCount = MIX_delta;
	//			}
	//			int MXI_delta = int((startMXIPrice - bestBidPriceMXI.ToDouble()) / MXI_min_step);
	//			if (MXI_delta > lastArbParams->MXI_PriceLevelsCount)
	//			{
	//				lastArbParams->MXI_PriceLevelsCount = MXI_delta;
	//			}
	//		}	
	//	}
	//}

	//return;

//**********************************************************************************

	// ЛОГИКА ВЫХОДА ИЗ ПОЗИЦИИ
	if (waitActionResult)	// ждём пока придёт ответ из ядра о результатах посланного приказа
	{
		return;
	}

	const int isin_id = MIX_isin_id;	// исин того инструмента, по которому торгуем
	StrategyParams& sp = params.at(isin_id);

	const PositionEx& pos = positions[isin_id];
	const int posSize = pos.GetPos(); // направление позы это знак этой переменной
	if (posSize != 0)
	{
		// если поза открыта и ордер на закрытие ещё не послан, то посылаем его или если ордер был послан, но был реджект, то посылаем ещё раз ордер
		if (!sp.closePosOrder || sp.closePosOrder->status == SOrderEx::REJECTED_ADD)
		{
			const t_id order_id = GetNextOrderID();
			sp.closePosOrder = &CreateOrder(isin_id,
				posSize > 0 ? pos.WAPrice + sp.minStep : pos.WAPrice - sp.minStep,
				sp.maxPositionSize,
				order_id,
				SOrderEx::FAS,
				posSize > 0 ? ASK : BID,
				lastEventTime + timeDelay);
			AddOrder(isin_id, order_id);
		}

		// если ордер стоит в стакане и прошло уже много времени, то отменяем ордер и выходим по рынку
		if (sp.closePosOrder->status == SOrderEx::SUBMITTED && (lastEventTime - sp.closePosOrder->responseTime) > sp.timeToExitAfterEnter)
		{
			CancelOrder(isin_id, sp.closePosOrder->order_id);
		}

		// если ордер отменён в соответствии с предыдущим if, то надо выходить из позы по рынку
		if (sp.closePosOrder->status == SOrderEx::CANCELED)
		{
			const TradeSessionData::PriceLimits priceLimits = sessionData.GetPriceLimits(isin_id);
			const Price upLimit = priceLimits.first, downLimit = priceLimits.second;
			const t_id order_id = GetNextOrderID();
			sp.closePosOrder = &CreateOrder(isin_id,
				posSize > 0 ? downLimit.ToDouble() : upLimit.ToDouble(),
				sp.maxPositionSize,
				order_id,
				SOrderEx::FAS,
				posSize > 0 ? ASK : BID,
				lastEventTime + timeDelay);
			AddOrder(isin_id, order_id);
		}
		
		return;	// пока сделаем чтобы только одину позицию можно было держать открытой
	}


	// ЛОГИКА ВХОДА В ПОЗИЦИЮ
	if (info.isin_id != RTS_isin_id) // Ri
	{
		return;
	}

	const bool isTradeReady = sessionData.isLastAggrTradeReady(info.isin_id);
	if (!isTradeReady)
	{
		return;
	}

	const AggregatedTrade* trade = sessionData.GetLastTrade(info.isin_id);
	if (trade && sp.lastDealID == trade->ordID)
	{
		return;
	}
	sp.lastDealID = trade->ordID;   // помечаем что данный трейд уже обработан логикой стратегии

	if (trade->executedPriceLevelsCount < sp.priceLevelsAbsorb)
	{
		return;
	}

	BACKTEST_ORDLOG_LOG("%s", "\nFind big trade")
	std::string dump = sessionData.DumpBookToString(isin_id, 3);
	BACKTEST_ORDLOG_LOG("%s", dump.c_str())

	const TradeSessionData::PriceLimits priceLimits = sessionData.GetPriceLimits(isin_id);
	const Price upLimit = priceLimits.first, downLimit = priceLimits.second;
	const t_id order_id = GetNextOrderID();
	/*sp.closePosOrder = &*/CreateOrder(isin_id,
		trade->agressorSide == 'B' ? upLimit.ToDouble() : downLimit.ToDouble(),
		sp.maxPositionSize,
		order_id,
		SOrderEx::FAS,
		trade->agressorSide == 'B' ? BID : ASK,
		lastEventTime + timeDelay);
	AddOrder(isin_id, order_id);
	
	sp.closePosOrder = NULL;
	
	
// ********************************************************** корреляция между Si, Ri, MIX	
	
	//StrategyParams& sp = params.at(info.isin_id);

	//Price bestBidPrice, bestAskPrice;
	//sessionData.GetBestPrices(info.isin_id, bestBidPrice, bestAskPrice);

	//if (!bestBidPrice.IsPrice() || !bestAskPrice.IsPrice() || (bestBidPrice == sp.prevBidPrice && bestAskPrice == sp.prevAskPrice))
	//{
	//	return;
	//}
	//else
	//{
	//	sp.prevBidPrice = bestBidPrice;
	//	sp.prevAskPrice = bestAskPrice;
	//}

	//const double avgPrice = (bestAskPrice.ToDouble() + bestBidPrice.ToDouble()) / 2;
	//sp.avgPrice = avgPrice;
	//sp.vola.AddValue(avgPrice / sp.minStep, info.moment_ns);
	////sp.priceRange.AddValue(avgPrice / sp.minStep, info.moment_ns);
	////sp.timeVola.AddValue(std::abs(sp.priceRange.GetDelta()), info.moment_ns);

	//static ExpMovingAvarage<double> ema(1000);
	//static Sigma<double> sigma(1000);
	//static Correlation<double> corr(300);

	///*const double logPrice1 = params.at(RTS_isin_id).priceRange.GetLogRatio();
	//const double logPrice2 = params.at(Si_isin_id).priceRange.GetLogRatio();
	//const double logPrice3 = params.at(MIX_isin_id).priceRange.GetLogRatio();
	//if (logPrice1 == 0 || logPrice2 == 0 || logPrice3 == 0)
	//{
	//	return;
	//}*/

	//const double netPrice1 = params.at(RTS_isin_id).avgPrice / params.at(RTS_isin_id).minStep;
	//const double netPrice2 = params.at(Si_isin_id).avgPrice / params.at(Si_isin_id).minStep;
	//const double netPrice3 = params.at(MIX_isin_id).avgPrice / params.at(MIX_isin_id).minStep;
	//if (netPrice1 == 0 || /*netPrice2 == 0 ||*/ netPrice3 == 0)
	//{
	//	return;
	//}


	//if (!params.at(RTS_isin_id).vola.isTimeRangeFull() || !params.at(Si_isin_id).vola.isTimeRangeFull() || !params.at(MIX_isin_id).vola.isTimeRangeFull())
	//{
	//	return;
	//}
	//const double vola1 = params.at(RTS_isin_id).vola.GetVolat();
	//const double vola2 = params.at(Si_isin_id).vola.GetVolat();
	//const double vola3 = params.at(MIX_isin_id).vola.GetVolat();

	//double deltaPrice = netPrice1 - netPrice3;

	//// приведение к одинаковой волатильности
	///*if (vola1 > vola2)
	//{
	//	const double volatilityCoeff = (vola1 / vola2);
	//	deltaPrice = std::abs(netPrice1*volatilityCoeff - netPrice2);
	//}
	//else
	//{
	//	const double volatilityCoeff = (vola2 / vola1);
	//	deltaPrice = std::abs(netPrice2*volatilityCoeff - netPrice1);
	//}*/

	//// плотность вероятности для временного рэнджа 100 мс
	//static TimeRangeDeltaValues<double> priceDelta100(1000000ll * 100);
	//priceDelta100.AddValue(deltaPrice, info.moment_ns);
	////probabilityDensity100[int(priceDelta100.GetDelta())] += 1;

	//ema.AddValue(deltaPrice);
	//sigma.AddValue(deltaPrice);
	//corr.AddValues(netPrice1, netPrice3);
	///*if (!sigma.isMaxElementsCount())
	//{
	//	return;
	//}*/
	///*if (!corr.isFullRange())
	//{
	//	return;
	//}*/

	//const double emaValue = ema.GetAvg();
	//const double sigmaVal = sigma.Calc();
	////if (std::isnan(sigmaVal))
	////{
	////	sigmaVal = 0;
	////}
	//const double upSigma = emaValue + (sigmaVal * 2);
	//const double downSigma = emaValue - (sigmaVal * 2);
	//const double corrValue = corr.Calc();

	////const double deltaPrice1 = std::abs(params.at(RTS_isin_id).priceRange.GetDelta());
	////const double deltaPrice2 = std::abs(params.at(Si_isin_id).priceRange.GetDelta());
	////const double deltaPrice3 = std::abs(params.at(MIX_isin_id).priceRange.GetDelta());

	//const double avgPrice1 = params.at(RTS_isin_id).avgPrice;
	//const double avgPrice2 = params.at(Si_isin_id).avgPrice;
	//const double avgPrice3 = params.at(MIX_isin_id).avgPrice;

	//char buf[1000] = { 0 };
	//sprintf_s(buf, "%f;%f;%f;%f;%f;%f;%f;%f;%d;%f;%f;%f;", 
	//			  avgPrice1, avgPrice2, avgPrice3, 
	//		      deltaPrice, emaValue, upSigma, downSigma, corrValue, 
	//			  int(priceDelta100.GetDelta()), 
	//			  vola1, vola2, vola3);
	//char* pch;
	//while ((pch = strchr(buf, '.')) != NULL)
	//{
	//	*pch = ',';
	//}
	//std::string stringTime;
	//fo2base::CP2Time::FromUnixNanosecondsToString3(info.moment_ns, stringTime);
	//
	//char buf1[1000] = { 0 };
	//sprintf_s(buf1, "%s;%s;", stringTime.c_str(), buf);
	//StrategyResultToFile(buf1);

//******************************************************************************************

	//const AggregatedTrade* trade = sessionData.GetLastTrade(isin_id);
	//const bool isTradeReady = sessionData.isLastAggrTradeReady(isin_id);

	//if (trade == NULL || !isTradeReady /*|| trade->IODir == UNDEFINED*/)
	//{
	//	return;
	//}

	//const t_id trade_id = trade->ordID;/*trade->components.back().tradeID;*/
	//if (params.lastDealID == trade_id)
	//{
	//	return;
	//}
	//params.lastDealID = trade_id;   // помечаем что данный трейд уже обработан логикой стратегии

	//static int calculationFrequency = 0;	// делаем перерасчёт значений раз в 10 трейдов для ускорения алгоритма
	//++calculationFrequency;
	//if (params.ordersChannel.GetCount() < params.ordersChannel.GetRangeCount() ||
	//	calculationFrequency == 10)
	//{
	//	calculationFrequency = 0;
	//}

	//params.vpin1.AddValue(trade->agressorSide == 'B' ? (int)trade->oppositeOrdersAbsorb : -(int)trade->oppositeOrdersAbsorb, trade->time);
	//params.spreadIndicator.AddValue(trade->agressorSide == 'B' ? (int)trade->oppositeOrdersAbsorb : -(int)trade->oppositeOrdersAbsorb, trade->time);
	
	//if (!params.vpin1.isMaxElementsCount())	// пока не наполнится нужное количество данных для расчёта не торгуем
	//{
	//	return;
	//}
	//params.ordersChannel.AddValue(int(trade->totalBidOrdersCount - trade->totalAskOrdersCount));
	//if (!params.ordersChannel.isMaxElementsCount())	// пока не наполнится нужное количество данных для расчёта не торгуем
	//{
	//	return;
	//}

	
	//params.windowPriceSpread.AddValue(trade->endPrice.ToDouble());

	//const int avgValue = params.ordersChannel.CalcAverage();
	//const int stdDev = params.ordersChannel.CalcStdDev();
	//const double channelTop = (double)avgValue + 2 * stdDev;
	//const double channelBottom = (double)avgValue - 2 * stdDev;

	//const double vpin = params.vpin1.CalcVPIN();
	//params.vpinMoving1.AddValue(vpin);
	//params.vpinMoving2.AddValue(vpin);

	//const double spreadEventIndicator = params.spreadIndicator.Calc();
	//params.spreadMoving.AddValue(spreadEventIndicator);

	//params.correlation.AddValues(vpin, spreadEventIndicator);

	//double indicator;
	//const AggregatedTrade* _trade = NULL;
	//if (isin_id == RTS_isin_id)	// RTS
	//{
	//	_trade = sessionData.GetLastTrade(Si_isin_id);
	//}
	//else if (isin_id == Si_isin_id)	// Si
	//{
	//	_trade = sessionData.GetLastTrade(RTS_isin_id);
	//}
	//indicator = _trade ? _trade->startPrice.ToDouble() : 0.0;

	//const double shortVPIN_EMA = params.vpinMoving1.CalcAverage();
	//SetIndicator1(isin_id, lastEventTime, trade->startPrice.ToDouble());
	//const double longVPIN_EMA = params.vpinMoving2.CalcAverage();
	//const double spreadEma = params.spreadMoving.CalcAverage();
	//const double correlationCoeff = params.correlation.Calc();
	//SetIndicator2(isin_id, lastEventTime, indicator);
	//params.timeChannel.AddValue(trade->time);
	//params.spredChangesChannel.AddValue(trade->spreadChangesCount);
	//indicator = params.timeChannel.GetDelta();
	//indicator = params.windowPriceSpread.GetDelta() * vpin;
	//indicator = params.spredChangesChannel.GetDelta();
	//params.vpin1.AddValue(trade->agressorSide == 'B' ? (int)trade->oppositeOrdersAbsorb : -(int)trade->oppositeOrdersAbsorb, trade->time);
	//params.vpin2.AddValue(trade->agressorSide == 'B' ? (int)trade->oppositeOrdersAbsorb : -(int)trade->oppositeOrdersAbsorb);
	//params.vpin1.AddValue(trade->agressorSide == 'B' ? trade->matchSize : -trade->matchSize);
	//params.vpin2.AddValue(trade->agressorSide == 'B' ? trade->matchSize : -trade->matchSize);
	//params.vpin1.AddValue(trade->agressorSide == 'B' ? (int)trade->oppositeOrdersAbsorb : -(int)trade->oppositeOrdersAbsorb, trade->IODir);
	//params.vpin2.AddValue(trade->agressorSide == 'B' ? (int)trade->oppositeOrdersAbsorb : -(int)trade->oppositeOrdersAbsorb, trade->IODir);
	//indicator = params.vpin1.CalcVPIN();
	
	//indicator = params.vpin2.CalcVPIN();
	//SetIndicator2(isin_id, lastEventTime, indicator);

	// проверяем в какой зоне мы сейчас
	//const int ordersDeltaCount = int(trade->totalBidOrdersCount - trade->totalAskOrdersCount);
	/*StrategyParams::Zone currentZone = MyFirstStrategy::StrategyParams::BETWEEN;
	if (longVPIN_EMA > shortVPIN_EMA)
	{
		currentZone = MyFirstStrategy::StrategyParams::UNDER;
	}
	else if (longVPIN_EMA < shortVPIN_EMA)
	{
		currentZone = MyFirstStrategy::StrategyParams::ABOVE;
	}*/

	/*if (ordersDeltaCount <= channelTop && ordersDeltaCount >= channelBottom)
	{
		currentZone = MyFirstStrategy::StrategyParams::BETWEEN;
	}
	else if (ordersDeltaCount > channelTop)
	{
		currentZone = MyFirstStrategy::StrategyParams::ABOVE;
	}
	else if (ordersDeltaCount < channelBottom)
	{
		currentZone = MyFirstStrategy::StrategyParams::UNDER;
	}*/

	// если в этой зоне уже был трейд, то выходим
	/*if (currentZone == params.zone && params.zoneTradeFlag)
	{
		return;
	}
	else if (currentZone != params.zone)
	{
		params.zoneTradeFlag = false;
		params.zone = currentZone;
	}*/

	//const TradeSessionData::PriceLimits priceLimits = sessionData.GetPriceLimits(isin_id);
	//const Price upLimit = priceLimits.first;
	//const Price downLimit = priceLimits.second;

	//// проверяем условия входа/выхода в позицию
	//const int posSize = positions.at(isin_id).GetPos();
	//if (posSize == 0 /*&& !params.zoneTradeFlag && params.zone != MyFirstStrategy::StrategyParams::BETWEEN*/)
	//{
	//	Price price;
	//	P2Dir dir;
	//	if (longVPIN_EMA > 0.1 && params.zone == MyFirstStrategy::StrategyParams::UNDER && currentZone == MyFirstStrategy::StrategyParams::ABOVE)
	//	{
	//		price = upLimit;
	//		dir = BID;
	//	}
	//	else if (longVPIN_EMA < -0.1 && params.zone == MyFirstStrategy::StrategyParams::ABOVE && currentZone == MyFirstStrategy::StrategyParams::UNDER)
	//	{
	//		price = downLimit;
	//		dir = ASK;
	//	}
	//	else
	//	{
	//		params.zone = currentZone;
	//		return;
	//	}

	//	//params.zoneTradeFlag = true;
	//	const t_id order_id = GetNextOrderID();
	//	/*SOrderEx& order = */CreateOrder(isin_id,
	//		price,
	//		params.maxPositionSize,
	//		order_id,
	//		SOrderEx::FAS,
	//		dir,
	//		lastEventTime + timeDelay);

	//	AddOrder(isin_id, order_id);
	//}
	//else if (posSize > 0 && 
	//		 (longVPIN_EMA < 0 || (params.zone == MyFirstStrategy::StrategyParams::ABOVE && currentZone == MyFirstStrategy::StrategyParams::UNDER)) /* && params.zone == MyFirstStrategy::StrategyParams::UNDER && !params.zoneTradeFlag*/)
	//{
	//	//params.zoneTradeFlag = true;
	//	const t_id order_id = GetNextOrderID();
	//	/*SOrderEx& order = */CreateOrder(isin_id,
	//									  downLimit,
	//									  std::abs(posSize)/* + params.maxPositionSize*/,
	//									  order_id,
	//									  SOrderEx::FAS,
	//									  ASK,
	//									  lastEventTime + timeDelay);

	//	AddOrder(isin_id, order_id);
	//	return;
	//}
	//else if (posSize < 0 &&
	//	     (longVPIN_EMA > 0 || (params.zone == MyFirstStrategy::StrategyParams::UNDER && currentZone == MyFirstStrategy::StrategyParams::ABOVE)) /* && params.zone == MyFirstStrategy::StrategyParams::ABOVE && !params.zoneTradeFlag*/)
	//{
	//	//params.zoneTradeFlag = true;
	//	const t_id order_id = GetNextOrderID();
	//	/*SOrderEx& order = */CreateOrder(isin_id,
	//									  upLimit,
	//									  std::abs(posSize)/* + params.maxPositionSize*/,
	//									  order_id,
	//									  SOrderEx::FAS,
	//									  BID,
	//									  lastEventTime + timeDelay);

	//	AddOrder(isin_id, order_id);
	//	return;
	//}

	//if (params.zone != currentZone)
	//{
	//	params.zone = currentZone;	
	//}

	// проверяем ограничение на количество открытых позиций если пришёл сигнал на наращивание позиции
	/*if ( (trade->agressorSide == 'B' && posSize > 0 && posSize >= params.maxPositionSize) ||
		 (trade->agressorSide == 'S' && posSize < 0 && std::abs(posSize) >= params.maxPositionSize) )
	{
		return;
	}*/
	

 //   const AggregatedTrade* trade = sessionData.GetLastTrade(isin_id);
 //   if (trade == NULL || 
	//	/*trade->IODir == UNDEFINED || 
	//	trade->IODir == UNKNOWN_POSITION ||
	//	trade->IODir == CLOSE_POSITION ||*/
	//	trade->IODir != OPEN_POSITION /*||
	//	trade->matchSize < params.bigTradeSize*/)
 //   {
 //       return;
 //   }

 //   const t_id trade_id = trade->components.back().tradeID;
 //   if (params.lastDealID == trade_id)
 //   {
 //       return;
 //   }

	//std::deque<AggregatedTrade>* window = NULL;
	//if (trade->agressorSide == 'B')
	//{
	//	window = &GetLongWindow(isin_id);
	//}
	//else
	//{
	//	window = &GetShortWindow(isin_id);
	//}
	//
	//window->push_back(*trade);
	//CheckWindowContent(isin_id, *window);
	//
	//Position pos;
	//for (const AggregatedTrade pTrade : *window)
	//{
	//	pos.AddDeal(true, static_cast<int>(pTrade.matchSize), pTrade.startPrice.ToDouble());
	//}

	//double windowWAPrice = window->empty() ? 0.0 : pos.GetWAPrice();

	//if (trade->agressorSide == 'B')
	//{
	//	indicator1Map[isin_id][lastEventTime] = windowWAPrice;
	//}
	//else
	//{
	//	indicator2Map[isin_id][lastEventTime] = windowWAPrice;
	//}

    // эти функции можно использовать в торговой логике
    //AddOrder(const InstrumentID isin_id, const t_id id2);
    //CancelOrder(const InstrumentID isin_id, const t_id id2);
    //MassCancelOrder(const InstrumentID isin_id, const std::set<t_id>& ids);
    //MoveOrders(const InstrumentID isin_id, const std::set<t_id>& ids);
}

void MyFirstStrategy::PostDEALSLogic(const InstrumentID isin_id, StrategyParams& params, const t_id deal_id)
{
    
}

void MyFirstStrategy::EndSessionAction(const std::string& date)
{
#define BACKTEST_STATISTICS_LOG(message, ...) PRINT_TO_LOG((*backtestStatisticsLogger.get()), LOG_TRACE, message, __VA_ARGS__)

	if (backtestStatistics.empty())
	{
		return;
	}

	std::string fileName = "strategy_statistics(" + date + ").csv";
	std::unique_ptr<FileLogger> backtestStatisticsLogger;
	backtestStatisticsLogger.reset(new FileLogger(GetPathToLog(), fileName));


	char str1[100] = { 0 };
	char str2[100] = { 0 };
	char str3[100] = { 0 };
	char str4[100] = { 0 };
	char str5[100] = { 0 };
	int pos1 = 0, pos2 = 0, pos3 = 0, pos4 = 0, pos5 = 0;
	for (auto& it = backtestStatistics.begin(), end = backtestStatistics.end(); it != end; ++it)
	{
		pos1 += sprintf_s(str1 + pos1, sizeof(str1) - pos1, "%d;", it->first);
		pos2 += sprintf_s(str2 + pos2, sizeof(str2) - pos2, "%d;", it->second->MIX_PriceLevelsCount);
		pos3 += sprintf_s(str3 + pos3, sizeof(str3) - pos3, "%d;", it->second->MXI_PriceLevelsCount);
		pos4 += sprintf_s(str4 + pos4, sizeof(str4) - pos4, "%d;", it->second->RTS_OrdersAbsorbCount);
		pos5 += sprintf_s(str5 + pos5, sizeof(str5) - pos5, "%d;", it->second->RTS_SizeAbsorbCount);
	}

	backtestStatistics.clear();

	BACKTEST_STATISTICS_LOG("RTS_PriceLevelsCount;%s\n"
							"MIX_PriceLevelsCount;%s\n"
							"MXI_PriceLevelsCount;%s\n"
							"RTS_OrdersAbsorbCount;%s\n"
							"RTS_SizeAbsorbCount;%s\n\n", str1, str2, str3, str4, str5);
}

void MyFirstStrategy::EndSessionRangeAction()
{

}

std::string MyFirstStrategy::GetStrategyResult() const
{
    std::string str;
    for (const std::pair<InstrumentID, StrategyParams>& pair : params)
    {
        const InstrumentID isin_id = pair.first;
        if (positions.find(isin_id) != positions.end())
        {
            const PositionEx& pos = positions.at(isin_id);
            str += "Isin: " + sessionData.GetIsinFromId(isin_id) + '\n' + pos.GetPositionInfo() + pos.GetPositionExInfo() + '\n';
        }
    }
    return str;
}

void StrategyLogger::StrategyOrdlogToLog(const fo2base::UnixNanosecondsTime time, const InstrumentID isin_id, const std::string& action, const SOrderEx& order)
{
	if (!strategyResultLogger)
	{
		return;
	}

	std::string stringTime;
	fo2base::CP2Time::FromUnixNanosecondsToString3(time, stringTime);
	BACKTEST_ORDLOG_LOG("%s; isin_id=%d; action=%s;			dir=%s;initPrice=%f;realprice=%f;initqty=%lld;remqty=%lld;type=%s;id=%lld;",
		stringTime.c_str(),
		isin_id,
		action.c_str(),
		order.dir == ASK ? "ASK" : "BID",
		order.initialPrice,
		order.realPrice,
		order.initialQuantity,
		order.remainedQuantity,
		order.type == SOrderEx::FAS ? "FAS" :
		order.type == SOrderEx::IOC ? "IOC" : "FOK",
		order.order_id)
}

void StrategyLogger::StrategyTradesToLog(const fo2base::UnixNanosecondsTime time, 
	const InstrumentID isin_id, const double price, const uint64_t quantity, const bool is_long, const int posSize, const int pnl)
{
	if (!strategyTradesLogger)
	{
		return;
	}

	std::string stringTime;
	fo2base::CP2Time::FromUnixNanosecondsToString3(time, stringTime);
	BACKTEST_TRADES_LOG("%s; isin_id=%d; price=%f; qty=%lld; dir=%s; pos=%d; pnl=%d;", stringTime.c_str(), isin_id, price, quantity, is_long ? "Long" : "Short", posSize, pnl)
}
