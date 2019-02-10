#include "stdafx.h"

#include "DEFINE.h"
#include "Algovisor.h"
#include "IPlaza2_Connector.h"
#include "RithmicDialog.h"
#include "RithmicCallbacks.h"
#include "CQGDialog.h"
#include "NinjaDialog.h"
#include "ConnectionDialog.h"
#include "Plaza2Dialog.h"
#include "Plaza2ReplayDialog.h"
#include "QuoteRun.h"
#include "Quant_Chart.h"
#include "Heat_Map.h"
#include "Auxiliary_methods.h"
#include "Instrument.h"
#include "Plaza2Instruments.h"
#include "Plaza2Callbacks.h"
#include "MatchingData.h"

#include "MainWindow.h"

extern CAAApp theApp;
MainWindow* pMainWnd = nullptr;

MainWindow::MainWindow(CWnd* pParent /*=NULL*/) : 
	CDialogEx(MainWindow::IDD, pParent), 
	rithmicCallbacks(nullptr),
	rithmicDlg(nullptr),
	cqgDlg(nullptr),
	ninjaDlg(nullptr),
	cqgIp(nullptr),
	/*ninjaIdata(NULL),*/
	plaza2Dlg(nullptr),
	plaza2ReplayDlg(nullptr)
{
	/*char durstr[100];
	std::vector<int> vec;
	vec.push_back(1);
	
	for (int c = 0; c < 30; ++c)
	{
		CStopwatch stopwatch;
		int i = vec[0];
		sprintf_s(durstr, 100, "Vector duration: %d", stopwatch.Now());
		FUNCTION_NAME_TRACER(durstr);
	}

	std::map<int, int> int_map;
	int_map.insert(std::make_pair<int, int> (1,1));

	for (int c = 0; c < 30; ++c)
	{
		CStopwatch stopwatch;
		int i = int_map[1];
		sprintf_s(durstr, 100, "Map duration: %d", stopwatch.Now());
		FUNCTION_NAME_TRACER(durstr);
	}

	int massiv[1];
	massiv[0] = 1;
	for (int c = 0; c < 30; ++c)
	{
		CStopwatch stopwatch;
		int i = massiv[0];
		sprintf_s(durstr, 100, "Massiv duration: %d", stopwatch.Now());
		FUNCTION_NAME_TRACER(durstr);
	}*/	//это для отладки

	//SetBackgroundColor(BGRNCOLOR, false);

    pMainWnd = this;
	
	std::vector<CString> str; 
	
	str.push_back(L"6A - Australian Dollar");			str.push_back(L"CME");		str.push_back(L"6A");		str.push_back(L"DA6");	vecSymbol.push_back(str); str.resize(0);
	str.push_back(L"6B - British Pound");				str.push_back(L"CME");		str.push_back(L"6B");		str.push_back(L"BP6");	vecSymbol.push_back(str); str.resize(0);
	str.push_back(L"6C - Canadian Dollar");				str.push_back(L"CME");		str.push_back(L"6C");		str.push_back(L"CA6");	vecSymbol.push_back(str); str.resize(0);
	str.push_back(L"6E - Euro FX");						str.push_back(L"CME");		str.push_back(L"6E");		str.push_back(L"EU6");	vecSymbol.push_back(str); str.resize(0);
	str.push_back(L"M6E - E-Micro EUR/USD");			str.push_back(L"CME");		str.push_back(L"M6E");		str.push_back(L"M6E");	vecSymbol.push_back(str); str.resize(0);
	str.push_back(L"6J - Japanese Yen");				str.push_back(L"CME");		str.push_back(L"6J");		str.push_back(L"JY6");	vecSymbol.push_back(str); str.resize(0);
	str.push_back(L"6S - Swiss Franc");					str.push_back(L"CME");		str.push_back(L"6S");		str.push_back(L"SF6");	vecSymbol.push_back(str); str.resize(0);
	str.push_back(L"DX - Dollar Index");				str.push_back(L"NYBOT");	str.push_back(L"DX");		str.push_back(L"DXE");	vecSymbol.push_back(str); str.resize(0);
	str.push_back(L"ES - S&P 500 E-mini");				str.push_back(L"CME");		str.push_back(L"ES");		str.push_back(L"EP");	vecSymbol.push_back(str); str.resize(0);
	str.push_back(L"NQ - Nasdaq");						str.push_back(L"CME");		str.push_back(L"NQ");		str.push_back(L"ENQ");	vecSymbol.push_back(str); str.resize(0);
	str.push_back(L"YM - Mini-Dow");					str.push_back(L"CBOT");		str.push_back(L"YM");		str.push_back(L"YM");	vecSymbol.push_back(str); str.resize(0);
	str.push_back(L"NKD - Nikkei 225");					str.push_back(L"CME");		str.push_back(L"NKD");		str.push_back(L"NKD");	vecSymbol.push_back(str); str.resize(0);
	str.push_back(L"TF - Russell 2000 (ICE)");			str.push_back(L"NYBOT");	str.push_back(L"TF");		str.push_back(L"TFE");	vecSymbol.push_back(str); str.resize(0);
	str.push_back(L"CL - Crude Oil");					str.push_back(L"NYMEX");	str.push_back(L"CL");		str.push_back(L"CLE");	vecSymbol.push_back(str); str.resize(0);
	str.push_back(L"GC - Gold");						str.push_back(L"COMEX");	str.push_back(L"GC");		str.push_back(L"GCE");	vecSymbol.push_back(str); str.resize(0);
	str.push_back(L"SI - Silver");						str.push_back(L"COMEX");	str.push_back(L"SI");		str.push_back(L"SIE");	vecSymbol.push_back(str); str.resize(0);
	str.push_back(L"HG - Copper");						str.push_back(L"COMEX");	str.push_back(L"HG");		str.push_back(L"CPE");	vecSymbol.push_back(str); str.resize(0);
	str.push_back(L"PL - Platinum");					str.push_back(L"NYMEX");	str.push_back(L"PL");		str.push_back(L"PL");	vecSymbol.push_back(str); str.resize(0);
	str.push_back(L"ZC - Corn");						str.push_back(L"CBOT");		str.push_back(L"ZC");		str.push_back(L"ZCE"); 	vecSymbol.push_back(str); str.resize(0);
	str.push_back(L"ZS - Soybeans");					str.push_back(L"CBOT");		str.push_back(L"ZS");		str.push_back(L"ZSE"); 	vecSymbol.push_back(str); str.resize(0);
	str.push_back(L"ZW - Wheat");						str.push_back(L"CBOT"); 	str.push_back(L"ZW");		str.push_back(L"ZWA");	vecSymbol.push_back(str); str.resize(0);
	str.push_back(L"ZL - Soybean Oil");					str.push_back(L"CBOT");		str.push_back(L"ZL");		str.push_back(L"ZLE");	vecSymbol.push_back(str); str.resize(0);
	str.push_back(L"ZN - US 10-Year Treasury Note");	str.push_back(L"CBOT");		str.push_back(L"ZN");		str.push_back(L"TYA"); 	vecSymbol.push_back(str); str.resize(0);
	str.push_back(L"ZB - US 30-Year Treasury Bond");	str.push_back(L"CBOT");		str.push_back(L"ZB");		str.push_back(L"USA");	vecSymbol.push_back(str); str.resize(0);
	str.push_back(L"FGBL - Euro Bond 10-Year");			str.push_back(L"EUREX");	str.push_back(L"FGBL");		str.push_back(L"DB");	vecSymbol.push_back(str); str.resize(0);

	vecSymbol.shrink_to_fit();

	rithmicStatus = -1;
	cqgStatus = -1;
	ninjaStatus = -1;
	cqgSubscribeStatus = false;

	spreadLegTrades = FALSE; //по-умолчанию спрэд-леги для ритмика выключены

	//узнаем кол-во физических ядер процессора
	iNumberOfProcessors = NumberOfProcessors();
}

DWORD MainWindow::NumberOfProcessors()
{
	//узнаем кол-во физических ядер процессора
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
	BOOL done = FALSE;
	DWORD returnLength = 0;
	DWORD byteOffset = 0;
    DWORD processorCoreCount = 0;
	DWORD CountSetBits(ULONG_PTR bitMask);

	while (!done)
    {
        DWORD rc = GetLogicalProcessorInformation(buffer, &returnLength);

        if (FALSE == rc) 
        {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) 
            {
                if (buffer) 
                    free(buffer);

                buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(returnLength);
            } 
            else 
            {
               break;
            }
        } 
        else
        {
            done = TRUE;
        }
    }

	void* pBuffer = (void*)buffer;
	
	while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength) 
	{
		switch (buffer->Relationship) 
		{
		case RelationProcessorCore:
			processorCoreCount++;
			break;
		}
		byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
		buffer++;
	}	

	if (pBuffer)
		free(pBuffer);

	//если процессор одноядерный то ничего не меняем
	if (processorCoreCount > 1)
		return processorCoreCount-1;
	else 
		return 1;
}

// Helper function to count set bits in the processor mask.
//DWORD CountSetBits(ULONG_PTR bitMask)
//{
//    DWORD LSHIFT = sizeof(ULONG_PTR)*8 - 1;
//    DWORD bitSetCount = 0;
//    ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;    
//    DWORD i;
//    
//    for (i = 0; i <= LSHIFT; ++i)
//    {
//        bitSetCount += ((bitMask & bitTest)?1:0);
//        bitTest/=2;
//    }
//
//    return bitSetCount;
//}

MainWindow::~MainWindow() 
{
	//отключаемся от плазы 2 если было подключение (похоже это код нужно удалить, отключение раньше происходит)
	if (p2Connector)
	{
		p2Connector->Disconnect();
        p2Connector.reset();
	}

	delete rithmicCallbacks;

	delete cqgIp;
	//delete ninjaIdata;

	HWND hwn = ::FindWindow(_T("CQG_Algovisor_Server"), NULL);
	if (hwn)
	{
		//выключаем CQG_Server.exe
		COPYDATASTRUCT cdStruct = { 0, CQGDialog::CQG_SHUTDOWN, NULL };
		::SendMessage(hwn, WM_DESTROY, NULL, (LPARAM)(LPVOID)&cdStruct);
	}

	Disconnect_Algovisor_Server();
}

void MainWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ACTIVITYMONITOR, activityMonitor);
	DDX_Control(pDX, MONTH_COMBOBOX, comboBoxMonth);
	DDX_Control(pDX, YEAR_COMBOBOX, comboBoxYear);
	DDX_Control(pDX, INSTRUMENT_LIST, instrumentList);
	DDX_Control(pDX, SYMBOL_COMBOBOX, comboSymbol);
	DDX_Control(pDX, DATAFEED_COMBOBOX, comboFeedType);
	DDX_Control(pDX, IDC_CONNECTION_STATUS_FEED1, rithmic_connection_indicator);
	DDX_Control(pDX, IDC_CONNECTION_STATUS_FEED2, cqg_connector_indicator);
	DDX_Control(pDX, IDC_CONNECTION_STATUS_FEED3, ninjatrader_connection_indicator);
	DDX_Control(pDX, IDC_CONNECTION_STATUS_PLAZA2, plaza2_connection_indicator);
	DDX_Control(pDX, IDC_RITHMIC_LICENSE_LOGO, rithmicLogo);
	DDX_Control(pDX, IDC_OMNE_LICENSE_LOGO, omneLogo);
}

BEGIN_MESSAGE_MAP(MainWindow, CDialogEx)
	ON_WM_DRAWITEM()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(BUTTON_CLEAR_ACTIVITI_MONITOR, &MainWindow::OnBnClickedButtonClear)
	ON_BN_CLICKED(BUTTON_QUOTE_RUN, &MainWindow::OnBnClickedQuoteRun)
	ON_BN_CLICKED(BUTTON_QUANT_CHART, &MainWindow::OnBnClickedQuantChart)
	ON_BN_CLICKED(BUTTON_QUOTE_MAPPER, &MainWindow::OnBnClickedHeatMap)
	ON_WM_COPYDATA()
	ON_BN_CLICKED(BUTTON_SUBSCRIBE, &MainWindow::OnBnClickedSubscribe)
	ON_BN_CLICKED(BUTTON_GETHISTORY, &MainWindow::OnBnClickedGethistory)
	ON_BN_CLICKED(BUTTON_REMOVE, &MainWindow::OnBnClickedUnsubscribe)
	ON_BN_CLICKED(BUTTON_REMOVE_ALL, &MainWindow::OnBnClickedUnsubscribeAll)
	ON_MESSAGE(WM_POWERBROADCAST, &MainWindow::OnPowerMsgRcvd)
	
	ON_UPDATE_COMMAND_UI(BUTTON_SUBSCRIBE, OnUpdateSubscribe)
	ON_UPDATE_COMMAND_UI(BUTTON_GETHISTORY, OnUpdateGetHistoricalData)
	ON_UPDATE_COMMAND_UI(BUTTON_REMOVE, OnUpdateUnSubscribe)
	ON_UPDATE_COMMAND_UI(BUTTON_REMOVE_ALL, OnUpdateUnSubscribe)
	ON_UPDATE_COMMAND_UI(DATAFEED_COMBOBOX, OnDatafeedComboBox)
	
	ON_UPDATE_COMMAND_UI(SYMBOL_COMBOBOX, OnSymbolComboBox)
	ON_UPDATE_COMMAND_UI(MONTH_COMBOBOX, OnMonthComboBox)
	ON_UPDATE_COMMAND_UI(YEAR_COMBOBOX, OnYearComboBox)

	ON_UPDATE_COMMAND_UI(BUTTON_QUOTE_RUN, OnQuoteRunButton)
	ON_UPDATE_COMMAND_UI(BUTTON_QUANT_CHART, OnQuantChartButton)
	ON_UPDATE_COMMAND_UI(BUTTON_QUOTE_MAPPER, OnQuoteMapperButton)

	ON_COMMAND(ID_CONNECTION_RITHMIC, &MainWindow::OnConnectionRithmic)
	ON_COMMAND(ID_CONNECTION_CQG, &MainWindow::OnConnectionCqg)
	ON_COMMAND(ID_CONNECTION_NINJATRADER, &MainWindow::OnConnectionNinjatrader)
	ON_COMMAND(ID_CONNECTION_PLAZA2, &MainWindow::OnConnectionPlaza2)
	ON_COMMAND(ID_CONNECTION_PLAZAII_REPLAY, &MainWindow::OnPlaza2Replay)
	ON_COMMAND(ID_HELP_ABOUT, OnAppAbout)
	ON_MESSAGE(WM_APP + 1, &MainWindow::CQGInstrumentProperty)
	ON_MESSAGE(WM_APP + 2, &MainWindow::CQGInstrumentTrade)
	ON_MESSAGE(WM_APP + 3, &MainWindow::CQGInstrumentAsk)
	ON_MESSAGE(WM_APP + 4, &MainWindow::CQGInstrumentBid)
	ON_MESSAGE(WM_APP + 5, &MainWindow::CQGInstrumentDOM)
	ON_MESSAGE(WM_APP + 6, &MainWindow::NinjaInstrumentData)
	ON_MESSAGE(WM_APP + 7, &MainWindow::NinjaInstrumentTrade)
	ON_MESSAGE(WM_APP + 8, &MainWindow::NinjaInstrumentAsk)
	ON_MESSAGE(WM_APP + 9, &MainWindow::NinjaInstrumentBid)
	ON_MESSAGE(WM_APP + 10, &MainWindow::NinjaInstrumentDOM)
	ON_MESSAGE(WM_APP + 11, &MainWindow::NinjaInstrumentDELETE)
	ON_MESSAGE(WM_APP + 12, &MainWindow::NinjaGettingData)
	ON_MESSAGE(WM_APP + 13, &MainWindow::InvalidateNinjaConnectionIndicator)
	ON_WM_TIMER()
//	ON_WM_KEYDOWN()
	ON_WM_CLOSE()
	ON_CBN_SELCHANGE(DATAFEED_COMBOBOX, &MainWindow::OnSelchangeDatafeedCombobox)
END_MESSAGE_MAP()

BOOL MainWindow::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//иконка
 	HICON m_hIconSmall = (HICON) ::LoadImageW(theApp.m_hInstance, MAKEINTRESOURCEW(IDR_MAINFRAME), IMAGE_ICON, 16, 16, 0/*LR_DEFAULTCOLOR*/); // Загрузить иконку 16х16
 	HICON m_hIconBig = (HICON) ::LoadImageW(theApp.m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 48, 48, 0/*LR_DEFAULTCOLOR*/); 	// Загрузить иконку 48х48
	SetIcon(m_hIconSmall, FALSE);		// Set small icon
	SetIcon(m_hIconBig, TRUE);			// Set big icon

	//backgroundBrush.CreateSolidBrush(BGRNCOLOR);

	//ставим по умолчанию значения месяца и года 
	comboBoxMonth.SetCurSel(0);
	comboBoxYear.SetCurSel(0);

	//заполняем колонки таблицы инструментов
	instrumentList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_SINGLESEL);
	instrumentList.InsertColumn(0, _T(""), LVCFMT_LEFT, 0);
	instrumentList.InsertColumn(1, _T("Datafeed"), LVCFMT_CENTER, 90);
	instrumentList.InsertColumn(2, _T("Ticker"), LVCFMT_CENTER, 75);
	instrumentList.InsertColumn(3, _T("ID"), LVCFMT_CENTER, 70);
	instrumentList.InsertColumn(4, _T("Connection state"), LVCFMT_CENTER, 95);
	instrumentList.InsertColumn(5, _T("Exchange"), LVCFMT_CENTER, 65);
	instrumentList.InsertColumn(6, _T("Expiration date"), LVCFMT_CENTER, 90);
	instrumentList.InsertColumn(7, _T("Description"), LVCFMT_LEFT, 200);

	//размер буфера для активити монитора
	activityMonitor.SetLimitText(1000000u);

	//пишем стартовый текст в окно активити монитора
	//wchar_t* startText = new wchar_t[50];
	//wsprintf(startText, L"Select the data feed in the Connection menu\n");
	//AfxBeginThread((AFX_THREADPROC)TypeTextToActivMon, (PVOID)(startText));

	//logo for rithmic and omne
	HICON hBitmap = (HICON)LoadImageW(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_RITHMIC_LICENSE_ICON_2), IMAGE_ICON, 134, 28, 0);
	rithmicLogo.SetIcon(hBitmap);
	hBitmap = (HICON)LoadImageW(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_RITHMIC_LICENSE_ICON_3), IMAGE_ICON, 122, 28, 0);
	omneLogo.SetIcon(hBitmap);
	rithmicLogo.ShowWindow(0);
	omneLogo.ShowWindow(0);

#ifdef CONNECT_THROUGH_SERVER
	//запускаем часовой таймер для проверки временной метки о лицензии из реестра
	SetTimer(2, 3600000, NULL);
#endif

	RECT rt;
	::GetWindowRect(GetDlgItem(IDR_MAINMENU)->GetSafeHwnd(), &rt);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void MainWindow::FillInstrumentComboBox(const std::vector<CString>& list)
{
	comboSymbol.ResetContent();
	std::for_each(list.begin(), list.end(), [this](const CString& str) {comboSymbol.AddString(str); });
}

void MainWindow::DisconnectFromFeed(FeedType ft)
{
	switch(ft)
	{
	case RITHMIC:
        {
            for (size_t x = 0; x < rithmic_data.size(); ++x)
            {
                rithmic_data[x]->SetMode("Disconnect");
            }
            for (int x = 0; x < instrumentList.GetItemCount(); ++x)
            {
                if (instrumentList.GetItemText(x, 1) == L"Rithmic")
                    instrumentList.SetItemText(x, 4, L"Disconnect");	
            }
        }
		break;
	case CQG:
        {
		    for (size_t x = 0; x < cqg_data.size(); ++x)
            {
			    cqg_data[x]->SetMode("Disconnect");
            }
            for (int x = 0; x < instrumentList.GetItemCount(); ++x)
            {
                if (instrumentList.GetItemText(x, 1) == L"CQG")
                    instrumentList.SetItemText(x, 4, L"Disconnect");
            }
        }
		break;
	case NINJATRADER:
        {
            for (size_t x = 0; x < ninja_data.size(); ++x)
            {
                ninja_data[x]->SetMode("Disconnect");
            }
            for (int x = 0; x < instrumentList.GetItemCount(); ++x)
            {
                if (instrumentList.GetItemText(x, 1) == L"NinjaTrader")
                    instrumentList.SetItemText(x, 4, L"Disconnect");
            }
        }
		break;
    case PLAZA2:
        {
            for (int x = 0; x < instrumentList.GetItemCount(); ++x)
            {
                if (instrumentList.GetItemText(x, 1) == L"Plaza2")
                    instrumentList.SetItemText(x, 4, L"Disconnect");
            }
        }
        break;
	}
}

void MainWindow::UpdateActivityWindow(const wchar_t* message)
{
	SYSTEMTIME st; 
	wchar_t str[200];

	::GetLocalTime(&st);
    StringCchPrintf(str, 150, _T("[%.2d:%.2d:%.2d.%0.3d] %s\r\n"), st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, message);	
	//activityMonitor.SetFocus();
	//activityMonitor.SetSel(0,-1); // select all text and move cursor at the end
	activityMonitor.SetSel((DWORD)-1);
	//activityMonitor.SetSel(activityMonitor.GetWindowTextLengthW(), activityMonitor.GetWindowTextLengthW() + wcslen(message));
	activityMonitor.ReplaceSel(str);
}
void MainWindow::OnBnClickedButtonClear()
{
	activityMonitor.SetWindowText(L"");
}

void MainWindow::OnOK() {}

IInstrument* MainWindow::GetSelectedInstrument(FeedType& ft)
{
	POSITION p = instrumentList.GetFirstSelectedItemPosition();
	int nItem;
	if (p == NULL) {
		nItem = instrumentList.GetItemCount()-1; //если ничего не выбрано, то выбираем последний инструмент в списке
		//UpdateActivityWindow(L"Select instrument!");
		//return NULL;
	} else {
		while (p)
			nItem = instrumentList.GetNextSelectedItem(p);
	}

	CString feed(instrumentList.GetItemText(nItem, 1));
	CStringA name(instrumentList.GetItemText(nItem, 2));
	size_t i = 0;

	if (feed == _T("Rithmic"))
	{
		ft = RITHMIC;
		while (rithmic_data[i]->GetName().compare((char*)name.GetString()) != 0)
			++i;
		return rithmic_data[i];
	}
	else if (feed == _T("CQG"))
	{
		ft = CQG;
		while (cqg_data[i]->GetName().compare((char*)name.GetString()) != 0)
			++i;
		return cqg_data[i];
	}
	else if (feed == _T("NinjaTrader"))
	{
		ft = NINJATRADER;
		while (ninja_data[i]->GetName().compare((char*)name.GetString()) != 0)
			++i;
		return ninja_data[i];
	}	
	else if (feed == _T("Plaza2") || feed == _T("Plaza2Replay"))
	{
		ft = PLAZA2;
		CStringA instrument_id(instrumentList.GetItemText(nItem, 3));
		return new Plaza2TickerAdapterClass(atoi(instrument_id));
	}

	return NULL;
}
void MainWindow::OnBnClickedQuoteRun()		//визуализация Quote run
{
	FeedType feed;
	IInstrument* _i = GetSelectedInstrument(feed);
	
	if (_i)
	{
		quoteRunWindows.push_back(new QuoteRun(this, feed, _i));
		quoteRunWindows[quoteRunWindows.size()-1]->Create(NULL, NULL, WS_POPUP|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME);
		quoteRunWindows[quoteRunWindows.size()-1]->CenterWindow();
		quoteRunWindows[quoteRunWindows.size()-1]->ShowWindow(SW_SHOW);
	}
}
void MainWindow::OnBnClickedQuantChart()
{
	FeedType feed;
	IInstrument* _i = GetSelectedInstrument(feed);

	if (_i)
	{
		quantChartWindows.push_back(new QuantChart(this, feed, _i));
		quantChartWindows[quantChartWindows.size()-1]->Create(NULL, NULL, WS_POPUP|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME|WS_HSCROLL);
		quantChartWindows[quantChartWindows.size()-1]->CenterWindow();
		quantChartWindows[quantChartWindows.size()-1]->ShowWindow(SW_SHOW);
	}		
}
void MainWindow::OnBnClickedHeatMap()
{
	FeedType feed;
	IInstrument* _i = GetSelectedInstrument(feed);

	if (_i) 
	{
		heatMapWindows.push_back(new HeatMap(this, feed, _i));
		heatMapWindows[heatMapWindows.size()-1]->Create(NULL, NULL, WS_POPUP|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME|WS_HSCROLL);
		heatMapWindows[heatMapWindows.size()-1]->CenterWindow();
		heatMapWindows[heatMapWindows.size()-1]->ShowWindow(SW_SHOW);
	}
}

HBRUSH MainWindow::BrushForOnCtlColor(CString s)
{
	if (s == _T("Rithmic"))
	{
		switch (rithmicStatus) 
		{
		case -1: return ::CreateSolidBrush(RGB(192, 192, 192));	//disconnect стандартный серый
		case 0:	rithmicLogo.ShowWindow(0);
				omneLogo.ShowWindow(0);
				return ::CreateSolidBrush(RGB(255, 0, 0));		//disconnect красный
		case 1:	return ::CreateSolidBrush(RGB(255, 155, 55));	//trying to connect коричневый
		case 2:	rithmicLogo.ShowWindow(1);
				omneLogo.ShowWindow(1);
				return ::CreateSolidBrush(RGB(0, 255, 128));	//connect ok зеленый
		}
	}
	else if (s == _T("CQG"))
	{
		switch (cqgStatus) 
		{
		case -1: return ::CreateSolidBrush(RGB(192, 192, 192));	//disconnect стандартный серый
		case 0:	return ::CreateSolidBrush(RGB(255, 0, 0));		//disconnect красный
		case 1:	return ::CreateSolidBrush(RGB(255, 155, 55));	//trying to connect коричневый
		case 2:	return ::CreateSolidBrush(RGB(0, 255, 128));	//connect ok зеленый
		}
	}
	else if (s == _T("NinjaTrader"))
	{
		switch (ninjaStatus)
		{
		case -1: return ::CreateSolidBrush(RGB(192, 192, 192));	//disconnect стандартный серый
		case 0:	return ::CreateSolidBrush(RGB(255, 0, 0));		//disconnect (красный)
		case 1:	return ::CreateSolidBrush(RGB(255, 155, 55));	//trying to connect (коричневый)
		case 2:	return ::CreateSolidBrush(RGB(255, 155, 55));	//есть подключение, но нет подключенных инструментов (коричневый)
		case 3: return ::CreateSolidBrush(RGB(0, 255, 128));	//есть подключенный инструменты (зеленый)
		}
	}
	else if (s == _T("Plaza2"))
	{
		if (!p2Connector)
			return ::CreateSolidBrush(RGB(192, 192, 192));	//None стандартный серый
		else
		{
			switch (p2Connector->GetConnectionStatus())
			{
			case NONE:			return ::CreateSolidBrush(RGB(192, 192, 192));	//None стандартный серый
			case DISCONNECT:	return ::CreateSolidBrush(RGB(255, 0, 0));		//disconnect (красный)
			case WAIT:			return ::CreateSolidBrush(RGB(255, 155, 55));	//trying to connect (коричневый)
			case OPEN:			return ::CreateSolidBrush(RGB(0, 255, 128));	//есть подключение
			}
		}
	}

	return NULL;
}
HBRUSH MainWindow::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	HBRUSH _hbr = NULL;

	if (pWnd == &rithmic_connection_indicator)
	{
		pDC->SetBkMode(TRANSPARENT);
		_hbr = BrushForOnCtlColor(L"Rithmic");	
	}
	else if (pWnd == &cqg_connector_indicator)
	{
		pDC->SetBkMode(TRANSPARENT);
		_hbr = BrushForOnCtlColor(L"CQG");
	}
	else if (pWnd == &ninjatrader_connection_indicator)
	{
		pDC->SetBkMode(TRANSPARENT);
		_hbr = BrushForOnCtlColor(L"NinjaTrader");
	}
	else if (pWnd == &plaza2_connection_indicator)
	{
		pDC->SetBkMode(TRANSPARENT);
		_hbr = BrushForOnCtlColor(L"Plaza2");
	}

	if (_hbr) 
		return _hbr;
	else
		return hbr;

	//цвет статических элементов
	/*switch (pWnd->GetDlgCtrlID())
	{
	case IDC_STATIC:
		SetTextColor(pDC->m_hDC, RGB(255,255,255));
		SetBkColor( pDC->m_hDC, BGRNCOLOR);
		return (HBRUSH)(backgroundBrush.GetSafeHandle());	 
		break;
	}*/
}

BOOL MainWindow::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{	
	int ret = 0;

	switch(pCopyDataStruct->dwData)
	{
		case CQGDialog::CQG_TEXT:
			UpdateActivityWindow(((TEXT*)pCopyDataStruct->lpData)->text);
			cqgSubscribeStatus = false;
			break;
		case CQGDialog::CQG_CONNECTION_STATUS:
			if (cqgDlg)
				cqgDlg->GetDlgItem(CQG_CONNECT_STATUS)->SetWindowTextW(((CONNECTIONSTATUS*)pCopyDataStruct->lpData)->text);
			cqgStatus = ((CONNECTIONSTATUS*)pCopyDataStruct->lpData)->connectionStatus;
			switch (cqgStatus)
			{
			case 0:
				DisconnectFromFeed(CQG);
				//убираем из списка фид cqg
				RemoveDatafeedFromComboBox(_T("CQG"));
				cqg_connector_indicator.RedrawWindow();
				if (cqgDlg)
				{
					cqgDlg->EnableWindow(1);
					cqgDlg->waitCursor = false;		//убираем песочные часы
					cqgDlg->GetDlgItem(CQG_CONNECT_STATUS)->RedrawWindow();
				}
				break;
			case 1:
				if (cqgDlg)
					cqgDlg->GetDlgItem(CQG_CONNECT_STATUS)->RedrawWindow();
				cqg_connector_indicator.RedrawWindow();
				break;
			case 2:
				if (!cqgIp)
					cqgIp = new INSTRUMENT_PROPERTIES;
				//заносим в список фид ритмика
				if (comboFeedType.FindStringExact(0, L"CQG") == CB_ERR)
				{
					AddNewDatafeedToComboBox(_T("CQG"));
				}
				if (!cqg_connector_indicator.IsWindowVisible())
					cqg_connector_indicator.ShowWindow(SW_SHOW);
				else cqg_connector_indicator.RedrawWindow();
				if (cqgDlg)
				{
					cqgDlg->EnableWindow(1);
					cqgDlg->waitCursor = false;		//убираем песочные часы
					cqgDlg->GetDlgItem(CQG_CONNECT_STATUS)->RedrawWindow();
				}
				break;
			}			
			break;

		case CQGDialog::CQG_INSTRUMENT_PROPERTIES:
			*cqgIp = *((INSTRUMENT_PROPERTIES*)pCopyDataStruct->lpData);
			PostMessageW(WM_APP+1, 0, 0);
			cqgSubscribeStatus = false;
			break;
		case CQGDialog::CQG_TRADE:
			cqgTrade.push_back(*((CQG_INSTRUMENT_TRADE*)pCopyDataStruct->lpData));
			PostMessageW(WM_APP+2, 0, 0);
			break;
		case CQGDialog::CQG_ASK:
			cqg_BBO_ASK.push_back(*((CQG_INSTRUMENT_BBO_ASK*)pCopyDataStruct->lpData));
			PostMessageW(WM_APP+3, 0, 0);
			break;
		case CQGDialog::CQG_BID:
			cqg_BBO_BID.push_back(*((CQG_INSTRUMENT_BBO_BID*)pCopyDataStruct->lpData));
			PostMessageW(WM_APP+4, 0, 0);
			break;
		case CQGDialog::CQG_DOM:
			cqg_DOM.push_back(*((CQG_INSTRUMENT_DOM*)pCopyDataStruct->lpData));
			PostMessageW(WM_APP+5, 0, 0);
			break;

		/*case NinjaDialog::NINJA_INSTRUMENT_DATA:
			if (ninjaStatus)
			{
				if (!ninjaIdata) 
					ninjaIdata = new Instrument_DATA;

				*ninjaIdata = *((Instrument_DATA*)pCopyDataStruct->lpData);
				PostMessageW(WM_APP+6, 0, 0);
				ret = 111;
			}
			break;
		case NinjaDialog::NINJA_TRADE:
			if (ninjaStatus)
			{
				ninjaTrade.push_back(*((Trade_DATA*)pCopyDataStruct->lpData));
				PostMessageW(WM_APP+7, 0, 0);
				ret = 111;
			}
			break;
		case NinjaDialog::NINJA_BBO_ASK:
			if (ninjaStatus)
			{
				ninja_BBO_ASK.push_back(*((BBO_DATA*)pCopyDataStruct->lpData));
				PostMessageW(WM_APP+8, 0, 0);
				ret = 111;
			}
			break;
		case NinjaDialog::NINJA_BBO_BID:
			if (ninjaStatus)
			{
				ninja_BBO_BID.push_back(*((BBO_DATA*)pCopyDataStruct->lpData));
				PostMessageW(WM_APP+9, 0, 0);
				ret = 111;
			}
			break;
		case NinjaDialog::NINJA_DOM:
			if (ninjaStatus)
			{
				ninja_DOM.push_back(*((DOM_DATA*)pCopyDataStruct->lpData));
				PostMessageW(WM_APP+10, 0, 0);
				ret = 111;
			}
			break;
		case NinjaDialog::INSTRUMENT_DELETE:
			if (ninjaStatus)
			{
				ninjaDelete.push_back(*((DELETE_INSTRUMENT*)pCopyDataStruct->lpData));
				PostMessageW(WM_APP+11, 0, 0);
				ret = 111;
			}
			break;*/
	}
	
	CDialogEx::OnCopyData(pWnd, pCopyDataStruct);
	
	return ret;
}

FeedType MainWindow::GetFeedType() const
{
	CString str;
	FeedType type;

	comboFeedType.GetWindowText(str);

	if (str == _T("Rithmic"))
		type = RITHMIC;
	else if (str == _T("CQG"))
		type = CQG;
	else if (str == _T("Plaza2"))
		type = PLAZA2;
	else if (str == _T("Plaza2Replay"))
		type = PLAZA2REPLAY;
	else 
		type = EMPTY;

	return type;
}
void MainWindow::OnBnClickedSubscribe()
{
	FeedType type = GetFeedType();
	
	switch (type)
	{
	case RITHMIC:
		RithmicSubscribe();
		break;

	case CQG:
		CQGSubsribe();
		break;

	case NINJATRADER:
		NinjaSubscribe();
		break;

	case PLAZA2:
		Plaza2Subscribe();
		break;

	case PLAZA2REPLAY:
		Plaza2ReplaySubscribe();
		break;
	
	default:
		break;
	}
}

void  MainWindow::RithmicSubscribe()
{
	CString ticker, month, year;

	comboSymbol.GetWindowText(ticker);
	comboBoxMonth.GetWindowText(month);
	comboBoxYear.GetWindowText(year);

	if (ticker.IsEmpty()) 
	{
		UpdateActivityWindow(_T("Rithmic: Please enter symbol"));
		return;
	}	

	CStringA tickerA(ticker), exchA, monthA(month), yearA(year);

	tsNCharcb sExchange, sTicker;
	bool knownTicker = false;

	for (size_t i = 0; i < vecSymbol.size(); i++) 
	{
		if (vecSymbol[i][0] == ticker)
		{
			tickerA = vecSymbol[comboSymbol.GetCurSel()][2].GetBuffer();
			tickerA += monthA[0];
			tickerA += yearA[3];
			exchA = vecSymbol[comboSymbol.GetCurSel()][1];
			knownTicker = true;
			break;
		}
	}

	if (!knownTicker) 
	{
		int pos = tickerA.Find('.');
		int len = tickerA.GetLength();
		exchA = tickerA.Right(len-pos-1);
		tickerA = tickerA.Left(pos);
	}

	sTicker.pData = (char*)tickerA.GetString();
	sTicker.iDataLen = tickerA.GetLength();
	sExchange.pData = (char*)exchA.GetString();
	sExchange.iDataLen = exchA.GetLength();

	rithmicCallbacks->IsRealTicker(sTicker, sExchange);
}

void MainWindow::CQGSubsribe()
{
	CString ticker, month, year;

	comboSymbol.GetWindowText(ticker);
	comboBoxMonth.GetWindowText(month);
	comboBoxYear.GetWindowText(year);

	if (ticker.IsEmpty())
	{
		UpdateActivityWindow(_T("CQG: Please enter symbol"));
		return;
	}

	CStringA tickerA(ticker), monthA(month), yearA(year);

	HWND cqgHwnd = IsCQGServerRun();

	if (!cqgHwnd)
		return;
	
	for (size_t i = 0; i < vecSymbol.size(); i++) 
	{
		if (vecSymbol[i][0] == ticker)
		{
			tickerA = vecSymbol[comboSymbol.GetCurSel()][3].GetBuffer();
			tickerA += monthA[0];
			tickerA += yearA[2];
			tickerA += yearA[3];
			break;
		}
	}

	//CString F_US_Ticker;
	//F_US_Ticker.Format(L"F.US.%s", ticker.GetString());

	//проверяем повторную подписку на инструмент
	for (int x = instrumentList.GetItemCount()-1; x >= 0 ; --x)
	{
		if (instrumentList.GetItemText(x,1) == _T("CQG") && instrumentList.GetItemText(x,2) == ticker && instrumentList.GetItemText(x,4) != _T("Disconnect")/* || F_US_Ticker == instrumentList.GetItemText(x,2)*/)
		{
			UpdateActivityWindow((_T("CQG: Instrument already in list: ") + ticker).GetString());
			return;
		}
	}

	SUBSCRIBE sb;

	StringCchPrintf(sb.ticker, 100, L"%S", tickerA.GetString());
	
	sb.dom = true;
	
	COPYDATASTRUCT cdStruct = { CQGDialog::CQG_SUBSCRIBE, sizeof(SUBSCRIBE), &sb };
	cqgSubscribeStatus = true;
	::SendMessageW(cqgHwnd, WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)(LPVOID)&cdStruct);	
}

void MainWindow::NinjaSubscribe()
{
	// эта функция просто для аналогии сделана
}

void MainWindow::Plaza2Subscribe()
{
	CString ticker;
	comboSymbol.GetWindowText(ticker);

	if (ticker.IsEmpty()) 
	{
		UpdateActivityWindow(_T("Plaza2: Please enter symbol"));
		return;
	}

	// проверяем повторную подписку на инструмент
	CStringA tickerA(ticker);
    FORTS_FUTINFO_REPL::fut_sess_contents* pContents(static_cast<FORTS_FUTINFO_REPL::fut_sess_contents*>(GetInstrumentContents(p2Connector.get(), tickerA.GetString())));
    // если инструмент есть, но по нему был дисконнект, просто удаляем его из списка
    for (int itemNumber = instrumentList.GetItemCount()-1; itemNumber >= 0 ; --itemNumber)
    {
        if (_wtoi(instrumentList.GetItemText(itemNumber, 3)) == pContents->isin_id && instrumentList.GetItemText(itemNumber, 4) == _T("Disconnect"))
        {
            Plaza2Unsubscribe(itemNumber);
        }
    }
	if (Plaza2DB::GetInstance()->IsInstrumentExist_ST(pContents->isin_id))
	{
		UpdateActivityWindow((_T("Plaza2: Instrument already in list: ") + ticker).GetString());
		return;
	}    

	// добавляем инструмент в контейнер
	Plaza2Instrument* instrument = Plaza2Instrument::CreateNewInstrument();
	instrument->Info(*pContents);
	Plaza2DB::GetInstance()->AddNewInstrument_ST(instrument);

    // создадим промежуточный стакан по данному инструменту
    Plaza2Callbacks::mMatchingData.PushOrdersBook(pContents->isin_id, new COrdersBook());

	// добавляем инструмент в таблицу главного окна
	TCHAR instrumentName[sizeof(pContents->isin)];	    /* = */ mbstowcs(instrumentName, pContents->isin, sizeof(pContents->isin));
	TCHAR id[10];										/* = */ _itow(pContents->isin_id, id, 10);
	TCHAR expiration[15];								/* = */ _stprintf_s(expiration, _countof(expiration), _T("%02d.%02d.%d"), pContents->d_exp.day, pContents->d_exp.month, pContents->d_exp.year);
	tstring description									   =	::wide_string(pContents->name, std::locale("rus"));
	AddNewInstrumentToListControl(tstring(_T("Plaza2")), 
                                  tstring(instrumentName), 
                                  tstring(id), 
                                  tstring(_T("Getting data")), 
		                          tstring(_T("MOEX")),
                                  tstring(expiration), description);
	
    try // запускаем нужные потоки данных по инструменту
    {
        P2ListnerParameters streamParams;
        streamParams.prefix = P2REPL;
        streamParams.name = "FORTS_DEALS_REPL";
        streamParams.mode = "online"; // snapshot+online
        streamParams.func1 = Plaza2Callbacks::SystemCallback;
        streamParams.func2 = Plaza2Callbacks::Trade;
        streamParams.tables = "deal";
        streamParams.isin = pContents->isin_id;
        streamParams.precision = pContents->roundto;
        p2Connector->OpenP2ReplListener(streamParams);

        streamParams.prefix = P2ORDBOOK;
        streamParams.name = "FORTS_ORDLOG_REPL";
        streamParams.mode.clear();
        //streamParams.mode = "snapshot+online";
        streamParams.postfixParams = "snapshot=FORTS_ORDBOOK_REPL";
        streamParams.func2 = Plaza2Callbacks::OrderBook;
        streamParams.tables.clear(); 
        //streamParams.tables = "orders,orders_log";
        p2Connector->OpenP2ReplListener(streamParams);

		::GlobalFree((HGLOBAL)pContents);

        /*streamParams.replStreamName = "FORTS_FUTCOMMON_REPL";
        streamParams.replStreamMode = "online";
        streamParams.func2 = Plaza2Callbacks::BBO;
        streamParams.replStreamTables = "common";
        p2Connector->OpenP2ReplListener(streamParams);*/
    }
    catch (std::exception& e)
    {
        wchar_t err[512];
        mbstowcs(err, e.what(), 512);
        UpdateActivityWindow(err);
        return;
    }

	UpdateActivityWindow(_T("Plaza2: ticker " + ticker + " subscribed"));
}

void MainWindow::Plaza2ReplaySubscribe()
{
	CString ticker;
	comboSymbol.GetWindowText(ticker);

	if (ticker.IsEmpty())
	{
		UpdateActivityWindow(_T("Plaza2Replay: Please enter symbol"));
		return;
	}

	// проверяем повторную подписку на инструмент
	CStringA tickerA(ticker);
	const FORTS_FUTINFO_REPL::fut_sess_contents* pContents = nullptr;
	for (const auto& info : plaza2ReplayDlg->GetIsinList())
	{
		if (tickerA == info.isin)
		{
			pContents = &info;
			break;
		}
	}
	assert(pContents);

	plaza2ReplayDlg->listenerList.insert(make_pair(pContents->isin_id, ReplayListener(pContents->isin_id, pContents->roundto)));

	if (Plaza2DB::GetInstance()->IsInstrumentExist_ST(pContents->isin_id))
	{
		UpdateActivityWindow((_T("Plaza2Replay: Instrument already in list: ") + ticker).GetString());
		return;
	}

	// добавляем инструмент в контейнер
	Plaza2Instrument* instrument = Plaza2Instrument::CreateNewInstrument();
	instrument->Info(*pContents);
	Plaza2DB::GetInstance()->AddNewInstrument_ST(instrument);

	// создадим промежуточный стакан по данному инструменту
	Plaza2Callbacks::mMatchingData.PushOrdersBook(pContents->isin_id, new COrdersBook());

	// добавляем инструмент в таблицу главного окна
	TCHAR instrumentName[sizeof(pContents->isin)];	    /* = */ mbstowcs(instrumentName, pContents->isin, sizeof(pContents->isin));
	TCHAR id[10];										/* = */ _itow(pContents->isin_id, id, 10);
	TCHAR expiration[15];								/* = */ _stprintf_s(expiration, _countof(expiration), _T("%02d.%02d.%d"), pContents->d_exp.day, pContents->d_exp.month, pContents->d_exp.year);
	tstring description = ::wide_string(pContents->name, std::locale("rus"));
	AddNewInstrumentToListControl(tstring(_T("Plaza2Replay")),
								  tstring(instrumentName),
								  tstring(id),
								  tstring(_T("Getting data")),
								  tstring(_T("MOEX")),
								  tstring(expiration), description);
}

void MainWindow::OnUpdateSubscribe(CCmdUI* pCmdUI)
{
	FeedType type = GetFeedType();

	switch (type)
	{
	case RITHMIC:
		if (rithmicStatus == 2)
			pCmdUI->Enable(1);
		else pCmdUI->Enable(0);
		break;
	case CQG:
		if (cqgStatus == 2 && !cqgSubscribeStatus)
			pCmdUI->Enable(1);
		else pCmdUI->Enable(0);
		break;
	case NINJATRADER:
		if (ninjaStatus == 2 || ninjaStatus == 3)
			pCmdUI->Enable(1);
		else pCmdUI->Enable(0);
		break;
	case PLAZA2:
		if (p2Connector && p2Connector->GetConnectionStatus() == OPEN)
			pCmdUI->Enable(1);
		else pCmdUI->Enable(0); 
		break;
	case PLAZA2REPLAY:
		if (plaza2ReplayDlg->GetStatus() == ReplayStatus::DBOPENED)
			pCmdUI->Enable(1);
		else pCmdUI->Enable(0);
		break;
	case EMPTY:
		pCmdUI->Enable(0);
		break;
	}
}
void MainWindow::OnBnClickedGethistory()
{
	CString ticker, exch, feedName;
	tsNCharcb sExchange, sTicker;

	POSITION p = instrumentList.GetFirstSelectedItemPosition();
	int nItem;
	if (p == NULL) {
		UpdateActivityWindow(L"Rithmic: Select instrument!");
		return;
	} else {
		while (p)
			nItem = instrumentList.GetNextSelectedItem(p);
	}

	feedName = instrumentList.GetItemText(nItem, 1);
	ticker = instrumentList.GetItemText(nItem, 2);
	exch = instrumentList.GetItemText(nItem, 5);

	if (feedName == "Rithmic") //эта проверка нужна если уже есть подключение к другому фиду например к нинзе и есть инструмент в списке
	{
		CStringA tickerA(ticker), exchA(exch);

		sTicker.pData = (char*)tickerA.GetString();
		sTicker.iDataLen = tickerA.GetLength();
		sExchange.pData = (char*)exchA.GetBuffer();
		sExchange.iDataLen = exchA.GetLength();

		//определяем индекс текущего инструмента в контейнере инструментов
		size_t i = 0; 
		std::string name(sTicker.pData, sTicker.iDataLen);
	
		if (rithmic_data.size()) {
			while (rithmic_data[i]->GetName().compare(name) != 0 && i < rithmic_data.size())
				++i;
		}
	
		if (rithmic_data[i]->historyFlag)
		{
			UpdateActivityWindow(L"Rithmic: Historical data for this Instrument already downloaded");
			return;
		}
		else 
			rithmic_data[i]->historyFlag = 1;

		//получаем интрадей историю по инструменту
		int iCode;
		TCHAR str[64];
		StringCchPrintfW(str, 64, L"Rithmic: Getting historical data for %.*S...", sTicker.iDataLen, sTicker.pData);
		UpdateActivityWindow(str);
		rithmicCallbacks->pEngine->replayTrades(&sExchange, &sTicker, 0, 0, &iCode);
	}
}
void MainWindow::OnUpdateGetHistoricalData(CCmdUI* pCmdUI)
{
	if (GetFeedType() == RITHMIC && rithmicStatus == 2)
		pCmdUI->Enable(1);
	else pCmdUI->Enable(0);
}
void MainWindow::OnUpdateSpreadLegTrades(CCmdUI* pCmdUI)
{
	if (GetFeedType() == RITHMIC && rithmicStatus == 2)
		pCmdUI->Enable(1);
	else pCmdUI->Enable(0);
}
void MainWindow::OnDatafeedComboBox(CCmdUI* pCmdUI)
{
	bool bRithmic = (rithmicStatus == 2) ? true : false;
	bool bCQG = (cqgStatus == 2) ? true : false;
	bool bPlaza2 = (p2Connector && p2Connector->IsConnected()) ? true : false;
	bool bPlaza2Replay = (plaza2ReplayDlg && plaza2ReplayDlg->GetStatus() != ReplayStatus::DBCLOSED) ? true : false;

	if (bRithmic || bCQG || bPlaza2 || bPlaza2Replay)
		pCmdUI->Enable(1);
	else pCmdUI->Enable(0);
}
void MainWindow::OnUpdateUnSubscribe(CCmdUI* pCmdUI)
{
	bool enable = instrumentList.GetItemCount() && GetFeedType() != EMPTY;
	if (GetFeedType() == PLAZA2REPLAY)
		enable = (plaza2ReplayDlg->status == ReplayStatus::DBOPENED) ? true : false;

	if (enable)
		pCmdUI->Enable(1);
	else pCmdUI->Enable(0);
}
void MainWindow::OnSymbolComboBox(CCmdUI* pCmdUI)
{
	int curSel = comboFeedType.GetCurSel();
	if (curSel != CB_ERR)
		pCmdUI->Enable(1);
	else
		pCmdUI->Enable(0);
}
void MainWindow::OnMonthComboBox(CCmdUI* pCmdUI)
{
	if (rithmicStatus == 2 || cqgStatus == 2)
		pCmdUI->Enable(1);
	else pCmdUI->Enable(0);
}
void MainWindow::OnYearComboBox(CCmdUI* pCmdUI)
{
	if (rithmicStatus == 2 || cqgStatus == 2)
		pCmdUI->Enable(1);
	else pCmdUI->Enable(0);
}

void MainWindow::OnQuoteRunButton(CCmdUI* pCmdUI)
{
	if (instrumentList.GetItemCount())
		pCmdUI->Enable(1);
	else pCmdUI->Enable(0);
}
void MainWindow::OnQuantChartButton(CCmdUI* pCmdUI)
{
	if (instrumentList.GetItemCount())
		pCmdUI->Enable(1);
	else pCmdUI->Enable(0);
}
void MainWindow::OnQuoteMapperButton(CCmdUI* pCmdUI)
{
	if (instrumentList.GetItemCount())
		pCmdUI->Enable(1);
	else pCmdUI->Enable(0);
}

void MainWindow::ResetInstrumentPointers(CStringA& s)
{
	//обнуляем указатели у всех окон для данного инструмента
	for (ptrdiff_t i = quoteRunWindows.size()-1; i >= 0; --i)	//окна QuoteRun
	{
		if (!quoteRunWindows[i]->name.compare(s.GetString()))
			quoteRunWindows[i]->Unsubscribe();
	}
	for (ptrdiff_t i = quantChartWindows.size()-1; i >= 0; --i)	//окна Quant chart
	{
		if (!quantChartWindows[i]->name.compare(s.GetString()))
			quantChartWindows[i]->Unsubscribe();
	}
	for (ptrdiff_t i = heatMapWindows.size()-1; i >= 0; --i)	//окна Quant chart
	{
		if (!heatMapWindows[i]->name.compare(s.GetString()))
			heatMapWindows[i]->Unsubscribe();
	}
}
void MainWindow::DeleteFromVector(FeedType ft, CStringA& s)
{
	switch (ft)
	{
	case RITHMIC:
		::AcquireSRWLockExclusive(&SRWLockRithmic);
		//удаление из контейнера
		for (size_t i = 0; i < rithmic_data.size(); ++i) {
			if (!rithmic_data[i]->GetName().compare(s.GetString())) {
				delete rithmic_data[i];
				rithmic_data.erase(rithmic_data.begin()+i);
			}
		}
		::ReleaseSRWLockExclusive(&SRWLockRithmic);
		break;
	case CQG:
		for (size_t i = 0; i < cqg_data.size(); ++i) 
		{
			if (!cqg_data[i]->GetName().compare(s.GetString())) 
			{
				delete cqg_data[i];
				cqg_data.erase(cqg_data.begin()+i);
			}
		}
		break;
	case NINJATRADER:
		for (size_t i = 0; i < ninja_data.size(); ++i) 
		{
			if (!ninja_data[i]->GetName().compare(s.GetString())) 
			{
				delete ninja_data[i];
				ninja_data.erase(ninja_data.begin()+i);
			}
		}
		//если еще есть подписанные инсрументы то статус подключения 3 иначе 2
		if (ninja_data.size())
			ninjaStatus = 3;
		else ninjaStatus = 2;
		InvalidateNinjaConnectionIndicator(0, 0);
		break;
	}
}
void MainWindow::OnBnClickedUnsubscribe()
{
	//определяем строку выбранную в списке инструментов
	POSITION p = instrumentList.GetFirstSelectedItemPosition();
	int nItem;
	if (p == NULL) {
		UpdateActivityWindow(_T("Please, select instrument for Unsubsсribe!"));
		return;
	} else {
		while (p)
			nItem = instrumentList.GetNextSelectedItem(p);
	}

    CStringA feed(instrumentList.GetItemText(nItem, 1));

	if (feed == _T("Rithmic"))
	{
		RithmicUnsubscribe(nItem);
	}
	else if (feed == _T("CQG"))
	{
		CQGUnsubscribe(nItem);
	}
	else if (feed == _T("Plaza2"))
	{
		Plaza2Unsubscribe(nItem);
	}
	else if (feed == _T("Plaza2Replay"))
	{
		Plaza2ReplayUnsubscribe(nItem);
	}

	//выбрать другой инструмент если остался или обнулить вектор
	nItem = instrumentList.GetItemCount();
	if (nItem)
	{
		instrumentList.SetFocus();
		instrumentList.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	}
}
void MainWindow::OnBnClickedUnsubscribeAll()
{
	int selRow = instrumentList.GetItemCount();
	
	CStringA name, exchange;
	tsNCharcb sExchange, sTicker;

	for (int j = 0; j < selRow; ++j)
	{
		name = instrumentList.GetItemText(j, 2);

		if (instrumentList.GetItemText(j, 1) == _T("Rithmic"))
		{
			//получаем имя текущего инструмента
			exchange = instrumentList.GetItemText(j, 5);
			sTicker.pData = (char*)name.GetString();
			sTicker.iDataLen = name.GetLength();
			sExchange.pData = (char*)exchange.GetString();
			sExchange.iDataLen = exchange.GetLength();

			//отписываемся от инструмента
			rithmicCallbacks->UnSubscribe(sTicker, sExchange);
		}

		//обнуляем указатели у всех окон для данного инструмента
		ResetInstrumentPointers(name);
	}  

	//отписываемся от всех инструментов CQG
	if (cqgStatus == 2)
	{
		HWND cqgHwnd = IsCQGServerRun();
		COPYDATASTRUCT cdStruct = { CQGDialog::CQG_UNSUBSCRIBEALL, 0, NULL };
		::SendMessage(cqgHwnd, WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)(LPVOID)&cdStruct);
	}

	//очищаем весь список кроме нинзи
	int i = 0;
	for (int j = 0; j < selRow; ++j)
	{
		CString feed = instrumentList.GetItemText(i, 1);
		bool isFeed = feed == _T("Rithmic") || feed == _T("CQG");
		if (isFeed)
			instrumentList.DeleteItem(i);
		else ++i;
	}

	//обнуляем все контейнеры
	rithmic_data.resize(0);
	cqg_data.resize(0);

	//отписываемся от всех plaza2 инструментов если таковые есть
	for (int j = 0; j < selRow;)
	{
		CString feed(instrumentList.GetItemText(j, 1));
		if (feed == _T("Plaza2"))
			Plaza2Unsubscribe(j);
		else if (feed == _T("Plaza2Replay"))
			Plaza2ReplayUnsubscribe(j);
		else
			++j;
	}
}

void MainWindow::RithmicUnsubscribe(int nItem)
{
	//заполняем имя инструмента и название биржи
    CStringA name(instrumentList.GetItemText(nItem, 2)), 
			 exchange(instrumentList.GetItemText(nItem, 5));

	tsNCharcb sExchange, sTicker;
			
	sTicker.pData = (char*)name.GetString();
	sTicker.iDataLen = name.GetLength();
	sExchange.pData = (char*)exchange.GetString();
	sExchange.iDataLen = exchange.GetLength();

	//отписываемся от инструмента
	rithmicCallbacks->UnSubscribe(sTicker, sExchange);

	//обнуляем указатели у всех окон для данного инструмента
	ResetInstrumentPointers(name);
		
	//удаление из контейнера
	DeleteFromVector(RITHMIC, name);

	//удаление из списка
	instrumentList.DeleteItem(nItem);
}
void MainWindow::CQGUnsubscribe(int nItem)
{
	//заполняем имя инструмента и название биржи
    CStringA name(instrumentList.GetItemText(nItem, 2));

	//отписываемся от инструмента
	TEXT ticker;
	StringCchPrintf(ticker.text, 100, L"%s", instrumentList.GetItemText(nItem, 3).GetString());

	HWND cqgHwnd = IsCQGServerRun();

	if (!cqgHwnd)
    {
		return;
    }

	COPYDATASTRUCT cdStruct = { CQGDialog::CQG_UNSUBSCRIBE, sizeof(TEXT), &ticker };
	::SendMessage(cqgHwnd, WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)(LPVOID)&cdStruct);

	//обнуляем указатели у всех окон для данного инструмента
	ResetInstrumentPointers(name);

	//удаление из контейнера
	DeleteFromVector(CQG, name);

	//удаление из списка
	instrumentList.DeleteItem(nItem);
}
void MainWindow::NinjaUnsubscribe(int nItem)
{
	// пустая функция для архитектуры сделана
}
void MainWindow::Plaza2Unsubscribe(int nItem)
{    
    CStringA isin_id(instrumentList.GetItemText(nItem, 3));

	// останавливаем поток данных по этому инструменту
    int id = atoi(isin_id);
    if (p2Connector)
    {
        p2Connector->CloseP2ReplListener(id);
    }

	// обнуляем указатели у всех окон для данного инструмента
    CStringA name(instrumentList.GetItemText(nItem, 2));
	ResetInstrumentPointers(name);
		
	// удаление из контейнера
	auto& instrumentsDB = *Plaza2DB::GetInstance();
	instrumentsDB.RemoveInstrument_ST(id/*instrumentKey*/);

	// удаление из списка
	instrumentList.DeleteItem(nItem);
}

void MainWindow::Plaza2ReplayUnsubscribe(int nItem)
{
	CStringA isin_id(instrumentList.GetItemText(nItem, 3));
	int id = atoi(isin_id);

	plaza2ReplayDlg->listenerList.erase(id);
	
	// обнуляем указатели у всех окон для данного инструмента
	CStringA name(instrumentList.GetItemText(nItem, 2));
	ResetInstrumentPointers(name);

	// удаление из контейнера
	auto& instrumentsDB = *Plaza2DB::GetInstance();
	instrumentsDB.RemoveInstrument_ST(id/*instrumentKey*/);

	// удаление из списка
	instrumentList.DeleteItem(nItem);
}

void MainWindow::CloseAllWindows()
{
    CloseAllConnectionWindows();
    CloseAllGraphicWindows();
}

void MainWindow::DisconnectAllDataFeeds()
{
    if (rithmicCallbacks)
    {
        rithmicCallbacks->Disconnect();
    }

    if (p2Connector)
    {
        p2Connector->Disconnect();
        StopPlaza2RouterProcess(PLAZA2_ROUTER_NAME_X86);
        StopPlaza2RouterProcess(PLAZA2_ROUTER_NAME_X64);
    }
}

void MainWindow::CloseAllConnectionWindows()
{
    if (rithmicDlg)
    {
        rithmicDlg->CloseWindow();
        delete rithmicDlg;
    }

    if (cqgDlg)
    {
        cqgDlg->CloseWindow();
        delete cqgDlg;
    }

    if (ninjaDlg)
    {
        ninjaDlg->CloseWindow();
        delete ninjaDlg;
    }

    if (plaza2Dlg)
    {
        plaza2Dlg->CloseWindow();
        delete plaza2Dlg;
    }

	if (plaza2ReplayDlg)
	{
		plaza2ReplayDlg->CloseWindow();
		delete plaza2ReplayDlg;
	}
}

void MainWindow::CloseAllGraphicWindows()
{
    //закрываем все окна и освобождаем память
    size_t size = quoteRunWindows.size();
    for (size_t i = 0; i < size; ++i) 
    {
        quoteRunWindows[0]->CloseWindow();
        delete quoteRunWindows[0];
    }

    size = quantChartWindows.size();
    for (size_t i = 0; i < size; ++i) 
    {
        quantChartWindows[0]->CloseWindow();
        delete quantChartWindows[0];
    }

    size = heatMapWindows.size();
    for (size_t i = 0; i < size; ++i) 
    {
        heatMapWindows[0]->CloseWindow();
        delete heatMapWindows[0];
    }
}

BOOL MainWindow::PreTranslateMessage(MSG* pMsg)
{
	UpdateDialogControls(this, FALSE);
	return CDialogEx::PreTranslateMessage(pMsg);
}

void MainWindow::OnConnectionRithmic()
{
	if (!rithmicDlg)
	{
		rithmicDlg = new RithmicDialog();
		rithmicDlg->Create(DIALOG_RITHMIC_CONNECTION, this);
		RECT rt1, rt2;
		::GetWindowRect(rithmicDlg->m_hWnd, &rt1);
		::GetWindowRect(m_hWnd, &rt2);
		int x1 = rt2.left - (rt1.right-rt1.left);
		rithmicDlg->MoveWindow(x1, rt1.top, rt2.left-x1, rt1.bottom-rt1.top);
		rithmicDlg->ShowWindow(SW_SHOW);
	}
}
void MainWindow::OnConnectionCqg()
{
	if (!cqgDlg)
	{
		cqgDlg = new CQGDialog();
		cqgDlg->Create(DIALOG_CQG_CONNECTION, this);
		RECT rt1, rt2;
		::GetWindowRect(cqgDlg->m_hWnd, &rt1);
		::GetWindowRect(m_hWnd, &rt2);
		int x1 = rt2.left - (rt1.right-rt1.left);
		cqgDlg->MoveWindow(x1, rt1.top, rt2.left-x1, rt1.bottom-rt1.top);
		cqgDlg->ShowWindow(SW_SHOW);
	}
}
void MainWindow::OnConnectionNinjatrader()
{
	if (!ninjaDlg)
	{
		ninjaDlg = new NinjaDialog();
		ninjaDlg->Create(DIALOG_NINJATRADER_CONNECTION, this);
		RECT rt1, rt2;
		::GetWindowRect(ninjaDlg->m_hWnd, &rt1);
		::GetWindowRect(m_hWnd, &rt2);
		int x1 = rt2.left - (rt1.right-rt1.left);
		ninjaDlg->MoveWindow(x1, rt1.top, rt2.left-x1, rt1.bottom-rt1.top);
		ninjaDlg->ShowWindow(SW_SHOW);
	}
}
void MainWindow::OnConnectionPlaza2()
{
	if (!plaza2Dlg)
	{
		plaza2Dlg = new Plaza2Dialog();
		plaza2Dlg->Create(Plaza2Dialog::IDD, this);
		RECT rt1, rt2;
		::GetWindowRect(plaza2Dlg->m_hWnd, &rt1);
		::GetWindowRect(m_hWnd, &rt2);
		int x1 = rt2.left - (rt1.right-rt1.left);
		plaza2Dlg->MoveWindow(x1, rt1.top, rt2.left-x1, rt1.bottom-rt1.top);
		plaza2Dlg->ShowWindow(SW_SHOW);
	}
}

void MainWindow::OnPlaza2Replay()
{
	if (plaza2ReplayDlg == nullptr)
	{
		plaza2ReplayDlg = new Plaza2ReplayDialog();
		plaza2ReplayDlg->Create(Plaza2ReplayDialog::IDD, GetDesktopWindow());
		RECT rt1, rt2;
		::GetWindowRect(plaza2ReplayDlg->m_hWnd, &rt1);
		::GetWindowRect(m_hWnd, &rt2);
		int x1 = rt2.left - (rt1.right - rt1.left);
		plaza2ReplayDlg->MoveWindow(x1, rt1.top, rt2.left - x1, rt1.bottom - rt1.top);
		plaza2ReplayDlg->ShowWindow(SW_SHOW);
	}
}

HWND MainWindow::IsCQGServerRun()
{
	//проверяем запущен сервер cqg или нет
	HWND hwn = ::FindWindowW(_T("CQG_Algovisor_Server"), NULL);
	int count = 0;

	if (!hwn)
	{
		STARTUPINFO si = { sizeof(si) };
		PROCESS_INFORMATION pi;
#ifdef _DEBUG
		if(!::CreateProcessW(_T("CQG\\CQG_Server.exe"), NULL, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
#else
		if(!::CreateProcessW(_T("CQG\\CQG_Server.exe"), NULL, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
#endif		
		{
			UpdateActivityWindow(_T("CQG: Load CQG_Algovisor_Server.exe failed"));
			return NULL;
		}

		::CloseHandle(pi.hProcess);
		::CloseHandle(pi.hThread);
		
		while (!(hwn = ::FindWindowW(_T("CQG_Algovisor_Server"), NULL)))
		{
			::Sleep(100);			
			++count;
			if (count == 50)
			{
				UpdateActivityWindow(_T("CQG: Load CQG_Algovisor_Server.exe failed"));
				return NULL;
			}
		}
	}

	return hwn;
}

void MainWindow::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)	
	{
	case 1:	//для проверки подключенной нинзи
		if (!CheckProcessInMemory(L"NinjaTrader.exe"))	// анализ результата
		{
			ninjaStatus = -1;
			
			DisconnectFromFeed(NINJATRADER);
			UpdateActivityWindow(L"NinjaTrader is not running! Connection closed");
			Invalidate();
			if (ninjaDlg)
			{				
				ninjaDlg->Invalidate();
				securityString.clear();
			}
			KillTimer(1);
			StopPipeServerForNinja();

			//if (ninjaDlg)
			//	ninjaDlg->EnableWindow(1);
		}
		break;
	case 2:	//для проверки временной метки о лицензии из реестра
		{
			//time_t t1 = time(0), 
			//	   t2 = theApp.GetProfileInt(APPLICATION_NAME, _T("Lic_time"), 0);	//проверить что будет если переменной в реестре нет, удалили например
			//int diff = t1 - t2;
			//if (diff > 10/*86400*/)
			//{

#ifdef CONNECT_THROUGH_SERVER
			//посылаем запрос серверу для проверки валидности текущей лицензии
			using namespace rapidjson;

			StringBuffer s;
			Writer<StringBuffer> writer(s);
    
			writer.StartObject();
			writer.String("action"); writer.String("ping");
			writer.EndObject();

			std::string request(""), answer("");
			const wchar_t* errorWStr = SFLB_0_encryptRSA((char*)s.GetString(), request);
			if (!request.compare(""))
			{
				UpdateActivityWindow(errorWStr);
				OnCancel();
				return;
			}

			std::wstring curlRequest = SFLB_0_SendCurlRequest(request, answer);				
			if (!answer.compare("PONG"))
				return;
			else 
			{
				
				CloseWnd();
				return;
			}
#endif		
			//}
			//else if (diff < 0) //если после подключения перевели время назад, то выключаем софт
			//{
			//	OnCancel();
			//	return;
			//}
		}
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

void MainWindow::CloseWnd()
{
	OnCancel();
}

void MainWindow::StopPipeServerForNinja()
{
	//очищаем мьютекс
	if (hGlobalMutex != INVALID_HANDLE_VALUE)
	{
		ReleaseMutex(hGlobalMutex);
		CloseHandle(hGlobalMutex);
		hGlobalMutex = INVALID_HANDLE_VALUE;
	}

	//завершаем цикл всех потоков пайп-сервера
	for (std::list<HANDLE>::iterator i = hPipeServer.begin(); i != hPipeServer.end(); i++)
	{
		if (*i != INVALID_HANDLE_VALUE)
		{
			//CancelSynchronousIo(hPipeServer);
			//DWORD error = GetLastError();
			/*BOOL res = */CancelIoEx(*i, NULL);
			//FlushFileBuffers(*i); 
			//DisconnectNamedPipe(*i);
			CloseHandle(*i);
			*i = INVALID_HANDLE_VALUE;
			
		}
	}
	hPipeServer.clear();

	//ждем завершения всех потоков пайп-сервера
	HANDLE* hwnd = new HANDLE[hPipeThreads.size()];
	int x = 0;
	for (std::list<HANDLE>::iterator i = hPipeThreads.begin(); i != hPipeThreads.end(); i++, ++x)
	{
		if (*i) hwnd[x] = *i;
	}
	::WaitForMultipleObjects((DWORD)hPipeThreads.size(), hwnd, TRUE, INFINITE);
	delete[] hwnd;
	hPipeThreads.clear();
	
}

// CAboutDlg dialog used for App About

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ABOUT_LOGO, logo);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	TCHAR version[32];
	LoadString(GetModuleHandle(NULL), ALGOVISOR_VERSION, version, 32);
	GetDlgItem(IDC_ABOUT_VERSION)->SetWindowTextW(version);

	HICON hBitmap = (HICON)LoadImageW(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 64, 64, 0);
	logo.SetIcon(hBitmap);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void MainWindow::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void MainWindow::OnClose()
{
    // закрываем все открытые окна (графические и диалоговые)
    CloseAllWindows();

    // отписываемся от всех инструментов
    OnBnClickedUnsubscribeAll();

    // дисконектимся от всех фидов
    DisconnectAllDataFeeds();

	//закрываем пайп-сервер если он открыт и завершаем все потоки (это нельзя делать в деструкторе, т.к. там уже окно разрушено, а потоки пайп-сервера с ним работают еще)
	StopPipeServerForNinja();

	CDialogEx::OnClose();
}

void MainWindow::AddNewDatafeedToComboBox(tstring feedName)
{
	int nItem = comboFeedType.FindString(0, feedName.c_str());
	if (nItem == CB_ERR)
	{
		comboFeedType.AddString(feedName.c_str());
		comboFeedType.SetCurSel(comboFeedType.FindStringExact(0, feedName.c_str()));
		comboFeedType.RedrawWindow();
	}
}

void MainWindow::RemoveDatafeedFromComboBox(tstring feedName)
{
	int nIndex = comboFeedType.FindString(0, feedName.c_str());
	comboFeedType.DeleteString(nIndex);
	comboFeedType.RedrawWindow();
}

void MainWindow::OnSelchangeDatafeedCombobox()
{
	int curSel = comboFeedType.GetCurSel();
	CString datafeedName;
	if (curSel != CB_ERR)
    {
		comboFeedType.GetLBText(curSel, datafeedName);
    }

	if (datafeedName == _T("Rithmic") || datafeedName == _T("CQG"))
	{
		std::vector<CString> list;
		for (auto it = vecSymbol.begin(), end = vecSymbol.end(); it != end; ++it)
        {
			list.push_back(*(it[0].begin()));
        }
		FillInstrumentComboBox(list);
	}
	else if (datafeedName == _T("Ninjatrader"))
	{
		FillInstrumentComboBox(std::vector<CString>());
	}
	else if (datafeedName == _T("Plaza2"))
	{
		FORTS_FUTINFO_REPL::fut_sess_contents* instrumentsParams = NULL;
		int instrumentsCount;
        GetAllInstrumentsContents(p2Connector.get(), (void**)&instrumentsParams, instrumentsCount);
		std::vector<CString> list;
		for (int i = 0; i < instrumentsCount; ++i)
		{
			list.push_back(CString(instrumentsParams[i].isin));
		}
		::GlobalFree((HGLOBAL)instrumentsParams);

		FillInstrumentComboBox(list);
	}
	else if (datafeedName == _T("Plaza2Replay"))
	{
		std::vector<CString> list;
		for (const auto& info : plaza2ReplayDlg->GetIsinList())
		{
			list.emplace_back(info.isin);
		}
		FillInstrumentComboBox(list);
	}
}

void MainWindow::AddNewInstrumentToListControl(tstring& datafeedName, 
											   tstring& ticker, 
											   tstring& ID, 
											   tstring& status, 
											   tstring& exchange, 
											   tstring& expiration, 
											   tstring& description)
{
	instrumentList.InsertItem(0, _T(""));
	instrumentList.SetItemText(0, 1, datafeedName.c_str());
	instrumentList.SetItemText(0, 2, ticker.c_str());
	instrumentList.SetItemText(0, 3, ID.c_str());
	instrumentList.SetItemText(0, 4, status.c_str());
	instrumentList.SetItemText(0, 5, exchange.c_str());
	instrumentList.SetItemText(0, 6, expiration.c_str());
	instrumentList.SetItemText(0, 7, description.c_str());
}

LRESULT MainWindow::OnPowerMsgRcvd(WPARAM wParam, LPARAM lParam)
{
	DisconnectAllDataFeeds();

	/*switch (wParam) {
        case PBT_APMPOWERSTATUSCHANGE:
            TRACE0("PBT_APMPOWERSTATUSCHANGE  received\n");
            break;
        case PBT_APMRESUMEAUTOMATIC:
            TRACE0("PBT_APMRESUMEAUTOMATIC  received\n");
            break;
        case PBT_APMRESUMESUSPEND:
            TRACE0("PBT_APMRESUMESUSPEND  received\n");
            break;
        case PBT_APMSUSPEND:
            TRACE0("PBT_APMSUSPEND  received\n");
            break;
    }*/

    return 0;
}
