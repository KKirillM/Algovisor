#pragma once
#include "resource.h"
#include "Instrument.h"
#include "Auxiliary_methods.h"

class MainWindow;
class QuantChart;
class HeatMap;
//struct ThreadPoolData;

//BaseChart

class BaseChart : public CFrameWnd
{
public:
	enum ScaleType { VISIBLE_RANGE, ONLY_LAST_VALUE, STATIC_PRICE_RANGE };

	BaseChart(MainWindow* wnd, FeedType feed, IInstrument* i = NULL);
	virtual ~BaseChart();

	void GetPriceAxisWidth();
	int SetScrollBar();
	void SetRects(int cx, int cy);

	void Unsubscribe();

	void FillTradesOnlyVector();

	void FindMaxMinPricesForTrades(HDC dc, const size_t start, const size_t end, Price& maxP, Price& minP, Price& lastP);
	int FindMaxMinPricesForDOM(HDC dc, const size_t start, const size_t end, Price& maxP, Price& minP, Price& lastP, Price& lastBidP, Price& lastAskP);
	void DrawHorizDOM(HDC dc, double count, Price& k, const size_t y, const int x, const Price& scaleMaxPrice);
	void VerticScaleTrade(Price& scaleMaxPrice, Price& scaleMinPrice, Price& lastPrice);
	void VerticScaleDOM(Price& scaleMaxPrice, Price& scaleMinPrice, Price& lastAskPrice, Price& lastBidPrice);
	SIZE DrawHorizAxis(HDC dc, Price scaleMaxPrice, const double count);
	void DrawDOMLabel(HDC dc, UINT size, COLORREF& color1, COLORREF& color2, int Ycoord);

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

public:
	FeedType fType;
	IInstrument* instrumentPtr;
	IInstrument* subInstrumentPtr;
	std::string name;
	size_t timerIndex;
	size_t syncIndex;

	MainWindow* mWnd;
	CStatusBar	statusBar;	// ����� ������ ���������

	RECT clientRect;
	RECT plotZone;
	RECT plotZoneAdd;
	RECT priceZone;
	RECT timeZone;
	RECT plotMoveZone1;

	RECT scaleClientRect;
	RECT scalePlotZone;
	RECT scaleButtonRect;

	int price_axis_width;
	int time_axis_width;
	int YscaleTop;
	int YscaleBottom;
	int Xscale;
	int YlastTrade;
	bool YlastTradeFlag;
	int YlastAsk;
	int YlastBid;
	bool YlastBidAskFlag;

	ScaleType scale_type;
	Price topPriceRange;
	Price bottomPriceRange;

    INT bEventAutoScale;			// �������������� ������� �� �������
    UINT uManualScaleMaxValue;		// �������� ��� ������� ���������������, ��� �������� ������������� � maxDomValue
    UINT uManualScaleMinValue;		// �������� ��� ������� ���������������, ��� �������� ������������� � minDomValue (��� �������� ������ ��� �������)

	BOOL alwaysOnTop;
	BOOL tradesLabels;
	BOOL limitsLabels;
	BOOL ordersLabels;
	BOOL showOrders;                // ���� ������������� ��� ����������, ��������� ���� ������� �� ������� ������ ��� ���������� ������� �� ������
	BOOL tradesOnly;
	BOOL showGridLines;
	BOOL showHorizontalDOM;

    BOOL aggregatedOrders;

	bool lButtonOnPlotArea;
	bool lButtonOnMoveZone1;
	bool mouseOnMoveZone1;
	bool lButtonOnTimeZone;
	bool lButtonOnPriceZone;
	bool mouseOnScaleButton;
	CPoint lButtonPoint;		//���������� �����, � ������� ��������� ������� ������ ���� �� ��� �

	CFont qFont;	

	UINT updateFrequency;		//������� ����������� �������

	COLORREF defaultbackgroundColor;
	COLORREF plotBackgoundColor;
	COLORREF defaultTextColor;
	COLORREF axisColor;
	COLORREF gridColor;
	COLORREF tradeColor; //���� ������� �����
	COLORREF buyTradeColor;
	COLORREF sellTradeColor;
	COLORREF noagressorColor;
	COLORREF tradeLabelColor;	
	COLORREF bidLimitColor;
	COLORREF newBidLimitColor;
	COLORREF askLimitColor;
	COLORREF newAskLimitColor;
	COLORREF bidOrdersColor;
	COLORREF newBidOrdersColor;
	COLORREF askOrdersColor;
	COLORREF newAskOrdersColor;
	COLORREF tradeSizeAlertColor;
	COLORREF limitAddAlertColor;
	COLORREF limitRemoveAlertColor;

    COLORREF OIIncreaseBackground;
    COLORREF OIDecreaseBackground;
    COLORREF OINotChangeBackground;

    BOOL showOpenInterestChange;

	//PTP_WORK gWorkItem1;				//������� ������� ��� ���� �������
	//ThreadPoolData* poolData1;			//������ ��� ������ ����
	//volatile LONG numberOfPoolThread;	//��� ������������� ���� �������

	static ThreadPool tPool;
	
};

//struct ThreadPoolData
//{
//	void SetData(BaseChart* base, const size_t _start, const size_t _end, std::vector<std::vector<std::pair<size_t,size_t>>>* pTimeLabelsVector, 
//				 std::vector<Price>* maxPoolPrice,std::vector<Price>* minPoolPrice,std::vector<Price>* lastBidPoolPrice,std::vector<Price>* lastAskPoolPrice)
//	{
//		numberOfPoolThread = &base->numberOfPoolThread;
//		_i =	base->_i;		 
//		start =	_start;
//		end =	_end;
//		scale_type = base->scale_type;
//		maxPrice =	  maxPoolPrice;
//		minPrice =	  minPoolPrice;
//		lastBidPrice = lastBidPoolPrice;
//		lastAskPrice = lastAskPoolPrice;
//		timeLabelsVector = pTimeLabelsVector;
//	}
//
//	volatile LONG* numberOfPoolThread;
//	Ticker* _i;
//	size_t start;
//	size_t end;
//	int scale_type;
//	std::vector<Price>* maxPrice;
//	std::vector<Price>* minPrice;
//	std::vector<Price>* lastBidPrice;
//	std::vector<Price>* lastAskPrice;
//	std::vector<std::vector<std::pair<size_t,size_t>>>* timeLabelsVector;
//};