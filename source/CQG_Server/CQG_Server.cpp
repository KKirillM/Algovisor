#include "stdafx.h"
#include "CQG_Server.h"

class CTEMPORALModule : public ATL::CComModule
{
};

namespace 
{
   // This instance is auxiliary. It will initialize all COM required
   // modules in its constructor.
   CTEMPORALModule atlTemp;
}

CString TsDateTimeToString(DATE tsDateTime, 
								bool withDate/* =true*/,
								bool withTime/* =true*/, 
								bool withSeconds/* =false */)
{
  // using std::setw;

   SYSTEMTIME stDateTime;
   VariantTimeToSystemTime(tsDateTime, &stDateTime);

   CString timeString;
   CString dateTimeString;
   if (withDate)
   {
      dateTimeString.Format(L"%.2d/%.2d/%.2d", stDateTime.wMonth, stDateTime.wDay, stDateTime.wYear);
   }

   if (withTime)
   {
      timeString.Format(L"%.2d:%.2d", stDateTime.wHour, stDateTime.wMinute);
      dateTimeString += withDate ? _T(" ") : _T("");
      dateTimeString += timeString;
   }

   if (withSeconds)
   {
	   timeString.Format(L":%.2d", stDateTime.wSecond);
      dateTimeString += timeString;
   }

   return dateTimeString;
}

CString VariantToString(const ATL::CComVariant& value)
{
   CString strValue;
   switch (value.vt)
   {
      case VT_I4:
         strValue.Format(L"%d", value.intVal);
   	   break;
      case VT_UI4:
         strValue.Format(L"%d", value.uintVal);
         break;
      case VT_R8:
         strValue.Format(L"%f", value.dblVal);
         break;
      case VT_DATE:
         strValue = TsDateTimeToString(value.date);
   	   break;
      case VT_BSTR:
         strValue = ATL::CW2T(value.bstrVal);
         break;
      case VT_BOOL:
         strValue = (value.boolVal == VARIANT_TRUE) ? _T("True") : _T("False");
         break;
      default:
         throw std::runtime_error(std::string("Unknown type"));
   }

   return strValue;
}

CQGServer::CQGServer()
{
	m_LogonState = LogonState_LoggedOff;
}

CQGServer::~CQGServer()
{
	SendText(L"CQG: Connection DOWN.");
	SendConnectionStatus(L"Connection status: DOWN", 0);
}

 void CQGServer::Connect(wchar_t* login, wchar_t* password, bool demo, GUID guid)
 {
	HRESULT hr;
	
	if (!m_celHost.p || FAILED(m_celHost->get_LogonState(&m_LogonState)) || m_LogonState != LogonState_LoggedOn)
	{	
		if (!m_celHost.p)
		{
			hr = m_celHost.CoCreateInstance(/*CLSID_CELHost*/guid, NULL, CLSCTX_INPROC_SERVER);
			if (FAILED(hr))
			{
				SendText(L"CQG: Unable to create CELHost.");
				SendConnectionStatus(L"Connection status: DOWN", 0);
				return;
			}
		}

		if (ICQGCELHostEventImpl::m_dwEventCookie == 0xFEFEFEFE)
		{
			hr = ICQGCELHostEventImpl::DispEventAdvise(m_celHost);
			if (FAILED(hr))
			{
				SendText(L"CQG: Unable to Advise to CELHost events.");
				SendConnectionStatus(L"Connection status: DOWN", 0);
				return;
			}
		}

		_bstr_t bstr_login(login), bstr_password(password);
		m_celHost->Logon(bstr_login, bstr_password, demo);
	}
	else if (m_LogonState == LogonState_LoggedOn)
	{
		SendText(L"CQG: Logon completed successfully.");
		SendConnectionStatus(L"Connection status: UP", 2);
		CreateCQGCEL();
	}
 }

 void CQGServer::CreateCQGCEL()
 {
	 HRESULT hr;

	 if (!m_spCQGCEL.p)
	 {
		 m_spCQGCEL = m_celHost->CreateCQGCEL();
		 ASSERT(m_spCQGCEL);
	 }

	 try
	 {
		 VARIANT_BOOL celStarted;
		 hr = m_spCQGCEL->get_IsStarted(&celStarted);
		 AssertCOMError<ICQGCEL>(m_spCQGCEL, hr);
		
		 if (celStarted)
		 {
			 hr = ICQGCELDispEventImpl::DispEventUnadvise(m_spCQGCEL);
			 AssertCOMError<ICQGCEL>(m_spCQGCEL, hr);
			 
			 hr = m_spCQGCEL->Shutdown();
			 AssertCOMError<ICQGCEL>(m_spCQGCEL, hr);
		 }

		// Change configuration in order not to check Ready status,
		// and let CQCCEL know that it is used from ATL client.
		ATL::CComPtr<ICQGAPIConfig> spConf;
		hr = m_spCQGCEL->get_APIConfiguration(&spConf);
		AssertCOMError<ICQGCEL>(m_spCQGCEL, hr);

		hr = spConf->put_NewInstrumentMode(VARIANT_TRUE);
		AssertCOMError<ICQGAPIConfig>(spConf, hr);

		hr = spConf->put_ReadyStatusCheck(rscOff);	//здесь true ставится только когда для экселя запускаем
		AssertCOMError<ICQGAPIConfig>(spConf, hr);

		hr = spConf->put_UsedFromATLClient(VARIANT_TRUE);
		AssertCOMError<ICQGAPIConfig>(spConf, hr);

		hr = spConf->put_CollectionsThrowException(VARIANT_FALSE);
		AssertCOMError<ICQGAPIConfig>(spConf, hr);

		hr = spConf->put_TimeZoneCode(eTimeZone::tzUndefined);
		AssertCOMError<ICQGAPIConfig>(spConf, hr);

		//hr = spConf->put_DefaultInstrumentSubscriptionLevel(eDataSubscriptionLevel::dsNone);
		//AssertCOMError<ICQGAPIConfig>(spConf, hr);

		//hr = spConf->put_DOMUpdatesMode(eDOMUpdatesMode::domUMDynamic);
		//AssertCOMError<ICQGAPIConfig>(spConf, hr);

		//hr = spConf->put_NonsynchronizedData(VARIANT_FALSE);
		//AssertCOMError<ICQGAPIConfig>(spConf, hr);

		//hr = spConf->put_LogSeverity(eLogSeverity::);
		//AssertCOMError<ICQGAPIConfig>(spConf, hr);

		// Now advise the connection, to get events
		if (ICQGCELDispEventImpl::m_dwEventCookie == 0xFEFEFEFE)
		{
			hr = ICQGCELDispEventImpl::DispEventAdvise(m_spCQGCEL);
			ATLASSERT(SUCCEEDED(hr));
		}

		hr = m_spCQGCEL->Startup();
		AssertCOMError<ICQGCEL>(m_spCQGCEL, hr);

		//hr = m_spCQGCEL->RequestTradableExchanges(0); //какие биржи есть, но для этого ивент нужен
		//AssertCOMError<ICQGCEL>(m_spCQGCEL, hr);

		//версия АПИ
		ATL::CComPtr<ICQGEnvironment> spEnvp;
		hr = m_spCQGCEL->get_Environment(&spEnvp);
		AssertCOMError<ICQGCEL>(m_spCQGCEL, hr);

		_bstr_t verNum;
		/*hr = spEnvp->get_CQGVersion(verNum.GetAddress());
		AssertCOMError<ICQGEnvironment>(spEnvp, hr);
		SendText((L"CQG: CQGIC version: " + verNum).GetBSTR());*/

		hr = spEnvp->get_CELVersion(verNum.GetAddress());
		AssertCOMError<ICQGEnvironment>(spEnvp, hr);
		SendText((L"CQG API version: " + verNum).GetBSTR());
	}    
	catch (std::exception& e) 
	{
		TCHAR str[64];
		StringCchPrintf(str, 64, L"%s", e.what());
		SendText(str);
	}
 }

 void CQGServer::Disconnect()
 {
	 HRESULT hr;

	 UnsubscribeAll();

	 if (m_spCQGCEL.p)
	 {
		 VARIANT_BOOL celStarted;
		 m_spCQGCEL->get_IsStarted(&celStarted);
		
		 if (celStarted)
		 {
			 try
			 {
				 hr = ICQGCELDispEventImpl::DispEventUnadvise(m_spCQGCEL);
				 AssertCOMError<ICQGCEL>(m_spCQGCEL, hr);

				 hr = m_spCQGCEL->Shutdown();
				 AssertCOMError<ICQGCEL>(m_spCQGCEL, hr);
			 }
			catch (std::exception& e) 
			{
				 TCHAR str[64];
				 StringCchPrintf(str, 64, L"%s", e.what());
				 SendText(str);
			}
		 }
	 }

	 if (m_celHost.p)
	 {
		 hr = m_celHost->Logoff();
		 ICQGCELHostEventImpl::DispEventUnadvise(m_celHost);
	 }
	  
	 SendText(L"CQG: Connection DOWN.");
	 SendConnectionStatus(L"Connection status: DOWN", 0);
 }

 void CQGServer::Subscribe(wchar_t* ticker, bool dom)
 {
	 if (m_LogonState == LogonState_LoggedOn || m_LogonState == LogonState_LoggingOn)
	 {
		 try
		 {
			ATL::CComPtr<ICQGInstrumentSubscriptionRequest> param;
		
			HRESULT hr = m_spCQGCEL->CreateInstrumentRequest(&param);
			AssertCOMError<ICQGCEL>(m_spCQGCEL, hr);

			param->put_DOMBBAType(eDOMandBBAType::/*dbtImplied*//*dbtOutright*/dbtCombined);
			if (dom)
				param->put_DOMStatus(VARIANT_TRUE);
			else param->put_DOMStatus(VARIANT_FALSE);
			param->put_PropertyLevel(ePropertySubscriptionLevel::psNone);
			param->put_QuoteLevel((eQuoteSubscriptionLevel)3); //eQuoteSubscriptionLevel::qsQuotes | eQuoteSubscriptionLevel::qsBBA
			param->put_Symbol(CW2W(ticker));

			hr = m_spCQGCEL->SubscribeNewInstrument(param);
			//или hr = m_spCQGCEL->NewInstrument(CW2W(ticker));
			AssertCOMError(m_spCQGCEL, hr);
		}
		catch (std::runtime_error& e) 
		{
			TCHAR str[64];
			StringCchPrintf(str, 64, L"%s", e.what());
			SendText(str);
		}
	 }
	 else
	 {
		 SendText(L"CQG: Connection DOWN.");
		 SendConnectionStatus(L"Connection status: DOWN", 0);
	 }
 }
 void CQGServer::Unsubscribe(wchar_t* id/*symbol*/)
 {
	  if (m_LogonState == LogonState_LoggedOn || m_LogonState == LogonState_LoggingOn)
	  {
		 try
		 {
			 // Get instruments collection
			 ATL::CComPtr<ICQGInstruments> spInstruments;
			 HRESULT hr = m_spCQGCEL->get_Instruments(&spInstruments);
			 AssertCOMError(m_spCQGCEL, hr);

			 // Get instrument from collection by it's full name
			 //ATL::CComBSTR fullname = ATL::CT2W(symbol);
			 long id_long = _wtoi(id);
			 ATL::CComVariant idx(id_long);
			 ATL::CComPtr<ICQGInstrument> spInstrument;
			 hr = spInstruments->get_Item(idx, &spInstrument);
			 AssertCOMError(spInstruments, hr);
			 if (!spInstrument.p)
				 return;

			 BSTR fullname;
			 hr = spInstrument->get_FullName(&fullname);
			 wchar_t text[100];
			 StringCchPrintfW(text, 100, L"CQG: Instrument unsubscribed: %s", fullname);
			 SendText(text);

			 hr = m_spCQGCEL->RemoveInstrument(spInstrument.p);
			 AssertCOMError(m_spCQGCEL, hr);
		}
		catch (std::runtime_error& e) 
		{
			TCHAR str[64];
			StringCchPrintf(str, 64, L"%s", e.what());
			SendText(str);
		}
	 }
	 else
	 {
		 SendText(L"CQG: Connection DOWN.");
		 SendConnectionStatus(L"Connection status: DOWN", 0);
	 }
 }
 void CQGServer::UnsubscribeAll()
 {
	  if (m_LogonState == LogonState_LoggedOn || m_LogonState == LogonState_LoggingOn)
	  {
		 try
		 {
			 HRESULT hr = m_spCQGCEL->RemoveAllInstruments();
			 AssertCOMError(m_spCQGCEL, hr);
			 SendText(L"CQG: Unsubscribe all instruments");
		}
		catch (std::runtime_error& e) 
		{
			TCHAR str[64];
			StringCchPrintf(str, 64, L"%s", e.what());
			SendText(str);
		}
	 }
	 else
	 {
		 SendText(L"CQG: Connection DOWN.");
		 SendConnectionStatus(L"Connection status: DOWN", 0);
	 }
 }

 void CQGServer::SendText(wchar_t* str)
 {
	TEXT txt;
	StringCchCopy(txt.text, 100, str);
	
	HWND hwn = ::FindWindow(NULL, AlgovisorStr);
	if (!hwn)
		::PostQuitMessage(0);

	COPYDATASTRUCT cdStruct;

	cdStruct.dwData = CQG_TEXT;				// function identifier
	cdStruct.cbData = sizeof(TEXT);			// size of data
	cdStruct.lpData = &txt;					// data structure

	::SendMessage(hwn, WM_COPYDATA, NULL, (LPARAM)(LPVOID)&cdStruct);
 }
 void CQGServer::SendConnectionStatus(wchar_t* str, UINT status)
 {
	CONNECTIONSTATUS st;
	StringCchCopy(st.text, 100, str);
	st.connectionStatus = status;
	
	HWND hwn = ::FindWindow(NULL, AlgovisorStr);
	if (!hwn)
		::PostQuitMessage(0);

	COPYDATASTRUCT cdStruct;

	cdStruct.dwData = CQG_CONNECTION_STATUS;			// function identifier
	cdStruct.cbData = sizeof(CONNECTIONSTATUS);			// size of data
	cdStruct.lpData = &st;								// data structure

	::SendMessage(hwn, WM_COPYDATA, NULL, (LPARAM)(LPVOID)&cdStruct);
 }
 void CQGServer::SendInstrumentProperties(INSTRUMENT_PROPERTIES& ip)
 {
	 HWND hwn = ::FindWindow(NULL, AlgovisorStr);
	 if (!hwn)
		::PostQuitMessage(0);

	 COPYDATASTRUCT cdStruct;

	 cdStruct.dwData = CQGServer::CQG_INSTRUMENT_PROPERTIES;      // function identifier
	 cdStruct.cbData = sizeof(INSTRUMENT_PROPERTIES);			// size of data
	 cdStruct.lpData = &ip;							// data structure

	 ::SendMessage(hwn, WM_COPYDATA, NULL, (LPARAM)(LPVOID)&cdStruct);
 }

 void CQGServer::SendInstrumentTrade(CQG_INSTRUMENT_TRADE& it)
 {
	 HWND hwn = ::FindWindow(NULL, AlgovisorStr);
	 if (!hwn)
		::PostQuitMessage(0);

	 COPYDATASTRUCT cdStruct = { CQGServer::CQG_TRADE, sizeof(CQG_INSTRUMENT_TRADE), &it };
	 ::SendMessage(hwn, WM_COPYDATA, NULL, (LPARAM)(LPVOID)&cdStruct);
 }

 void CQGServer::SendInstrumentAsk(CQG_INSTRUMENT_BBO_ASK& iBBO)
 {
	 HWND hwn = ::FindWindow(NULL, AlgovisorStr);
	 if (!hwn)
		::PostQuitMessage(0);

	 COPYDATASTRUCT cdStruct = { CQGServer::CQG_ASK, sizeof(CQG_INSTRUMENT_BBO_ASK), &iBBO };
	 ::SendMessage(hwn, WM_COPYDATA, NULL, (LPARAM)(LPVOID)&cdStruct);
 }
	
 void CQGServer::SendInstrumentBid(CQG_INSTRUMENT_BBO_BID& iBBO)
 {
	 HWND hwn = ::FindWindow(NULL, AlgovisorStr);
	 if (!hwn)
		::PostQuitMessage(0);

	 COPYDATASTRUCT cdStruct = { CQGServer::CQG_BID, sizeof(CQG_INSTRUMENT_BBO_BID), &iBBO };
	 ::SendMessage(hwn, WM_COPYDATA, NULL, (LPARAM)(LPVOID)&cdStruct);
 }

 void CQGServer::SendInstrumentDOM(CQG_INSTRUMENT_DOM& iDOM)
 {
	  HWND hwn = ::FindWindow(NULL, AlgovisorStr);
	 if (!hwn)
		::PostQuitMessage(0);

	 COPYDATASTRUCT cdStruct = { CQGServer::CQG_DOM, sizeof(CQG_INSTRUMENT_DOM), &iDOM };
	 ::SendMessage(hwn, WM_COPYDATA, NULL, (LPARAM)(LPVOID)&cdStruct);
 }