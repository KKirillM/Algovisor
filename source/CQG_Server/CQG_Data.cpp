#include "stdafx.h"
#include "CQG_Server.h"

extern HWND hwnMain;

HRESULT CQGServer::LogonStateChanged(LogonState logonState)
{
   m_LogonState = logonState;
 
   switch(m_LogonState) 
   {
   case LogonState_LoggingOn:
	   SendText(L"CQG: Loging on, please wait");
	   SendConnectionStatus(L"Trying to logon...", 1);
	   break;
   case LogonState_LoggedOn:
       SendText(L"CQG: Logon completed successfully.");
	   SendConnectionStatus(L"Connection status: UP", 2);
	   CreateCQGCEL();
	   break;
   case LogonState_Failed:
	   SendText(L"CQG: Logon failed. Try again.");
	   SendConnectionStatus(L"Connection status: DOWN", 0);
	   break;
   };

   return S_OK;
}

HRESULT CQGServer::ErrorReceived (BSTR message)
{
	CString errMsg = L"CQG: Error message: ";
    errMsg += ATL::CW2A(message);
    SendText((wchar_t*)errMsg.GetString());

    //if (!wcscmp(message, L"Invalid User Or Password"))
	SendConnectionStatus(L"Connection status: DOWN", 0);
	m_LogonState = LogonState_LoggedOff;

    return S_OK;
}

STDMETHODIMP CQGServer::OnDataConnectionStatusChanged(eConnectionStatus newStatus)
{
   try
   {
      CString status;
      switch (newStatus)
      {
      case csConnectionDelayed:
         status = _T("delayed");
		 SendConnectionStatus((wchar_t*)(L"Connection status: DELAYED" + status).GetString(), 1);
		 SendText((wchar_t*)(L"CQG: Connection status: " + status).GetString());
		 m_LogonState = LogonState::LogonState_Failed;
         break;

      case csConnectionDown:
         status = _T("DOWN");
		 SendConnectionStatus((wchar_t*)(L"Connection status: " + status).GetString(), 0);
		 SendText((wchar_t*)(L"CQG: Connection status: " + status).GetString());
		 m_LogonState = LogonState::LogonState_Failed;
         break;

      case csConnectionUp:
         status = _T("UP");
		 SendConnectionStatus((wchar_t*)(L"Connection status: " + status).GetString(), 2);
		 SendText((wchar_t*)(L"CQG: Connection status: " + status).GetString());
		 m_LogonState = LogonState::LogonState_LoggingOn;
         break;

      default:
         status = _T("unknown");
		 SendConnectionStatus((wchar_t*)(L"Connection status: ?" + status).GetString(), 0);
		  SendText((wchar_t*)(L"CQG: Connection status: " + status).GetString());
		 m_LogonState = LogonState::LogonState_Failed;
         break;
      }
   } 
   catch (std::runtime_error& e) 
   {
	   CString str(e.what());
       AfxMessageBox(str);
       return E_FAIL;
   }
   return S_OK;
}

STDMETHODIMP CQGServer::OnCELStarted()
{
   return S_OK;
}

STDMETHODIMP CQGServer::OnCELHostErrorReceived(BSTR message )
{
   CString errMsg("CELHost error: ");
   errMsg += ATL::CW2A(message);
   SendText((wchar_t*)errMsg.GetString());
   
   return S_OK;
}

STDMETHODIMP CQGServer::OnDataError(LPDISPATCH obj, BSTR errorDescription)
{
   CString errMsg(_T("Data error: "));
   errMsg += ATL::CW2T(errorDescription);
   SendText((wchar_t*)errMsg.GetString());

   return S_OK;
}

STDMETHODIMP CQGServer::OnLineTimeChanged(DATE new_line_time)
{
	//CString time(TsDateTimeToString(new_line_time,false,true,true));
	
	return S_OK;
}

STDMETHODIMP CQGServer::OnIncorrectSymbol(BSTR symbl)
{
	CString str;
	str.Format(L"CQG: Invalid symbol: %s", symbl);
	SendText((wchar_t*)str.GetString());

    return S_OK;
}

STDMETHODIMP CQGServer::OnInstrumentSubscribed(BSTR symbol, ICQGInstrument* instrument)
{
	//отсылаем главному приложению свойства по данному инструменту (дата экспирации, шаг цены и др.)
	try
	{
		INSTRUMENT_PROPERTIES ip;

		//ID
		HRESULT hr = instrument->get_InstrumentID(&ip.instrumentID);
		AssertCOMError<ICQGInstrument>(instrument, hr);

		//ticker
		StringCchCopy(ip.ticker, 100, symbol);

		//fullname
		//ATL::CComBSTR fullName;
		//hr = instrument->get_FullName(&fullName);
		//AssertCOMError<ICQGInstrument>(instrument, hr);
		//StringCchCopy(ip.fullName, 100, fullName);

		//описание инструмента
		ATL::CComBSTR instrDescr;
		hr = instrument->get_Description(&instrDescr);
		AssertCOMError<ICQGInstrument>(instrument, hr);
		StringCchCopy(ip.description, 100, instrDescr);

		//дата экспирации
		DATE exp;
		hr = instrument->get_ExpirationDate(&exp);
		AssertCOMError<ICQGInstrument>(instrument, hr);
		StringCchCopy(ip.expirationDate, 100, TsDateTimeToString(exp, true, false));

		//minTickInc and precision
		double tickSize;
		hr = instrument->get_TickSize(&tickSize);
		AssertCOMError<ICQGInstrument>(instrument, hr);

		bool intValue = false;
		ip.precision = 0;
		while (1)
		{
			tickSize *= 10;
			++ip.precision;

			if ((int)tickSize)
				intValue = true;

			if (intValue && !((int)tickSize % 10))
			{
				tickSize /= 10;
				ip.MinTickInc = (int)tickSize;
				--ip.precision;
				break;
			}			
		}	
		
		//BSTR tSize[50];
		//instrument->ToDisplayPrice(tickSize, tSize);
		//ip.MinTickInc = ::_wtoi(tSize[0]);

		/*ip.precision = 0;
		while (ip.MinTickInc != (int)tickSize && ip.MinTickInc > (int)tickSize)
		{
			tickSize *= 10;
			++ip.precision;
		}	*/	
		
		//биржа
		hr = instrument->get_ExchangeID(&ip.exchID);
		AssertCOMError<ICQGInstrument>(instrument, hr);

		//session mask
		//hr = instrument->get_SessionMask((long*)&ip.sessionMask);
		//AssertCOMError<ICQGInstrument>(instrument, hr); 

		/*ATL::CComPtr<ICQGInstrumentProperties> spProperties;
		instrument->get_Properties(&spProperties);	
		ATL::CComPtr<ICQGInstrumentProperty> spProperty;
		spProperties->get_Item(eInstrumentProperty::ipExchangeID, &spProperty);
		ATL::CComVariant propValue;
		spProperty->get_Value(&propValue);*/

		//BSTR exch[50];
		//hr = instrument->get_ExchangeAbbreviation(exch);
		//AssertCOMError<ICQGInstrument>(instrument, hr);
		//StringCchCopy(ip.exchange, 100, instrDescr);

		//market status
		/*ATL::CComPtr<ICQGSessions> spSessions;
		hr = instrument->get_Sessions(&spSessions);
		AssertCOMError<ICQGInstrument>(instrument, hr);
		
		DATE sessionEnd;
		hr = spSessions->get_DescriptionEnd(&sessionEnd);
		AssertCOMError<ICQGSessions>(spSessions, hr);
		CString sessEnd = TsDateTimeToString(sessionEnd, true, true, true);

		long descrNum;
		hr = spSessions->get_DescriptionNumber(&descrNum);
		AssertCOMError<ICQGSessions>(spSessions, hr);

		long sessionCount;
		hr = spSessions->get_Count(&sessionCount);
		AssertCOMError<ICQGSessions>(spSessions, hr);

		BSTR sessionName[50];
		hr = spSessions->get_Name(sessionName);
		AssertCOMError<ICQGSessions>(spSessions, hr);*/

		//глубина подписки
		//eQuoteSubscriptionLevel qLvl;
		//hr = instrument->get_QuoteLevel(&qLvl);
		//AssertCOMError<ICQGInstrument>(instrument, hr);

		VARIANT_BOOL enableDOM;
		hr = instrument->get_DOMStatus(&enableDOM);
		AssertCOMError<ICQGInstrument>(instrument, hr);

		if (enableDOM == VARIANT_TRUE)
			ip.type = 3;
		else 
			ip.type = 1;

		SendInstrumentProperties(ip);
	}
	catch (std::runtime_error& e) 
	{
		TCHAR str[64];
		StringCchPrintf(str, 64, L"%s", e.what());
		SendText(str);
		return E_FAIL;
	}

   return S_OK;
}

STDMETHODIMP CQGServer::OnInstrumentChanged(ICQGInstrument* instrument, ICQGQuotes* quotes, ICQGInstrumentProperties* props)
{
	try
	{
		//ATL::CComBSTR fullname;
		//HRESULT hr = instrument->get_FullName(&fullname);
		//AssertCOMError<ICQGInstrument>(instrument, hr);
		
		ATL::CComPtr<ICQGQuote> spQuote;

		//трейд
		HRESULT hr = quotes->get_Item(eQuoteType::qtTrade, &spQuote);
		if (hr == S_OK)
		{
			CQG_INSTRUMENT_TRADE it;		
			
			//StringCchCopyW(it.fullName, 100, fullname);
			
			//ID
			hr = instrument->get_InstrumentID(&it.instrumentID);
			AssertCOMError<ICQGInstrument>(instrument, hr);

			::GetLocalTime(&it.timestamp);

			//DATE timestamp;
			//hr = spQuote->get_Timestamp(&timestamp);
			//hr = spQuote->get_ServerTimestamp(&timestamp);
			//AssertCOMError(spQuote, hr);
			//VariantTimeToSystemTime(timestamp, &it.timestamp);
			
			hr = spQuote->get_Price(&it.price);
			AssertCOMError(spQuote, hr);
			hr = spQuote->get_Volume(&it.volume);
			AssertCOMError(spQuote, hr);

			//itVec.push_back(it);
			//::PostMessageW(hwnMain, WM_APP+1, 0, 0);
			SendInstrumentTrade(it);

			spQuote.Release();
			

			//FILE* f;
			//if (fopen_s(&f, "time&salse.txt", "a+"))
			//{
			//	SendText(L"Error create T&S file");
			//}
			////setbuf(f, NULL);
			//fprintf(f, "%d:%d:%d.%d  %f  %d\n", it.timestamp.wHour, it.timestamp.wMinute, it.timestamp.wSecond, it.timestamp.wMilliseconds, 
			//	it.price, it.volume);
			//fclose(f);
		}
		
		//лучший офер
		hr = quotes->get_Item(eQuoteType::qtAsk, &spQuote);
		if (hr == S_OK)
		{
			CQG_INSTRUMENT_BBO_ASK iBBO;

			//StringCchCopyW(iBBO.fullName, 100, fullname);

			//ID
			hr = instrument->get_InstrumentID(&iBBO.instrumentID);
			AssertCOMError<ICQGInstrument>(instrument, hr);

			//DATE timestamp;
			//hr = spQuote->get_Timestamp(&timestamp);
			//hr = spQuote->get_ServerTimestamp(&timestamp);
           // AssertCOMError(spQuote, hr);
			//VariantTimeToSystemTime(timestamp, &iBBO.timestamp);

			hr = spQuote->get_Price(&iBBO.price);
            AssertCOMError(spQuote, hr);

			//hr = spQuote->get_Volume(&iBBO.size);
			//AssertCOMError(spQuote, hr);

			SendInstrumentAsk(iBBO);

			spQuote.Release();
		}

		//лучший бид
		hr = quotes->get_Item(eQuoteType::qtBid, &spQuote);
		if (hr == S_OK)
		{
			CQG_INSTRUMENT_BBO_BID iBBO;

			//StringCchCopyW(iBBO.fullName, 100, fullname);

			//ID
			hr = instrument->get_InstrumentID(&iBBO.instrumentID);
			AssertCOMError<ICQGInstrument>(instrument, hr);

			//DATE timestamp;
			//hr = spQuote->get_Timestamp(&timestamp);
			//hr = spQuote->get_ServerTimestamp(&timestamp);
            //AssertCOMError(spQuote, hr);
			//VariantTimeToSystemTime(timestamp, &iBBO.timestamp);

			hr = spQuote->get_Price(&iBBO.price);
            AssertCOMError(spQuote, hr);

			//hr = spQuote->get_Volume(&iBBO.size);
			//AssertCOMError(spQuote, hr);

			SendInstrumentBid(iBBO);

			spQuote.Release();
		}		
	}
	catch (std::runtime_error& e) 
	{
		TCHAR str[64];
		StringCchPrintf(str, 64, L"%s", e.what());
		SendText(str);
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP CQGServer::OnInstrumentDOMChanged(ICQGInstrument* instrument, ICQGDOMQuotes* prevAsks, ICQGDOMQuotes* prevBids)
{
	try
	{
		CQG_INSTRUMENT_DOM iDOM;
		
		for (int i = 0; i < 11; ++i)
			iDOM.askSize[i] = iDOM.bidSize[i] = 0;
		iDOM.domLevelCount = 0;

		::GetLocalTime(&iDOM.timestamp);

		//тикер
		//ATL::CComBSTR fullname;
		//HRESULT hr = instrument->get_FullName(&fullname);
		//AssertCOMError<ICQGInstrument>(instrument, hr);

		//ID
		HRESULT hr = instrument->get_InstrumentID(&iDOM.instrumentID);
		AssertCOMError<ICQGInstrument>(instrument, hr);

		//StringCchCopyW(iDOM.fullName, 100, fullname);

		//аски
		//ATL::CComPtr<ICQGDOMQuotes> quotes;
		//hr = instrument->get_DOMAsks(&quotes);
		//AssertCOMError<ICQGInstrument>(instrument, hr);

		//количество уровней на аске
		//hr = quotes->get_Count(&iDOM.domLevelCount);
		//ATL::CComPtr<ICQGDOMQuotes> qAsks;
		//qAsks.Attach(prevAsks);
		hr = prevAsks->get_Count(&iDOM.domLevelCount);
		if (iDOM.domLevelCount > 10) 
			iDOM.domLevelCount = 10;
		else if (iDOM.domLevelCount < 5)
			iDOM.domLevelCount = 0;
		//AssertCOMError(qAsks, hr);

		ATL::CComPtr<ICQGQuote> spAsk;
		VARIANT_BOOL isValid;

		DATE timestamp_last = 0;

		for (int i = 0; i < iDOM.domLevelCount; ++i)
		{
			//hr = quotes->get_Item(i, &spAsk);
			//AssertCOMError(quotes, hr);
			hr = prevAsks->get_Item(i, &spAsk);
			//AssertCOMError(qAsks, hr);

			hr = spAsk->get_IsValid(&isValid);
			AssertCOMError(spAsk, hr);

			if (isValid == VARIANT_TRUE)
			{
				//DATE timestamp;
				//hr = spAsk->get_Timestamp(&timestamp);
				//hr = spAsk->get_ServerTimestamp(&timestamp);
				//AssertCOMError(spAsk, hr);
				//VariantTimeToSystemTime(timestamp_last, &iDOM.timestamp);

				hr = spAsk->get_Price(&iDOM.askPrice[i]);
				AssertCOMError(spAsk, hr);

				hr = spAsk->get_Volume(&iDOM.askSize[i]);
				AssertCOMError(spAsk, hr);
			}

			spAsk.Release();
		}

		//qAsks.Release();

		//quotes.Release();

		//hr = instrument->get_DOMBids(&quotes);
		//AssertCOMError(quotes, hr);

		
		long bidsCount;
		//ATL::CComPtr<ICQGDOMQuotes> qBids;
		//qBids.Attach(prevBids);
		hr = prevBids->get_Count(&bidsCount);
		if (bidsCount > 10) 
			bidsCount = 10;
		//AssertCOMError(qBids, hr);

		ATL::CComPtr<ICQGQuote> spBid;

		for (int i = 0; i < bidsCount; ++i)
		{
			//hr = quotes->get_Item(i, &spBid);
			//AssertCOMError(quotes, hr);
			hr = prevBids->get_Item(i, &spBid);
			//AssertCOMError(prevBids, hr);

			hr = spBid->get_IsValid(&isValid);
			AssertCOMError(spAsk, hr);

			if (isValid == VARIANT_TRUE)
			{
				//DATE timestamp;
				//hr = spBid->get_Timestamp(&timestamp);
				//hr = spBid->get_ServerTimestamp(&timestamp);
				//AssertCOMError(spBid, hr);
				//VariantTimeToSystemTime(timestamp_last, &iDOM.timestamp);

				hr = spBid->get_Price(&iDOM.bidPrice[i]);
				AssertCOMError(spBid, hr);

				hr = spBid->get_Volume(&iDOM.bidSize[i]);
				AssertCOMError(spBid, hr);
			}

			spBid.Release();
		}

		//qBids.Release();

		SendInstrumentDOM(iDOM);
	}
	catch (std::runtime_error& e) 
	{
		TCHAR str[64];
		StringCchPrintf(str, 64, L"%s", e.what());
		SendText(str);
		return E_FAIL;
	}
   
	return S_OK;
}