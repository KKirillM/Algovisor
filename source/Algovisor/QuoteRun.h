#pragma once
#include "resource.h"
#include "Instrument.h"
#include "afxwin.h"
#include "Auxiliary_methods.h"

class MainWindow;
class QuoteRun;


class List : public CListCtrl
{
public:
	List(QuoteRun* parent = NULL) : CListCtrl()/*, pWnd(parent)*/ {}
	~List() {}

	DECLARE_MESSAGE_MAP()

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};

class QuoteRunSettings : public CDialogEx
{
public:
	QuoteRunSettings(QuoteRun* pParent = NULL);
	~QuoteRunSettings() {}

// Dialog Data
	enum { IDD = IDD_QuoteRun_SETTINGS };

	virtual BOOL OnInitDialog();

	QuoteRun* parentWindowPtr;
	CBrush backgroundBrush;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonAskTrade();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButtonBidTradeText();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnBnClickedButton13();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton12();
	afx_msg void OnBnClickedButton14();
	afx_msg void OnBnClickedButton15();
	afx_msg void OnBnClickedButton17();
	afx_msg void OnBnClickedButton19();
	afx_msg void OnBnClickedButton21();
	afx_msg void OnBnClickedButton16();
	afx_msg void OnBnClickedButton18();
	afx_msg void OnBnClickedButton20();
	afx_msg void OnBnClickedButton22();
	afx_msg void OnBnClickedButton32();
	afx_msg void OnBnClickedButtonFont();
	afx_msg void OnBnClickedButton33();
	afx_msg void OnBnClickedButton23();
	afx_msg void OnBnClickedButton25();
	afx_msg void OnBnClickedButton27();
	afx_msg void OnBnClickedButton29();
	afx_msg void OnBnClickedButton24();
	afx_msg void OnBnClickedButton26();
	afx_msg void OnBnClickedButton28();
	afx_msg void OnBnClickedButton30();
	afx_msg void OnBnClickedButton36();
	afx_msg void OnBnClickedButton37();
	afx_msg void OnBnClickedButton34();
	afx_msg void OnBnClickedButton35();
	afx_msg void OnBnClickedButton38();
    afx_msg void OnBnClickedButtonOpeninterestIncreaseTextColor();
    afx_msg void OnBnClickedButtonOpeninterestIncreaseBackgroundColor();
    afx_msg void OnBnClickedButtonOpeninterestDecreaseTextColor();
    afx_msg void OnBnClickedButtonOpeninterestDecreaseBackgroundColor();
    afx_msg void OnBnClickedButtonOpeninterestNotchangeTextColor();
    afx_msg void OnBnClickedButtonOpeninterestNotchangeBackgroundColor();
};


class QuoteRun : public CFrameWnd
{
    friend class QuoteRunSettings;
public:
	enum : int { TimeWidth = 100, DeltaMicroWidth = 55, PriceWidth = 70, SizeWidth = 60, TradePriceWidth = 75, TradeSizeWidth = 65 };

	QuoteRun(MainWindow* wnd, FeedType feed, IInstrument* instrument = NULL);
	virtual ~QuoteRun();

	void DefineColorsAndText(TCHAR* name, int nameSize, COLORREF& crText, COLORREF& crBkgnd, int colNum/*LPWSTR pszText*/, size_t Indx);
	void GetColors(COLORREF& crText, COLORREF& crBkgnd, size_t Indx, bool sizeCol);
	void GetBBOBidLimitsColor(COLORREF& crText, COLORREF& crBkgnd, size_t Indx);
	void GetBBOAskLimitsColor(COLORREF& crText, COLORREF& crBkgnd, size_t Indx);
	
	void FillAggregationData();
	void FillFilteredData();
    void FillTradesOnly();
	void FillTable();

	void SelectDOMColor(COLORREF& crText, COLORREF& crBkgnd, int colNum, char agressorSide, size_t Indx);
	void Unsubscribe();

private:
    void SetTimerIndex();

private:
    size_t			timerIndex;         // последний индекс, который был обработан коллбэком таймера
    size_t			syncIndex;          // индекс для синхронизации данных по инструментам доступным по указателям instrumentPtr и subInstrumentPtr
    size_t          topIndex;           // последний индекс, для данных полученных от датафида

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg LRESULT InitializeControlList(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnRButtonClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void ContextMenu(UINT nID);
	//afx_msg void OnLvnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawLvCustomDraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

public:
	FeedType		fType;
	IInstrument*	instrumentPtr;
	IInstrument*	subInstrumentPtr;
	std::string		name;

	MainWindow*		pParentWindow;
	List			m_listCtrl;
	CStatusBar		statusBar;	// класс панели состояния
	//CMFCStatusBar	statusBar;


	enum : unsigned { TIME=1, DELTAMS, TRADEPRICE, TRADESIZE, INTEREST, CONDITION, BUYVOL, SELLVOL, DELTABIDLIMITS, DELTAASKLIMITS, 
					BID1PRICE=21, BID2PRICE, BID3PRICE, BID4PRICE, BID5PRICE, BID6PRICE, BID7PRICE, BID8PRICE, BID9PRICE, BID10PRICE, 
					BID1SIZE, BID2SIZE, BID3SIZE, BID4SIZE, BID5SIZE, BID6SIZE, BID7SIZE, BID8SIZE, BID9SIZE, BID10SIZE, 
					BID1ORDERS, BID2ORDERS, BID3ORDERS, BID4ORDERS, BID5ORDERS, BID6ORDERS, BID7ORDERS, BID8ORDERS, BID9ORDERS, BID10ORDERS, 
					ASK1PRICE, ASK2PRICE, ASK3PRICE, ASK4PRICE, ASK5PRICE, ASK6PRICE, ASK7PRICE, ASK8PRICE, ASK9PRICE, ASK10PRICE,
					ASK1SIZE, ASK2SIZE, ASK3SIZE, ASK4SIZE, ASK5SIZE, ASK6SIZE, ASK7SIZE, ASK8SIZE, ASK9SIZE, ASK10SIZE,
					ASK1ORDERS, ASK2ORDERS, ASK3ORDERS, ASK4ORDERS, ASK5ORDERS, ASK6ORDERS, ASK7ORDERS, ASK8ORDERS, ASK9ORDERS, ASK10ORDERS };
	
	std::vector<unsigned> colType; //0 - колонка Time, 1 - DeltaMs, 3 - TradePrice, 4 - TradeSize, 5 - condition, 6(7) - buy/sell volume, 8(9) - delta limits и т.д.

	BOOL freeze;
	BOOL alwaysOnTop;
	QuoteRunSettings* settingsWnd;

	CFont listFont;

	BOOL timeColumn;
	BOOL deltaMicroColumn;
	BOOL tradePriceColumn;
	BOOL tradeSizeColumn;
	BOOL tradeCondition;
	//BOOL Level1PriceColumn;
	//BOOL Level1SizeColumn;
	//BOOL Level1OrdersColumn;
	BOOL Level2PriceColumn;
	BOOL Level2SizeColumn;
	BOOL Level2OrdersColumn;
	BOOL deltaLimitColumn;
	BOOL cumulativeDelta;
	BOOL showOnlyBuyTrades;
	BOOL showOnlySellTrades;
    BOOL showInterestColumn;
	BOOL showTitleBar;
	BOOL showGridLines;

	BOOL moveOrderAddLiquid;
	BOOL moveOrderRemoveLiquid;
	BOOL algoOrderAddLiquid;
	BOOL algoOrderRemoveLiquid;

    BOOL showOpenInterestChange;

	BOOL alertTradeSize;
	BOOL alertDeltaLimitAdd;
	BOOL alertDeltaLimitRemove;
	BOOL alertDeltaMicrosecond;

	BOOL noAggregation;

	BOOL filteringTradeSize;
	UINT filteringTradeValue;

	bool showOnlyTrades;

	COLORREF deltaMicroText;
	COLORREF askTradeText;
	COLORREF bidTradeText;
	COLORREF noAgressorText;
	COLORREF addAskText;
	COLORREF removeAskText;
	COLORREF addBidText;
	COLORREF removeBidText;
	COLORREF moveOrderAddText;
	COLORREF moveOrderRemoveText;
	COLORREF algoOrderAddText;
	COLORREF algoOrderRemoveText;
	COLORREF alertTradeSizeText;
	COLORREF alertDeltaLimitAddText;
	COLORREF alertDeltaLimitRemoveText;
	COLORREF alertDeltaMicrosecondText;

	COLORREF deltaMicroBackgrnd;
	COLORREF askTradeBackgrnd;
	COLORREF bidTradeBackgrnd;
	COLORREF noAgressorBackgrnd;
	COLORREF addAskBackgrnd;
	COLORREF removeAskBackgrnd;
	COLORREF addBidBackgrnd;
	COLORREF removeBidBackgrnd;
	COLORREF moveOrderAddBackgrnd;
	COLORREF moveOrderRemoveBackgrnd;
	COLORREF algoOrderAddBackgrnd;
	COLORREF algoOrderRemoveBackgrnd;
	COLORREF alertTradeSizeBackgrnd;
	COLORREF alertDeltaLimitAddBackgrnd;
	COLORREF alertDeltaLimitRemoveBackgrnd;
	COLORREF alertDeltaMicrosecondBackgrnd;

    COLORREF OIIncreaseText;
    COLORREF OIIncreaseBackground;
    COLORREF OIDecreaseText;
    COLORREF OIDecreaseBackground;
    COLORREF OINotChangeText;
    COLORREF OINotChangeBackground;

	COLORREF defaultbackgroundColor;
	COLORREF defaultTextColor;

	COLORREF defaultBidBackgroundColor;
	COLORREF defaultAskBackgroundColor;
	COLORREF defaultBidTextColor;
	COLORREF defaultAskTextColor;

	COLORREF gridlineColor;

	UINT alertTradeSizeValue;
	int alertDeltaLimitAddValue;
	int alertDeltaLimitRemoveValue;
	UINT alertDeltaMicrosecondValue;

	int alertTradeCondition;
	int alertDeltaLimitAddCondition;
	int alertDeltaLimitRemoveCondition;
	int alertDeltaMicrosecCondition;

	int filteringTradeCondition;
	afx_msg void OnClose();
};