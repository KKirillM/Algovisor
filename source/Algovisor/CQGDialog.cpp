#include "stdafx.h"

#include "Algovisor.h"
#include "MainWindow.h"
#include "QuoteRun.h"
#include "Quant_Chart.h"
#include "Heat_Map.h"
#include "Instrument.h"
#include "Auxiliary_methods.h"

#include "CQGDialog.h"

HWND cqgHwnd;

// CQGDialog dialog

CQGDialog::CQGDialog(MainWindow* pParent /*=NULL*/) : CDialogEx(CQGDialog::IDD, pParent)
{	
	waitCursor = false;
}

CQGDialog::~CQGDialog() 
{
    MainWindow* pMainWnd = MainWnd::GetInstance();
    if (pMainWnd)
    {
        pMainWnd->cqgDlg = NULL; 
    }
}

void CQGDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CQGDialog, CDialogEx)
	ON_BN_CLICKED(BUTTON_CQG_CONNECT, &CQGDialog::OnBnClickedConnectCqg)
	ON_UPDATE_COMMAND_UI(BUTTON_CQG_CONNECT, OnUpdateConnection)
	ON_WM_CTLCOLOR()
	ON_WM_SETCURSOR()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CQGDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

	//проверяем наличие ключа в реестре в ID и заносим его значение в поле редактирования
	if (theApp.GetProfileInt(APPLICATION_NAME, _T("SaveLPCQG"), 0)) 
	{
		CheckDlgButton(CQG_SAVE_LOGIN_PASSWORD, 1);
		GetDlgItem(CQG_LOGIN)->SetWindowTextW(SFLB_0_CryptString(theApp.GetProfileString(APPLICATION_NAME, _T("LCQG"))));
		GetDlgItem(CQG_PASSWORD)->SetWindowTextW(SFLB_0_CryptString(theApp.GetProfileString(APPLICATION_NAME, _T("PCQG"))));
		if (theApp.GetProfileInt(APPLICATION_NAME, _T("DemoLoginCQG"), 0))
			CheckDlgButton(CQG_DEMO_CONNECTION, 1);
	}

    return TRUE;  // return TRUE  unless you set the focus to a control
}

static UINT AFX_CDECL CQGConnectionThread(LPVOID pointer)
{
	COPYDATASTRUCT* cd = (COPYDATASTRUCT*)pointer;

	if (cd->dwData == CQGDialog::CQG_CONNECT)
	{
		LOGINDATA* ld = (LOGINDATA*)cd->lpData;
		//CStringA login(ld->login), password(ld->password);
		//CryptString((std::string&)login);
		//CryptString((std::string&)password);

		//посылаем запрос на сервер для получения GUID
		using namespace rapidjson;

		StringBuffer s;
		Writer<StringBuffer> writer(s);
    
		writer.StartObject();
		writer.String("action");				writer.String("getfeed");
		writer.String("feedname");				writer.String("CQG");
		//writer.String("login");					writer.String(login.GetString());
		//writer.String("password");				writer.String(password.GetString());
		writer.EndObject();

		std::string request(""), answer("");
		const wchar_t* errorWStr = SFLB_0_encryptRSA((char*)s.GetString(), request);
		if (!request.compare(""))
		{
			pMainWnd->UpdateActivityWindow(errorWStr);
			pMainWnd->cqgDlg->EnableWindow(1);
			return 1;
		}

		tstring curlRequest = SFLB_0_SendCurlRequest(request, answer);
		if (!answer.compare(""))
		{
			const wchar_t* error = L"Algovisor: ERROR: ";
			curlRequest.insert(0, error, wcslen(error));
			pMainWnd->UpdateActivityWindow(curlRequest.c_str());
		}

		MyHandler handler;
		Reader reader;
		StringStream ss(answer.c_str());
		reader.Parse(ss, handler);
		
		if (handler.strings.size() == 4)
		{
			ld->guID.Data1 = atoi(handler.strings[0].c_str());
			ld->guID.Data2 = atoi(handler.strings[1].c_str());
			ld->guID.Data3 = atoi(handler.strings[2].c_str());
			int destLen = 8;
			ATL::Base64Decode(handler.strings[3].c_str(), (int)handler.strings[3].size(), ld->guID.Data4, &destLen);
		}
	}

	::SendMessageW(cqgHwnd, WM_COPYDATA, NULL, (LPARAM)(LPVOID)cd);

	if (cd->dwData == CQGDialog::CQG_CONNECT)
		delete (LOGINDATA*)cd->lpData;

	delete cd;

	return 0;
}
void CQGDialog::OnBnClickedConnectCqg()
{
	cqgHwnd = pMainWnd->IsCQGServerRun();

	if (!cqgHwnd)
		return;

	CString wLogin, wPassword;

	GetDlgItem(CQG_LOGIN)->GetWindowTextW(wLogin);
	if (wLogin.IsEmpty()) 
	{
		pMainWnd->UpdateActivityWindow(L"CQG: Please enter login");
		return;
	}

	GetDlgItem(CQG_PASSWORD)->GetWindowTextW(wPassword);
	if (wPassword.IsEmpty()) 
	{
		pMainWnd->UpdateActivityWindow(L"CQG: Please enter password");
		return;
	}

	//замораживаем окно на время подключения или отключения
	waitCursor = true;
	EnableWindow(0);
	pMainWnd->cqgStatus = 1; //Trying to logon...

	CString wTitle;
	GetDlgItem(BUTTON_CQG_CONNECT)->GetWindowTextW(wTitle);

	COPYDATASTRUCT* cdStruct = new COPYDATASTRUCT;
	
	if (wTitle == L"Connect") 
	{
		LOGINDATA* ld = new LOGINDATA;
		
		if (IsDlgButtonChecked(CQG_DEMO_CONNECTION))
			ld->demo = true;
		else ld->demo = false;

		StringCchCopy(ld->login, 100, wLogin.GetString());
		StringCchCopy(ld->password, 100, wPassword.GetString());
		
		cdStruct->dwData = CQGDialog::CQG_CONNECT;		// function identifier
		cdStruct->cbData = sizeof(LOGINDATA);			// size of data
		cdStruct->lpData = ld;							// data structure
	}
	else 
	{
		cdStruct->dwData = CQGDialog::CQG_DISCONNECT;      
		cdStruct->cbData = 0;			
		cdStruct->lpData = NULL;
	}	

	AfxBeginThread(CQGConnectionThread, cdStruct);

	//запишем в реестр данные логина если стоит галочка сохранять данные
	if (IsDlgButtonChecked(CQG_SAVE_LOGIN_PASSWORD)) 
	{
		theApp.WriteProfileString(APPLICATION_NAME, _T("LCQG"), SFLB_0_CryptString(wLogin));
		theApp.WriteProfileString(APPLICATION_NAME, _T("PCQG"), SFLB_0_CryptString(wPassword));
		theApp.WriteProfileInt(APPLICATION_NAME, _T("SaveLPCQG"), 1);
		if (IsDlgButtonChecked(CQG_DEMO_CONNECTION))
			theApp.WriteProfileInt(APPLICATION_NAME, _T("DemoLoginCQG"), 1);
	} else {
		theApp.WriteProfileString(APPLICATION_NAME, _T("LCQG"), _T(""));
		theApp.WriteProfileString(APPLICATION_NAME, _T("PCQG"),  _T(""));
		theApp.WriteProfileInt(APPLICATION_NAME, _T("SaveLPCQG"), 0);
		theApp.WriteProfileInt(APPLICATION_NAME, _T("DemoLoginCQG"), 0);
	}
}

BOOL CQGDialog::PreTranslateMessage(MSG* pMsg)
{
	UpdateDialogControls(this, FALSE);
	return CDialogEx::PreTranslateMessage(pMsg);
}
void CQGDialog::OnUpdateConnection(CCmdUI* pCmdUI)
{
	switch (pMainWnd->cqgStatus)
	{
	case 0:
		GetDlgItem(CQG_LOGIN)->EnableWindow(1);
		GetDlgItem(CQG_PASSWORD)->EnableWindow(1);
		GetDlgItem(BUTTON_CQG_CONNECT)->EnableWindow(1);
		pCmdUI->SetText(L"Connect");
		break;
	case 1:
		GetDlgItem(CQG_LOGIN)->EnableWindow(0);
		GetDlgItem(CQG_PASSWORD)->EnableWindow(0);
		GetDlgItem(BUTTON_CQG_CONNECT)->EnableWindow(0);
		pCmdUI->SetText(L"Connect");
		break;
	case 2:
		GetDlgItem(CQG_LOGIN)->EnableWindow(0);
		GetDlgItem(CQG_PASSWORD)->EnableWindow(0);
		GetDlgItem(BUTTON_CQG_CONNECT)->EnableWindow(1);
		pCmdUI->SetText(L"Disconnect");
		break;
	}
}

void CQGDialog::OnOK() {}
void CQGDialog::OnCancel() { DestroyWindow(); }
void CQGDialog::PostNcDestroy()
{
	CDialogEx::PostNcDestroy();
	delete this;
}

HBRUSH CQGDialog::OnCtlColor(CDC* pDC, CWnd* pMainWindow, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pMainWnd, nCtlColor);
	
	if (pMainWindow->GetDlgCtrlID() == CQG_CONNECT_STATUS) 
	{
		pDC->SetBkMode(TRANSPARENT);
		switch (pMainWnd->cqgStatus)
		{
		case 0:	hbr = ::CreateSolidBrush(RGB(255, 0, 0)); break;		//disconnect красный
		case 1:	hbr = ::CreateSolidBrush(RGB(255, 155, 55)); break;		//trying to connect коричневый
		case 2:	hbr = ::CreateSolidBrush(RGB(0, 255, 128));	break;		//connect ok зеленый
		}
	}

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

BOOL CQGDialog::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (waitCursor)
	{
		RestoreWaitCursor();
		return TRUE;
	}

	return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}


void CQGDialog::OnClose()
{
	if (pMainWnd->cqgStatus == 0)
	{
		pMainWnd->cqgStatus = -1;
		pMainWnd->GetDlgItem(IDC_CONNECTION_STATUS_FEED2)->Invalidate(0);
	}

	CDialogEx::OnClose();
}
