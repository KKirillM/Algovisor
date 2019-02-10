#include "stdafx.h"
#include "BaseChart.h"
#include "Heat_Map.h"
#include "Instrument.h"
#include "MainWindow.h"
#include "resource.h"
#include "Auxiliary_methods.h"

// HM_Settings

BEGIN_MESSAGE_MAP(HM_Settings, CDialogEx)
	ON_BN_CLICKED(IDOK, &HM_Settings::OnBnClickedOk)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON15, &HM_Settings::OnBnClickedButton15)
	ON_BN_CLICKED(IDC_BUTTON17, &HM_Settings::OnBnClickedButton17)
	ON_BN_CLICKED(IDC_BUTTON19, &HM_Settings::OnBnClickedButton19)
	ON_BN_CLICKED(IDC_BUTTON21, &HM_Settings::OnBnClickedButton21)
	ON_BN_CLICKED(IDC_BUTTON_ASKTRADETEXT, &HM_Settings::OnBnClickedButtonAsktradetext)
	ON_BN_CLICKED(IDC_BUTTON_BIDTRADETEXT, &HM_Settings::OnBnClickedButtonBidtradetext)
	ON_BN_CLICKED(IDC_BUTTON5, &HM_Settings::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON33, &HM_Settings::OnBnClickedButton33)
	ON_BN_CLICKED(IDC_BUTTON7, &HM_Settings::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON9, &HM_Settings::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON11, &HM_Settings::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON13, &HM_Settings::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_BUTTON14, &HM_Settings::OnBnClickedButton14)
	ON_BN_CLICKED(IDC_BUTTON39, &HM_Settings::OnBnClickedButton39)
	ON_BN_CLICKED(IDC_BUTTON36, &HM_Settings::OnBnClickedButton36)
	ON_BN_CLICKED(IDC_BUTTON37, &HM_Settings::OnBnClickedButton37)
	ON_BN_CLICKED(IDC_BUTTON23, &HM_Settings::OnBnClickedButton23)
	ON_BN_CLICKED(IDC_BUTTON25, &HM_Settings::OnBnClickedButton25)
	ON_BN_CLICKED(IDC_BUTTON27, &HM_Settings::OnBnClickedButton27)
	ON_BN_CLICKED(IDC_BUTTON_FONT, &HM_Settings::OnBnClickedButtonFont)
	ON_BN_CLICKED(HEATMAP_CHECK_SHOWGRIDLINES, &HM_Settings::OnBnClickedCheckShowgridlines)
	ON_UPDATE_COMMAND_UI(HEATMAP_EDIT_MAX_SCALE_VALUE, &HM_Settings::ManualMaxScaleUpdate)
	ON_UPDATE_COMMAND_UI(HEATMAP_EDIT_MIN_SCALE_VALUE, &HM_Settings::ManualMinScaleUpdate)
	ON_BN_CLICKED(HEATMAP_RADIO_MANUAL_SCALE, &HM_Settings::OnBnClickedRadioManualScale)
	ON_NOTIFY(UDN_DELTAPOS, HEATMAP_SPIN_SCALE_VALUE, &HM_Settings::OnDeltaposSpinScaleValue)
	ON_BN_CLICKED(HEATMAP_RADIO_ONLYLASTVALUE, &HM_Settings::OnBnClickedRadioOnlylastvalue)
	ON_BN_CLICKED(HEATMAP_RADIO_VISIBLERANGE, &HM_Settings::OnBnClickedRadioVisiblerange)
	ON_NOTIFY(UDN_DELTAPOS, HEATMAP_SPIN_SCALE_VALUE2, &HM_Settings::OnDeltaposSpinScaleValue2)
	ON_EN_CHANGE(HEATMAP_EDIT_MAX_SCALE_VALUE, &HM_Settings::OnEnChangeEditMaxScaleValue)
	ON_EN_CHANGE(HEATMAP_EDIT_MIN_SCALE_VALUE, &HM_Settings::OnEnChangeEditMinScaleValue)
	ON_BN_CLICKED(HEATMAP_RADIO_COLOR_THEME, &HM_Settings::OnBnClickedRadioColorTheme)
	ON_BN_CLICKED(HEATMAP_RADIO_MONO_THEME, &HM_Settings::OnBnClickedRadioMonoTheme)
END_MESSAGE_MAP()

BOOL HM_Settings::PreTranslateMessage(MSG* pMsg)
{
	UpdateDialogControls(this, FALSE);
	return CDialogEx::PreTranslateMessage(pMsg);
}

void HM_Settings::OnBnClickedRadioOnlylastvalue()
{
	dWnd->bEventAutoScale = 0;
	dWnd->Invalidate(0);
}

void HM_Settings::OnBnClickedRadioVisiblerange()
{
	dWnd->bEventAutoScale = 1;
	dWnd->Invalidate(0);
}

void HM_Settings::OnBnClickedRadioManualScale()
{
	dWnd->bEventAutoScale = 2;
	dWnd->Invalidate(0);
}

void HM_Settings::ManualMaxScaleUpdate(CCmdUI *pCmdUI)
{
	if (IsDlgButtonChecked(HEATMAP_RADIO_MANUAL_SCALE))
		pCmdUI->Enable(1);
	else
		pCmdUI->Enable(0);
}
void HM_Settings::ManualMinScaleUpdate(CCmdUI *pCmdUI)
{
	if (IsDlgButtonChecked(HEATMAP_RADIO_MANUAL_SCALE))
		pCmdUI->Enable(1);
	else
		pCmdUI->Enable(0);
}

void HM_Settings::OnDeltaposSpinScaleValue(NMHDR *pNMHDR, LRESULT *pResult)	//for MAX value
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	UINT uMaxNum = dWnd->uManualScaleMaxValue/*GetDlgItemInt(HEATMAP_EDIT_MAX_SCALE_VALUE)*/, 
		 uMinNum = dWnd->uManualScaleMinValue/*GetDlgItemInt(HEATMAP_EDIT_MIN_SCALE_VALUE)*/;
	
	if (pNMUpDown->iDelta > 0)	// -
	{
		if (uMaxNum-1 > uMinNum)
		{
			--uMaxNum;
			SetDlgItemInt(HEATMAP_EDIT_MAX_SCALE_VALUE, uMaxNum);
		}
	}
	else						// +
	{
		if (uMaxNum+1 != 9999)
		{
			++uMaxNum;
			SetDlgItemInt(HEATMAP_EDIT_MAX_SCALE_VALUE, uMaxNum);
		}
	}

	//dWnd->uManualScaleMaxValue = uMaxNum;

	dWnd->Invalidate(0);
}

void HM_Settings::OnDeltaposSpinScaleValue2(NMHDR *pNMHDR, LRESULT *pResult)	//for MIN value
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	UINT uMaxNum = dWnd->uManualScaleMaxValue/*GetDlgItemInt(HEATMAP_EDIT_MAX_SCALE_VALUE)*/, 
		 uMinNum = dWnd->uManualScaleMinValue/*GetDlgItemInt(HEATMAP_EDIT_MIN_SCALE_VALUE)*/;

	if (pNMUpDown->iDelta > 0)	// -
	{
		if (uMinNum-1 > 1)
		{
			--uMinNum;
			SetDlgItemInt(HEATMAP_EDIT_MIN_SCALE_VALUE, uMinNum);
		}
	}
	else						// +
	{
		if (uMinNum+1 < uMaxNum)
		{
			++uMinNum;
			SetDlgItemInt(HEATMAP_EDIT_MIN_SCALE_VALUE, uMinNum);
		}
	}

	//dWnd->uManualScaleMinValue = uMinNum;

	dWnd->Invalidate(0);
}

void HM_Settings::OnEnChangeEditMaxScaleValue()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	UINT uMaxNum = GetDlgItemInt(HEATMAP_EDIT_MAX_SCALE_VALUE), 
		 uMinNum = GetDlgItemInt(HEATMAP_EDIT_MIN_SCALE_VALUE);
	
	if (uMaxNum <= uMinNum)
		uMaxNum = uMinNum+1;

	dWnd->uManualScaleMaxValue = uMaxNum;

	UpdateData(0);

	dWnd->Invalidate(0);
}

void HM_Settings::OnEnChangeEditMinScaleValue()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	UINT uMaxNum = GetDlgItemInt(HEATMAP_EDIT_MAX_SCALE_VALUE), 
		 uMinNum = GetDlgItemInt(HEATMAP_EDIT_MIN_SCALE_VALUE);
	
	if (uMaxNum <= uMinNum)
		uMinNum = uMaxNum-1;

	dWnd->uManualScaleMinValue = uMinNum;

	UpdateData(0);

	dWnd->Invalidate(0);
}

void HM_Settings::OnBnClickedRadioColorTheme()
{
	dWnd->iColorTheme = dWnd->COLOR;
	dWnd->Invalidate(0);
}

void HM_Settings::OnBnClickedRadioMonoTheme()
{
	dWnd->iColorTheme = dWnd->MONOCHROME;
	dWnd->Invalidate(0);
}

void HM_Settings::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, HEATMAP_CHECK_SHOWGRIDLINES, dWnd->showGridLines);
	DDX_Radio(pDX, HEATMAP_RADIO_ONLYLASTVALUE, dWnd->bEventAutoScale);
	DDX_Text(pDX, HEATMAP_EDIT_MAX_SCALE_VALUE, dWnd->uManualScaleMaxValue);
	DDX_Text(pDX, HEATMAP_EDIT_MIN_SCALE_VALUE, dWnd->uManualScaleMinValue);
	DDV_MinMaxUInt(pDX, dWnd->uManualScaleMaxValue, 2, 9999);
	DDV_MinMaxUInt(pDX, dWnd->uManualScaleMinValue, 1, 9998);
	DDX_Text(pDX, IDC_EDIT_UPDATEFREQUENCY, dWnd->updateFrequency);
	DDV_MinMaxInt(pDX, dWnd->updateFrequency, 1, 5000);	
	DDX_Radio(pDX, HEATMAP_RADIO_COLOR_THEME, dWnd->iColorTheme);
}
BOOL HM_Settings::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	LOGFONT log;
	TCHAR str[15];

	dWnd->qFont.GetLogFont(&log);
	StringCchPrintf(str, 15, L"%.10s", log.lfFaceName);
	GetDlgItem(IDC_BUTTON_FONT)->SetWindowText(str);

	return TRUE;
}
void HM_Settings::OnBnClickedOk()
{
	CDialogEx::OnOK();

	if (dWnd->subInstrumentPtr != NULL) 
	{
		delete dWnd->instrumentPtr;
		dWnd->instrumentPtr = dWnd->subInstrumentPtr;
		dWnd->subInstrumentPtr = NULL;
		dWnd->timerIndex = 0;
	}

	UpdateData();
	dWnd->KillTimer(1);
	dWnd->SetTimer(1, dWnd->updateFrequency, 0);

	dWnd->Invalidate(0);
}
HBRUSH HM_Settings::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	
	switch (pWnd->GetDlgCtrlID())
	{
	case IDC_STATIC_BACKGROUNDCOLOR:				pDC->SetTextColor(dWnd->defaultbackgroundColor); break;
	case IDC_STATIC_DEFAULTTEXTCOLOR:				pDC->SetTextColor(dWnd->defaultTextColor); break;
	case IDC_STATIC_PLOTBACKGROUNDCOLOR:			pDC->SetTextColor(dWnd->plotBackgoundColor); break;
	case IDC_STATIC_AXISCOLOR:						pDC->SetTextColor(dWnd->axisColor); break;
	case IDC_STATIC_BUYTRADECOLOR:					pDC->SetTextColor(dWnd->buyTradeColor); break;
	case IDC_STATIC_SELLTRADECOLOR:					pDC->SetTextColor(dWnd->sellTradeColor); break;
	case IDC_STATIC_IMPLIEDTRADECOLOR:				pDC->SetTextColor(dWnd->noagressorColor); break;
	case IDC_STATIC_GRIDLINESCOLOR:					pDC->SetTextColor(dWnd->gridColor); break;	
	case IDC_STATIC:
		//SetTextColor(pDC->m_hDC, RGB(255,255,255));
		//SetBkColor( pDC->m_hDC, BGRNCOLOR);
		//return (HBRUSH)(backgroundBrush.GetSafeHandle());	 
		break;
	}	
	
	return hbr;
}

void HM_Settings::OnBnClickedCheckShowgridlines()
{
	dWnd->showGridLines = IsDlgButtonChecked(HEATMAP_CHECK_SHOWGRIDLINES);
	dWnd->Invalidate(0);
}
void HM_Settings::OnBnClickedButton15()
{
	CColorDialog clrDlg(dWnd->defaultbackgroundColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		dWnd->defaultbackgroundColor = clrDlg.GetColor();
		Invalidate(0);
		dWnd->Invalidate(0);
	}
}
void HM_Settings::OnBnClickedButton17()
{
	CColorDialog clrDlg(dWnd->plotBackgoundColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		dWnd->plotBackgoundColor = clrDlg.GetColor();
		Invalidate(0);
		dWnd->Invalidate(0);
	}
}
void HM_Settings::OnBnClickedButton19()
{
	CColorDialog clrDlg(dWnd->defaultTextColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		dWnd->defaultTextColor = clrDlg.GetColor();
		Invalidate(0);
		dWnd->Invalidate(0);
	}
}
void HM_Settings::OnBnClickedButton21()
{
	CColorDialog clrDlg(dWnd->axisColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		dWnd->axisColor = clrDlg.GetColor();
		Invalidate(0);
		dWnd->Invalidate(0);
	}
}
void HM_Settings::OnBnClickedButtonAsktradetext()
{
	CColorDialog clrDlg(dWnd->buyTradeColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		dWnd->buyTradeColor = clrDlg.GetColor();
		Invalidate(0);
		dWnd->Invalidate(0);
	}
}
void HM_Settings::OnBnClickedButtonBidtradetext()
{
	CColorDialog clrDlg(dWnd->sellTradeColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		dWnd->sellTradeColor = clrDlg.GetColor();
		Invalidate(0);
		dWnd->Invalidate(0);
	}
}
void HM_Settings::OnBnClickedButton5()
{
	CColorDialog clrDlg(dWnd->noagressorColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		dWnd->noagressorColor = clrDlg.GetColor();
		Invalidate(0);
		dWnd->Invalidate(0);
	}
}
void HM_Settings::OnBnClickedButton33()
{
	CColorDialog clrDlg(dWnd->gridColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		dWnd->gridColor = clrDlg.GetColor();
		Invalidate(0);
		dWnd->Invalidate(0);
	}
}
void HM_Settings::OnBnClickedButton7()
{
	CColorDialog clrDlg(dWnd->askLimitColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		dWnd->askLimitColor = clrDlg.GetColor();
		Invalidate(0);
		dWnd->Invalidate(0);
	}
}
void HM_Settings::OnBnClickedButton9()
{
	CColorDialog clrDlg(dWnd->newAskLimitColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		dWnd->newAskLimitColor = clrDlg.GetColor();
		Invalidate(0);
		dWnd->Invalidate(0);
	}
}
void HM_Settings::OnBnClickedButton11()
{
	CColorDialog clrDlg(dWnd->bidLimitColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		dWnd->bidLimitColor = clrDlg.GetColor();
		Invalidate(0);
		dWnd->Invalidate(0);
	}
}
void HM_Settings::OnBnClickedButton13()
{
	CColorDialog clrDlg(dWnd->newBidLimitColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		dWnd->newBidLimitColor = clrDlg.GetColor();
		Invalidate(0);
		dWnd->Invalidate(0);
	}
}
void HM_Settings::OnBnClickedButton14()
{
	CColorDialog clrDlg(dWnd->askOrdersColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		dWnd->askOrdersColor = clrDlg.GetColor();
		Invalidate(0);
		dWnd->Invalidate(0);
	}
}
void HM_Settings::OnBnClickedButton39()
{
	CColorDialog clrDlg(dWnd->newAskOrdersColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		dWnd->newAskOrdersColor = clrDlg.GetColor();
		Invalidate(0);
		dWnd->Invalidate(0);
	}
}
void HM_Settings::OnBnClickedButton36()
{
	CColorDialog clrDlg(dWnd->bidOrdersColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		dWnd->bidOrdersColor = clrDlg.GetColor();
		Invalidate(0);
		dWnd->Invalidate(0);
	}
}
void HM_Settings::OnBnClickedButton37()
{
	CColorDialog clrDlg(dWnd->newBidOrdersColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		dWnd->newBidOrdersColor = clrDlg.GetColor();
		Invalidate(0);
		dWnd->Invalidate(0);
	}
}
void HM_Settings::OnBnClickedButton23()
{
	CColorDialog clrDlg(dWnd->tradeSizeAlertColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		dWnd->tradeSizeAlertColor = clrDlg.GetColor();
		Invalidate(0);
		dWnd->Invalidate(0);
	}
}
void HM_Settings::OnBnClickedButton25()
{
	CColorDialog clrDlg(dWnd->limitAddAlertColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		dWnd->limitAddAlertColor = clrDlg.GetColor();
		Invalidate(0);
		dWnd->Invalidate(0);
	}
}
void HM_Settings::OnBnClickedButton27()
{
	CColorDialog clrDlg(dWnd->limitRemoveAlertColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		dWnd->limitRemoveAlertColor = clrDlg.GetColor();
		Invalidate(0);
		dWnd->Invalidate(0);
	}
}
void HM_Settings::OnBnClickedButtonFont()
{
	LOGFONT log; 
	
	dWnd->qFont.GetLogFont(&log);
	
	CFontDialog fontDlg(&log, CF_EFFECTS|CF_SCREENFONTS, NULL, this);

	if (fontDlg.DoModal() == IDOK) {
		TCHAR str[15];
		//static CFont font; 

		fontDlg.GetCurrentFont(&log);
		StringCchPrintf(str, 15, L"%.10s", log.lfFaceName);
		GetDlgItem(IDC_BUTTON_FONT)->SetWindowText(str);

		dWnd->qFont.DeleteObject();
		dWnd->qFont.CreateFontIndirect(&log);

		dWnd->GetPriceAxisWidth();
		Invalidate(0);
		dWnd->Invalidate(0);
	}

	/*size_t i = 0;
	while (!dWnd->_i->Get_tradePrice(i).IsPrice())
		++i;
	HDC dc = ::GetDC(dWnd->m_hWnd);
	::SelectObject(dc, (HFONT)dWnd->qFont);
	TCHAR str[64];
	SIZE sSize;
	StringCchPrintf(str, 64, L"%s", dWnd->_i->Get_tradePrice(i).GetWString(dWnd->_i->GetPrecision()));
	::GetTextExtentPoint32(dc, str, (int)_tcslen(str), &sSize);
	dWnd->price_axis_width = sSize.cx + 15;
	dWnd->time_axis_width = sSize.cy*2;
	::DeleteDC(dc);

	dWnd->plotZone.left = dWnd->plotZone.right = 0;
	dWnd->plotZone.right = dWnd->clientRect.right - dWnd->price_axis_width;
	dWnd->plotZone.bottom = dWnd->clientRect.bottom - dWnd->time_axis_width;
			
	dWnd->plotMoveZone1.left = dWnd->clientRect.right - dWnd->price_axis_width - 5;
	dWnd->plotMoveZone1.top = dWnd->clientRect.bottom - dWnd->time_axis_width+1; 
	dWnd->plotMoveZone1.right = dWnd->clientRect.right - dWnd->price_axis_width; 
	dWnd->plotMoveZone1.bottom = dWnd->clientRect.bottom;*/
}

//HeatMap

BEGIN_MESSAGE_MAP(HeatMap, BaseChart)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_QCMENU_ALWAYSONTOP, ID_QCMENU_SETTINGS, ContextMenu)
	ON_COMMAND_RANGE(ID_QCMENU_TRADELABELS, ID_QCMENU_CONSTANTRANGE, LabelsMenu)
	ON_COMMAND_RANGE(ID_PRICEZONEADD_HORIZONTALDOM, ID_PRICEZONEADD_HORIZONTALDOM, PlotZoneAddMenu)
	ON_COMMAND(ID_TIMEZONE_RESECSCALE, TimeZoneMenu)
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_UPDATE_COMMAND_UI(ID_QCMENU_ORDERSSHOW, &HeatMap::OnUpdateQcmenuOrdersshow)
END_MESSAGE_MAP()

HeatMap::HeatMap(MainWindow* wnd, FeedType feed, IInstrument* i) : BaseChart(wnd, feed, i) 
{
	uManualScaleMaxValue = 2;
	uManualScaleMinValue = 1;

	iColorTheme = COLOR; //по-умолчанию цветовая схема используется color

	iAddSizeColor = RGB(0,255,0)/*(128,255,128)*/;		//зеленый
	iRemoveSizeColor = RGB(255,0,0)/*(255,128,128)*/;	//красный

	//создаем массив цветов для рисовния
	int x = 0;
	for (int i = 0; i < 1020; ++i)
	{
		if (i >= 0 && i <= 255)		//синий - голубой
		{
			colors[i] = RGB(0,i,255);
		}
		else if (i > 255 && i <= 510)	//голубой - зеленый
		{
			if (i > 382)
				x = i - 256 - (i - 383) * 2;
			else
				x = i - 256 + (383 - i) * 2;
			colors[i] = RGB(0,255,x);
		}
		else if (i > 510 && i <= 765)	//зеленый - желтый
		{
			x = i - 511;
			colors[i] = RGB(x,255,0);
		}
		else if (i > 765)				//желтый - красный
		{
			if (i > 892)
				x = i - 766 - (i - 893) * 2;
			else
				x = i - 766 + (893 - i) * 2;
			colors[i] = RGB(255,x,0);
		}
	}
	//для монохромной раскраски
	for (int i = 50, j = LAST_MONO_INDEX; i < 256; ++i, --j)
	{
		monochrome[j] = RGB(i,i,i);
	}
}
HeatMap::~HeatMap() 
{
	for (ptrdiff_t x = mWnd->heatMapWindows.size()-1; x >= 0; --x) 
	{
		if (this == mWnd->heatMapWindows[x])
		{
			mWnd->heatMapWindows.erase(mWnd->heatMapWindows.begin()+x);
			break;
		}
	}
}

int HeatMap::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	//указываем в заголовке окна название инструмента
	wchar_t title[100];
	StringCchPrintfW(title, 100, L"Quote Mapper | %S", name.c_str());
	SetWindowTextW(title);

 	HICON m_hIconSmall = (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_QUOTEMAPPER), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR); // Загрузить иконку 16х16 
 	HICON m_hIconBig = (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_QUOTEMAPPER), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);	// Загрузить иконку 32x32 
	SetIcon(m_hIconSmall, false); 
	SetIcon(m_hIconBig, true);

	if (BaseChart::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void HeatMap::OnPaint()
{
	/*RECT r, rt;
	GetClientRect(&r);
	statusBar.GetClientRect(&rt);
	if (clientRect.bottom != r.bottom)
		SetRects(r.right, r.bottom - rt.bottom);*/
	

	CPaintDC dc(this); // device context for painting
	HDC memDC = ::CreateCompatibleDC(dc);
	HBITMAP memBitmap = ::CreateCompatibleBitmap(dc, scaleClientRect.right, scaleClientRect.bottom);
	::SelectObject(memDC, memBitmap);

	if (!instrumentPtr)	//если от инструмента за которым было закреплено окно отписались
	{
		DrawUnsubscribeString(memDC, clientRect, fType, name.c_str());	//рисуем надпись

		::BitBlt(dc, 0,0, clientRect.right, clientRect.bottom, memDC, 0,0, SRCCOPY);
		::DeleteObject(memBitmap);
		::DeleteDC(memDC);
		return;
	}
	
	::SelectObject(memDC, (HPEN)::GetStockObject(DC_PEN));
	::SelectObject(memDC, (HBRUSH)::GetStockObject(DC_BRUSH));
	::SelectObject(memDC, (HFONT)qFont);
	::SetTextAlign(memDC, TA_NOUPDATECP | TA_CENTER | TA_BOTTOM/*TA_BASELINE*/);
	::SetBkMode(memDC, TRANSPARENT);
	::SetTextColor(memDC, defaultTextColor);

	//цвет фона
	::SetDCBrushColor(memDC, defaultbackgroundColor);
	::FillRect(memDC, &scaleClientRect, (HBRUSH)::GetStockObject(DC_BRUSH));
	::SetDCBrushColor(memDC, plotBackgoundColor);
	::FillRect(memDC, &scalePlotZone, (HBRUSH)::GetStockObject(DC_BRUSH));

	//если ширина ценовой шкалы еще не определена
	if (!price_axis_width)
	{
		//определяем ширину ценовой шкалы
		GetPriceAxisWidth();	

		::BitBlt(dc, 0,0, clientRect.right, clientRect.bottom, memDC, 0,0, SRCCOPY);
		::DeleteObject(memBitmap);
		::DeleteDC(memDC);
		return;
	}
	
	//рисуем координатные оси
	::SetDCPenColor(memDC, axisColor);
	::MoveToEx(memDC, scaleClientRect.right-price_axis_width, 0, NULL); ::LineTo(memDC, scaleClientRect.right-price_axis_width, scalePlotZone.bottom +1);
	::MoveToEx(memDC, 0, scalePlotZone.bottom, NULL); ::LineTo(memDC, scaleClientRect.right-price_axis_width, scalePlotZone.bottom);

	//если горизонтальный масштаб менялся рисуем кнопку 'S'
	//если горизонтальный или вертикальный масштабы менялись рисуем кнопку 'S'
	bool YScale = YscaleTop < 0 || YscaleBottom < 0 || YscaleTop > 1 || YscaleBottom > 1;
	if (Xscale || YScale)
		DrawButton(memDC, scaleButtonRect,scaleClientRect, clientRect, L"S", defaultTextColor, axisColor);

	//рисуем
	if (fType == RITHMIC) ::AcquireSRWLockShared(&SRWLockRithmic);
	//if (fType == NINJATRADER) ::AcquireSRWLockShared(&srwLockNinja);

	if (tradesOnly)
    {
		Draw1(memDC);
    }
	else 
    {
		Draw3(memDC);
    }

	if (fType == RITHMIC) ::ReleaseSRWLockShared(&SRWLockRithmic);
	//if (fType == NINJATRADER) ::ReleaseSRWLockShared(&srwLockNinja);

	//если идет процесс закачки истории, то затемняем экран и отображаем надпись
	if ((subInstrumentPtr == NULL && instrumentPtr->GetHistoryFlag() == 1) || (subInstrumentPtr != NULL && subInstrumentPtr->GetHistoryFlag() == 1))
		DrawHistoryLoad(memDC, clientRect);
	
	HDC stretchDC = ::CreateCompatibleDC(dc);
	HBITMAP stretchBitmap = ::CreateCompatibleBitmap(dc, clientRect.right, clientRect.bottom);
	::SelectObject(stretchDC, stretchBitmap);

	//копируем ценовую шкалу вместе с plotZoneAdd
	::BitBlt(stretchDC, plotZone.right, 0, clientRect.right, clientRect.bottom, 
		memDC, scaleClientRect.right - price_axis_width - (plotZoneAdd.right-plotZoneAdd.left), 0, SRCCOPY);
	
	if (!Xscale)
		//копируем только plotZone
		::BitBlt(stretchDC, 0, 0, plotZone.right, clientRect.bottom, memDC, 0, 0, SRCCOPY);
	else
	{	
		//натягиваем область плот
		::SetStretchBltMode(stretchDC, /*STRETCH_ORSCANS*//*STRETCH_DELETESCANS*/STRETCH_HALFTONE); 
		::StretchBlt(stretchDC, 0, 0, plotZone.right, clientRect.bottom, memDC, 0, 0, scalePlotZone.right, scaleClientRect.bottom, SRCCOPY);
	}

	//дорисовываем ось X
	::SetDCPenColor(stretchDC, axisColor);
	::SelectObject(stretchDC, (HPEN)::GetStockObject(DC_PEN));
	::MoveToEx(stretchDC, 0, plotZone.bottom, NULL); ::LineTo(stretchDC, clientRect.right-price_axis_width, plotZone.bottom);

	if (mouseOnMoveZone1)
	{
		::SetDCBrushColor(stretchDC, RGB(255-GetRValue(defaultbackgroundColor),255-GetGValue(defaultbackgroundColor),255-GetBValue(defaultbackgroundColor)));
		::FillRect(stretchDC, &plotMoveZone1, (HBRUSH)::GetStockObject(DC_BRUSH));
	}	

	////копирайт
	//DrawCopyright(stretchDC, clientRect);
	////копирайт ритмика
	//if (fType == RITHMIC)
	//	DrawRithmicCopyright(stretchDC, clientRect);

	::BitBlt(dc, 0, 0, clientRect.right, clientRect.bottom, stretchDC, 0, 0, SRCCOPY);

	::DeleteObject(stretchBitmap);
	::DeleteDC(stretchDC);
	::DeleteObject(memBitmap);
	::DeleteDC(memDC);
}
void HeatMap::Draw1(HDC dc)
{
	//получаем индексы начала и конца диапазона
	size_t start, end;
	
	if (GetRangeIndexes(m_hWnd, instrumentPtr, start, end, scalePlotZone))
		return;

	//ищем максимальные и минимальные цены данного диапазона и смасштабированные макс и мин цены
	Price maxPrice((int)INT_MAX, (UINT)0, true),
			minPrice((int)INT_MAX, (UINT)0),
				scaleMaxPrice, scaleMinPrice, lastPrice;
	
	FindMaxMinPricesForTrades(dc, start, end, maxPrice, minPrice, lastPrice);

	if (scale_type != STATIC_PRICE_RANGE)
	{
		scaleMaxPrice =  maxPrice;
		scaleMinPrice = minPrice;
	}
	else
	{
		scaleMaxPrice =  topPriceRange;
		scaleMinPrice = bottomPriceRange;
	}

	//вертикальное масштабирование
	VerticScaleTrade(scaleMaxPrice, scaleMinPrice, lastPrice);

	Price p1 = (maxPrice <= scaleMaxPrice) ? maxPrice : scaleMaxPrice,
		p2 = (minPrice >= scaleMinPrice) ? minPrice : scaleMinPrice;

	//количество пикселов между ценами
	double count = (double)scalePlotZone.bottom / instrumentPtr->PriceDifference(scaleMaxPrice, scaleMinPrice);
	if (!count) count = 1;
	//расположение последнего трейда по оси Y
	YlastTrade = instrumentPtr->PriceDifference(scaleMaxPrice, lastPrice) * count + 1;

	//рисуем горизонтальные оси с метками и надписями 
	SIZE sSize = DrawHorizAxis(dc, scaleMaxPrice, count);

	//рисуем метку трейда
	if (tradesLabels)
	{
		::SetDCPenColor(dc, tradeColor);
		::SetDCBrushColor(dc, tradeLabelColor);
		::SetBkColor(dc, tradeLabelColor);
		int xCoord = scaleClientRect.right - price_axis_width/2 + 5,
			yCoord = (double)instrumentPtr->PriceDifference(scaleMaxPrice, lastPrice) * count + sSize.cy/2;		
		//RECT r = { xCoord - sSize.cx/2 -3, yCoord - sSize.cy/2 -3, xCoord + sSize.cx/2 -1, yCoord + sSize.cy/2 -4 };
		//::ExtTextOut(dc, xCoord, yCoord, ETO_OPAQUE, &r, k.GetWString(_i->GetPrecision()), _tcslen(k.GetWString(_i->GetPrecision())), NULL);
		::Rectangle(dc, xCoord - sSize.cx/2 -3, yCoord - sSize.cy, 
						xCoord + sSize.cx/2 +2, yCoord);
		::TextOut(dc, xCoord, yCoord, lastPrice.GetWString(instrumentPtr->GetPrecision()), (int)_tcslen(lastPrice.GetWString(instrumentPtr->GetPrecision())));
	}

	//для каждой найденной уникальной цены заполняем массив значений и рисуем его
	DrawTrades(dc, start, end, scaleMaxPrice, p1, p2, count);
}
void HeatMap::DrawTrades(HDC dc, const size_t start, const size_t end, const Price& scaleMaxPrice, Price& p1, Price& p2, const double count)
{
	double bar;
	int bidTradeFromX = 0, bidTradeToX = 0, askTradeFromX = 0, askTradeToX = 0, inspredTradeFromX = 0, inspredTradeToX = 0;

	for (size_t j = 0, y = start; y <= end; j++, y++)
	{
		for (Price k = p2; k <= p1; instrumentPtr->IncPrice(k))
		{
			if (k == instrumentPtr->Get_tradePrice(y))
			{
				//трейд по биду
				if (instrumentPtr->Get_agressorSide(y) == 'S' || instrumentPtr->Get_impliedSide(y) == 'S')
				{
					if (instrumentPtr->Get_agressorSide(y) == '?') ::SetDCPenColor(dc, noagressorColor);
					else ::SetDCPenColor(dc, sellTradeColor);
					bar = count/2 - 1;
					if (bar < 1.) ++bar;
					//if (bar > (count - 1))	bar = count - 1;	//не понимаю когда размер этой переменной будет больше чем count-1, этого вообще не должно быть
					bidTradeFromX = ((double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count) - 1;
					::MoveToEx(dc, (int)j, bidTradeFromX, NULL);
					bidTradeToX = ((double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count - bar) - 1;
					LineTo(dc, (int)j, bidTradeToX);
				}
				//трейд по аску
				else if (instrumentPtr->Get_agressorSide(y) == 'B' || instrumentPtr->Get_impliedSide(y) == 'B')
				{
					if (instrumentPtr->Get_agressorSide(y) == '?') ::SetDCPenColor(dc, noagressorColor);
					else ::SetDCPenColor(dc, buyTradeColor);
					bar = count/2 - 1;
					if (bar < 1.) ++bar;
					askTradeFromX = ((double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count) + 1;
					::MoveToEx(dc, (int)j, askTradeFromX, NULL);
					askTradeToX = ((double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count + bar) + 1;
					if (askTradeToX <= scalePlotZone.bottom)
						LineTo(dc, (int)j, askTradeToX);
				}
				//трейд внутри спрэда
				else if (instrumentPtr->Get_agressorSide(y) == '?' || instrumentPtr->Get_impliedSide(y) == 'M')
				{
					::SetDCPenColor(dc, noagressorColor);
					bar = count/2 - 1;
					if (bar < 1.) ++bar;
					inspredTradeFromX = ((double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count);
					::MoveToEx(dc, (int)j, inspredTradeFromX, NULL);
					inspredTradeToX = ((double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count + bar) + 1;
					if (inspredTradeToX <= scalePlotZone.bottom)
						LineTo(dc, (int)j, inspredTradeToX);
				}
			}
		}
	}
}
void HeatMap::Draw3(HDC dc)
{
	//получаем индексы начала и конца диапазона
	size_t start, end;

	if (GetRangeIndexes(m_hWnd, instrumentPtr, start, end, scalePlotZone))
		return;

	//ищем максимальные и минимальные цены данного диапазона и смасштабированные макс и мин цены
	Price maxPrice((int)INT_MAX, (UINT)0, true),
		  minPrice((int)INT_MAX, (UINT)0),
		  scaleMaxPrice, scaleMinPrice, lastTrade, lastBidPrice, lastAskPrice;
	
	if (FindMaxMinPricesForDOM(dc, start, end, maxPrice, minPrice, lastTrade, lastBidPrice, lastAskPrice))
		return;	

	//вертикальное масштабирование
	if (scale_type != STATIC_PRICE_RANGE)
	{
		scaleMaxPrice =  maxPrice;
		scaleMinPrice = minPrice;
	}
	else
	{
		scaleMaxPrice =  topPriceRange;
		scaleMinPrice = bottomPriceRange;
	}

	//вертикальное масштабирование
	VerticScaleDOM(scaleMaxPrice, scaleMinPrice, lastAskPrice, lastBidPrice);
	
	//ищем максимальное значение объема для масштабирования
	Price p1 = (maxPrice <= scaleMaxPrice) ? maxPrice : scaleMaxPrice,
		  p2 = (minPrice >= scaleMinPrice) ? minPrice : scaleMinPrice;
	UINT maxDomValue = 0, minDomValue = UINT_MAX;
	if (bEventAutoScale == 1)	//visible range
	{
		FindMaxDomValue(start, end, p1, p2, maxPrice, scaleMaxPrice, maxDomValue, minDomValue);
	}
	else if (bEventAutoScale == 2) //manual range
	{
		maxDomValue = uManualScaleMaxValue;
		minDomValue = uManualScaleMinValue;
	}

	//количество пикселов между ценами
	double count = (double)plotZone.bottom / instrumentPtr->PriceDifference(scaleMaxPrice, scaleMinPrice);
	if (!count) count = 1;
	YlastAsk = (double)instrumentPtr->PriceDifference(scaleMaxPrice, instrumentPtr->Get_askPrice(end, 0)) * count + 1;
	YlastBid = (double)instrumentPtr->PriceDifference(scaleMaxPrice, instrumentPtr->Get_bidPrice(end, 0)) * count - 1;

	//рисуем горизонтальные оси с метками и надписями
	SIZE sSize = DrawHorizAxis(dc, scaleMaxPrice, count);

	//метка трейда
	if (tradesLabels && lastTrade != INT_MAX) //если в текущем диапазоне был вообще хоть один трейд
	{
		//SIZE sSize; 
		//::GetTextExtentPoint32(dc, lastTrade.GetWString(_i->GetPrecision()), (int)_tcslen(lastTrade.GetWString(_i->GetPrecision())), &sSize);
		
		::SetDCPenColor(dc, tradeColor);
		::SetDCBrushColor(dc, tradeLabelColor);
		
		int xCoord = scaleClientRect.right - price_axis_width/2 + 5,
			yCoord = (double)instrumentPtr->PriceDifference(scaleMaxPrice, lastTrade) * count + sSize.cy/2;	
		::Rectangle(dc, xCoord - sSize.cx/2 -3, yCoord - sSize.cy, xCoord + sSize.cx/2 +2, yCoord);
		::TextOut(dc, xCoord, yCoord, lastTrade.GetWString(instrumentPtr->GetPrecision()), (int)_tcslen(lastTrade.GetWString(instrumentPtr->GetPrecision())));
	}

	//для каждой найденной уникальной цены рисуем
	p1 = (maxPrice <= scaleMaxPrice) ? maxPrice : scaleMaxPrice;
	DrawDOM(dc, start, end, scaleMaxPrice, p1, p2, maxDomValue, minDomValue, count);
}
void HeatMap::DrawDOM(HDC dc, const size_t start, const size_t end, const Price& scaleMaxPrice, Price& p1, Price& p2, UINT& maxDomValue, UINT& minDomValue, const double count)
{
	double bar;
	int bidTradeFromX = 0, bidTradeToX = 0, askTradeFromX = 0, askTradeToX = 0, inspredTradeFromX = 0, inspredTradeToX = 0, limitsToX = 0;
	
	for (size_t j = 0, y = start; y <= end; j++, y++)
	{
		for (Price k = p2; k <= p1; instrumentPtr->IncPrice(k))
		{							
			for (int x = instrumentPtr->GetDomLevelsCount()-1; x >= 0; x--)	//все уровни в стакане
			{
				if (bEventAutoScale == 0)	//only last value
				{
					if (!showOrders)
					{
						maxDomValue = instrumentPtr->Get_maxDomLimit(y);
						minDomValue = instrumentPtr->Get_minDomLimit(y);
					}
					else
					{
						maxDomValue = instrumentPtr->Get_maxDomOrder(y);
						minDomValue = instrumentPtr->Get_minDomOrder(y);
					}
					if (maxDomValue == minDomValue || minDomValue == UINT_MAX)
						goto DRAWTRADES;
				}

				//бид лимиты
				if (!showOrders && k == instrumentPtr->Get_bidPrice(y, x) && instrumentPtr->Get_bidSize(y, x))
				{
					switch (iColorTheme)
					{
					case COLOR:
						{
							int colorIndx = (double)(instrumentPtr->Get_bidSize(y, x) - minDomValue) / (maxDomValue - minDomValue) * 1019;
							if (colorIndx > 1019) colorIndx = 1019;
							else if (colorIndx < 0) colorIndx = 0; //когда текущее значение бида меньше установленного вручную minDomValue
							::SetDCPenColor(dc, colors[colorIndx]);	
						}
						break;
					case MONOCHROME:
						{
							int deltaLimitSize = instrumentPtr->Get_deltaBidLimitSize(y,x);
							if (deltaLimitSize > 0)
								::SetDCPenColor(dc, iAddSizeColor);
							else if (deltaLimitSize < 0)
								::SetDCPenColor(dc, iRemoveSizeColor);
							else
							{
								int colorIndx = (double)(instrumentPtr->Get_bidSize(y, x) - minDomValue) / (maxDomValue - minDomValue) * LAST_MONO_INDEX;
								if (colorIndx > LAST_MONO_INDEX) colorIndx = LAST_MONO_INDEX;
								else if (colorIndx < 0) colorIndx = 0; //когда текущее значение бида меньше установленного вручную minDomValue
								::SetDCPenColor(dc, monochrome[colorIndx]);	
							}
						}
						break;
					}
					bar = count - 1;
					int moveTo = (double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count + count/2 - 1;
					if (moveTo <= plotZone.bottom)
						::MoveToEx(dc, (int)j, moveTo, NULL);
					else 
						::MoveToEx(dc, (int)j, plotZone.bottom-1, NULL);
					limitsToX = (double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count - bar + count/2 - 1;
					if (bidTradeToX == limitsToX)
						++limitsToX;
					LineTo(dc, (int)j, limitsToX);

					//метки
					if (limitsLabels && y == end)
						DrawDOMLabel(dc, instrumentPtr->Get_bidSize(end, x), newBidLimitColor, bidLimitColor, limitsToX + count/2);
				}

				//бид ордера 
				if (showOrders && k == instrumentPtr->Get_bidPrice(y, x) && instrumentPtr->Get_bidOrders(y, x))
				{
					switch (iColorTheme)
					{
					case COLOR:
						{
							int colorIndx = (double)(instrumentPtr->Get_bidOrders(y, x) - minDomValue) / (maxDomValue - minDomValue) * 1019;
							if (colorIndx > 1019) colorIndx = 1019;
							else if (colorIndx < 0) colorIndx = 0;
							::SetDCPenColor(dc, colors[colorIndx]);	
						}
						break;
					case MONOCHROME:
						{
							int deltaLimitSize = instrumentPtr->Get_deltaBidLimitSize(y,x);
							if (deltaLimitSize > 0)
								::SetDCPenColor(dc, iAddSizeColor);
							else if (deltaLimitSize < 0)
								::SetDCPenColor(dc, iRemoveSizeColor);
							else
							{
								int colorIndx = (double)(instrumentPtr->Get_bidOrders(y, x) - minDomValue) / (maxDomValue - minDomValue) * LAST_MONO_INDEX;
								if (colorIndx > LAST_MONO_INDEX) colorIndx = LAST_MONO_INDEX;
								else if (colorIndx < 0) colorIndx = 0;
								::SetDCPenColor(dc, monochrome[colorIndx]);
							}
						}
						break;
					}					
					bar = count - 1;
					int moveTo = (double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count + count/2 - 1;
					if (moveTo <= plotZone.bottom)
						::MoveToEx(dc, (int)j, moveTo, NULL);
					else 
						::MoveToEx(dc, (int)j, plotZone.bottom-1, NULL);
					limitsToX = (double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count - bar + count/2 - 1;
					LineTo(dc, (int)j, limitsToX);

					//метка
					if (ordersLabels && y == end)
						DrawDOMLabel(dc, instrumentPtr->Get_bidOrders(end, x), newBidOrdersColor, bidOrdersColor, limitsToX + count/2);
				}
			
				//аск лимиты
				if (!showOrders && k == instrumentPtr->Get_askPrice(y, x) && instrumentPtr->Get_askSize(y, x))
				{
					switch (iColorTheme)
					{
					case COLOR:
						{
							int colorIndx = (double)(instrumentPtr->Get_askSize(y, x) - minDomValue) / (maxDomValue - minDomValue) * 1019;
							if (colorIndx > 1019) colorIndx = 1019;
							else if (colorIndx < 0) colorIndx = 0;
							::SetDCPenColor(dc, colors[colorIndx]);	
						}
						break;
					case MONOCHROME:
						{
							int deltaLimitSize = instrumentPtr->Get_deltaAskLimitSize(y,x);
							if (deltaLimitSize > 0)
								::SetDCPenColor(dc, iAddSizeColor);
							else if (deltaLimitSize < 0)
								::SetDCPenColor(dc, iRemoveSizeColor);
							else
							{
								int colorIndx = (double)(instrumentPtr->Get_askSize(y, x) - minDomValue) / (maxDomValue - minDomValue) * LAST_MONO_INDEX;
								if (colorIndx > LAST_MONO_INDEX) colorIndx = LAST_MONO_INDEX;
								else if (colorIndx < 0) colorIndx = 0;
								::SetDCPenColor(dc, monochrome[colorIndx]);	
							}
						}
						break;
					}
					bar = count - 1;
					::MoveToEx(dc, (int)j, (double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count - count/2 + 1, NULL);
					limitsToX = (double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count + bar - count/2 + 1;
					//if (limitsToX <= plotZone.bottom)
						LineTo(dc, (int)j, limitsToX);

					//метки
					if (limitsLabels && y == end)
						DrawDOMLabel(dc, instrumentPtr->Get_askSize(end, x), newAskLimitColor, askLimitColor, limitsToX - count/2);
				}				

				//аск ордера 
				if (showOrders && k == instrumentPtr->Get_askPrice(y, x) && instrumentPtr->Get_askOrders(y, x))
				{
					switch (iColorTheme)
					{
					case COLOR:
						{
							int colorIndx = (double)(instrumentPtr->Get_askOrders(y, x) - minDomValue) / (maxDomValue - minDomValue) * 1019;
							if (colorIndx > 1019) colorIndx = 1019;
							else if (colorIndx < 0) colorIndx = 0;
							::SetDCPenColor(dc, colors[colorIndx]);		
						}
						break;
					case MONOCHROME:
						{
							int deltaLimitSize = instrumentPtr->Get_deltaAskLimitSize(y,x);
							if (deltaLimitSize > 0)
								::SetDCPenColor(dc, iAddSizeColor);
							else if (deltaLimitSize < 0)
								::SetDCPenColor(dc, iRemoveSizeColor);
							else
							{
								int colorIndx = (double)(instrumentPtr->Get_askOrders(y, x) - minDomValue) / (maxDomValue - minDomValue) * LAST_MONO_INDEX;
								if (colorIndx > LAST_MONO_INDEX) colorIndx = LAST_MONO_INDEX;
								else if (colorIndx < 0) colorIndx = 0;
								::SetDCPenColor(dc, monochrome[colorIndx]);	
							}
						}
						break;
					}
					bar = count - 1;
					::MoveToEx(dc, (int)j, (double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count - count/2 + 1, NULL);
					limitsToX = (double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count + bar - count/2 + 1;
					//if (limitsToX <= plotZone.bottom)
						LineTo(dc, (int)j, limitsToX);

					//метка
					if (ordersLabels && y == end)
						DrawDOMLabel(dc, instrumentPtr->Get_askOrders(end, x), newAskOrdersColor, askOrdersColor, limitsToX - count/2);
				}
DRAWTRADES:

				if (k == instrumentPtr->Get_tradePrice(y))
				{
					//трейд по биду
					if (instrumentPtr->Get_agressorSide(y) == 'S' || instrumentPtr->Get_impliedSide(y) == 'S')
					{
						if (instrumentPtr->Get_agressorSide(y) == '?') ::SetDCPenColor(dc, noagressorColor);
						else ::SetDCPenColor(dc, sellTradeColor);
						bar = count - 1;
						if (bar < 1) ++bar;
						bidTradeFromX = (double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count + count/2 - 1;
						::MoveToEx(dc, (int)j, bidTradeFromX, NULL);
						bidTradeToX = (double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count - bar + count/2 - 1;
						LineTo(dc, (int)j, bidTradeToX);
					}
					//трейд по аску
					else if (instrumentPtr->Get_agressorSide(y) == 'B' || instrumentPtr->Get_impliedSide(y) == 'B')
					{
						if (instrumentPtr->Get_agressorSide(y) == '?') ::SetDCPenColor(dc, noagressorColor);
						else ::SetDCPenColor(dc, buyTradeColor);
						bar = count - 1;
						if (bar < 1) ++bar;
						askTradeFromX = (double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count - count/2 + 1;
						::MoveToEx(dc, (int)j, askTradeFromX, NULL);
						askTradeToX = (double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count + bar - count/2 + 1;
						if (askTradeToX <= plotZone.bottom)
							LineTo(dc, (int)j, askTradeToX);
					}
					//трейд внутри спрэда
					else if (instrumentPtr->Get_agressorSide(y) == '?' || instrumentPtr->Get_impliedSide(y) == 'M')
					{
						::SetDCPenColor(dc, noagressorColor);
						bar = (double)count/2 - 1;
						if (bar < 1.) ++bar;
						inspredTradeFromX = (double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count;
						::MoveToEx(dc, (int)j, inspredTradeFromX, NULL);
						inspredTradeToX = (double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count + bar + 1;
						LineTo(dc, (int)j, inspredTradeToX);
					}
				}

				//горизонтальный стакан
				if (y == end)
					if (showHorizontalDOM && plotZoneAdd.left != plotZoneAdd.right && maxDomValue && minDomValue != UINT_MAX)
						DrawColorHorizDOM(dc, count, k, y, x, scaleMaxPrice, maxDomValue, minDomValue);

				//обнуляем макс и мин значения в стакане для новых записей
				if (bEventAutoScale == 0)
					maxDomValue = 0, minDomValue = UINT_MAX;
			}
		}
	}
}
void HeatMap::DrawColorHorizDOM(HDC dc, double count, Price& k, const size_t& y, const int& x, const Price& scaleMaxPrice, UINT maxDomValue, UINT minDomValue)
{
	double bar = 0;
	COLORREF color;
	int colorIndx;

	if (k == instrumentPtr->Get_askPrice(y, x) && instrumentPtr->Get_askSize(y, x))	//аски
	{
		
		switch (iColorTheme)
		{
		case COLOR:
			if (!showOrders)
				colorIndx = (double)(instrumentPtr->Get_askSize(y, x) - minDomValue) / (double)(maxDomValue - minDomValue) * 1019;
			else 
				colorIndx = (double)(instrumentPtr->Get_askOrders(y, x) - minDomValue) / (double)(maxDomValue - minDomValue) * 1019;
			if (colorIndx > 1019) colorIndx = 1019;
			else if (colorIndx < 0) colorIndx = 0;
			color = colors[colorIndx];
			::SetDCBrushColor(dc, color); ::SetDCPenColor(dc, color);
			break;
		case MONOCHROME:
			if (!showOrders)
				colorIndx = (double)(instrumentPtr->Get_askSize(y, x) - minDomValue) / (maxDomValue - minDomValue) * LAST_MONO_INDEX;
			else 
				colorIndx = (double)(instrumentPtr->Get_askOrders(y, x) - minDomValue) / (maxDomValue - minDomValue) * LAST_MONO_INDEX;
			if (colorIndx > LAST_MONO_INDEX) colorIndx = LAST_MONO_INDEX;
			else if (colorIndx < 0) colorIndx = 0;
			color = monochrome[colorIndx];
			::SetDCBrushColor(dc, color); ::SetDCPenColor(dc, color);
			break;
		}
			
		if (!showOrders)
			bar = (double)instrumentPtr->Get_askSize(y, x) / instrumentPtr->Get_maxDomLimit(y) * (plotZoneAdd.right - plotZoneAdd.left);
		else bar = (double)instrumentPtr->Get_askOrders(y, x) / instrumentPtr->Get_maxDomOrder(y) * (plotZoneAdd.right - plotZoneAdd.left);
	}
	else if (k == instrumentPtr->Get_bidPrice(y, x) && instrumentPtr->Get_bidSize(y, x))	//биды
	{
		switch (iColorTheme)
		{
		case COLOR:
			if (!showOrders)
				colorIndx = (double)(instrumentPtr->Get_bidSize(y, x) - minDomValue) / (maxDomValue - minDomValue) * 1019;
			else 
				colorIndx = (double)(instrumentPtr->Get_bidOrders(y, x) - minDomValue) / (maxDomValue - minDomValue) * 1019;
			if (colorIndx > 1019) colorIndx = 1019;
			else if (colorIndx < 0) colorIndx = 0;
			color = colors[colorIndx];
			::SetDCBrushColor(dc, color); ::SetDCPenColor(dc, color);
			break;
		case MONOCHROME:
			if (!showOrders)
				colorIndx = (double)(instrumentPtr->Get_bidSize(y, x) - minDomValue) / (maxDomValue - minDomValue) * LAST_MONO_INDEX;
			else 
				colorIndx = (double)(instrumentPtr->Get_bidOrders(y, x) - minDomValue) / (maxDomValue - minDomValue) * LAST_MONO_INDEX;
			if (colorIndx > LAST_MONO_INDEX) colorIndx = LAST_MONO_INDEX;
			else if (colorIndx < 0) colorIndx = 0;
			color = monochrome[colorIndx];
			::SetDCBrushColor(dc, color); ::SetDCPenColor(dc, color);
			break;
		}	

		if (!showOrders)
			bar = (double)instrumentPtr->Get_bidSize(y, x) / instrumentPtr->Get_maxDomLimit(y) * (plotZoneAdd.right - plotZoneAdd.left);
		else bar = (double)instrumentPtr->Get_bidOrders(y, x) / instrumentPtr->Get_maxDomOrder(y) * (plotZoneAdd.right - plotZoneAdd.left);
	}

	if (bar)
	{
		int priceDiff = instrumentPtr->PriceDifference(scaleMaxPrice, k);
		int Y1 = priceDiff * count + count/2;
		int Y2 = priceDiff * count - count/2 + 1;

		if (Y1 >= plotZone.bottom)
			Y1 = plotZone.bottom-1;

		::Rectangle(dc, scaleClientRect.right - price_axis_width - bar+1, Y1, scaleClientRect.right - price_axis_width, Y2);
	}
}
void HeatMap::FindMaxDomValue(const size_t start, const size_t end, Price& p1, Price& p2, Price& maxPrice, Price& scaleMaxPrice, UINT& maxDomValue, UINT& minDomValue)
{
	if (!showOrders)
	{
		for (size_t x = 0, y = start; y <= end; y++, x++)
		{
			while (p1 >= p2)
			{		
				for (int k = instrumentPtr->GetDomLevelsCount()-1; k >= 0; k--)	//смотрим все уровни в стакане
				{		
					if (p1 == instrumentPtr->Get_bidPrice(y, k) && maxDomValue < instrumentPtr->Get_bidSize(y, k))
						maxDomValue = instrumentPtr->Get_bidSize(y, k);
					if (p1 == instrumentPtr->Get_askPrice(y, k) && maxDomValue < instrumentPtr->Get_askSize(y, k))
						maxDomValue = instrumentPtr->Get_askSize(y, k);
						
					if (p1 == instrumentPtr->Get_askPrice(y, k) && minDomValue > instrumentPtr->Get_askSize(y, k))
						minDomValue = instrumentPtr->Get_askSize(y, k);
					if (p1 == instrumentPtr->Get_bidPrice(y, k) && minDomValue > instrumentPtr->Get_bidSize(y, k))
						minDomValue = instrumentPtr->Get_bidSize(y, k);
				}

				instrumentPtr->DecPrice(p1);
			}	
			p1 = (maxPrice <= scaleMaxPrice) ? maxPrice : scaleMaxPrice;
		}
	}
	else
	{
		for (size_t x = 0, y = start; y <= end; y++, x++)
		{
			while (p1 >= p2)
			{		
				for (int k = instrumentPtr->GetDomLevelsCount()-1; k >= 0; k--)
				{		
					if (p1 == instrumentPtr->Get_askPrice(y, k) && maxDomValue < instrumentPtr->Get_askOrders(y, k))
						maxDomValue = instrumentPtr->Get_askOrders(y, k);
					if (p1 == instrumentPtr->Get_bidPrice(y, k) && maxDomValue < instrumentPtr->Get_bidOrders(y, k))
						maxDomValue = instrumentPtr->Get_bidOrders(y, k);

					if (p1 == instrumentPtr->Get_askPrice(y, k) && minDomValue > instrumentPtr->Get_askOrders(y, k))
						minDomValue = instrumentPtr->Get_askOrders(y, k);
					if (p1 == instrumentPtr->Get_bidPrice(y, k) && minDomValue > instrumentPtr->Get_bidOrders(y, k))
						minDomValue = instrumentPtr->Get_bidOrders(y, k);
				}

				instrumentPtr->DecPrice(p1);
			}	
			p1 = (maxPrice <= scaleMaxPrice) ? maxPrice : scaleMaxPrice;
		}
	}
}

BOOL HeatMap::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (zDelta > 0)
	{
		++YscaleTop;
		++YscaleBottom;
	}
	else
	{
		--YscaleBottom;
		--YscaleTop;
	}

	if (scale_type == VISIBLE_RANGE)
	{
        /*if (_i->IsTradesOnly())
        {
        if (!YscaleTop) YscaleTop = 1;
        if (!YscaleBottom) YscaleBottom = 1;
        }*/
	}

	Invalidate(false);
	::UpdateWindow(m_hWnd);	

	return BaseChart::OnMouseWheel(nFlags, zDelta, pt);
}
void HeatMap::OnRButtonDown(UINT nFlags, CPoint point)
{
	POINT p;
    ::GetCursorPos(&p);
	CMenu menu;

	if (::PtInRect(&plotZone, point)) //область рисования
	{
		menu.LoadMenuW(MAKEINTRESOURCE(IDR_CONTEXTMENU_QUANTCHART));

		if (alwaysOnTop)
			menu.GetSubMenu(0)->CheckMenuItem(ID_QCMENU_ALWAYSONTOP, MF_CHECKED);
		if (showOrders)
			menu.GetSubMenu(0)->CheckMenuItem(ID_QCMENU_ORDERSSHOW, MF_CHECKED);	

		menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON, p.x, p.y, this);
	}
	else if (::PtInRect(&plotZoneAdd, point))	//область горизонтального стакана
	{
		menu.LoadMenuW(MAKEINTRESOURCE(IDR_MENU_PRICEZONEADD));
		
		if (showHorizontalDOM) 
			menu.GetSubMenu(0)->CheckMenuItem(ID_PRICEZONEADD_HORIZONTALDOM, MF_CHECKED);

		menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON, p.x, p.y, this);
	}
	else if (::PtInRect(&priceZone, point))	//область ценовой шкалы
	{
		menu.LoadMenuW(MAKEINTRESOURCE(IDR_CONTEXTMENU_QUANTCHARTLABELS));

        if (tradesLabels) menu.GetSubMenu(0)->CheckMenuItem(ID_QCMENU_TRADELABELS, MF_CHECKED);
        if (limitsLabels) menu.GetSubMenu(0)->CheckMenuItem(ID_QCMENU_LIMITSLABELS, MF_CHECKED);
        if (ordersLabels) menu.GetSubMenu(0)->CheckMenuItem(ID_QCMENU_ORDERSLABELS, MF_CHECKED);

		switch (scale_type)
		{
		case VISIBLE_RANGE:
			menu.GetSubMenu(0)->CheckMenuItem(ID_QCMENU_VISIBLERANGE, MF_CHECKED);
			break;
		case ONLY_LAST_VALUE:
			menu.GetSubMenu(0)->CheckMenuItem(ID_QCMENU_ONLYLASTVALUE, MF_CHECKED);
			break;
		case STATIC_PRICE_RANGE:
			menu.GetSubMenu(0)->CheckMenuItem(ID_QCMENU_CONSTANTRANGE, MF_CHECKED);
			break;
		}

		menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON, p.x, p.y, this);
	}	
	else if (::PtInRect(&timeZone, point))	//область временной шкалы
	{
		menu.LoadMenuW(MAKEINTRESOURCE(IDR_MENU_TIMEZONESCALE));
		
		menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON, p.x, p.y, this);
	}
	
	BaseChart::OnRButtonDown(nFlags, point);
}
//функция для затемнения меню "Show orders" для фидов где эта возможность отсутствует
void HeatMap::OnUpdateQcmenuOrdersshow(CCmdUI *pCmdUI)
{
    if (instrumentPtr->GetInstrumentINFO().ordersNumber)
        pCmdUI->Enable();
    else
        pCmdUI->Enable(0);
}
void HeatMap::ContextMenu(UINT nID)
{
	if (instrumentPtr != NULL)
	{
		switch (nID)
		{
		case ID_QCMENU_ALWAYSONTOP:
			if (!alwaysOnTop)
				SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);		//ставим always on top	
			else SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);	//убираем always on top
			alwaysOnTop = !alwaysOnTop;
			break;
		case ID_QCMENU_ORDERSSHOW:
			if (!showOrders && instrumentPtr->tInfo.ordersNumber)
			{
				showOrders = true;
				ordersLabels = true;
			}
			else 
			{
				showOrders = false;
				ordersLabels = false;
			}
			Invalidate(0);
			break;
		case ID_QCMENU_SETTINGS:
			{
				HM_Settings* qs = new HM_Settings(this);
				qs->DoModal();
				delete qs;
			}
			break;
		}
	}
}
void HeatMap::LabelsMenu(UINT nID)
{
	if (instrumentPtr != NULL)
	{
		switch (nID)
		{
		case ID_QCMENU_TRADELABELS:
			if (!tradesLabels)
				tradesLabels = true;
			else tradesLabels = false;
			break;
		case ID_QCMENU_LIMITSLABELS:
			if (!limitsLabels)
				limitsLabels = true;
			else limitsLabels = false;
			break;
		case ID_QCMENU_ORDERSLABELS:
			if (!ordersLabels)
				ordersLabels = true;
			else ordersLabels = false;
			break;
		case ID_QCMENU_RESETSCALE:
            YscaleTop = 0;
			YscaleBottom = 0;
			break;
		case ID_QCMENU_VISIBLERANGE:
			scale_type = VISIBLE_RANGE;
			YscaleTop = 0;
			YscaleBottom = 0;
			break;
		case ID_QCMENU_ONLYLASTVALUE:
			scale_type = ONLY_LAST_VALUE;
			break;
		case ID_QCMENU_CONSTANTRANGE:
			scale_type = STATIC_PRICE_RANGE;
			YscaleTop = 0;
			YscaleBottom = 0;
			break;
		}

		Invalidate(false);
	}
}
void HeatMap::PlotZoneAddMenu(UINT nID)
{
	if (instrumentPtr != NULL)
	{
		switch (nID)
		{
		case ID_PRICEZONEADD_HORIZONTALDOM:
			if (showHorizontalDOM)
				showHorizontalDOM = false;
			else showHorizontalDOM = true;
		break;
		}
		Invalidate(false);
	}
}
void HeatMap::TimeZoneMenu()
{
	Xscale = 0;
	RECT rt;
	::GetClientRect(m_hWnd, &rt);
	SetRects(rt.right, rt.bottom);
	SetScrollBar();
	Invalidate(0);
}
