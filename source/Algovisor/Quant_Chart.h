#pragma once
#include "afxwin.h"
#include "resource.h"
#include "BaseChart.h"

class MainWindow;

class QuantChartSettings : public CDialogEx
{
public:
	QuantChartSettings(QuantChart* pParent = NULL) : CDialogEx(IDD, (CWnd*)pParent), parentWindowPtr(pParent)
	{	
		//SetBackgroundColor(BGRNCOLOR);
		//backgroundBrush.CreateSolidBrush(BGRNCOLOR);
	}
	~QuantChartSettings() {}

// Dialog Data
	enum { IDD = IDD_QUANTCHART_SETTINGS };

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	QuantChart* parentWindowPtr;
	//CBrush backgroundBrush;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButton15();
	afx_msg void OnBnClickedButton17();
	afx_msg void OnBnClickedButton19();
	afx_msg void OnBnClickedButton21();
	afx_msg void OnBnClickedButtonAsktradetext();
	afx_msg void OnBnClickedButtonBidtradetext();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton33();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnBnClickedButton13();
	afx_msg void OnBnClickedButton14();
	afx_msg void OnBnClickedButton39();
	afx_msg void OnBnClickedButton36();
	afx_msg void OnBnClickedButton37();
	afx_msg void OnBnClickedButton23();
	afx_msg void OnBnClickedButton25();
	afx_msg void OnBnClickedButton27();
	afx_msg void OnBnClickedIOIncreaseColor();
	afx_msg void OnBnClickedIODecreaseColor();
	afx_msg void OnBnClickedIONotChaneColor();
	afx_msg void OnBnClickedButtonFont();
	afx_msg void ManualScaleUpdate(CCmdUI *pCmdUI);
	afx_msg void OnBnClickedChartRadioManualScale();
	afx_msg void OnBnClickedChartRadioAutoScale();
	afx_msg void OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckShowgridlines();
	afx_msg void OnBnClickedChartCheckShowgridlines();
    afx_msg void OnUpdateRadioAggregatedorders(CCmdUI *pCmdUI);
    afx_msg void OnUpdateRadioSplittedorders(CCmdUI *pCmdUI);
};

class QuantChart : public BaseChart
{
public:
	QuantChart(MainWindow* wnd, FeedType feed, IInstrument* i = NULL) : BaseChart(wnd, feed, i) 
	{
		tPool.CreatePool(2);
	}
	virtual ~QuantChart();
	
	void Draw1(HDC dc);
	void Draw3(HDC dc);
	void DrawTrades(HDC dc, const size_t start, const size_t end, const Price& scaleMaxPrice, Price& p1, Price& p2, const double maxDomValue, const double count);
	void DrawDOM(HDC dc, const size_t start, const size_t end, Price& scaleMaxPrice, Price& p1, Price& p2, const double maxDomValue, const double count);
	UINT FindMaxDomValue(const size_t start, const size_t end, Price& p1, Price& p2, Price& maxPrice, Price& scaleMaxPrice);

public:
	

	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void ContextMenu(UINT nID);
	afx_msg void LabelsMenu(UINT nID);
	afx_msg void PlotZoneAddMenu(UINT nID);
	afx_msg void TimeZoneMenu();
	afx_msg void OnPaint();
	afx_msg void OnUpdateQcmenuOrdersshow(CCmdUI *pCmdUI);
};

