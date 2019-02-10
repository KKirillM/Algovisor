#pragma once

class CQGServer;
extern CQGServer cqg;

const wchar_t* const AlgovisorStr = L"Algovisor";

struct LOGINDATA
{
	wchar_t login[100];
	wchar_t password[100];
	bool demo;
	GUID guID;
};
struct TEXT
{
	wchar_t text[100];
};
struct CONNECTIONSTATUS
{
	BYTE connectionStatus;
	wchar_t text[100];	
};
struct SUBSCRIBE
{
	wchar_t ticker[100];
	bool dom;
};
struct INSTRUMENT_PROPERTIES
{
	wchar_t ticker[100];
	//wchar_t fullName[100];
	long instrumentID;
	wchar_t description[100];
	//wchar_t exchange[100];
	wchar_t expirationDate[100];
	int type;
	long exchID;
	int MinTickInc;						//минимальный шаг цены по инструменту
	int precision;						//кол-во значащих знаков после запятой
	//int sessionMask;
};
struct CQG_INSTRUMENT_TRADE
{
	//wchar_t fullName[100];
	long instrumentID;
	SYSTEMTIME timestamp;
	double price;
	long volume;
};
struct CQG_INSTRUMENT_BBO_ASK
{
	//wchar_t fullName[100];
	long instrumentID;
	SYSTEMTIME timestamp;
	double price;
	long size;
};
struct CQG_INSTRUMENT_BBO_BID
{
	//wchar_t fullName[100];
	long instrumentID;
	SYSTEMTIME timestamp;
	double price;
	long size;
};
struct CQG_INSTRUMENT_DOM
{
	//wchar_t fullName[100];
	long instrumentID;
	SYSTEMTIME timestamp;
	double bidPrice[11];
	double askPrice[11];
	long bidSize[11];
	long askSize[11];
	long domLevelCount;
};

/// \brief Returns latest COM error description
/// \param spClass [in,out] - Pointer to COM object which raised error
/// \param descr [out] - Error description
/// \return Result code
template <class IFaceT>
STDMETHODIMP GetErrorString(const ATL::CComPtr<IFaceT>& spClass, ATL::CComBSTR& descr)
{
   HRESULT hr = S_OK;

   ATL::CComPtr<ISupportErrorInfo> spSupportErrorInfo;
   hr = spClass->QueryInterface(__uuidof(ISupportErrorInfo), (void** )&spSupportErrorInfo);

   if (SUCCEEDED(hr))
   {
      hr = spSupportErrorInfo->InterfaceSupportsErrorInfo(__uuidof(IFaceT));
      if (hr == S_OK)
      {
         ATL::CComPtr<IErrorInfo> spErrorInfo;
         hr = GetErrorInfo(0, &spErrorInfo);

         if (hr == S_OK)
         {
            spErrorInfo->GetDescription(&descr);
         }
      }
   }
   return hr;
}

/// \brief Throws runtime exception depending on passed result code
/// \param spIFace - Pointer to object which is the source result code
/// \param hr - Result code 
template <class IFaceT>
void AssertCOMError(const ATL::CComPtr<IFaceT>& spIFace, HRESULT hr)
{
   if (FAILED(hr)) 
   {
      HRESULT hResult;
      ATL::CComBSTR errMessage;

      ATL::CComBSTR errDescCOM;
      hResult = GetErrorString(spIFace, errDescCOM);
      if (hResult == S_OK)
      {
         errMessage.Append(L"COM error occurred. Description: ");
         errMessage.Append(errDescCOM);
		 cqg.SendText(errMessage.m_str);
      }
      else
      {
         char* errDesc = NULL;
         FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            hr,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&errDesc,
            0,
            NULL);
         errMessage.Append(errDesc);
		 cqg.SendText(errMessage.m_str);
         LocalFree(errDesc);
      }
      throw std::runtime_error(std::string(ATL::CW2A(errMessage.m_str)));
   }
}

/// \brief Converts raw date format to string
/// \param tsTime - Raw date
/// \param withTime - Add time to string
/// \param withDate - Add date to string
/// \return withDate - Formated string of date and time
CString TsDateTimeToString(DATE tsDateTime, bool withDate = true, bool withTime = true, bool withSeconds = false);

/// \brief Converts variant value to string
/// \param value - Variant value
/// \return withDate - String representation of the value
CString VariantToString(const ATL::CComVariant& value);

typedef ATL::IDispEventImpl<1, 
                              CQGServer,
                              &__uuidof(_ICQGCELEvents),
                              &__uuidof(__CQG), 4, 0>
                            ICQGCELDispEventImpl;

typedef ATL::IDispEventImpl<1,
                              CQGServer, // Implementation of event handler.
                              &__uuidof(ICELHostEvent), // ID of interface that provides events
                              &__uuidof(__CQG_Trader_Services_CEL_Host_ActiveX), // CELHost.ActiveX library ID
                              1, 0> // CELHost.ActiveX library version
                            ICQGCELHostEventImpl;

// CQGServer class

class CQGServer : public ICQGCELDispEventImpl, public ICQGCELHostEventImpl
{
public:
	CQGServer();   // standard constructor
	~CQGServer();

	enum : UINT { CQG_CONNECT = 1, CQG_TEXT, CQG_CONNECTION_STATUS, CQG_DISCONNECT, CQG_SUBSCRIBE, 
		CQG_UNSUBSCRIBE, CQG_UNSUBSCRIBEALL, CQG_INSTRUMENT_PROPERTIES, CQG_SHUTDOWN, CQG_TRADE, CQG_ASK, CQG_BID, CQG_DOM };

	/// \brief This map is used to declare handler function for event specified event.
	/// \brief One can use "OLE/COM Object Viewer" to open CQGCEL.dll and get get event ids. 
	/// \brief The handler signature as well as the event ids can be found in the type library 
	/// \brief or shown by "OLE/COM Object Viewer". 
	BEGIN_SINK_MAP(CQGServer)
		SINK_ENTRY_EX(1, __uuidof(ICELHostEvent), 1, LogonStateChanged)
		SINK_ENTRY_EX(1, __uuidof(ICELHostEvent), 2, ErrorReceived)
        SINK_ENTRY_EX(1, __uuidof(_ICQGCELEvents), 0x00000003, OnDataConnectionStatusChanged)
		SINK_ENTRY_EX(1, __uuidof(_ICQGCELEvents), 0x0000000b, OnCELStarted)
		SINK_ENTRY_EX(1, __uuidof(ICELHostEvent), 0x00000002, OnCELHostErrorReceived)
        SINK_ENTRY_EX(1, __uuidof(_ICQGCELEvents), 0x0000000a, OnDataError)
		SINK_ENTRY_EX(1, __uuidof(_ICQGCELEvents), 0x00000001, OnLineTimeChanged)
		SINK_ENTRY_EX(1, __uuidof(_ICQGCELEvents), 0x00000004, OnInstrumentSubscribed)
		SINK_ENTRY_EX(1, __uuidof(_ICQGCELEvents), 0x00000005, OnInstrumentChanged)
		SINK_ENTRY_EX(1, __uuidof(_ICQGCELEvents), 0x00000006, OnInstrumentDOMChanged)
		SINK_ENTRY_EX(1, __uuidof(_ICQGCELEvents), 0x0000000c, OnIncorrectSymbol)
	END_SINK_MAP()

protected:
	/// \brief Raised when state has been changed.
	/// \param logonState - new state
	HRESULT __stdcall  LogonStateChanged (LogonState logonState );

	/// \brief Raised when detailed error message has arrived.  
	/// \param message - error description
	HRESULT __stdcall  ErrorReceived (BSTR message);
	/// \brief Fired when some changes occurred 
	/// \brief in the connection with the CQG data server.
	/// \param newStatus - New connection status
	/// \return S_OK on success
	STDMETHOD(OnDataConnectionStatusChanged)(eConnectionStatus newStatus);
	
	/// \brief OnCELStarted is fired when CQGCEL object 
	/// \brief is successfully started up.
	STDMETHOD(OnCELStarted)();
  
	/// \brief Handler of CELHost errors.
	/// \param message - error description
	STDMETHOD (OnCELHostErrorReceived)(BSTR message );

	/// \brief Handle event of some abnormal discrepancy
	/// \brief between data expected and data received from CQG.
	/// \brief This event also can be fired when CQGCEL startup fails.
	/// \param obj - Object where the error occurred
	/// \param errorDescription - String describing the error
	/// \return S_OK on success
	STDMETHOD(OnDataError)(LPDISPATCH obj, BSTR errorDescription);

	/// \brief This event is fired once every second to synchronize with the exchange.
	STDMETHOD(OnLineTimeChanged)(DATE new_line_time);

	/// \brief Fired when the request by the incorrect symbol is made.
	/// \param symbl - Requested symbol
	/// \return S_OK on success
	STDMETHOD(OnIncorrectSymbol)(BSTR symbl);

	/// \brief Fired when new instrument is resolved and subscribed.
	/// \param symbol - Requested symbol
	/// \param instrument - Subscribed instrument object
	/// \return S_OK on success
	STDMETHOD(OnInstrumentSubscribed)(BSTR symbol, ICQGInstrument* instrument);

	/// \brief Fired when any of instrument quotes or 
	/// \brief dynamic instrument properties are changed.
	/// \param instrument - Changed instrument object
	/// \param Quotes - Changed quotes collection
	/// \param props - Changed properties collection
	/// \return S_OK on success
	STDMETHOD(OnInstrumentChanged)(ICQGInstrument* instrument, 
									ICQGQuotes* quotes, 
									ICQGInstrumentProperties* props);

	/// \brief Fired whenever instrument DOM is updated.
	/// \param instrument - Changed instrument object
	/// \param prevAsks - Asks old values collection
	/// \param prevBids - Bids old values collection
	/// \return S_OK on success
	STDMETHOD(OnInstrumentDOMChanged)(ICQGInstrument* instrument, 
										ICQGDOMQuotes* prevAsks, 
										ICQGDOMQuotes* prevBids);

public:

	void Connect(wchar_t* login, wchar_t* password, bool demo, GUID guid);
	void Disconnect();
	void SendText(wchar_t* str);
	void SendConnectionStatus(wchar_t* str, UINT status);
	void CreateCQGCEL();
	void Subscribe(wchar_t* ticker, bool dom);
	void Unsubscribe(wchar_t*);
	void UnsubscribeAll();
	void SendInstrumentProperties(INSTRUMENT_PROPERTIES& ip);
	void SendInstrumentTrade(CQG_INSTRUMENT_TRADE& it);
	void SendInstrumentAsk(CQG_INSTRUMENT_BBO_ASK& iBBO);
	void SendInstrumentBid(CQG_INSTRUMENT_BBO_BID& iBBO);
	void SendInstrumentDOM(CQG_INSTRUMENT_DOM& iDOM);

protected:
	//*****API-зависимые переменные*****************************************
	ATL::CComPtr<ICELHost> m_celHost;	/// \brief CELHost object
	ATL::CComPtr<ICQGCEL> m_spCQGCEL;	/// \brief CQGCEL object
	ATL::CComPtr<ICQGInstrument> m_spActiveInstrument;	/// \brief current active instrument object
	LogonState m_LogonState;			/// \brief Logon state flag

public:
	std::vector<CQG_INSTRUMENT_TRADE> itVec;
};
