#include "stdafx.h"
#include "BaseChart.h"
#include "Quant_Chart.h"
#include "Instrument.h"
#include "Plaza2Instruments.h"
#include "MainWindow.h"
#include "Price.h"
#include "resource.h"
#include "Auxiliary_methods.h"

//QC_Settings

BEGIN_MESSAGE_MAP(QuantChartSettings, CDialogEx)
	ON_BN_CLICKED(IDOK, &QuantChartSettings::OnBnClickedOk)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON15, &QuantChartSettings::OnBnClickedButton15)
	ON_BN_CLICKED(IDC_BUTTON17, &QuantChartSettings::OnBnClickedButton17)
	ON_BN_CLICKED(IDC_BUTTON19, &QuantChartSettings::OnBnClickedButton19)
	ON_BN_CLICKED(IDC_BUTTON21, &QuantChartSettings::OnBnClickedButton21)
	ON_BN_CLICKED(IDC_BUTTON_ASKTRADETEXT, &QuantChartSettings::OnBnClickedButtonAsktradetext)
	ON_BN_CLICKED(IDC_BUTTON_BIDTRADETEXT, &QuantChartSettings::OnBnClickedButtonBidtradetext)
	ON_BN_CLICKED(IDC_BUTTON5, &QuantChartSettings::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON33, &QuantChartSettings::OnBnClickedButton33)
	ON_BN_CLICKED(IDC_BUTTON7, &QuantChartSettings::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON9, &QuantChartSettings::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON11, &QuantChartSettings::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON13, &QuantChartSettings::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_BUTTON14, &QuantChartSettings::OnBnClickedButton14)
	ON_BN_CLICKED(IDC_BUTTON39, &QuantChartSettings::OnBnClickedButton39)
	ON_BN_CLICKED(IDC_BUTTON36, &QuantChartSettings::OnBnClickedButton36)
	ON_BN_CLICKED(IDC_BUTTON37, &QuantChartSettings::OnBnClickedButton37)
	ON_BN_CLICKED(IDC_BUTTON23, &QuantChartSettings::OnBnClickedButton23)
	ON_BN_CLICKED(IDC_BUTTON25, &QuantChartSettings::OnBnClickedButton25)
	ON_BN_CLICKED(IDC_BUTTON27, &QuantChartSettings::OnBnClickedButton27)
	ON_BN_CLICKED(IDC_BUTTON_OPENINTEREST_INCREASE_COLOR, &QuantChartSettings::OnBnClickedIOIncreaseColor)
	ON_BN_CLICKED(IDC_BUTTON_OPENINTEREST_DECREASE_COLOR, &QuantChartSettings::OnBnClickedIODecreaseColor)
	ON_BN_CLICKED(IDC_BUTTON_OPENINTEREST_NOTCHANGE_COLOR, &QuantChartSettings::OnBnClickedIONotChaneColor)
	ON_BN_CLICKED(IDC_BUTTON_FONT, &QuantChartSettings::OnBnClickedButtonFont)
	ON_UPDATE_COMMAND_UI(QUANT_CHART_EDIT_SCALE_VALUE, &QuantChartSettings::ManualScaleUpdate)
	ON_BN_CLICKED(QUANT_CHART_RADIO_MANUAL_SCALE, &QuantChartSettings::OnBnClickedChartRadioManualScale)
	ON_BN_CLICKED(QUANT_CHART_RADIO_AUTO_SCALE, &QuantChartSettings::OnBnClickedChartRadioAutoScale)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, &QuantChartSettings::OnDeltaposSpin)
	ON_BN_CLICKED(IDC_CHECK_SHOWGRIDLINES, &QuantChartSettings::OnBnClickedCheckShowgridlines)
	ON_BN_CLICKED(QUANT_CHART_CHECK_SHOWGRIDLINES, &QuantChartSettings::OnBnClickedChartCheckShowgridlines)
    ON_UPDATE_COMMAND_UI(IDC_RADIO_AGGREGATEDORDERS, &QuantChartSettings::OnUpdateRadioAggregatedorders)
    ON_UPDATE_COMMAND_UI(IDC_RADIO_SPLITTED_ORDERS, &QuantChartSettings::OnUpdateRadioSplittedorders)
END_MESSAGE_MAP()

BOOL QuantChartSettings::PreTranslateMessage(MSG* pMsg)
{
	UpdateDialogControls(this, FALSE);
	return CDialogEx::PreTranslateMessage(pMsg);
}

void QuantChartSettings::ManualScaleUpdate(CCmdUI *pCmdUI)
{
	if (IsDlgButtonChecked(QUANT_CHART_RADIO_AUTO_SCALE))
		pCmdUI->Enable(0);
	else
		pCmdUI->Enable(1);
}

void QuantChartSettings::OnBnClickedChartRadioManualScale()
{
	parentWindowPtr->bEventAutoScale = 1;
	parentWindowPtr->Invalidate(0);
}

void QuantChartSettings::OnBnClickedChartRadioAutoScale()
{
	parentWindowPtr->bEventAutoScale = 0;
	parentWindowPtr->Invalidate(0);
}

void QuantChartSettings::OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	UINT uNum;
	if (pNMUpDown->iDelta > 0)	// -
	{
		uNum =	GetDlgItemInt(QUANT_CHART_EDIT_SCALE_VALUE);
		if (uNum != 1)
			--uNum;
		SetDlgItemInt(QUANT_CHART_EDIT_SCALE_VALUE, uNum);
	}
	else						// +
	{
		uNum =	GetDlgItemInt(QUANT_CHART_EDIT_SCALE_VALUE);
		if (uNum != 9999)
			++uNum;
		SetDlgItemInt(QUANT_CHART_EDIT_SCALE_VALUE, uNum);
	}

	parentWindowPtr->uManualScaleMaxValue = uNum;

	parentWindowPtr->Invalidate(0);
}

void QuantChartSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_TRADESONLY, parentWindowPtr->tradesOnly);
	DDX_Check(pDX, QUANT_CHART_CHECK_SHOWGRIDLINES, parentWindowPtr->showGridLines);
	DDX_Text(pDX, IDC_EDIT_UPDATEFREQUENCY, parentWindowPtr->updateFrequency);
	DDV_MinMaxInt(pDX, parentWindowPtr->updateFrequency, 1, 5000);
	DDX_Text(pDX, QUANT_CHART_EDIT_SCALE_VALUE, parentWindowPtr->uManualScaleMaxValue);
	DDV_MinMaxUInt(pDX, parentWindowPtr->uManualScaleMaxValue, 1, 9999);
	DDX_Radio(pDX, QUANT_CHART_RADIO_AUTO_SCALE, parentWindowPtr->bEventAutoScale);
    DDX_Check(pDX, IDC_CHECK_OPEN_INTEREST_CHANGE_COLOR, parentWindowPtr->showOpenInterestChange);
    DDX_Check(pDX, IDC_RADIO_AGGREGATEDORDERS, parentWindowPtr->aggregatedOrders);
}

BOOL QuantChartSettings::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	LOGFONT log;
	TCHAR str[15];

	parentWindowPtr->qFont.GetLogFont(&log);
	StringCchPrintf(str, 15, L"%.10s", log.lfFaceName);
	GetDlgItem(IDC_BUTTON_FONT)->SetWindowText(str);

    if (parentWindowPtr->aggregatedOrders)
    {
        CheckRadioButton(IDC_RADIO_SPLITTED_ORDERS, IDC_RADIO_SPLITTED_ORDERS, IDC_RADIO_AGGREGATEDORDERS);
    }
    else
    {
        CheckRadioButton(IDC_RADIO_SPLITTED_ORDERS, IDC_RADIO_SPLITTED_ORDERS, IDC_RADIO_SPLITTED_ORDERS);
    }

	// делаем недоступными элементы, которые не имеют смысла в других датафидах
	switch (parentWindowPtr->fType)
	{
	case CQG:
	case NINJATRADER:
	case RITHMIC:
		GetDlgItem(IDC_CHECK_OPEN_INTEREST_CHANGE_COLOR)->EnableWindow(0);
		break;
	}

	return TRUE;
}

void QuantChartSettings::OnBnClickedOk()
{
	CDialogEx::OnOK();

	if (parentWindowPtr->subInstrumentPtr != NULL) 
	{
		delete parentWindowPtr->instrumentPtr;
		parentWindowPtr->instrumentPtr = parentWindowPtr->subInstrumentPtr;
		parentWindowPtr->subInstrumentPtr = NULL;
		parentWindowPtr->timerIndex = 0;
		parentWindowPtr->syncIndex = 0;
	}

	//если установлена галочка только трейды
	if (parentWindowPtr->tradesOnly)
	{		
		parentWindowPtr->subInstrumentPtr = parentWindowPtr->instrumentPtr;
		parentWindowPtr->timerIndex = 0;
		parentWindowPtr->instrumentPtr = (IInstrument*)parentWindowPtr->subInstrumentPtr->create_object();
		parentWindowPtr->instrumentPtr->CopyAllParameters(parentWindowPtr->subInstrumentPtr);
		parentWindowPtr->FillTradesOnlyVector();
	} 

	UpdateData();
	parentWindowPtr->KillTimer(1);
	parentWindowPtr->SetTimer(1, parentWindowPtr->updateFrequency, 0);

	parentWindowPtr->Invalidate(0);
}

HBRUSH QuantChartSettings::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	
	switch (pWnd->GetDlgCtrlID())
	{
	case IDC_STATIC_BACKGROUNDCOLOR:				pDC->SetTextColor(parentWindowPtr->defaultbackgroundColor); break;
	case IDC_STATIC_DEFAULTTEXTCOLOR:				pDC->SetTextColor(parentWindowPtr->defaultTextColor); break;
	case IDC_STATIC_PLOTBACKGROUNDCOLOR:			pDC->SetTextColor(parentWindowPtr->plotBackgoundColor); break;
	case IDC_STATIC_AXISCOLOR:						pDC->SetTextColor(parentWindowPtr->axisColor); break;
	case IDC_STATIC_BUYTRADECOLOR:					pDC->SetTextColor(parentWindowPtr->buyTradeColor); break;
	case IDC_STATIC_SELLTRADECOLOR:					pDC->SetTextColor(parentWindowPtr->sellTradeColor); break;
	case IDC_STATIC_IMPLIEDTRADECOLOR:				pDC->SetTextColor(parentWindowPtr->noagressorColor); break;
	case IDC_STATIC_ASKLIMIT:						pDC->SetTextColor(parentWindowPtr->askLimitColor);	break;
	case IDC_STATIC_NEWASKLIMIT:					pDC->SetTextColor(parentWindowPtr->newAskLimitColor);	break;
	case IDC_STATIC_BIDLIMIT:						pDC->SetTextColor(parentWindowPtr->bidLimitColor);	break;
	case IDC_STATIC_NEWBIDLIMIT:					pDC->SetTextColor(parentWindowPtr->newBidLimitColor);	break;
	case IDC_STATIC_ASKORDER:						pDC->SetTextColor(parentWindowPtr->askOrdersColor); break;
	case IDC_STATIC_NEWASKORDER:					pDC->SetTextColor(parentWindowPtr->newAskOrdersColor); break;
	case IDC_STATIC_BIDORDER:						pDC->SetTextColor(parentWindowPtr->bidOrdersColor); break;
	case IDC_STATIC_NEWBIDORDER:					pDC->SetTextColor(parentWindowPtr->newBidOrdersColor); break;
	case IDC_STATIC_TRADESIZEALERTCOLOR:			pDC->SetTextColor(parentWindowPtr->tradeSizeAlertColor); break;
	case IDC_STATIC_LIMITADDALERTCOLOR:				pDC->SetTextColor(parentWindowPtr->limitAddAlertColor); break;
	case IDC_STATIC_LIMITREMOVEALERTCOLOR:			pDC->SetTextColor(parentWindowPtr->limitRemoveAlertColor); break;
	case IDC_STATIC_GRIDLINESCOLOR:					pDC->SetTextColor(parentWindowPtr->gridColor); break;	
    case IDC_STATIC_OPENINTEREST_INCREASE_COLOR:    pDC->SetTextColor(parentWindowPtr->OIIncreaseBackground); break;
    case IDC_STATIC_OPENINTEREST_DECREASE_COLOR:    pDC->SetTextColor(parentWindowPtr->OIDecreaseBackground); break;
    case IDC_STATIC_OPENINTEREST_NOTCHANGE_COLOR:   pDC->SetTextColor(parentWindowPtr->OINotChangeBackground); break;
	case IDC_STATIC:
		//SetTextColor(pDC->m_hDC, RGB(255,255,255));
		//SetBkColor( pDC->m_hDC, BGRNCOLOR);
		//return (HBRUSH)(backgroundBrush.GetSafeHandle());	 
		break;
	}	
	
	return hbr;
}

void QuantChartSettings::OnBnClickedChartCheckShowgridlines()
{
	parentWindowPtr->showGridLines = IsDlgButtonChecked(QUANT_CHART_CHECK_SHOWGRIDLINES);
	parentWindowPtr->Invalidate(0);
}
void QuantChartSettings::OnBnClickedCheckShowgridlines()
{
	parentWindowPtr->showGridLines = IsDlgButtonChecked(QUANT_CHART_CHECK_SHOWGRIDLINES);
	parentWindowPtr->Invalidate(0);
}
void QuantChartSettings::OnBnClickedButton15()
{
	CColorDialog clrDlg(parentWindowPtr->defaultbackgroundColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->defaultbackgroundColor = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuantChartSettings::OnBnClickedButton17()
{
	CColorDialog clrDlg(parentWindowPtr->plotBackgoundColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->plotBackgoundColor = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuantChartSettings::OnBnClickedButton19()
{
	CColorDialog clrDlg(parentWindowPtr->defaultTextColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->defaultTextColor = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuantChartSettings::OnBnClickedButton21()
{
	CColorDialog clrDlg(parentWindowPtr->axisColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->axisColor = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuantChartSettings::OnBnClickedButtonAsktradetext()
{
	CColorDialog clrDlg(parentWindowPtr->buyTradeColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->buyTradeColor = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuantChartSettings::OnBnClickedButtonBidtradetext()
{
	CColorDialog clrDlg(parentWindowPtr->sellTradeColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->sellTradeColor = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuantChartSettings::OnBnClickedButton5()
{
	CColorDialog clrDlg(parentWindowPtr->noagressorColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->noagressorColor = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuantChartSettings::OnBnClickedButton33()
{
	CColorDialog clrDlg(parentWindowPtr->gridColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->gridColor = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuantChartSettings::OnBnClickedButton7()
{
	CColorDialog clrDlg(parentWindowPtr->askLimitColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->askLimitColor = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuantChartSettings::OnBnClickedButton9()
{
	CColorDialog clrDlg(parentWindowPtr->newAskLimitColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->newAskLimitColor = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuantChartSettings::OnBnClickedButton11()
{
	CColorDialog clrDlg(parentWindowPtr->bidLimitColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->bidLimitColor = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuantChartSettings::OnBnClickedButton13()
{
	CColorDialog clrDlg(parentWindowPtr->newBidLimitColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->newBidLimitColor = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuantChartSettings::OnBnClickedButton14()
{
	CColorDialog clrDlg(parentWindowPtr->askOrdersColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->askOrdersColor = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuantChartSettings::OnBnClickedButton39()
{
	CColorDialog clrDlg(parentWindowPtr->newAskOrdersColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->newAskOrdersColor = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuantChartSettings::OnBnClickedButton36()
{
	CColorDialog clrDlg(parentWindowPtr->bidOrdersColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->bidOrdersColor = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuantChartSettings::OnBnClickedButton37()
{
	CColorDialog clrDlg(parentWindowPtr->newBidOrdersColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->newBidOrdersColor = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuantChartSettings::OnBnClickedButton23()
{
	CColorDialog clrDlg(parentWindowPtr->tradeSizeAlertColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->tradeSizeAlertColor = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuantChartSettings::OnBnClickedButton25()
{
	CColorDialog clrDlg(parentWindowPtr->limitAddAlertColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->limitAddAlertColor = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuantChartSettings::OnBnClickedButton27()
{
	CColorDialog clrDlg(parentWindowPtr->limitRemoveAlertColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->limitRemoveAlertColor = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuantChartSettings::OnBnClickedIOIncreaseColor()
{
	CColorDialog clrDlg(parentWindowPtr->OIIncreaseBackground, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->OIIncreaseBackground = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuantChartSettings::OnBnClickedIODecreaseColor()
{
	CColorDialog clrDlg(parentWindowPtr->OIDecreaseBackground, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->OIDecreaseBackground = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuantChartSettings::OnBnClickedIONotChaneColor()
{
	CColorDialog clrDlg(parentWindowPtr->OINotChangeBackground, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->OINotChangeBackground = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuantChartSettings::OnBnClickedButtonFont()
{
	LOGFONT log; 
	
	parentWindowPtr->qFont.GetLogFont(&log);
	
	CFontDialog fontDlg(&log, CF_EFFECTS|CF_SCREENFONTS, NULL, this);

	if (fontDlg.DoModal() == IDOK) {
		TCHAR str[15];
		//static CFont font; 

		fontDlg.GetCurrentFont(&log);
		StringCchPrintf(str, 15, L"%.10s", log.lfFaceName);
		GetDlgItem(IDC_BUTTON_FONT)->SetWindowText(str);

		parentWindowPtr->qFont.DeleteObject();
		parentWindowPtr->qFont.CreateFontIndirect(&log);
		
		parentWindowPtr->GetPriceAxisWidth();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}

	/*size_t i = 0;
	while (!dWnd->_i->Get_tradePrice(i).IsPrice())
		++i;
	HDC dc = ::GetDC(dWnd->m_hWnd);
	::SelectObject(dc, (HFONT)dWnd->qFont);
	TCHAR str[64];
	SIZE sSize;
	StringCchPrintf(str, 64, L"%s", dWnd->_i->Get_tradePrice(i).GetWString(dWnd->_i->GetPrecision()));
	::GetTextExtentPoint32W(dc, str, (int)_tcslen(str), &sSize);
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
void QuantChartSettings::OnUpdateRadioAggregatedorders(CCmdUI *pCmdUI)
{
    if (IsDlgButtonChecked(IDC_CHECK_TRADESONLY))
        pCmdUI->Enable(1);
    else
        pCmdUI->Enable(0);
}
void QuantChartSettings::OnUpdateRadioSplittedorders(CCmdUI *pCmdUI)
{
    if (IsDlgButtonChecked(IDC_CHECK_TRADESONLY))
        pCmdUI->Enable(1);
    else
        pCmdUI->Enable(0);
}



// *****************************QuantChart*****************************

BEGIN_MESSAGE_MAP(QuantChart, BaseChart)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_QCMENU_ALWAYSONTOP, ID_QCMENU_SETTINGS, ContextMenu)
	ON_COMMAND_RANGE(ID_QCMENU_TRADELABELS, ID_QCMENU_CONSTANTRANGE, LabelsMenu)
	ON_COMMAND_RANGE(ID_PRICEZONEADD_HORIZONTALDOM, ID_PRICEZONEADD_HORIZONTALDOM, PlotZoneAddMenu)
	ON_COMMAND(ID_TIMEZONE_RESECSCALE, TimeZoneMenu)
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_UPDATE_COMMAND_UI(ID_QCMENU_ORDERSSHOW, &QuantChart::OnUpdateQcmenuOrdersshow)
END_MESSAGE_MAP()

QuantChart::~QuantChart() 
{
	for (ptrdiff_t x = mWnd->quantChartWindows.size()-1; x >= 0; --x)
	{
		if (this == mWnd->quantChartWindows[x])
		{
			mWnd->quantChartWindows.erase(mWnd->quantChartWindows.begin()+x);
			break;
		}
	}

	/*if (fType == PLAZA2)
		delete _i;*/
}

int QuantChart::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	//указываем в заголовке окна название инструмента
	wchar_t title[100];
	StringCchPrintfW(title, 100, _T("Quant Chart | %S"), name.c_str());
	SetWindowTextW(title);

 	HICON m_hIconSmall = (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_QUANTCHART), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR); // Загрузить иконку 16х16 
 	HICON m_hIconBig = (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_QUANTCHART), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);	// Загрузить иконку 32x32 
	SetIcon(m_hIconSmall, false); 
	SetIcon(m_hIconBig, true);

	if (BaseChart::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void QuantChart::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	HDC memDC = ::CreateCompatibleDC(dc);
	HBITMAP memBitmap = ::CreateCompatibleBitmap(dc, scaleClientRect.right, scaleClientRect.bottom);
	::SelectObject(memDC, memBitmap);

	if (!instrumentPtr)	//если от инструмента за которым было закреплено окно отписались
	{
		//рисуем надпись
		DrawUnsubscribeString(memDC, clientRect, fType, name.c_str());

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
		//определяем ширину ценовоц шкалы
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

	::BitBlt(dc, 0, 0, clientRect.right, clientRect.bottom, stretchDC, 0, 0, SRCCOPY);

	::DeleteObject(stretchBitmap);
	::DeleteDC(stretchDC);
	::DeleteObject(memBitmap);
	::DeleteDC(memDC);
}
void QuantChart::Draw1(HDC dc)
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

	//ищем максимальное значение трейда для масштабирования
	Price p1 = (maxPrice <= scaleMaxPrice) ? maxPrice : scaleMaxPrice,
		  p2 = (minPrice >= scaleMinPrice) ? minPrice : scaleMinPrice;
	UINT maxDomValue = 0;
	if (!bEventAutoScale)	//проверяем автоматически искать максимальное значение в стакане или использовать вручную заданное значение (0 - автоматический масштаб)
	{
		for (size_t x = 0, y = start; y <= end; y++, x++)
		{
			for (Price k = p1; k >= p2; instrumentPtr->DecPrice(k))
			{	
				if (k == instrumentPtr->Get_tradePrice(y) && maxDomValue < instrumentPtr->Get_tradeSize(y))
					maxDomValue = instrumentPtr->Get_tradeSize(y);
			}	
		}
	}
	else
	{
		maxDomValue = uManualScaleMaxValue;
	}

	if (!maxDomValue)
		return;

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
		::TextOutW(dc, xCoord, yCoord, lastPrice.GetWString(instrumentPtr->GetPrecision()), (int)_tcslen(lastPrice.GetWString(instrumentPtr->GetPrecision())));
	}

	//для каждой найденной уникальной цены заполняем массив значений и рисуем его
	DrawTrades(dc, start, end, scaleMaxPrice, p1, p2, maxDomValue, count);
}
void QuantChart::DrawTrades(HDC dc, const size_t start, const size_t end, const Price& scaleMaxPrice, Price& p1, Price& p2, const double maxDomValue, const double count)
{
	double bar;
	int bidTradeFromX = 0, bidTradeToX = 0, askTradeFromX = 0, askTradeToX = 0, inspredTradeFromX = 0, inspredTradeToX = 0;
	for (size_t j = 0, y = start; y <= end; j++, y++)
	{
		for (Price k = p2; k <= p1; instrumentPtr->IncPrice(k))
		{
			if (k == instrumentPtr->Get_tradePrice(y))
			{
                if (showOpenInterestChange == TRUE)
                {
                    Plaza2Instrument* instrument = ((Plaza2TickerAdapterClass*)instrumentPtr)->GetInstrument();
                    Plaza2Trade* trade = instrument->GetBook(y).trade;

                    switch (trade->interestDir)
                    {
                    case UNDEFINED: ::SetDCPenColor(dc, OINotChangeBackground); break;
                    case OPEN_POSITION: ::SetDCPenColor(dc, OIIncreaseBackground); break;
                    case CLOSE_POSITION: ::SetDCPenColor(dc, OIDecreaseBackground); break;
                    }
                }

				//трейд по биду
				if (instrumentPtr->Get_agressorSide(y) == 'S' || instrumentPtr->Get_impliedSide(y) == 'S')
				{
                    if (showOpenInterestChange == FALSE)
                    {
                        if (instrumentPtr->Get_agressorSide(y) == '?') 
                            ::SetDCPenColor(dc, noagressorColor);
                        else 
                            ::SetDCPenColor(dc, sellTradeColor);
                    }
					bar = (double)instrumentPtr->Get_tradeSize(y) / maxDomValue * (count - 1);
					if (bar < 1.) ++bar;
					if (bar > count) bar = count; //это на случай если установлено ручное масштабирование событий и текущее значение события больше установленного вручную
					//if (bar > (count - 1))	bar = count - 1;	//не понимаю когда размер этой переменной будет больше чем count-1, этого вообще не должно быть
					bidTradeFromX = ((double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count) - 1;
					::MoveToEx(dc, (int)j, bidTradeFromX, NULL);
					bidTradeToX = ((double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count - bar) - 1;
					LineTo(dc, (int)j, bidTradeToX);	//проверка нужна чтобы трейд не залезал на временную шкалу
				}
				//трейд по аску
				else if (instrumentPtr->Get_agressorSide(y) == 'B' || instrumentPtr->Get_impliedSide(y) == 'B')
				{
                    if (showOpenInterestChange == FALSE)
                    {
					    if (instrumentPtr->Get_agressorSide(y) == '?') 
                            ::SetDCPenColor(dc, noagressorColor);
					    else 
                            ::SetDCPenColor(dc, buyTradeColor);
                    }
					bar = (double)instrumentPtr->Get_tradeSize(y) / maxDomValue * (count - 1);
					if (bar < 1.) ++bar;
					if (bar > count) bar = count; //это на случай если установлено ручное масштабирование событий и текущее значение события больше установленного вручную
					askTradeFromX = ((double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count) + 1;
					::MoveToEx(dc, (int)j, askTradeFromX, NULL);
					askTradeToX = ((double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count + bar) + 1;
					if (askTradeToX <= scalePlotZone.bottom)	//проверка нужна чтобы трейд не залезал на временную шкалу
						LineTo(dc, (int)j, askTradeToX);
				}
				//трейд внутри спрэда
				else if (instrumentPtr->Get_agressorSide(y) == '?' || instrumentPtr->Get_impliedSide(y) == 'M')
				{
                    if (showOpenInterestChange == FALSE)
                    {
					    ::SetDCPenColor(dc, noagressorColor);
                    }
					bar = (double)instrumentPtr->Get_tradeSize(y) / maxDomValue * (count - 1);
					if (bar < 1.) ++bar;
					if (bar > count) bar = count; //это на случай если установлено ручное масштабирование событий и текущее значение события больше установленного вручную
					inspredTradeFromX = ((double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count);
					::MoveToEx(dc, (int)j, inspredTradeFromX, NULL);
					inspredTradeToX = ((double)instrumentPtr->PriceDifference(scaleMaxPrice, k) * count + bar) + 1;
					if (inspredTradeToX <= scalePlotZone.bottom)	//проверка нужна чтобы трейд не залезал на временную шкалу
						LineTo(dc, (int)j, inspredTradeToX);
				}
			}
		}
	}
}
void QuantChart::Draw3(HDC dc)
{
	//получаем индексы начала и конца диапазона, который будем отображать на экран
	size_t start, end;
	
	if (GetRangeIndexes(m_hWnd, instrumentPtr, start, end, scalePlotZone))
		return;

	//ищем максимальные и минимальные цены данного диапазона и смасштабированные макс и мин цены
	Price maxPrice((int)INT_MAX, (UINT)0, true),
			minPrice((int)INT_MAX, (UINT)0),
			scaleMaxPrice, scaleMinPrice, lastTrade, lastBidPrice, lastAskPrice;
	
	if (FindMaxMinPricesForDOM(dc, start, end, maxPrice, minPrice, lastTrade, lastBidPrice, lastAskPrice))
		return;	
	
	//этот кусок кода добавил когда подключал плазу2, т.к. на тестовом полигоне
	//у них слишком разряженный стакан и большие ценовые интервалы надо на эран 
	//выводить и из-за этого алгоритм сильно тормозит
	/*if (_i->PriceDifference(maxPrice, minPrice) > 10)
	{
		if (lastBidPrice.IsZero() || lastAskPrice.IsZero())
			return;

		maxPrice = lastAskPrice;
		minPrice = lastBidPrice;

		if (_i->PriceDifference(maxPrice, minPrice) == 1)
		{
			for (int i = 0; i < 10; ++i)
			{
				_i->IncPrice(maxPrice);
				_i->DecPrice(minPrice);
			}
		}

		if (_i->PriceDifference(maxPrice, minPrice) > 10)
		{
			while (_i->PriceDifference(maxPrice, minPrice) > 10)
			{
				_i->DecPrice(maxPrice);
				_i->IncPrice(minPrice);
			}
			lastAskPrice = maxPrice;
			lastBidPrice = minPrice;
		}
	}*/

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
	
	//assert(_i->PriceDifference(maxPrice, minPrice) <= 10);

	UINT maxDomValue;
	if (!bEventAutoScale)	//проверяем автоматически искать максимальное значение в стакане или использовать вручную заданное значение (0 - автоматический масштаб)
	{
		maxDomValue = FindMaxDomValue(start, end, p1, p2, maxPrice, scaleMaxPrice);
	}
	else 
		maxDomValue = uManualScaleMaxValue;

	if (!maxDomValue)
		return;

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
		//::GetTextExtentPoint32W(dc, lastTrade.GetWString(_i->GetPrecision()), (int)_tcslen(lastTrade.GetWString(_i->GetPrecision())), &sSize);
		
		::SetDCPenColor(dc, tradeColor);
		::SetDCBrushColor(dc, tradeLabelColor);
		
		int xCoord = scaleClientRect.right - price_axis_width/2 + 5,
			yCoord = (double)instrumentPtr->PriceDifference(scaleMaxPrice, lastTrade) * count + sSize.cy/2;	
		::Rectangle(dc, xCoord - sSize.cx/2 -3, yCoord - sSize.cy, xCoord + sSize.cx/2 +2, yCoord);
		::TextOut(dc, xCoord, yCoord, lastTrade.GetWString(instrumentPtr->GetPrecision()), (int)_tcslen(lastTrade.GetWString(instrumentPtr->GetPrecision())));
	}

	//для каждой найденной уникальной цены рисуем
	p1 = (maxPrice <= scaleMaxPrice) ? maxPrice : scaleMaxPrice;
	DrawDOM(dc, start, end, scaleMaxPrice, p1, p2, maxDomValue, count);
}
void QuantChart::DrawDOM(HDC dc, const size_t start, const size_t end, Price& scaleMaxPrice, Price& p1, Price& p2, const double maxDomValue, const double count)
{
	//CStopwatch stopwatch;

	//готовим данные для потоков
	std::vector<std::vector<Coordinate>> drawVector;
	drawVector.resize(iNumberOfProcessors);
	std::vector<LabelsCoordinate> labelsVector;

	tPool.SetQuantData(this, start, end, &scaleMaxPrice, &p1, &p2, maxDomValue, count, &drawVector, &labelsVector);

	//вычисляем диапазон для текущего потока
	size_t diff = end - start, distance = 0, _start, _end, x;
	if (diff < 200 || iNumberOfProcessors == 1)
	{
		_start = start;
		_end = end;
		x = 0;
	}
	else
	{	
		//запускаем параллельные потоки
		for (DWORD p = 0; p < iNumberOfProcessors-1; ++p)
			tPool.StartNewThread(2);

		distance = double(diff / iNumberOfProcessors) + 0.5;
		_start = start + distance * (iNumberOfProcessors - 1);
		_end = end;
		x = distance * (iNumberOfProcessors - 1);
	}
	
	//вычисляем координаты для текущего потока
	GetDOMCoordinates(this, _start, _end, instrumentPtr, scaleMaxPrice, p1, p2, maxDomValue, count, iNumberOfProcessors-1, distance, &drawVector, &labelsVector);
	
	//ждем пока отработают все потоки
	tPool.WaitForThread(2);
	
	//рисуем графику
	for(size_t q = 0; q < drawVector.size(); ++q)
	{
		for (size_t n = 0; n < drawVector[q].size(); ++n)
		{
			::SetDCPenColor(dc, drawVector[q][n].color);
			::MoveToEx(dc, drawVector[q][n].From.first, drawVector[q][n].From.second, NULL);
			::LineTo(dc, drawVector[q][n].To.first, drawVector[q][n].To.second);
		}
	}

	//рисуем горизонтальный стакан и метки
    if (showHorizontalDOM && plotZoneAdd.left != plotZoneAdd.right)
    {
        for (Price p = p2; p <= p1; instrumentPtr->IncPrice(p))
        {
            for (int l = instrumentPtr->GetDomLevelsCount()-1; l >= 0; l--)
            {
                DrawHorizDOM(dc, count, p, end, l, scaleMaxPrice);
            }
        }
    }

	//рисуем метки
	for(size_t q = 0; q < labelsVector.size(); ++q)
	{
		DrawDOMLabel(dc, labelsVector[q].size, labelsVector[q].color1, labelsVector[q].color2, labelsVector[q].Ycoord);
	}

	//double bar;
	//int bidTradeFromX = 0, bidTradeToX = 0, askTradeFromX = 0, askTradeToX = 0, inspredTradeFromX = 0, inspredTradeToX = 0, limitsToX = 0;
	//
	//for (size_t j = 0, y = start; y <= end; j++, y++)
	//{
	//	for (Price k = p2; k <= p1; _i->IncPrice(k))
	//	{		
	//		if (k == _i->Get_tradePrice(y))
	//		{
	//			//трейд по биду
	//			if (_i->Get_agressorSide(y) == 'S' || _i->Get_impliedSide(y) == 'S')
	//			{
	//				if (_i->Get_agressorSide(y) == '?') ::SetDCPenColor(dc, noagressorColor);
	//				else ::SetDCPenColor(dc, sellTradeColor);
	//				if (!bEventAutoScale)	
	//					bar = (double)_i->Get_tradeSizeQuant(y) / maxDomValue * (count - 1);
	//				else	//если ручное масштабирование событий
	//				{
	//					bar = (double)_i->Get_tradeSizeQuant(y) / maxDomValue * (count/2 - 1);
	//					if (bar >= count/2) bar = count/2;
	//				}
	//				if (bar < 1.) ++bar;
	//				bidTradeFromX = (double)_i->PriceDifference(scaleMaxPrice, k) * count - 1;
	//				::MoveToEx(dc, (int)j, bidTradeFromX, NULL);
	//				bidTradeToX = (double)_i->PriceDifference(scaleMaxPrice, k) * count - bar - 1;
	//				LineTo(dc, (int)j, bidTradeToX);
	//			}
	//			//трейд по аску
	//			else if (_i->Get_agressorSide(y) == 'B' || _i->Get_impliedSide(y) == 'B')
	//			{
	//				if (_i->Get_agressorSide(y) == '?') ::SetDCPenColor(dc, noagressorColor);
	//				else ::SetDCPenColor(dc, buyTradeColor);
	//				if (!bEventAutoScale)
	//					bar = (double)_i->Get_tradeSizeQuant(y) / maxDomValue * (count - 1);
	//				else
	//				{
	//					bar = (double)_i->Get_tradeSizeQuant(y) / maxDomValue * (count/2 - 1);
	//					if (bar >= count/2) bar = count/2;
	//				}
	//				if (bar < 1.) ++bar;
	//				askTradeFromX = (double)_i->PriceDifference(scaleMaxPrice, k) * count + 1;
	//				::MoveToEx(dc, (int)j, askTradeFromX, NULL);
	//				askTradeToX = (double)_i->PriceDifference(scaleMaxPrice, k) * count + bar + 1;
	//				if (askTradeToX <= plotZone.bottom)
	//					LineTo(dc, (int)j, askTradeToX);
	//			}
	//			//трейд внутри спрэда
	//			else if (_i->Get_agressorSide(y) == '?' || _i->Get_impliedSide(y) == 'M')
	//			{
	//				::SetDCPenColor(dc, noagressorColor);
	//				if (!bEventAutoScale)
	//					bar = (double)_i->Get_tradeSizeQuant(y) / maxDomValue * (count - 1);
	//				else
	//				{
	//					bar = (double)_i->Get_tradeSizeQuant(y) / maxDomValue * (count/2 - 1);
	//					if (bar >= count/2) bar = count/2;
	//				}
	//				if (bar < 1.) ++bar;
	//				inspredTradeFromX = (double)_i->PriceDifference(scaleMaxPrice, k) * count;
	//				::MoveToEx(dc, (int)j, inspredTradeFromX, NULL);
	//				inspredTradeToX = (double)_i->PriceDifference(scaleMaxPrice, k) * count + bar + 1;
	//				LineTo(dc, (int)j, inspredTradeToX);
	//			}
	//		}
	//						
	//		//все уровни в стакане
	//		for (int x = _i->GetDomLevelsCount()-1; x >= 0; x--)
	//		{
	//			//бид лимиты
	//			if (k == _i->Get_bidPrice(y, x) && _i->Get_bidSize(y, x))
	//			{
	//				if ((_i->Get_agressorSide(y) == 'b' || _i->Get_agressorSide(y) == 'x') && _i->Get_eventBidLevel(y) & (1 << x)) 
	//					::SetDCPenColor(dc, newBidLimitColor);
	//				else ::SetDCPenColor(dc, bidLimitColor);
	//				if (!bEventAutoScale)
	//					bar = (double)_i->Get_bidSize(y, x) / maxDomValue * (count - 1);
	//				else
	//				{
	//					bar = (double)_i->Get_bidSize(y, x) / maxDomValue * (count/2 - 1);
	//					if (bar >= count/2) bar = count/2;
	//				}
	//				::MoveToEx(dc, (int)j, (double)_i->PriceDifference(scaleMaxPrice, k) * count - 1, NULL);
	//				limitsToX = (double)_i->PriceDifference(scaleMaxPrice, k) * count - bar - 1;
	//				if (bidTradeToX == limitsToX)
	//					++limitsToX;
	//				LineTo(dc, (int)j, limitsToX);

	//				//метки
	//				if (limitsLabels && y == end)
	//					DrawDOMLabel(dc, _i->Get_bidSize(end, x), newBidLimitColor, bidLimitColor, limitsToX);
	//			}

	//			//бид ордера 
	//			if (showOrders && k == _i->Get_bidPrice(y, x) && _i->Get_bidOrders(y, x))
	//			{
	//				if ((_i->Get_agressorSide(y) == 'b' || _i->Get_agressorSide(y) == 'x') && _i->Get_eventBidLevel(y) & (1 << x)) 
	//					::SetDCPenColor(dc, newBidOrdersColor);
	//				else ::SetDCPenColor(dc, bidOrdersColor);				
	//				bar = (double)_i->Get_bidOrders(y, x) / maxDomValue * (count - 1);
	//				if (bar >= count) bar = count-1; //это на случай если установлено ручное масштабирование событий и текущее значение события больше установленного вручную
	//				::MoveToEx(dc, (int)j, (double)_i->PriceDifference(scaleMaxPrice, k) * count - 1, NULL);
	//				limitsToX = (double)_i->PriceDifference(scaleMaxPrice, k) * count - bar - 1;
	//				/*if (bidTradeToX && bidTradeToX == limitsToX)
	//					++limitsToX;*/
	//				LineTo(dc, (int)j, limitsToX);

	//				//метка
	//				if (ordersLabels && y == end)
	//					DrawDOMLabel(dc, _i->Get_bidOrders(end, x), newBidOrdersColor, bidOrdersColor, limitsToX);						
	//			}
	//		
	//			//аск лимиты
	//			if (k == _i->Get_askPrice(y, x) && _i->Get_askSize(y, x))
	//			{
	//				if ((_i->Get_agressorSide(y) == 'a' || _i->Get_agressorSide(y) == 'x') && _i->Get_eventAskLevel(y) & (1 << x)) 
	//					::SetDCPenColor(dc, newAskLimitColor);
	//				else ::SetDCPenColor(dc, askLimitColor);
	//				if (!bEventAutoScale)
	//					bar = (double)_i->Get_askSize(y, x) / maxDomValue * (count - 1);
	//				else
	//				{
	//					bar = (double)_i->Get_askSize(y, x) / maxDomValue * (count/2 - 1);
	//					if (bar >= count/2) bar = count/2;
	//				}
	//				::MoveToEx(dc, (int)j, (double)_i->PriceDifference(scaleMaxPrice, k) * count + 1, NULL);
	//				limitsToX = (double)_i->PriceDifference(scaleMaxPrice, k) * count + bar + 1;
	//				if (askTradeToX == limitsToX)
	//					--limitsToX;
	//				if (limitsToX <= plotZone.bottom)
	//					LineTo(dc, (int)j, limitsToX);

	//				//метки
	//				if (limitsLabels && y == end)
	//					DrawDOMLabel(dc, _i->Get_askSize(end, x), newAskLimitColor, askLimitColor, limitsToX);						
	//			}				

	//			//аск ордера 
	//			if (showOrders && k == _i->Get_askPrice(y, x) && _i->Get_askOrders(y, x))
	//			{
	//				if ((_i->Get_agressorSide(y) == 'a' || _i->Get_agressorSide(y) == 'x') && _i->Get_eventAskLevel(y) & (1 << x)) 
	//					::SetDCPenColor(dc, newAskOrdersColor);
	//				else ::SetDCPenColor(dc, askOrdersColor);
	//				if (bar >= count) bar = count-1; //это на случай если установлено ручное масштабирование событий и текущее значение события больше установленного вручную
	//				bar = (double)_i->Get_askOrders(y, x) / maxDomValue * (count - 1);
	//				::MoveToEx(dc, (int)j, (double)_i->PriceDifference(scaleMaxPrice, k) * count + 1, NULL);
	//				limitsToX = (double)_i->PriceDifference(scaleMaxPrice, k) * count + bar + 1;
	//				/*if (bidTradeToX && bidTradeToX == limitsToX)
	//					++limitsToX;*/
	//				if (limitsToX <= plotZone.bottom)
	//					LineTo(dc, (int)j, limitsToX);

	//				//метка
	//				if (ordersLabels && y == end)
	//					DrawDOMLabel(dc, _i->Get_askOrders(end, x), newAskOrdersColor, askOrdersColor, limitsToX);
	//			}

	//			//горизонтальный стакан
	//			if (y == end)
	//				if (showHorizDOM && plotZoneAdd.left != plotZoneAdd.right)
	//					DrawHorizDOM(dc, count, k, end, x, scaleMaxPrice);
	//		}			
	//	}
	//}

	/*__int64 qwElapsedTime = stopwatch.Now();
	char delay[32];
	sprintf_s(delay, "%Id\n", qwElapsedTime);
#ifdef _DEBUG
	//WriteToFile(delay, "DrawCycleDelay_release.txt", true);
#else
	WriteToFile(delay, "DrawCycleDelay_release.txt", true);
#endif*/
	
}
UINT QuantChart::FindMaxDomValue(const size_t start, const size_t end, Price& p1, Price& p2, Price& maxPrice, Price& scaleMaxPrice)
{
	//CStopwatch stopwatch;
	
	UINT maxDomValue = 0;	

	for (size_t y = start; y <= end; y++)
	{
		while (p1 >= p2)
		{
			//смотрим все уровни в стакане кроме нулевого
			for (int k = instrumentPtr->GetDomLevelsCount()-1; k >= 1; k--)
			{		
				//биды
				if (p1 == instrumentPtr->Get_bidPrice(y, k) && p1 != instrumentPtr->Get_tradePrice(y) && maxDomValue < instrumentPtr->Get_bidSize(y, k))
					maxDomValue = instrumentPtr->Get_bidSize(y, k);
				else if (p1 == instrumentPtr->Get_bidPrice(y, k) && p1 == instrumentPtr->Get_tradePrice(y) && maxDomValue < instrumentPtr->Get_bidSize(y, k) + instrumentPtr->Get_tradeSize(y))
					maxDomValue = instrumentPtr->Get_bidSize(y, k) + instrumentPtr->Get_tradeSize(y);
		
				//аски
				if (p1 == instrumentPtr->Get_askPrice(y, k) && p1 != instrumentPtr->Get_tradePrice(y) && maxDomValue < instrumentPtr->Get_askSize(y, k))
					maxDomValue = instrumentPtr->Get_askSize(y, k);
				else if (p1 == instrumentPtr->Get_askPrice(y, k) && p1 == instrumentPtr->Get_tradePrice(y) && maxDomValue < instrumentPtr->Get_askSize(y, k) + instrumentPtr->Get_tradeSize(y))
					maxDomValue = instrumentPtr->Get_askSize(y, k) + instrumentPtr->Get_tradeSize(y);	
			}

			//теперь нулевой уровень
			if (instrumentPtr->PriceDifference(instrumentPtr->Get_askPrice(y, 0), instrumentPtr->Get_bidPrice(y, 0)) == 1)
			{
				if (p1 == instrumentPtr->Get_askPrice(y, 0) && maxDomValue < instrumentPtr->Get_tradeSize(y) + instrumentPtr->Get_askSize(y, 0) + instrumentPtr->Get_bidSize(y, 0))
					maxDomValue = instrumentPtr->Get_tradeSize(y) + instrumentPtr->Get_askSize(y, 0) + instrumentPtr->Get_bidSize(y, 0);
			}
			else	//отдельно нулевой уровень бидов, отдельно асков и отдельно трейд
			{
				if (p1 == instrumentPtr->Get_askPrice(y, 0) && p1 == instrumentPtr->Get_tradePrice(y) &&  maxDomValue < instrumentPtr->Get_askSize(y, 0) + instrumentPtr->Get_tradeSize(y))
					maxDomValue = instrumentPtr->Get_askSize(y, 0) + instrumentPtr->Get_tradeSize(y);
				else if (p1 == instrumentPtr->Get_askPrice(y, 0) && p1 != instrumentPtr->Get_tradePrice(y) &&  maxDomValue < instrumentPtr->Get_askSize(y, 0))
					maxDomValue = instrumentPtr->Get_askSize(y, 0);
				
				if (p1 == instrumentPtr->Get_bidPrice(y, 0) && p1 == instrumentPtr->Get_tradePrice(y) && maxDomValue < instrumentPtr->Get_bidSize(y, 0) + instrumentPtr->Get_tradeSize(y))
					maxDomValue = instrumentPtr->Get_bidSize(y, 0) + instrumentPtr->Get_tradeSize(y);
				else if (p1 == instrumentPtr->Get_bidPrice(y, 0) && p1 != instrumentPtr->Get_tradePrice(y) && maxDomValue < instrumentPtr->Get_bidSize(y, 0))
					maxDomValue = instrumentPtr->Get_bidSize(y, 0);
				
				if (p1 == instrumentPtr->Get_tradePrice(y) && maxDomValue < instrumentPtr->Get_tradeSize(y))
					maxDomValue = instrumentPtr->Get_tradeSize(y);
			}	
			instrumentPtr->DecPrice(p1);
		}	
		p1 = (maxPrice <= scaleMaxPrice) ? maxPrice : scaleMaxPrice;
	}

	return maxDomValue;
}

BOOL QuantChart::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
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
void QuantChart::OnRButtonDown(UINT nFlags, CPoint point)
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

		menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON, p.x, p.y, AfxGetMainWnd());
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
void QuantChart::OnUpdateQcmenuOrdersshow(CCmdUI *pCmdUI)
{
	if (instrumentPtr->GetInstrumentINFO().ordersNumber)
		pCmdUI->Enable();
	else
		pCmdUI->Enable(0);
}
void QuantChart::ContextMenu(UINT nID)
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
				QuantChartSettings* qs = new QuantChartSettings(this);
				qs->DoModal();
				delete qs;
			}
			break;
		}
	}
}
void QuantChart::LabelsMenu(UINT nID)
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
void QuantChart::PlotZoneAddMenu(UINT nID)
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
void QuantChart::TimeZoneMenu()
{
	Xscale = 0;
	RECT rt;
	::GetClientRect(m_hWnd, &rt);
	SetRects(rt.right, rt.bottom);
	SetScrollBar();
	Invalidate(0);
}
