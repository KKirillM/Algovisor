#include "stdafx.h"
#include "Algovisor.h"
#include "MainWindow.h"
#include "QuoteRun.h"
#include "BaseChart.h"
#include "Heat_Map.h"
#include "Auxiliary_methods.h"
#include "Instrument.h"

#include "NinjaDialog.h"


NinjaDialog::NinjaDialog(MainWindow* pParent /*=NULL*/) : CDialogEx(NinjaDialog::IDD, pParent)
{	
}

NinjaDialog::~NinjaDialog() 
{
    MainWindow* pMainWnd = MainWnd::GetInstance();
    if (pMainWnd)
    {
	    pMainWnd->ninjaDlg = NULL; 
    }
}

void NinjaDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(NinjaDialog, CDialogEx)
	ON_BN_CLICKED(BUTTON_NINJATRADER_CONNECT, &NinjaDialog::OnBnClickedConnect)
	ON_UPDATE_COMMAND_UI(BUTTON_NINJATRADER_CONNECT, &NinjaDialog::OnUpdateConnection)
	ON_WM_CTLCOLOR()	
	ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL NinjaDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void NinjaDialog::OnBnClickedConnect()
{
	EnableWindow(0);

	if (pMainWnd->ninjaStatus == -1 || pMainWnd->ninjaStatus == 0)
	{ 
		using namespace rapidjson;

		StringBuffer s;
		Writer<StringBuffer> writer(s);
    
		writer.StartObject();
		writer.String("action");				writer.String("getfeed");
		writer.String("feedname");				writer.String("NINJA");
		//writer.String("login");					writer.String(login.GetString());
		//writer.String("password");				writer.String(password.GetString());
		writer.EndObject();

		std::string request(""), answer("");
		const wchar_t* errorWStr = SFLB_0_encryptRSA((char*)s.GetString(), request);
		if (!request.compare(""))
		{
			pMainWnd->UpdateActivityWindow(errorWStr);
			EnableWindow(1);
			return;
		}

		tstring curlRequest = SFLB_0_SendCurlRequest(request, answer);
		if (!answer.compare(""))
		{
			const wchar_t* error = _T("Algovisor: ERROR: ");
			curlRequest.insert(0, error, wcslen(error));
			pMainWnd->UpdateActivityWindow(curlRequest.c_str());
			EnableWindow(1);
			return;
		}

		MyHandler handler;
		Reader reader;
		StringStream ss(answer.c_str());
		reader.Parse(ss, handler);

		if (handler.strings.size() == 1)
		{
			TCHAR* str = new TCHAR[handler.strings[0].size()*2+1];
			StringCchPrintf(str, handler.strings[0].size()*2+1, L"%S", handler.strings[0].c_str());
			pMainWnd->securityString.append(str);
			delete[] str;
		}

		if (!CheckProcessInMemory(L"NinjaTrader.exe"))	// анализ результата
		{
			MSG msg;
			::PeekMessageW(&msg, NULL, NULL, NULL, PM_REMOVE); //опусташаем очередь кликов по кнопке если их будет много
			pMainWnd->UpdateActivityWindow(L"NinjaTrader is not running!");
			EnableWindow(1);
			return;
		}

		pMainWnd->ninjaStatus = 2;
		pMainWnd->UpdateActivityWindow(L"NinjaTrader: Connection open");
		pMainWnd->SetTimer(1, 3000, NULL);

		if (!pMainWnd->ninjatrader_connection_indicator.IsWindowVisible())
			pMainWnd->ninjatrader_connection_indicator.ShowWindow(SW_SHOW);
		else pMainWnd->ninjatrader_connection_indicator.RedrawWindow();

		//Запускаем пайп сервер
		RunPipeServer();
	}
	else	//disconnect
	{
		pMainWnd->securityString.clear();
		pMainWnd->ninjaStatus = 0;
		pMainWnd->DisconnectFromFeed(NINJATRADER);
		pMainWnd->UpdateActivityWindow(L"NinjaTrader: Connection closed");
		pMainWnd->ninjatrader_connection_indicator.RedrawWindow();

		//останавливаем пайп-сервер
		StopPipeServer();
	}

	pMainWnd->InvalidateNinjaConnectionIndicator(0, 0);

	EnableWindow(1);

	return;
}
BOOL NinjaDialog::PreTranslateMessage(MSG* pMsg)
{
	UpdateDialogControls(this, FALSE);
	return CDialogEx::PreTranslateMessage(pMsg);
}
void NinjaDialog::OnUpdateConnection(CCmdUI* pCmdUI)
{
	switch (pMainWnd->ninjaStatus)
	{
	case -1:
	case 0:
		pCmdUI->SetText(L"Connect");
		break;
	case 2:
	case 3:
		pCmdUI->SetText(L"Disconnect");
		break;
	}
}

void NinjaDialog::OnOK() {}
void NinjaDialog::OnCancel() { DestroyWindow(); }
void NinjaDialog::PostNcDestroy()
{
	CDialogEx::PostNcDestroy();
	delete this;
}

HBRUSH NinjaDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID() == NINJATRADER_CONNECTION_STATUS) 
	{
		pDC->SetBkMode(TRANSPARENT);
		switch (pMainWnd->ninjaStatus)
		{
		case 0:	hbr = ::CreateSolidBrush(RGB(255, 0, 0)); break;		//disconnect (красный)
		case 1:	hbr = ::CreateSolidBrush(RGB(255, 155, 55)); break;		//trying to connect (коричневый)
		case 2:	hbr = ::CreateSolidBrush(RGB(255, 155, 55)); break;		//есть подключение, но нет подключенных инструментов (коричневый)
		case 3: hbr = ::CreateSolidBrush(RGB(0, 255, 128));	break;		//есть подключенный инструменты (зеленый)
		}
	}

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

void NinjaDialog::RunPipeServer()
{
	int res = StartPipeServerForNinja();
}

void NinjaDialog::StopPipeServer()
{
	pMainWnd->StopPipeServerForNinja();
}

void NinjaDialog::OnClose()
{
	if (pMainWnd->ninjaStatus == 0)
	{
		pMainWnd->ninjaStatus = -1;
		pMainWnd->GetDlgItem(IDC_CONNECTION_STATUS_FEED3)->Invalidate(0);
	}

	CDialogEx::OnClose();
}
