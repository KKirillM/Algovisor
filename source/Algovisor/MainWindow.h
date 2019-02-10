#pragma once
#include "resource.h"
#include "Auxiliary_methods.h"
#include "singleton.h"

class RithmicCallbacks;
class RithmicDialog;
class CQGDialog;
class NinjaDialog;
class Plaza2Dialog;
class Plaza2ReplayDialog;
class IPlaza2ConnectorType2;
class QuoteRun;
class QuantChart;
class HeatMap;
class IInstrument;

struct INSTRUMENT_PROPERTIES;
struct Instrument_DATA;
struct Trade_DATA;
struct BBO_DATA;
struct BBO_DATA;
struct DOM_DATA;
struct DELETE_INSTRUMENT;


class MainWindow : public CDialogEx
{
public:	
    MainWindow(CWnd* pParent = NULL);
	virtual ~MainWindow();

	virtual void OnOK();

	enum { IDD = DIALOG_MAINWINDOW };

	void UpdateActivityWindow(const wchar_t* message);
	void DisconnectFromFeed(FeedType ft);
	IInstrument* GetSelectedInstrument(FeedType& ft);
	HBRUSH BrushForOnCtlColor(CString s);
	HWND IsCQGServerRun();
	void ResetInstrumentPointers(CStringA& s);
	void DeleteFromVector(FeedType ft, CStringA& s);
	void CloseWnd();
	void StartPipeServerForNinja();
	void StopPipeServerForNinja();
	void AddNewDatafeedToComboBox(tstring feedName);				//заносит новый датафид в комбобокс
	void RemoveDatafeedFromComboBox(tstring feedName);				//удаляет датафид из списка в комбобоксе
	void FillInstrumentComboBox(const std::vector<CString>& list);	//заполняет раскрывающийся список названиями инструментов
	void AddNewInstrumentToListControl(tstring& datafeedName, 
									   tstring& ticker, 
									   tstring& ID, 
									   tstring& status, 
									   tstring& exchange, 
									   tstring& expiration, 
									   tstring& description);		//добавление нового инструмента в список главного окна

private:
	DWORD NumberOfProcessors();					//возвращает кол-во физических ядер процессора
	FeedType GetFeedType() const;				//возвращает датафид который в настоящее время выбран в раскрывающемся списке
	void RithmicSubscribe();
	void CQGSubsribe();
	void NinjaSubscribe();
	void Plaza2Subscribe();
	void Plaza2ReplaySubscribe();
	void RithmicUnsubscribe(int nItem);
	void CQGUnsubscribe(int nItem);
	void NinjaUnsubscribe(int nItem);
	void Plaza2Unsubscribe(int nItem);
	void Plaza2ReplayUnsubscribe(int nItem);
    void CloseAllConnectionWindows();
    void CloseAllGraphicWindows();
    void CloseAllWindows();
    void DisconnectAllDataFeeds();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:	
	std::vector<std::vector<CString>> vecSymbol;		//0 - название в списке, 1 - тикер ритмика, 2 - биржа ритмика, 3 - тикер cqg
	CComboBox comboFeedType;
	CComboBox comboBoxMonth;
	CComboBox comboBoxYear;
	CComboBox comboSymbol;
	CListCtrl instrumentList;
	CStatic rithmic_connection_indicator;
	CStatic cqg_connector_indicator;
	CStatic ninjatrader_connection_indicator;
	CStatic plaza2_connection_indicator;
	CEdit activityMonitor;
	//CBrush backgroundBrush;

	std::vector<QuoteRun*> quoteRunWindows;
	std::vector<QuantChart*> quantChartWindows;
	std::vector<HeatMap*> heatMapWindows;

	// диалоговые окна для коннектов к разным фидам
	RithmicDialog* rithmicDlg;
	CQGDialog* cqgDlg;
	NinjaDialog* ninjaDlg;
	Plaza2Dialog* plaza2Dlg;
	Plaza2ReplayDialog* plaza2ReplayDlg;

	// Rithmic variables
	int rithmicStatus;
	RithmicCallbacks* rithmicCallbacks;
	BOOL spreadLegTrades;

	// CQG variables
	int cqgStatus;
	bool cqgSubscribeStatus;
	INSTRUMENT_PROPERTIES* cqgIp;
	std::vector<CQG_INSTRUMENT_TRADE> cqgTrade;
	std::vector<CQG_INSTRUMENT_BBO_ASK> cqg_BBO_ASK;
	std::vector<CQG_INSTRUMENT_BBO_BID> cqg_BBO_BID;
	std::vector<CQG_INSTRUMENT_DOM> cqg_DOM;

	// Ninja variables
	int ninjaStatus;
	std::wstring securityString;
	ninjaList<Instrument_DATA> ninjaIdata;
	ninjaList<Trade_DATA> ninjaTrade;
	ninjaList<BBO_DATA> ninja_BBO_ASK;
	ninjaList<BBO_DATA> ninja_BBO_BID;
	ninjaList<DOM_DATA> ninja_DOM;
	ninjaList<DELETE_INSTRUMENT> ninjaDelete;
	std::list<HANDLE> hPipesList; //список хэндлов для пайпов
	std::list<HANDLE> hThreadList; //список потоков для пайпов

	// rithmic and omne logo
	CStatic rithmicLogo;
	CStatic omneLogo;

	// Plaza2
	std::shared_ptr<IPlaza2ConnectorType2> p2Connector;
	std::vector<CString> p2InstrumentNames;

public:	
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnSelchangingTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonClear();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedQuoteRun();
	afx_msg void OnBnClickedQuantChart();
	afx_msg void OnBnClickedHeatMap();
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnUpdateSubscribe(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGetHistoricalData(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUnSubscribe(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpreadLegTrades(CCmdUI* pCmdUI);
	afx_msg void OnDatafeedComboBox(CCmdUI* pCmdUI);
	afx_msg void OnSymbolComboBox(CCmdUI* pCmdUI);
	afx_msg void OnMonthComboBox(CCmdUI* pCmdUI);
	afx_msg void OnYearComboBox(CCmdUI* pCmdUI);
	afx_msg void OnBnClickedSubscribe();
	afx_msg void OnCbnSelchangeCombobox();
	afx_msg void OnBnClickedUnsubscribe();
	afx_msg void OnBnClickedUnsubscribeAll();
	afx_msg void OnBnClickedGethistory();
	afx_msg void OnBnClickedCheckSpreadlegtrades();
	afx_msg void OnConnectionRithmic();
	afx_msg void OnConnectionCqg();
	afx_msg void OnConnectionNinjatrader();
	afx_msg void OnConnectionPlaza2();
	afx_msg void OnPlaza2Replay();
	afx_msg void OnQuoteRunButton(CCmdUI* pCmdUI);
	afx_msg void OnQuantChartButton(CCmdUI* pCmdUI);
	afx_msg void OnQuoteMapperButton(CCmdUI* pCmdUI);
	afx_msg void OnAppAbout();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnPowerMsgRcvd(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT CQGInstrumentProperty(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT CQGInstrumentTrade(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT CQGInstrumentAsk(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT CQGInstrumentBid(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT CQGInstrumentDOM(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT NinjaInstrumentData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT NinjaInstrumentTrade(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT NinjaInstrumentAsk(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT NinjaInstrumentBid(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT NinjaInstrumentDOM(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT NinjaInstrumentDELETE(WPARAM wParam, LPARAM lParam);	
	afx_msg LRESULT NinjaGettingData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT InvalidateNinjaConnectionIndicator(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClose();
	afx_msg void OnSelchangeDatafeedCombobox();

	DECLARE_MESSAGE_MAP()
private:
	BAN_COPY_CONSTRUCTOR(MainWindow)
        
};  // MainWindow

typedef CSingletonHolder<MainWindow> MainWnd;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CStatic logo;
};