#include "stdafx.h"

#include "Instrument.h"
#include "MainWindow.h"
#include "RithmicDialog.h"
#include "resource.h"
#include "Auxiliary_methods.h"
#include "Plaza2Instruments.h"
#include "BaseChart.h"



ThreadPool BaseChart::tPool;

BaseChart::BaseChart(MainWindow* wnd, FeedType feed, IInstrument* i) : CFrameWnd(), mWnd(wnd), fType(feed), instrumentPtr(i), subInstrumentPtr(NULL), timerIndex(0), syncIndex(0)
{
	name = instrumentPtr->GetName();

	alwaysOnTop = false;

	price_axis_width = 0;
	time_axis_width = 0;

	Xscale = 0;
	scale_type = VISIBLE_RANGE;

	tradesLabels = true;
	limitsLabels = true;
	YscaleTop = 1;
	YscaleBottom = 1;

	ordersLabels = false;

	showGridLines = true;
	lButtonOnPlotArea = false;
	lButtonOnMoveZone1 = false;
	lButtonOnTimeZone = false;
	lButtonOnPriceZone = false;
	mouseOnScaleButton = false;
	mouseOnMoveZone1 = false;
	showOrders = false;
	showHorizontalDOM = false;

	updateFrequency = 150;
	aggregatedOrders = FALSE;

	bEventAutoScale = 0;		// 0 - автоматический масштаб события (максимально видимый), 1 - ручной
	uManualScaleMaxValue = 1;		// размер масштаба события в ручном режиме по-умолчанию равен 1

	defaultbackgroundColor = RGB(0, 0, 0);	    // черный
	plotBackgoundColor = RGB(20, 20, 20);		    // темно-серый
	defaultTextColor = RGB(255, 255, 255);	    // белый
	axisColor = RGB(200, 200, 200);			    // светло серый
	gridColor = RGB(50, 50, 50);				    // темно серый
	tradeColor = RGB(255, 255, 0);			    // желтый
	buyTradeColor = RGB(255, 255, 0);			    // желтый
	sellTradeColor = RGB(255, 255, 0);		    // желтый
	noagressorColor = RGB(0, 255, 255);		    // светло-голубой
	tradeLabelColor = RGB(255, 128, 64);		    // оранжевый
	bidLimitColor = RGB(0, 0, 128);			    // темно-синий
	newBidLimitColor = RGB(0, 0, 255);		    // синий
	askLimitColor = RGB(128, 0, 0);			    // темно-красный
	newAskLimitColor = RGB(255, 0, 0);		    // красный
	bidOrdersColor = RGB(64, 0, 128);			    // темно-фиолетовый
	newBidOrdersColor = RGB(128, 0, 255);		    // фиолетовый
	askOrdersColor = RGB(64, 0, 128);			    // темно-фиолетовый
	newAskOrdersColor = RGB(128, 0, 255);		    // фиолетовый
	tradeSizeAlertColor = RGB(255, 255, 255);	    // белый
	limitAddAlertColor = RGB(0, 128, 0);		    // темно-зеленый
	limitRemoveAlertColor = RGB(255, 128, 0);	    // оранжевый	
	OIIncreaseBackground = RGB(0, 255, 0);		// ярко-зеленый
	OIDecreaseBackground = RGB(255, 0, 0);		// ярко-красный
	OINotChangeBackground = RGB(192, 192, 192);   // бледно-серый

	tradesOnly = FALSE;
	aggregatedOrders = FALSE;
	showOpenInterestChange = FALSE;

	//пул потоков для данного окна
	tPool.CreatePool(1);
}

void BaseChart::GetPriceAxisWidth()
{
	size_t i = 0;
	TCHAR str[64] = L"";

	::AcquireSRWLockShared(&SRWLockRithmic);

	size_t book_size = instrumentPtr->GetAggregatedOrderbookSize();
	if (book_size == 0)
		return;

	i = book_size - 1;
	if (instrumentPtr->Get_askPrice(i, 0).IsPrice())
	{
		StringCchPrintf(str, 64, L"%s", instrumentPtr->Get_askPrice(i, 0).GetWString(instrumentPtr->GetPrecision()));
	}
	else if (instrumentPtr->Get_bidPrice(i, 0).IsPrice())
	{
		StringCchPrintf(str, 64, L"%s", instrumentPtr->Get_bidPrice(i, 0).GetWString(instrumentPtr->GetPrecision()));
	}
	else if (instrumentPtr->Get_tradePrice(i).IsPrice())
	{
		StringCchPrintf(str, 64, L"%s", instrumentPtr->Get_tradePrice(i).GetWString(instrumentPtr->GetPrecision()));
	}

	/*while (i < instrumentPtr->GetAggregatedOrderbookSize() && !instrumentPtr->Get_askPrice(i, 0).IsPrice())
    {
		++i;
    }*/

	/*if (i < instrumentPtr->GetAggregatedOrderbookSize())
    {
		StringCchPrintf(str, 64, L"%s", instrumentPtr->Get_askPrice(i, 0).GetWString(instrumentPtr->GetPrecision()));
    }*/
	
	::ReleaseSRWLockShared(&SRWLockRithmic);

	if (!wcscmp(str, L""))
		return;

	//если есть строка с трейдом
	HDC dc = ::GetDC(m_hWnd);
	::SelectObject(dc, (HFONT)qFont);
	SIZE sSize;
	::GetTextExtentPoint32W(dc, str, (int)_tcslen(str), &sSize);
	::DeleteDC(dc);

	price_axis_width = sSize.cx + 15;
	time_axis_width = sSize.cy*2;	

	RECT rt;
	::GetClientRect(m_hWnd, &rt);
	SetRects(rt.right, rt.bottom);
			
	/*plotZone.left = 0;
	plotZone.right = clientRect.right - price_axis_width;
	plotZone.bottom = clientRect.bottom - time_axis_width;
			
	plotMoveZone1.left = clientRect.right - price_axis_width - 5;
	plotMoveZone1.top = clientRect.bottom - time_axis_width+1; 
	plotMoveZone1.right = clientRect.right - price_axis_width; 
	plotMoveZone1.bottom = clientRect.bottom;*/
}
BaseChart::~BaseChart()
{
	if (subInstrumentPtr != NULL)
		delete instrumentPtr;

	//delete poolData1;

	//закрываем рабочий элемент пула потоков
	//CloseThreadpoolWork(gWorkItem1);
}
void BaseChart::Unsubscribe()
{
	if (subInstrumentPtr != NULL)
		delete instrumentPtr;
	
	subInstrumentPtr = NULL;

	instrumentPtr = NULL;

	timerIndex = 0;	
    syncIndex = 0;
}

BEGIN_MESSAGE_MAP(BaseChart, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// BaseChart message handlers

BOOL BaseChart::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.cx = 650;
	cs.cy = 400;
	
	cs.style |= WS_HSCROLL; 
	
	return CFrameWnd::PreCreateWindow(cs);
}
int BaseChart::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//добавляем в стиль окна CS_OWNDC
	//DWORD style = ::GetClassLong(m_hWnd, GCL_STYLE);
	//::SetClassLong(m_hWnd, GCL_STYLE, style | CS_OWNDC);

	//по-умолчанию черный фон
	::SetClassLongPtrW(m_hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)::GetStockObject(BLACK_BRUSH));

	//шрифт
	qFont.CreateFont(14, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, VARIABLE_PITCH|FF_SWISS, L"Arial");

	//создаем строку состояния
	statusBar.Create(this);
	statusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	if (fType == RITHMIC)
	{
		HICON rithmicIcon = (HICON)LoadImageW(AfxGetInstanceHandle(), MAKEINTRESOURCE(/*IDI_RITHMIC_LICENSE_ICON_2*/IDI_RITHMIC_TRADING_PLATFORM), IMAGE_ICON, /*134*/161, /*28*/15, 0);
		HICON omneIcond = (HICON)LoadImageW(AfxGetInstanceHandle(), MAKEINTRESOURCE(/*IDI_RITHMIC_LICENSE_ICON_3*/IDI_RITHMIC_OMNE), IMAGE_ICON, 122, /*28*/15, 0);
		statusBar.GetStatusBarCtrl().SetIcon(1, rithmicIcon);
		statusBar.GetStatusBarCtrl().SetIcon(2, omneIcond);
	}

	//размер клиентской области 
	GetClientRect(&clientRect);
	//clientRect.bottom -= 19;
	memcpy(&plotZone, &clientRect, sizeof(RECT));
	memcpy(&plotMoveZone1, &clientRect, sizeof(RECT));
	::SetRect(&priceZone, 0,0,0,0);
	::SetRect(&timeZone, 0,0,0,0);
	::SetRect(&plotZoneAdd, 0,0,0,0);	

	//запускаем таймер
	SetTimer(1, updateFrequency, 0);

	return 0;
}
BOOL BaseChart::PreTranslateMessage(MSG* pMsg)
{
	return FALSE;
}

void BaseChart::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// ограничение минимальных и максимальных размеров окна
	lpMMI->ptMinTrackSize.x = 300;
	lpMMI->ptMinTrackSize.y = 200;
	CFrameWnd::OnGetMinMaxInfo(lpMMI);
}
void BaseChart::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	RECT rt;
	statusBar.GetClientRect(&rt);
	SetRects(cx, cy-rt.bottom);
	SetScrollBar();
}
void BaseChart::SetRects(int cx, int cy)
{
	plotMoveZone1.top = cy - time_axis_width+1;
	plotMoveZone1.bottom = cy;
	//если ползунок был в крайнем правом положении или дальше крайнего правого, то отавляем в крайнем правом
	if (price_axis_width && (plotMoveZone1.right >= clientRect.right - price_axis_width || plotMoveZone1.right > cx - price_axis_width))
	{
		plotMoveZone1.right = cx - price_axis_width;
		plotMoveZone1.left = cx - price_axis_width - 5;
	}
	else if (price_axis_width && plotMoveZone1.right < (cx - price_axis_width)/2) //если получается для нового размера окна сдвинут был за середину, то оставляем середину
	{
		plotMoveZone1.right = (cx - price_axis_width)/2;
		plotMoveZone1.left = (cx - price_axis_width)/2 - 5;
	}

	plotZone.bottom = cy - time_axis_width;
	if (price_axis_width && (plotZone.right >= clientRect.right - price_axis_width || plotZone.right > cx - price_axis_width))
		plotZone.right = cx - price_axis_width;
	else if (price_axis_width && plotZone.right < (cx - price_axis_width)/2)
		plotZone.right = (cx - price_axis_width)/2;
	plotZone.left = plotZone.top = 0;

	plotZoneAdd.bottom = cy - time_axis_width;
	plotZoneAdd.top = 0;
	plotZoneAdd.left = plotZone.right;
	plotZoneAdd.right = cx - price_axis_width;

	clientRect.left = clientRect.top = 0;
	clientRect.right = cx;
	clientRect.bottom = cy;

	priceZone.left = clientRect.right - price_axis_width; 
	priceZone.top = 0;
	priceZone.right = clientRect.right;
	priceZone.bottom = clientRect.bottom - time_axis_width;
	
	timeZone.left = 0;
	timeZone.top = clientRect.bottom - time_axis_width;
	timeZone.right = clientRect.right - price_axis_width;
	timeZone.bottom = clientRect.bottom;

	scalePlotZone.bottom = plotZone.bottom;
	scalePlotZone.right = plotZone.right + Xscale;
	scalePlotZone.left = scalePlotZone.top = 0;

	scaleClientRect.bottom = clientRect.bottom;
	scaleClientRect.right = clientRect.right + Xscale;
	scaleClientRect.left = scaleClientRect.top = 0;
}

void BaseChart::OnTimer(UINT_PTR nIDEvent)
{
	if (instrumentPtr)
	{
		if ((subInstrumentPtr == NULL && timerIndex != instrumentPtr->GetAggregatedOrderbookSize()) || (subInstrumentPtr && timerIndex != subInstrumentPtr->GetAggregatedOrderbookSize()))
		{
			if (subInstrumentPtr == NULL)
			{
				timerIndex = instrumentPtr->GetAggregatedOrderbookSize();
			}
			else timerIndex = subInstrumentPtr->GetAggregatedOrderbookSize();

			if (subInstrumentPtr)
            {
				FillTradesOnlyVector();
            }
			
			size_t start = SetScrollBar();
			start += scalePlotZone.right/*-1*/;
			

			if (tradesOnly || (start > timerIndex || start == timerIndex)) //это условие необходимо чтобы не было холостых отрисовок, когда скроллбар в середине
				Invalidate(0);
		}
	}
	else
	{
		::AcquireSRWLockShared(&SRWLockRithmic);

		switch (fType)
		{
		case RITHMIC:
			for (size_t x = 0; x < rithmic_data.size(); ++x)
			{
				if (!rithmic_data[x]->GetName().compare(name)) { instrumentPtr = rithmic_data[x]; break; }
			}
			break;
		case CQG:
			for (size_t x = 0; x < cqg_data.size(); ++x)
			{
				if (!cqg_data[x]->GetName().compare(name)) { instrumentPtr = cqg_data[x]; break; }
			}
			break;
		case NINJATRADER:
			for (size_t x = 0; x < ninja_data.size(); ++x)
			{
				if (!ninja_data[x]->GetName().compare(name)) { instrumentPtr = ninja_data[x]; break; }
			}
			break;
		case PLAZA2REPLAY:
        case PLAZA2:
            {
                Plaza2Instrument* instrument = NULL;
                instrument = Plaza2DB::GetInstance()->GetInstrumentPtrByName(name);
                if (instrument != NULL)
                {
                    instrumentPtr = new Plaza2TickerAdapterClass(instrument->GetKey());
                }
            }
            break;
		}
		
		::ReleaseSRWLockShared(&SRWLockRithmic);

		Invalidate(false);
	}

	CFrameWnd::OnTimer(nIDEvent);
}

int BaseChart::SetScrollBar()
{
	SCROLLINFO info = { sizeof(SCROLLINFO), SIF_ALL, 0, 0, 0, 0, 0 };
	::GetScrollInfo(m_hWnd, SB_HORZ, &info);

	int width = scalePlotZone.right - scalePlotZone.left;
	if (info.nMax == 100 && instrumentPtr && (int)instrumentPtr->GetAggregatedOrderbookSize() > width)	// это происходит при первом вызове этой функции
		info.nPos = (int)instrumentPtr->GetAggregatedOrderbookSize() - scalePlotZone.right;
	
	if (instrumentPtr && info.nPos == info.nMax - info.nPage + 1)
		info.nPos = (int)instrumentPtr->GetAggregatedOrderbookSize() - scalePlotZone.right;
    info.fMask = SIF_ALL;   
	info.nMin = 0;	
	if (instrumentPtr)
		info.nMax = (int)instrumentPtr->GetAggregatedOrderbookSize()-1;	
	info.nPage = scalePlotZone.right;	
	if ((int)info.nPage > info.nMax)
		info.nPos = info.nMin;

	::SetScrollInfo(m_hWnd, SB_HORZ, &info, TRUE);

	//это решает проблему правильной перерисовки графика, когда впервые появляется горизонтальный скролбар
	RECT rts, rtw;
	statusBar.GetClientRect(&rts);
	GetClientRect(&rtw);
	SetRects(rtw.right, rtw.bottom-rts.bottom);

	return info.nPos;
}
void BaseChart::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO info;
	info.cbSize = sizeof(SCROLLINFO);
    info.fMask = SIF_ALL;
	GetScrollInfo(SB_HORZ, &info, SIF_ALL);

    int newPos = info.nPos;
    switch (nSBCode)
    {
    case SB_LEFT:
        newPos = info.nMin;
        break;
    case SB_RIGHT:
        newPos = info.nMax;
        break;
    case SB_LINELEFT:
        newPos -= (info.nPage > 10) ? info.nPage / 10 : 1;
        break;
    case SB_LINERIGHT:
        newPos += (info.nPage > 10) ? info.nPage / 10 : 1;
        break;
    case SB_PAGELEFT:
        newPos -= info.nPage;
        break;
    case SB_PAGERIGHT:
        newPos += info.nPage;
        break;
    case SB_THUMBTRACK:
        newPos = info.nTrackPos;
        break;
    }
    if (newPos < info.nMin) 
		newPos = info.nMin;
	if (newPos > (info.nMax - (int)info.nPage + 1)) 
		newPos = info.nMax - (int)info.nPage + 1;
	
	SetScrollPos(SB_HORZ, newPos);
	//ScrollWindow(0, nPos);
	Invalidate(false);
	::UpdateWindow(m_hWnd);
}

void BaseChart::FindMaxMinPricesForTrades(HDC dc, const size_t start, const size_t end, Price& maxP, Price& minP, Price& lastP)
{
	for (size_t x = 0, y = start; y <= end; y++, x++)
	{	
		//ищем максимальную цену
		if (instrumentPtr->Get_tradePrice(y).IsPrice() && maxP < instrumentPtr->Get_tradePrice(y))
			maxP = instrumentPtr->Get_tradePrice(y);
		
		//ищем минимальную цену
		if (instrumentPtr->Get_tradePrice(y).IsPrice() &&  minP > instrumentPtr->Get_tradePrice(y))
			minP = instrumentPtr->Get_tradePrice(y);
		
		//рисуем вертикальные оси с метками и надписями
		if (instrumentPtr->Get_yAxis(y))
		{
			TCHAR time[64];
			StringCchPrintf(time, 64, L"[%.2d:%.2d:%.2d]", instrumentPtr->Get_hour(y), instrumentPtr->Get_minute(y), instrumentPtr->Get_second(y));

			if (showGridLines)
			{
				HPEN gridPen = ::CreatePen(PS_DOT, 1, gridColor);
				::SelectObject(dc, (HPEN)gridPen);
				MoveToEx(dc, (int)x,  0, NULL);
				LineTo(dc, (int)x, scalePlotZone.bottom);
				::DeleteObject(gridPen);
			}

			::SelectObject(dc, (HPEN)::GetStockObject(DC_PEN));
			MoveToEx(dc, (int)x, scalePlotZone.bottom, NULL);
			LineTo(dc, (int)x, scalePlotZone.bottom + 5);			
			::TextOutW(dc, (int)x, scaleClientRect.bottom - time_axis_width/3, time, (int)_tcslen(time));			
		}
	}	

	lastP = instrumentPtr->Get_tradePrice(end);
}
void BaseChart::VerticScaleTrade(Price& scaleMaxPrice, Price& scaleMinPrice, Price& lastPrice)
{
	if (YscaleTop > 0)
		for (int x = 1; x <= YscaleTop; ++x)
			instrumentPtr->IncPrice(scaleMaxPrice);
	else if (YscaleTop < 0)
	{
		for (int x = -1; x >= YscaleTop; --x)
		{
			if (scaleMaxPrice > lastPrice)
				instrumentPtr->DecPrice(scaleMaxPrice);
				
			if (scaleMaxPrice == lastPrice)
			{
				instrumentPtr->IncPrice(scaleMaxPrice);
				YscaleTop = ++x;
				break;
			}
		}
	}

	if (YscaleBottom > 0)
		for (int x = 1; x <= YscaleBottom; ++x)
			instrumentPtr->DecPrice(scaleMinPrice);
	else if (YscaleBottom < 0)
	{
		for (int x = -1; x >= YscaleBottom; --x)
		{
			if (scaleMinPrice < lastPrice)
				instrumentPtr->IncPrice(scaleMinPrice);
				
			if (scaleMinPrice == lastPrice)
			{
				instrumentPtr->DecPrice(scaleMinPrice);
				YscaleBottom = ++x;
				break;
			}
		}
	}
	
	if (scale_type == STATIC_PRICE_RANGE)
	{
		if (lastPrice > scaleMaxPrice)
		{
			UINT diff = instrumentPtr->PriceDifference(lastPrice, scaleMaxPrice);
			while (diff--)
				instrumentPtr->IncPrice(scaleMinPrice);
			scaleMaxPrice = lastPrice;
		}
		else if (lastPrice < scaleMinPrice)
		{
			UINT diff = instrumentPtr->PriceDifference(scaleMinPrice, lastPrice);
			while (diff--) 
				instrumentPtr->DecPrice(scaleMaxPrice);
			scaleMinPrice = lastPrice;
		}
		YscaleTop = 0;
		YscaleBottom = 0;
	}

	topPriceRange = scaleMaxPrice;
	bottomPriceRange = scaleMinPrice;
}
int BaseChart::FindMaxMinPricesForDOM(HDC dc, const size_t start, const size_t end, Price& maxP, Price& minP, Price& lastP, Price& lastBidP, Price& lastAskP)
{
	//CStopwatch stopwatch;	//для старта измерения времени этого участка кода

	//готовим данные для потоков
	std::vector<std::vector<std::pair<size_t,size_t>>> timeLabelsVector;
	timeLabelsVector.resize(iNumberOfProcessors);
	std::vector<Price> maxPoolPrice;	maxPoolPrice.resize(iNumberOfProcessors);
	std::vector<Price> minPoolPrice;	minPoolPrice.resize(iNumberOfProcessors);
	for (DWORD i = 0; i < iNumberOfProcessors; ++i)
	{
		maxPoolPrice[i].SetPrice((int)INT_MAX, (UINT)0, true);
		minPoolPrice[i].SetPrice((int)INT_MAX, (UINT)0);
	}
	std::vector<Price> lastBidPoolPrice;	lastBidPoolPrice.resize(iNumberOfProcessors);
	std::vector<Price> lastAskPoolPrice;	lastAskPoolPrice.resize(iNumberOfProcessors);

	tPool.SetBaseData(this, start, end, &timeLabelsVector, &maxPoolPrice, &minPoolPrice, &lastBidPoolPrice, &lastAskPoolPrice);

	//вычисляем диапазон для текущего потока
	size_t diff = end - start, distance, _start, _end, x;
	if (diff < 200 || iNumberOfProcessors == 1)
	{
		_start = start;
		_end = end;
		x = 0;
	}
	else
	{	
		//запускаем параллельные потоки
		for (DWORD p = 0; p < iNumberOfProcessors-1; ++p)
			tPool.StartNewThread(1);

		distance = double(diff / iNumberOfProcessors) + 0.5;
		_start = start + distance * (iNumberOfProcessors - 1);
		_end = end;
		x = distance * (iNumberOfProcessors - 1);
	}

	//находим максимальные и минимальные цены
	FindMaxMinPriceInRange(_start, _end, x, instrumentPtr, maxP, minP, scale_type, lastBidP, lastAskP, &lastP, &timeLabelsVector, iNumberOfProcessors - 1);

	//ждем пока отработают все потоки
	tPool.WaitForThread(1);

	//рисуем временные метки
	for(size_t q = 0; q < timeLabelsVector.size(); ++q)
	{
		for(size_t t = 0; t < timeLabelsVector[q].size(); ++t)
		{
			size_t y = timeLabelsVector[q][t].first;
			size_t x = timeLabelsVector[q][t].second;

			TCHAR time[32];
			StringCchPrintf(time, 32, L"[%.2d:%.2d:%.2d]", instrumentPtr->Get_hour(y), instrumentPtr->Get_minute(y), instrumentPtr->Get_second(y));
			if (showGridLines)
			{
				HPEN gridPen = ::CreatePen(PS_DOT, 1, gridColor);
				::SelectObject(dc, (HPEN)gridPen);
				MoveToEx(dc, (int)x,  0, NULL);
				LineTo(dc, (int)x, scalePlotZone.bottom);
				::DeleteObject(gridPen);
			}
			::SelectObject(dc, (HPEN)::GetStockObject(DC_PEN));
			MoveToEx(dc, (int)x, scalePlotZone.bottom, NULL);
			LineTo(dc, (int)x, scalePlotZone.bottom + 5);
			::TextOut(dc, (int)x , scaleClientRect.bottom - time_axis_width/3, time, (int)_tcslen(time));
		}
		timeLabelsVector[q].resize(0);
	}

	//получаем итоговые значения после распараллеливания
	for (DWORD i = 0; i < iNumberOfProcessors-1; ++i)
	{
		if (maxPoolPrice[i].IsPrice() && maxP < maxPoolPrice[i])
			maxP = maxPoolPrice[i];

		if (minPoolPrice[i].IsPrice() && minP > minPoolPrice[i])
			minP = minPoolPrice[i];

		//if (scale_type == VISIBLE_RANGE)
		//{
			if (lastAskPoolPrice[i].IsPrice() && lastAskP < lastAskPoolPrice[i])
				lastAskP = lastAskPoolPrice[i];

			if (lastBidPoolPrice[i].IsPrice() && lastBidP > lastBidPoolPrice[i])
				lastBidP = lastBidPoolPrice[i];
		//}
	}

	/*__int64 qwElapsedTime = stopwatch.Now();
	char delay[32];
	sprintf_s(delay, "%Id\n", qwElapsedTime);
#ifdef _DEBUG
	WriteToFile(delay, "FindMaxValueDelay_debug_Pool.txt", true);
#else
	WriteToFile(delay, "FindMaxValueDelay_release_Pool.txt", true);
#endif*/

	if (!minP.IsPrice() || !maxP.IsPrice())
		return 1;

	if (scale_type != VISIBLE_RANGE)
	{
		if (instrumentPtr->Get_tradePrice(end).IsPrice() && instrumentPtr->Get_agressorSide(end) == 'B')
			lastAskP = instrumentPtr->Get_tradePrice(end);
		else lastAskP = instrumentPtr->Get_askPrice(end, 0);

		if (instrumentPtr->Get_tradePrice(end).IsPrice() && instrumentPtr->Get_agressorSide(end) == 'S')
			lastBidP = instrumentPtr->Get_tradePrice(end);
		else lastBidP = instrumentPtr->Get_bidPrice(end, 0);
	}

	return 0;
}
void BaseChart::VerticScaleDOM(Price& scaleMaxPrice, Price& scaleMinPrice, Price& lastAskPrice, Price& lastBidPrice)
{
	if (YscaleTop > 0)
		for (int x = 1; x <= YscaleTop; ++x)
			instrumentPtr->IncPrice(scaleMaxPrice);
	else if (YscaleTop < 0)
	{
		for (int x = -1; x >= YscaleTop; --x)
		{
			if (scaleMaxPrice >= lastAskPrice)
				instrumentPtr->DecPrice(scaleMaxPrice);
				
			if (scaleMaxPrice < lastAskPrice)
			{
				instrumentPtr->IncPrice(scaleMaxPrice);
				YscaleTop = ++x;
				break;
			}
		}
	}

	if (YscaleBottom > 0)
		for (int x = 1; x <= YscaleBottom; ++x)
			instrumentPtr->DecPrice(scaleMinPrice);
	else if (YscaleBottom < 0)
	{
		for (int x = -1; x >= YscaleBottom; --x)
		{
			if (scaleMinPrice <= lastBidPrice)
				instrumentPtr->IncPrice(scaleMinPrice);
				
			if (scaleMinPrice > lastBidPrice)
			{
				instrumentPtr->DecPrice(scaleMinPrice);
				YscaleBottom = ++x;
				break;
			}
		}
	}
	
	if (scale_type == STATIC_PRICE_RANGE)
	{
		if (lastAskPrice > scaleMaxPrice)
		{
			UINT diff = instrumentPtr->PriceDifference(lastAskPrice, scaleMaxPrice);
			while (diff--)
				instrumentPtr->IncPrice(scaleMinPrice);
			scaleMaxPrice = lastAskPrice;
		}
		else if (lastBidPrice < scaleMinPrice)
		{
			UINT diff = instrumentPtr->PriceDifference(scaleMinPrice, lastBidPrice);
			while (diff--) 
				instrumentPtr->DecPrice(scaleMaxPrice);
			scaleMinPrice = lastBidPrice;
		}
		YscaleTop = 0;
		YscaleBottom = 0;
	}

	topPriceRange = scaleMaxPrice;
	bottomPriceRange = scaleMinPrice;
}
SIZE BaseChart::DrawHorizAxis(HDC dc, Price scaleMaxPrice, const double count)
{
	SIZE sSize;
	
	::GetTextExtentPoint32(dc, scaleMaxPrice.GetWString(instrumentPtr->GetPrecision()), (int)_tcslen(scaleMaxPrice.GetWString(instrumentPtr->GetPrecision())), &sSize);
	for (double x = 0, verticScale = sSize.cy; x <= scalePlotZone.bottom; x += count, verticScale += count, instrumentPtr->DecPrice(scaleMaxPrice))
	{
		if (x != plotZone.bottom && showGridLines)	//это проверка нужна чтобы координатная ось не залезала на нижнюю ось временную
		{
			HPEN gridPen = ::CreatePen(PS_DOT, 1, gridColor);
			::SelectObject(dc, (HPEN)gridPen);
			::MoveToEx(dc, 0, x, NULL);
			::LineTo(dc, scalePlotZone.right, x);
			::DeleteObject(gridPen);
		}
		::SelectObject(dc, (HPEN)::GetStockObject(DC_PEN));
		::MoveToEx(dc, scaleClientRect.right - price_axis_width, x, NULL);
		::LineTo(dc, scaleClientRect.right - price_axis_width + 5, x);
		//рисуем цифры цены возле метки, но учитываем размер между осями чтобы цифры не сливались
		if (verticScale >= sSize.cy)
		{
			::TextOut(dc, scaleClientRect.right - price_axis_width/2 + 5, x+sSize.cy/2, scaleMaxPrice.GetWString(instrumentPtr->GetPrecision()), (int)_tcslen(scaleMaxPrice.GetWString(instrumentPtr->GetPrecision())));
			verticScale = 0;
		}
	}	
	
	return sSize;
}
void BaseChart::DrawDOMLabel(HDC dc, UINT size, COLORREF& color1, COLORREF& color2, int Ycoord)
{
	TCHAR s[10]; 
	StringCchPrintf(s, 10, L"%d", size);
	
	SIZE sSize; 
	::GetTextExtentPoint32W(dc, s, (int)_tcslen(s), &sSize);
	
	::SetDCPenColor(dc, color1);
	::SetDCBrushColor(dc, color2);

	int xCoord = scaleClientRect.right - price_axis_width + 6,
		yCoord = Ycoord + sSize.cy/2;	

	::Rectangle(dc, xCoord - 2, yCoord - sSize.cy, xCoord + sSize.cx +2, yCoord);
	
	UINT oldAlign = ::SetTextAlign(dc, TA_NOUPDATECP | TA_LEFT | TA_BOTTOM);
	::TextOut(dc, xCoord, yCoord, s, (int)_tcslen(s));
	::SetTextAlign(dc, oldAlign);
}
void BaseChart::DrawHorizDOM(HDC dc, double count, Price& k, const size_t y, const int x, const Price& scaleMaxPrice)
{
	double bar;

	if (k == instrumentPtr->Get_askPrice(y, x) && instrumentPtr->Get_askSize(y, x))	//аски
	{
		::SetDCPenColor(dc, newAskLimitColor); ::SetDCBrushColor(dc, askLimitColor);
		if (!showOrders)
			bar = (double)instrumentPtr->Get_askSize(y, x) / instrumentPtr->Get_maxDomLimit(y) * (plotZoneAdd.right - plotZoneAdd.left);
		else bar = (double)instrumentPtr->Get_askOrders(y, x) / instrumentPtr->Get_maxDomOrder(y) * (plotZoneAdd.right - plotZoneAdd.left);

		if (x)
			::Rectangle(dc, scaleClientRect.right - price_axis_width - bar+1, (double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count + 1, 
				scaleClientRect.right - price_axis_width, (double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count + count);
		else
			::Rectangle(dc, scaleClientRect.right - price_axis_width - bar+1, (double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count + 1, 
				scaleClientRect.right - price_axis_width, (double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count + count/2);
	}
	else if (k == instrumentPtr->Get_bidPrice(y, x) && instrumentPtr->Get_bidSize(y, x))	//биды
	{
		::SetDCPenColor(dc, newBidLimitColor); ::SetDCBrushColor(dc, bidLimitColor);
		if (!showOrders)
			bar = (double)instrumentPtr->Get_bidSize(y, x) / instrumentPtr->Get_maxDomLimit(y) * (plotZoneAdd.right - plotZoneAdd.left);
		else bar = (double)instrumentPtr->Get_bidOrders(y, x) / instrumentPtr->Get_maxDomOrder(y) * (plotZoneAdd.right - plotZoneAdd.left);

		if (x)
			::Rectangle(dc, scaleClientRect.right - price_axis_width - bar+1, (double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count - 1, 
				scaleClientRect.right - price_axis_width, (double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count - count);
		else
			::Rectangle(dc, scaleClientRect.right - price_axis_width - bar+1, (double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count - 1, 
				scaleClientRect.right - price_axis_width, (double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count - count/2);
	}
}

BOOL BaseChart::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
	//return CFrameWnd::OnEraseBkgnd(pDC);
}

void BaseChart::FillTradesOnlyVector()
{
    if (fType != PLAZA2)
    {
	    //записываем из инструмента subInstrumentPtr в инструмента instrumentPtr только трейды
	    GetTradesOnlyInVector(subInstrumentPtr, instrumentPtr, syncIndex);
    }
    else
    {
        Plaza2Instrument* instrument = NULL;
        size_t arraySize;
        Plaza2Trade* trade = NULL;
        Plaza2Book book;

        if (subInstrumentPtr)
        {
            instrument = ((Plaza2TickerAdapterClass*)subInstrumentPtr)->GetInstrument();
        }
        else
        {
            instrument = ((Plaza2TickerAdapterClass*)instrumentPtr)->GetInstrument();
        }

        if (!aggregatedOrders)  // нет агрегации
        {
            arraySize = instrument->NonAggregatedTradesArraySize();
            if (syncIndex > arraySize)
            {
                size_t maxSize = instrument->NonAggregatedTradesArrayMaxSize();
                syncIndex = maxSize/2 - (maxSize - syncIndex);
            }

            while (syncIndex < arraySize)
            {	
                trade = &instrument->GetNonAggregatedTrade(syncIndex);
                book.timestamp = trade->timestamp;
                book.agressorSide = trade->agressorSide;
                book.trade = trade;
                instrumentPtr->push_back(&book);
                ++syncIndex;
            }
        }
        else
        {
            arraySize = instrument->AggregatedTradesArraySize();
            if (syncIndex > arraySize)
            {
                size_t maxSize = instrument->AggregatedTradesArrayMaxSize();
                syncIndex = maxSize/2 - (maxSize - syncIndex);
            }

            while (syncIndex < arraySize)
            {	
                trade = &instrument->GetAggregatedTrade(syncIndex);
                book.timestamp = trade->timestamp;
                book.agressorSide = trade->agressorSide;
                book.trade = trade;
                instrumentPtr->push_back(&book);
                ++syncIndex;
            }
        }
    }
}

void BaseChart::OnMouseMove(UINT nFlags, CPoint point)
{
	if (lButtonOnPlotArea)					//перемещаем график
	{
		SCROLLINFO info = { sizeof(SCROLLINFO), SIF_POS|SIF_RANGE|SIF_PAGE, 0, 0, 0, 0, 0 };
		::GetScrollInfo(m_hWnd, SB_HORZ, &info);
		info.nPos += lButtonPoint.x - point.x;
		lButtonPoint.x = point.x;

		if (info.nPos < info.nMin) 
			info.nPos = info.nMin;
		if (info.nPos > (info.nMax - (int)info.nPage + 1)) 
			info.nPos = info.nMax - (int)info.nPage + 1;
	
		SetScrollPos(SB_HORZ, info.nPos);
		Invalidate(false);
		//::UpdateWindow(m_hWnd);
		
		CFrameWnd::OnMouseMove(nFlags, point);
		return;
	}
	else if (lButtonOnMoveZone1)			//перемещаем moveZone1
	{
		::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
		//ограничим перемещение
		int delta = point.x - lButtonPoint.x;
		if (plotMoveZone1.right + delta >= (clientRect.right - price_axis_width)/2 && plotMoveZone1.right + delta <= clientRect.right - price_axis_width)
		{
			::OffsetRect(&plotMoveZone1, delta, 0);
			plotZone.right += delta;
			scalePlotZone.right += delta;
			plotZoneAdd.left += delta;
			
			SetScrollBar();
			Invalidate(0);
		}
		lButtonPoint.x = point.x;

		CFrameWnd::OnMouseMove(nFlags, point);
		return;
	}
	else if (lButtonOnTimeZone)					//изменяем горизонтальный масштаб
	{
		if (!(point.x % 5))
		{
			int delta = point.x - lButtonPoint.x;
			Xscale -= delta*10;
			if (Xscale > 3500)				//ограничим максмальный горизонтальный масштаб
				Xscale = 3500;

			lButtonPoint.x = point.x;
			if (Xscale >= 0)
			{
				RECT rt;
				::GetClientRect(m_hWnd, &rt);
				SetRects(rt.right, rt.bottom);
				SetScrollBar();
				Invalidate(0);
				//UpdateWindow();
			}
			else Xscale = 0;
		}		

		CFrameWnd::OnMouseMove(nFlags, point);
		return;
	}
	else if (lButtonOnPriceZone)			//изменяем вертикальный масштаб
	{
		if ((GetKeyState(VK_LCONTROL) & 0x1000) && !(point.y % 5))	//если нажат левый Ctrl
		{
			int delta = point.y - lButtonPoint.y;
			if (delta > 0)
			{
				++YscaleTop;
				++YscaleBottom;
			}
			else
			{
				--YscaleBottom;
				--YscaleTop;
			}
			lButtonPoint.y = point.y;
			Invalidate(0);
			//UpdateWindow();
		}
		else if (!(point.y % 5))
		{
			int delta = point.y - lButtonPoint.y;
			
			if (YlastBidAskFlag)	//нажатие выше офера
			{
				if (delta > 0)
					++YscaleTop;
				else --YscaleTop;
			}
			else					//нажатие ниже бида
			{
				if (delta > 0)
					--YscaleBottom;
				else ++YscaleBottom;
			}
			
			lButtonPoint.y = point.y;
			Invalidate(0);
			//UpdateWindow();
		}

		if (scale_type == VISIBLE_RANGE)
		{
			/*if (_i->IsTradesOnly())
			{
				if (!YscaleTop) YscaleTop = 1;
				if (!YscaleBottom) YscaleBottom = 1;
			}*/
		}			

		CFrameWnd::OnMouseMove(nFlags, point);
		return;
	}
	else  //перемещений с нажатой кнопкой нет
	{	
		if (::PtInRect(&plotMoveZone1, point)) //навели мышь на moveZone1
		{
			::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
			if (!mouseOnMoveZone1)
			{
				HDC dc = ::GetDC(m_hWnd);
				::InvertRect(dc, &plotMoveZone1);
				::DeleteDC(dc);
			}
			mouseOnMoveZone1 = true;
		} 
		else if (mouseOnMoveZone1) //убрали мышь с moveZone1
		{
			::SetCursor(::LoadCursor(NULL, IDC_ARROW));
			mouseOnMoveZone1 = false;
			HDC dc = ::GetDC(m_hWnd);
			::InvertRect(dc, &plotMoveZone1);
			::DeleteDC(dc);
		}
		else if(::PtInRect(&scaleButtonRect, point))
		{
			mouseOnScaleButton = true;
		}
		else mouseOnScaleButton = false;
	}	

	CFrameWnd::OnMouseMove(nFlags, point);
}
void BaseChart::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	lButtonPoint = point;
	
	if (::PtInRect(&plotZone, point))
		lButtonOnPlotArea = true;
	else if (::PtInRect(&plotMoveZone1, point))
		lButtonOnMoveZone1 = true;
	else if (::PtInRect(&timeZone, point))
		lButtonOnTimeZone = true;
	else if (::PtInRect(&priceZone, point))
	{
		if ( !(GetKeyState(VK_LCONTROL) & 0x1000) )	//если не нажат левый Ctrl
		{
			if (YlastAsk > point.y)			//нажатие выше офера
				YlastBidAskFlag = true;	
			else if (YlastBid < point.y)	//нажатие ниже бида
				YlastBidAskFlag = false;
		}
		lButtonOnPriceZone = true;
	}
	else if (::PtInRect(&scaleButtonRect, point) && mouseOnScaleButton)
	{
		//сброс вертикального масштаба
		YscaleTop = 0;
		YscaleBottom = 0;

		//сброс горизонтального масштаба
		Xscale = 0;
		RECT rt;
		::GetClientRect(m_hWnd, &rt);
		SetRects(rt.right, rt.bottom);
		SetScrollBar();
		Invalidate(0);
	}

	CFrameWnd::OnLButtonDown(nFlags, point);
}
void BaseChart::OnLButtonUp(UINT nFlags, CPoint point)
{
	lButtonOnPlotArea = false;
	lButtonOnMoveZone1 = false;
	lButtonOnTimeZone = false;
	lButtonOnPriceZone = false;
	ReleaseCapture();

	CFrameWnd::OnLButtonUp(nFlags, point);
}


