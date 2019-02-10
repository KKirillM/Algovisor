#include "stdafx.h"

#include "CQGDialog.h"
#include "MainWindow.h"
#include "QuoteRun.h"
#include "Auxiliary_methods.h"
#include "Instrument.h"

LRESULT MainWindow::CQGInstrumentProperty(WPARAM wParam, LPARAM lParam)
{
	//INSTRUMENT_PROPERTIES& ip = *(INSTRUMENT_PROPERTIES*)wParam;	
	
	wchar_t wstr[100];

	//проверяем повторную подписку на инструмент
	for (int x = instrumentList.GetItemCount()-1; x >= 0 ; --x)
	{
		if (_wtoi(instrumentList.GetItemText(x,3).GetString()) == cqgIp->instrumentID && instrumentList.GetItemText(x,4) != L"Disconnect")
		{
			StringCchPrintf(wstr, 100, L"CQG: Instrument already in list: %s", cqgIp->ticker);
			UpdateActivityWindow(wstr);
			return 1;
		}
		else if (_wtoi(instrumentList.GetItemText(x,3).GetString()) == cqgIp->instrumentID && instrumentList.GetItemText(x,4) == L"Disconnect")
		{
			instrumentList.SetItemText(x, 4, L"Getting data");
			return 0;
		}
	}

	CQGTicker* _i;
	char str[100];

	StringCchPrintfA(str, 100, "%S", cqgIp->ticker);
	
	_i = new CQGDOMTicker(str);

	//ID
	_i->SetID(cqgIp->instrumentID);

	//полное имя
	//_i->SetFullName(ip->fullName);

	//получаем описание текущего инструмента
	StringCchPrintfA(str, 100, "%S", cqgIp->description);
	_i->SetDescription(str);

	//дата экспирации
	StringCchPrintfA(str, 100, "%S", cqgIp->expirationDate);
	_i->SetExpiration(str);

	//биржа
	_i->SetExchangeID(cqgIp->exchID);

	_i->SetPrecision(cqgIp->precision);
	_i->SetMinTick(cqgIp->MinTickInc);

	//заносим данные по инструменту в контейнер и в список
	cqg_data.push_back(_i);	

	instrumentList.InsertItem(0, _T(""));
	instrumentList.SetItemText(0, 1, L"CQG");
	instrumentList.SetItemText(0, 2, cqgIp->ticker);
	//instrumentList.SetItemText(0, 2, ip.fullName);
	StringCchPrintfW(wstr, 100, L"%d", cqgIp->instrumentID); instrumentList.SetItemText(0, 3, wstr);
	instrumentList.SetItemText(0, 4, L"Getting data");
	StringCchPrintf(wstr, 100, L"%d", cqgIp->exchID); instrumentList.SetItemText(0, 5, wstr);
	instrumentList.SetItemText(0, 6, cqgIp->expirationDate);
	instrumentList.SetItemText(0, 7, cqgIp->description);

	instrumentList.SetFocus();
	int n = instrumentList.GetTopIndex();
	int nLast = n + instrumentList.GetCountPerPage();
	for (; n < nLast; n++)
	{
		instrumentList.SetItemState(n, 0, LVIS_SELECTED|LVIS_FOCUSED);
	}
	instrumentList.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);	
	instrumentList.EnsureVisible(0, FALSE);

	return 0;
}

LRESULT MainWindow::CQGInstrumentTrade(WPARAM wParam, LPARAM lParam)
{
	CQG_INSTRUMENT_TRADE* cit = &cqgTrade[0];

	//определяем индекс текущего инструмента в контейнере инструментов
	size_t i = 0; 
	if (cqg_data.size()) 
	{
		//while (cqg_data[i]->GetFullName().compare(it.fullName) != 0 && i < cqg_data.size())
		while (i < cqg_data.size() && cqg_data[i]->GetID() != cit->instrumentID)
			++i;
	}
	
	if (!cqg_data.size() || i == cqg_data.size())
		return 0;

	//заносим данные трейда в нужный инструмент
	cqg_data[i]->TradePrint(cit);

	cqgTrade.erase(cqgTrade.begin());
	
	return 0;
}

LRESULT MainWindow::CQGInstrumentAsk(WPARAM wParam, LPARAM lParam)
{
	CQG_INSTRUMENT_BBO_ASK* iBBO = &cqg_BBO_ASK[0];

	//определяем индекс текущего инструмента в контейнере инструментов
	size_t i = 0; 
	if (cqg_data.size()) 
	{
		//while (cqg_data[i]->GetFullName().compare(iBBO.fullName) != 0 && i < cqg_data.size())
		while (i < cqg_data.size() && cqg_data[i]->GetID() != iBBO->instrumentID)
			++i;
	}
	
	if (!cqg_data.size() || i == cqg_data.size())
		return 0;

	cqg_data[i]->SetLastAsk(iBBO->price);
	//cqg_data[i]->lastAskSize = iBBO.size;

	cqg_BBO_ASK.erase(cqg_BBO_ASK.begin());

	return 0;
}
LRESULT MainWindow::CQGInstrumentBid(WPARAM wParam, LPARAM lParam)
{
	CQG_INSTRUMENT_BBO_BID* iBBO = &cqg_BBO_BID[0];

	//определяем индекс текущего инструмента в контейнере инструментов
	size_t i = 0; 
	if (cqg_data.size()) 
	{
		//while (cqg_data[i]->GetFullName().compare(iBBO.fullName) != 0 && i < cqg_data.size())
		while (i < cqg_data.size() && cqg_data[i]->GetID() != iBBO->instrumentID)
			++i;
	}
	
	if (!cqg_data.size() || i == cqg_data.size())
		return 0;

	cqg_data[i]->SetLastBid(iBBO->price);
	//cqg_data[i]->lastBidSize = iBBO.size;

	cqg_BBO_BID.erase(cqg_BBO_BID.begin());

	return 0;
}

LRESULT MainWindow::CQGInstrumentDOM(WPARAM wParam, LPARAM lParam)
{
	CQG_INSTRUMENT_DOM* ciDOM = &cqg_DOM[0];

	//определяем индекс текущего инструмента в контейнере инструментов
	size_t i = 0; 
	if (cqg_data.size()) 
	{
		//while (cqg_data[i]->GetFullName().compare(iDOM.fullName) != 0 && i < cqg_data.size())
		while (i < cqg_data.size() && cqg_data[i]->GetID() != ciDOM->instrumentID)
			++i;
	}
	
	if (!cqg_data.size() || i == cqg_data.size())
		return 0;

	//устанавливаем размер глубины стакана
	if (cqg_data[i]->GetDomLevelsCount() < ciDOM->domLevelCount)
	{
		cqg_data[i]->SetDomLevelsCount(ciDOM->domLevelCount);
		
		//обнуляем указатели у всех окон для данного инструмента
		if (quoteRunWindows.size())
		{
			for (ptrdiff_t j = quoteRunWindows.size()-1; j >= 0; --j)	//окна QuoteRun
			{
				if (quoteRunWindows[j]->fType == CQG)
					quoteRunWindows[j]->InitializeControlList(0,0);
			}
		}
	}

	if (cqg_data[i]->GetDomLevelsCount())
	{
		//заносим данные трейда в нужный инструмент
		cqg_data[i]->GetDOM(ciDOM);
	}

	cqg_DOM.erase(cqg_DOM.begin());
	
	return 0;
}