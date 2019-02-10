#include "stdafx.h"

#include "Algovisor.h"
#include "RithmicCallbacks.h"
#include "RithmicDialog.h"
#include "MainWindow.h"
#include "Instrument.h"
#include "QuoteRun.h"
#include "Quant_Chart.h"
#include "Heat_Map.h"
#include "Auxiliary_methods.h"
#include "Logger.h"

#ifdef DEBUG
static FileLogger logger(".\\log", "Rithimic_Callbacks.txt", std::ios_base::ate, LOGGER_PARAM_TIME | LOGGER_PARAM_TID);
#define DEBUG_LOG(message, ...) PRINT_TO_LOG(logger, LOG_DEBUG, message, __VA_ARGS__)
#else
#define DEBUG_LOG(message, ...)
#endif

#define RITHMIC_STUB(method_name)   \
    DEBUG_LOG("%s", method_name)    \
    *aiCode = API_OK;               \
    return (OK);

//bool g_bMdLoginComplete = false;


RithmicCallbacks::RithmicCallbacks(char** _envp, REngineParams& rp, LoginParams& lp, std::string& login, std::string& password) : pEngine(NULL)
{
	SetValues(_envp, rp, lp, login, password);

	marketDataConnectionStatus = 0;
	historicalDataConnectionStatus = 0;
}

RithmicCallbacks::~RithmicCallbacks() 
{
	delete pEngine;
	pEngine = NULL;

	delete rEngineParams.pAdmCallbacks;
	rEngineParams.pAdmCallbacks = NULL;
}

void RithmicCallbacks::SetValues(char** _envp, REngineParams& rp, LoginParams& lp, std::string& login, std::string& password)
{
	rEngineParams.pAdmCallbacks = NULL;
	rEngineParams = rp;
	oLoginParams = lp;

	for (int i = 0; i < 11; ++i)
	{
		envp[i] = _envp[i];
	}
	
	rEngineParams.envp	= envp;
	
	oLoginParams.sUser.pData = const_cast<char*>(login.c_str());
	oLoginParams.sUser.iDataLen = (int)strlen(oLoginParams.sUser.pData);
	oLoginParams.sPassword.pData = const_cast<char*>(password.c_str());
	oLoginParams.sPassword.iDataLen = (int)strlen(oLoginParams.sPassword.pData);
	
	oLoginParams.sMdUser.pData = const_cast<char*>(login.c_str());
	oLoginParams.sMdUser.iDataLen = (int)strlen(oLoginParams.sUser.pData);
	oLoginParams.sMdPassword.pData = const_cast<char*>(password.c_str());
	oLoginParams.sMdPassword.iDataLen = (int)strlen(oLoginParams.sPassword.pData);
}

//эта функция вызывается первая после логина
int RithmicCallbacks::Alert(AlertInfo* pInfo, void* pContext, int* aiCode)
{
	DEBUG_LOG("Alert(): sTicker: %s; sExchange: %s; sMessage: %s", 
			  std::string(pInfo->sTicker.pData, pInfo->sTicker.iDataLen).c_str(),
			  std::string(pInfo->sExchange.pData, pInfo->sExchange.iDataLen).c_str(),
			  std::string(pInfo->sMessage.pData, pInfo->sMessage.iDataLen).c_str())

	CString str; 
	str.Format(_T("Rithmic: %s"), (LPCTSTR)CString(pInfo->sMessage.pData, pInfo->sMessage.iDataLen)); 
				//pInfo->iAlertType, pInfo->iConnectionId, pInfo->iRpCode);

	 //if (pInfo->iAlertType == ALERT_LOGIN_COMPLETE && pInfo->iConnectionId == MARKET_DATA_CONNECTION_ID)
     //     g_bMdLoginComplete = true;
	
	if (pMainWnd->m_hWnd) 
	{
		pMainWnd->UpdateActivityWindow(str);
		
		//если хардбит был то реконектимся
		if (ALERT_QUIET_HEARTBEAT == pInfo->iAlertType)
		{
			pMainWnd->rithmicCallbacks->pEngine->reconnect(pInfo->iConnectionId, aiCode);
		}

		//TRADING_SYSTEM_CONNECTION_ID
		//PNL_CONNECTION_ID

		//статус коннекта для рыночных данных
		if (pInfo->iConnectionId == RApi::MARKET_DATA_CONNECTION_ID)
		{
			marketDataConnectionStatus = pInfo->iAlertType;
		}

		//статус коннекта для исторических данных
		if (pInfo->iConnectionId == RApi::INTRADAY_HISTORY_CONNECTION_ID)
		{
			historicalDataConnectionStatus = pInfo->iAlertType;
		}

		//конечный статус подключения для ритмика
		if (marketDataConnectionStatus == RApi::ALERT_LOGIN_COMPLETE && historicalDataConnectionStatus == RApi::ALERT_LOGIN_COMPLETE)
		{
			pMainWnd->rithmicStatus = 2;

			if (pMainWnd->rithmicDlg)
				SendMessage(pMainWnd->rithmicDlg->GetDlgItem(RITHMIC_CONNECTION_STATUS)->m_hWnd, WM_SETTEXT, 0, LPARAM(L"Connection status: UP"));
			if (!pMainWnd->rithmic_connection_indicator.IsWindowVisible())
				pMainWnd->rithmic_connection_indicator.ShowWindow(SW_SHOW);
			else pMainWnd->rithmic_connection_indicator.RedrawWindow();
			
			//заносим в список фид ритмика
			pMainWnd->AddNewDatafeedToComboBox(_T("Rithmic"));
		}
		else if (marketDataConnectionStatus == RApi::ALERT_CONNECTION_CLOSED || marketDataConnectionStatus == RApi::ALERT_CONNECTION_BROKEN ||
				 marketDataConnectionStatus == RApi::ALERT_LOGIN_FAILED || marketDataConnectionStatus == RApi::ALERT_SHUTDOWN_SIGNAL ||
				 marketDataConnectionStatus == RApi::ALERT_FORCED_LOGOUT)
		{
			pMainWnd->rithmicStatus = 0;

			pMainWnd->DisconnectFromFeed(RITHMIC);
			
			if (pMainWnd->rithmicDlg)
				SendMessage(pMainWnd->rithmicDlg->GetDlgItem(RITHMIC_CONNECTION_STATUS)->m_hWnd, WM_SETTEXT, 0, LPARAM(L"Connection status: DOWN"));
			pMainWnd->rithmic_connection_indicator.RedrawWindow();

			//убираем из списка фид ритмика
			pMainWnd->RemoveDatafeedFromComboBox(_T("Rithmic"));
		}

		if (pMainWnd->rithmicDlg)
		{
			::PostMessageW(pMainWnd->rithmicDlg->m_hWnd, WM_PAINT, 0, 0);
		}
		
		switch (pInfo->iAlertType)
		{
			case ALERT_CONNECTION_OPENED:	//SendMessage(pWnd->GetDlgItem(IDC_CONNECTION_STATUS_RITHMIC)->m_hWnd, WM_SETTEXT, 0, LPARAM(L"Connection OPENED"));
											break;
			case ALERT_CONNECTION_CLOSED:	//SendMessage(pWnd->GetDlgItem(IDC_CONNECTION_STATUS_RITHMIC)->m_hWnd, WM_SETTEXT, 0, LPARAM(L"Connection CLOSED"));
											pMainWnd->DisconnectFromFeed(RITHMIC);
											break;
			case ALERT_CONNECTION_BROKEN:	//SendMessage(pWnd->GetDlgItem(IDC_CONNECTION_STATUS_RITHMIC)->m_hWnd, WM_SETTEXT, 0, LPARAM(L"Connection BROKEN"));
											break;
			case ALERT_LOGIN_COMPLETE:		//SendMessage(pWnd->GetDlgItem(IDC_CONNECTION_STATUS_RITHMIC)->m_hWnd, WM_SETTEXT, 0, LPARAM(L"Login COMPLETE"));
											break;		
			case ALERT_LOGIN_FAILED:		//PostMessage(pWnd->GetDlgItem(IDC_CONNECTION_STATUS_RITHMIC)->m_hWnd, WM_SETTEXT, 0, LPARAM(L"Login FAILED"));
											break;
			case ALERT_SERVICE_ERROR:		//SendMessage(pWnd->GetDlgItem(IDC_CONNECTION_STATUS_RITHMIC)->m_hWnd, WM_SETTEXT, 0, LPARAM(L"Service ERROR"));
											break;
			case ALERT_FORCED_LOGOUT:		//SendMessage(pWnd->GetDlgItem(IDC_CONNECTION_STATUS_RITHMIC)->m_hWnd, WM_SETTEXT, 0, LPARAM(L"Forced LOGOUT"));
											pMainWnd->DisconnectFromFeed(RITHMIC);
											break;
			case ALERT_QUIET_HEARTBEAT:		//SendMessage(pWnd->GetDlgItem(IDC_CONNECTION_STATUS_RITHMIC)->m_hWnd, WM_SETTEXT, 0, LPARAM(L"Quiet HEARTBEAT"));
											break;
			case ALERT_SHUTDOWN_SIGNAL:		break;
		}
	}

	//если есть инструменты со статусом Disconnect, то восстанавливаем их статус
	if (pInfo->iAlertType == ALERT_LOGIN_COMPLETE) 
	{
		int selRow = pMainWnd->instrumentList.GetItemCount();
		//CStringA name, exchange;
		//tsNCharcb sExchange, sTicker;
		
		for (int j = 0; j < selRow; ++j)
		{
			if (pMainWnd->instrumentList.GetItemText(j, 1) == L"Rithmic" && pMainWnd->instrumentList.GetItemText(j, 4) == L"Disconnect") 
			{
				pMainWnd->instrumentList.SetItemText(j, 4, L"Getting Data");
				//получаем имя текущего инструмента
				/*name = ((RithmicDialog*)pWnd)->instrumentList.GetItemText(j, 1);
				exchange = ((RithmicDialog*)pWnd)->instrumentList.GetItemText(j, 4);
				sTicker.pData = (char*)name.GetString();
				sTicker.iDataLen = name.GetLength();
				sExchange.pData = (char*)exchange.GetString();
				sExchange.iDataLen = exchange.GetLength();
		
				IsRealTicker(sTicker, sExchange);*/
			}
		}
	}

	*aiCode = API_OK;
    return (OK);
}

int RithmicCallbacks::RefData(RefDataInfo* pInfo, void* pContext, int* aiCode)
{
	DEBUG_LOG("RefData(): sTicker: %s; sExchange: %s; sDescription: %s; sProductCode: %s; sPeriodCode: %s; sRpCode: %s",
				std::string(pInfo->sTicker.pData, pInfo->sTicker.iDataLen).c_str(),
				std::string(pInfo->sExchange.pData, pInfo->sExchange.iDataLen).c_str(),
				std::string(pInfo->sDescription.pData, pInfo->sDescription.iDataLen).c_str(),
				std::string(pInfo->sProductCode.pData, pInfo->sProductCode.iDataLen).c_str(),
				std::string(pInfo->sPeriodCode.pData, pInfo->sPeriodCode.iDataLen).c_str(),
			    std::string(pInfo->sRpCode.pData, pInfo->sRpCode.iDataLen).c_str())

	std::string str(pInfo->sTicker.pData, pInfo->sTicker.iDataLen);

	//проверяем существует инструмент или нет
	if (pInfo->iRpCode == 7) {
		StringCchPrintfW(werrstr, 100, L"Rithmic: Invalid instrument: %S, exchange: %.*S", str.c_str(), pInfo->sExchange.iDataLen, pInfo->sExchange.pData);
		pMainWnd->UpdateActivityWindow(werrstr);
		*aiCode = API_OK; return (OK);
	}

	bool disconnectInstr = false;
	for (size_t i = 0; i < rithmic_data.size(); ++i) {
		if (!rithmic_data[i]->GetName().compare(str) && rithmic_data[i]->GetMode().compare("Disconnect")) {
			StringCchPrintfW(werrstr, 100, L"Rithmic: Instrument already in list: %S", str.c_str());
			pMainWnd->UpdateActivityWindow(werrstr);
			*aiCode = API_OK; return (OK);
		} else if (!rithmic_data[i]->GetName().compare(str) && !rithmic_data[i]->GetMode().compare("Disconnect")) {
			disconnectInstr = true;
			break;
		}
	}

	//если инструмента не было в списке, т.е. он новый, то заполняем базовые данные по нему
	if (!disconnectInstr) 
	{
		//создаем структуры необходимые для инструмента, в соответсвии с выбранным флагом
		RTicker* tickerPtr;
		
		/*switch (pMainWnd->instrumentTypeFlag)
		{
		case TRADES_ONLY: 
			_i = new RTradeTicker(str); 
			_i->iFlags = MD_PRINTS | MD_MARKET_MODE;
			if (pMainWnd->spreadLegTrades)
				_i->iFlags |= MD_PRINTS_COND;
			break;
		case TRADES_DOM:*/
			tickerPtr = new RDOMTicker(str);
			tickerPtr->iFlags = MD_PRINTS | MD_QUOTES | MD_MARKET_MODE;
			if (pMainWnd->spreadLegTrades)
				tickerPtr->iFlags |= MD_PRINTS_COND;
			/*break;
		}*/

		//поле для возможности отличия одинаковых инструментов у раных фидов
		//_i->SetID(MainWindow::Rithmic);

		//получаем описание текущего инструмента
		char safe_str[100];
		StringCchPrintfA(safe_str, 100, "%.*s %.*s", pInfo->sDescription.iDataLen, pInfo->sDescription.pData,
			pInfo->sProductCode.iDataLen, pInfo->sProductCode.pData);
		tickerPtr->SetDescription(safe_str);

		//дата экспирации
		StringCchPrintfA(safe_str, 100, "%.*s", pInfo->sExpiration.iDataLen, pInfo->sExpiration.pData);
		tickerPtr->SetExpiration(safe_str);

		//время экспирации
		//StringCchPrintfA(safe_str, 100, "%.*s", pInfo->sExpirationTime.iDataLen, pInfo->sExpirationTime.pData);
		//CTime time(atoi(safe_str));
		//StringCchPrintfA(safe_str, 100, "%d:%d:%d", time.GetHour(), time.GetMinute(), time.GetSecond());
		//_i->SetExpTime(safe_str);

		//биржа
		StringCchPrintfA(safe_str, 100, "%.*s", pInfo->sExchange.iDataLen, pInfo->sExchange.pData);
		tickerPtr->SetExchange(safe_str);

		//заносим данные по инструменту в контейнер и в список
		InsertInstrument(tickerPtr);	
	}
	
	//получаем шаг цены инструмента
	if (!pEngine->getPriceIncrInfo(&pInfo->sExchange, &pInfo->sTicker, &errorCode))
    {
		StringCchPrintfW(werrstr, 100, L"Rithmic: REngine::getPriceIncrInfo() Error : %d, ", errorCode);
		pMainWnd->UpdateActivityWindow(werrstr);
		*aiCode = API_OK; return (OK);
	}

	*aiCode = API_OK; return (OK);
}

int RithmicCallbacks::PriceIncrUpdate(PriceIncrInfo* pInfo, void* pContext, int* aiCode)
{
	DEBUG_LOG("PriceIncrUpdate(): sTicker: %s; sExchange: %s; sRpCode: %s;",
			  std::string(pInfo->sTicker.pData, pInfo->sTicker.iDataLen).c_str(),
			  std::string(pInfo->sExchange.pData, pInfo->sExchange.iDataLen).c_str(),
			  std::string(pInfo->sRpCode.pData, pInfo->sRpCode.iDataLen).c_str())

	//определяем индекс текущего инструмента в контейнере инструментов
	size_t i = 0; 
	std::string str(pInfo->sTicker.pData, pInfo->sTicker.iDataLen);
	
	::AcquireSRWLockExclusive(&SRWLockRithmic);
	while (rithmic_data[i]->GetName().compare(str) != 0 && i < rithmic_data.size())
			++i;

	//устанавливаем шаг цены и точность
	if (pInfo->iArrayLen != 0)
	{
		rithmic_data[i]->SetPrecision(pInfo->asPriceIncrArray->iPrecision);
		rithmic_data[i]->SetMinTick(ToInt(pInfo->asPriceIncrArray->dPriceIncr, pInfo->asPriceIncrArray->iPrecision));
	}
	::ReleaseSRWLockExclusive(&SRWLockRithmic);

	//подписываемся на инструмент
	if (!pEngine->subscribe(&pInfo->sExchange, &pInfo->sTicker, ((RTicker*)rithmic_data[rithmic_data.size()-1])->iFlags, &errorCode))
    {
		StringCchPrintfW(werrstr, 100, L"Rithmic: REngine subscribe error : %d, ", errorCode);
		pMainWnd->UpdateActivityWindow(werrstr);
	} else {
		StringCchPrintfW(werrstr, 100, L"Rithmic: Instrument subscribed: %.*S", pInfo->sTicker.iDataLen, pInfo->sTicker.pData);
		pMainWnd->UpdateActivityWindow(werrstr);
	}

	*aiCode = API_OK; return(OK);
}

int RithmicCallbacks::MarketMode(MarketModeInfo* pInfo, void* pContext, int* aiCode)
{
	DEBUG_LOG("MarketMode(): sTicker: %s; sExchange: %s; sMarketMode: %s; sReason: %s; sEvent: %s",
			  std::string(pInfo->sTicker.pData, pInfo->sTicker.iDataLen).c_str(),
			  std::string(pInfo->sExchange.pData, pInfo->sExchange.iDataLen).c_str(),
			  std::string(pInfo->sMarketMode.pData, pInfo->sMarketMode.iDataLen).c_str(),
			  std::string(pInfo->sReason.pData, pInfo->sReason.iDataLen).c_str(),
			  std::string(pInfo->sEvent.pData, pInfo->sEvent.iDataLen).c_str())

	CStringA nameA;
	wchar_t wstr[100];

	//определяем индекс текущего инструмента в контейнере инструментов
	size_t i = 0; 
	std::string str(pInfo->sTicker.pData, pInfo->sTicker.iDataLen), 
		mode(pInfo->sMarketMode.pData, pInfo->sMarketMode.iDataLen);
	
	::AcquireSRWLockExclusive(&SRWLockRithmic);
	while (rithmic_data[i]->GetName().compare(str) != 0 && i < rithmic_data.size())
			++i;

	rithmic_data[i]->SetMode(mode.c_str());
	::ReleaseSRWLockExclusive(&SRWLockRithmic);

	//ищем номер инструмента в списке
	for (int x = 0; x < pMainWnd->instrumentList.GetItemCount(); ++x)
	{
		nameA = pMainWnd->instrumentList.GetItemText(x,2);
		if (nameA == str.c_str()) {
			StringCchPrintfW(wstr, 100, L"%S", mode.c_str()); 
			pMainWnd->instrumentList.SetItemText(0, 4, wstr);
		}
	}

    *aiCode = API_OK; return (OK);
}

int RithmicCallbacks::TradeReplay(TradeReplayInfo* pInfo, void* pContext, int* aiCode)
{	
	//вызов этой функции происходит после того, как полностью подкачаны все исторические тики
	size_t i = 0; 
	std::string name(pInfo->sTicker.pData, pInfo->sTicker.iDataLen);
	TCHAR str[64];
	StringCchPrintfW(str, 64, _T("Rithmic: Historical data for %S loaded"), name.c_str());
	pMainWnd->UpdateActivityWindow(str);

	::AcquireSRWLockExclusive(&SRWLockRithmic);
	//определяем индекс текущего инструмента в контейнере инструментов
	if (rithmic_data.size()) {
		while (i < rithmic_data.size() && rithmic_data[i]->GetName().compare(name) != 0)
			++i;
	}

	if (!rithmic_data.size() || i == rithmic_data.size()) //значит инструмента уже нету в контейнере (такое возможно при ансабскрайбе, когда запоздалые данные еще идут)
	{ 
		::ReleaseSRWLockExclusive(&SRWLockRithmic);
		*aiCode = API_OK; return (OK); 
	}

	//сортируем вектор и объединяем его с реальными данными
	((RDOMTicker*)rithmic_data[i])->CombineData();

	rithmic_data[i]->SetHistoryFlag(2);

	::ReleaseSRWLockExclusive(&SRWLockRithmic);
    
	*aiCode = API_OK; return (OK);
}

int RithmicCallbacks::TradePrint(TradeInfo* pInfo, void* pContext, int* aiCode)
{	
	DEBUG_LOG("TradePrint(): iSsboe: %.2d:%.2d:%.2d.%.6d; sTicker: %s; sExchange: %s; dPrice: %f; iSize: %d; sAggressorSide: %s; sCondition: %s;"
			  "dNetChange: %f; dPercentChange: %f; iVolumeBought: %d; iVolumeSold: %d; iType: %s; iSourceSsboe: %.2d:%.2d:%.2d.%.6d",
			  CTime(pInfo->iSsboe).GetHour(), CTime(pInfo->iSsboe).GetMinute(), CTime(pInfo->iSsboe).GetSecond(), pInfo->iUsecs,
			  std::string(pInfo->sTicker.pData, pInfo->sTicker.iDataLen).c_str(),
			  std::string(pInfo->sExchange.pData, pInfo->sExchange.iDataLen).c_str(),
			  pInfo->bPriceFlag ? pInfo->dPrice : 0.0,
			  pInfo->iSize,
			  std::string(pInfo->sAggressorSide.pData, pInfo->sAggressorSide.iDataLen).c_str(),
			  std::string(pInfo->sCondition.pData, pInfo->sCondition.iDataLen).c_str(),
			  pInfo->bNetChangeFlag ? pInfo->dNetChange : 0.0,
			  pInfo->bPercentChangeFlag ? pInfo->dPercentChange : 0.0,
			  pInfo->bVolumeBoughtFlag ? pInfo->iVolumeBought : 0,
			  pInfo->bVolumeSoldFlag ? pInfo->iVolumeSold : 0,
			  (pInfo->iType == 1) ? "MD_IMAGE_CB==1" : ((pInfo->iType == 2) ? "MD_UPDATE_CB ==2" : "MD_HISTORY_CB==3"),
			  CTime(pInfo->iSourceSsboe).GetHour(), CTime(pInfo->iSourceSsboe).GetMinute(), CTime(pInfo->iSourceSsboe).GetSecond(), pInfo->iSourceUsecs)

	//заплатка для трейда с нулевым объемом который перед пре-маркетом идет
	if (!pInfo->iSize)
	{ *aiCode = API_OK; return (OK); }

	//определяем индекс текущего инструмента в контейнере инструментов
	size_t i = 0; 
	std::string isin(pInfo->sTicker.pData, pInfo->sTicker.iDataLen);
	
	::AcquireSRWLockExclusive(&SRWLockRithmic);
	if (rithmic_data.size()) 
	{
		while (i < rithmic_data.size() && rithmic_data[i]->GetName().compare(isin) != 0)
			++i;
	}

	if (!rithmic_data.size() || i == rithmic_data.size()) //значит инструмента уже нету в контейнере (такое возможно при ансабскрайбе, когда запоздалые данные еще идут)
	{ 
		::ReleaseSRWLockExclusive(&SRWLockRithmic);
		*aiCode = API_OK; return (OK); 
	}

	// был трейд занчит после него придут изменения лимитов, которые нужно пропустить, для этого выставляем флаг
	((RDOMTicker*)rithmic_data[i])->SkipChanges() = true;

	//заносим данные трейда в нужный инструмент
	rithmic_data[i]->TradePrint(pInfo);
	
	::ReleaseSRWLockExclusive(&SRWLockRithmic);

    *aiCode = API_OK; return (OK);
}

int RithmicCallbacks::TradeCondition(TradeInfo* pInfo, void* pContext, int* aiCode)
{
	DEBUG_LOG("TradeCondition(): sTicker: %s; sExchange: %s; dPrice: %f; iSize: %d; sAggressorSide: %s; sCondition: %s; "
			  "dNetChange: %f; dPercentChange: %f; iVolumeBought: %d; iVolumeSold: %d; iType: %s",
			  std::string(pInfo->sTicker.pData, pInfo->sTicker.iDataLen).c_str(),
			  std::string(pInfo->sExchange.pData, pInfo->sExchange.iDataLen).c_str(),
			  pInfo->bPriceFlag ? pInfo->dPrice : 0.0,
			  pInfo->iSize,
			  std::string(pInfo->sAggressorSide.pData, pInfo->sAggressorSide.iDataLen).c_str(),
			  std::string(pInfo->sCondition.pData, pInfo->sCondition.iDataLen).c_str(),
			  pInfo->bNetChangeFlag ? pInfo->dNetChange : 0.0,
			  pInfo->bPercentChangeFlag ? pInfo->dPercentChange : 0.0,
			  pInfo->bVolumeBoughtFlag ? pInfo->iVolumeBought : 0,
			  pInfo->bVolumeSoldFlag ? pInfo->iVolumeSold : 0,
			  (pInfo->iType == 1) ? "MD_IMAGE_CB==1" : ((pInfo->iType == 2) ? "MD_UPDATE_CB==2" : "MD_HISTORY_CB==3"))

    //заплатка для трейда с нулевым объемом который перед пре-маркетом идет
	if (!pInfo->iSize)
	{
		 *aiCode = API_OK; return (OK);
	}

	//определяем индекс текущего инструмента в контейнере инструментов
	size_t i = 0; 
	std::string isin(pInfo->sTicker.pData, pInfo->sTicker.iDataLen);
	
	::AcquireSRWLockExclusive(&SRWLockRithmic);
	if (rithmic_data.size()) {
		while (i < rithmic_data.size() && rithmic_data[i]->GetName().compare(isin) != 0)
			++i;
	}

	if (!rithmic_data.size() || i == rithmic_data.size()) //значит инструмента уже нету в контейнере (такое возможно при ансабскрайбе, когда запоздалые данные еще идут)
	{ 
		::ReleaseSRWLockExclusive(&SRWLockRithmic);
		*aiCode = API_OK; return (OK); 
	}

	//заносим данные трейда в нужный инструмент если у этого инстумента есть подписка на кондишены
	if (rithmic_data[i]->iFlags & MD_PRINTS_COND)
		rithmic_data[i]->TradePrint(pInfo);
	
	::ReleaseSRWLockExclusive(&SRWLockRithmic);
    *aiCode = API_OK; return (OK);
}

int RithmicCallbacks::BestAskQuote(AskInfo* pInfo, void* pContext, int* aiCode)
{
	DEBUG_LOG("BestAskQuote(): iSsboe: %.2d:%.2d:%.2d.%.6d; sTicker: %s; sExchange: %s; dPrice: %f; bSizeFlag: %s; iSize: %d; iNumOrders: %d;",
			CTime(pInfo->iSsboe).GetHour(), CTime(pInfo->iSsboe).GetMinute(), CTime(pInfo->iSsboe).GetSecond(), pInfo->iUsecs,
			std::string(pInfo->sTicker.pData, pInfo->sTicker.iDataLen).c_str(),
			std::string(pInfo->sExchange.pData, pInfo->sExchange.iDataLen).c_str(),
			pInfo->bPriceFlag ? pInfo->dPrice : 0.0,
			pInfo->bSizeFlag ? "true" : "false",
			pInfo->bSizeFlag ? pInfo->iSize : 0,
			pInfo->iNumOrders)

	//определяем индекс текущего инструмента в контейнере инструментов
	size_t i = 0; 
	std::string name(pInfo->sTicker.pData, pInfo->sTicker.iDataLen);
	
	::AcquireSRWLockExclusive(&SRWLockRithmic);
	if (rithmic_data.size()) {
		while (i < rithmic_data.size() && rithmic_data[i]->GetName().compare(name) != 0)
			++i;
	}

	//если инструмента не оказалось
	if (!rithmic_data.size() || i == rithmic_data.size()) 
	{ 
		::ReleaseSRWLockExclusive(&SRWLockRithmic);
		*aiCode = API_OK; return (OK); 
	}

	//заносим данные в нужный инструмент
	//((RBBOTicker*)rithmic_data[i])->BestAskQuote(pInfo);

	::ReleaseSRWLockExclusive(&SRWLockRithmic);

	 *aiCode = API_OK; return (OK);
}

int RithmicCallbacks::BestBidQuote(BidInfo* pInfo, void* pContext, int* aiCode)
{
	DEBUG_LOG("BestBidQuote(): iSsboe: %.2d:%.2d:%.2d.%.6d; sTicker: %s; sExchange: %s; dPrice: %f; bSizeFlag: %s; iSize: %d; iNumOrders: %d;",
		CTime(pInfo->iSsboe).GetHour(), CTime(pInfo->iSsboe).GetMinute(), CTime(pInfo->iSsboe).GetSecond(), pInfo->iUsecs,
		std::string(pInfo->sTicker.pData, pInfo->sTicker.iDataLen).c_str(),
		std::string(pInfo->sExchange.pData, pInfo->sExchange.iDataLen).c_str(),
		pInfo->bPriceFlag ? pInfo->dPrice : 0.0,
		pInfo->bSizeFlag ? "true" : "false",
		pInfo->bSizeFlag ? pInfo->iSize : 0,
		pInfo->iNumOrders)

	//определяем индекс текущего инструмента в контейнере инструментов
	size_t i = 0; 
	std::string name(pInfo->sTicker.pData, pInfo->sTicker.iDataLen);
	
	::AcquireSRWLockExclusive(&SRWLockRithmic);
	if (rithmic_data.size()) {
		while (i < rithmic_data.size() && rithmic_data[i]->GetName().compare(name) != 0)
			++i;
	}

	//если инструмента не оказалось
	if (!rithmic_data.size() || i == rithmic_data.size()) 
	{ 
		::ReleaseSRWLockExclusive(&SRWLockRithmic);
		*aiCode = API_OK; return (OK); 
	}

	//заносим данные в нужный инструмент
	//((RBBOTicker*)rithmic_data[i])->BestBidQuote(pInfo);

	::ReleaseSRWLockExclusive(&SRWLockRithmic);

	 *aiCode = API_OK; return (OK);
}

int RithmicCallbacks::BestBidAskQuote(BidInfo* pBid, AskInfo* pAsk, void* pContext, int* aiCode)
 {
	DEBUG_LOG("BestBidAskQuote(): iSsboe: %.2d:%.2d:%.2d.%.6d; sTicker: %s; sExchange: %s; Bid_dPrice: %f; Bid_bSizeFlag: %s; Bid_iSize: %d; Bid_iNumOrders: %d; "
			  "Bid_dPrice: %f; Bid_bSizeFlag: %s; Bid_iSize: %d; Bid_iNumOrders: %d; ",
			  CTime(pBid->iSsboe).GetHour(), CTime(pBid->iSsboe).GetMinute(), CTime(pBid->iSsboe).GetSecond(), pBid->iUsecs,
			  std::string(pBid->sTicker.pData, pBid->sTicker.iDataLen).c_str(),
			  std::string(pBid->sExchange.pData, pBid->sExchange.iDataLen).c_str(),
			  pBid->bPriceFlag ? pBid->dPrice : 0.0,
			  pBid->bSizeFlag ? "true" : "false",
			  pBid->bSizeFlag ? pBid->iSize : 0,
			  pBid->iNumOrders,
			  pAsk->bPriceFlag ? pAsk->dPrice : 0.0,
			  pAsk->bSizeFlag ? "true" : "false",
			  pAsk->bSizeFlag ? pAsk->iSize : 0,
			  pAsk->iNumOrders)

	//определяем индекс текущего инструмента в контейнере инструментов
	//size_t i = 0; 
	//std::string name(pBid->sTicker.pData, pBid->sTicker.iDataLen);
	//
	//::AcquireSRWLockExclusive(&SRWLockRithmic);
	//if (rithmic_data.size()) {
	//	while (i < rithmic_data.size() && rithmic_data[i]->GetName().compare(name) != 0)
	//		++i;
	//}

	////если инструмента не оказалось
	//if (!rithmic_data.size() || i == rithmic_data.size()) 
	//{ 
	//	::ReleaseSRWLockExclusive(&SRWLockRithmic);
	//	*aiCode = API_OK; return (OK); 
	//}

	////заносим данные в нужный инструмент
	//((RBBOTicker*)rithmic_data[i])->BestBidAskQuote(pBid, pAsk);

	//::ReleaseSRWLockExclusive(&SRWLockRithmic);
	 *aiCode = API_OK; return (OK);
 }

int RithmicCallbacks::LimitOrderBook(LimitOrderBookInfo* pInfo, void* pContext, int* aiCode)
{
	DEBUG_LOG("%s", "LimitOrderBook()")

	//определяем индекс текущего инструмента в контейнере инструментов
	size_t i = 0; 
	std::string name(pInfo->sTicker.pData, pInfo->sTicker.iDataLen);
	
	::AcquireSRWLockExclusive(&SRWLockRithmic);
	if (rithmic_data.size()) 
	{
		while (i < rithmic_data.size() && rithmic_data[i]->GetName().compare(name) != 0)
			++i;
	}

	//если инструмента не оказалось
	if (!rithmic_data.size() || i == rithmic_data.size())
	{ 
		::ReleaseSRWLockExclusive(&SRWLockRithmic);
		*aiCode = API_OK; return (OK); 
	}

	//заносим данные в нужный инструмент
	((RDOMTicker*)rithmic_data[i])->LimitOrderBook(pInfo);

	::ReleaseSRWLockExclusive(&SRWLockRithmic);
	 *aiCode = API_OK; return (OK);
}
int RithmicCallbacks::AskQuote(AskInfo* pInfo, void* pContext, int* aiCode)
{
	DEBUG_LOG("AskQuote(): iSsboe: %.2d:%.2d:%.2d.%.6d; sTicker: %s; sExchange: %s; dPrice: %f; bSizeFlag: %s; iSize: %d; iNumOrders: %d; "
			  "iType: %s; iUpdateType: %s",
			  CTime(pInfo->iSsboe).GetHour(), CTime(pInfo->iSsboe).GetMinute(), CTime(pInfo->iSsboe).GetSecond(), pInfo->iUsecs,
			  std::string(pInfo->sTicker.pData, pInfo->sTicker.iDataLen).c_str(),
			  std::string(pInfo->sExchange.pData, pInfo->sExchange.iDataLen).c_str(),
			  pInfo->bPriceFlag ? pInfo->dPrice : 0.0,
			  pInfo->bSizeFlag ? "true" : "false",
			  pInfo->bSizeFlag ? pInfo->iSize : 0,
			  pInfo->iNumOrders, 
			  (pInfo->iType == 1) ? "MD_IMAGE_CB==1" : ((pInfo->iType == 2) ? "MD_UPDATE_CB==2" : "MD_HISTORY_CB==3"),
			  pInfo->iUpdateType == 0 ? "UPDATE_TYPE_UNDEFINED==0" :
				(pInfo->iUpdateType == 1) ? "UPDATE_TYPE_SOLO==1" :
				(pInfo->iUpdateType == 2) ? "UPDATE_TYPE_BEGIN==2" :
				(pInfo->iUpdateType == 3) ? "UPDATE_TYPE_MIDDLE==3" :
				(pInfo->iUpdateType == 4) ? "UPDATE_TYPE_END==4" : "UPDATE_TYPE_CLEAR==5")

	//TRACE("A: ticker: %.*s, time: %.2d:%.2d:%.2d.%.6d, price: %f, limit_size: %d, orders_size: %d, flag: %d\n", pInfo->sTicker.iDataLen, pInfo->sTicker.pData,
	//	/*gmtime((const time_t*)*/CTime(pInfo->iSsboe).GetHour(), CTime(pInfo->iSsboe).GetMinute(), CTime(pInfo->iSsboe).GetSecond(), pInfo->iUsecs, 
	//	pInfo->dPrice, pInfo->iSize, pInfo->iNumOrders, pInfo->iUpdateType);

	//определяем индекс текущего инструмента в контейнере инструментов
	size_t i = 0; 
	std::string isin(pInfo->sTicker.pData, pInfo->sTicker.iDataLen);
	
	::AcquireSRWLockExclusive(&SRWLockRithmic);
	if (rithmic_data.size()) 
	{
		while (i < rithmic_data.size() && rithmic_data[i]->GetName().compare(isin) != 0)
			++i;
	}

	if (!rithmic_data.size() || i == rithmic_data.size())	//если инструмента не оказалось
	{ 
		::ReleaseSRWLockExclusive(&SRWLockRithmic);
		*aiCode = API_OK; return (OK); 
	}

	((RDOMTicker*)rithmic_data[i])->AskQuote(pInfo);	//заносим данные в нужный инструмент

	::ReleaseSRWLockExclusive(&SRWLockRithmic);
	 *aiCode = API_OK; return (OK);
}
int RithmicCallbacks::BidQuote(BidInfo* pInfo, void* pContext,  int* aiCode)
{
	DEBUG_LOG("BidQuote(): iSsboe: %.2d:%.2d:%.2d.%.6d; sTicker: %s; sExchange: %s; dPrice: %f; bSizeFlag: %s; iSize: %d; iNumOrders: %d; "
			  "iType: %s; iUpdateType: %s",
			  CTime(pInfo->iSsboe).GetHour(), CTime(pInfo->iSsboe).GetMinute(), CTime(pInfo->iSsboe).GetSecond(), pInfo->iUsecs,
			  std::string(pInfo->sTicker.pData, pInfo->sTicker.iDataLen).c_str(),
			  std::string(pInfo->sExchange.pData, pInfo->sExchange.iDataLen).c_str(),
			  pInfo->bPriceFlag ? pInfo->dPrice : 0.0,
			  pInfo->bSizeFlag ? "true" : "false",
			  pInfo->bSizeFlag ? pInfo->iSize : 0,
			  pInfo->iNumOrders,
		      (pInfo->iType == 1) ? "MD_IMAGE_CB==1" : ((pInfo->iType == 2) ? "MD_UPDATE_CB==2" : "MD_HISTORY_CB==3"),
			  pInfo->iUpdateType == 0 ? "UPDATE_TYPE_UNDEFINED==0" : 
				(pInfo->iUpdateType == 1) ? "UPDATE_TYPE_SOLO==1" :
				(pInfo->iUpdateType == 2) ? "UPDATE_TYPE_MIDDLE==3" :
				(pInfo->iUpdateType == 3) ? "UPDATE_TYPE_MIDDLE==3" :
				(pInfo->iUpdateType == 4) ? "UPDATE_TYPE_END==4" : "UPDATE_TYPE_CLEAR==5")

	//TRACE("B: ticker: %.*s, time: %.2d:%.2d:%.2d.%.6d, price: %f, limit_size: %d, orders_size: %d, flag: %d\n", pInfo->sTicker.iDataLen, pInfo->sTicker.pData,
	//	/*gmtime((const time_t*)*/CTime(pInfo->iSsboe).GetHour(), CTime(pInfo->iSsboe).GetMinute(), CTime(pInfo->iSsboe).GetSecond(), pInfo->iUsecs, 
	//	pInfo->dPrice, pInfo->iSize, pInfo->iNumOrders, pInfo->iUpdateType);

	//определяем индекс текущего инструмента в контейнере инструментов
	size_t i = 0; 
	std::string isin(pInfo->sTicker.pData, pInfo->sTicker.iDataLen);
	
	::AcquireSRWLockExclusive(&SRWLockRithmic);
	if (rithmic_data.size()) 
	{
		while (i < rithmic_data.size() && rithmic_data[i]->GetName().compare(isin) != 0)
			++i;
	}
	
	if (!rithmic_data.size() || i == rithmic_data.size())	//если инструмента не оказалось
	{ 
		::ReleaseSRWLockExclusive(&SRWLockRithmic);
		*aiCode = API_OK; return (OK); 
	}

	((RDOMTicker*)rithmic_data[i])->BidQuote(pInfo);	//заносим данные в нужный инструмент

	::ReleaseSRWLockExclusive(&SRWLockRithmic);
	 *aiCode = API_OK; return (OK);
}

int RithmicCallbacks::Connect()
{
	try
	{
		if (rEngineParams.pAdmCallbacks != NULL)
		{
			delete rEngineParams.pAdmCallbacks;
		}
		rEngineParams.pAdmCallbacks = new MyAdmCallbacks();
	}
    catch (OmneException& oEx)
    {
		errorCode = oEx.getErrorCode();
		StringCchPrintfA(errstr, 100, "Rithmic: MyAdmCallbacks Error : %d, %s", errorCode, oEx.getErrorString());
		return 1;
    }
	
	try
    {
		if (pEngine != NULL)
		{
			delete pEngine;
		}
	    pEngine = new REngine(&rEngineParams);
    }
    catch (OmneException& oEx)
    {
		pEngine = NULL;
		delete rEngineParams.pAdmCallbacks; rEngineParams.pAdmCallbacks = NULL;
		errorCode = oEx.getErrorCode();
		StringCchPrintfA(errstr, 100, "Rithmic: REngine create error: code=%d, string=%s", errorCode, oEx.getErrorString());
		return 1;
    }

	oLoginParams.pCallbacks = this;

    if (!pEngine->login(&oLoginParams, &errorCode))
    {
		delete pEngine; pEngine = NULL; 
        delete rEngineParams.pAdmCallbacks; rEngineParams.pAdmCallbacks = NULL;
		StringCchPrintfA(errstr, 100, "Rithmic: REngine login error: code=%d", errorCode);
		return 1;
	}
	else
	{
		//версия АПИ
		tsNCharcb apiVer;
		pEngine->getVersion(&apiVer, &errorCode);
		StringCchPrintfA(errstr, 100, "R | API version: %.*s", apiVer.iDataLen, apiVer.pData);
	}

	/*   ----------------------------------------------------------------   */
    /*   After calling REngine::login, RCallbacks::Alert may be called a    */
    /*   number of times.  Wait for when the login to the MdCnnctPt is      */
    /*   complete.  (See MyCallbacks::Alert() for details).                 */

	//while (!g_bMdLoginComplete)
	//	Sleep(1000);
	Sleep(1000);
	//g_bMdLoginComplete = false;

	return 0;
}

void RithmicCallbacks::Disconnect()
{
	if (pEngine != NULL) 
	{
		pEngine->logout(&errorCode);
		delete pEngine;
		pEngine = NULL;
	}

	if (rEngineParams.pAdmCallbacks != NULL) 
	{
		delete rEngineParams.pAdmCallbacks;
		rEngineParams.pAdmCallbacks = NULL;
	}
}

void RithmicCallbacks::IsRealTicker(tsNCharcb& ticker, tsNCharcb& exchange)
{
	if (!pEngine->getRefData(&exchange, &ticker, &errorCode))
	{
		StringCchPrintfW(werrstr, 100, L"Rithmic: REngine getRefData Error : %d, ", errorCode);
		pMainWnd->UpdateActivityWindow(werrstr);
	}

	return;
}
void RithmicCallbacks::UnSubscribe(tsNCharcb& ticker, tsNCharcb& exchange)
{
	if (pEngine)
	{
		if (!pEngine->unsubscribe(&exchange, &ticker, &errorCode))
		{
			StringCchPrintfW(werrstr, 100, L"Rithmic: REngine unsubscribe error: %d", errorCode);
			pMainWnd->UpdateActivityWindow(werrstr);
		} else {
			StringCchPrintfW(werrstr, 100, L"Rithmic: Instrument unsubscribed: %.*S", ticker.iDataLen, ticker.pData);
			pMainWnd->UpdateActivityWindow(werrstr);
		}
	}
}

int RithmicCallbacks::ToInt(double x, const int p) const
{
	//переводим дробную часть в целую
	for (int k = p; k > 0; k--)
		x *= 10;
	
	return (int)(x+0.4);
}

void RithmicCallbacks::InsertInstrument(RTicker* i)
{
	//::AcquireSRWLockExclusive(&SRWLockRithmic);
	rithmic_data.push_back(i);
	//::ReleaseSRWLockExclusive(&SRWLockRithmic);

	//заносим название инструмента в список если его еще там нет
	/*bool findFlag = false;
	CStringA nameA;
	for (int x = 0; x < ((RithmicDialog*)pWnd)->instrumentList.GetItemCount(); ++x)
	{
		nameA = ((RithmicDialog*)pWnd)->instrumentList.GetItemText(x,1);
		if (nameA == rithmic_data[rithmic_data.size()-1]->GetName().c_str())
			findFlag = true;
	}*/
	wchar_t wstr[100];
//	if (!findFlag)
//	{
		pMainWnd->instrumentList.InsertItem(0, _T(""));
		pMainWnd->instrumentList.SetItemText(0, 1, L"Rithmic");
		StringCchPrintfW(wstr, 100, L"%S", i->GetName().c_str()); pMainWnd->instrumentList.SetItemText(0, 2, wstr);
		StringCchPrintfW(wstr, 100, L"%S", i->GetExchange().c_str()); pMainWnd->instrumentList.SetItemText(0, 5, wstr);
		StringCchPrintfW(wstr, 100, L"%S", i->GetExpiration().c_str()); pMainWnd->instrumentList.SetItemText(0, 6, wstr);
		StringCchPrintfW(wstr, 100, L"%S", i->GetDescription().c_str()); pMainWnd->instrumentList.SetItemText(0, 7, wstr);
		pMainWnd->instrumentList.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
//	}
}



int RithmicCallbacks::AccountList(AccountListInfo* pInfo, void* pContext, int* aiCode)
{
	////logs
	//CString str; SYSTEMTIME st; ::GetLocalTime(&st);
	//str.Format("[%d:%d:%d] Getting account data", st.wHour, st.wMinute, st.wSecond);
	//infoStr.push_back(str); p_mainWindow->SetTimer(1, USER_TIMER_MINIMUM, NULL);
	//p_mainWindow->UpdateLogFile(str.GetBuffer(), false);

	////заполняем данные Account Info
	//m_accInfo = *pInfo->asAccountInfoArray;

	////подписываемся на обновление PnL
	/////*if (!pEngine->subscribePnl(&m_accInfo, &iCode))
 ////   {
	////	str.Format("REngine::subscribePnl Error : %d, ", iCode);
	////	infoStr.push_back(str);
	////	p_mainWindow->SetTimer(1, USER_TIMER_MINIMUM, NULL);
	////} 
	////else 
	////{
	////	infoStr.push_back("PnL subscribe");
	////	p_mainWindow->SetTimer(1, USER_TIMER_MINIMUM, NULL);
	////}*/

	////это для комиссии
	//if (!pEngine->getProductRmsInfo(&m_accInfo, &iCode))
 //   {
	//	str.Format("REngine::getProductRmsInfo Error : %d, ", iCode);
	//	infoStr.push_back(str); p_mainWindow->SetTimer(1, USER_TIMER_MINIMUM, NULL);
	//	//p_mainWindow->UpdateActivityWindow(str.GetBuffer());
	//}

	////статус
	//str.SetString(pInfo->asAccountInfoArray->pRmsInfo->sStatus.pData, pInfo->asAccountInfoArray->pRmsInfo->sStatus.iDataLen);
	//m_accStatus.SetWindowTextA((LPCTSTR)str);

	////ID
	//str.SetString(pInfo->asAccountInfoArray->sAccountId.pData, pInfo->asAccountInfoArray->sAccountId.iDataLen);
	//m_accAccountID.SetWindowTextA(str);

	////PnL
	//str.Format("%g$", pInfo->asAccountInfoArray->pRmsInfo->dCashOnHand);
	//m_accPnL.SetWindowTextA(str);

	////MinBalance
	//str.Format("%g$", pInfo->asAccountInfoArray->pRmsInfo->dMinAccountBalance);
	//m_accMinBalance.SetWindowTextA(str);

	////CommissRate
	//str.Format("%g$", pInfo->asAccountInfoArray->pRmsInfo->dCommissionFillRate);
	//m_accCommissRate.SetWindowTextA(str);

	////MaxLoss
	//str.Format("%g$", pInfo->asAccountInfoArray->pRmsInfo->dLossLimit);
	//m_accMaxLoss.SetWindowTextA(str);

	////MaxOrderQty
	//str.Format("%d", pInfo->asAccountInfoArray->pRmsInfo->iMaxOrderQty);
	//m_accMaxOrderQty.SetWindowTextA(str);

	////BuyLimit
	//str.Format("%d", pInfo->asAccountInfoArray->pRmsInfo->iBuyLimit);
	//m_accBuyLimit.SetWindowTextA(str);

	////SellLimit
	//str.Format("%d", pInfo->asAccountInfoArray->pRmsInfo->iSellLimit);
	//m_accSellLimit.SetWindowTextA(str);

	////Algorithm
	//str.SetString(pInfo->asAccountInfoArray->pRmsInfo->sAlgorithm.pData, pInfo->asAccountInfoArray->pRmsInfo->sAlgorithm.iDataLen);
	//m_accAlgorithm.SetWindowTextA(str);

	//p_mainWindow->UpdateLogFile("", true);

	RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::TradeVolume(TradeVolumeInfo* pInfo, void* pContext, int* aiCode)
{
//#ifdef _DEBUG
//	CString s; SYSTEMTIME st; ::GetLocalTime(&st);
//	s.Format("[%d:%d:%d] TradeVolume(): %s ", st.wHour, st.wMinute, st.wSecond,	CString(pInfo->sTicker.pData, pInfo->sTicker.iDataLen));
//	p_mainWindow->UpdateLogFile(s.GetBuffer(), false);
//#endif
//
//	CString name(pInfo->sTicker.pData, pInfo->sTicker.iDataLen), str;
//	str.Format("%d", pInfo->iTotalVolume);
//	
//	for (int x = 0; x < p_mainWindow->instrumentList.GetItemCount(); ++x)
//	{
//		if (p_mainWindow->instrumentList.GetItemText(x,1) == name && p_mainWindow->instrumentList.GetItemText(x,2) != "History")
//			p_mainWindow->instrumentList.SetItemText(x, 6, str);
//	}
//
//#ifdef _DEBUG
//	p_mainWindow->UpdateLogFile("TradeVolume(): END\n", true);
//#endif

    RITHMIC_STUB(__FUNCTION__);
}
int RithmicCallbacks::OpenInterest(OpenInterestInfo* pInfo, void* pContext, int* aiCode)
{
//#ifdef _DEBUG
//	CString s; SYSTEMTIME st; ::GetLocalTime(&st);
//	s.Format("system date: %d-%d-%d, time: %d:%d:%d, OpenInterest: %s\n", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond,
//		CString(pInfo->sTicker.pData, pInfo->sTicker.iDataLen));
//	p_mainWindow->LogFile.Write(s, s.GetLength());
//#endif
//
//	CString name(pInfo->sTicker.pData, pInfo->sTicker.iDataLen), str;
//	str.Format("%d", pInfo->iQuantity);
//	
//	for (int x = 0; x < p_mainWindow->instrumentList.GetItemCount(); ++x)
//	{
//		if (p_mainWindow->instrumentList.GetItemText(x,1) == name && p_mainWindow->instrumentList.GetItemText(x,2) != "History")
//			p_mainWindow->instrumentList.SetItemText(x, 5, str);
//	}

    RITHMIC_STUB(__FUNCTION__);
}
int RithmicCallbacks::OpenPrice(OpenPriceInfo* pInfo, void* pContext, int* aiCode)
{
//#ifdef _DEBUG
//	CString s; SYSTEMTIME st; ::GetLocalTime(&st);
//	s.Format("system date: %d-%d-%d, time: %d:%d:%d, TradeVolume(): %s\n", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond,
//		CString(pInfo->sTicker.pData, pInfo->sTicker.iDataLen));
//	p_mainWindow->LogFile.Write(s, s.GetLength());
//#endif
//
//	CString name(pInfo->sTicker.pData, pInfo->sTicker.iDataLen), str;
//
//	//определяем индекс текущего инструмента в контейнере инструментов
//	int i = 0; 
//	while (i < mrithmic_dataContainer.size() && (mrithmic_dataContainer[i].name != name || mrithmic_dataContainer[i].mode == "History")) 
//			++i;
//
//	str.Format("%d", p_mainWindow->ToInt(pInfo->dPrice, i));
//	
//	for (int x = 0; x < p_mainWindow->instrumentList.GetItemCount(); ++x)
//	{
//		if (p_mainWindow->instrumentList.GetItemText(x,1) == name && p_mainWindow->instrumentList.GetItemText(x,2) != "History")
//			p_mainWindow->instrumentList.SetItemText(x, 7, str);
//	}

    RITHMIC_STUB(__FUNCTION__);
}
int RithmicCallbacks::HighPrice(HighPriceInfo* pInfo, void* pContext, int* aiCode)
{
//#ifdef _DEBUG
//	CString s; SYSTEMTIME st; ::GetLocalTime(&st);
//	s.Format("system date: %d-%d-%d, time: %d:%d:%d, TradeVolume(): %s\n", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond,
//		CString(pInfo->sTicker.pData, pInfo->sTicker.iDataLen));
//	p_mainWindow->LogFile.Write(s, s.GetLength());
//#endif
//
//	CString name(pInfo->sTicker.pData, pInfo->sTicker.iDataLen), str;
//
//	//определяем индекс текущего инструмента в контейнере инструментов
//	int i = 0; 
//	while (i < mrithmic_dataContainer.size() && (mrithmic_dataContainer[i].name != name || mrithmic_dataContainer[i].mode == "History")) 
//			++i;
//
//	str.Format("%d", p_mainWindow->ToInt(pInfo->dPrice, i));
//	
//	for (int x = 0; x < p_mainWindow->instrumentList.GetItemCount(); ++x)
//	{
//		if (p_mainWindow->instrumentList.GetItemText(x,1) == name && p_mainWindow->instrumentList.GetItemText(x,2) != "History")
//			p_mainWindow->instrumentList.SetItemText(x, 8, str);
//	}
     
	RITHMIC_STUB(__FUNCTION__);
}
int RithmicCallbacks::LowPrice(LowPriceInfo* pInfo, void* pContext, int* aiCode)
{
//#ifdef _DEBUG
//	CString s; SYSTEMTIME st; ::GetLocalTime(&st);
//	s.Format("system date: %d-%d-%d, time: %d:%d:%d, TradeVolume(): %s\n", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond,
//		CString(pInfo->sTicker.pData, pInfo->sTicker.iDataLen));
//	p_mainWindow->LogFile.Write(s, s.GetLength());
//#endif
//
//	CString name(pInfo->sTicker.pData, pInfo->sTicker.iDataLen), str;
//
//	//определяем индекс текущего инструмента в контейнере инструментов
//	int i = 0; 
//	while (i < mrithmic_dataContainer.size() && (mrithmic_dataContainer[i].name != name || mrithmic_dataContainer[i].mode == "History")) 
//			++i;
//
//	str.Format("%d", p_mainWindow->ToInt(pInfo->dPrice, i));
//	
//	for (int x = 0; x < p_mainWindow->instrumentList.GetItemCount(); ++x)
//	{
//		if (p_mainWindow->instrumentList.GetItemText(x,1) == name && p_mainWindow->instrumentList.GetItemText(x,2) != "History")
//			p_mainWindow->instrumentList.SetItemText(x, 9, str);
//	}

    RITHMIC_STUB(__FUNCTION__);
}
int RithmicCallbacks::ClosePrice(ClosePriceInfo* pInfo, void* pContext, int* aiCode)
{
//#ifdef _DEBUG
//	CString s; SYSTEMTIME st; ::GetLocalTime(&st);
//	s.Format("system date: %d-%d-%d, time: %d:%d:%d, TradeVolume(): %s\n", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond,
//		CString(pInfo->sTicker.pData, pInfo->sTicker.iDataLen));
//	p_mainWindow->LogFile.Write(s, s.GetLength());
//#endif
//
//	CString name(pInfo->sTicker.pData, pInfo->sTicker.iDataLen), str;
//
//	//определяем индекс текущего инструмента в контейнере инструментов
//	int i = 0; 
//	while (i < mrithmic_dataContainer.size() && (mrithmic_dataContainer[i].name != name || mrithmic_dataContainer[i].mode == "History")) 
//			++i;
//
//	str.Format("%d", p_mainWindow->ToInt(pInfo->dPrice, i));
//	
//	for (int x = 0; x < p_mainWindow->instrumentList.GetItemCount(); ++x)
//	{
//		if (p_mainWindow->instrumentList.GetItemText(x,1) == name && p_mainWindow->instrumentList.GetItemText(x,2) != "History")
//			p_mainWindow->instrumentList.SetItemText(x, 10, str);
//	}

    RITHMIC_STUB(__FUNCTION__);
}
int RithmicCallbacks::SettlementPrice(SettlementPriceInfo* pInfo, void* pContext, int* aiCode)
{
//#ifdef _DEBUG
//	CString s; SYSTEMTIME st; ::GetLocalTime(&st);
//	s.Format("system date: %d-%d-%d, time: %d:%d:%d, TradeVolume(): %s\n", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond,
//		CString(pInfo->sTicker.pData, pInfo->sTicker.iDataLen));
//	p_mainWindow->LogFile.Write(s, s.GetLength());
//#endif
//
//	CString name(pInfo->sTicker.pData, pInfo->sTicker.iDataLen), str;
//
//	//определяем индекс текущего инструмента в контейнере инструментов
//	int i = 0; 
//	while (i < mrithmic_dataContainer.size() && (mrithmic_dataContainer[i].name != name || mrithmic_dataContainer[i].mode == "History")) 
//			++i;
//
//	str.Format("%d", p_mainWindow->ToInt(pInfo->dPrice, i));
//	
//	for (int x = 0; x < p_mainWindow->instrumentList.GetItemCount(); ++x)
//	{
//		if (p_mainWindow->instrumentList.GetItemText(x,1) == name && p_mainWindow->instrumentList.GetItemText(x,2) != "History")
//			p_mainWindow->instrumentList.SetItemText(x, 11, str);
//	}

    RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::ProductRmsList(ProductRmsListInfo* pInfo, void* pContext, int* aiCode)
{
	//CString str, all("");
	//
	////CommissRate
	//str.Format("%g$", pInfo->asProductRmsInfoArray->dCommissionFillRate);
	//m_accCommissRate.SetWindowTextA(str);

	/*for (int i = 0; i < pInfo->iArrayLen; ++i)
	{
		str.SetString(pInfo->asProductRmsInfoArray[i].sProductCode.pData, pInfo->asProductRmsInfoArray[i].sProductCode.iDataLen);
		str += ": ";
		all += str;
		str.Format("%g$, ", pInfo->asProductRmsInfoArray[i].dBuyLongMarginRate);
		all += str;		
	}
	p_mainWindow->UpdateActivityWindow(all.GetBuffer());*/

    RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::StrategyList(StrategyListInfo *pInfo, void *pContext, int *aiCode)
{
	/*int exchangeLen = pInfo->iExchangeArrayLen,
		expirLen = pInfo->iExpirationCCYYMMArrayLen,
		productLen = pInfo->iProductCodeArrayLen,
		RpCode = pInfo->iRpCode,
		strategyLen = pInfo->iStrategyTypeArrayLen,
		tickerLen = pInfo->iTickerArrayLen, 
		i;

	CString str(pInfo->sRpCode.pData, pInfo->sRpCode.iDataLen);

	p_mainWindow->UpdateActivityWindow("Exchange array: ");
	for (i = 0; i < exchangeLen; ++i)
	{
		p_mainWindow->UpdateActivityWindow(CString(pInfo->pExchangeArray[i].pData, pInfo->pExchangeArray[i].iDataLen).GetBuffer());
	}
	
	p_mainWindow->UpdateActivityWindow("Expiration array: ");
	for (i = 0; i < expirLen; ++i)
	{
		p_mainWindow->UpdateActivityWindow(CString(pInfo->pExpirationCCYYMMArray[i].pData, pInfo->pExpirationCCYYMMArray[i].iDataLen).GetBuffer());
	}
	
	p_mainWindow->UpdateActivityWindow("Product code array: ");
	for (i = 0; i < productLen; ++i)
	{
		p_mainWindow->UpdateActivityWindow(CString(pInfo->pProductCodeArray[i].pData, pInfo->pProductCodeArray[i].iDataLen).GetBuffer());
	}
	
	p_mainWindow->UpdateActivityWindow("Strategy array: ");
	for (i = 0; i < strategyLen; ++i)
	{
		p_mainWindow->UpdateActivityWindow(CString(pInfo->pStrategyTypeArray[i].pData, pInfo->pStrategyTypeArray[i].iDataLen).GetBuffer());
	}
	
	p_mainWindow->UpdateActivityWindow("Ticker array: ");
	for (i = 0; i < tickerLen; ++i)
	{
		p_mainWindow->UpdateActivityWindow(CString(pInfo->pTickerArray[i].pData, pInfo->pTickerArray[i].iDataLen).GetBuffer());
	}    */ 
	
	RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::Strategy(StrategyInfo *pInfo, void *pContext, int *aiCode)
{
	/*int legLen = pInfo->iLegArrayLen,
		i;

	CString str;

	p_mainWindow->UpdateActivityWindow("Leg array: ");
	for (i = 0; i < legLen; ++i)
	{
		str.Format("iRatio = %d, Exchange: %s, InstrumentType: %s, ProductCode: %s, Ticker: %s", pInfo->asLegArray[i].iRatio, pInfo->asLegArray[i].sExchange.pData, pInfo->asLegArray[i].sInstrumentType.pData,
			pInfo->asLegArray[i].sProductCode.pData, pInfo->asLegArray[i].sTicker.pData);
		p_mainWindow->UpdateActivityWindow(str.GetBuffer());
	}  */

    RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::PasswordChange(PasswordChangeInfo* pInfo, void* pContext, int* aiCode)
{
     RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::ExecutionReplay(ExecutionReplayInfo* pInfo, void* pContext, int* aiCode)
{
     RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::LineUpdate(LineInfo* pInfo, void* pContext, int* aiCode)
{
     RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::OpenOrderReplay(OrderReplayInfo* pInfo, void* pContext, int* aiCode)
{
     RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::OrderReplay(OrderReplayInfo* pInfo, void* pContext, int* aiCode)
{
     RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::PnlReplay(PnlReplayInfo* pInfo, void* pContext, int* aiCode)
{
     RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::PnlUpdate(PnlInfo* pInfo, void* pContext, int* aiCode)
{
     RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::BustReport(OrderBustReport * pReport,                            void *            pContext,                            int *             aiCode)
{
     RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::CancelReport(OrderCancelReport * pReport,                              void *              pContext,                              int *               aiCode)
{
     RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::FailureReport(OrderFailureReport * pReport,                               void *               pContext,                               int *                aiCode)
{
     RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::FillReport(OrderFillReport * pReport,                            void *            pContext,                            int *             aiCode)
{
     RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::ModifyReport(OrderModifyReport * pReport,                              void *              pContext,                              int *               aiCode)
{
     RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::NotCancelledReport(OrderNotCancelledReport * pReport,                                    void *                    pContext,                                    int *                     aiCode)
{
     RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::NotModifiedReport(OrderNotModifiedReport * pReport,                                   void *                   pContext,                                   int *                    aiCode)
{
     RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::RejectReport(OrderRejectReport * pReport,                              void *              pContext,                              int *               aiCode)
{
     RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::StatusReport(OrderStatusReport * pReport,                              void *              pContext,                              int *               aiCode)
{
     RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::TradeCorrectReport(OrderTradeCorrectReport * pReport,                                    void *                    pContext,                                    int *                     aiCode)
{
     RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::TriggerPulledReport(OrderTriggerPulledReport * pReport,                                     void *                     pContext,                                     int *                      aiCode)
{
     RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::TriggerReport(OrderTriggerReport * pReport,                               void *               pContext,                               int *                aiCode)
{
     RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::OtherReport(OrderReport * pReport,                             void *        pContext,                             int *         aiCode)
{
     RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::SodUpdate(SodReport * pReport,                           void *      pContext,                           int *       aiCode)
{
    RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::Quote(QuoteReport* pReport, void* pContext, int* aiCode)
{
    RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::EndQuote(EndQuoteInfo * pInfo,			  void *         pContext,			  int *          aiCode)
{
     RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::EquityOptionStrategyList(EquityOptionStrategyListInfo * pInfo,					  void *                         pContext,					  int *                          aiCode)
{
     RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::OptionList(OptionListInfo * pInfo,			    void *           pContext,			    int *            aiCode)
{
    RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::Bar(BarInfo* pInfo, void* pContext, int* aiCode)
{
     RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::BarReplay(BarReplayInfo* pInfo, void* pContext, int* aiCode)
{
    RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::ExchangeList(ExchangeListInfo* pInfo, void* pContext, int* aiCode)
{
	/*for (int i = pInfo->iArrayLen-1; i > 0; --i)
		pInfo->asExchangeArray[i].pData;*/
    
	RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::ClosingIndicator(ClosingIndicatorInfo* pInfo, void* pContext, int* aiCode)
{
     RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::OpeningIndicator(OpeningIndicatorInfo* pInfo,  void* pContext, int* aiCode)
{
     RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::BinaryContractList(BinaryContractListInfo* pInfo, void* pContext, int* aiCode)
{
	RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::InstrumentByUnderlying(InstrumentByUnderlyingInfo* pInfo, void* pContext, int* aiCode)
{
	RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::TradeRoute(TradeRouteInfo* pInfo, void* pContext, int* aiCode)
{
	RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::TradeRouteList(TradeRouteListInfo* pInfo, void* pContext, int* aiCode)
{
	RITHMIC_STUB(__FUNCTION__);
}

int RithmicCallbacks::SingleOrderReplay(SingleOrderReplayInfo* pInfo, void* pContext,  int* aiCode)
{
	RITHMIC_STUB(__FUNCTION__);
}