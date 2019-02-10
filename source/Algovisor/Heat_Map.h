#pragma once
#include "afxwin.h"
#include "resource.h"
#include "BaseChart.h"

#define COLOR_COUNT 1020
#define MONO_COUNT 206
#define LAST_COLOR_INDEX COLOR_COUNT-1
#define LAST_MONO_INDEX MONO_COUNT-1

class MainWindow;

//HM_Settings

class HM_Settings : public CDialogEx
{
public:
	HM_Settings(HeatMap* pParent = NULL) : CDialogEx(IDD, (CWnd*)pParent), dWnd(pParent)
	{	
		//SetBackgroundColor(BGRNCOLOR);
		//backgroundBrush.CreateSolidBrush(BGRNCOLOR);
	}
	~HM_Settings() {}

// Dialog Data
	enum { IDD = IDD_HEATMAP_SETTINGS };

	virtual BOOL OnInitDialog();

	HeatMap* dWnd;
	//CBrush backgroundBrush;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
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
	afx_msg void OnBnClickedButtonFont();
	afx_msg void OnBnClickedCheckShowgridlines();
	afx_msg void ManualMaxScaleUpdate(CCmdUI *pCmdUI);
	afx_msg void ManualMinScaleUpdate(CCmdUI *pCmdUI);
	afx_msg void OnBnClickedRadioManualScale();
	afx_msg void OnDeltaposSpinScaleValue(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRadioOnlylastvalue();
	afx_msg void OnBnClickedRadioVisiblerange();
	afx_msg void OnDeltaposSpinScaleValue2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditMaxScaleValue();
	afx_msg void OnEnChangeEditMinScaleValue();
	afx_msg void OnBnClickedRadioColorTheme();
	afx_msg void OnBnClickedRadioMonoTheme();
};

//HeatMap

class HeatMap : public BaseChart
{
public:
	enum COLOR_THEME : int { COLOR, MONOCHROME };

	HeatMap(MainWindow* wnd, FeedType feed, IInstrument* i = NULL);
	virtual ~HeatMap();
	
	void Draw1(HDC dc);
	void Draw3(HDC dc);
	void DrawTrades(HDC dc, const size_t start, const size_t end, const Price& scaleMaxPrice, Price& p1, Price& p2, const double count);
	void DrawDOM(HDC dc, const size_t start, const size_t end, const Price& scaleMaxPrice, Price& p1, Price& p2, UINT& maxDomValue, UINT& minDomValue, const double count);
	void FindMaxDomValue(const size_t start, const size_t end, Price& p1, Price& p2, Price& maxPrice, Price& scaleMaxPrice, UINT& maxDomValue, UINT& minDomValue);
	void DrawColorHorizDOM(HDC dc, double count, Price& k, const size_t& y, const int& x, const Price& scaleMaxPrice, UINT maxDomValue, UINT minDomValue);
	
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

public:
	COLORREF colors[COLOR_COUNT];
	COLORREF monochrome[MONO_COUNT];
	int iColorTheme;

	COLORREF iAddSizeColor;
	COLORREF iRemoveSizeColor;
	afx_msg void OnUpdateQcmenuOrdersshow(CCmdUI *pCmdUI);
};