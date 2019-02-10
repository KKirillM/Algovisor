#include "stdafx.h"

#include "Auxiliary_methods.h"
#include "DEFINE.h"
#include "CQGDialog.h"
#include "NinjaDialog.h"

#include "Instrument.h"

//дополнительные функции
//без подчеркивания - это более поздний таймстемп
UINT MicroSpeed(BYTE hour, BYTE minute, BYTE second, UINT microsecond,  BYTE _hour, BYTE _minute, BYTE _second, UINT _microsecond)
{
	//когда микросекунды совпадают
	if (microsecond == _microsecond)
		return 0;
	//внутри одной минуты и секунды
	else if (minute == _minute && second == _second)
		return (microsecond - _microsecond);
	//внутри одной минуты и смежных секунд
	else if (minute == _minute && (second - _second) == 1)
		return (999999 - _microsecond + microsecond);
	//в смежные минуты смежных секунд
	else if ((minute - _minute) == 1 && _second == 59 && second == 0)
		return (999999 - _microsecond + microsecond);
	//все остальное больше секунды
	else 
		return 1000000;
}

void ZeroData(RITHMICTRADE& _d)
{
	_d.microDelta = 0;
	_d.Data.tradeSize = 0;
	_d.agressorSide = 'N';
	_d.buyVolume = 0;
	_d.sellVolume = 0;
	_d.condition = 0;
	_d.yAxis = false;
}
//void ZeroData(RITHMICBBO& _d)
//{
//	_d.askOrders = 0;
//	_d.askSize = 0;
//	_d.bidOrders = 0;
//	_d.bidSize = 0;
//	_d.deltaAskLimitSize = 0;
//	_d.deltaBidLimitSize = 0;
//	_d.Data.microDelta = 0;
//	_d.Data.Data.tradeSize = 0;
//	_d.Data.agressorSide = 'N';
//	_d.Data.buyVolume = 0;
//	_d.Data.sellVolume = 0;
//	_d.Data.condition = 0;
//	_d.Data.yAxis = 0;
//	_d.tradeSizeQuant = 0;
//}
void ZeroData(RITHMICDOM& _d, int domLevelsCount)
{
	_d.Data.microDelta = 0;
	_d.Data.Data.tradeSize = 0;
	_d.Data.agressorSide = '?';
	_d.updateType = 0;
	_d.orderType = 0;
	_d.Data.buyVolume = 0;
	_d.Data.sellVolume = 0;
	_d.Data.condition = 0;
	_d.tradeSizeQuant = 0;
	_d.Data.yAxis = false;
	_d.eventAskLevel = 0;
	_d.eventBidLevel = 0;
	_d.maxDomLimit = 0;
	_d.minDomLimit = UINT_MAX;
	_d.maxDomOrder = 0;
	_d.minDomOrder = UINT_MAX;

	for (int i = 0; i <= domLevelsCount; ++i) 
	{
		_d.askSize[i] = _d.bidSize[i] = _d.bidOrders[i] = _d.askOrders[i] = _d.deltaBidLimitSize[i] = _d.deltaAskLimitSize[i] = 0;
	}
}
void ZeroData(CQGTRADE& _d)
{
	_d.microDelta = 0;
	_d.Data.tradeSize = 0;
	_d.agressorSide = 'N';	// types of agressor side: B - bid trade, A - ask trade, ? - implied trade, N - not defined, a - ask event, b - bid event
	_d.yAxis = false;
}
void ZeroData(CQGDOM& _d, int domLevelsCount)
{
	_d.Data.microDelta = 0;
	_d.Data.Data.tradeSize = 0;
	_d.Data.agressorSide = 'N';	// types of agressor side: B - bid trade, A - ask trade, ? - implied trade, N - not defined, a - ask event, b - bid event
	_d.tradeSizeQuant = 0;
	_d.Data.yAxis = false;
	_d.eventAskLevel = 0;
	_d.eventBidLevel = 0;
	_d.maxDomLimit = 0;
	_d.minDomLimit = UINT_MAX;
	_d.orderType = 0;

	for (int i = 0; i < domLevelsCount; ++i) 
	{
		_d.askSize[i] = _d.bidSize[i] = _d.deltaBidLimitSize[i] = _d.deltaAskLimitSize[i] = 0;
	}
}
//void ZeroLimitsBBO(RITHMICBBO& _d)
//{
//	_d.askOrders = 0;
//	_d.askSize = 0;
//	_d.bidOrders = 0;
//	_d.bidSize = 0;
//	_d.deltaAskLimitSize = 0;
//	_d.deltaBidLimitSize = 0;
//	_d.tradeSizeQuant = _d.Data.Data.tradeSize;
//}
void ZeroLimitDOM(RITHMICDOM& _d, int domLevelsCount)
{
	_d.tradeSizeQuant = _d.Data.Data.tradeSize;
	for (int i = 0; i <= domLevelsCount; ++i) 
	{
		_d.askSize[i] = _d.bidSize[i] = _d.bidOrders[i] = _d.askOrders[i] = /*_d.updateLevel[i] =*/ 0;
	}
}

std::vector<RTicker*> rithmic_data;
std::vector<CQGTicker*> cqg_data;
std::vector<CQGTicker*> ninja_data;
extern SRWLOCK SRWLockRithmic;

//Price Ticker::PriceMinusPrice(const Price& price1, const Price& price2)
//{
//	Price result;
//	int divider=0;
//	int frac1, frac2, frac;
//
//	result.SetInt(price1.GetInt() - price2.GetInt());
//		
//	if (price1.GetInt() >= 0)
//		frac1 = price1.GetFrac();
//	else frac1 = -(int)price1.GetFrac();
//	if (price2.GetInt() >= 0)
//		frac2 = price2.GetFrac();
//	else frac2 = -(int)price2.GetFrac();
//
//	divider = (int)std::pow(10.0, precision);
//
//	frac = frac1 - frac2;
//
//	if (std::abs(frac) > divider) {
//		--result;
//		result.SetFrac(std::abs(frac) - divider);
//	}
//	else result.SetFrac(std::abs(frac));
//
//	return result;
//}
UINT IInstrument::PriceDifference(const Price& price1, const Price& price2) const
{
	UINT result = 0;
	int divider=0;
	double x, y;

	//if (price1 == price2)
	//	return 0;

	if (price1.IsPrice() && price2.IsPrice())
	{
		divider = (int)std::pow(10.0, precision);

		x = divider * price1.GetInt() + price1.GetFrac();
		y = divider * price2.GetInt() + price2.GetFrac();

		if (price1.GetZnak())
			x = -x;
		if (price2.GetZnak())
			y = -y;

		result = sqrt(pow(x - y, 2)) / MinTickInc;
	}
	
	return result;
}
void IInstrument::IncPrice(Price& price)
{
	if (precision)
	{
		int divider = (int)std::pow(10.0, precision);

		if (price.IsZero())
		{
			price.SetZnak(false);
			price.SetFrac(MinTickInc);
		}
		else if (!price.GetZnak())
		{
			if (price.GetFrac() + MinTickInc == divider)
			{
				price.IncInt();
				price.SetFrac(0);
			}
			else price.SetFrac(price.GetFrac()+MinTickInc);
		}
		else
		{
			if (!price.GetFrac())
			{
				price.DecInt();
				price.SetFrac(divider-MinTickInc);
			}
			else price.SetFrac(price.GetFrac()-MinTickInc);
		}
	}
	else
	{
		if (price.IsZero())
		{
			price.SetZnak(false);
			price.SetInt(MinTickInc);
		}
		else if (!price.GetZnak())
			price.SetInt(price.GetInt()+MinTickInc);
		else price.SetInt(price.GetInt()-MinTickInc);
	}
}
void IInstrument::DecPrice(Price& price)
{
	if (precision)
	{
		int divider = (int)std::pow(10.0, precision);

		if (price.IsZero())
		{
			price.SetZnak(true);
			price.SetFrac(MinTickInc);
		}
		else if (!price.GetZnak())									//если цена положительная
		{
			if (!price.GetFrac())
			{
				price.DecInt();
				price.SetFrac(divider-MinTickInc);
			}
			else 
				price.SetFrac(price.GetFrac()-MinTickInc);
		}
		else														//если цена отрицательная
		{
			if (price.GetFrac() + MinTickInc == divider)
			{
				price.IncInt();
				price.SetFrac(0);
			}
			else price.SetFrac(price.GetFrac()+MinTickInc);
		}
	}
	else
	{
		if (price.IsZero())
		{
			price.SetZnak(true);
			price.SetInt(MinTickInc);
		}
		else if (!price.GetZnak())
			price.SetInt(price.GetInt()-MinTickInc);
		else price.SetInt(price.GetInt()+MinTickInc);
	}
}

void RTradeTicker::TradePrint(void* p)
{
	RApi::TradeInfo* pInfo = (RApi::TradeInfo*)p;

	RITHMICTRADE _d;
	ZeroData(_d);

	//таймстемп
	CTime time(pInfo->iSsboe);
	_d.Data.hour = (BYTE)time.GetHour();
	_d.Data.minute = (BYTE)time.GetMinute();
	_d.Data.second = (BYTE)time.GetSecond();

	//микросекунды
	_d.microseconds = pInfo->iUsecs;

	if (counter >= 100 && (hour != _d.Data.hour || minute != _d.Data.minute || second != _d.Data.second)) 
	{		
		hour = _d.Data.hour;
		minute = _d.Data.minute;
		second = _d.Data.second;
		
		_d.yAxis = true;
		counter = 0;
	}
	++counter;

	//таймстемп
	//CTime s_time(pInfo->iSourceSsboe);
	//time.GetAsSystemTime(_d.Data.s_timeStamp);

	//микросекунды
	//_d.s_microseconds = pInfo->iSourceUsecs;

	//дельта между текущим таймстемпом и предыдущим
	_d.microDelta = 0;
	if (pInfo->iType != RApi::MD_HISTORY_CB && Data.size())
		_d.microDelta = MicroSpeed(_d.Data.hour, _d.Data.minute, _d.Data.second, _d.microseconds, 
							Data[Data.size()-1].Data.hour, Data[Data.size()-1].Data.minute, Data[Data.size()-1].Data.second, Data[Data.size()-1].microseconds);
	else if (HData.size())
		HData[HData.size()-1].microDelta = MicroSpeed(HData[HData.size()-1].Data.hour, HData[HData.size()-1].Data.minute, HData[HData.size()-1].Data.second, HData[HData.size()-1].microseconds,
													_d.Data.hour, _d.Data.minute, _d.Data.second, _d.microseconds);
	
	//цена и объем
	_d.Data.tradePrice.SetPrice(pInfo->dPrice, precision);
	_d.Data.tradeSize = pInfo->iSize;

	//сторона агрессора
	if (pInfo->sAggressorSide.iDataLen)
		_d.agressorSide = pInfo->sAggressorSide.pData[0];
	else _d.agressorSide = '?';
	_d.impliedSide = 'N';

	//кондишен
	if (pInfo->sCondition.iDataLen)
		//_d.condition.assign(pInfo->sCondition.pData, pInfo->sCondition.iDataLen);
		_d.condition = 1;

	//дельты
	if (pInfo->bVolumeBoughtFlag)
		_d.buyVolume = pInfo->iVolumeBought;
	if (pInfo->bVolumeSoldFlag)
		_d.sellVolume = pInfo->iVolumeSold;

	if (pInfo->iType == RApi::MD_HISTORY_CB)
		push_back_history(_d);
	else
		TickerData<RITHMICTRADE>::push_back(_d);
}
//void RBBOTicker::TradePrint(void* p)
//{
//	RApi::TradeInfo* pInfo = (RApi::TradeInfo*)p;
//
//	RITHMICBBO _d;
//	ZeroData(_d);
//
//	//таймстемп
//	CTime time(pInfo->iSsboe);
//	_d.Data.Data.hour = (BYTE)time.GetHour();
//	_d.Data.Data.minute = (BYTE)time.GetMinute();
//	_d.Data.Data.second = (BYTE)time.GetSecond();
//
//	//микросекунды
//	_d.Data.microseconds = pInfo->iUsecs;
//
//	if (counter >= 100 && (hour != _d.Data.Data.hour || minute != _d.Data.Data.minute || second != _d.Data.Data.second)) 
//	{		
//		hour = _d.Data.Data.hour;
//		minute = _d.Data.Data.minute;
//		second = _d.Data.Data.second;
//		
//		_d.Data.yAxis++;
//		counter = 0;
//	}
//	++counter;
//
//	//дельта между текущим тайстемпом и предыдущим
//	_d.Data.microDelta = 0;
//	if (pInfo->iType != RApi::MD_HISTORY_CB && Data.size())
//		_d.Data.microDelta = MicroSpeed(_d.Data.Data.hour, _d.Data.Data.minute, _d.Data.Data.second, _d.Data.microseconds, 
//							Data[Data.size()-1].Data.Data.hour, Data[Data.size()-1].Data.Data.minute, Data[Data.size()-1].Data.Data.second, Data[Data.size()-1].Data.microseconds);
//	else if (HData.size())
//		HData[HData.size()-1].Data.microDelta = MicroSpeed(HData[HData.size()-1].Data.Data.hour, HData[HData.size()-1].Data.Data.minute, HData[HData.size()-1].Data.Data.second, HData[HData.size()-1].Data.microseconds,
//													_d.Data.Data.hour, _d.Data.Data.minute, _d.Data.Data.second, _d.Data.microseconds);
//
//	//цена и объем
//	_d.Data.Data.tradePrice.SetPrice(pInfo->dPrice, precision);
//	_d.Data.Data.tradeSize = pInfo->iSize;
//
//	//сторона агрессора
//	if (pInfo->sAggressorSide.iDataLen)
//		_d.Data.agressorSide = pInfo->sAggressorSide.pData[0];
//	else {
//		_d.Data.agressorSide = '?';
//		//определяем сторону имплайд-трейда
//		if (Data.size() && pInfo->iType != RApi::MD_HISTORY_CB) {
//			if (_d.Data.Data.tradePrice == Data[Data.size()-1].bidPrice && Data[Data.size()-1].bidSize)
//				_d.Data.impliedSide = 'S';
//			else if (_d.Data.Data.tradePrice == Data[Data.size()-1].askPrice && Data[Data.size()-1].askSize)
//				_d.Data.impliedSide = 'B';
//			else if (_d.Data.Data.tradePrice < Data[Data.size()-1].bidPrice)
//				_d.Data.impliedSide = 'S';
//			else if (_d.Data.Data.tradePrice > Data[Data.size()-1].askPrice)
//				_d.Data.impliedSide = 'B';
//			else 
//				_d.Data.impliedSide = 'M';
//		}
//	}
//
//	//перезапишем предыдущие значения лимитов
//	if (Data.size() && pInfo->iType != RApi::MD_HISTORY_CB) {
//		_d.askPrice = Data[Data.size()-1].askPrice;
//		_d.askSize = Data[Data.size()-1].askSize;
//		_d.askOrders = Data[Data.size()-1].askOrders;
//		_d.bidPrice = Data[Data.size()-1].bidPrice;
//		_d.bidSize = Data[Data.size()-1].bidSize;
//		_d.bidOrders = Data[Data.size()-1].bidOrders;
//
//		//устанавливаем лимиты напротив трейдов
//		if ((_d.Data.agressorSide == 'S' || _d.Data.impliedSide == 'S') && _d.Data.Data.tradePrice == _d.bidPrice) { //трейд по биду
//			//проверяем больше ли трейд чем размер текущего лимита
//			if (_d.bidSize > _d.Data.Data.tradeSize) {
//				_d.tradeSizeQuant = _d.bidSize;
//				_d.bidSize = _d.bidSize - _d.Data.Data.tradeSize;
//				_d.bidOrders = _d.bidOrders - 1;
//			} else {	//если меньше то трейд дошел до следующего уровня или обнулил текущий уровень если размер лимита был равен трейду
//				_d.bidSize = 0;
//				_d.bidOrders = 0;
//				_d.tradeSizeQuant = _d.Data.Data.tradeSize;
//			}		
//		} else if ((_d.Data.agressorSide == 'B' || _d.Data.impliedSide == 'B') && _d.Data.Data.tradePrice == _d.askPrice) {	//трейд по аску
//			//проверяем больше ли трейд чем размер текущего лимита
//			if (_d.askSize > _d.Data.Data.tradeSize) {
//				_d.tradeSizeQuant = _d.askSize;
//				_d.askSize = _d.askSize - _d.Data.Data.tradeSize;
//				_d.askOrders = _d.askOrders - 1;
//			} else {	//если меньше то трейд дошел до следующего уровня или обнулил текущий уровень если размер лимита был равен трейду
//				_d.askSize = 0;
//				_d.askOrders = 0;
//				_d.tradeSizeQuant = _d.Data.Data.tradeSize;
//			}		
//		} else if (_d.Data.impliedSide == 'M')	//трейд внутри спрэда	
//			_d.tradeSizeQuant = _d.Data.Data.tradeSize;
//	}
//
//	//кондишен
//	if (pInfo->sCondition.iDataLen)
//		//_d.Data.condition.assign(pInfo->sCondition.pData, pInfo->sCondition.iDataLen);
//		_d.Data.condition = 1;
//
//	//дельты
//	if (pInfo->bVolumeBoughtFlag)
//		_d.Data.buyVolume = pInfo->iVolumeBought;
//	if (pInfo->bVolumeSoldFlag)
//		_d.Data.sellVolume = pInfo->iVolumeSold;
//
//	if (pInfo->iType == RApi::MD_HISTORY_CB)
//	{
//		ZeroLimitsBBO(_d);
//		push_back_history(_d);
//	}
//	else
//		TickerData<RITHMICBBO>::push_back(_d);
//}
void RDOMTicker::TradePrint(void* p)
{
	RApi::TradeInfo* pInfo = (RApi::TradeInfo*)p;

	RITHMICDOM _d;
	ZeroData(_d, 11);

	//таймстемп
	CTime time(pInfo->iSsboe);
	_d.Data.Data.hour = (BYTE)time.GetHour();
	_d.Data.Data.minute = (BYTE)time.GetMinute();
	_d.Data.Data.second = (BYTE)time.GetSecond();

	//микросекунды
	_d.Data.microseconds = pInfo->iUsecs;

	if (counter >= 100 && (hour != _d.Data.Data.hour || minute != _d.Data.Data.minute || second != _d.Data.Data.second)) 
	{		
		hour = _d.Data.Data.hour;
		minute = _d.Data.Data.minute;
		second = _d.Data.Data.second;
		
		_d.Data.yAxis = true;
		counter = 0;
	}
	++counter;

	//дельта между текущим тайстемпом и предыдущим
	_d.Data.microDelta = 0;
	if (pInfo->iType != RApi::MD_HISTORY_CB && Data.size())
		_d.Data.microDelta = MicroSpeed(_d.Data.Data.hour, _d.Data.Data.minute, _d.Data.Data.second, _d.Data.microseconds, 
							Data[Data.size()-1].Data.Data.hour, Data[Data.size()-1].Data.Data.minute, Data[Data.size()-1].Data.Data.second, Data[Data.size()-1].Data.microseconds);
	else if (HData.size())
		HData[HData.size()-1].Data.microDelta = MicroSpeed(HData[HData.size()-1].Data.Data.hour, HData[HData.size()-1].Data.Data.minute, HData[HData.size()-1].Data.Data.second, HData[HData.size()-1].Data.microseconds,
													_d.Data.Data.hour, _d.Data.Data.minute, _d.Data.Data.second, _d.Data.microseconds);

	//цена и объем
	_d.Data.Data.tradePrice.SetPrice(pInfo->dPrice, precision);
	_d.Data.Data.tradeSize = pInfo->iSize;

	//сторона агрессора
	if (pInfo->sAggressorSide.iDataLen)
		_d.Data.agressorSide = pInfo->sAggressorSide.pData[0];
	else 
	{
		_d.Data.agressorSide = '?';

		int x;
		for (x = 0; x < domLevelsCount; ++x)
		{
			if (_d.Data.Data.tradePrice == Data[Data.size()-1].bidPrice[x] && Data[Data.size()-1].bidSize[x])
			{
				_d.Data.impliedSide = 'S';
				x = domLevelsCount+1;
				break;
			}
			if (_d.Data.Data.tradePrice == Data[Data.size()-1].askPrice[x] && Data[Data.size()-1].askSize[x])
			{
				_d.Data.impliedSide = 'B';
				x = domLevelsCount+1;
				break;
			}
		}
		if (x == domLevelsCount)
		{
			if (_d.Data.Data.tradePrice <= Data[Data.size()-1].bidPrice[0])
				_d.Data.impliedSide = 'S';
			else if (_d.Data.Data.tradePrice >= Data[Data.size()-1].askPrice[0])
				_d.Data.impliedSide = 'B';
			else _d.Data.impliedSide = 'M';
		}
	}

	//кондишен
	if (pInfo->sCondition.iDataLen)
		//_d.Data.condition.assign(pInfo->sCondition.pData, pInfo->sCondition.iDataLen);
		_d.Data.condition = 1;

	//дельты
	if (pInfo->bVolumeBoughtFlag)
		_d.Data.buyVolume = pInfo->iVolumeBought;
	if (pInfo->bVolumeSoldFlag)
		_d.Data.sellVolume = pInfo->iVolumeSold;

	//перезаписываем предыдущие значения
	if (pInfo->iType != RApi::MD_HISTORY_CB)
	{
		WritePreviousBidValue(_d);
		WritePreviousAskValue(_d);
	}
	
	//устанавливаем лимиты напротив трейдов
	if (Data.size() && pInfo->iType != RApi::MD_HISTORY_CB)
		TradesToLimits(_d);

	//определяем максимальные значения в стакане (для хитмепа)
	for (int x = domLevelsCount; x >= 0; --x)
	{
		if (_d.maxDomLimit < _d.askSize[x])
			_d.maxDomLimit = _d.askSize[x];
		if (_d.maxDomLimit < _d.bidSize[x])
			_d.maxDomLimit = _d.bidSize[x];
		if (_d.askSize[x] > 0 && _d.minDomLimit > _d.askSize[x])	//проверка ">0" нужна чтобы отсеч те уровни на которых объем равен нулю
			_d.minDomLimit = _d.askSize[x];
		if (_d.bidSize[x] > 0 && _d.minDomLimit > _d.bidSize[x])
			_d.minDomLimit = _d.bidSize[x];

		if (_d.maxDomOrder < _d.askOrders[x])
			_d.maxDomOrder = _d.askOrders[x];
		if (_d.maxDomOrder < _d.bidOrders[x])
			_d.maxDomOrder = _d.bidOrders[x];
		if (_d.askOrders[x] > 0 && _d.minDomOrder > _d.askOrders[x])
			_d.minDomOrder = _d.askOrders[x];
		if (_d.bidOrders[x] > 0 && _d.minDomOrder > _d.bidOrders[x])
			_d.minDomOrder = _d.bidOrders[x];
	}

	if (pInfo->iType == RApi::MD_HISTORY_CB)
	{
		ZeroLimitDOM(_d, 11);
		push_back_history(_d);
	}
	else
		TickerData<RITHMICDOM>::push_back(_d);
}

//void RBBOTicker::BestBidQuote(RApi::BidInfo* pBid)
//{
//	RITHMICBBO _d;
//	ZeroData(_d);
//	
//	//таймстемп
//	CTime time(pBid->iSsboe);
//	_d.Data.Data.hour = (BYTE)time.GetHour();
//	_d.Data.Data.minute = (BYTE)time.GetMinute();
//	_d.Data.Data.second = (BYTE)time.GetSecond();
//
//	//микросекунды
//	_d.Data.microseconds = pBid->iUsecs;
//
//	if (counter >= 100 && (hour != _d.Data.Data.hour || minute != _d.Data.Data.minute || second != _d.Data.Data.second)) 
//	{		
//		hour = _d.Data.Data.hour;
//		minute = _d.Data.Data.minute;
//		second = _d.Data.Data.second;
//		
//		_d.Data.yAxis++;
//		counter = 0;
//	}
//	++counter;
//
//	//дельта между текущим тайстемпом и предыдущим
//	_d.Data.microDelta = 0;
//	if (Data.size())
//		_d.Data.microDelta = MicroSpeed(_d.Data.Data.hour, _d.Data.Data.minute, _d.Data.Data.second, _d.Data.microseconds, 
//									Data[Data.size()-1].Data.Data.hour, Data[Data.size()-1].Data.Data.minute, Data[Data.size()-1].Data.Data.second, Data[Data.size()-1].Data.microseconds);
//
//	//цена и объем бида
//	_d.bidPrice.SetPrice(pBid->dPrice, precision);
//	_d.bidSize = pBid->iSize;
//	_d.bidOrders = pBid->iNumOrders;
//	//цена и объем аска
//	if (Data.size()) {
//		_d.askPrice = Data[Data.size()-1].askPrice;
//		_d.askSize = Data[Data.size()-1].askSize;
//		_d.askOrders = Data[Data.size()-1].askOrders;
//	}
//
//	//сторона агрессора
//	_d.Data.agressorSide = 'b';
//	_d.Data.impliedSide = 'N';
//
//	//расчитываем дельты изменений
//	if (Data.size() && Data[Data.size()-1].bidPrice == _d.bidPrice)
//		_d.deltaBidLimitSize = pBid->iSize - Data[Data.size()-1].bidSize;
//
//	TickerData<RITHMICBBO>::push_back(_d);
//}
//void RBBOTicker::BestAskQuote(RApi::AskInfo* pAsk)
//{
//	RITHMICBBO _d;
//	ZeroData(_d);
//
//	//таймстемп
//	CTime time(pAsk->iSsboe);
//	_d.Data.Data.hour = (BYTE)time.GetHour();
//	_d.Data.Data.minute = (BYTE)time.GetMinute();
//	_d.Data.Data.second = (BYTE)time.GetSecond();
//
//	if (counter >= 100 && (hour != _d.Data.Data.hour || minute != _d.Data.Data.minute || second != _d.Data.Data.second)) 
//	{		
//		hour = _d.Data.Data.hour;
//		minute = _d.Data.Data.minute;
//		second = _d.Data.Data.second;
//		
//		_d.Data.yAxis++;
//		counter = 0;
//	}
//	++counter;
//
//	//микросекунды
//	_d.Data.microseconds = pAsk->iUsecs;
//
//	//дельта между текущим тайстемпом и предыдущим
//	_d.Data.microDelta = 0;
//	if (Data.size())
//		_d.Data.microDelta = MicroSpeed(_d.Data.Data.hour, _d.Data.Data.minute, _d.Data.Data.second, _d.Data.microseconds, 
//									Data[Data.size()-1].Data.Data.hour, Data[Data.size()-1].Data.Data.minute, Data[Data.size()-1].Data.Data.second, Data[Data.size()-1].Data.microseconds);
//
//	//цена и объем бида
//	_d.askPrice.SetPrice(pAsk->dPrice, precision);
//	_d.askSize = pAsk->iSize;
//	_d.askOrders = pAsk->iNumOrders;
//	//цена и объем аска
//	if (Data.size()) {
//		_d.bidPrice = Data[Data.size()-1].bidPrice;
//		_d.bidSize = Data[Data.size()-1].bidSize;
//		_d.bidOrders = Data[Data.size()-1].bidOrders;
//	}
//
//	//сторона агрессора
//	_d.Data.agressorSide = 'a';
//	_d.Data.impliedSide = 'N';
//
//	//расчитываем дельты изменений
//	if (Data.size() && Data[Data.size()-1].askPrice == _d.askPrice)
//		_d.deltaAskLimitSize = pAsk->iSize - Data[Data.size()-1].askSize;
//
//	TickerData<RITHMICBBO>::push_back(_d);
//}
//void RBBOTicker::BestBidAskQuote(RApi::BidInfo* pBid, RApi::AskInfo* pAsk)
//{
//	RITHMICBBO _d;
//	ZeroData(_d);
//
//	//таймстемп
//	CTime time(pBid->iSsboe);
//	_d.Data.Data.hour = (BYTE)time.GetHour();
//	_d.Data.Data.minute = (BYTE)time.GetMinute();
//	_d.Data.Data.second = (BYTE)time.GetSecond();
//
//	//микросекунды
//	_d.Data.microseconds = pBid->iUsecs;
//
//	if (counter >= 100 && (hour != _d.Data.Data.hour || minute != _d.Data.Data.minute || second != _d.Data.Data.second)) 
//	{		
//		hour = _d.Data.Data.hour;
//		minute = _d.Data.Data.minute;
//		second = _d.Data.Data.second;
//		
//		_d.Data.yAxis++;
//		counter = 0;
//	}
//	++counter;
//
//	//дельта между текущим тайстемпом и предыдущим
//	_d.Data.microDelta = 0;
//	if (Data.size())
//		_d.Data.microDelta = MicroSpeed(_d.Data.Data.hour, _d.Data.Data.minute, _d.Data.Data.second, _d.Data.microseconds, 
//									Data[Data.size()-1].Data.Data.hour, Data[Data.size()-1].Data.Data.minute, Data[Data.size()-1].Data.Data.second, Data[Data.size()-1].Data.microseconds);
//
//	//цена и объем бида
//	_d.bidPrice.SetPrice(pBid->dPrice, precision);
//	_d.bidSize = pBid->iSize;
//	_d.bidOrders = pBid->iNumOrders;
//	//цена и объем аска
//	_d.askPrice.SetPrice(pAsk->dPrice, precision);
//	_d.askSize = pAsk->iSize;
//	_d.askOrders = pAsk->iNumOrders;
//
//	//сторона агрессора
//	if (Data.size()) {
//		if (Data[Data.size()-1].askSize != pAsk->iSize && Data[Data.size()-1].bidSize != pBid->iSize)
//			_d.Data.agressorSide = '2';
//		else if (Data[Data.size()-1].askSize != pAsk->iSize)
//			_d.Data.agressorSide = 'a';
//		else if (Data[Data.size()-1].bidSize != pBid->iSize)
//			_d.Data.agressorSide = 'b';
//	} else {
//		_d.Data.agressorSide = '0';
//	}
//	_d.Data.impliedSide = 'N';
//
//	//расчитываем дельты изменений
//	if (Data.size()) {
//		if (Data[Data.size()-1].askPrice == _d.askPrice)
//			_d.deltaAskLimitSize = pAsk->iSize - Data[Data.size()-1].askSize;
//		if (Data[Data.size()-1].bidPrice == _d.bidPrice)
//			_d.deltaBidLimitSize = pBid->iSize - Data[Data.size()-1].bidSize;
//	}
//
//	TickerData<RITHMICBBO>::push_back(_d);
//}

void RDOMTicker::LimitOrderBook(RApi::LimitOrderBookInfo* pInfo)
{
	RITHMICDOM _d;
	ZeroData(_d, 11);

	//количество ценовых уровней в стакане
	if (pInfo->iAskArrayLen > 5) 
		domLevelsCount = 10;
	else domLevelsCount = 5;

	//таймстемп
	CTime time(pInfo->iSsboe);
	_d.Data.Data.hour = (BYTE)time.GetHour();
	_d.Data.Data.minute = (BYTE)time.GetMinute();
	_d.Data.Data.second = (BYTE)time.GetSecond();

	//микросекунды
	_d.Data.microseconds = pInfo->iUsecs;

	if (counter >= 100 && (hour != _d.Data.Data.hour || minute != _d.Data.Data.minute || second != _d.Data.Data.second)) 
	{		
		hour = _d.Data.Data.hour;
		minute = _d.Data.Data.minute;
		second = _d.Data.Data.second;
		
		_d.Data.yAxis = true;
		counter = 0;
	}
	++counter;

	//заполняем цены, объемы и ордера
	for (int j = 0; j < domLevelsCount; j++)
	{
		_d.askPrice[j].SetPrice(pInfo->adAskPriceArray[j], precision);	//аски цены
		_d.askSize[j] = pInfo->aiAskSizeArray[j];						//аски объемы
		_d.askOrders[j] = pInfo->aiAskNumOrdersArray[j];				//аски ордера
	
		_d.bidPrice[j].SetPrice(pInfo->adBidPriceArray[j], precision);	//биды цены
		_d.bidSize[j] = pInfo->aiBidSizeArray[j];						//биды объемы
		_d.bidOrders[j] = pInfo->aiBidNumOrdersArray[j];				//биды ордера
	}

	//определяем максимальные значения в стакане (для хитмепа)
	for (int x = domLevelsCount-1; x >= 0; --x)
	{
		if (_d.maxDomLimit < _d.askSize[x])
			_d.maxDomLimit = _d.askSize[x];
		if (_d.maxDomLimit < _d.bidSize[x])
			_d.maxDomLimit = _d.bidSize[x];
		if (_d.minDomLimit > _d.askSize[x])
			_d.minDomLimit = _d.askSize[x];
		if (_d.minDomLimit > _d.bidSize[x])
			_d.minDomLimit = _d.bidSize[x];

		if (_d.maxDomOrder < _d.askOrders[x])
			_d.maxDomOrder = _d.askOrders[x];
		if (_d.maxDomOrder < _d.bidOrders[x])
			_d.maxDomOrder = _d.bidOrders[x];
		if (_d.minDomOrder > _d.askOrders[x])
			_d.minDomOrder = _d.askOrders[x];
		if (_d.minDomOrder > _d.bidOrders[x])
			_d.minDomOrder = _d.bidOrders[x];
	}

	TickerData<RITHMICDOM>::push_back(_d);
}
void RDOMTicker::AskQuote(RApi::AskInfo* pInfo)
{
	RITHMICDOM _d;
	ZeroData(_d, GetDomLevelsCount());

	//таймстемп
	CTime time(pInfo->iSsboe);
	_d.Data.Data.hour = (BYTE)time.GetHour();
	_d.Data.Data.minute = (BYTE)time.GetMinute();
	_d.Data.Data.second = (BYTE)time.GetSecond();

	//микросекунды
	_d.Data.microseconds = pInfo->iUsecs;

	if (counter >= 100 && (hour != _d.Data.Data.hour || minute != _d.Data.Data.minute || second != _d.Data.Data.second)) 
	{		
		hour = _d.Data.Data.hour;
		minute = _d.Data.Data.minute;
		second = _d.Data.Data.second;
		
		_d.Data.yAxis = true;
		counter = 0;
	}
	++counter;

	//дельта между текущим тайстемпом и предыдущим
	_d.Data.microDelta = 0;
	if (Data.size())
		_d.Data.microDelta = MicroSpeed(_d.Data.Data.hour, _d.Data.Data.minute, _d.Data.Data.second, _d.Data.microseconds, 
									Data[Data.size()-1].Data.Data.hour, Data[Data.size()-1].Data.Data.minute, Data[Data.size()-1].Data.Data.second, Data[Data.size()-1].Data.microseconds);

	//сторона агрессора
	_d.Data.agressorSide = 'a';

	//новые значения цен и сайзов
	WriteNewData(_d, 'A', Price(pInfo->dPrice, precision), pInfo->iSize, pInfo->iNumOrders);

	//определяем максимальные значения в стакане (для хитмепа)
	for (int x = domLevelsCount; x >= 0; --x)
	{
		if (_d.maxDomLimit < _d.askSize[x])
			_d.maxDomLimit = _d.askSize[x];
		if (_d.maxDomLimit < _d.bidSize[x])
			_d.maxDomLimit = _d.bidSize[x];
		if (_d.askSize[x] > 0 && _d.minDomLimit > _d.askSize[x])	//проверка ">0" нужна чтобы отсеч те уровни на которых объем равен нулю
			_d.minDomLimit = _d.askSize[x];
		if (_d.bidSize[x] > 0 && _d.minDomLimit > _d.bidSize[x])
			_d.minDomLimit = _d.bidSize[x];

		if (_d.maxDomOrder < _d.askOrders[x])
			_d.maxDomOrder = _d.askOrders[x];
		if (_d.maxDomOrder < _d.bidOrders[x])
			_d.maxDomOrder = _d.bidOrders[x];
		if (_d.askOrders[x] > 0 && _d.minDomOrder > _d.askOrders[x])
			_d.minDomOrder = _d.askOrders[x];
		if (_d.bidOrders[x] > 0 && _d.minDomOrder > _d.bidOrders[x])
			_d.minDomOrder = _d.bidOrders[x];
	}

	//алгоритм работы с флагами
	_d.updateType = pInfo->iUpdateType;
	OrderTypeSpec(_d, pInfo->iUpdateType, pInfo->iSize);

	TickerData<RITHMICDOM>::push_back(_d);
}
void RDOMTicker::BidQuote(RApi::BidInfo* pInfo)
{
	RITHMICDOM _d;
	ZeroData(_d, GetDomLevelsCount());

	//таймстемп
	CTime time(pInfo->iSsboe);
	_d.Data.Data.hour = (BYTE)time.GetHour();
	_d.Data.Data.minute = (BYTE)time.GetMinute();
	_d.Data.Data.second = (BYTE)time.GetSecond();

	//микросекунды
	_d.Data.microseconds = pInfo->iUsecs;

	if (counter >= 100 && (hour != _d.Data.Data.hour || minute != _d.Data.Data.minute || second != _d.Data.Data.second)) 
	{		
		hour = _d.Data.Data.hour;
		minute = _d.Data.Data.minute;
		second = _d.Data.Data.second;
		
		_d.Data.yAxis = true;
		counter = 0;
	}
	++counter;

	//дельта между текущим тайстемпом и предыдущим
	_d.Data.microDelta = 0;
	if (Data.size())
		_d.Data.microDelta = MicroSpeed(_d.Data.Data.hour, _d.Data.Data.minute, _d.Data.Data.second, _d.Data.microseconds, 
									Data[Data.size()-1].Data.Data.hour, Data[Data.size()-1].Data.Data.minute, Data[Data.size()-1].Data.Data.second, Data[Data.size()-1].Data.microseconds);

	//сторона агрессора
	_d.Data.agressorSide = 'b';

	//новые значения цен и сайзов
	WriteNewData(_d, 'B', Price(pInfo->dPrice, precision), pInfo->iSize, pInfo->iNumOrders);

	//определяем максимальные значения в стакане (для хитмепа)
	for (int x = domLevelsCount; x >= 0; --x)
	{
		if (_d.maxDomLimit < _d.askSize[x])
			_d.maxDomLimit = _d.askSize[x];
		if (_d.maxDomLimit < _d.bidSize[x])
			_d.maxDomLimit = _d.bidSize[x];
		if (_d.askSize[x] > 0 && _d.minDomLimit > _d.askSize[x])	//проверка ">0" нужна чтобы отсеч те уровни на которых объем равен нулю
			_d.minDomLimit = _d.askSize[x];
		if (_d.bidSize[x] > 0 && _d.minDomLimit > _d.bidSize[x])
			_d.minDomLimit = _d.bidSize[x];

		if (_d.maxDomOrder < _d.askOrders[x])
			_d.maxDomOrder = _d.askOrders[x];
		if (_d.maxDomOrder < _d.bidOrders[x])
			_d.maxDomOrder = _d.bidOrders[x];
		if (_d.askOrders[x] > 0 && _d.minDomOrder > _d.askOrders[x])
			_d.minDomOrder = _d.askOrders[x];
		if (_d.bidOrders[x] > 0 && _d.minDomOrder > _d.bidOrders[x])
			_d.minDomOrder = _d.bidOrders[x];
	}

	//алгоритм работы с флагами
	_d.updateType = pInfo->iUpdateType;
	OrderTypeSpec(_d, pInfo->iUpdateType, pInfo->iSize);

	TickerData<RITHMICDOM>::push_back(_d);
}

void RDOMTicker::OrderTypeSpec(RITHMICDOM& _d, int type, int size)
{
	//для одиночного ордера
	if (type == RApi::UPDATE_TYPE_SOLO)
	{
		if (size > 0) _d.orderType = 1;
		else if (size < 0) _d.orderType = 2;
	}
	//для составного ордера
	else if (type == RApi::UPDATE_TYPE_END && size)
	{
		//так как ордер составной ищем индекс начала этого ордера
		bool thirdFlag = false;
		size_t j = Data.size()-1;
		
		if (Data[j].updateType == RApi::UPDATE_TYPE_BEGIN || Data[j].updateType == RApi::UPDATE_TYPE_MIDDLE) 
		{
			//определяем какой ордер был 2-4 или 2-3-4 и ищем индекс начала этого ордера
			while (Data[j].updateType != RApi::UPDATE_TYPE_BEGIN)
			{			
				//если был третий флаг, значит устанавливаем thirdFlag
				if (j > 0 && Data[j].updateType == RApi::UPDATE_TYPE_MIDDLE)
					thirdFlag = true;
			
				--j;
			}

			//переменные для флагов 234
			for (; j > 0 && j < Data.size(); j++)
			{
				if (!thirdFlag) //значит ордер 2-4
				{
					if (GetDeltaLimitSize(Data[j]) > 0) Data[j].orderType = 3;
					else if (GetDeltaLimitSize(Data[j]) < 0) Data[j].orderType = 4;
				}
				else //ордер 2-3-4
				{
					if (GetDeltaLimitSize(Data[j]) > 0) Data[j].orderType = 5;
					else if (GetDeltaLimitSize(Data[j]) < 0) Data[j].orderType = 6;
				}
			}

			//тип оредра текущего значения
			if (!thirdFlag) //значит ордер 2-4
			{
				if (GetDeltaLimitSize(_d) > 0) _d.orderType = 3;
				else if (GetDeltaLimitSize(_d) < 0) _d.orderType = 4;
			}
			else //ордер 2-3-4
			{
				if (GetDeltaLimitSize(_d) > 0) _d.orderType = 5;
				else if (GetDeltaLimitSize(_d) < 0) _d.orderType = 6;
			}
		}
	}
}
int RDOMTicker::GetDeltaLimitSize(RITHMICDOM& rd)
{
	for (int j = domLevelsCount-1; j >= 0; --j)
	{
		if (rd.deltaAskLimitSize[j])
			return rd.deltaAskLimitSize[j];
		if (rd.deltaBidLimitSize[j])
			return rd.deltaBidLimitSize[j];
	}
	return 0;
}

void RDOMTicker::WritePreviousBidValue(RITHMICDOM& _d)	//перезапись предыдущих значений бидов в текущие значения
{
	if (Data.size()) {
		for (int j = 0; j <= domLevelsCount; j++)
		{
			_d.bidPrice[j].SetPrice(Data[Data.size()-1].bidPrice[j]);	//цены
			_d.bidSize[j] = Data[Data.size()-1].bidSize[j];				//объемы
			_d.bidOrders[j] = Data[Data.size()-1].bidOrders[j];			//ордера
		}
	}
}
void RDOMTicker::WritePreviousAskValue(RITHMICDOM& _d)	//перезапись предыдущих значений асков в текущие значения
{
	if (Data.size()) {
		for (int j = 0; j <= domLevelsCount; j++)
		{
			_d.askPrice[j].SetPrice(Data[Data.size()-1].askPrice[j]);	//цены
			_d.askSize[j] = Data[Data.size()-1].askSize[j];				//объемы
			_d.askOrders[j] = Data[Data.size()-1].askOrders[j];			//ордера
		}
	}
}
void CQGDOMTicker::WritePreviousBidValue(CQGDOM& _d)	//перезапись предыдущих значений бидов в текущие значения
{
	if (Data.size()) {
		for (int j = 0; j <= domLevelsCount; j++)
		{
			_d.bidPrice[j].SetPrice(Data[Data.size()-1].bidPrice[j]);	//цены
			_d.bidSize[j] = Data[Data.size()-1].bidSize[j];				//объемы
		}
	}
}
void CQGDOMTicker::WritePreviousAskValue(CQGDOM& _d)	//перезапись предыдущих значений асков в текущие значения
{
	if (Data.size()) {
		for (int j = 0; j <= domLevelsCount; j++)
		{
			_d.askPrice[j].SetPrice(Data[Data.size()-1].askPrice[j]);	//цены
			_d.askSize[j] = Data[Data.size()-1].askSize[j];				//объемы
		}
	}
}

void RDOMTicker::TradesToLimits(RITHMICDOM& _d)
{
	if ((_d.Data.agressorSide == 'S' || _d.Data.impliedSide == 'S') && _d.Data.Data.tradePrice == _d.bidPrice[0]) {	//трейд по биду
	
		if (_d.bidSize[0] > _d.Data.Data.tradeSize)	//проверяем больше ли трейд чем размер текущего лимита
		{
			_d.tradeSizeQuant = _d.bidSize[0];
			_d.bidSize[0] = _d.bidSize[0] - _d.Data.Data.tradeSize;
			if (Data[Data.size()-1].bidOrders[0]) _d.bidOrders[0] = Data[Data.size()-1].bidOrders[0] - 1;
		} else { //если меньше то трейд дошел до следующего уровня или обнулил текущий уровень если размер лимита был равен трейду
			_d.tradeSizeQuant = _d.Data.Data.tradeSize;
			_d.bidSize[0] = 0;
			_d.bidOrders[0] = 0;
		}

	} else if ((_d.Data.agressorSide == 'B' || _d.Data.impliedSide == 'B') && _d.Data.Data.tradePrice == _d.askPrice[0]) { //трейд по аску
		
		//проверяем больше ли трейд чем размер текущего лимита
		if (_d.askSize[0] > _d.Data.Data.tradeSize)
		{
			_d.tradeSizeQuant = _d.askSize[0];
			_d.askSize[0] = _d.askSize[0] - _d.Data.Data.tradeSize;
			if (Data[Data.size()-1].askOrders[0])
				_d.askOrders[0] = Data[Data.size()-1].askOrders[0] - 1;
		} else { //если меньше то трейд дошел до следующего уровня или обнулил текущий уровень если размер лимита был равен трейду
			_d.tradeSizeQuant = _d.Data.Data.tradeSize;
			_d.askSize[0] = 0;
			_d.askOrders[0] = 0;
		}	

	} else if (_d.Data.Data.tradePrice > _d.bidPrice[0] && _d.Data.Data.tradePrice < _d.askPrice[0]) { //трейд внутри спрэда	

		_d.tradeSizeQuant = _d.Data.Data.tradeSize;

	} else if ((_d.Data.agressorSide == 'S' || _d.Data.impliedSide == 'S') && _d.Data.Data.tradePrice < _d.bidPrice[0]) { //трейд ниже спрэда
		
		for (int j = 1; j <= domLevelsCount; j++)
		{
			if (_d.Data.Data.tradePrice == _d.bidPrice[j])
			{
				if (_d.bidSize[j] > _d.Data.Data.tradeSize)
				{
					_d.tradeSizeQuant = _d.bidSize[j];
					_d.bidSize[j] = _d.bidSize[j] - _d.Data.Data.tradeSize;
					if (Data[Data.size()-1].bidOrders[j])
						_d.bidOrders[j] = Data[Data.size()-1].bidOrders[j] - 1;
					j = domLevelsCount;
				}
				else
				{
					_d.tradeSizeQuant = _d.Data.Data.tradeSize;
					_d.bidSize[j] = 0;
					_d.bidOrders[j] = 0;
				}
			}
		}

	} else if ((_d.Data.agressorSide == 'B' || _d.Data.impliedSide == 'B') && _d.Data.Data.tradePrice > _d.askPrice[0]) { //трейд выше спрэда
		
		for (int j = 1; j <= domLevelsCount; j++)
		{
			if (_d.Data.Data.tradePrice == _d.askPrice[j])
			{
				if (_d.askSize[j] > _d.Data.Data.tradeSize)
				{
					_d.tradeSizeQuant = _d.askSize[j];
					_d.askSize[j] = _d.askSize[j] - _d.Data.Data.tradeSize;
					if (Data[Data.size()-1].askOrders[j])
						_d.askOrders[j] = Data[Data.size()-1].askOrders[j] - 1;
					j = domLevelsCount;
				}
				else
				{
					_d.tradeSizeQuant = _d.Data.Data.tradeSize;
					_d.askSize[j] = 0;
					_d.askOrders[j] = 0;
				}
			}
		}
	}
}
void CQGDOMTicker::TradesToLimits(CQGDOM& _d)
{
	if ((_d.Data.agressorSide == 'S') && _d.Data.Data.tradePrice == _d.bidPrice[0]) {	//трейд по биду
	
		if (_d.bidSize[0] > _d.Data.Data.tradeSize)	//проверяем больше ли трейд чем размер текущего лимита
		{
			_d.tradeSizeQuant = _d.bidSize[0];
			_d.bidSize[0] = _d.bidSize[0] - _d.Data.Data.tradeSize;
		} else { //если меньше то трейд дошел до следующего уровня или обнулил текущий уровень если размер лимита был равен трейду
			_d.tradeSizeQuant = _d.Data.Data.tradeSize;
			_d.bidSize[0] = 0;
		}

	} else if ((_d.Data.agressorSide == 'B') && _d.Data.Data.tradePrice == _d.askPrice[0]) { //трейд по аску
		
		//проверяем больше ли трейд чем размер текущего лимита
		if (_d.askSize[0] > _d.Data.Data.tradeSize)
		{
			_d.tradeSizeQuant = _d.askSize[0];
			_d.askSize[0] = _d.askSize[0] - _d.Data.Data.tradeSize;
		} else { //если меньше то трейд дошел до следующего уровня или обнулил текущий уровень если размер лимита был равен трейду
			_d.tradeSizeQuant = _d.Data.Data.tradeSize;
			_d.askSize[0] = 0;
		}	

	} else if (_d.Data.Data.tradePrice > _d.bidPrice[0] && _d.Data.Data.tradePrice < _d.askPrice[0]) { //трейд внутри спрэда	

		_d.tradeSizeQuant = _d.Data.Data.tradeSize;

	} else if ((_d.Data.agressorSide == 'S') && _d.Data.Data.tradePrice < _d.bidPrice[0]) { //трейд ниже спрэда
		
		for (int j = 1; j <= domLevelsCount; j++)
		{
			if (_d.Data.Data.tradePrice == _d.bidPrice[j])
			{
				if (_d.bidSize[j] > _d.Data.Data.tradeSize)
				{
					_d.tradeSizeQuant = _d.bidSize[j];
					_d.bidSize[j] = _d.bidSize[j] - _d.Data.Data.tradeSize;
					j = domLevelsCount;
				}
				else
				{
					_d.tradeSizeQuant = _d.Data.Data.tradeSize;
					_d.bidSize[j] = 0;
				}
			}
		}

	} else if ((_d.Data.agressorSide == 'B') && _d.Data.Data.tradePrice > _d.askPrice[0]) { //трейд выше спрэда
		
		for (int j = 1; j <= domLevelsCount; j++)
		{
			if (_d.Data.Data.tradePrice == _d.askPrice[j])
			{
				if (_d.askSize[j] > _d.Data.Data.tradeSize)
				{
					_d.tradeSizeQuant = _d.askSize[j];
					_d.askSize[j] = _d.askSize[j] - _d.Data.Data.tradeSize;
					j = domLevelsCount;
				}
				else
				{
					_d.tradeSizeQuant = _d.Data.Data.tradeSize;
					_d.askSize[j] = 0;
				}
			}
		}
	}
}

void RDOMTicker::WriteNewData(RITHMICDOM& _d, char Type, const Price& price, int limits, int traders)
{
	//флаг найденной цены
	bool findPrice = false;

	//перезаписываем предыдущие значения
	WritePreviousBidValue(_d);
	WritePreviousAskValue(_d);

	if (Type == 'A')
	{
		//вычисляем дельту поставленного/убранного лимита если такая цена уже есть в стакане
		int j;
		for (j = 0; j <= domLevelsCount; j++)
		{
			if (price == _d.askPrice[j])
			{
				if (j < domLevelsCount-1 || (j == domLevelsCount-1 && Data[Data.size()-1].askSize[j]))
					_d.deltaAskLimitSize[j] = limits - Data[Data.size()-1].askSize[j];	//размер поставленного/убранного лимита
				findPrice = true;
				break;	//принудительно заканчиваем цикл
			}
		}

		//для каждого уровня проверяем равенство объема нулю и сдвигаем все вниз если так
		for (int k = 0; k < domLevelsCount; k++)
		{
			bool isVol = false;
			if (!_d.askSize[k] && _d.askPrice[k].IsPrice())	//нашли элемент с нулевым сайзом и нормальной ценой
			{
				for (j = domLevelsCount; j >= k; --j)		//проверяем есть ли за ним ликвидность еще
					if (_d.askSize[j]) 
						isVol = true;

				if (!isVol) break;		//ликвидности нет, выходим из цикла
			}			
			if (isVol)
			{
				do {
					for (j = k; j < domLevelsCount; j++)
					{
						_d.askPrice[j].SetPrice(_d.askPrice[j+1]);
						_d.askSize[j] = _d.askSize[j+1];
						_d.askOrders[j] = _d.askOrders[j+1];
					}
					
					_d.askPrice[j-1] = _d.askPrice[j-2];
					IncPrice(_d.askPrice[j-1]);
				}
				while (!_d.askSize[k]);
			}
		}

		//потом записываем новое значение
		for (j = 0; j <= domLevelsCount; j++)
		{
			if (price == _d.askPrice[j])
			{
				_d.askSize[j] = limits;
				if (traders) _d.askOrders[j] = traders;
				_d.eventAskLevel = 1 << j;	
				break;	//принудительно заканчиваем цикл
			}
		}

		//если такой цены не оказалось
		if (!findPrice && limits)
		{
			bool zeroFlag = false;	//флаг наличия нулевого уровня в стакане, на какой-либо цене
			int zeroLvl;			//номер уровня на котором нулевой объем есть
			//меньше чем цена Lvl1, то ищем нет ли пустых уровней в стакане с нулевым объемом начиная снизу стакана
			if (price < _d.askPrice[0])
			{
				//for (j = 0; j < domLevelsCount; j++)
				//{
				//	if (!_d.askSize[j]) {	
				//		zeroFlag = true; zeroLvl = j; break;	
				//	}
				//}
				//				
				//if (zeroFlag) j = zeroLvl;								//и есть пустые уровни, то двигаем массив вниз до цены с нулевым объемом
				//else j = domLevelsCount;								//и нет пустых уровней, то двигаем весь массив вниз
				//сдвигаем уровни
				for (j = domLevelsCount; j > 0; j--)
				{
					_d.askPrice[j] = _d.askPrice[j-1];
					_d.askSize[j] = _d.askSize[j-1];
					_d.askOrders[j] = _d.askOrders[j-1];
				}			
				
				//и вставляем в первую позицию новые значения
				_d.askPrice[0] = price;
				_d.askSize[0] = limits;
				if (traders) _d.askOrders[0] = traders;
				_d.eventAskLevel = 1 << 0;
				_d.deltaAskLimitSize[0] = limits;

				//обнуляем значение лимитов и ордеров на 11 уровне в стакане, т.к. оно может измениться в любой момент, а мы этого не увидим,
				//поэтому эти значения нам не нужны
				_d.askSize[domLevelsCount] = 0;
				_d.askOrders[domLevelsCount] = 0;
			}
			//если цена больше чем цена domLevelsCount
			else if (price > _d.askPrice[domLevelsCount])
			{
				for (j = domLevelsCount; j >= 0; j--)
				{
					if (!_d.askSize[j]) { zeroFlag = true; zeroLvl = j; break; }
				}
				
				if (zeroFlag) j = zeroLvl;				//и есть пустые уровни, то двигаем массив вверх до цены с нулевым объемом
				else 
					j = 0;

				//сдвигаем уровни	
				if (j > 0)
				{
					for (; j < domLevelsCount; j++)
					{
						_d.askPrice[j] = _d.askPrice[j+1];
						_d.askSize[j] = _d.askSize[j+1];
						_d.askOrders[j] = _d.askOrders[j+1];
					}
					j = domLevelsCount;

					//и вставляем в последнюю позицию новые значения
					_d.askPrice[j] = price;
					_d.askSize[j] = limits;
					if (traders) _d.askOrders[j] = traders;
					_d.eventAskLevel = 1 << j;
				}

			} else if (price < _d.askPrice[domLevelsCount] && price > _d.askPrice[0]) { //если цена находится внутри стакана по аскам
				int index;
				//ищем индекс места в которое будем вставлять новое значение
				for (j = 1; j <= domLevelsCount; j++)
				{
					if (price <= _d.askPrice[j] && price >= _d.askPrice[j-1]) 
					{	index = j;	break;	}
				}
				//если у одного из значений кол-во лимитов равно нулю, то на его место пишем
				if (_d.askSize[index] == 0)
				{
					//вставляем на место индекса текущее значение
					_d.askPrice[index] = price;
					_d.askSize[index] = limits;
					if (traders) _d.askOrders[index] = traders;
					_d.eventAskLevel = 1 << index;
					if (index != domLevelsCount-1)
						_d.deltaAskLimitSize[index] = limits;
				}
				else if (_d.askSize[index-1] == 0)
				{
					//вставляем на место индекса текущее значение
					_d.askPrice[index-1] = price;
					_d.askSize[index-1] = limits;
					if (traders) _d.askOrders[index-1] = traders;
					_d.eventAskLevel = 1 << (index-1);
					_d.deltaAskLimitSize[index-1] = limits;
				}
				//если нулей нет, то сдвигаем все вверх начиная с уровня на который указывается индекс
				else
				{
					for (j = domLevelsCount; j > index; j--)
					{
						_d.askPrice[j] = _d.askPrice[j-1];
						_d.askSize[j] = _d.askSize[j-1];
						_d.askOrders[j] = _d.askOrders[j-1];
					}
					//вставляем на место индекса текущее значение
					_d.askPrice[index] = price;
					_d.askSize[index] = limits;
					if (traders) _d.askOrders[index] = traders;
					_d.eventAskLevel = 1 << index;
					_d.deltaAskLimitSize[index] = limits;
				}
			}
		}
	}
	else if (Type == 'B')
	{
		//вычисляем дельту поставленного/убранного лимита если такая цена уже есть в стакане
		int j; 
		for (j = 0; j <= domLevelsCount; j++)
		{
			if (price == _d.bidPrice[j])
			{
				if (j < domLevelsCount-1 || (j == domLevelsCount-1 && Data[Data.size()-1].bidSize[j]))
					_d.deltaBidLimitSize[j] =  limits - Data[Data.size()-1].bidSize[j];	//размер поставленного/убранного лимита
				findPrice = true;
				break;	//принудительно заканчиваем цикл
			}
		}

		//для каждого уровня проверяем равенство объема нулю и сдвигаем все вверх если так
		for (int k = 0; k < domLevelsCount; k++)
		{
			bool isVol = false;
			if (!_d.bidSize[k] && _d.bidPrice[k].IsPrice())	//нашли элемент с нулевым сайзом и нормальной ценой
			{
				for (j = domLevelsCount; j >= k; --j)		//проверяем есть ли за ним ликвидность еще
					if (_d.bidSize[j]) 
						isVol = true;

				if (!isVol) break;		//ликвидности нет, выходим из цикла
			}			
			if (isVol)
			{
				do {
					for (j = k; j < domLevelsCount; j++)
					{
						_d.bidPrice[j].SetPrice(_d.bidPrice[j+1]);
						_d.bidSize[j] = _d.bidSize[j+1];
						_d.bidOrders[j] = _d.bidOrders[j+1];
					}
					
					_d.bidPrice[j-1] = _d.bidPrice[j-2];
					DecPrice(_d.bidPrice[j-1]);
				}
				while (!_d.bidSize[k]);
			}
		}
		
		//записываем новое значение
		for (j = 0; j <= domLevelsCount; j++)
		{
			if (price == _d.bidPrice[j])
			{
				_d.bidSize[j] = limits;
				if (traders) _d.bidOrders[j] = traders;
				_d.eventBidLevel = 1 << j;
				break;	//принудительно заканчиваем цикл
			}
		}

		//если такой цены не оказалось
		if (!findPrice && limits)
		{
			bool zeroFlag = false;	//флаг наличия нулевого уровня в стакане, на какой-либо цене
			int zeroLvl;			//номер уровня на котором нулевой объем есть
			//больше чем цена Lvl1, то ищем нет ли пустых уровней в стакане с нулевым объемом начиная сверху стакана
			if (price > _d.bidPrice[0])
			{
				//for (j = 0; j < domLevelsCount; j++)
				//{
				//	if (!_d.bidSize[j]) { zeroFlag = true; zeroLvl = j; break; }
				//}
				//				
				//if (zeroFlag) j = zeroLvl;								//и есть пустые уровни, то двигаем массив вниз до цены с нулевым объемом
				//else j = domLevelsCount;			//и нет пустых уровней, то двигаем весь массив вниз
				
				for (j = domLevelsCount; j > 0; j--)
				{
					_d.bidPrice[j] = _d.bidPrice[j-1];
					_d.bidSize[j] = _d.bidSize[j-1];
					_d.bidOrders[j] = _d.bidOrders[j-1];
				}		
				
				//и вставляем в первую позицию новые значения
				_d.bidPrice[0] = price;
				_d.bidSize[0] = limits;
				if (traders) _d.bidOrders[0] = traders;
				_d.eventBidLevel = 1 << 0;
				_d.deltaBidLimitSize[0] = limits;

				//обнуляем значение лимитов и ордеров на 11 уровне в стакане, т.к. оно может измениться в любой момент, а мы этого не увидим,
				//поэтому эти значения нам не нужны
				_d.bidSize[domLevelsCount] = 0;
				_d.bidOrders[domLevelsCount] = 0;
			}
			//если цена меньше чем цена m_domLevelsCount
			else if (price < _d.bidPrice[domLevelsCount]) 
			{
				for (j = domLevelsCount; j >= 0; j--)
				{
					if (!_d.bidSize[j]) { zeroFlag = true; zeroLvl = j; }
				}
				
				if (zeroFlag) j = zeroLvl;				//и есть пустые уровни, то двигаем массив вверх до цены с нулевым объемом
				else 
					j = 0;
				
				if (j > 0)
				{
					for (; j < domLevelsCount; j++)
					{
						_d.bidPrice[j] = _d.bidPrice[j+1];
						_d.bidSize[j] = _d.bidSize[j+1];
						_d.bidOrders[j] = _d.bidOrders[j+1];
					}
					j = domLevelsCount;
				
					//и вставляем в последнюю позицию новые значения
					_d.bidPrice[j] = price;
					_d.bidSize[j] = limits;
					if (traders) _d.bidOrders[j] = traders;
					_d.eventBidLevel = 1 << j;
				}
			
			} else if (price < _d.bidPrice[0] && price > _d.bidPrice[domLevelsCount]) { //если цена находится внутри стакана по бидам

				int index;
				//ищем индекс места в которое будем вставлять новое значение
				for (j = 1; j <= domLevelsCount; j++)
				{
					if (price >= _d.bidPrice[j] && price <= _d.bidPrice[j-1]) 
					{	index = j;	break;	}
				}
				//если у одного из значений кол-во лимитов равно нулю, то на его место пишем
				if (_d.bidSize[index] == 0) {
					//вставляем на место индекса текущее значение
					_d.bidPrice[index] = price;
					_d.bidSize[index] = limits;
					if (traders) _d.bidOrders[index] = traders;
					_d.eventBidLevel = 1 << index;
					if (index != domLevelsCount-1)
						_d.deltaBidLimitSize[index] = limits;
				} else if (_d.bidSize[index-1] == 0) {
					//вставляем на место индекса текущее значение
					_d.bidPrice[index-1] = price;
					_d.bidSize[index-1] = limits;
					if (traders) _d.bidOrders[index-1] = traders;
					_d.eventBidLevel = 1 << (index-1);
					_d.deltaBidLimitSize[index-1] = limits;
				
				} else {		//если нулей нет, то сдвигаем все вверх начиная с уровня на который указывается индекс
					
					//сдвигаем все вниз начиная с уровня на который указывается индекс
					for (j = domLevelsCount; j > index; j--)
					{
						_d.bidPrice[j] = _d.bidPrice[j-1];
						_d.bidSize[j] = _d.bidSize[j-1];
						_d.bidOrders[j] = _d.bidOrders[j-1];
					}
					//вставляем на место индекса текущее значение
					_d.bidPrice[index] = price;
					_d.bidSize[index] = limits;
					if (traders) _d.bidOrders[index] = traders;
					_d.eventBidLevel = 1 << index;
					_d.deltaBidLimitSize[index] = limits;
				}
			}
		}
	}
	//если это значение не бида и не аска
	else
	{
		//перезаписываем таймстемп
		//_d.Data.Data.timeStamp = Data[Data.size()-1].Data.Data.timeStamp;
	}
}

int RDOMTicker::Get_deltaBidLimitSum(size_t i) const
{
	int sum = 0;

	for (int j = domLevelsCount-1; j >= 0; --j)
		sum += Data[i].deltaBidLimitSize[j];

	return sum;
}
int RDOMTicker::Get_deltaAskLimitSum(size_t i) const
{
	int sum = 0;

	for (int j = domLevelsCount-1; j >= 0; --j)
		sum += Data[i].deltaAskLimitSize[j];

	return sum;
}


void CQGTradeTicker::TradePrint(void* p)
{
	CQG_INSTRUMENT_TRADE it = *(CQG_INSTRUMENT_TRADE*)p;

	CQGTRADE _d;
	ZeroData(_d);

	//таймстемп
	_d.Data.hour = it.timestamp.wHour;
	_d.Data.minute = it.timestamp.wMinute;
	_d.Data.second = it.timestamp.wSecond;

	//микросекунды
	_d.microseconds = (UINT)it.timestamp.wMilliseconds*1000;

	if (counter >= 100 && (hour != _d.Data.hour || minute != _d.Data.minute || second != _d.Data.second)) 
	{		
		hour = _d.Data.hour;
		minute = _d.Data.minute;
		second = _d.Data.second;
		
		_d.yAxis = true;
		counter = 0;
	}
	++counter;

	//дельта между текущим таймстемпом и предыдущим
	_d.microDelta = 0;
	if (Data.size())
		_d.microDelta = MicroSpeed(_d.Data.hour, _d.Data.minute, _d.Data.second, _d.microseconds, 
							Data[Data.size()-1].Data.hour, Data[Data.size()-1].Data.minute, Data[Data.size()-1].Data.second, Data[Data.size()-1].microseconds);
	
	//цена и объем
	_d.Data.tradePrice.SetPrice(it.price, GetPrecision());
	_d.Data.tradeSize = it.volume;

	//сторона агрессора
	if (lastAsk.IsPrice() && lastBid.IsPrice())
	{
		if (_d.Data.tradePrice >= lastAsk)
			_d.agressorSide = 'B';
		else if (_d.Data.tradePrice <= lastBid)
			_d.agressorSide = 'S';
		else if (_d.Data.tradePrice < lastAsk && _d.Data.tradePrice > lastBid)
			_d.agressorSide = '?';
	}
	else
		_d.agressorSide = '?';

	TickerData<CQGTRADE>::push_back(_d);
}
void CQGDOMTicker::TradePrint(void* p)
{
	CQG_INSTRUMENT_TRADE it = *(CQG_INSTRUMENT_TRADE*)p;

	CQGDOM _d;
	ZeroData(_d, 11);

	//таймстемп
	_d.Data.Data.hour = it.timestamp.wHour;
	_d.Data.Data.minute = it.timestamp.wMinute;
	_d.Data.Data.second = it.timestamp.wSecond;

	//микросекунды
	_d.Data.microseconds = (UINT)it.timestamp.wMilliseconds*1000;

	if (counter >= 100 && (hour != _d.Data.Data.hour || minute != _d.Data.Data.minute || second != _d.Data.Data.second)) 
	{		
		hour = _d.Data.Data.hour;
		minute = _d.Data.Data.minute;
		second = _d.Data.Data.second;
		
		_d.Data.yAxis = true;
		counter = 0;
	}
	++counter;

	//дельта между текущим таймстемпом и предыдущим
	_d.Data.microDelta = 0;
	if (Data.size())
		_d.Data.microDelta = MicroSpeed(_d.Data.Data.hour, _d.Data.Data.minute, _d.Data.Data.second, _d.Data.microseconds, 
							Data[Data.size()-1].Data.Data.hour, Data[Data.size()-1].Data.Data.minute, Data[Data.size()-1].Data.Data.second, Data[Data.size()-1].Data.microseconds);
	
	//цена и объем
	_d.Data.Data.tradePrice.SetPrice(it.price, GetPrecision());
	_d.Data.Data.tradeSize = it.volume;

	//сторона агрессора
	if (lastAsk.IsPrice() && lastBid.IsPrice())
	{
		if (_d.Data.Data.tradePrice >= lastAsk)
			_d.Data.agressorSide = 'B';
		else if (_d.Data.Data.tradePrice <= lastBid)
			_d.Data.agressorSide = 'S';
		else if (_d.Data.Data.tradePrice < lastAsk && _d.Data.Data.tradePrice > lastBid)
			_d.Data.agressorSide = '?';
	}
	else
		_d.Data.agressorSide = '?';

	//перезаписываем предыдущие значения
	if (Data.size())
	{
		WritePreviousBidValue(_d);
		WritePreviousAskValue(_d);
	}
	
	//устанавливаем лимиты напротив трейдов
	if (Data.size())
		TradesToLimits(_d);

	//определяем максимальные значения в стакане (для хитмепа)
	for (int x = domLevelsCount-1; x >= 0; --x)
	{
		if (_d.maxDomLimit < _d.askSize[x])
			_d.maxDomLimit = _d.askSize[x];
		if (_d.maxDomLimit < _d.bidSize[x])
			_d.maxDomLimit = _d.bidSize[x];
		if (_d.minDomLimit > _d.askSize[x])
			_d.minDomLimit = _d.askSize[x];
		if (_d.minDomLimit > _d.bidSize[x])
			_d.minDomLimit = _d.bidSize[x];
	}

	TickerData<CQGDOM>::push_back(_d);
}
void CQGDOMTicker::GetDOM(void* d)
{
	CQG_INSTRUMENT_DOM iDOM = *(CQG_INSTRUMENT_DOM*)d;

	CQGDOM _d;
	ZeroData(_d, 11);
	size_t lastIndx = Data.size();
	if (lastIndx)
		--lastIndx;

	//таймстемп
	_d.Data.Data.hour = iDOM.timestamp.wHour;
	_d.Data.Data.minute = iDOM.timestamp.wMinute;
	_d.Data.Data.second = iDOM.timestamp.wSecond;

	//микросекунды
	_d.Data.microseconds = (UINT)iDOM.timestamp.wMilliseconds*1000;

	//дельта между текущим таймстемпом и предыдущим
	_d.Data.microDelta = 0;
	if (lastIndx)
		_d.Data.microDelta = MicroSpeed(_d.Data.Data.hour, _d.Data.Data.minute, _d.Data.Data.second, _d.Data.microseconds, 
							Data[lastIndx].Data.Data.hour, Data[lastIndx].Data.Data.minute, Data[lastIndx].Data.Data.second, Data[lastIndx].Data.microseconds);

	if (counter >= 100 && (hour != _d.Data.Data.hour || minute != _d.Data.Data.minute || second != _d.Data.Data.second)) 
	{		
		hour = _d.Data.Data.hour;
		minute = _d.Data.Data.minute;
		second = _d.Data.Data.second;
		
		_d.Data.yAxis = true;
		counter = 0;
	}
	++counter;
	
	//цена и объем
	for (int i = domLevelsCount-1; i >= 0; --i)
	{
		_d.askPrice[i].SetPrice(iDOM.askPrice[i], GetPrecision());
		_d.bidPrice[i].SetPrice(iDOM.bidPrice[i], GetPrecision());
		_d.askSize[i] = iDOM.askSize[i];
		_d.bidSize[i] = iDOM.bidSize[i];
	}

	if (lastIndx && Data[lastIndx].Data.Data.tradeSize && lastIndx != lastSyncTradeIndx) //в предыдущем значении был трейд значит надо сместить лимиты
	{
		while (lastIndx && Data[lastIndx].Data.Data.tradeSize)
			lastIndx--;

		if (!lastIndx)
		{
			lastIndx = Data.size()-1;
			goto metka;
		}

		//меняем таймштемп
		_d.Data.Data.hour = Data[lastIndx].Data.Data.hour;
		_d.Data.Data.minute = Data[lastIndx].Data.Data.minute;
		_d.Data.Data.second = Data[lastIndx].Data.Data.second;
		_d.Data.microseconds = Data[lastIndx].Data.microseconds;
		_d.Data.microDelta = 0;
	}

metka:
		
	//ищем уровень на котором произошло изменение и устанавливаем размеры ордера, сторону агрессора и флаги
	int changeCount = 0;
	if (lastIndx)
	{
		for (int i = domLevelsCount-1; i >= 0; --i)
		{
			if (_d.askPrice[i] == Data[lastIndx].askPrice[i] && _d.askSize[i] != Data[lastIndx].askSize[i])
			{
				++changeCount;
				_d.eventAskLevel |= 1 << i;
				bool cqg_lastLevel_BUG = i == (domLevelsCount-1) && !_d.askSize[i];
				if (!cqg_lastLevel_BUG)
					_d.deltaAskLimitSize[i] = (int)_d.askSize[i] - (int)Data[lastIndx].askSize[i];
				if (_d.Data.agressorSide == 'N')
					_d.Data.agressorSide = 'a';
				else if (_d.Data.agressorSide != 'a')
					_d.Data.agressorSide = 'x'; // 'x' - для изменений сразу на бидах и на оферах стакана за одно событие
			}

			if (_d.bidPrice[i] == Data[lastIndx].bidPrice[i] && _d.bidSize[i] != Data[lastIndx].bidSize[i])
			{
				++changeCount;
				_d.eventBidLevel |= 1 << i;
				bool cqg_lastLevel_BUG = i == (domLevelsCount-1) && !_d.bidSize[i];
				if (!cqg_lastLevel_BUG)
					_d.deltaBidLimitSize[i] = (int)_d.bidSize[i] - (int)Data[lastIndx].bidSize[i];
				if (_d.Data.agressorSide == 'N')
					_d.Data.agressorSide = 'b';
				else if (_d.Data.agressorSide != 'b')
					_d.Data.agressorSide = 'x';
			}
		}

		_d.orderType = changeCount;
	}

	//определяем максимальные значения в стакане (для хитмепа)
	for (int x = domLevelsCount-1; x >= 0; --x)
	{
		if (_d.maxDomLimit < _d.askSize[x])
			_d.maxDomLimit = _d.askSize[x];
		if (_d.maxDomLimit < _d.bidSize[x])
			_d.maxDomLimit = _d.bidSize[x];
		if (_d.minDomLimit > _d.askSize[x])
			_d.minDomLimit = _d.askSize[x];
		if (_d.minDomLimit > _d.bidSize[x])
			_d.minDomLimit = _d.bidSize[x];
	}

	if (lastIndx && lastIndx != Data.size()-1)
	{
		insert(++lastIndx, _d);

		CQGDOM _dRecount;

		while (lastIndx < Data.size()-1)
		{
			_dRecount = Data[lastIndx++];
			_dRecount.Data.Data.hour = Data[lastIndx].Data.Data.hour;
			_dRecount.Data.Data.minute = Data[lastIndx].Data.Data.minute;
			_dRecount.Data.Data.second = Data[lastIndx].Data.Data.second;
			_dRecount.Data.microseconds = Data[lastIndx].Data.microseconds;
			_dRecount.Data = Data[lastIndx].Data;

			_dRecount.Data.microDelta = MicroSpeed(_dRecount.Data.Data.hour, _dRecount.Data.Data.minute, _dRecount.Data.Data.second, _dRecount.Data.microseconds, 
							Data[lastIndx].Data.Data.hour, Data[lastIndx].Data.Data.minute, Data[lastIndx].Data.Data.second, Data[lastIndx].Data.microseconds);

			TradesToLimits(_dRecount);

			//определяем максимальные значения в стакане (для хитмепа)
			for (int x = domLevelsCount-1; x >= 0; --x)
			{
				if (_dRecount.maxDomLimit < _dRecount.askSize[x])
					_dRecount.maxDomLimit = _dRecount.askSize[x];
				if (_dRecount.maxDomLimit < _dRecount.bidSize[x])
					_dRecount.maxDomLimit = _dRecount.bidSize[x];
				if (_dRecount.minDomLimit > _dRecount.askSize[x])
					_dRecount.minDomLimit = _dRecount.askSize[x];
				if (_dRecount.minDomLimit > _dRecount.bidSize[x])
					_dRecount.minDomLimit = _dRecount.bidSize[x];
			}

			Data[lastIndx] = _dRecount;
		}	

		lastSyncTradeIndx = lastIndx;

		return;
	}	

	TickerData<CQGDOM>::push_back(_d);
}

int CQGDOMTicker::Get_deltaBidLimitSum(size_t i) const
{
	int sum = 0;

	for (int j = domLevelsCount-1; j >= 0; --j)
		sum += Data[i].deltaBidLimitSize[j];

	return sum;
}
	
int CQGDOMTicker::Get_deltaAskLimitSum(size_t i) const
{
	int sum = 0;

	for (int j = domLevelsCount-1; j >= 0; --j)
		sum += Data[i].deltaAskLimitSize[j];

	return sum;
}


void CQGTradeTicker::TradePrintNinja(void* p)
{
	Trade_DATA it = *(Trade_DATA*)p;

	CQGTRADE _d;
	ZeroData(_d);

	//таймстемп
	_d.Data.hour = it.hour;
	_d.Data.minute = it.minute;
	_d.Data.second = it.second;

	//микросекунды
	_d.microseconds = (UINT)it.millisecond*1000;

	if (counter >= 100 && (hour != _d.Data.hour || minute != _d.Data.minute || second != _d.Data.second)) 
	{		
		hour = _d.Data.hour;
		minute = _d.Data.minute;
		second = _d.Data.second;
		
		_d.yAxis = true;
		counter = 0;
	}
	++counter;

	//дельта между текущим таймстемпом и предыдущим
	_d.microDelta = 0;
	if (Data.size())
		_d.microDelta = MicroSpeed(_d.Data.hour, _d.Data.minute, _d.Data.second, _d.microseconds, 
							Data[Data.size()-1].Data.hour, Data[Data.size()-1].Data.minute, Data[Data.size()-1].Data.second, Data[Data.size()-1].microseconds);
	
	//цена и объем
	_d.Data.tradePrice.SetPrice(it.Price, GetPrecision());
	_d.Data.tradeSize = it.Volume;

	//сторона агрессора
	if (lastAsk.IsPrice() && lastBid.IsPrice())
	{
		if (_d.Data.tradePrice >= lastAsk)
			_d.agressorSide = 'B';
		else if (_d.Data.tradePrice <= lastBid)
			_d.agressorSide = 'S';
		else if (_d.Data.tradePrice < lastAsk && _d.Data.tradePrice > lastBid)
			_d.agressorSide = '?';
	}
	else
		_d.agressorSide = '?';

	TickerData<CQGTRADE>::push_back(_d);
}
void CQGDOMTicker::TradePrintNinja(void* p)
{
	Trade_DATA it = *(Trade_DATA*)p;

	CQGDOM _d;
	ZeroData(_d, 11);

	//таймстемп
	_d.Data.Data.hour = it.hour;
	_d.Data.Data.minute = it.minute;
	_d.Data.Data.second = it.second;

	//микросекунды
	_d.Data.microseconds = (UINT)it.millisecond*1000;

	if (counter >= 100 && (hour != _d.Data.Data.hour || minute != _d.Data.Data.minute || second != _d.Data.Data.second)) 
	{		
		hour = _d.Data.Data.hour;
		minute = _d.Data.Data.minute;
		second = _d.Data.Data.second;
		
		_d.Data.yAxis = true;
		counter = 0;
	}
	++counter;

	//дельта между текущим таймстемпом и предыдущим
	_d.Data.microDelta = 0;
	if (Data.size())
		_d.Data.microDelta = MicroSpeed(_d.Data.Data.hour, _d.Data.Data.minute, _d.Data.Data.second, _d.Data.microseconds, 
							Data[Data.size()-1].Data.Data.hour, Data[Data.size()-1].Data.Data.minute, Data[Data.size()-1].Data.Data.second, Data[Data.size()-1].Data.microseconds);
	
	//цена и объем
	_d.Data.Data.tradePrice.SetPrice(it.Price, GetPrecision());
	_d.Data.Data.tradeSize = it.Volume;

	//сторона агрессора
	if (lastAsk.IsPrice() && lastBid.IsPrice())
	{
		if (_d.Data.Data.tradePrice >= lastAsk)
			_d.Data.agressorSide = 'B';
		else if (_d.Data.Data.tradePrice <= lastBid)
			_d.Data.agressorSide = 'S';
		else if (_d.Data.Data.tradePrice < lastAsk && _d.Data.Data.tradePrice > lastBid)
			_d.Data.agressorSide = '?';
	}
	else
		_d.Data.agressorSide = '?';

	//перезаписываем предыдущие значения
	if (Data.size())
	{
		WritePreviousBidValue(_d);
		WritePreviousAskValue(_d);
	}
	
	//устанавливаем лимиты напротив трейдов
	if (Data.size())
		TradesToLimits(_d);

	//определяем максимальные значения в стакане (для хитмепа)
	for (int x = domLevelsCount-1; x >= 0; --x)
	{
		if (_d.maxDomLimit < _d.askSize[x])
			_d.maxDomLimit = _d.askSize[x];
		if (_d.maxDomLimit < _d.bidSize[x])
			_d.maxDomLimit = _d.bidSize[x];
		if (_d.minDomLimit > _d.askSize[x])
			_d.minDomLimit = _d.askSize[x];
		if (_d.minDomLimit > _d.bidSize[x])
			_d.minDomLimit = _d.bidSize[x];
	}

	TickerData<CQGDOM>::push_back(_d);
}

void CQGDOMTicker::WriteNewData(CQGDOM& _d, char Type, const Price& price, UINT limits, int nPos, wchar_t type)
{
	//флаг найденной цены
	//bool findPrice = false;

	//перезаписываем предыдущие значения
	WritePreviousBidValue(_d);
	WritePreviousAskValue(_d);

	if (Type == 'a')
	{
		//вычисляем дельту поставленного/убранного лимита если такая цена уже есть в стакане
		int j;
		for (j = 0; j < domLevelsCount; j++)
		{
			if (price == _d.askPrice[j])
			{
				if (j < domLevelsCount-1 || (j == domLevelsCount-1 && Data[Data.size()-1].askSize[j]))
					_d.deltaAskLimitSize[j] = limits - Data[Data.size()-1].askSize[j];	//размер поставленного/убранного лимита
				//findPrice = true;
				break;	//принудительно заканчиваем цикл
			}
		}

		switch(type)
		{
		case 'i':
			for (j = domLevelsCount-1; j > nPos; --j)
			{
				_d.askPrice[j] = _d.askPrice[j-1];
				_d.askSize[j] = _d.askSize[j-1];
			}
		case 'u':
			_d.askPrice[nPos] = price;
			_d.askSize[nPos] = limits;
			_d.eventAskLevel = 1 << nPos;	
			break;
		case 'r':
			for (j = nPos; j < domLevelsCount-1; ++j)
			{
				_d.askPrice[j] = _d.askPrice[j+1];
				_d.askSize[j] = _d.askSize[j+1];
			}
			break;
		}

		/*if (type == 'i')
		{
			for (j = domLevelsCount-1; j > nPos; --j)
			{
				_d.askPrice[j] = _d.askPrice[j-1];
				_d.askSize[j] = _d.askSize[j-1];
			}
		}
		else if (type == 'r')
		{
			for (j = nPos; j < domLevelsCount-1; ++j)
			{
				_d.askPrice[j] = _d.askPrice[j+1];
				_d.askSize[j] = _d.askSize[j+1];
			}
			return;
		}

		_d.askPrice[nPos] = price;
		_d.askSize[nPos] = limits;
		_d.eventAskLevel = 1 << nPos;	*/
	}
	else if (Type == 'b')
	{
		//вычисляем дельту поставленного/убранного лимита если такая цена уже есть в стакане
		int j; 
		for (j = 0; j < domLevelsCount; j++)
		{
			if (price == _d.bidPrice[j])
			{
				if (j < domLevelsCount-1 || (j == domLevelsCount-1 && Data[Data.size()-1].bidSize[j]))
					_d.deltaBidLimitSize[j] =  limits - Data[Data.size()-1].bidSize[j];	//размер поставленного/убранного лимита
				//findPrice = true;
				break;	//принудительно заканчиваем цикл
			}
		}

		switch(type)
		{
		case 'i':
			for (j = domLevelsCount-1; j > nPos; --j)
			{
				_d.bidPrice[j] = _d.bidPrice[j-1];
				_d.bidSize[j] = _d.bidSize[j-1];
			}
		case 'u':
			_d.bidPrice[nPos] = price;
			_d.bidSize[nPos] = limits;
			_d.eventBidLevel = 1 << nPos;	
			break;
		case 'r':
			for (j = nPos; j < domLevelsCount-1; ++j)
			{
				_d.bidPrice[j] = _d.bidPrice[j+1];
				_d.bidSize[j] = _d.bidSize[j+1];
			}
			break;
		}

		/*if (type == 'i')
		{
			for (j = domLevelsCount-1; j > nPos; --j)
			{
				_d.bidPrice[j] = _d.bidPrice[j-1];
				_d.bidSize[j] = _d.bidSize[j-1];
			}
		}
		else if (type == 'r')
		{
			for (j = nPos; j < domLevelsCount-1; ++j)
			{
				_d.bidPrice[j] = _d.bidPrice[j+1];
				_d.bidSize[j] = _d.bidSize[j+1];
			}
			return;
		}

		_d.bidPrice[nPos] = price;
		_d.bidSize[nPos] = limits;
		_d.eventBidLevel = 1 << nPos;*/	
	}
	//если это значение не бида и не аска
	else
	{
		//перезаписываем таймстемп
		//_d.Data.Data.timeStamp = Data[Data.size()-1].Data.Data.timeStamp;
	}
}
void CQGDOMTicker::GetDOMNinja(void* d)
{
	DOM_DATA iDOM = *(DOM_DATA*)d;

	CQGDOM _d;
	ZeroData(_d, 11);

	size_t lastIndx = Data.size()-1;

	//таймстемп
	_d.Data.Data.hour = iDOM.hour;
	_d.Data.Data.minute = iDOM.minute;
	_d.Data.Data.second = iDOM.second;

	//микросекунды
	_d.Data.microseconds = (UINT)iDOM.millisecond*1000;

	//дельта между текущим таймстемпом и предыдущим
	_d.Data.microDelta = 0;
	if (Data.size())
		_d.Data.microDelta = MicroSpeed(_d.Data.Data.hour, _d.Data.Data.minute, _d.Data.Data.second, _d.Data.microseconds, 
							Data[lastIndx].Data.Data.hour, Data[lastIndx].Data.Data.minute, Data[lastIndx].Data.Data.second, Data[lastIndx].Data.microseconds);

	if (counter >= 100 && (hour != _d.Data.Data.hour || minute != _d.Data.Data.minute || second != _d.Data.Data.second)) 
	{		
		hour = _d.Data.Data.hour;
		minute = _d.Data.Data.minute;
		second = _d.Data.Data.second;
		
		_d.Data.yAxis = true;
		counter = 0;
	}
	++counter;

	_d.Data.agressorSide = iDOM.agressor;
	
	WriteNewData(_d, iDOM.agressor, Price(iDOM.Price, GetPrecision()), iDOM.Volume, iDOM.pos, iDOM.type);

	//определяем максимальные значения в стакане (для хитмепа)
	for (int x = domLevelsCount-1; x >= 0; --x)
	{
		if (_d.maxDomLimit < _d.askSize[x])
			_d.maxDomLimit = _d.askSize[x];
		if (_d.maxDomLimit < _d.bidSize[x])
			_d.maxDomLimit = _d.bidSize[x];
		if (_d.minDomLimit > _d.askSize[x])
			_d.minDomLimit = _d.askSize[x];
		if (_d.minDomLimit > _d.bidSize[x])
			_d.minDomLimit = _d.bidSize[x];
	}

	TickerData<CQGDOM>::push_back(_d);
}