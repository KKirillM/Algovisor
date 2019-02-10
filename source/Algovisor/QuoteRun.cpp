#include "stdafx.h"
#include "QuoteRun.h"
#include "Instrument.h"
#include "Plaza2Instruments.h"
#include "MainWindow.h"
#include "RithmicDialog.h"
#include "Price.h"
#include "resource.h"
#include "Auxiliary_methods.h"

BEGIN_MESSAGE_MAP(List, CListCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

BOOL List::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
	//return CListCtrl::OnEraseBkgnd(pDC);
}
void List::OnPaint()
{
	// TODO: Add your message handler code here
	// Do not call CListCtrl::OnPaint() for painting messages
	
	if (!((QuoteRun*)GetParent())->instrumentPtr)
	{
		CPaintDC dc(this);

		RECT rt;
		((QuoteRun*)GetParent())->GetClientRect(&rt);

		//черный фон
		FillRect(dc, &rt, (HBRUSH)GetStockObject(BLACK_BRUSH));

		//выводим текст
		::SetTextColor(dc, RGB(0,0,0));		//черный текст
		::SetBkColor(dc, RGB(255,255,0));	//желтый фон текста
		::SetBkMode(dc, OPAQUE);
		::SetTextAlign(dc, TA_NOUPDATECP | TA_CENTER | TA_BASELINE);
		::SelectObject(dc, (HFONT)::GetStockObject(DEFAULT_GUI_FONT));
		TCHAR ticker[64];
		
		switch (((QuoteRun*)GetParent())->fType)
		{
		case RITHMIC:		StringCchPrintf(ticker, 64, L" Waiting for subscribe instrument: Rithmic: %S ", ((QuoteRun*)GetParent())->name.c_str()); break;
		case CQG:			StringCchPrintf(ticker, 64, L" Waiting for subscribe instrument: CQG: %S ", ((QuoteRun*)GetParent())->name.c_str()); break;
		case NINJATRADER:	StringCchPrintf(ticker, 64, L" Waiting for subscribe instrument: Ninjatrader: %S ", ((QuoteRun*)GetParent())->name.c_str()); break;
        default:            StringCchPrintf(ticker, 64, L" Waiting for subscribe instrument: %S ", ((QuoteRun*)GetParent())->name.c_str()); break;
		}

		::TextOut(dc, rt.right/2, rt.bottom/2, ticker, (int)_tcslen(ticker));
	}
	else
	{
		const MSG *msg = GetCurrentMessage();
		DefWindowProc(msg->message, msg->wParam, msg->lParam);
	}
}
void List::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)	//здесь нужен инвалидейт чтобы копирайт нормально отрисовывался
{																		//при перемещении вертикального скрола
	Invalidate();
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}
void List::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	Invalidate();
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}
BOOL List::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	Invalidate();
	return CListCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

//Названия колонок
#define TIMECOLUMN					L"Time"
#define DELTAMICROSECONDCOLUMN		L"DeltaMs"
#define TRADEPRICECOLUMN			L"Trade Price"
#define TRADESIZECOLUMN				L"Trade Size"

QuoteRunSettings::QuoteRunSettings(QuoteRun* pParent) : CDialogEx(IDD, (CWnd*)pParent), parentWindowPtr(pParent)
{	
	//SetBackgroundColor(BGRNCOLOR);
	//backgroundBrush.CreateSolidBrush(BGRNCOLOR);
}

BEGIN_MESSAGE_MAP(QuoteRunSettings, CDialogEx)
	ON_BN_CLICKED(IDOK, &QuoteRunSettings::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_ASKTRADETEXT, &QuoteRunSettings::OnBnClickedButtonAskTrade)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_BIDTRADETEXT, &QuoteRunSettings::OnBnClickedButtonBidTradeText)
	ON_BN_CLICKED(IDC_BUTTON5, &QuoteRunSettings::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON7, &QuoteRunSettings::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON9, &QuoteRunSettings::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON11, &QuoteRunSettings::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON13, &QuoteRunSettings::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_BUTTON2, &QuoteRunSettings::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON4, &QuoteRunSettings::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON6, &QuoteRunSettings::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON8, &QuoteRunSettings::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON10, &QuoteRunSettings::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON12, &QuoteRunSettings::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON14, &QuoteRunSettings::OnBnClickedButton14)
	ON_BN_CLICKED(IDC_BUTTON15, &QuoteRunSettings::OnBnClickedButton15)
	ON_BN_CLICKED(IDC_BUTTON17, &QuoteRunSettings::OnBnClickedButton17)
	ON_BN_CLICKED(IDC_BUTTON19, &QuoteRunSettings::OnBnClickedButton19)
	ON_BN_CLICKED(IDC_BUTTON21, &QuoteRunSettings::OnBnClickedButton21)
	ON_BN_CLICKED(IDC_BUTTON16, &QuoteRunSettings::OnBnClickedButton16)
	ON_BN_CLICKED(IDC_BUTTON18, &QuoteRunSettings::OnBnClickedButton18)
	ON_BN_CLICKED(IDC_BUTTON20, &QuoteRunSettings::OnBnClickedButton20)
	ON_BN_CLICKED(IDC_BUTTON22, &QuoteRunSettings::OnBnClickedButton22)
	ON_BN_CLICKED(IDC_BUTTON32, &QuoteRunSettings::OnBnClickedButton32)
	ON_BN_CLICKED(IDC_BUTTON_FONT, &QuoteRunSettings::OnBnClickedButtonFont)
	ON_BN_CLICKED(IDC_BUTTON33, &QuoteRunSettings::OnBnClickedButton33)
	ON_BN_CLICKED(IDC_BUTTON23, &QuoteRunSettings::OnBnClickedButton23)
	ON_BN_CLICKED(IDC_BUTTON25, &QuoteRunSettings::OnBnClickedButton25)
	ON_BN_CLICKED(IDC_BUTTON27, &QuoteRunSettings::OnBnClickedButton27)
	ON_BN_CLICKED(IDC_BUTTON29, &QuoteRunSettings::OnBnClickedButton29)
	ON_BN_CLICKED(IDC_BUTTON24, &QuoteRunSettings::OnBnClickedButton24)
	ON_BN_CLICKED(IDC_BUTTON26, &QuoteRunSettings::OnBnClickedButton26)
	ON_BN_CLICKED(IDC_BUTTON28, &QuoteRunSettings::OnBnClickedButton28)
	ON_BN_CLICKED(IDC_BUTTON30, &QuoteRunSettings::OnBnClickedButton30)
	ON_BN_CLICKED(IDC_BUTTON36, &QuoteRunSettings::OnBnClickedButton36)
	ON_BN_CLICKED(IDC_BUTTON37, &QuoteRunSettings::OnBnClickedButton37)
	ON_BN_CLICKED(IDC_BUTTON34, &QuoteRunSettings::OnBnClickedButton34)
	ON_BN_CLICKED(IDC_BUTTON35, &QuoteRunSettings::OnBnClickedButton35)
	ON_BN_CLICKED(IDC_BUTTON38, &QuoteRunSettings::OnBnClickedButton38)
    ON_BN_CLICKED(IDC_BUTTON_OPENINTEREST_INCREASE_TEXT_COLOR, &QuoteRunSettings::OnBnClickedButtonOpeninterestIncreaseTextColor)
    ON_BN_CLICKED(IDC_BUTTON_OPENINTEREST_INCREASE_BACKGROUND_COLOR, &QuoteRunSettings::OnBnClickedButtonOpeninterestIncreaseBackgroundColor)
    ON_BN_CLICKED(IDC_BUTTON_OPENINTEREST_DECREASE_TEXT_COLOR, &QuoteRunSettings::OnBnClickedButtonOpeninterestDecreaseTextColor)
    ON_BN_CLICKED(IDC_BUTTON_OPENINTEREST_DECREASE_BACKGROUND_COLOR, &QuoteRunSettings::OnBnClickedButtonOpeninterestDecreaseBackgroundColor)
    ON_BN_CLICKED(IDC_BUTTON_OPENINTEREST_NOTCHANGE_TEXT_COLOR, &QuoteRunSettings::OnBnClickedButtonOpeninterestNotchangeTextColor)
    ON_BN_CLICKED(IDC_BUTTON_OPENINTEREST_NOTCHANGE_BACKGROUND_COLOR, &QuoteRunSettings::OnBnClickedButtonOpeninterestNotchangeBackgroundColor)
END_MESSAGE_MAP()

void QuoteRunSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_TIME, parentWindowPtr->timeColumn);
	DDX_Check(pDX, IDC_CHECK_DELTAMICROSECOND, parentWindowPtr->deltaMicroColumn);
	DDX_Check(pDX, IDC_CHECK_TRADEPRICE, parentWindowPtr->tradePriceColumn);
	DDX_Check(pDX, IDC_CHECK_TRADESIZE, parentWindowPtr->tradeSizeColumn);
	DDX_Check(pDX, IDC_CHECK_TRADECONDITION, parentWindowPtr->tradeCondition);
	DDX_Check(pDX, IDC_CHECK_LEVEL2PRICE, parentWindowPtr->Level2PriceColumn);
	DDX_Check(pDX, IDC_CHECK_LEVEL2SIZE, parentWindowPtr->Level2SizeColumn);
	DDX_Check(pDX, IDC_CHECK_LEVEL2ORDERS, parentWindowPtr->Level2OrdersColumn);
	DDX_Check(pDX, IDC_CHECK_DELTALIMITS, parentWindowPtr->deltaLimitColumn);
	DDX_Check(pDX, IDC_CHECK_CUMULATIVEDELTA, parentWindowPtr->cumulativeDelta);
	DDX_Check(pDX, IDC_CHECK_SHOWTITLEBAR, parentWindowPtr->showTitleBar);
	DDX_Check(pDX, IDC_CHECK_SHOWGRIDLINES, parentWindowPtr->showGridLines);
	DDX_Check(pDX, IDC_CHECK_ALERTTRADESIZE, parentWindowPtr->alertTradeSize);
	DDX_Check(pDX, IDC_CHECK_ALERTDELTALIMITADD, parentWindowPtr->alertDeltaLimitAdd);
	DDX_Check(pDX, IDC_CHECK_ALERTDELTALIMITREMOVE, parentWindowPtr->alertDeltaLimitRemove);
	DDX_Check(pDX, IDC_CHECK_ALERTDELTAMICROSECOND, parentWindowPtr->alertDeltaMicrosecond);
	DDX_Check(pDX, IDC_CHECK_ONLYBUY, parentWindowPtr->showOnlyBuyTrades);
	DDX_Check(pDX, IDC_CHECK_ONLYSELL, parentWindowPtr->showOnlySellTrades);
    DDX_Check(pDX, IDC_CHECK_OPEN_INTEREST, parentWindowPtr->showInterestColumn); 

	DDX_Check(pDX, IDC_RADIO_NOAGGREGATION, parentWindowPtr->noAggregation);

	DDX_Check(pDX, IDC_CHECK_MOVEORDERADD, parentWindowPtr->moveOrderAddLiquid);
	DDX_Check(pDX, IDC_CHECK_MOVEORDERREMOVE, parentWindowPtr->moveOrderRemoveLiquid);
	DDX_Check(pDX, IDC_CHECK_ALGOORDERADDLIQUID, parentWindowPtr->algoOrderAddLiquid);
	DDX_Check(pDX, IDC_CHECK_ALGOORDERREMOVE, parentWindowPtr->algoOrderRemoveLiquid); 
    DDX_Check(pDX, IDC_CHECK_OPEN_INTEREST_CHANGE_COLOR, parentWindowPtr->showOpenInterestChange);

	DDX_Text(pDX, IDC_EDIT_ALERTTRADESIZE, parentWindowPtr->alertTradeSizeValue);
	DDV_MinMaxInt(pDX, parentWindowPtr->alertTradeSizeValue, 0, 10000);
	DDX_Text(pDX, IDC_EDIT_ALERTDELTALIMITADD, parentWindowPtr->alertDeltaLimitAddValue);
	DDV_MinMaxInt(pDX, parentWindowPtr->alertDeltaLimitAddValue, 0, 10000);
	DDX_Text(pDX, IDC_EDIT_DELTALIMITREMOVE, parentWindowPtr->alertDeltaLimitRemoveValue);
	DDV_MinMaxInt(pDX, parentWindowPtr->alertDeltaLimitRemoveValue, 0, 10000);
	DDX_Text(pDX, IDC_EDIT_ALERTDELTAMICROSECOND, parentWindowPtr->alertDeltaMicrosecondValue);
	DDV_MinMaxInt(pDX, parentWindowPtr->alertDeltaMicrosecondValue, 0, 999999);

	DDX_Check(pDX, IDC_CHECK_FILTERINGTRADESIZE, parentWindowPtr->filteringTradeSize);
	DDX_Text(pDX, IDC_EDIT_FILTERINGTRADEVALUE, parentWindowPtr->filteringTradeValue);
	DDV_MinMaxInt(pDX, parentWindowPtr->filteringTradeValue, 0, 9999);
}

BOOL QuoteRunSettings::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	LOGFONT log;
	TCHAR str[15];

    // устанавливаем имя текущего шрифта
	parentWindowPtr->m_listCtrl.GetFont()->GetLogFont(&log);
	StringCchPrintf(str, 15, L"%.10s", log.lfFaceName);
	GetDlgItem(IDC_BUTTON_FONT)->SetWindowText(str);

	//выключаем ненужные элементы окна
	if (!parentWindowPtr->instrumentPtr->tInfo.ordersNumber)
    {
		GetDlgItem(IDC_CHECK_LEVEL2ORDERS)->EnableWindow(0);
    }

	// делаем недоступными элементы, которые не имеют смысла в других датафидах
	switch (parentWindowPtr->fType)
	{
	case CQG:
	case NINJATRADER:
		GetDlgItem(IDC_CHECK_OPEN_INTEREST_CHANGE_COLOR)->EnableWindow(0);
	case PLAZA2REPLAY:
	case PLAZA2:
		GetDlgItem(IDC_CHECK_CUMULATIVEDELTA)->EnableWindow(0);
		GetDlgItem(IDC_CHECK_TRADECONDITION)->EnableWindow(0);
		break;

	case RITHMIC:
		GetDlgItem(IDC_CHECK_OPEN_INTEREST_CHANGE_COLOR)->EnableWindow(0);
		break;
	}	

	if (parentWindowPtr->noAggregation)
    {
		CheckRadioButton(IDC_RADIO_NOAGGREGATION, IDC_RADIO_CMECOREAGGREGATION, IDC_RADIO_NOAGGREGATION);
    }
	else
    {
		CheckRadioButton(IDC_RADIO_NOAGGREGATION, IDC_RADIO_CMECOREAGGREGATION, IDC_RADIO_CMECOREAGGREGATION);
    }

	//устанавливаем переключатели condition
	((CComboBox*)GetDlgItem(IDC_COMBO_ALERTTRADESIZE))->SetCurSel(parentWindowPtr->alertTradeCondition);
	((CComboBox*)GetDlgItem(IDC_COMBO_ALERTDELTALIMITADD))->SetCurSel(parentWindowPtr->alertDeltaLimitAddCondition);
	((CComboBox*)GetDlgItem(IDC_COMBO_DELTALIMITREMOVE))->SetCurSel(parentWindowPtr->alertDeltaLimitRemoveCondition);
	((CComboBox*)GetDlgItem(IDC_COMBO_ALERTDELTAMICROSECOND))->SetCurSel(parentWindowPtr->alertDeltaMicrosecCondition);
	((CComboBox*)GetDlgItem(IDC_COMBO_FILTERINGTRADECONDITION))->SetCurSel(parentWindowPtr->filteringTradeCondition);

	return TRUE;
}

void SaveInstrumentPtrAndCreateNewInstrument(IInstrument** ptr, IInstrument** subPtr)
{
	if (*subPtr == NULL)
    {
		*subPtr = *ptr;
    }
	else
    {
        delete *ptr;
    }
		
	*ptr = (IInstrument*)(*subPtr)->create_object();
	(*ptr)->CopyAllParameters(*subPtr);
}

void QuoteRunSettings::OnBnClickedOk()
{
	//устанавливаем переключатели condition
	parentWindowPtr->alertTradeCondition = ((CComboBox*)GetDlgItem(IDC_COMBO_ALERTTRADESIZE))->GetCurSel();
	parentWindowPtr->alertDeltaLimitAddCondition = ((CComboBox*)GetDlgItem(IDC_COMBO_ALERTDELTALIMITADD))->GetCurSel();
	parentWindowPtr->alertDeltaLimitRemoveCondition = ((CComboBox*)GetDlgItem(IDC_COMBO_DELTALIMITREMOVE))->GetCurSel();
	parentWindowPtr->alertDeltaMicrosecCondition = ((CComboBox*)GetDlgItem(IDC_COMBO_ALERTDELTAMICROSECOND))->GetCurSel();
	parentWindowPtr->filteringTradeCondition = ((CComboBox*)GetDlgItem(IDC_COMBO_FILTERINGTRADECONDITION))->GetCurSel();

	CDialogEx::OnOK();

	if (parentWindowPtr->subInstrumentPtr != NULL)
	{
		delete parentWindowPtr->instrumentPtr;
		parentWindowPtr->instrumentPtr = parentWindowPtr->subInstrumentPtr;
		parentWindowPtr->subInstrumentPtr = NULL;
		parentWindowPtr->timerIndex = 0;
		parentWindowPtr->syncIndex = 0;
		parentWindowPtr->showOnlyTrades = false;
	}

	// проверяем условие "показывать только трейды"
    // оно выполняется, если колонки отображающие стакан выключены или включена агрегация или фильтрация или включено отображение только покупок или только продаж
	if ((!parentWindowPtr->Level2PriceColumn && !parentWindowPtr->Level2SizeColumn && !parentWindowPtr->Level2OrdersColumn && !parentWindowPtr->deltaLimitColumn) ||
        !parentWindowPtr->noAggregation || 
        parentWindowPtr->filteringTradeSize || 
        parentWindowPtr->showOnlyBuyTrades || 
        parentWindowPtr->showOnlySellTrades)
    {
		parentWindowPtr->showOnlyTrades = true;
        parentWindowPtr->timerIndex = 0;
        SaveInstrumentPtrAndCreateNewInstrument(&parentWindowPtr->instrumentPtr, &parentWindowPtr->subInstrumentPtr);
        //pParentWindow->FillTable();
    }

	//if (pParentWindow->noAggregation && 
 //       !pParentWindow->filteringTradeSize && 
 //       pParentWindow->subInstrumentPtr != NULL && 
	//	(pParentWindow->Level2PriceColumn || pParentWindow->Level2SizeColumn || pParentWindow->Level2OrdersColumn || pParentWindow->deltaLimitColumn) &&
	//	!pParentWindow->showOnlyTrades)
 //   {
	//	//pParentWindow->FillTable();
 //   }
	
	parentWindowPtr->InitializeControlList(0,0);
	parentWindowPtr->FillTable();	//это нужно чтобы полоса прокрутки внизу появлялась в режимие freeze
}

HBRUSH QuoteRunSettings::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	
	switch (pWnd->GetDlgCtrlID())
	{
	case IDC_STATIC_ASKTRADETEXT:					pDC->SetTextColor(parentWindowPtr->askTradeText); break;
	case IDC_STATIC_BIDTRADETEXT:					pDC->SetTextColor(parentWindowPtr->bidTradeText); break;
	case IDC_STATIC_IMPLIEDTRADECOLOR:				pDC->SetTextColor(parentWindowPtr->noAgressorText); break;
	case IDC_STATIC_ASKLIMITADD:					pDC->SetTextColor(parentWindowPtr->addAskText); break;
	case IDC_STATIC_ASKLIMITREMOVE:					pDC->SetTextColor(parentWindowPtr->removeAskText); break;
	case IDC_STATIC_BIDLIMITADD:					pDC->SetTextColor(parentWindowPtr->addBidText);	break;
	case IDC_STATIC_BIDLIMITREMOVE:					pDC->SetTextColor(parentWindowPtr->removeBidText);	break;
	case IDC_STATIC_ASKTRADEBACKGROUND:				pDC->SetTextColor(parentWindowPtr->askTradeBackgrnd);	break;
	case IDC_STATIC_BIDTRADEBACKGROUND:				pDC->SetTextColor(parentWindowPtr->bidTradeBackgrnd);	break;
	case IDC_STATIC_IMPLIEDTRADEBACKGROUND:			pDC->SetTextColor(parentWindowPtr->noAgressorBackgrnd); break;
	case IDC_STATIC_ASKLIMITADDBACKGROUND:			pDC->SetTextColor(parentWindowPtr->addAskBackgrnd); break;
	case IDC_STATIC_ASKLIMITREMOVEBACKGROUND:		pDC->SetTextColor(parentWindowPtr->removeAskBackgrnd); break;
	case IDC_STATIC_BIDLIMITADDBACKGROUND:			pDC->SetTextColor(parentWindowPtr->addBidBackgrnd); break;
	case IDC_STATIC_BIDLIMITREMOVEBACKGROUND:		pDC->SetTextColor(parentWindowPtr->removeBidBackgrnd); break;
	case IDC_STATIC_REPLACEORDERADDLIQUIDITY:		pDC->SetTextColor(parentWindowPtr->moveOrderAddText); break;
	case IDC_STATIC_REPLACEORDERREMOVELIQUIDITY:	pDC->SetTextColor(parentWindowPtr->moveOrderRemoveText); break;
	case IDC_STATIC_ALGOORDERADDLIQUIDITY:			pDC->SetTextColor(parentWindowPtr->algoOrderAddText);	break;
	case IDC_STATIC_ALGOOREDERREMOVELIQUDITY:		pDC->SetTextColor(parentWindowPtr->algoOrderRemoveText); break;
	case IDC_STATIC_REPLACEORDERADDLIQUIDITYB:		pDC->SetTextColor(parentWindowPtr->moveOrderAddBackgrnd); break;
	case IDC_STATIC_REPLACEORDERREMOVELIQUIDITYB:	pDC->SetTextColor(parentWindowPtr->moveOrderRemoveBackgrnd); break;
	case IDC_STATIC_ALGOORDERADDLIQUIDITYB:			pDC->SetTextColor(parentWindowPtr->algoOrderAddBackgrnd); break;
	case IDC_STATIC_ALGOOREDERREMOVELIQUDITYB:		pDC->SetTextColor(parentWindowPtr->algoOrderRemoveBackgrnd); break;
	case IDC_STATIC_BACKGROUNDCOLOR:				pDC->SetTextColor(parentWindowPtr->defaultbackgroundColor); break;
	case IDC_STATIC_DEFAULTTEXTCOLOR:				pDC->SetTextColor(parentWindowPtr->defaultTextColor); break;
	case IDC_STATIC_TRADESIZETEXTCOLOR:				pDC->SetTextColor(parentWindowPtr->alertTradeSizeText); break;
	case IDC_STATIC_DELTALIMITADDTEXTCOLOR:			pDC->SetTextColor(parentWindowPtr->alertDeltaLimitAddText); break;
	case IDC_STATIC_DELTALIMITREMOVETEXTCOLOR:		pDC->SetTextColor(parentWindowPtr->alertDeltaLimitRemoveText); break;
	case IDC_STATIC_DELTAMICROSECONDTEXTCOLOR:		pDC->SetTextColor(parentWindowPtr->alertDeltaMicrosecondText); break;
	case IDC_STATIC_TRADESIZEBACKCOLOR:				pDC->SetTextColor(parentWindowPtr->alertTradeSizeBackgrnd); break;
	case IDC_STATIC_DELTALIMITADDBACKCOLOR:			pDC->SetTextColor(parentWindowPtr->alertDeltaLimitAddBackgrnd); break;
	case IDC_STATIC_DELTALIMITREMOVEBACKCOLOR:		pDC->SetTextColor(parentWindowPtr->alertDeltaLimitRemoveBackgrnd); break;
	case IDC_STATIC_DELTAMICROSECONDBACKCOLOR:		pDC->SetTextColor(parentWindowPtr->alertDeltaMicrosecondBackgrnd); break;
	case IDC_STATIC_DEFAULTBIDTEXTCOLOR:			pDC->SetTextColor(parentWindowPtr->defaultBidTextColor); break;
	case IDC_STATIC_DEFAULTASKTEXTCOLOR:			pDC->SetTextColor(parentWindowPtr->defaultAskTextColor); break;
	case IDC_STATIC_DEFAULTBIDBACKGROUNDCOLOR:		pDC->SetTextColor(parentWindowPtr->defaultBidBackgroundColor); break;
	case IDC_STATIC_DEFAULTASKBACKGROUNDCOLOR:		pDC->SetTextColor(parentWindowPtr->defaultAskBackgroundColor); break;
	case IDC_STATIC_GRIDLINESCOLOR:					pDC->SetTextColor(parentWindowPtr->gridlineColor); break;
    case IDC_STATIC_OPENINTEREST_INCREASE_TEXT_COLOR:           pDC->SetTextColor(parentWindowPtr->OIIncreaseText); break;
    case IDC_STATIC_OPENINTEREST_INCREASE_BACKGROUND_COLOR:     pDC->SetTextColor(parentWindowPtr->OIIncreaseBackground); break;
    case IDC_STATIC_OPENINTEREST_DECREASE_TEXT_COLOR:           pDC->SetTextColor(parentWindowPtr->OIDecreaseText); break;
    case IDC_STATIC_OPENINTEREST_DECREASE_BACKGROUND_COLOR:     pDC->SetTextColor(parentWindowPtr->OIDecreaseBackground); break;
    case IDC_STATIC_OPENINTEREST_NOTCHANGE_TEXT_COLOR:          pDC->SetTextColor(parentWindowPtr->OINotChangeText); break;
    case IDC_STATIC_OPENINTEREST_NOTCHANGE_BACKGROUND_COLOR:    pDC->SetTextColor(parentWindowPtr->OINotChangeBackground); break;

	/*case IDC_STATIC:
		SetTextColor(pDC->m_hDC, RGB(255,255,255));
		SetBkColor( pDC->m_hDC, BGRNCOLOR);
		return (HBRUSH)(backgroundBrush.GetSafeHandle());	*/ 
		break;
	}	
	
	return hbr;
}




// ************************ QuoteRun class ************************

QuoteRun::QuoteRun(MainWindow* parent, FeedType feed, IInstrument* instrument) : 
    CFrameWnd(), 
    pParentWindow(parent), 
    fType(feed), 
    instrumentPtr(instrument),  
    subInstrumentPtr(NULL), 
    timerIndex(0), 
    syncIndex(0)
{		
	name = instrument->GetName();

    showOpenInterestChange = FALSE;

	topIndex = 0;
	freeze = false;
	alwaysOnTop = false;

    showOnlyTrades = false;

    Level2PriceColumn = false;
    Level2SizeColumn = true;
	Level2OrdersColumn = false;
    deltaLimitColumn = true;
	timeColumn = true;
	deltaMicroColumn = false;
	tradePriceColumn = true;
	tradeSizeColumn = true;
	tradeCondition = false;
	cumulativeDelta = false;	
	showOnlyBuyTrades = false;
	showOnlySellTrades = false;
    showInterestColumn = 0;

	showTitleBar = true;
	showGridLines = false;

	alertTradeSize = false;
	alertDeltaLimitAdd = false;
	alertDeltaLimitRemove = false;
	alertDeltaMicrosecond = false;

	moveOrderAddLiquid = false;
	moveOrderRemoveLiquid = false;
	algoOrderAddLiquid = false;
	algoOrderRemoveLiquid = false;

	deltaMicroText = askTradeText = bidTradeText = noAgressorText =
		addAskText = removeAskText = addBidText = removeBidText = alertTradeSizeText =
		alertDeltaLimitAddText = alertDeltaLimitRemoveText = alertDeltaMicrosecondText = 
        OIIncreaseText = OIDecreaseText = OINotChangeText = RGB(0,0,0); // RGB(255,255,255);

	deltaMicroBackgrnd = RGB(255,255,255);		// белый
	askTradeBackgrnd = RGB(128,255,128);		// бледно-зеленый
	bidTradeBackgrnd = RGB(255,128,128);		// бледно-красный
	noAgressorBackgrnd = RGB(128,255,255);		// бледно-голубой
	addAskBackgrnd = RGB(0,128,255);			// голубой
	removeAskBackgrnd = RGB(255,0,0);			// ярко-красный
	addBidBackgrnd = RGB(0,128,255);			// голубой
	removeBidBackgrnd = RGB(255,0,0);			// ярко-красный

	moveOrderAddText = moveOrderRemoveText = algoOrderAddText = algoOrderRemoveText = RGB(0,0,0);
	
	moveOrderAddBackgrnd = RGB(128,64,0);		// коричневый
	moveOrderRemoveBackgrnd = RGB(255,128,0);	// оранжевый
	algoOrderAddBackgrnd = RGB(128,0,255);		// фиолетовый
	algoOrderRemoveBackgrnd = RGB(128,128,255);	// фиолетовый

    defaultbackgroundColor = RGB(0,0,0);	// белый
    defaultTextColor = RGB(255,255,255);	// черный

    defaultBidBackgroundColor = RGB(0,0,160);				        // темно-синий
    defaultAskBackgroundColor = RGB(160,0,0);				        // темно-красный
	defaultBidTextColor = defaultAskTextColor = RGB(255,255,255);   // белый

    alertTradeSizeBackgrnd = RGB(255,128,0);				// оранжевый
    alertDeltaLimitAddBackgrnd = RGB(0,128,0);				// темно-зеленый
    alertDeltaLimitRemoveBackgrnd = RGB(128,0,0);			// темно-красный
    alertDeltaMicrosecondBackgrnd = RGB(0,128,255);			// голубой

    OIIncreaseBackground = RGB(128,255,128);		// бледно-зеленый
    OIDecreaseBackground = RGB(255,128,128);		// бледно-красный
    OINotChangeBackground = RGB(192,192,192);       // бледно-серый

	gridlineColor = RGB(0,0,0);

	alertTradeSizeValue = 0;
	alertDeltaLimitAddValue = 0;
	alertDeltaLimitRemoveValue = 0;
	alertDeltaMicrosecondValue = 0;

	alertTradeCondition = 0;
	alertDeltaLimitAddCondition = 0;
	alertDeltaLimitRemoveCondition = 0;
	alertDeltaMicrosecCondition = 0;

	noAggregation = true;

	filteringTradeSize = false;

	filteringTradeValue = 0;
	filteringTradeCondition = 0;
}

QuoteRun::~QuoteRun()
{
	if (subInstrumentPtr != NULL)
		delete instrumentPtr;

	for (ptrdiff_t x = pParentWindow->quoteRunWindows.size()-1; x >= 0; --x)
	{
		if (this == pParentWindow->quoteRunWindows[x])
		{
			pParentWindow->quoteRunWindows.erase(pParentWindow->quoteRunWindows.begin()+x);
			break;
		}
	}
}

void QuoteRun::Unsubscribe()
{
	if (subInstrumentPtr != NULL)
    {
		delete instrumentPtr;
    }
	
	subInstrumentPtr = NULL;
	instrumentPtr = NULL;

	m_listCtrl.SetItemCountEx(0, LVSICF_NOSCROLL);
	timerIndex = 0;
    syncIndex = 0;
}

void QuoteRun::SetTimerIndex()
{
    if (subInstrumentPtr == NULL)
    {
        timerIndex = instrumentPtr->GetAggregatedOrderbookSize();
    }
    else 
    {
        timerIndex = subInstrumentPtr->GetAggregatedOrderbookSize();
    }
}

BEGIN_MESSAGE_MAP(QuoteRun, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_NOTIFY(NM_RCLICK, 133, OnRButtonClick)
	ON_COMMAND_RANGE(ID_DFMENU_FREEZE, ID_DFMENU_SETTINGS, ContextMenu)
	ON_NOTIFY(NM_CUSTOMDRAW, 133, OnNMCustomdrawLvCustomDraw)
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_APP + 1, &QuoteRun::InitializeControlList)	
END_MESSAGE_MAP()

// QuoteRun message handlers

BOOL QuoteRun::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.cx = 1150;
	cs.cy = 600;
	
	return CFrameWnd::PreCreateWindow(cs);
}

int QuoteRun::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//добавляем в стиль окна CS_OWNDC
	//DWORD style = ::GetClassLong(m_hWnd, GCL_STYLE);
	//::SetClassLong(m_hWnd, GCL_STYLE, style | CS_OWNDC);

	//по-умолчанию черный фон
	::SetClassLongPtrW(m_hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)::GetStockObject(BLACK_BRUSH));

	//указываем в заголовке окна название инструмента
	wchar_t title[100];
	StringCchPrintfW(title, 100, _T("Quote Run | %S"), name.c_str());
	SetWindowText(title);

 	// Загрузить иконку 16х16
 	HICON m_hIconSmall = (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_QUOTERUN), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
 	// Загрузить иконку 32x32 
 	HICON m_hIconBig = (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_QUOTERUN), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
	SetIcon(m_hIconSmall, false); 
	SetIcon(m_hIconBig, true);

	//создаем строку состояния
	statusBar.Create(this);
	statusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	//statusBar.GetPaneInfo(0, nID, nStyle, cxWidth);
    //statusBar.SetPaneInfo(0, nID, nStyle | SBT_OWNERDRAW, cxWidth);
	if (fType == RITHMIC)
	{
		HICON rithmicIcon = (HICON)LoadImageW(AfxGetInstanceHandle(), MAKEINTRESOURCE(/*IDI_RITHMIC_LICENSE_ICON_2*/IDI_RITHMIC_TRADING_PLATFORM), IMAGE_ICON, /*134*/161, /*28*/15, 0);
		HICON omneIcond = (HICON)LoadImageW(AfxGetInstanceHandle(), MAKEINTRESOURCE(/*IDI_RITHMIC_LICENSE_ICON_3*/IDI_RITHMIC_OMNE), IMAGE_ICON, 122, /*28*/15, 0);
		statusBar.GetStatusBarCtrl().SetIcon(1, rithmicIcon);
		statusBar.GetStatusBarCtrl().SetIcon(2, omneIcond);
	}

	//listFont.CreateFont(14, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 
	//OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, VARIABLE_PITCH|FF_SWISS, L"Arial");
	
	//создаем таблицу для ленты
	CRect rect;
	GetClientRect(&rect);
	m_listCtrl.Create(LVS_ICON|LVS_REPORT|WS_VISIBLE|LVS_OWNERDATA, rect, this, 133);
	//m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | /*LVS_EX_ONECLICKACTIVATE |*/ LVS_EX_HEADERDRAGDROP);
	
	LOGFONT lg;
	m_listCtrl.GetFont()->GetLogFont(&lg);
	listFont.CreateFontIndirectW(&lg);

	InitializeControlList(0,0);

	m_listCtrl.SetBkColor(defaultbackgroundColor);

	//запускаем таймер
	SetTimer(3, 150, 0);
	
	return 0;
}

LRESULT QuoteRun::InitializeControlList(WPARAM wParam, LPARAM lParam)
{
	//убираем все старые колонки в списке
	int iNbrOfColumns;
    CHeaderCtrl* pHeader = (CHeaderCtrl*)m_listCtrl.GetDlgItem(0);
   
	if (pHeader) 
	{
		iNbrOfColumns = pHeader->GetItemCount();
  
		for (int i = iNbrOfColumns; i >= 0; i--)
        {
			m_listCtrl.DeleteColumn(i);
        }
	}
	
	//заголовок таблицы
	if (showTitleBar)
    {
		m_listCtrl.ModifyStyle(LVS_NOCOLUMNHEADER, 0);
    }
	else 
    {
		m_listCtrl.ModifyStyle(0, LVS_NOCOLUMNHEADER);
    }

	m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);

	//сетка
	//if (showGridLines)
	//	m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);
		//m_listCtrl.ModifyStyleEx(0, LVS_EX_GRIDLINES);
	//else
		//m_listCtrl.ModifyStyleEx(LVS_EX_GRIDLINES, 0);
	//	m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);

	//заполняем список колонками
	int columnNumber = 0;

	m_listCtrl.InsertColumn(columnNumber++, _T(""), LVCFMT_LEFT, 0);	

	colType.clear();		//список колонок
	colType.push_back(0);	//для нулевой колонки
	
	if (timeColumn) { m_listCtrl.InsertColumn(columnNumber++, TIMECOLUMN, LVCFMT_CENTER, TimeWidth); colType.push_back(TIME); }
	
	if (deltaMicroColumn) { m_listCtrl.InsertColumn(columnNumber++, DELTAMICROSECONDCOLUMN, LVCFMT_CENTER, DeltaMicroWidth); colType.push_back(DELTAMS); }

    if (!showOnlyTrades)
    {
	    if (instrumentPtr && instrumentPtr->GetDomLevelsCount() > 5)
	    {
		    if (Level2PriceColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid10 Price"), LVCFMT_CENTER, PriceWidth); colType.push_back(BID10PRICE); }
		    if (Level2SizeColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid10 Size"), LVCFMT_CENTER, SizeWidth); colType.push_back(BID10SIZE); }
		    if (Level2OrdersColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid10 Order"), LVCFMT_CENTER, SizeWidth); colType.push_back(BID10ORDERS); }

		    if (Level2PriceColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid9 Price"), LVCFMT_CENTER, PriceWidth); colType.push_back(BID9PRICE); }
		    if (Level2SizeColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid9 Size"), LVCFMT_CENTER, SizeWidth); colType.push_back(BID9SIZE); }
		    if (Level2OrdersColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid9 Order"), LVCFMT_CENTER, SizeWidth); colType.push_back(BID9ORDERS); }

		    if (Level2PriceColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid8 Price"), LVCFMT_CENTER, PriceWidth); colType.push_back(BID8PRICE); }
		    if (Level2SizeColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid8 Size"), LVCFMT_CENTER, SizeWidth); colType.push_back(BID8SIZE); }
		    if (Level2OrdersColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid8Order"), LVCFMT_CENTER, SizeWidth); colType.push_back(BID8ORDERS); }

		    if (Level2PriceColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid7 Price"), LVCFMT_CENTER, PriceWidth); colType.push_back(BID7PRICE); }
		    if (Level2SizeColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid7 Size"), LVCFMT_CENTER, SizeWidth); colType.push_back(BID7SIZE); }
		    if (Level2OrdersColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid7 Order"), LVCFMT_CENTER, SizeWidth); colType.push_back(BID7ORDERS); }

		    if (Level2PriceColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid6 Price"), LVCFMT_CENTER, PriceWidth); colType.push_back(BID6PRICE); }
		    if (Level2SizeColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid6 Size"), LVCFMT_CENTER, SizeWidth); colType.push_back(BID6SIZE); }
		    if (Level2OrdersColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid6 Order"), LVCFMT_CENTER, SizeWidth); colType.push_back(BID6ORDERS); }
	    }

	    if (Level2PriceColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid5 Price"), LVCFMT_CENTER, PriceWidth); colType.push_back(BID5PRICE); }
	    if (Level2SizeColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid5 Size"), LVCFMT_CENTER, SizeWidth); colType.push_back(BID5SIZE); }
	    if (Level2OrdersColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid5 Order"), LVCFMT_CENTER, SizeWidth); colType.push_back(BID5ORDERS); }

	    if (Level2PriceColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid4 Price"), LVCFMT_CENTER, PriceWidth); colType.push_back(BID4PRICE); }
	    if (Level2SizeColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid4 Size"), LVCFMT_CENTER, SizeWidth); colType.push_back(BID4SIZE); }
	    if (Level2OrdersColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid4 Order"), LVCFMT_CENTER, SizeWidth); colType.push_back(BID4ORDERS); }

	    if (Level2PriceColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid3 Price"), LVCFMT_CENTER, PriceWidth); colType.push_back(BID3PRICE); }
	    if (Level2SizeColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid3 Size"), LVCFMT_CENTER, SizeWidth); colType.push_back(BID3SIZE); }
	    if (Level2OrdersColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid3 Order"), LVCFMT_CENTER, SizeWidth); colType.push_back(BID3ORDERS); }

	    if (Level2PriceColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid2 Price"), LVCFMT_CENTER, PriceWidth); colType.push_back(BID2PRICE); }
	    if (Level2SizeColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid2 Size"), LVCFMT_CENTER, SizeWidth); colType.push_back(BID2SIZE); }
	    if (Level2OrdersColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid2 Order"), LVCFMT_CENTER, SizeWidth); colType.push_back(BID2ORDERS); }

	    if (Level2PriceColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid1 Price"), LVCFMT_CENTER, PriceWidth); colType.push_back(BID1PRICE); }
	    if (Level2SizeColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid1 Size"), LVCFMT_CENTER, SizeWidth); colType.push_back(BID1SIZE); }
	    if (Level2OrdersColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid1 Order"), LVCFMT_CENTER, SizeWidth); colType.push_back(BID1ORDERS); }
    }

	if (tradePriceColumn) { m_listCtrl.InsertColumn(columnNumber++, TRADEPRICECOLUMN, LVCFMT_CENTER, TradePriceWidth); colType.push_back(TRADEPRICE); }
	if (tradeSizeColumn) { m_listCtrl.InsertColumn(columnNumber++, TRADESIZECOLUMN, LVCFMT_CENTER, TradeSizeWidth); colType.push_back(TRADESIZE); }
	if (tradeCondition) { m_listCtrl.InsertColumn(columnNumber++, _T("Trade Condition"), LVCFMT_CENTER, TradePriceWidth); colType.push_back(CONDITION); }
    if (showInterestColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Interest Size"), LVCFMT_CENTER, TradePriceWidth); colType.push_back(INTEREST); }

    if (!showOnlyTrades)
    {
	    if (Level2PriceColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask1 Price"), LVCFMT_CENTER, PriceWidth); colType.push_back(ASK1PRICE); }
	    if (Level2SizeColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask1 Size"), LVCFMT_CENTER, SizeWidth); colType.push_back(ASK1SIZE); }
	    if (Level2OrdersColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask1 Order"), LVCFMT_CENTER, SizeWidth); colType.push_back(ASK1ORDERS); }

	    if (Level2PriceColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask2 Price"), LVCFMT_CENTER, PriceWidth); colType.push_back(ASK2PRICE); }
	    if (Level2SizeColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask2 Size"), LVCFMT_CENTER, SizeWidth); colType.push_back(ASK2SIZE); }
	    if (Level2OrdersColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask2 Order"), LVCFMT_CENTER, SizeWidth); colType.push_back(ASK2ORDERS); }
	
	    if (Level2PriceColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask3 Price"), LVCFMT_CENTER, PriceWidth); colType.push_back(ASK3PRICE); }
	    if (Level2SizeColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask3 Size"), LVCFMT_CENTER, SizeWidth); colType.push_back(ASK3SIZE); }
	    if (Level2OrdersColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask3 Order"), LVCFMT_CENTER, SizeWidth); colType.push_back(ASK3ORDERS); }
	
	    if (Level2PriceColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask4 Price"), LVCFMT_CENTER, PriceWidth); colType.push_back(ASK4PRICE); }
	    if (Level2SizeColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask4 Size"), LVCFMT_CENTER, SizeWidth); colType.push_back(ASK4SIZE); }
	    if (Level2OrdersColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask4 Order"), LVCFMT_CENTER, SizeWidth); colType.push_back(ASK4ORDERS); }
	
	    if (Level2PriceColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask5 Price"), LVCFMT_CENTER, PriceWidth); colType.push_back(ASK5PRICE); }
	    if (Level2SizeColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask5 Size"), LVCFMT_CENTER, SizeWidth); colType.push_back(ASK5SIZE); }
	    if (Level2OrdersColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask5 Order"), LVCFMT_CENTER, SizeWidth); colType.push_back(ASK5ORDERS); }

	    if (instrumentPtr && instrumentPtr->GetDomLevelsCount() > 5) 
	    {

		    if (Level2PriceColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask6 Price"), LVCFMT_CENTER, PriceWidth); colType.push_back(ASK6PRICE); }
		    if (Level2SizeColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask6 Size"), LVCFMT_CENTER, SizeWidth); colType.push_back(ASK6SIZE); }
		    if (Level2OrdersColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask6 Order"), LVCFMT_CENTER, SizeWidth); colType.push_back(ASK6ORDERS); }
	
		    if (Level2PriceColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask7 Price"), LVCFMT_CENTER, PriceWidth); colType.push_back(ASK7PRICE); }
		    if (Level2SizeColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask7 Size"), LVCFMT_CENTER, SizeWidth); colType.push_back(ASK7SIZE); }
		    if (Level2OrdersColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask7 Order"), LVCFMT_CENTER, SizeWidth); colType.push_back(ASK7ORDERS); }

		    if (Level2PriceColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask8 Price"), LVCFMT_CENTER, PriceWidth); colType.push_back(ASK8PRICE); }
		    if (Level2SizeColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask8 Size"), LVCFMT_CENTER, SizeWidth); colType.push_back(ASK8SIZE); }
		    if (Level2OrdersColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask8Order"), LVCFMT_CENTER, SizeWidth); colType.push_back(ASK8ORDERS); }

		    if (Level2PriceColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask9 Price"), LVCFMT_CENTER, PriceWidth); colType.push_back(ASK9PRICE); }
		    if (Level2SizeColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask9 Size"), LVCFMT_CENTER, SizeWidth); colType.push_back(ASK9SIZE); }
		    if (Level2OrdersColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask9 Order"), LVCFMT_CENTER, SizeWidth); colType.push_back(ASK9ORDERS); }

		    if (Level2PriceColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask10 Price"), LVCFMT_CENTER, PriceWidth); colType.push_back(ASK10PRICE); }
		    if (Level2SizeColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask10 Size"), LVCFMT_CENTER, SizeWidth); colType.push_back(ASK10SIZE); }
		    if (Level2OrdersColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask10 Order"), LVCFMT_CENTER, SizeWidth); colType.push_back(ASK10ORDERS); }
	    }

        if (deltaLimitColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Bid Limit Delta"), LVCFMT_CENTER, SizeWidth); colType.push_back(DELTABIDLIMITS); }
        if (deltaLimitColumn) { m_listCtrl.InsertColumn(columnNumber++, _T("Ask Limit Delta"), LVCFMT_CENTER, SizeWidth); colType.push_back(DELTAASKLIMITS); }
    }

	if (cumulativeDelta) { m_listCtrl.InsertColumn(columnNumber++, _T("Buy Volume"), LVCFMT_CENTER, SizeWidth); colType.push_back(BUYVOL); }
	if (cumulativeDelta) { m_listCtrl.InsertColumn(columnNumber++, _T("Sell Volume"), LVCFMT_CENTER, SizeWidth); colType.push_back(SELLVOL); }

	return 0;
}

void QuoteRun::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// ограничение минимальных и максимальных размеров окна
	lpMMI->ptMinTrackSize.x = 100;
	lpMMI->ptMinTrackSize.y = 200;
	CFrameWnd::OnGetMinMaxInfo(lpMMI);
}

void QuoteRun::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);
	RECT rt;
	statusBar.GetClientRect(&rt);
	m_listCtrl.MoveWindow(0, 0, cx, cy-rt.bottom);	//нужно для того чтобы уместилась строка состояния
}

void QuoteRun::OnTimer(UINT_PTR nIDEvent)
{
	if (instrumentPtr && (subInstrumentPtr == NULL && timerIndex != instrumentPtr->GetAggregatedOrderbookSize()) || (subInstrumentPtr && timerIndex != subInstrumentPtr->GetAggregatedOrderbookSize()))
	{
		SetTimerIndex();
		FillTable();
	}
	else if (!instrumentPtr)
	{		
		if (fType == RITHMIC) ::AcquireSRWLockShared(&SRWLockRithmic);
		//if (fType == NINJATRADER) ::AcquireSRWLockShared(&srwLockNinja);

		switch (fType)
		{
		case RITHMIC:
			for (size_t x = 0; x < rithmic_data.size(); ++x)
			{
				if (!rithmic_data[x]->GetName().compare(name)) { instrumentPtr = rithmic_data[x]; break; }
			}
			break;
		case CQG:
			for (size_t x = 0; x < cqg_data.size(); ++x)
			{
				if (!cqg_data[x]->GetName().compare(name)) { instrumentPtr = cqg_data[x]; break; }
			}
			break;
		case NINJATRADER:
			for (size_t x = 0; x < ninja_data.size(); ++x)
			{
				if (!ninja_data[x]->GetName().compare(name)) { instrumentPtr = ninja_data[x]; break; }
			}
		case PLAZA2REPLAY:
        case PLAZA2:
            {
                Plaza2Instrument* instrument = NULL;
                instrument = Plaza2DB::GetInstance()->GetInstrumentPtrByName(name);
                if (instrument != NULL)
                {
                    instrumentPtr = new Plaza2TickerAdapterClass(instrument->GetKey());
                }
            }
			break;
		}

		//если установлена агрегация или фильтрация трейдов, то создаем новый вектор фильтрованных данных
		if (instrumentPtr && (!noAggregation || filteringTradeSize || showOnlyTrades || showOnlyBuyTrades || showOnlySellTrades))
        {
			SaveInstrumentPtrAndCreateNewInstrument(&instrumentPtr, &subInstrumentPtr);
        }

		if (fType == RITHMIC) ::ReleaseSRWLockShared(&SRWLockRithmic);
		//if (fType == NINJATRADER) ::ReleaseSRWLockShared(&srwLockNinja);

		Invalidate(false);
	}
	
	CFrameWnd::OnTimer(nIDEvent);
}

void QuoteRun::OnRButtonClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	//координата курсора
	POINT point;
    GetCursorPos(&point);
	
	//создаем контекстное меню
	CMenu menu;

	menu.LoadMenuW(MAKEINTRESOURCE(IDR_CONTEXTMENU_DIGITFLOW));

	if (freeze)
		menu.GetSubMenu(0)->CheckMenuItem(ID_DFMENU_FREEZE, MF_CHECKED);
	if (alwaysOnTop)
		menu.GetSubMenu(0)->CheckMenuItem(ID_DFMENU_ALWAYSONTOP, MF_CHECKED);

	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON, point.x, point.y, this);
}

void QuoteRun::ContextMenu(UINT nID)
{
	if (instrumentPtr != NULL)
	{
		switch (nID)
		{
		case ID_DFMENU_FREEZE:
			if (!freeze) 
			{
				KillTimer(3);
			} 
			else 
			{
				SetTimer(3, 150, 0);
			}
			freeze = !freeze;
			break;
		case ID_DFMENU_ALWAYSONTOP:
			if (!alwaysOnTop)
				SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);		//ставим always on top	
			else SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);	//убираем always on top
			alwaysOnTop = !alwaysOnTop;
			break;
		case ID_DFMENU_SETTINGS:
			settingsWnd = new QuoteRunSettings(this);
			settingsWnd->DoModal();
			break;
		}
	}
}

//отрисовка фона и текста
void QuoteRun::OnNMCustomdrawLvCustomDraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCUSTOMDRAW pNMCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	
	if (!instrumentPtr)
	{
		// Будем выполнять стандартную обработку для всех сообщений по умолчанию
		*pResult = CDRF_DODEFAULT;
		return;
	}

	static HBITMAP hbmBuff;
	static HDC hdcBuff;

	const unsigned int CDDS_SUBITEMPREPAINT = CDDS_ITEM|CDDS_SUBITEM|CDDS_PREPAINT;

	if (pNMCD->nmcd.dwDrawStage == CDDS_PREPAINT)                               // пришло уведомление о начале отрисовки
	{
		CRect r;
        m_listCtrl.GetClientRect(&r);

        // создаем вспомогательный bitmap и контекст устройства, совместимый с параметрами desktop
		hbmBuff = ::CreateCompatibleBitmap(pNMCD->nmcd.hdc, r.Width(), r.Height());
        hdcBuff = ::CreateCompatibleDC(pNMCD->nmcd.hdc);
        ::SelectObject(hdcBuff, hbmBuff);

		::SelectObject(hdcBuff, ::GetStockObject(DC_PEN));
		::SelectObject(hdcBuff, ::GetStockObject(DC_BRUSH));

		::SetBkMode(hdcBuff, TRANSPARENT); //чтобы фон текста совпадал с фоном ячейки
		::SelectObject(hdcBuff, (HFONT)listFont);	//шрифт
       
		::SetDCBrushColor(hdcBuff, defaultbackgroundColor);
		::FillRect(hdcBuff, &r, (HBRUSH)::GetStockObject(DC_BRUSH));
		
        // запрашиваем уведомление об окончании отрисовки
        // контрола и начале отрисовки итемов
        *pResult = CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYITEMDRAW;
        return;

		//если рисуется весь элемент целиком - запрашиваем получение сообщений
		//для каждого элемента списка.
		//*pResult = CDRF_NOTIFYITEMDRAW;
		//return;
	
	} 
    else if (pNMCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)                    // пришло уведомление о pre-paint stage для итема
    {
		
		//LVITEM lvi;
		//memset((void*)&lvi, 0, sizeof( lvi ));

  //      int num = static_cast<int>(pNMCD->nmcd.dwItemSpec);
  //      TCHAR name[64];        

  //      // получаем текст и иконку
  //      lvi.mask       = LVIF_TEXT | LVIF_IMAGE;
  //      lvi.iItem      = num;
  //      lvi.pszText    = name;
  //      lvi.cchTextMax = sizeof( name );
  //      m_listCtrl.GetItem(&lvi);

  //      CRect        rIcon;
  //      CRect        rText;
  //      CRect        rItem;
  //     // CImageList* images;

  //      // получаем области вывода для частей итема
  //      m_listCtrl.GetItemRect(num, &rIcon, LVIR_ICON);
  //      m_listCtrl.GetItemRect(num, &rText, LVIR_LABEL);
  //      m_listCtrl.GetItemRect(num, &rItem, LVIR_BOUNDS);

        // различные настройки отображения взависимости от режима
        //int format = ;

        //long style = m_listCtrl.GetStyle() & LVS_TYPEMASK;

        //style != LVS_REPORT ? format |= DT_CENTER : format |= DT_LEFT;

        /*style == LVS_ICON ?
            images = m_listCtrl.GetImageList( LVSIL_NORMAL ):
            images = m_listCtrl.GetImageList( LVSIL_SMALL  );*/

        // рисуем иконку
        /*if(images)
        {
            CPoint pt;
            int    cx, cy;

            ::ImageList_GetIconSize( *images, &cx, &cy );

            pt.x = rIcon.left + ( rIcon.Width () - cx ) / 2;
            pt.y = rIcon.top  + ( rIcon.Height() - cy ) / 2;

            m_listCtrl.GetItemState( num, LVIS_SELECTED ) == LVIS_SELECTED ?
            images->Draw( CDC::FromHandle(hdcBuff), lvi.iImage, pt, ILD_TRANSPARENT|ILD_SELECTED ):
            images->Draw( CDC::FromHandle(hdcBuff), lvi.iImage, pt, ILD_TRANSPARENT              );
        }*/
        // выводим текст
       // m_listCtrl.GetItemState(num, LVIS_SELECTED) == LVIS_SELECTED ?
       //     ::SetTextColor(hdcBuff, ::GetSysColor(COLOR_HIGHLIGHTTEXT)) :
       //     ::SetTextColor(hdcBuff, defaultTextColor);

        // рисуем выделение, если необходимо
       /* if(m_listCtrl.GetItemState(num, LVIS_SELECTED) == LVIS_SELECTED)
            ::FillRect(hdcBuff, &rText, CBrush(::GetSysColor( COLOR_HIGHLIGHT)));

        ::SetBkMode(hdcBuff, TRANSPARENT);
		::SelectObject(hdcBuff, (HFONT)listFont);
        ::DrawText(hdcBuff, name, _tcslen(name), &rText, DT_WORDBREAK|DT_WORD_ELLIPSIS|DT_CENTER);*/

        *pResult |= CDRF_NOTIFYSUBITEMDRAW; //уведомление для перерисовки каждого субэлемента
        return;
		
		//*pResult = CDRF_NOTIFYSUBITEMDRAW; //уведомление для перерисовки каждого субэлемента
        //return;

	} 
	else if( CDDS_SUBITEMPREPAINT == pNMCD->nmcd.dwDrawStage )
    {
        int num = static_cast<int>(pNMCD->nmcd.dwItemSpec);
		TCHAR  name[64] = {0};
       
		//LVITEM lvi;
		//memset((void*)&lvi, 0, sizeof( lvi ));
		// получаем текст и иконку
        /* lvi.mask       = LVIF_TEXT;
        lvi.iItem      = num;
        lvi.pszText    = name;
        lvi.cchTextMax = sizeof(name);
        lvi.iSubItem = pNMCD->iSubItem;
        m_listCtrl.GetItem(&lvi);*/
        
		 // получаем области вывода для частей итема
		//CRect       rText;
        CRect       rItem; 
       
        //m_listCtrl.GetSubItemRect(num,pNMCD->iSubItem,LVIR_LABEL,rText);	//границы текста
        m_listCtrl.GetSubItemRect(num,pNMCD->iSubItem,LVIR_BOUNDS,rItem);	//границы ячейки		
    
		if (rItem.Width()) //если колонка сужена до нуля, то не рисуем её
		{
			COLORREF crText, crBkgnd;
			size_t Indx = topIndex - 1 - pNMCD->nmcd.dwItemSpec;
			TCHAR colName[64];
			LVCOLUMNW col;
		
			col.mask = LVCF_TEXT;
			col.cchTextMax = 64;
			col.pszText = colName;	//текст заголовка столбца
			m_listCtrl.GetColumn(pNMCD->iSubItem, &col);
        
			//рисуем фон ячейки
			DefineColorsAndText(name, 64, crText, crBkgnd, pNMCD->iSubItem, Indx);
			::SetDCBrushColor(hdcBuff, crBkgnd);
			::FillRect(hdcBuff, &rItem, (HBRUSH)::GetStockObject(DC_BRUSH));

			//прямоугольник вокруг ячейки (сетка)
			if (showGridLines)
			{
				rItem.top -= 1; rItem.right += 1;
				::SetDCBrushColor(hdcBuff, gridlineColor);
				::FrameRect(hdcBuff, &rItem, (HBRUSH)::GetStockObject(DC_BRUSH));
				rItem.top += 1; rItem.right -= 1;
			}
        
			// рисуем выделение, если необходимо
			if(m_listCtrl.GetItemState(num, LVIS_SELECTED) == LVIS_SELECTED)
			{
				rItem.right += 1;
				::SetDCBrushColor(hdcBuff, ::GetSysColor(COLOR_MENU));
				::FrameRect(hdcBuff, &rItem, (HBRUSH)::GetStockObject(DC_BRUSH));
			}

			// выводим текст
			//m_listCtrl.GetItemState(num, LVIS_SELECTED ) == LVIS_SELECTED ?
			//	::SetTextColor(hdcBuff, ::GetSysColor(COLOR_HIGHLIGHTTEXT)) :
				::SetTextColor(hdcBuff, crText);
				
			::DrawTextW(hdcBuff, name, (int)_tcslen(name), &rItem, DT_SINGLELINE|DT_END_ELLIPSIS|DT_CENTER|DT_VCENTER);
		}

		// посылаем сигнал того, что итем нами отрисован
        *pResult = CDRF_SKIPDEFAULT;
        return;
    }	
	else if (CDDS_POSTPAINT == pNMCD->nmcd.dwDrawStage)
    {
		CRect r;
        GetClientRect(&r);

		//если идет процесс закачки истории, то затемняем экран и отображаем надпись
		if ((subInstrumentPtr == NULL && instrumentPtr->GetHistoryFlag() == 1) || (subInstrumentPtr != NULL && subInstrumentPtr->GetHistoryFlag() == 1))
		{
			// создаём временный буфер для вне-экранного рисования
			//int BufWidth = r.Width();
			//int BufHeight = r.Height();
			//BITMAPINFO bi;
			//bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			//bi.bmiHeader.biWidth = BufWidth;
			//bi.bmiHeader.biHeight = BufHeight;
			//bi.bmiHeader.biPlanes = 1;
			//bi.bmiHeader.biBitCount = 32;
			//bi.bmiHeader.biCompression = BI_RGB;
			//bi.bmiHeader.biSizeImage = 0;
			//bi.bmiHeader.biXPelsPerMeter = 0;
			//bi.bmiHeader.biYPelsPerMeter = 0;
			//bi.bmiHeader.biClrUsed = 0;
			//bi.bmiHeader.biClrImportant = 0;
			//void* pBufPixels;
			//HBITMAP hBuf = CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, &pBufPixels, NULL, 0);

			//// заполняем буфер прозрачным цветом
			//int NumPixels = BufHeight * BufWidth;
			//unsigned int* pPix = static_cast<unsigned int*>(pBufPixels);
			//
			//for (int i = 0; i < NumPixels; i++)
			//{
			//	*pPix++ = RGB(0,0,0);
			//}

			//// рисуем многоугольник в буфер
			//HDC hBufDC = CreateCompatibleDC(NULL);
			//::SelectObject(hBufDC, hBuf);
			////::SelectObject(hBufDC, GetStockObject(DC_BRUSH));
			////SetDCBrushColor(hBufDC, FillColor);
			//::SelectObject(hBufDC, GetStockObject(NULL_PEN)); // контур не рисовать
			////SetWindowOrgEx(hBufDC, MinX, MinY, NULL);
			////Polygon(hBufDC, pVerts, NumVerts);
			//::FillRect(hBufDC, &r, (HBRUSH)GetStockObject(DC_BRUSH));
			//SetWindowOrgEx(hBufDC, 0, 0, NULL);

			//// приводим содержимое буфера к формату с premultiplied alpha
			//pPix = static_cast<unsigned int*>(pBufPixels);
			//for (int i = 0; i < NumPixels; i++)
			//{
			//	unsigned int c = *pPix;
			//	if (c == RGB(0,0,0))
			//	{
			//		*pPix = 0;
			//	}
			//	else
			//	{
			//		c |= 0xFF000000; // сделать alpha = 255
			//		*pPix = c;
			//	}
			//	pPix++;
			//}

			//// рисуем содержимое буфера в указанный DC, со смешиванием (blending) цветов
			//BLENDFUNCTION bf;
			//bf.BlendOp = AC_SRC_OVER;
			//bf.BlendFlags = 0;
			//bf.SourceConstantAlpha = 128;	//степень прозрачности (0-255)
			//bf.AlphaFormat = AC_SRC_ALPHA;
			//AlphaBlend(hdcBuff, 0, 0, BufWidth, BufHeight, hBufDC, 0, 0, BufWidth, BufHeight, bf);

			//DeleteDC(hBufDC);
			//DeleteObject(hBuf)

			DrawHistoryLoad(hdcBuff, r);
		}
		//если инструмент не подписан или произошел дисконнект
		//if ((_i_old == NULL && !((RTicker*)_i)->GetMode().compare("Disconnect")) || (_i_old != NULL && !((RTicker*)_i_old)->GetMode().compare("Disconnect")))
		//{
		//	//выводим текст
		//	::SetTextColor(hdcBuff, RGB(0,0,0));	//черный текст
		//	::SetBkColor(hdcBuff, RGB(255,0,0));	//красный фон
		//	::SetBkMode(hdcBuff, OPAQUE);
		//	::SetTextAlign(hdcBuff, TA_NOUPDATECP | TA_CENTER | TA_BASELINE);
		//	::SelectObject(hdcBuff, (HFONT)::GetStockObject(DEFAULT_GUI_FONT));
		//	::TextOut(hdcBuff, r.right/2, r.bottom/2, L" Disconnect... ", 15);
		//}
		::BitBlt(pNMCD->nmcd.hdc, 0, 0, r.Width(), r.Height(), hdcBuff, 0, 0, SRCCOPY);
        DeleteObject(hbmBuff);
        DeleteDC(hdcBuff);
        *pResult = CDRF_SKIPDEFAULT;
        return;
    }
	//} else if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPOSTPAINT) {
	//	// If this item is selected, re-draw the icon in its normal
	//	// color (not blended with the highlight color).

	//	LVITEM rItem;
	//	int    nItem = static_cast<int>(pLVCD->nmcd.dwItemSpec);

	//	// Get the image index and state of this item.  Note that we need to
	//	// check the selected state manually.  The docs _say_ that the
	//	// item's state is in pLVCD->nmcd.uItemState, but during my testing
	//	// it was always equal to 0x0201, which doesn't make sense, since
	//	// the max CDIS_* constant in commctrl.h is 0x0100.

	//	ZeroMemory ( &rItem, sizeof(LVITEM) );
	//	rItem.mask  = LVIF_IMAGE | LVIF_STATE;
	//	rItem.iItem = nItem;
	//	rItem.stateMask = LVIS_SELECTED;
	//	m_listCtrl.GetItem(&rItem );

	//	// If this item is selected, redraw the icon with its normal colors.
	//	if ( rItem.state & LVIS_SELECTED )
 //       {
	//		CDC*  pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
	//		CRect rcIcon;

	//		// Get the rect that holds the item's icon.
	//		m_listCtrl.GetItemRect(nItem, &rcIcon, LVIR_ICON);

	//		// Draw the icon.
	//		m_imglist.Draw(pDC, rItem.iImage, rcIcon.TopLeft(),	ILD_TRANSPARENT);

	//		*pResult = CDRF_SKIPDEFAULT;
 //       }
 //   }
}
BOOL QuoteRun::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE; //был TRUE
	//return CFrameWnd::OnEraseBkgnd(pDC);
}

void QuoteRun::GetColors(COLORREF& crText, COLORREF& crBkgnd, size_t Indx, bool sizeCol)
{
	bool condition = false;
	
    if (alertTradeSize && sizeCol) {	//если стоит алерт на трейды
		switch (alertTradeCondition)
		{
		case 0: if (instrumentPtr->Get_tradeSize(Indx) <= alertTradeSizeValue) condition = true; break;
		case 1: if (instrumentPtr->Get_tradeSize(Indx) == alertTradeSizeValue) condition = true;  break;
		case 2: if (instrumentPtr->Get_tradeSize(Indx) >= alertTradeSizeValue) condition = true;  break;
		}
	} 
	
    if (condition) 
    { 
        crText = alertTradeSizeText; crBkgnd = alertTradeSizeBackgrnd; 
    } 
	else 
    {
        if (showOpenInterestChange == TRUE)
        {
            Plaza2Instrument* instrument = ((Plaza2TickerAdapterClass*)instrumentPtr)->GetInstrument();
            Plaza2Trade* trade = instrument->GetBook(Indx).trade;

            if (trade)
            {
                switch (trade->interestDir)
                {
                case UNDEFINED: crText = OINotChangeText; crBkgnd = OINotChangeBackground; break;
                case OPEN_POSITION: crText = OIIncreaseText; crBkgnd = OIIncreaseBackground; break;
                case CLOSE_POSITION: crText = OIDecreaseText; crBkgnd = OIDecreaseBackground; break;
                }
            }
        }
        else
        {
            switch (instrumentPtr->Get_agressorSide(Indx)) 
            {
            case 'B': crText = askTradeText; crBkgnd = askTradeBackgrnd; break;
            case 'S': crText = bidTradeText; crBkgnd = bidTradeBackgrnd; break;
            case '?': crText = noAgressorText; crBkgnd = noAgressorBackgrnd; break;
            }
        }
	}
}
void QuoteRun::GetBBOBidLimitsColor(COLORREF& crText, COLORREF& crBkgnd, size_t Indx)
{
	bool conditionAdd = false;
	bool conditionRemove = false;
	crText = defaultBidTextColor, crBkgnd = defaultBidBackgroundColor;

	if (alertDeltaLimitAdd)
	{
		switch (alertDeltaLimitAddCondition)
		{
		case 0: if (instrumentPtr->Get_deltaBidLimitSize(Indx) <= alertDeltaLimitAddValue) conditionAdd = true; break;
		case 1: if (instrumentPtr->Get_deltaBidLimitSize(Indx) == alertDeltaLimitAddValue) conditionAdd = true;  break;
		case 2: if (instrumentPtr->Get_deltaBidLimitSize(Indx) >= alertDeltaLimitAddValue) conditionAdd = true;  break;
		}
	}
	if (alertDeltaLimitRemove)
	{
		switch (alertDeltaLimitRemoveCondition)
		{
		case 0: if (-instrumentPtr->Get_deltaBidLimitSize(Indx) <= alertDeltaLimitAddValue) conditionRemove = true; break;
		case 1: if (-instrumentPtr->Get_deltaBidLimitSize(Indx) == alertDeltaLimitAddValue) conditionRemove = true;  break;
		case 2: if (-instrumentPtr->Get_deltaBidLimitSize(Indx) >= alertDeltaLimitAddValue) conditionRemove = true;  break;
		}
	}

	switch (instrumentPtr->Get_agressorSide(Indx)) 
	{
	case '2':
	case 'b':
		if (instrumentPtr->Get_deltaBidLimitSize(Indx) > 0) 
		{
			if (conditionAdd) { crText = alertDeltaLimitAddText; crBkgnd = alertDeltaLimitAddBackgrnd; }
			else { crText = addBidText; crBkgnd = addBidBackgrnd; }
		} 
		else if (instrumentPtr->Get_deltaBidLimitSize(Indx) < 0) 
		{
			if (conditionRemove) { crText = alertDeltaLimitRemoveText; crBkgnd = alertDeltaLimitRemoveBackgrnd; }
			else { crText = removeBidText; crBkgnd = removeBidBackgrnd; }
		}
		break;
	}
}
void QuoteRun::GetBBOAskLimitsColor(COLORREF& crText, COLORREF& crBkgnd, size_t Indx)
{
	bool conditionAdd = false;
	bool conditionRemove = false;
	crText = defaultAskTextColor, crBkgnd = defaultAskBackgroundColor;

	if (alertDeltaLimitAdd) 
	{
		switch (alertDeltaLimitAddCondition)
		{
		case 0: if (instrumentPtr->Get_deltaAskLimitSize(Indx) <= alertDeltaLimitAddValue) conditionAdd = true; break;
		case 1: if (instrumentPtr->Get_deltaAskLimitSize(Indx) == alertDeltaLimitAddValue) conditionAdd = true;  break;
		case 2: if (instrumentPtr->Get_deltaAskLimitSize(Indx) >= alertDeltaLimitAddValue) conditionAdd = true;  break;
		}
	}
	if (alertDeltaLimitRemove) 
	{
		switch (alertDeltaLimitRemoveCondition)
		{
		case 0: if (-instrumentPtr->Get_deltaAskLimitSize(Indx) <= alertDeltaLimitAddValue) conditionRemove = true; break;
		case 1: if (-instrumentPtr->Get_deltaAskLimitSize(Indx) == alertDeltaLimitAddValue) conditionRemove = true;  break;
		case 2: if (-instrumentPtr->Get_deltaAskLimitSize(Indx) >= alertDeltaLimitAddValue) conditionRemove = true;  break;
		}
	}

	switch (instrumentPtr->Get_agressorSide(Indx)) 
	{
	case '2':
	case 'a': 
		{
			int size = instrumentPtr->Get_deltaAskLimitSize(Indx);
			if (size > 0) 
			{
				if (conditionAdd) { crText = alertDeltaLimitAddText; crBkgnd = alertDeltaLimitAddBackgrnd; }
				else { crText = addAskText; crBkgnd = addAskBackgrnd; }
			} 
			else if (size < 0)
			{
				if (conditionRemove) { crText = alertDeltaLimitRemoveText; crBkgnd = alertDeltaLimitRemoveBackgrnd; }
				else { crText = removeAskText; crBkgnd = removeAskBackgrnd; }
			}
		}
		break;
	}
}
void QuoteRun::SelectDOMColor(COLORREF& crText, COLORREF& crBkgnd, int colNum, char agressorSide, size_t Indx)
{
	bool conditionAdd = false;
	bool conditionRemove = false;
	bool columnFlag = false;

	//цвета по-умолчанию для колонок с бидами и асками и проверяем номер колонки
	switch (agressorSide)
	{
	case 'a':
		crText = defaultAskTextColor, crBkgnd = defaultAskBackgroundColor;
		columnFlag = instrumentPtr->Get_eventAskLevel(Indx) & (1 << colNum) ? true : false;
		break;
	case 'b':
		crText = defaultBidTextColor, crBkgnd = defaultBidBackgroundColor;
		columnFlag = instrumentPtr->Get_eventBidLevel(Indx) & (1 << colNum) ? true : false;
		break;
	case 'x':
		columnFlag = instrumentPtr->Get_eventBidLevel(Indx) & (1 << colNum) ? true : false;
		break;
	}

	//проверяем для этого значения нужно цвет подобрать или нет
	if ((instrumentPtr->Get_agressorSide(Indx) == agressorSide || instrumentPtr->Get_agressorSide(Indx) == 'x') && columnFlag)
	{		
		//цвета по-умолчанию для добавления и снятия лимитов
		if (agressorSide == 'b') 
		{
			if (instrumentPtr->Get_deltaBidLimitSize(Indx, colNum) > 0) { crText = addBidText; crBkgnd = addBidBackgrnd; }
			else if (instrumentPtr->Get_deltaBidLimitSize(Indx, colNum) < 0) { crText = removeBidText; crBkgnd = removeBidBackgrnd; }

			if (alertDeltaLimitAdd) {	//цвета алертов для добавления лимитов
				switch (alertDeltaLimitAddCondition)
				{
				case 0: if (instrumentPtr->Get_deltaBidLimitSize(Indx, colNum) <= alertDeltaLimitAddValue) conditionAdd = true; break;
				case 1: if (instrumentPtr->Get_deltaBidLimitSize(Indx, colNum) == alertDeltaLimitAddValue) conditionAdd = true; break;
				case 2: if (instrumentPtr->Get_deltaBidLimitSize(Indx, colNum) >= alertDeltaLimitAddValue) conditionAdd = true; break;
				}

				if (instrumentPtr->Get_deltaBidLimitSize(Indx, colNum) > 0 && conditionAdd) { crText = alertDeltaLimitAddText; crBkgnd = alertDeltaLimitAddBackgrnd; }
			}
			if (alertDeltaLimitRemove) {	//цвета алертов для снятия лимитов
				switch (alertDeltaLimitRemoveCondition)
				{
				case 0: if (-instrumentPtr->Get_deltaBidLimitSize(Indx, colNum) <= alertDeltaLimitAddValue) conditionRemove = true; break;
				case 1: if (-instrumentPtr->Get_deltaBidLimitSize(Indx, colNum) == alertDeltaLimitAddValue) conditionRemove = true; break;
				case 2: if (-instrumentPtr->Get_deltaBidLimitSize(Indx, colNum) >= alertDeltaLimitAddValue) conditionRemove = true; break;
				}

				if (instrumentPtr->Get_deltaBidLimitSize(Indx, colNum) < 0 && conditionRemove) { crText = alertDeltaLimitRemoveText; crBkgnd = alertDeltaLimitRemoveBackgrnd; }
			}

			//мув ордера и алгоордера если установлено отображать
			if (moveOrderAddLiquid || moveOrderRemoveLiquid || algoOrderAddLiquid || algoOrderRemoveLiquid)
			{
				switch (fType)
				{
				case CQG:
					{
						int size = instrumentPtr->Get_deltaBidLimitSize(Indx, colNum);
						if (instrumentPtr->Get_orderType(Indx) == 2)	
						{
						
							if (moveOrderAddLiquid && size > 0)
								crText = moveOrderAddText, crBkgnd = moveOrderAddBackgrnd;
							else if (moveOrderRemoveLiquid &&  size < 0)
								crText = moveOrderRemoveText, crBkgnd = moveOrderRemoveBackgrnd;
						}						
						else if (instrumentPtr->Get_orderType(Indx) == 3)
						{
							if (algoOrderAddLiquid && size > 0)
								crText = algoOrderAddText, crBkgnd = algoOrderAddBackgrnd;
							else if (algoOrderRemoveLiquid && size < 0)
								crText = algoOrderRemoveText, crBkgnd = algoOrderRemoveBackgrnd;
						}
					}
					break;
				case RITHMIC:
					{
						int orderType = instrumentPtr->Get_orderType(Indx);
						if (moveOrderAddLiquid && orderType == 3)	crText = moveOrderAddText, crBkgnd = moveOrderAddBackgrnd;
						else if (moveOrderRemoveLiquid && orderType == 4)	crText = moveOrderRemoveText, crBkgnd = moveOrderRemoveBackgrnd;
						else if (algoOrderAddLiquid && orderType == 5)	crText = algoOrderAddText, crBkgnd = algoOrderAddBackgrnd;
						else if (algoOrderRemoveLiquid && orderType == 6)	crText = algoOrderRemoveText, crBkgnd = algoOrderRemoveBackgrnd;
					}
					break;
				}
			}
		} 
		else if (agressorSide == 'a')
		{
			int askLimitSize = instrumentPtr->Get_deltaAskLimitSize(Indx, colNum);
			if (askLimitSize > 0) { crText = addAskText; crBkgnd = addAskBackgrnd; }
			else if (askLimitSize < 0) { crText = removeAskText; crBkgnd = removeAskBackgrnd; }

			if (alertDeltaLimitAdd) {	//цвета алертов для добавления лимитов
				switch (alertDeltaLimitAddCondition)
				{
				case 0: if (askLimitSize <= alertDeltaLimitAddValue) conditionAdd = true; break;
				case 1: if (askLimitSize == alertDeltaLimitAddValue) conditionAdd = true; break;
				case 2: if (askLimitSize >= alertDeltaLimitAddValue) conditionAdd = true; break;
				}

				if (askLimitSize > 0 && conditionAdd) { crText = alertDeltaLimitAddText; crBkgnd = alertDeltaLimitAddBackgrnd; }
			}
			if (alertDeltaLimitRemove) {	//цвета алертов для снятия лимитов
				switch (alertDeltaLimitRemoveCondition)
				{
				case 0: if (-askLimitSize <= alertDeltaLimitAddValue) conditionRemove = true; break;
				case 1: if (-askLimitSize == alertDeltaLimitAddValue) conditionRemove = true; break;
				case 2: if (-askLimitSize >= alertDeltaLimitAddValue) conditionRemove = true; break;
				}

				if (askLimitSize < 0 && conditionRemove) { crText = alertDeltaLimitRemoveText; crBkgnd = alertDeltaLimitRemoveBackgrnd; }
			}

			//мув ордера и алгоордера если установлено отображать
			if (moveOrderAddLiquid || moveOrderRemoveLiquid || algoOrderAddLiquid || algoOrderRemoveLiquid)
			{
				int orderType = instrumentPtr->Get_orderType(Indx);
				switch (fType)
				{
				case CQG:
					if (orderType == 2)	
					{
						if (moveOrderAddLiquid && instrumentPtr->Get_deltaAskLimitSize(Indx, colNum) > 0)
							crText = moveOrderAddText, crBkgnd = moveOrderAddBackgrnd;
						else if (moveOrderRemoveLiquid &&  instrumentPtr->Get_deltaAskLimitSize(Indx, colNum) < 0)
							crText = moveOrderRemoveText, crBkgnd = moveOrderRemoveBackgrnd;
					}						
					else if (orderType == 3)
					{
						if (algoOrderAddLiquid && instrumentPtr->Get_deltaAskLimitSize(Indx, colNum) > 0)
							crText = algoOrderAddText, crBkgnd = algoOrderAddBackgrnd;
						else if (algoOrderRemoveLiquid && instrumentPtr->Get_deltaAskLimitSize(Indx, colNum) < 0)
							crText = algoOrderRemoveText, crBkgnd = algoOrderRemoveBackgrnd;
					}
					break;
				case RITHMIC:
						 if	(moveOrderAddLiquid		&& orderType == 3)	crText = moveOrderAddText,		crBkgnd = moveOrderAddBackgrnd;
					else if (moveOrderRemoveLiquid	&& orderType == 4)	crText = moveOrderRemoveText,	crBkgnd = moveOrderRemoveBackgrnd;
					else if (algoOrderAddLiquid		&& orderType == 5)	crText = algoOrderAddText,		crBkgnd = algoOrderAddBackgrnd;
					else if (algoOrderRemoveLiquid	&& orderType == 6)	crText = algoOrderRemoveText,	crBkgnd = algoOrderRemoveBackgrnd;
					break;
				}
			}
		}
	}	
}

void QuoteRun::DefineColorsAndText(TCHAR* name, int nameSize, COLORREF& crText, COLORREF& crBkgnd, int colNum, size_t Indx)
{
    crText = defaultTextColor, crBkgnd = defaultbackgroundColor;

	if (fType == RITHMIC) ::AcquireSRWLockShared(&SRWLockRithmic);
	//if (fType == NINJATRADER) ::AcquireSRWLockShared(&srwLockNinja);
		//while(!(b = TryAcquireSRWLockShared(&srwLockNinja)))
		
    switch (colType[colNum])
    {
    case TIME:
        StringCchPrintf(name, nameSize, L"%.2d:%.2d:%.2d.%0.6d ", instrumentPtr->Get_hour(Indx), instrumentPtr->Get_minute(Indx), instrumentPtr->Get_second(Indx), instrumentPtr->Get_microseconds(Indx));
        GetColors(crText, crBkgnd, Indx, 0);
        break;

    case DELTAMS:
        {
            UINT microDelta = instrumentPtr->Get_microdelta(Indx);
            if (microDelta > 999999)
                StringCchPrintf(name, nameSize, L"> 1sec ");
            else 
                StringCchPrintf(name, nameSize, L"%d ", microDelta);
            switch (alertDeltaMicrosecCondition)
            {
            case 0: if (microDelta <= alertDeltaMicrosecondValue) { crText = alertDeltaMicrosecondText; crBkgnd = alertDeltaMicrosecondBackgrnd; } break;
            case 1: if (microDelta == alertDeltaMicrosecondValue) { crText = alertDeltaMicrosecondText; crBkgnd = alertDeltaMicrosecondBackgrnd; } break;
            case 2: if (microDelta >= alertDeltaMicrosecondValue) { crText = alertDeltaMicrosecondText; crBkgnd = alertDeltaMicrosecondBackgrnd; } break;
            }
        }
        break;

    case TRADEPRICE:
        if (instrumentPtr->Get_tradePrice(Indx).IsPrice())
        {
            StringCchPrintf(name, nameSize, L"%s ", instrumentPtr->Get_tradePrice(Indx).GetWString(instrumentPtr->GetPrecision()));
            GetColors(crText, crBkgnd, Indx, 0);
        }
        break;

    case TRADESIZE:
        if (instrumentPtr->Get_tradeSize(Indx))
        {
            StringCchPrintf(name, nameSize, L"%u ", instrumentPtr->Get_tradeSize(Indx));
            GetColors(crText, crBkgnd, Indx, 1);
        }
        break;

    case INTEREST:
        if (instrumentPtr->Get_tradeSize(Indx))
        {
            StringCchPrintf(name, nameSize, L"%u ", instrumentPtr->GetInterestAfterTrade(Indx));
            GetColors(crText, crBkgnd, Indx, 1);
        }
        break;

    case CONDITION:
        if (instrumentPtr->Get_condition(Indx))
        {
            StringCchPrintf(name, nameSize, L"Spread Leg ");
            GetColors(crText, crBkgnd, Indx, 0);
        }
        break;

    case BUYVOL:
        if (instrumentPtr->Get_buyVolume(Indx))
        {
            StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_buyVolume(Indx));
            GetColors(crText, crBkgnd, Indx, 0);
        }
        break;

    case SELLVOL:
        if (instrumentPtr->Get_sellVolume(Indx))
        {
            StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_sellVolume(Indx));
            GetColors(crText, crBkgnd, Indx, 0);
        }
        break;

    case DELTABIDLIMITS:
        crText = defaultBidTextColor, crBkgnd = defaultBidBackgroundColor;
        {
            int sum = instrumentPtr->Get_deltaBidLimitSum(Indx);
            if ((instrumentPtr->Get_agressorSide(Indx) == 'b' || instrumentPtr->Get_agressorSide(Indx) == 'x') && sum != 0)
            {
                StringCchPrintf(name, nameSize, sum > 0 ? L"+%d " : L"%d ", sum);

                if (sum > 0) {
                    crText = addBidText; crBkgnd = addBidBackgrnd; 
                } else if (sum < 0) {
                    crText = removeBidText; crBkgnd = removeBidBackgrnd; 
                }
            }
        }
        break;

    case DELTAASKLIMITS:
        crText = defaultAskTextColor, crBkgnd = defaultAskBackgroundColor;
        {
            int sum = instrumentPtr->Get_deltaAskLimitSum(Indx);
            if ((instrumentPtr->Get_agressorSide(Indx) == 'a' || instrumentPtr->Get_agressorSide(Indx) == 'x') && sum != 0)
            {
                StringCchPrintf(name, nameSize, sum > 0 ? L"+%d " : L"%d ", sum);

                if (sum > 0) {
                    crText = addAskText; crBkgnd = addAskBackgrnd; 
                } else if (sum < 0) {
                    crText = removeAskText; crBkgnd = removeAskBackgrnd; 
                }
            }
        }
        break;

    case BID1PRICE: StringCchPrintf(name, nameSize, L"%s ", instrumentPtr->Get_bidPrice(Indx, 0).GetWString(instrumentPtr->GetPrecision())); SelectDOMColor(crText, crBkgnd, 0, 'b', Indx); break;
    case BID2PRICE: StringCchPrintf(name, nameSize, L"%s ", instrumentPtr->Get_bidPrice(Indx, 1).GetWString(instrumentPtr->GetPrecision())); SelectDOMColor(crText, crBkgnd, 1, 'b', Indx); break;
    case BID3PRICE: StringCchPrintf(name, nameSize, L"%s ", instrumentPtr->Get_bidPrice(Indx, 2).GetWString(instrumentPtr->GetPrecision())); SelectDOMColor(crText, crBkgnd, 2, 'b', Indx); break;
    case BID4PRICE: StringCchPrintf(name, nameSize, L"%s ", instrumentPtr->Get_bidPrice(Indx, 3).GetWString(instrumentPtr->GetPrecision())); SelectDOMColor(crText, crBkgnd, 3, 'b', Indx); break;
    case BID5PRICE: StringCchPrintf(name, nameSize, L"%s ", instrumentPtr->Get_bidPrice(Indx, 4).GetWString(instrumentPtr->GetPrecision())); SelectDOMColor(crText, crBkgnd, 4, 'b', Indx); break;
    case BID6PRICE: StringCchPrintf(name, nameSize, L"%s ", instrumentPtr->Get_bidPrice(Indx, 5).GetWString(instrumentPtr->GetPrecision())); SelectDOMColor(crText, crBkgnd, 5, 'b', Indx); break;
    case BID7PRICE: StringCchPrintf(name, nameSize, L"%s ", instrumentPtr->Get_bidPrice(Indx, 6).GetWString(instrumentPtr->GetPrecision())); SelectDOMColor(crText, crBkgnd, 6, 'b', Indx); break;
    case BID8PRICE: StringCchPrintf(name, nameSize, L"%s ", instrumentPtr->Get_bidPrice(Indx, 7).GetWString(instrumentPtr->GetPrecision())); SelectDOMColor(crText, crBkgnd, 7, 'b', Indx); break;
    case BID9PRICE: StringCchPrintf(name, nameSize, L"%s ", instrumentPtr->Get_bidPrice(Indx, 8).GetWString(instrumentPtr->GetPrecision())); SelectDOMColor(crText, crBkgnd, 8, 'b', Indx); break;
    case BID10PRICE: StringCchPrintf(name, nameSize, L"%s ", instrumentPtr->Get_bidPrice(Indx, 9).GetWString(instrumentPtr->GetPrecision())); SelectDOMColor(crText, crBkgnd, 9, 'b', Indx); break;

    case ASK1PRICE: StringCchPrintf(name, nameSize, L"%s ", instrumentPtr->Get_askPrice(Indx, 0).GetWString(instrumentPtr->GetPrecision())); SelectDOMColor(crText, crBkgnd, 0, 'a', Indx); break;
    case ASK2PRICE: StringCchPrintf(name, nameSize, L"%s ", instrumentPtr->Get_askPrice(Indx, 1).GetWString(instrumentPtr->GetPrecision())); SelectDOMColor(crText, crBkgnd, 1, 'a', Indx); break;
    case ASK3PRICE: StringCchPrintf(name, nameSize, L"%s ", instrumentPtr->Get_askPrice(Indx, 2).GetWString(instrumentPtr->GetPrecision())); SelectDOMColor(crText, crBkgnd, 2, 'a', Indx); break;
    case ASK4PRICE: StringCchPrintf(name, nameSize, L"%s ", instrumentPtr->Get_askPrice(Indx, 3).GetWString(instrumentPtr->GetPrecision())); SelectDOMColor(crText, crBkgnd, 3, 'a', Indx); break;
    case ASK5PRICE: StringCchPrintf(name, nameSize, L"%s ", instrumentPtr->Get_askPrice(Indx, 4).GetWString(instrumentPtr->GetPrecision())); SelectDOMColor(crText, crBkgnd, 4, 'a', Indx); break;
    case ASK6PRICE: StringCchPrintf(name, nameSize, L"%s ", instrumentPtr->Get_askPrice(Indx, 5).GetWString(instrumentPtr->GetPrecision())); SelectDOMColor(crText, crBkgnd, 5, 'a', Indx); break;
    case ASK7PRICE: StringCchPrintf(name, nameSize, L"%s ", instrumentPtr->Get_askPrice(Indx, 6).GetWString(instrumentPtr->GetPrecision())); SelectDOMColor(crText, crBkgnd, 6, 'a', Indx); break;
    case ASK8PRICE: StringCchPrintf(name, nameSize, L"%s ", instrumentPtr->Get_askPrice(Indx, 7).GetWString(instrumentPtr->GetPrecision())); SelectDOMColor(crText, crBkgnd, 7, 'a', Indx); break;
    case ASK9PRICE: StringCchPrintf(name, nameSize, L"%s ", instrumentPtr->Get_askPrice(Indx, 8).GetWString(instrumentPtr->GetPrecision())); SelectDOMColor(crText, crBkgnd, 8, 'a', Indx); break;
    case ASK10PRICE: StringCchPrintf(name, nameSize, L"%s ", instrumentPtr->Get_askPrice(Indx, 9).GetWString(instrumentPtr->GetPrecision())); SelectDOMColor(crText, crBkgnd, 9, 'a', Indx); break;

    case BID1SIZE: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_bidSize(Indx, 0)); SelectDOMColor(crText, crBkgnd, 0, 'b', Indx); break;
    case BID2SIZE: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_bidSize(Indx, 1)); SelectDOMColor(crText, crBkgnd, 1, 'b', Indx); break;
    case BID3SIZE: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_bidSize(Indx, 2)); SelectDOMColor(crText, crBkgnd, 2, 'b', Indx); break;
    case BID4SIZE: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_bidSize(Indx, 3)); SelectDOMColor(crText, crBkgnd, 3, 'b', Indx); break;
    case BID5SIZE: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_bidSize(Indx, 4)); SelectDOMColor(crText, crBkgnd, 4, 'b', Indx); break;
    case BID6SIZE: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_bidSize(Indx, 5)); SelectDOMColor(crText, crBkgnd, 5, 'b', Indx); break;
    case BID7SIZE: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_bidSize(Indx, 6)); SelectDOMColor(crText, crBkgnd, 6, 'b', Indx); break;
    case BID8SIZE: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_bidSize(Indx, 7)); SelectDOMColor(crText, crBkgnd, 7, 'b', Indx); break;
    case BID9SIZE: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_bidSize(Indx, 8)); SelectDOMColor(crText, crBkgnd, 8, 'b', Indx); break;
    case BID10SIZE: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_bidSize(Indx, 9)); SelectDOMColor(crText, crBkgnd, 9, 'b', Indx); break;

    case ASK1SIZE: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_askSize(Indx, 0)); SelectDOMColor(crText, crBkgnd, 0, 'a', Indx); break;
    case ASK2SIZE: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_askSize(Indx, 1)); SelectDOMColor(crText, crBkgnd, 1, 'a', Indx); break;
    case ASK3SIZE: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_askSize(Indx, 2)); SelectDOMColor(crText, crBkgnd, 2, 'a', Indx); break;
    case ASK4SIZE: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_askSize(Indx, 3)); SelectDOMColor(crText, crBkgnd, 3, 'a', Indx); break;
    case ASK5SIZE: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_askSize(Indx, 4)); SelectDOMColor(crText, crBkgnd, 4, 'a', Indx); break;
    case ASK6SIZE: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_askSize(Indx, 5)); SelectDOMColor(crText, crBkgnd, 5, 'a', Indx); break;
    case ASK7SIZE: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_askSize(Indx, 6)); SelectDOMColor(crText, crBkgnd, 6, 'a', Indx); break;
    case ASK8SIZE: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_askSize(Indx, 7)); SelectDOMColor(crText, crBkgnd, 7, 'a', Indx); break;
    case ASK9SIZE: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_askSize(Indx, 8)); SelectDOMColor(crText, crBkgnd, 8, 'a', Indx); break;
    case ASK10SIZE: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_askSize(Indx, 9)); SelectDOMColor(crText, crBkgnd, 9, 'a', Indx); break;

    case BID1ORDERS: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_bidOrders(Indx, 0)); SelectDOMColor(crText, crBkgnd, 0, 'b', Indx); break;
    case BID2ORDERS: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_bidOrders(Indx, 1)); SelectDOMColor(crText, crBkgnd, 1, 'b', Indx); break;
    case BID3ORDERS: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_bidOrders(Indx, 2)); SelectDOMColor(crText, crBkgnd, 2, 'b', Indx); break;
    case BID4ORDERS: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_bidOrders(Indx, 3)); SelectDOMColor(crText, crBkgnd, 3, 'b', Indx); break;
    case BID5ORDERS: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_bidOrders(Indx, 4)); SelectDOMColor(crText, crBkgnd, 4, 'b', Indx); break;
    case BID6ORDERS: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_bidOrders(Indx, 5)); SelectDOMColor(crText, crBkgnd, 5, 'b', Indx); break;
    case BID7ORDERS: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_bidOrders(Indx, 6)); SelectDOMColor(crText, crBkgnd, 6, 'b', Indx); break;
    case BID8ORDERS: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_bidOrders(Indx, 7)); SelectDOMColor(crText, crBkgnd, 7, 'b', Indx); break;
    case BID9ORDERS: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_bidOrders(Indx, 8)); SelectDOMColor(crText, crBkgnd, 8, 'b', Indx); break;
    case BID10ORDERS: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_bidOrders(Indx, 9)); SelectDOMColor(crText, crBkgnd, 9, 'b', Indx); break;

    case ASK1ORDERS: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_askOrders(Indx, 0)); SelectDOMColor(crText, crBkgnd, 0, 'a', Indx); break;
    case ASK2ORDERS: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_askOrders(Indx, 1)); SelectDOMColor(crText, crBkgnd, 1, 'a', Indx); break;
    case ASK3ORDERS: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_askOrders(Indx, 2)); SelectDOMColor(crText, crBkgnd, 2, 'a', Indx); break;
    case ASK4ORDERS: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_askOrders(Indx, 3)); SelectDOMColor(crText, crBkgnd, 3, 'a', Indx); break;
    case ASK5ORDERS: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_askOrders(Indx, 4)); SelectDOMColor(crText, crBkgnd, 4, 'a', Indx); break;
    case ASK6ORDERS: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_askOrders(Indx, 5)); SelectDOMColor(crText, crBkgnd, 5, 'a', Indx); break;
    case ASK7ORDERS: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_askOrders(Indx, 6)); SelectDOMColor(crText, crBkgnd, 6, 'a', Indx); break;
    case ASK8ORDERS: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_askOrders(Indx, 7)); SelectDOMColor(crText, crBkgnd, 7, 'a', Indx); break;
    case ASK9ORDERS: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_askOrders(Indx, 8)); SelectDOMColor(crText, crBkgnd, 8, 'a', Indx); break;
    case ASK10ORDERS: StringCchPrintf(name, nameSize, L"%d ", instrumentPtr->Get_askOrders(Indx, 9)); SelectDOMColor(crText, crBkgnd, 9, 'a', Indx); break;
    }	

	if (fType == RITHMIC) ::ReleaseSRWLockShared(&SRWLockRithmic);
	//if (fType == NINJATRADER) ::ReleaseSRWLockShared(&srwLockNinja);
}

void QuoteRun::FillAggregationData()
{
	static bool conditionTrue = false;
	static bool newTrade = false;

	if (fType == PLAZA2)
	{
		Plaza2TickerAdapterClass* adapter = (Plaza2TickerAdapterClass*)subInstrumentPtr;
		size_t sizeOfTradeArray = adapter->GetInstrument()->NonAggregatedTradesArraySize();
		while (syncIndex < sizeOfTradeArray)
		{
			Plaza2Book book;
			Plaza2Trade* trade = &adapter->GetInstrument()->GetNonAggregatedTrade(syncIndex);
					
			book.trade = trade;
			book.agressorSide = trade->agressorSide;
			book.timestamp = trade->timestamp;
			
			//проверяем наличие галочек BuyOnly и SellOnly и фильтрации
			if ((!showOnlyBuyTrades && !showOnlySellTrades) || (showOnlyBuyTrades && showOnlySellTrades))	//показывать и покупки и продажи
			{ 
				if (filteringTradeValue)
				{
					switch (filteringTradeCondition)
					{
					case 0: if (trade->size <= filteringTradeValue) instrumentPtr->push_back((void*)&book); break;
					case 1: if (trade->size == filteringTradeValue) instrumentPtr->push_back((void*)&book); break;
					case 2: if (trade->size >= filteringTradeValue) instrumentPtr->push_back((void*)&book); break;
					}
				}
				else
					instrumentPtr->push_back((void*)&book);
			}
			else if (showOnlyBuyTrades && !showOnlySellTrades && trade->agressorSide == 'B')	//только покупки
			{
				if (filteringTradeValue)
				{
					switch (filteringTradeCondition)
					{
					case 0: if (trade->size <= filteringTradeValue) instrumentPtr->push_back((void*)&book); break;
					case 1: if (trade->size == filteringTradeValue) instrumentPtr->push_back((void*)&book); break;
					case 2: if (trade->size >= filteringTradeValue) instrumentPtr->push_back((void*)&book); break;
					}
				}
				else
					instrumentPtr->push_back((void*)&book); 
			}
			else if (!showOnlyBuyTrades && showOnlySellTrades && trade->agressorSide == 'S') //только продажи
			{
				if (filteringTradeValue)
				{
					switch (filteringTradeCondition)
					{
					case 0: if (trade->size <= filteringTradeValue) instrumentPtr->push_back((void*)&book); break;
					case 1: if (trade->size == filteringTradeValue) instrumentPtr->push_back((void*)&book); break;
					case 2: if (trade->size >= filteringTradeValue) instrumentPtr->push_back((void*)&book); break;
					}
				}
				else
					instrumentPtr->push_back((void*)&book);
			}

			++syncIndex;
		}

		return;
	}

	while (syncIndex < subInstrumentPtr->GetAggregatedOrderbookSize()) 
	{				
		size_t j = syncIndex, y = 0;
		if (instrumentPtr->GetAggregatedOrderbookSize())
			y = instrumentPtr->GetAggregatedOrderbookSize()-1;
		
		//это трейд
		if (subInstrumentPtr->Get_tradeSize(j)) 
		{
			//первый трейд в группе просто записываем
			if (!newTrade) 
			{
					 if ((!showOnlyBuyTrades && !showOnlySellTrades) || (showOnlyBuyTrades && showOnlySellTrades))
				{ instrumentPtr->push_back(subInstrumentPtr->get_data(j)); newTrade = true; }
				else if (showOnlyBuyTrades && !showOnlySellTrades && subInstrumentPtr->Get_agressorSide(syncIndex) == 'B')
				{ instrumentPtr->push_back(subInstrumentPtr->get_data(j)); newTrade = true; }
				else if (!showOnlyBuyTrades && showOnlySellTrades && subInstrumentPtr->Get_agressorSide(syncIndex) == 'S')
				{ instrumentPtr->push_back(subInstrumentPtr->get_data(j)); newTrade = true; }

				++syncIndex;
				continue;
			}
			
			//разница в микросекундах не более 1000 и сторона агрессора у соседних трейдов одинаковая, в том числе и имплайдная
			if (instrumentPtr->Get_agressorSide(y) == subInstrumentPtr->Get_agressorSide(j) && subInstrumentPtr->Get_microdelta(j) < 1000u)
				instrumentPtr->Add_tradeSize(y, subInstrumentPtr->Get_tradeSize(j));
			else
			{
				//если есть фильтрация то проверяем этот трейд
				if (filteringTradeSize && filteringTradeValue)
				{
					switch (filteringTradeCondition)
					{
					case 0: if (instrumentPtr->Get_tradeSize(y) <= filteringTradeValue) conditionTrue = true; break;
					case 1: if (instrumentPtr->Get_tradeSize(y) == filteringTradeValue) conditionTrue = true; break;
					case 2: if (instrumentPtr->Get_tradeSize(y) >= filteringTradeValue) conditionTrue = true; break;
					}

					if (!conditionTrue)
						instrumentPtr->erase_data(y);

					conditionTrue = false;
				}

				newTrade = false;
			}			
		}
		else
		{
			if (newTrade && filteringTradeSize && instrumentPtr->Get_tradeSize(y)) //эта проверка нужна, если был один первый трейд и сразу после него лимит идет
			{
				switch (filteringTradeCondition)
				{
				case 0: if (instrumentPtr->Get_tradeSize(y) <= filteringTradeValue) conditionTrue = true; break;
				case 1: if (instrumentPtr->Get_tradeSize(y) == filteringTradeValue) conditionTrue = true; break;
				case 2: if (instrumentPtr->Get_tradeSize(y) >= filteringTradeValue) conditionTrue = true; break;
				}

				if (!conditionTrue)
					instrumentPtr->erase_data(y);
					
				conditionTrue = false;
			}

			newTrade = false;
				
			if (!showOnlyTrades) //если отображаются лимиты, то заносим текущий лимит
				instrumentPtr->push_back(subInstrumentPtr->get_data(j));
		}

		++syncIndex;
	}
}
void QuoteRun::FillFilteredData()
{
	bool conditionTrue = false;

	while (syncIndex < subInstrumentPtr->GetAggregatedOrderbookSize()) 
	{				
		size_t j = syncIndex;
		
		if (subInstrumentPtr->Get_tradeSize(j)) 
		{					
			switch (filteringTradeCondition)
			{
			case 0: if (subInstrumentPtr->Get_tradeSize(j) <= filteringTradeValue) conditionTrue = true; break;
			case 1: if (subInstrumentPtr->Get_tradeSize(j) == filteringTradeValue) conditionTrue = true; break;
			case 2: if (subInstrumentPtr->Get_tradeSize(j) >= filteringTradeValue) conditionTrue = true; break;
			}
					
			if (conditionTrue) 
			{
				if ((!showOnlyBuyTrades && !showOnlySellTrades) || (showOnlyBuyTrades && showOnlySellTrades))
					instrumentPtr->push_back(subInstrumentPtr->get_data(j));
				else if (showOnlyBuyTrades && !showOnlySellTrades && subInstrumentPtr->Get_agressorSide(syncIndex) == 'B')
					instrumentPtr->push_back(subInstrumentPtr->get_data(j));
				else if (!showOnlyBuyTrades && showOnlySellTrades && subInstrumentPtr->Get_agressorSide(syncIndex) == 'S')
					instrumentPtr->push_back(subInstrumentPtr->get_data(j));

				conditionTrue = false;
			}
		}
		else if (!showOnlyTrades)
        {
			instrumentPtr->push_back(subInstrumentPtr->get_data(j));
        }

		++syncIndex;
	}
}

void QuoteRun::FillTradesOnly()
{
    while (syncIndex < subInstrumentPtr->GetAggregatedOrderbookSize())
    {				
        if (subInstrumentPtr->Get_tradeSize(syncIndex))
        {
            if ((!showOnlyBuyTrades && !showOnlySellTrades) || (showOnlyBuyTrades && showOnlySellTrades))
            {
                instrumentPtr->push_back(subInstrumentPtr->get_data(syncIndex));
            }
            else if (showOnlyBuyTrades && !showOnlySellTrades && subInstrumentPtr->Get_agressorSide(syncIndex) == 'B')
            {
                instrumentPtr->push_back(subInstrumentPtr->get_data(syncIndex));
            }
            else if (!showOnlyBuyTrades && showOnlySellTrades && subInstrumentPtr->Get_agressorSide(syncIndex) == 'S')
            {
                instrumentPtr->push_back(subInstrumentPtr->get_data(syncIndex));
            }
        }

        ++syncIndex;
    }
}

void QuoteRun::FillTable()
{
    if (fType != PLAZA2)
    {
	    if (fType == RITHMIC) ::AcquireSRWLockShared(&SRWLockRithmic);
	    //if (fType == NINJATRADER) ::AcquireSRWLockShared(&srwLockNinja);
	    //while(!TryAcquireSRWLockShared(&srwLockNinja))

        // нет агрегации, нет фильтрации и отображаются только трейды
	    if (noAggregation && !filteringTradeSize && (showOnlyTrades || showOnlyBuyTrades || showOnlySellTrades))
	    {
		    FillTradesOnly();
	    }

        // нет агрегации, но есть фильтрация
	    if (noAggregation && filteringTradeSize && filteringTradeValue)         
        {
		    FillFilteredData();
        }

        // есть агрегация
	    if (!noAggregation)
        {
		    FillAggregationData();
        }

	    if (fType == RITHMIC) ::ReleaseSRWLockShared(&SRWLockRithmic);
	    //if (fType == NINJATRADER) ::ReleaseSRWLockShared(&srwLockNinja);
    }
    else /*if (fType == PLAZA2)*/
    {        
        Plaza2Instrument* instrument = NULL;
        Plaza2Book book;
        Plaza2Trade* trade = NULL;
        size_t arraySize;
        if (subInstrumentPtr != NULL)
        {
            instrument = static_cast<Plaza2TickerAdapterClass*>(subInstrumentPtr)->GetInstrument();
        }
        else
        {
            instrument = static_cast<Plaza2TickerAdapterClass*>(instrumentPtr)->GetInstrument();
        }
        
        // нет агрегации, но включено отображение только трейдов или есть фильтрация
        if (noAggregation && (showOnlyTrades || showOnlyBuyTrades || showOnlySellTrades))
        {
            arraySize = instrument->NonAggregatedTradesArraySize();
            if (syncIndex > arraySize)
            {
                size_t maxSize = instrument->NonAggregatedTradesArrayMaxSize();
                syncIndex = maxSize/2 - (maxSize - syncIndex);
            }

            while (syncIndex < arraySize)
            {	
                trade = &instrument->GetNonAggregatedTrade(syncIndex);
                if (filteringTradeSize == FALSE) // нет фильтрации
                {
                    book.timestamp = trade->timestamp;
                    book.agressorSide = trade->agressorSide;
                    book.trade = trade;
                }
                else
                {
                    bool res = false;
                    switch (filteringTradeCondition)
                    {
                    case 0: if (trade->size <= filteringTradeValue) res = true; break;
                    case 1: if (trade->size == filteringTradeValue) res = true; break;
                    case 2: if (trade->size >= filteringTradeValue) res = true; break;
                    }

                    if (!res) 
                    {
                        ++syncIndex;
                        continue;
                    }

                    book.timestamp = trade->timestamp;
                    book.agressorSide = trade->agressorSide;
                    book.trade = trade;
                }

                if ((!showOnlyBuyTrades && !showOnlySellTrades) || (showOnlyBuyTrades && showOnlySellTrades))
                {
                    instrumentPtr->push_back(&book);
                }
                else if (showOnlyBuyTrades && !showOnlySellTrades && trade->agressorSide == 'B')
                {
                    instrumentPtr->push_back(&book);
                }
                else if (!showOnlyBuyTrades && showOnlySellTrades && trade->agressorSide == 'S')
                {
                    instrumentPtr->push_back(&book);
                }

                ++syncIndex;
            }
        }
        else if (!noAggregation) // есть агрегация
        {
            arraySize = instrument->AggregatedTradesArraySize();
            if (syncIndex > arraySize)
            {
                size_t maxSize = instrument->AggregatedTradesArrayMaxSize();
                syncIndex = maxSize/2 - (maxSize - syncIndex);
            }

            while (syncIndex < arraySize)
            {	
                trade = &instrument->GetAggregatedTrade(syncIndex);

                if (!filteringTradeSize) // нет фильтрации
                {
                    book.timestamp = trade->timestamp;
                    book.agressorSide = trade->agressorSide;
                    book.trade = trade;
                }
                else
                {
                    bool res = false;
                    switch (filteringTradeCondition)
                    {
                    case 0: if (trade->size <= filteringTradeValue) res = true; break;
                    case 1: if (trade->size == filteringTradeValue) res = true; break;
                    case 2: if (trade->size >= filteringTradeValue) res = true; break;
                    }

                    if (!res) 
                    {
                        ++syncIndex;
                        continue;
                    }

                    book.timestamp = trade->timestamp;
                    book.agressorSide = trade->agressorSide;
                    book.trade = trade;
                }

                if ((!showOnlyBuyTrades && !showOnlySellTrades) || (showOnlyBuyTrades && showOnlySellTrades))
                {
                    instrumentPtr->push_back(&book);
                }
                else if (showOnlyBuyTrades && !showOnlySellTrades && trade->agressorSide == 'B')
                {
                    instrumentPtr->push_back(&book);
                }
                else if (!showOnlyBuyTrades && showOnlySellTrades && trade->agressorSide == 'S')
                {
                    instrumentPtr->push_back(&book);
                }

                ++syncIndex;
            }
        }   
    }

    topIndex = instrumentPtr->GetAggregatedOrderbookSize();

	//указываем списку, сколько элементов он содержит
	m_listCtrl.SetItemCountEx((int)topIndex, LVSICF_NOSCROLL);
}

void QuoteRun::OnClose()
{
    CFrameWnd::OnClose();
}

void QuoteRunSettings::OnBnClickedButtonAskTrade()
{
	CColorDialog clrDlg(parentWindowPtr->askTradeText, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->askTradeText = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButtonBidTradeText()
{
	CColorDialog clrDlg(parentWindowPtr->bidTradeText, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->bidTradeText = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton5()
{
	CColorDialog clrDlg(parentWindowPtr->noAgressorText, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->noAgressorText = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton7()
{
	CColorDialog clrDlg(parentWindowPtr->addAskText, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->addAskText = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton9()
{
	CColorDialog clrDlg(parentWindowPtr->removeAskText, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->removeAskText = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton11()
{
	CColorDialog clrDlg(parentWindowPtr->addBidText, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->addBidText = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton13()
{
	CColorDialog clrDlg(parentWindowPtr->removeBidText, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->removeBidText = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton2()
{
	CColorDialog clrDlg(parentWindowPtr->askTradeBackgrnd, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->askTradeBackgrnd = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton4()
{
	CColorDialog clrDlg(parentWindowPtr->bidTradeBackgrnd, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->bidTradeBackgrnd = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton6()
{
	CColorDialog clrDlg(parentWindowPtr->noAgressorBackgrnd, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->noAgressorBackgrnd = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton8()
{
	CColorDialog clrDlg(parentWindowPtr->addAskBackgrnd, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->addAskBackgrnd = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton10()
{
	CColorDialog clrDlg(parentWindowPtr->removeAskBackgrnd, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->removeAskBackgrnd = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton12()
{
	CColorDialog clrDlg(parentWindowPtr->addBidBackgrnd, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->addBidBackgrnd = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton14()
{
	CColorDialog clrDlg(parentWindowPtr->removeBidBackgrnd, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->removeBidBackgrnd = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton15()
{
	CColorDialog clrDlg(parentWindowPtr->moveOrderAddText, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->moveOrderAddText = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton17()
{
	CColorDialog clrDlg(parentWindowPtr->moveOrderRemoveText, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->moveOrderRemoveText = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton19()
{
	CColorDialog clrDlg(parentWindowPtr->algoOrderAddText, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->algoOrderAddText = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton21()
{
	CColorDialog clrDlg(parentWindowPtr->algoOrderRemoveText, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->algoOrderRemoveText = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton16()
{
	CColorDialog clrDlg(parentWindowPtr->moveOrderAddBackgrnd, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->moveOrderAddBackgrnd = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton18()
{
	CColorDialog clrDlg(parentWindowPtr->moveOrderRemoveBackgrnd, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->moveOrderRemoveBackgrnd = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton20()
{
	CColorDialog clrDlg(parentWindowPtr->algoOrderAddBackgrnd, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->algoOrderAddBackgrnd = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton22()
{
	CColorDialog clrDlg(parentWindowPtr->algoOrderRemoveBackgrnd, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->algoOrderRemoveBackgrnd = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton32()
{
	CColorDialog clrDlg(parentWindowPtr->defaultbackgroundColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->defaultbackgroundColor = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
		parentWindowPtr->m_listCtrl.SetBkColor(clrDlg.GetColor());
	}
}
void QuoteRunSettings::OnBnClickedButton33()
{
	CColorDialog clrDlg(parentWindowPtr->defaultTextColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->defaultTextColor = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton23()
{
	CColorDialog clrDlg(parentWindowPtr->alertTradeSizeText, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->alertTradeSizeText = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton25()
{
	CColorDialog clrDlg(parentWindowPtr->alertDeltaLimitAddText, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->alertDeltaLimitAddText = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton27()
{
	CColorDialog clrDlg(parentWindowPtr->alertDeltaLimitRemoveText, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->alertDeltaLimitRemoveText = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton29()
{
	CColorDialog clrDlg(parentWindowPtr->alertDeltaMicrosecondText, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->alertDeltaMicrosecondText = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton24()
{
	CColorDialog clrDlg(parentWindowPtr->alertTradeSizeBackgrnd, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->alertTradeSizeBackgrnd = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton26()
{
	CColorDialog clrDlg(parentWindowPtr->alertDeltaLimitAddBackgrnd, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->alertDeltaLimitAddBackgrnd = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton28()
{
	CColorDialog clrDlg(parentWindowPtr->alertDeltaLimitRemoveBackgrnd, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->alertDeltaLimitRemoveBackgrnd = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton30()
{
	CColorDialog clrDlg(parentWindowPtr->alertDeltaMicrosecondBackgrnd, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->alertDeltaMicrosecondBackgrnd = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton36()
{
	CColorDialog clrDlg(parentWindowPtr->defaultBidTextColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->defaultBidTextColor = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton37()
{
	CColorDialog clrDlg(parentWindowPtr->defaultAskTextColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->defaultAskTextColor = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton34()
{
	CColorDialog clrDlg(parentWindowPtr->defaultBidBackgroundColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->defaultBidBackgroundColor = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton35()
{
	CColorDialog clrDlg(parentWindowPtr->defaultAskBackgroundColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->defaultAskBackgroundColor = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}
void QuoteRunSettings::OnBnClickedButton38()
{
	CColorDialog clrDlg(parentWindowPtr->gridlineColor, CC_FULLOPEN, this);

	if (clrDlg.DoModal() == IDOK) {
		parentWindowPtr->gridlineColor = clrDlg.GetColor();
		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}

void QuoteRunSettings::OnBnClickedButtonFont()
{
	LOGFONT log; 
	
	parentWindowPtr->m_listCtrl.GetFont()->GetLogFont(&log);
	//dWnd->listFont.GetLogFont(&log);
	
	CFontDialog fontDlg(&log, CF_EFFECTS|CF_SCREENFONTS, NULL, this);

	if (fontDlg.DoModal() == IDOK) {
		TCHAR str[15];
		//static CFont font; 

		fontDlg.GetCurrentFont(&log);
		StringCchPrintf(str, 15, L"%.10s", log.lfFaceName);
		GetDlgItem(IDC_BUTTON_FONT)->SetWindowText(str);

		//font.DeleteObject();
		//font.CreateFontIndirect(&log); 
		//dWnd->m_listCtrl.SetFont(&dWnd->listFont, TRUE);

		parentWindowPtr->listFont.DeleteObject();
		parentWindowPtr->listFont.CreateFontIndirect(&log);
		parentWindowPtr->m_listCtrl.SetFont(&parentWindowPtr->listFont, TRUE);

		//HDC hDC = ::GetDC(dWnd->m_hWnd);
		//::SetBkMode(hdcBuff, TRANSPARENT); //чтобы фон текста совпадал с фоном ячейки
		//::SelectObject(hDC, (HFONT)dWnd->listFont);
		//::ReleaseDC(dWnd->m_hWnd, hDC);

		Invalidate(0);
		parentWindowPtr->Invalidate(0);
	}
}

void QuoteRunSettings::OnBnClickedButtonOpeninterestIncreaseTextColor()
{
    CColorDialog clrDlg(parentWindowPtr->OIIncreaseText, CC_FULLOPEN, this);

    if (clrDlg.DoModal() == IDOK) {
        parentWindowPtr->OIIncreaseText = clrDlg.GetColor();
        Invalidate(0);
        parentWindowPtr->Invalidate(0);
    }
}
void QuoteRunSettings::OnBnClickedButtonOpeninterestIncreaseBackgroundColor()
{
    CColorDialog clrDlg(parentWindowPtr->OIIncreaseBackground, CC_FULLOPEN, this);

    if (clrDlg.DoModal() == IDOK) {
        parentWindowPtr->OIIncreaseBackground = clrDlg.GetColor();
        Invalidate(0);
        parentWindowPtr->Invalidate(0);
    }
}
void QuoteRunSettings::OnBnClickedButtonOpeninterestDecreaseTextColor()
{
    CColorDialog clrDlg(parentWindowPtr->OIDecreaseText, CC_FULLOPEN, this);

    if (clrDlg.DoModal() == IDOK) {
        parentWindowPtr->OIDecreaseText = clrDlg.GetColor();
        Invalidate(0);
        parentWindowPtr->Invalidate(0);
    }
}
void QuoteRunSettings::OnBnClickedButtonOpeninterestDecreaseBackgroundColor()
{
    CColorDialog clrDlg(parentWindowPtr->OIDecreaseBackground, CC_FULLOPEN, this);

    if (clrDlg.DoModal() == IDOK) {
        parentWindowPtr->OIDecreaseBackground = clrDlg.GetColor();
        Invalidate(0);
        parentWindowPtr->Invalidate(0);
    }
}
void QuoteRunSettings::OnBnClickedButtonOpeninterestNotchangeTextColor()
{
    CColorDialog clrDlg(parentWindowPtr->OINotChangeText, CC_FULLOPEN, this);

    if (clrDlg.DoModal() == IDOK) {
        parentWindowPtr->OINotChangeText = clrDlg.GetColor();
        Invalidate(0);
        parentWindowPtr->Invalidate(0);
    }
}
void QuoteRunSettings::OnBnClickedButtonOpeninterestNotchangeBackgroundColor()
{
    CColorDialog clrDlg(parentWindowPtr->OINotChangeBackground, CC_FULLOPEN, this);

    if (clrDlg.DoModal() == IDOK) {
        parentWindowPtr->OINotChangeBackground = clrDlg.GetColor();
        Invalidate(0);
        parentWindowPtr->Invalidate(0);
    }
}
