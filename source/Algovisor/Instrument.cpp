#include "stdafx.h"

#include "Auxiliary_methods.h"
#include "DEFINE.h"
#include "CQGDialog.h"
#include "NinjaDialog.h"

#include "Instrument.h"

//�������������� �������
//��� ������������� - ��� ����� ������� ���������
UINT MicroSpeed(BYTE hour, BYTE minute, BYTE second, UINT microsecond,  BYTE _hour, BYTE _minute, BYTE _second, UINT _microsecond)
{
	//����� ������������ ���������
	if (microsecond == _microsecond)
		return 0;
	//������ ����� ������ � �������
	else if (minute == _minute && second == _second)
		return (microsecond - _microsecond);
	//������ ����� ������ � ������� ������
	else if (minute == _minute && (second - _second) == 1)
		return (999999 - _microsecond + microsecond);
	//� ������� ������ ������� ������
	else if ((minute - _minute) == 1 && _second == 59 && second == 0)
		return (999999 - _microsecond + microsecond);
	//��� ��������� ������ �������
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
		else if (!price.GetZnak())									//���� ���� �������������
		{
			if (!price.GetFrac())
			{
				price.DecInt();
				price.SetFrac(divider-MinTickInc);
			}
			else 
				price.SetFrac(price.GetFrac()-MinTickInc);
		}
		else														//���� ���� �������������
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

	//���������
	CTime time(pInfo->iSsboe);
	_d.Data.hour = (BYTE)time.GetHour();
	_d.Data.minute = (BYTE)time.GetMinute();
	_d.Data.second = (BYTE)time.GetSecond();

	//������������
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

	//���������
	//CTime s_time(pInfo->iSourceSsboe);
	//time.GetAsSystemTime(_d.Data.s_timeStamp);

	//������������
	//_d.s_microseconds = pInfo->iSourceUsecs;

	//������ ����� ������� ����������� � ����������
	_d.microDelta = 0;
	if (pInfo->iType != RApi::MD_HISTORY_CB && Data.size())
		_d.microDelta = MicroSpeed(_d.Data.hour, _d.Data.minute, _d.Data.second, _d.microseconds, 
							Data[Data.size()-1].Data.hour, Data[Data.size()-1].Data.minute, Data[Data.size()-1].Data.second, Data[Data.size()-1].microseconds);
	else if (HData.size())
		HData[HData.size()-1].microDelta = MicroSpeed(HData[HData.size()-1].Data.hour, HData[HData.size()-1].Data.minute, HData[HData.size()-1].Data.second, HData[HData.size()-1].microseconds,
													_d.Data.hour, _d.Data.minute, _d.Data.second, _d.microseconds);
	
	//���� � �����
	_d.Data.tradePrice.SetPrice(pInfo->dPrice, precision);
	_d.Data.tradeSize = pInfo->iSize;

	//������� ���������
	if (pInfo->sAggressorSide.iDataLen)
		_d.agressorSide = pInfo->sAggressorSide.pData[0];
	else _d.agressorSide = '?';
	_d.impliedSide = 'N';

	//��������
	if (pInfo->sCondition.iDataLen)
		//_d.condition.assign(pInfo->sCondition.pData, pInfo->sCondition.iDataLen);
		_d.condition = 1;

	//������
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
//	//���������
//	CTime time(pInfo->iSsboe);
//	_d.Data.Data.hour = (BYTE)time.GetHour();
//	_d.Data.Data.minute = (BYTE)time.GetMinute();
//	_d.Data.Data.second = (BYTE)time.GetSecond();
//
//	//������������
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
//	//������ ����� ������� ���������� � ����������
//	_d.Data.microDelta = 0;
//	if (pInfo->iType != RApi::MD_HISTORY_CB && Data.size())
//		_d.Data.microDelta = MicroSpeed(_d.Data.Data.hour, _d.Data.Data.minute, _d.Data.Data.second, _d.Data.microseconds, 
//							Data[Data.size()-1].Data.Data.hour, Data[Data.size()-1].Data.Data.minute, Data[Data.size()-1].Data.Data.second, Data[Data.size()-1].Data.microseconds);
//	else if (HData.size())
//		HData[HData.size()-1].Data.microDelta = MicroSpeed(HData[HData.size()-1].Data.Data.hour, HData[HData.size()-1].Data.Data.minute, HData[HData.size()-1].Data.Data.second, HData[HData.size()-1].Data.microseconds,
//													_d.Data.Data.hour, _d.Data.Data.minute, _d.Data.Data.second, _d.Data.microseconds);
//
//	//���� � �����
//	_d.Data.Data.tradePrice.SetPrice(pInfo->dPrice, precision);
//	_d.Data.Data.tradeSize = pInfo->iSize;
//
//	//������� ���������
//	if (pInfo->sAggressorSide.iDataLen)
//		_d.Data.agressorSide = pInfo->sAggressorSide.pData[0];
//	else {
//		_d.Data.agressorSide = '?';
//		//���������� ������� �������-������
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
//	//����������� ���������� �������� �������
//	if (Data.size() && pInfo->iType != RApi::MD_HISTORY_CB) {
//		_d.askPrice = Data[Data.size()-1].askPrice;
//		_d.askSize = Data[Data.size()-1].askSize;
//		_d.askOrders = Data[Data.size()-1].askOrders;
//		_d.bidPrice = Data[Data.size()-1].bidPrice;
//		_d.bidSize = Data[Data.size()-1].bidSize;
//		_d.bidOrders = Data[Data.size()-1].bidOrders;
//
//		//������������� ������ �������� �������
//		if ((_d.Data.agressorSide == 'S' || _d.Data.impliedSide == 'S') && _d.Data.Data.tradePrice == _d.bidPrice) { //����� �� ����
//			//��������� ������ �� ����� ��� ������ �������� ������
//			if (_d.bidSize > _d.Data.Data.tradeSize) {
//				_d.tradeSizeQuant = _d.bidSize;
//				_d.bidSize = _d.bidSize - _d.Data.Data.tradeSize;
//				_d.bidOrders = _d.bidOrders - 1;
//			} else {	//���� ������ �� ����� ����� �� ���������� ������ ��� ������� ������� ������� ���� ������ ������ ��� ����� ������
//				_d.bidSize = 0;
//				_d.bidOrders = 0;
//				_d.tradeSizeQuant = _d.Data.Data.tradeSize;
//			}		
//		} else if ((_d.Data.agressorSide == 'B' || _d.Data.impliedSide == 'B') && _d.Data.Data.tradePrice == _d.askPrice) {	//����� �� ����
//			//��������� ������ �� ����� ��� ������ �������� ������
//			if (_d.askSize > _d.Data.Data.tradeSize) {
//				_d.tradeSizeQuant = _d.askSize;
//				_d.askSize = _d.askSize - _d.Data.Data.tradeSize;
//				_d.askOrders = _d.askOrders - 1;
//			} else {	//���� ������ �� ����� ����� �� ���������� ������ ��� ������� ������� ������� ���� ������ ������ ��� ����� ������
//				_d.askSize = 0;
//				_d.askOrders = 0;
//				_d.tradeSizeQuant = _d.Data.Data.tradeSize;
//			}		
//		} else if (_d.Data.impliedSide == 'M')	//����� ������ ������	
//			_d.tradeSizeQuant = _d.Data.Data.tradeSize;
//	}
//
//	//��������
//	if (pInfo->sCondition.iDataLen)
//		//_d.Data.condition.assign(pInfo->sCondition.pData, pInfo->sCondition.iDataLen);
//		_d.Data.condition = 1;
//
//	//������
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

	//���������
	CTime time(pInfo->iSsboe);
	_d.Data.Data.hour = (BYTE)time.GetHour();
	_d.Data.Data.minute = (BYTE)time.GetMinute();
	_d.Data.Data.second = (BYTE)time.GetSecond();

	//������������
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

	//������ ����� ������� ���������� � ����������
	_d.Data.microDelta = 0;
	if (pInfo->iType != RApi::MD_HISTORY_CB && Data.size())
		_d.Data.microDelta = MicroSpeed(_d.Data.Data.hour, _d.Data.Data.minute, _d.Data.Data.second, _d.Data.microseconds, 
							Data[Data.size()-1].Data.Data.hour, Data[Data.size()-1].Data.Data.minute, Data[Data.size()-1].Data.Data.second, Data[Data.size()-1].Data.microseconds);
	else if (HData.size())
		HData[HData.size()-1].Data.microDelta = MicroSpeed(HData[HData.size()-1].Data.Data.hour, HData[HData.size()-1].Data.Data.minute, HData[HData.size()-1].Data.Data.second, HData[HData.size()-1].Data.microseconds,
													_d.Data.Data.hour, _d.Data.Data.minute, _d.Data.Data.second, _d.Data.microseconds);

	//���� � �����
	_d.Data.Data.tradePrice.SetPrice(pInfo->dPrice, precision);
	_d.Data.Data.tradeSize = pInfo->iSize;

	//������� ���������
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

	//��������
	if (pInfo->sCondition.iDataLen)
		//_d.Data.condition.assign(pInfo->sCondition.pData, pInfo->sCondition.iDataLen);
		_d.Data.condition = 1;

	//������
	if (pInfo->bVolumeBoughtFlag)
		_d.Data.buyVolume = pInfo->iVolumeBought;
	if (pInfo->bVolumeSoldFlag)
		_d.Data.sellVolume = pInfo->iVolumeSold;

	//�������������� ���������� ��������
	if (pInfo->iType != RApi::MD_HISTORY_CB)
	{
		WritePreviousBidValue(_d);
		WritePreviousAskValue(_d);
	}
	
	//������������� ������ �������� �������
	if (Data.size() && pInfo->iType != RApi::MD_HISTORY_CB)
		TradesToLimits(_d);

	//���������� ������������ �������� � ������� (��� �������)
	for (int x = domLevelsCount; x >= 0; --x)
	{
		if (_d.maxDomLimit < _d.askSize[x])
			_d.maxDomLimit = _d.askSize[x];
		if (_d.maxDomLimit < _d.bidSize[x])
			_d.maxDomLimit = _d.bidSize[x];
		if (_d.askSize[x] > 0 && _d.minDomLimit > _d.askSize[x])	//�������� ">0" ����� ����� ����� �� ������ �� ������� ����� ����� ����
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
//	//���������
//	CTime time(pBid->iSsboe);
//	_d.Data.Data.hour = (BYTE)time.GetHour();
//	_d.Data.Data.minute = (BYTE)time.GetMinute();
//	_d.Data.Data.second = (BYTE)time.GetSecond();
//
//	//������������
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
//	//������ ����� ������� ���������� � ����������
//	_d.Data.microDelta = 0;
//	if (Data.size())
//		_d.Data.microDelta = MicroSpeed(_d.Data.Data.hour, _d.Data.Data.minute, _d.Data.Data.second, _d.Data.microseconds, 
//									Data[Data.size()-1].Data.Data.hour, Data[Data.size()-1].Data.Data.minute, Data[Data.size()-1].Data.Data.second, Data[Data.size()-1].Data.microseconds);
//
//	//���� � ����� ����
//	_d.bidPrice.SetPrice(pBid->dPrice, precision);
//	_d.bidSize = pBid->iSize;
//	_d.bidOrders = pBid->iNumOrders;
//	//���� � ����� ����
//	if (Data.size()) {
//		_d.askPrice = Data[Data.size()-1].askPrice;
//		_d.askSize = Data[Data.size()-1].askSize;
//		_d.askOrders = Data[Data.size()-1].askOrders;
//	}
//
//	//������� ���������
//	_d.Data.agressorSide = 'b';
//	_d.Data.impliedSide = 'N';
//
//	//����������� ������ ���������
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
//	//���������
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
//	//������������
//	_d.Data.microseconds = pAsk->iUsecs;
//
//	//������ ����� ������� ���������� � ����������
//	_d.Data.microDelta = 0;
//	if (Data.size())
//		_d.Data.microDelta = MicroSpeed(_d.Data.Data.hour, _d.Data.Data.minute, _d.Data.Data.second, _d.Data.microseconds, 
//									Data[Data.size()-1].Data.Data.hour, Data[Data.size()-1].Data.Data.minute, Data[Data.size()-1].Data.Data.second, Data[Data.size()-1].Data.microseconds);
//
//	//���� � ����� ����
//	_d.askPrice.SetPrice(pAsk->dPrice, precision);
//	_d.askSize = pAsk->iSize;
//	_d.askOrders = pAsk->iNumOrders;
//	//���� � ����� ����
//	if (Data.size()) {
//		_d.bidPrice = Data[Data.size()-1].bidPrice;
//		_d.bidSize = Data[Data.size()-1].bidSize;
//		_d.bidOrders = Data[Data.size()-1].bidOrders;
//	}
//
//	//������� ���������
//	_d.Data.agressorSide = 'a';
//	_d.Data.impliedSide = 'N';
//
//	//����������� ������ ���������
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
//	//���������
//	CTime time(pBid->iSsboe);
//	_d.Data.Data.hour = (BYTE)time.GetHour();
//	_d.Data.Data.minute = (BYTE)time.GetMinute();
//	_d.Data.Data.second = (BYTE)time.GetSecond();
//
//	//������������
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
//	//������ ����� ������� ���������� � ����������
//	_d.Data.microDelta = 0;
//	if (Data.size())
//		_d.Data.microDelta = MicroSpeed(_d.Data.Data.hour, _d.Data.Data.minute, _d.Data.Data.second, _d.Data.microseconds, 
//									Data[Data.size()-1].Data.Data.hour, Data[Data.size()-1].Data.Data.minute, Data[Data.size()-1].Data.Data.second, Data[Data.size()-1].Data.microseconds);
//
//	//���� � ����� ����
//	_d.bidPrice.SetPrice(pBid->dPrice, precision);
//	_d.bidSize = pBid->iSize;
//	_d.bidOrders = pBid->iNumOrders;
//	//���� � ����� ����
//	_d.askPrice.SetPrice(pAsk->dPrice, precision);
//	_d.askSize = pAsk->iSize;
//	_d.askOrders = pAsk->iNumOrders;
//
//	//������� ���������
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
//	//����������� ������ ���������
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

	//���������� ������� ������� � �������
	if (pInfo->iAskArrayLen > 5) 
		domLevelsCount = 10;
	else domLevelsCount = 5;

	//���������
	CTime time(pInfo->iSsboe);
	_d.Data.Data.hour = (BYTE)time.GetHour();
	_d.Data.Data.minute = (BYTE)time.GetMinute();
	_d.Data.Data.second = (BYTE)time.GetSecond();

	//������������
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

	//��������� ����, ������ � ������
	for (int j = 0; j < domLevelsCount; j++)
	{
		_d.askPrice[j].SetPrice(pInfo->adAskPriceArray[j], precision);	//���� ����
		_d.askSize[j] = pInfo->aiAskSizeArray[j];						//���� ������
		_d.askOrders[j] = pInfo->aiAskNumOrdersArray[j];				//���� ������
	
		_d.bidPrice[j].SetPrice(pInfo->adBidPriceArray[j], precision);	//���� ����
		_d.bidSize[j] = pInfo->aiBidSizeArray[j];						//���� ������
		_d.bidOrders[j] = pInfo->aiBidNumOrdersArray[j];				//���� ������
	}

	//���������� ������������ �������� � ������� (��� �������)
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

	//���������
	CTime time(pInfo->iSsboe);
	_d.Data.Data.hour = (BYTE)time.GetHour();
	_d.Data.Data.minute = (BYTE)time.GetMinute();
	_d.Data.Data.second = (BYTE)time.GetSecond();

	//������������
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

	//������ ����� ������� ���������� � ����������
	_d.Data.microDelta = 0;
	if (Data.size())
		_d.Data.microDelta = MicroSpeed(_d.Data.Data.hour, _d.Data.Data.minute, _d.Data.Data.second, _d.Data.microseconds, 
									Data[Data.size()-1].Data.Data.hour, Data[Data.size()-1].Data.Data.minute, Data[Data.size()-1].Data.Data.second, Data[Data.size()-1].Data.microseconds);

	//������� ���������
	_d.Data.agressorSide = 'a';

	//����� �������� ��� � ������
	WriteNewData(_d, 'A', Price(pInfo->dPrice, precision), pInfo->iSize, pInfo->iNumOrders);

	//���������� ������������ �������� � ������� (��� �������)
	for (int x = domLevelsCount; x >= 0; --x)
	{
		if (_d.maxDomLimit < _d.askSize[x])
			_d.maxDomLimit = _d.askSize[x];
		if (_d.maxDomLimit < _d.bidSize[x])
			_d.maxDomLimit = _d.bidSize[x];
		if (_d.askSize[x] > 0 && _d.minDomLimit > _d.askSize[x])	//�������� ">0" ����� ����� ����� �� ������ �� ������� ����� ����� ����
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

	//�������� ������ � �������
	_d.updateType = pInfo->iUpdateType;
	OrderTypeSpec(_d, pInfo->iUpdateType, pInfo->iSize);

	TickerData<RITHMICDOM>::push_back(_d);
}
void RDOMTicker::BidQuote(RApi::BidInfo* pInfo)
{
	RITHMICDOM _d;
	ZeroData(_d, GetDomLevelsCount());

	//���������
	CTime time(pInfo->iSsboe);
	_d.Data.Data.hour = (BYTE)time.GetHour();
	_d.Data.Data.minute = (BYTE)time.GetMinute();
	_d.Data.Data.second = (BYTE)time.GetSecond();

	//������������
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

	//������ ����� ������� ���������� � ����������
	_d.Data.microDelta = 0;
	if (Data.size())
		_d.Data.microDelta = MicroSpeed(_d.Data.Data.hour, _d.Data.Data.minute, _d.Data.Data.second, _d.Data.microseconds, 
									Data[Data.size()-1].Data.Data.hour, Data[Data.size()-1].Data.Data.minute, Data[Data.size()-1].Data.Data.second, Data[Data.size()-1].Data.microseconds);

	//������� ���������
	_d.Data.agressorSide = 'b';

	//����� �������� ��� � ������
	WriteNewData(_d, 'B', Price(pInfo->dPrice, precision), pInfo->iSize, pInfo->iNumOrders);

	//���������� ������������ �������� � ������� (��� �������)
	for (int x = domLevelsCount; x >= 0; --x)
	{
		if (_d.maxDomLimit < _d.askSize[x])
			_d.maxDomLimit = _d.askSize[x];
		if (_d.maxDomLimit < _d.bidSize[x])
			_d.maxDomLimit = _d.bidSize[x];
		if (_d.askSize[x] > 0 && _d.minDomLimit > _d.askSize[x])	//�������� ">0" ����� ����� ����� �� ������ �� ������� ����� ����� ����
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

	//�������� ������ � �������
	_d.updateType = pInfo->iUpdateType;
	OrderTypeSpec(_d, pInfo->iUpdateType, pInfo->iSize);

	TickerData<RITHMICDOM>::push_back(_d);
}

void RDOMTicker::OrderTypeSpec(RITHMICDOM& _d, int type, int size)
{
	//��� ���������� ������
	if (type == RApi::UPDATE_TYPE_SOLO)
	{
		if (size > 0) _d.orderType = 1;
		else if (size < 0) _d.orderType = 2;
	}
	//��� ���������� ������
	else if (type == RApi::UPDATE_TYPE_END && size)
	{
		//��� ��� ����� ��������� ���� ������ ������ ����� ������
		bool thirdFlag = false;
		size_t j = Data.size()-1;
		
		if (Data[j].updateType == RApi::UPDATE_TYPE_BEGIN || Data[j].updateType == RApi::UPDATE_TYPE_MIDDLE) 
		{
			//���������� ����� ����� ��� 2-4 ��� 2-3-4 � ���� ������ ������ ����� ������
			while (Data[j].updateType != RApi::UPDATE_TYPE_BEGIN)
			{			
				//���� ��� ������ ����, ������ ������������� thirdFlag
				if (j > 0 && Data[j].updateType == RApi::UPDATE_TYPE_MIDDLE)
					thirdFlag = true;
			
				--j;
			}

			//���������� ��� ������ 234
			for (; j > 0 && j < Data.size(); j++)
			{
				if (!thirdFlag) //������ ����� 2-4
				{
					if (GetDeltaLimitSize(Data[j]) > 0) Data[j].orderType = 3;
					else if (GetDeltaLimitSize(Data[j]) < 0) Data[j].orderType = 4;
				}
				else //����� 2-3-4
				{
					if (GetDeltaLimitSize(Data[j]) > 0) Data[j].orderType = 5;
					else if (GetDeltaLimitSize(Data[j]) < 0) Data[j].orderType = 6;
				}
			}

			//��� ������ �������� ��������
			if (!thirdFlag) //������ ����� 2-4
			{
				if (GetDeltaLimitSize(_d) > 0) _d.orderType = 3;
				else if (GetDeltaLimitSize(_d) < 0) _d.orderType = 4;
			}
			else //����� 2-3-4
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

void RDOMTicker::WritePreviousBidValue(RITHMICDOM& _d)	//���������� ���������� �������� ����� � ������� ��������
{
	if (Data.size()) {
		for (int j = 0; j <= domLevelsCount; j++)
		{
			_d.bidPrice[j].SetPrice(Data[Data.size()-1].bidPrice[j]);	//����
			_d.bidSize[j] = Data[Data.size()-1].bidSize[j];				//������
			_d.bidOrders[j] = Data[Data.size()-1].bidOrders[j];			//������
		}
	}
}
void RDOMTicker::WritePreviousAskValue(RITHMICDOM& _d)	//���������� ���������� �������� ����� � ������� ��������
{
	if (Data.size()) {
		for (int j = 0; j <= domLevelsCount; j++)
		{
			_d.askPrice[j].SetPrice(Data[Data.size()-1].askPrice[j]);	//����
			_d.askSize[j] = Data[Data.size()-1].askSize[j];				//������
			_d.askOrders[j] = Data[Data.size()-1].askOrders[j];			//������
		}
	}
}
void CQGDOMTicker::WritePreviousBidValue(CQGDOM& _d)	//���������� ���������� �������� ����� � ������� ��������
{
	if (Data.size()) {
		for (int j = 0; j <= domLevelsCount; j++)
		{
			_d.bidPrice[j].SetPrice(Data[Data.size()-1].bidPrice[j]);	//����
			_d.bidSize[j] = Data[Data.size()-1].bidSize[j];				//������
		}
	}
}
void CQGDOMTicker::WritePreviousAskValue(CQGDOM& _d)	//���������� ���������� �������� ����� � ������� ��������
{
	if (Data.size()) {
		for (int j = 0; j <= domLevelsCount; j++)
		{
			_d.askPrice[j].SetPrice(Data[Data.size()-1].askPrice[j]);	//����
			_d.askSize[j] = Data[Data.size()-1].askSize[j];				//������
		}
	}
}

void RDOMTicker::TradesToLimits(RITHMICDOM& _d)
{
	if ((_d.Data.agressorSide == 'S' || _d.Data.impliedSide == 'S') && _d.Data.Data.tradePrice == _d.bidPrice[0]) {	//����� �� ����
	
		if (_d.bidSize[0] > _d.Data.Data.tradeSize)	//��������� ������ �� ����� ��� ������ �������� ������
		{
			_d.tradeSizeQuant = _d.bidSize[0];
			_d.bidSize[0] = _d.bidSize[0] - _d.Data.Data.tradeSize;
			if (Data[Data.size()-1].bidOrders[0]) _d.bidOrders[0] = Data[Data.size()-1].bidOrders[0] - 1;
		} else { //���� ������ �� ����� ����� �� ���������� ������ ��� ������� ������� ������� ���� ������ ������ ��� ����� ������
			_d.tradeSizeQuant = _d.Data.Data.tradeSize;
			_d.bidSize[0] = 0;
			_d.bidOrders[0] = 0;
		}

	} else if ((_d.Data.agressorSide == 'B' || _d.Data.impliedSide == 'B') && _d.Data.Data.tradePrice == _d.askPrice[0]) { //����� �� ����
		
		//��������� ������ �� ����� ��� ������ �������� ������
		if (_d.askSize[0] > _d.Data.Data.tradeSize)
		{
			_d.tradeSizeQuant = _d.askSize[0];
			_d.askSize[0] = _d.askSize[0] - _d.Data.Data.tradeSize;
			if (Data[Data.size()-1].askOrders[0])
				_d.askOrders[0] = Data[Data.size()-1].askOrders[0] - 1;
		} else { //���� ������ �� ����� ����� �� ���������� ������ ��� ������� ������� ������� ���� ������ ������ ��� ����� ������
			_d.tradeSizeQuant = _d.Data.Data.tradeSize;
			_d.askSize[0] = 0;
			_d.askOrders[0] = 0;
		}	

	} else if (_d.Data.Data.tradePrice > _d.bidPrice[0] && _d.Data.Data.tradePrice < _d.askPrice[0]) { //����� ������ ������	

		_d.tradeSizeQuant = _d.Data.Data.tradeSize;

	} else if ((_d.Data.agressorSide == 'S' || _d.Data.impliedSide == 'S') && _d.Data.Data.tradePrice < _d.bidPrice[0]) { //����� ���� ������
		
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

	} else if ((_d.Data.agressorSide == 'B' || _d.Data.impliedSide == 'B') && _d.Data.Data.tradePrice > _d.askPrice[0]) { //����� ���� ������
		
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
	if ((_d.Data.agressorSide == 'S') && _d.Data.Data.tradePrice == _d.bidPrice[0]) {	//����� �� ����
	
		if (_d.bidSize[0] > _d.Data.Data.tradeSize)	//��������� ������ �� ����� ��� ������ �������� ������
		{
			_d.tradeSizeQuant = _d.bidSize[0];
			_d.bidSize[0] = _d.bidSize[0] - _d.Data.Data.tradeSize;
		} else { //���� ������ �� ����� ����� �� ���������� ������ ��� ������� ������� ������� ���� ������ ������ ��� ����� ������
			_d.tradeSizeQuant = _d.Data.Data.tradeSize;
			_d.bidSize[0] = 0;
		}

	} else if ((_d.Data.agressorSide == 'B') && _d.Data.Data.tradePrice == _d.askPrice[0]) { //����� �� ����
		
		//��������� ������ �� ����� ��� ������ �������� ������
		if (_d.askSize[0] > _d.Data.Data.tradeSize)
		{
			_d.tradeSizeQuant = _d.askSize[0];
			_d.askSize[0] = _d.askSize[0] - _d.Data.Data.tradeSize;
		} else { //���� ������ �� ����� ����� �� ���������� ������ ��� ������� ������� ������� ���� ������ ������ ��� ����� ������
			_d.tradeSizeQuant = _d.Data.Data.tradeSize;
			_d.askSize[0] = 0;
		}	

	} else if (_d.Data.Data.tradePrice > _d.bidPrice[0] && _d.Data.Data.tradePrice < _d.askPrice[0]) { //����� ������ ������	

		_d.tradeSizeQuant = _d.Data.Data.tradeSize;

	} else if ((_d.Data.agressorSide == 'S') && _d.Data.Data.tradePrice < _d.bidPrice[0]) { //����� ���� ������
		
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

	} else if ((_d.Data.agressorSide == 'B') && _d.Data.Data.tradePrice > _d.askPrice[0]) { //����� ���� ������
		
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
	//���� ��������� ����
	bool findPrice = false;

	//�������������� ���������� ��������
	WritePreviousBidValue(_d);
	WritePreviousAskValue(_d);

	if (Type == 'A')
	{
		//��������� ������ �������������/��������� ������ ���� ����� ���� ��� ���� � �������
		int j;
		for (j = 0; j <= domLevelsCount; j++)
		{
			if (price == _d.askPrice[j])
			{
				if (j < domLevelsCount-1 || (j == domLevelsCount-1 && Data[Data.size()-1].askSize[j]))
					_d.deltaAskLimitSize[j] = limits - Data[Data.size()-1].askSize[j];	//������ �������������/��������� ������
				findPrice = true;
				break;	//������������� ����������� ����
			}
		}

		//��� ������� ������ ��������� ��������� ������ ���� � �������� ��� ���� ���� ���
		for (int k = 0; k < domLevelsCount; k++)
		{
			bool isVol = false;
			if (!_d.askSize[k] && _d.askPrice[k].IsPrice())	//����� ������� � ������� ������ � ���������� �����
			{
				for (j = domLevelsCount; j >= k; --j)		//��������� ���� �� �� ��� ����������� ���
					if (_d.askSize[j]) 
						isVol = true;

				if (!isVol) break;		//����������� ���, ������� �� �����
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

		//����� ���������� ����� ��������
		for (j = 0; j <= domLevelsCount; j++)
		{
			if (price == _d.askPrice[j])
			{
				_d.askSize[j] = limits;
				if (traders) _d.askOrders[j] = traders;
				_d.eventAskLevel = 1 << j;	
				break;	//������������� ����������� ����
			}
		}

		//���� ����� ���� �� ���������
		if (!findPrice && limits)
		{
			bool zeroFlag = false;	//���� ������� �������� ������ � �������, �� �����-���� ����
			int zeroLvl;			//����� ������ �� ������� ������� ����� ����
			//������ ��� ���� Lvl1, �� ���� ��� �� ������ ������� � ������� � ������� ������� ������� ����� �������
			if (price < _d.askPrice[0])
			{
				//for (j = 0; j < domLevelsCount; j++)
				//{
				//	if (!_d.askSize[j]) {	
				//		zeroFlag = true; zeroLvl = j; break;	
				//	}
				//}
				//				
				//if (zeroFlag) j = zeroLvl;								//� ���� ������ ������, �� ������� ������ ���� �� ���� � ������� �������
				//else j = domLevelsCount;								//� ��� ������ �������, �� ������� ���� ������ ����
				//�������� ������
				for (j = domLevelsCount; j > 0; j--)
				{
					_d.askPrice[j] = _d.askPrice[j-1];
					_d.askSize[j] = _d.askSize[j-1];
					_d.askOrders[j] = _d.askOrders[j-1];
				}			
				
				//� ��������� � ������ ������� ����� ��������
				_d.askPrice[0] = price;
				_d.askSize[0] = limits;
				if (traders) _d.askOrders[0] = traders;
				_d.eventAskLevel = 1 << 0;
				_d.deltaAskLimitSize[0] = limits;

				//�������� �������� ������� � ������� �� 11 ������ � �������, �.�. ��� ����� ���������� � ����� ������, � �� ����� �� ������,
				//������� ��� �������� ��� �� �����
				_d.askSize[domLevelsCount] = 0;
				_d.askOrders[domLevelsCount] = 0;
			}
			//���� ���� ������ ��� ���� domLevelsCount
			else if (price > _d.askPrice[domLevelsCount])
			{
				for (j = domLevelsCount; j >= 0; j--)
				{
					if (!_d.askSize[j]) { zeroFlag = true; zeroLvl = j; break; }
				}
				
				if (zeroFlag) j = zeroLvl;				//� ���� ������ ������, �� ������� ������ ����� �� ���� � ������� �������
				else 
					j = 0;

				//�������� ������	
				if (j > 0)
				{
					for (; j < domLevelsCount; j++)
					{
						_d.askPrice[j] = _d.askPrice[j+1];
						_d.askSize[j] = _d.askSize[j+1];
						_d.askOrders[j] = _d.askOrders[j+1];
					}
					j = domLevelsCount;

					//� ��������� � ��������� ������� ����� ��������
					_d.askPrice[j] = price;
					_d.askSize[j] = limits;
					if (traders) _d.askOrders[j] = traders;
					_d.eventAskLevel = 1 << j;
				}

			} else if (price < _d.askPrice[domLevelsCount] && price > _d.askPrice[0]) { //���� ���� ��������� ������ ������� �� �����
				int index;
				//���� ������ ����� � ������� ����� ��������� ����� ��������
				for (j = 1; j <= domLevelsCount; j++)
				{
					if (price <= _d.askPrice[j] && price >= _d.askPrice[j-1]) 
					{	index = j;	break;	}
				}
				//���� � ������ �� �������� ���-�� ������� ����� ����, �� �� ��� ����� �����
				if (_d.askSize[index] == 0)
				{
					//��������� �� ����� ������� ������� ��������
					_d.askPrice[index] = price;
					_d.askSize[index] = limits;
					if (traders) _d.askOrders[index] = traders;
					_d.eventAskLevel = 1 << index;
					if (index != domLevelsCount-1)
						_d.deltaAskLimitSize[index] = limits;
				}
				else if (_d.askSize[index-1] == 0)
				{
					//��������� �� ����� ������� ������� ��������
					_d.askPrice[index-1] = price;
					_d.askSize[index-1] = limits;
					if (traders) _d.askOrders[index-1] = traders;
					_d.eventAskLevel = 1 << (index-1);
					_d.deltaAskLimitSize[index-1] = limits;
				}
				//���� ����� ���, �� �������� ��� ����� ������� � ������ �� ������� ����������� ������
				else
				{
					for (j = domLevelsCount; j > index; j--)
					{
						_d.askPrice[j] = _d.askPrice[j-1];
						_d.askSize[j] = _d.askSize[j-1];
						_d.askOrders[j] = _d.askOrders[j-1];
					}
					//��������� �� ����� ������� ������� ��������
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
		//��������� ������ �������������/��������� ������ ���� ����� ���� ��� ���� � �������
		int j; 
		for (j = 0; j <= domLevelsCount; j++)
		{
			if (price == _d.bidPrice[j])
			{
				if (j < domLevelsCount-1 || (j == domLevelsCount-1 && Data[Data.size()-1].bidSize[j]))
					_d.deltaBidLimitSize[j] =  limits - Data[Data.size()-1].bidSize[j];	//������ �������������/��������� ������
				findPrice = true;
				break;	//������������� ����������� ����
			}
		}

		//��� ������� ������ ��������� ��������� ������ ���� � �������� ��� ����� ���� ���
		for (int k = 0; k < domLevelsCount; k++)
		{
			bool isVol = false;
			if (!_d.bidSize[k] && _d.bidPrice[k].IsPrice())	//����� ������� � ������� ������ � ���������� �����
			{
				for (j = domLevelsCount; j >= k; --j)		//��������� ���� �� �� ��� ����������� ���
					if (_d.bidSize[j]) 
						isVol = true;

				if (!isVol) break;		//����������� ���, ������� �� �����
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
		
		//���������� ����� ��������
		for (j = 0; j <= domLevelsCount; j++)
		{
			if (price == _d.bidPrice[j])
			{
				_d.bidSize[j] = limits;
				if (traders) _d.bidOrders[j] = traders;
				_d.eventBidLevel = 1 << j;
				break;	//������������� ����������� ����
			}
		}

		//���� ����� ���� �� ���������
		if (!findPrice && limits)
		{
			bool zeroFlag = false;	//���� ������� �������� ������ � �������, �� �����-���� ����
			int zeroLvl;			//����� ������ �� ������� ������� ����� ����
			//������ ��� ���� Lvl1, �� ���� ��� �� ������ ������� � ������� � ������� ������� ������� ������ �������
			if (price > _d.bidPrice[0])
			{
				//for (j = 0; j < domLevelsCount; j++)
				//{
				//	if (!_d.bidSize[j]) { zeroFlag = true; zeroLvl = j; break; }
				//}
				//				
				//if (zeroFlag) j = zeroLvl;								//� ���� ������ ������, �� ������� ������ ���� �� ���� � ������� �������
				//else j = domLevelsCount;			//� ��� ������ �������, �� ������� ���� ������ ����
				
				for (j = domLevelsCount; j > 0; j--)
				{
					_d.bidPrice[j] = _d.bidPrice[j-1];
					_d.bidSize[j] = _d.bidSize[j-1];
					_d.bidOrders[j] = _d.bidOrders[j-1];
				}		
				
				//� ��������� � ������ ������� ����� ��������
				_d.bidPrice[0] = price;
				_d.bidSize[0] = limits;
				if (traders) _d.bidOrders[0] = traders;
				_d.eventBidLevel = 1 << 0;
				_d.deltaBidLimitSize[0] = limits;

				//�������� �������� ������� � ������� �� 11 ������ � �������, �.�. ��� ����� ���������� � ����� ������, � �� ����� �� ������,
				//������� ��� �������� ��� �� �����
				_d.bidSize[domLevelsCount] = 0;
				_d.bidOrders[domLevelsCount] = 0;
			}
			//���� ���� ������ ��� ���� m_domLevelsCount
			else if (price < _d.bidPrice[domLevelsCount]) 
			{
				for (j = domLevelsCount; j >= 0; j--)
				{
					if (!_d.bidSize[j]) { zeroFlag = true; zeroLvl = j; }
				}
				
				if (zeroFlag) j = zeroLvl;				//� ���� ������ ������, �� ������� ������ ����� �� ���� � ������� �������
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
				
					//� ��������� � ��������� ������� ����� ��������
					_d.bidPrice[j] = price;
					_d.bidSize[j] = limits;
					if (traders) _d.bidOrders[j] = traders;
					_d.eventBidLevel = 1 << j;
				}
			
			} else if (price < _d.bidPrice[0] && price > _d.bidPrice[domLevelsCount]) { //���� ���� ��������� ������ ������� �� �����

				int index;
				//���� ������ ����� � ������� ����� ��������� ����� ��������
				for (j = 1; j <= domLevelsCount; j++)
				{
					if (price >= _d.bidPrice[j] && price <= _d.bidPrice[j-1]) 
					{	index = j;	break;	}
				}
				//���� � ������ �� �������� ���-�� ������� ����� ����, �� �� ��� ����� �����
				if (_d.bidSize[index] == 0) {
					//��������� �� ����� ������� ������� ��������
					_d.bidPrice[index] = price;
					_d.bidSize[index] = limits;
					if (traders) _d.bidOrders[index] = traders;
					_d.eventBidLevel = 1 << index;
					if (index != domLevelsCount-1)
						_d.deltaBidLimitSize[index] = limits;
				} else if (_d.bidSize[index-1] == 0) {
					//��������� �� ����� ������� ������� ��������
					_d.bidPrice[index-1] = price;
					_d.bidSize[index-1] = limits;
					if (traders) _d.bidOrders[index-1] = traders;
					_d.eventBidLevel = 1 << (index-1);
					_d.deltaBidLimitSize[index-1] = limits;
				
				} else {		//���� ����� ���, �� �������� ��� ����� ������� � ������ �� ������� ����������� ������
					
					//�������� ��� ���� ������� � ������ �� ������� ����������� ������
					for (j = domLevelsCount; j > index; j--)
					{
						_d.bidPrice[j] = _d.bidPrice[j-1];
						_d.bidSize[j] = _d.bidSize[j-1];
						_d.bidOrders[j] = _d.bidOrders[j-1];
					}
					//��������� �� ����� ������� ������� ��������
					_d.bidPrice[index] = price;
					_d.bidSize[index] = limits;
					if (traders) _d.bidOrders[index] = traders;
					_d.eventBidLevel = 1 << index;
					_d.deltaBidLimitSize[index] = limits;
				}
			}
		}
	}
	//���� ��� �������� �� ���� � �� ����
	else
	{
		//�������������� ���������
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

	//���������
	_d.Data.hour = it.timestamp.wHour;
	_d.Data.minute = it.timestamp.wMinute;
	_d.Data.second = it.timestamp.wSecond;

	//������������
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

	//������ ����� ������� ����������� � ����������
	_d.microDelta = 0;
	if (Data.size())
		_d.microDelta = MicroSpeed(_d.Data.hour, _d.Data.minute, _d.Data.second, _d.microseconds, 
							Data[Data.size()-1].Data.hour, Data[Data.size()-1].Data.minute, Data[Data.size()-1].Data.second, Data[Data.size()-1].microseconds);
	
	//���� � �����
	_d.Data.tradePrice.SetPrice(it.price, GetPrecision());
	_d.Data.tradeSize = it.volume;

	//������� ���������
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

	//���������
	_d.Data.Data.hour = it.timestamp.wHour;
	_d.Data.Data.minute = it.timestamp.wMinute;
	_d.Data.Data.second = it.timestamp.wSecond;

	//������������
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

	//������ ����� ������� ����������� � ����������
	_d.Data.microDelta = 0;
	if (Data.size())
		_d.Data.microDelta = MicroSpeed(_d.Data.Data.hour, _d.Data.Data.minute, _d.Data.Data.second, _d.Data.microseconds, 
							Data[Data.size()-1].Data.Data.hour, Data[Data.size()-1].Data.Data.minute, Data[Data.size()-1].Data.Data.second, Data[Data.size()-1].Data.microseconds);
	
	//���� � �����
	_d.Data.Data.tradePrice.SetPrice(it.price, GetPrecision());
	_d.Data.Data.tradeSize = it.volume;

	//������� ���������
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

	//�������������� ���������� ��������
	if (Data.size())
	{
		WritePreviousBidValue(_d);
		WritePreviousAskValue(_d);
	}
	
	//������������� ������ �������� �������
	if (Data.size())
		TradesToLimits(_d);

	//���������� ������������ �������� � ������� (��� �������)
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

	//���������
	_d.Data.Data.hour = iDOM.timestamp.wHour;
	_d.Data.Data.minute = iDOM.timestamp.wMinute;
	_d.Data.Data.second = iDOM.timestamp.wSecond;

	//������������
	_d.Data.microseconds = (UINT)iDOM.timestamp.wMilliseconds*1000;

	//������ ����� ������� ����������� � ����������
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
	
	//���� � �����
	for (int i = domLevelsCount-1; i >= 0; --i)
	{
		_d.askPrice[i].SetPrice(iDOM.askPrice[i], GetPrecision());
		_d.bidPrice[i].SetPrice(iDOM.bidPrice[i], GetPrecision());
		_d.askSize[i] = iDOM.askSize[i];
		_d.bidSize[i] = iDOM.bidSize[i];
	}

	if (lastIndx && Data[lastIndx].Data.Data.tradeSize && lastIndx != lastSyncTradeIndx) //� ���������� �������� ��� ����� ������ ���� �������� ������
	{
		while (lastIndx && Data[lastIndx].Data.Data.tradeSize)
			lastIndx--;

		if (!lastIndx)
		{
			lastIndx = Data.size()-1;
			goto metka;
		}

		//������ ���������
		_d.Data.Data.hour = Data[lastIndx].Data.Data.hour;
		_d.Data.Data.minute = Data[lastIndx].Data.Data.minute;
		_d.Data.Data.second = Data[lastIndx].Data.Data.second;
		_d.Data.microseconds = Data[lastIndx].Data.microseconds;
		_d.Data.microDelta = 0;
	}

metka:
		
	//���� ������� �� ������� ��������� ��������� � ������������� ������� ������, ������� ��������� � �����
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
					_d.Data.agressorSide = 'x'; // 'x' - ��� ��������� ����� �� ����� � �� ������ ������� �� ���� �������
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

	//���������� ������������ �������� � ������� (��� �������)
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

			//���������� ������������ �������� � ������� (��� �������)
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

	//���������
	_d.Data.hour = it.hour;
	_d.Data.minute = it.minute;
	_d.Data.second = it.second;

	//������������
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

	//������ ����� ������� ����������� � ����������
	_d.microDelta = 0;
	if (Data.size())
		_d.microDelta = MicroSpeed(_d.Data.hour, _d.Data.minute, _d.Data.second, _d.microseconds, 
							Data[Data.size()-1].Data.hour, Data[Data.size()-1].Data.minute, Data[Data.size()-1].Data.second, Data[Data.size()-1].microseconds);
	
	//���� � �����
	_d.Data.tradePrice.SetPrice(it.Price, GetPrecision());
	_d.Data.tradeSize = it.Volume;

	//������� ���������
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

	//���������
	_d.Data.Data.hour = it.hour;
	_d.Data.Data.minute = it.minute;
	_d.Data.Data.second = it.second;

	//������������
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

	//������ ����� ������� ����������� � ����������
	_d.Data.microDelta = 0;
	if (Data.size())
		_d.Data.microDelta = MicroSpeed(_d.Data.Data.hour, _d.Data.Data.minute, _d.Data.Data.second, _d.Data.microseconds, 
							Data[Data.size()-1].Data.Data.hour, Data[Data.size()-1].Data.Data.minute, Data[Data.size()-1].Data.Data.second, Data[Data.size()-1].Data.microseconds);
	
	//���� � �����
	_d.Data.Data.tradePrice.SetPrice(it.Price, GetPrecision());
	_d.Data.Data.tradeSize = it.Volume;

	//������� ���������
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

	//�������������� ���������� ��������
	if (Data.size())
	{
		WritePreviousBidValue(_d);
		WritePreviousAskValue(_d);
	}
	
	//������������� ������ �������� �������
	if (Data.size())
		TradesToLimits(_d);

	//���������� ������������ �������� � ������� (��� �������)
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
	//���� ��������� ����
	//bool findPrice = false;

	//�������������� ���������� ��������
	WritePreviousBidValue(_d);
	WritePreviousAskValue(_d);

	if (Type == 'a')
	{
		//��������� ������ �������������/��������� ������ ���� ����� ���� ��� ���� � �������
		int j;
		for (j = 0; j < domLevelsCount; j++)
		{
			if (price == _d.askPrice[j])
			{
				if (j < domLevelsCount-1 || (j == domLevelsCount-1 && Data[Data.size()-1].askSize[j]))
					_d.deltaAskLimitSize[j] = limits - Data[Data.size()-1].askSize[j];	//������ �������������/��������� ������
				//findPrice = true;
				break;	//������������� ����������� ����
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
		//��������� ������ �������������/��������� ������ ���� ����� ���� ��� ���� � �������
		int j; 
		for (j = 0; j < domLevelsCount; j++)
		{
			if (price == _d.bidPrice[j])
			{
				if (j < domLevelsCount-1 || (j == domLevelsCount-1 && Data[Data.size()-1].bidSize[j]))
					_d.deltaBidLimitSize[j] =  limits - Data[Data.size()-1].bidSize[j];	//������ �������������/��������� ������
				//findPrice = true;
				break;	//������������� ����������� ����
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
	//���� ��� �������� �� ���� � �� ����
	else
	{
		//�������������� ���������
		//_d.Data.Data.timeStamp = Data[Data.size()-1].Data.Data.timeStamp;
	}
}
void CQGDOMTicker::GetDOMNinja(void* d)
{
	DOM_DATA iDOM = *(DOM_DATA*)d;

	CQGDOM _d;
	ZeroData(_d, 11);

	size_t lastIndx = Data.size()-1;

	//���������
	_d.Data.Data.hour = iDOM.hour;
	_d.Data.Data.minute = iDOM.minute;
	_d.Data.Data.second = iDOM.second;

	//������������
	_d.Data.microseconds = (UINT)iDOM.millisecond*1000;

	//������ ����� ������� ����������� � ����������
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

	//���������� ������������ �������� � ������� (��� �������)
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