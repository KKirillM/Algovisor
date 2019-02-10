#include "stdafx.h"

#include "NinjaDialog.h"
#include "MainWindow.h"
#include "QuoteRun.h"
#include "Quant_Chart.h"
#include "Heat_Map.h"
#include "Instrument.h"

LRESULT MainWindow::NinjaInstrumentData(WPARAM wParam, LPARAM lParam)	//WM_APP + 6
{
	Instrument_DATA data = ninjaIdata.front();
	wchar_t wstr[100];

	//проверяем не изменилась ли глубина подписки у существующего инструмента
	/*int i = 0; 
	char name[50];
	if (ninja_data.size()) 
	{
		StringCchPrintfA(name, 50, "%S", ip->ticker);
		while (i < ninja_data.size() && strcmp(ninja_data[i]->GetName().c_str(), name))
			++i;

		if (i < ninja_data.size() && ninja_data[i]->GetType() != ip->type)
		{
			ninja_data[i]->SetType(ip->type);

			if (!ip->type)
				StringCchPrintfW(wstr, 100, L"TRADES ONLY");
			else
				StringCchPrintfW(wstr, 100, L"TRADES & DOM");
			instrumentList.SetItemText(0, 4, wstr);
		}
	}	*/
	
	if (!securityString.compare(data.securityStr))	//если эта строка установлена не верно, то подключение к нинзе будет, а инструменты добавляться не будут
	{
		//проверяем повторную подписку на инструмент, т.е. когда инструмент уже был в списке, произошло отключение и потом заново подписались
		//PostMessage(WM_APP + 12, (WPARAM)data.ticker, 0);
		for (int x = instrumentList.GetItemCount()-1; x >= 0 ; --x)
		{
			if (instrumentList.GetItemText(x,3) == data.ID && instrumentList.GetItemText(x,4) == L"Disconnect")
			{
				instrumentList.SetItemText(x, 4, L"Getting data");
				ninjaIdata.pop_front();
				return 0;
			}
		}
		
		CQGTicker* _i;
		char str[100];

		StringCchPrintfA(str, 100, "%S", data.ticker);

		_i = new CQGDOMTicker(str);
		//_i->SetDomLevelsCount(5); //у нинзи всегда только 5 уровней

		//ID
		StringCchPrintfA(str, 100, "%S", data.ID);
		_i->SetStrID(str);

		//получаем описание текущего инструмента
		StringCchPrintfA(str, 100, "%S", data.description);
		_i->SetDescription(str);

		//дата экспирации
		StringCchPrintfA(str, 100, "%S", data.expiration);
		_i->SetExpiration(str);

		//биржа
		StringCchPrintfA(str, 100, "%S", data.exchange);
		_i->SetExchange(str);

		bool intValue = false;
		int precision = 0;
		int MinTickInc;
		while (true)
		{
			data.tickSize *= 10;
			++precision;

			if ((int)data.tickSize)
				intValue = true;

			if (intValue && !((int)data.tickSize % 10))
			{
				data.tickSize /= 10;
				MinTickInc = (int)data.tickSize;
				--precision;
				break;
			}			
		}

		_i->SetPrecision(precision);
		_i->SetMinTick(MinTickInc);

		//заносим данные по инструменту в контейнер и в список
		ninja_data.push_back(_i);	

		//статус подключения 3 когда идут данные по инструменту
		if (ninjaStatus != 3)
		{
			ninjaStatus = 3;
			PostMessage(WM_APP + 13, (WPARAM)&ninjaStatus, 0);
			//InvalidateNinjaConnectionIndicator(&ninjaStatus);
		}

		instrumentList.InsertItem(0, _T(""));
		instrumentList.SetItemText(0, 1, L"NinjaTrader");
		instrumentList.SetItemText(0, 2, data.ticker);
		StringCchPrintfW(wstr, 100, L"%s", data.ID); instrumentList.SetItemText(0, 3, wstr);
		instrumentList.SetItemText(0, 4, L"Getting data");
		instrumentList.SetItemText(0, 5, data.exchange);
		instrumentList.SetItemText(0, 6, data.expiration);
		instrumentList.SetItemText(0, 7, data.description);
	
		instrumentList.SetFocus();
		int n = instrumentList.GetTopIndex();
		int nLast = n + instrumentList.GetCountPerPage();
		for (; n < nLast; n++)
		{
		   instrumentList.SetItemState(n, 0, LVIS_SELECTED|LVIS_FOCUSED);
		}
		instrumentList.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);	
		instrumentList.EnsureVisible(0, FALSE);

		StringCchPrintf(wstr, 100, L"NinjaTrader: Instrument subscribe: %s", data.ticker);
		UpdateActivityWindow(wstr);
	}

	ninjaIdata.pop_front();
	
	return 0;
}

LRESULT MainWindow::NinjaInstrumentTrade(WPARAM wParam, LPARAM lParam)	//WM_APP + 7
{
	//Trade_DATA* cit = &ninjaTrade[0];
	Trade_DATA cit = ninjaTrade.front();

	//определяем индекс текущего инструмента в контейнере инструментов
	size_t i = 0; 
	char name[50];
	if (ninja_data.size())
	{
		StringCchPrintfA(name, 50, "%S", cit.ticker);
		while (i < ninja_data.size() && strcmp(ninja_data[i]->GetName().c_str(), name))
			++i;

		//проверяем повторную подписку на инструмент, т.е. когда инструмент уже был в списке, произошло отключение и потом заново подписались
		//PostMessage(WM_APP + 12, (WPARAM)cit.ticker, 0);
		for (int x = instrumentList.GetItemCount()-1; x >= 0 ; --x)
		{
			if (instrumentList.GetItemText(x,2) == cit.ticker)
			{
				if (instrumentList.GetItemText(x,4) == L"Disconnect")
					instrumentList.SetItemText(x, 4, L"Getting data");
				break;
			}			
		}

		if (i == ninja_data.size())
		{
			ninjaTrade.pop_front();
			return 0;
		}

		//заносим данные трейда в нужный инструмент
		
		ninja_data[i]->TradePrintNinja(&cit);
		ninjaTrade.pop_front();
		
	}	
	else 
		ninjaTrade.clear();
	
	//статус подключения 3 когда идут данные по инструменту
	if (ninjaStatus != 3)
	{
		ninjaStatus = 3;
		PostMessage(WM_APP + 13, (WPARAM)&ninjaStatus, 0);
		//InvalidateNinjaConnectionIndicator(&ninjaStatus);
	}
	
	return 0;
}

LRESULT MainWindow::NinjaInstrumentBid(WPARAM wParam, LPARAM lParam)	//WM_APP + 8
{
	//BBO_DATA* iBBO = &ninja_BBO_BID[0];
	BBO_DATA iBBO = ninja_BBO_BID.front();

	//определяем индекс текущего инструмента в контейнере инструментов
	size_t i = 0; 
	char name[50];
	if (ninja_data.size()) 
	{
		StringCchPrintfA(name, 50, "%S", iBBO.ticker);
		while (i < ninja_data.size() && strcmp(ninja_data[i]->GetName().c_str(), name))
			++i;

		//проверяем повторную подписку на инструмент, т.е. когда инструмент уже был в списке, произошло отключение и потом заново подписались
		//PostMessage(WM_APP + 12, (WPARAM)iBBO.ticker, 0);
		for (int x = instrumentList.GetItemCount()-1; x >= 0 ; --x)
		{
			if (instrumentList.GetItemText(x,2) == iBBO.ticker)
			{
				if (instrumentList.GetItemText(x,4) == L"Disconnect")
					instrumentList.SetItemText(x, 4, L"Getting data");
				break;
			}
		}

		if (i == ninja_data.size())
		{
			ninja_BBO_BID.pop_front();
			return 0;
		}

		
		ninja_data[i]->SetLastBid(iBBO.Price);
		ninja_BBO_BID.pop_front();		
	}	
	else
		ninja_BBO_BID.clear();
	
	//статус подключения 3 когда идут данные по инструменту
	if (ninjaStatus != 3)
	{
		ninjaStatus = 3;
		PostMessage(WM_APP + 13, (WPARAM)&ninjaStatus, 0);
		//InvalidateNinjaConnectionIndicator(&ninjaStatus);
	}

	return 0;
}

LRESULT MainWindow::NinjaInstrumentAsk(WPARAM wParam, LPARAM lParam)	//WM_APP + 9
{
	//BBO_DATA* iBBO = &ninja_BBO_ASK[0];
	BBO_DATA iBBO = ninja_BBO_ASK.front();
	
	//определяем индекс текущего инструмента в контейнере инструментов
	size_t i = 0; 
	char name[50];
	if (ninja_data.size()) 
	{
		StringCchPrintfA(name, 50, "%S", iBBO.ticker);
		while (i < ninja_data.size() && strcmp(ninja_data[i]->GetName().c_str(), name))
			++i;

		//проверяем повторную подписку на инструмент, т.е. когда инструмент уже был в списке, произошло отключение и потом заново подписались
		//PostMessage(WM_APP + 12, (WPARAM)iBBO.ticker, 0);
		for (int x = instrumentList.GetItemCount()-1; x >= 0 ; --x)
		{
			if (instrumentList.GetItemText(x,2) == iBBO.ticker)
			{
				if (instrumentList.GetItemText(x,4) == L"Disconnect")
					instrumentList.SetItemText(x, 4, L"Getting data");
				break;
			}
		}

		if (i == ninja_data.size())
		{
			ninja_BBO_ASK.pop_front();
			return 0;
		}

		
		ninja_data[i]->SetLastAsk(iBBO.Price);
		ninja_BBO_ASK.pop_front();
		
	}	
	else 
		ninja_BBO_ASK.clear();
	
	//статус подключения 3 когда идут данные по инструменту
	if (ninjaStatus != 3)
	{
		ninjaStatus = 3;
		PostMessage(WM_APP + 13, (WPARAM)&ninjaStatus, 0);
		//InvalidateNinjaConnectionIndicator(&ninjaStatus);
	}

	return 0;
}

LRESULT MainWindow::NinjaInstrumentDOM(WPARAM wParam, LPARAM lParam)	//WM_APP + 10
{
	//DOM_DATA* ciDOM = &ninja_DOM[0];
	DOM_DATA ciDOM = ninja_DOM.front();
	
	//определяем индекс текущего инструмента в контейнере инструментов
	size_t i = 0;
	char name[50];
	if (ninja_data.size())
	{
		StringCchPrintfA(name, 50, "%S", ciDOM.ticker);
		while (i < ninja_data.size() && strcmp(ninja_data[i]->GetName().c_str(), name))
			++i;

		//проверяем повторную подписку на инструмент, т.е. когда инструмент уже был в списке, произошло отключение и потом заново подписались
		//PostMessage(WM_APP + 12, (WPARAM)ciDOM.ticker, 0);
		for (int x = instrumentList.GetItemCount()-1; x >= 0 ; --x)
		{
			if (instrumentList.GetItemText(x,2) == ciDOM.ticker)
			{
				if (instrumentList.GetItemText(x,4) == L"Disconnect")
					instrumentList.SetItemText(x, 4, L"Getting data");
				break;
			}
		}

		if (i == ninja_data.size())
		{
			ninja_DOM.pop_front();
			return 0;
		}

		//устанавливаем размер глубины стакана
		int currentDomLevels = ninja_data[i]->GetDomLevelsCount();
		if (currentDomLevels < ciDOM.pos/*+1*/)
		{
			if (ciDOM.pos < 11)
				ninja_data[i]->SetDomLevelsCount(ciDOM.pos/*+1*/);
		
			//обнуляем указатели у всех окон для данного инструмента
			ptrdiff_t wndNums = quoteRunWindows.size();
			if (wndNums && currentDomLevels <= 5 && ciDOM.pos > 5)
			{
				for (ptrdiff_t j = wndNums-1; j >= 0; --j)	//окна QuoteRun
				{
					if (quoteRunWindows[j] && quoteRunWindows[j]->name.compare(name) == 0)
						//::PostMessageW(qrWindows[j]->m_hWnd, WM_APP+1, 0, 0);
						quoteRunWindows[j]->InitializeControlList(0,0);
				}  
			}
		}

		if (ciDOM.pos < 11)
		{
			ninja_data[i]->GetDOMNinja(&ciDOM);	
		}

		ninja_DOM.pop_front();		
	}	
	else 
		ninja_DOM.clear();	
	
	//статус подключения 3 когда идут данные по инструменту
	if (ninjaStatus != 3)
	{
		ninjaStatus = 3;
		PostMessage(WM_APP + 13, (WPARAM)&ninjaStatus, 0);
		//InvalidateNinjaConnectionIndicator(&ninjaStatus);
	}

	return 0;
}

LRESULT MainWindow::NinjaInstrumentDELETE(WPARAM wParam, LPARAM lParam)	//WM_APP + 11
{
	//DELETE_INSTRUMENT* id = &ninjaDelete[0];
	DELETE_INSTRUMENT id = ninjaDelete.front();
	CStringA name;
	name.Format("%S", id.ticker);	

	//обнуляем указатели у всех окон для данного инструмента
	for (ptrdiff_t i = quoteRunWindows.size()-1; i >= 0; --i)	//окна QuoteRun
	{
		if (!quoteRunWindows[i]->name.compare(name))
			quoteRunWindows[i]->Unsubscribe();
	}
	for (ptrdiff_t i = quantChartWindows.size()-1; i >= 0; --i)	//окна Quant chart
	{
		if (!quantChartWindows[i]->name.compare(name))
			quantChartWindows[i]->Unsubscribe();
	}
	for (ptrdiff_t i = heatMapWindows.size()-1; i >= 0; --i)	//окна Quant chart
	{
		if (!heatMapWindows[i]->name.compare(name))
			heatMapWindows[i]->Unsubscribe();
	}

	//удаление из контейнера
	DeleteFromVector(NINJATRADER, name);

	//удаляем инструмент из списка
	for (int x = instrumentList.GetItemCount()-1; x >= 0 ; --x)
	{
		if (instrumentList.GetItemText(x,2) == id.ticker)
		{
			instrumentList.DeleteItem(x);			
			break;
		}
	}	
	
	//выбрать другой инструмент если остался
	int nItem = instrumentList.GetItemCount();
	if (nItem)
	{
		instrumentList.SetFocus();
		instrumentList.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		instrumentList.EnsureVisible(0, FALSE);
	}

	wchar_t wstr[100];
	StringCchPrintf(wstr, 100, L"NinjaTrader: Instrument unsubscribe: %s", id.ticker);
	UpdateActivityWindow(wstr);

	ninjaDelete.pop_front();
	
	return 0;
}

LRESULT MainWindow::NinjaGettingData(WPARAM wParam, LPARAM lParam)
{
	const wchar_t* str_ticker = (const wchar_t*)wParam;
	//проверяем повторную подписку на инструмент, т.е. когда инструмент уже был в списке, произошло отключение и потом заново подписались
	for (int x = instrumentList.GetItemCount()-1; x >= 0 ; --x)
	{
		if (instrumentList.GetItemText(x,2) == str_ticker)
		{
			if (instrumentList.GetItemText(x,4) == L"Disconnect")
				instrumentList.SetItemText(x, 4, L"Getting data");
			break;
		}
	}

	return 0;
}

LRESULT MainWindow::InvalidateNinjaConnectionIndicator(WPARAM wParam, LPARAM lParam)
{
	GetDlgItem(IDC_CONNECTION_STATUS_FEED3)->Invalidate(0);
	if (ninjaDlg)
		ninjaDlg->Invalidate(0);

	return 0;
}