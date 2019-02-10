#include "stdafx.h"

#include "DEFINE.h"
#include "ConnectionDialog.h"
#include "MainWindow.h"
#include "IPlaza2_Connector.h"
#include "Plaza2Callbacks.h"
#include "IniFileWorker.h"
#include "Auxiliary_methods.h"

#include "Plaza2Dialog.h"

BEGIN_MESSAGE_MAP(Plaza2Dialog, CDialogEx)
	ON_BN_CLICKED(BUTTON_PLAZA2_CONNECT, OnBnClickedConnect)
    ON_BN_CLICKED(IDC_CHECK_PLAZA2_REMOTE_ROUTER, OnBnClickedCheckPlaza2RemoteRouter)
    ON_WM_CTLCOLOR()
    ON_WM_CLOSE()
END_MESSAGE_MAP()

Plaza2Dialog::Plaza2Dialog() : CDialogEx(IDD)
{
}

Plaza2Dialog::~Plaza2Dialog()
{
    MainWindow* pMainWnd = MainWnd::GetInstance();
    if (pMainWnd)
    {
	    pMainWnd->plaza2Dlg = NULL;
    }
}

BOOL Plaza2Dialog::OnInitDialog()
{
    ReadDataFromINI();      // считываем инишный файл со значениями для этого окна

    // заполняем необходимые поля окна
    GetDlgItem(PLAZA2_LOGIN)->SetWindowTextW(std::wstring(params.login.begin(), params.login.end()).c_str());
    GetDlgItem(PLAZA2_PASSWORD)->SetWindowTextW(std::wstring(params.password.begin(), params.password.end()).c_str());
    if (params.saveLoginPass)
    {
        CheckDlgButton(PLAZA2_SAVE_LOGIN_PASSWORD, 1);
    }
    if (params.useRemoteRouter)
    {
        CheckDlgButton(IDC_CHECK_PLAZA2_REMOTE_ROUTER, 1);
    }
    GetDlgItem(IDC_PLAZA2_IPADDRESS)->SetWindowTextW(std::wstring(params.ip.begin(), params.ip.end()).c_str());
    GetDlgItem(IDC_EDIT_PLAZA2_PORTNUMBER)->SetWindowTextW(std::wstring(params.port.begin(), params.port.end()).c_str());
    GetDlgItem(IDC_REMOTE_ROUTER_PASSWORD)->SetWindowTextW(std::wstring(params.remoteRouterPassword.begin(), params.remoteRouterPassword.end()).c_str());

    UpdateControls();

	return CDialogEx::OnInitDialog();
}

void Plaza2Dialog::OnBnClickedConnect()
{
	StartWaitCursor();

    try
    {
        CheckConnectionParameters();

		SaveDataToINI();

        Connect();  // здесь логика подключения

        UpdateControls();
    }
    catch(ConnectionException& e)
    {
        static_cast<MainWindow*>(GetParent())->UpdateActivityWindow(::CA2W(e.what()));
    }

	StopWaitCursor();
}

void Plaza2Dialog::Connect()
{
	MainWindow* pMainWnd = static_cast<MainWindow*>(GetParent());

	if (!pMainWnd->p2Connector)
	{
		pMainWnd->p2Connector.reset(GetPlaza2ConnectorType2());
	}

	IPlaza2ConnectorType2* p2Connector = pMainWnd->p2Connector.get();
    Plaza2ConnectionStatus status = GetConnectionStatus();

	if (status == NONE || status == DISCONNECT)
	{
		try 
		{
            if (IsDlgButtonChecked(IDC_CHECK_PLAZA2_REMOTE_ROUTER) == BST_UNCHECKED)
            {
                SetPlaza2RouterLoginPass(iniPath, params.login, params.password);
                StopPlaza2RouterProcess(PLAZA2_ROUTER_NAME_X86);
                StopPlaza2RouterProcess(PLAZA2_ROUTER_NAME_X64);
                StartPlaza2RouterProcess(routerPath, iniPath, false);
            }

            Plaza2InitParameters params;
            params.connectionString		= connectionString;
            params.environmentString	= environmentString;
            params.replstateFolderPath	= PLAZA2_REPLSTATEDB_PATH;

            p2Connector->OpenConnection(params);

			switch (GetConnectionStatus())
			{
			case NONE:		
			case DISCONNECT:
			case WAIT:			
				throw std::exception("Plaza2: Connection busy, please try again");
			case OPEN:			
				{
					pMainWnd->UpdateActivityWindow(_T("Plaza2: Connection open"));
					pMainWnd->AddNewDatafeedToComboBox(_T("Plaza2"));
					pMainWnd->OnSelchangeDatafeedCombobox();
                    pMainWnd->RedrawWindow();
				}
				break; 
			}
		}
		catch(std::exception& e)
		{
			wchar_t err[512];
			mbstowcs(err, e.what(), 512);
			p2Connector->Disconnect();
			Invalidate(0);
			pMainWnd->UpdateActivityWindow(err);
            pMainWnd->RedrawWindow();
		}
	}
	else
	{
		p2Connector->Disconnect();
		Invalidate(0);
		pMainWnd->p2Connector.reset();
        StopPlaza2RouterProcess(PLAZA2_ROUTER_NAME_X86);
        StopPlaza2RouterProcess(PLAZA2_ROUTER_NAME_X64);
		pMainWnd->RemoveDatafeedFromComboBox(_T("Plaza2"));
		pMainWnd->UpdateActivityWindow(_T("Plaza2: Connection closed"));
        pMainWnd->DisconnectFromFeed(PLAZA2);
        pMainWnd->RedrawWindow();
	}
}

void Plaza2Dialog::CheckConnectionParameters()
{
	environmentString = PLAZA2_ENVIRONMENT_STRING;

	CIPAddressCtrl* ipAddrCtrl = (CIPAddressCtrl*)GetDlgItem(IDC_PLAZA2_IPADDRESS);
    CString remoteRouterPassword;
    GetDlgItemTextW(IDC_REMOTE_ROUTER_PASSWORD, remoteRouterPassword);
    params.remoteRouterPassword = CW2A(remoteRouterPassword);

	BYTE ipAddr[4] = {0};
	ipAddrCtrl->GetAddress(ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);

    CString portString;
    GetDlgItemTextW(IDC_EDIT_PLAZA2_PORTNUMBER, portString);
    params.port = CW2A(portString);

	if (params.port.empty())
	{
		throw ConnectionException("Plaza2: Please enter port number!");
	}

	CStringA ipString;
    ipString.Format("%d.%d.%d.%d", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
    params.ip = ipString;
	CStringA aLogin(login);
	ipString.Format("p2tcp://%s:%s;timeout=5000;local_pass=%s;app_name=" APPLICATION_NAME_A "_%s", 
					params.ip.c_str(), 
					params.port.c_str(), 
					params.remoteRouterPassword.c_str(),
					aLogin.GetString());

    if (IsDlgButtonChecked(IDC_CHECK_PLAZA2_REMOTE_ROUTER) == BST_CHECKED)
    {
        connectionString = ipString;
        params.useRemoteRouter = true;
        routerPath = "";
        iniPath = "";
    }
    else
    {
        params.useRemoteRouter = false;
        connectionString = PLAZA2_CONNECTION_STRING;
#if _WIN32 || _WIN64
#if _WIN64
        routerPath = PLAZA2_ROUTER_NAME_X64;
#else
        routerPath = PLAZA2_ROUTER_NAME_X86;
#endif
#endif

        iniPath = PLAZA2_INI_PATH;
    }

    if (IsDlgButtonChecked(PLAZA2_SAVE_LOGIN_PASSWORD) == BST_CHECKED)
    {
        params.saveLoginPass = true;
        CString login, password;
        GetDlgItemTextW(PLAZA2_LOGIN, login);
        GetDlgItemTextW(PLAZA2_PASSWORD, password);
        params.login = CW2A(login);
        params.password = CW2A(password);
    }
    else
    {
        params.saveLoginPass = false;
        params.login = "";
        params.password = "";
    }
};

void Plaza2Dialog::ReadDataFromINI()
{
    IniFileWorker ini;

    try
    {
        ini.Open(".\\ini\\Algovisor.ini");

        params.saveLoginPass = ini.ReadInt("Plaza2ConnectionDialog", "saveLoginPass") == 1 ? true : false;
        params.login = ini.ReadStr("Plaza2ConnectionDialog", "login");
        params.password = ini.ReadStr("Plaza2ConnectionDialog", "password");
        params.useRemoteRouter = ini.ReadInt("Plaza2ConnectionDialog", "useRemoteRouter") == 1 ? true : false;
        params.ip = ini.ReadStr("Plaza2ConnectionDialog", "ip");
        params.port = ini.ReadStr("Plaza2ConnectionDialog", "port");
        params.remoteRouterPassword = ini.ReadStr("Plaza2ConnectionDialog", "remoteRouterPassword");
    }
    catch (IniFileException&)
    {
        static_cast<MainWindow*>(GetParent())->UpdateActivityWindow(_T("ERROR: Algovisor.ini has not the correct format"));
    }

    ini.Close();
}

void Plaza2Dialog::SaveDataToINI()
{
    IniFileWorker ini;

    try
    {
		if (params.saveLoginPass)
		{
			ini.Open(".\\ini\\Algovisor.ini");
			ini.WriteInt("Plaza2ConnectionDialog", "saveLoginPass", params.saveLoginPass == true ? 1 : 0);
			ini.WriteStr("Plaza2ConnectionDialog", "login", params.login);
			ini.WriteStr("Plaza2ConnectionDialog", "password", params.password);
			ini.WriteInt("Plaza2ConnectionDialog", "useRemoteRouter", params.useRemoteRouter == true ? 1 : 0);
			ini.WriteStr("Plaza2ConnectionDialog", "ip", params.ip);
			ini.WriteStr("Plaza2ConnectionDialog", "port", params.port);
			ini.WriteStr("Plaza2ConnectionDialog", "remoteRouterPassword", params.remoteRouterPassword);
		}
    }
    catch (IniFileException&)
    {
        static_cast<MainWindow*>(GetParent())->UpdateActivityWindow(_T("ERROR: Algovisor.ini has not the correct format"));
    }

    ini.Close();
}

Plaza2ConnectionStatus Plaza2Dialog::GetConnectionStatus()
{
	MainWindow* pMainWnd = static_cast<MainWindow*>(GetParent());
	Plaza2ConnectionStatus status = NONE;

	if (pMainWnd->p2Connector)
    {
		status = pMainWnd->p2Connector->GetConnectionStatus();
    }

	return status;
}

HBRUSH Plaza2Dialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

    if (pWnd->GetDlgCtrlID() == PLAZA2_CONNECT_STATUS)
    {
        pDC->SetBkMode(TRANSPARENT);
        switch (GetConnectionStatus())
        {
        case NONE:			hbr = ::CreateSolidBrush(RGB(192, 192, 192)); break;	//none серый
        case DISCONNECT:	hbr = ::CreateSolidBrush(RGB(255, 0, 0)); break;		//disconnect красный
        case WAIT:			hbr = ::CreateSolidBrush(RGB(255, 155, 55)); break;		//wait коричневый
        case OPEN:			hbr = ::CreateSolidBrush(RGB(0, 255, 128));	break;		//connect ok зеленый
        }
    }
    return hbr;
}

void Plaza2Dialog::StartWaitCursor()
{
    BeginWaitCursor();
    EnableWindow(0);
}

void Plaza2Dialog::StopWaitCursor()
{
    EndWaitCursor();
    EnableWindow(1);
    Invalidate(0);
}

void Plaza2Dialog::OnClose()
{
    CDialogEx::OnClose();

    delete this;
}

void Plaza2Dialog::OnOK()
{
}

BOOL Plaza2Dialog::PreTranslateMessage(MSG* pMsg)
{
    BOOL bRet = CDialogEx::PreTranslateMessage(pMsg);
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
    {
        bRet = TRUE; // do not dispatch message
        OnClose();
    }

    return bRet;
}

void Plaza2Dialog::UpdateControls()
{
    switch (GetConnectionStatus())
    {
    case NONE:
    case DISCONNECT:
        {
            if (IsDlgButtonChecked(IDC_CHECK_PLAZA2_REMOTE_ROUTER) == BST_CHECKED)
            {
                GetDlgItem(IDC_PLAZA2_IPADDRESS)->EnableWindow(1);
                GetDlgItem(IDC_EDIT_PLAZA2_PORTNUMBER)->EnableWindow(1);
                GetDlgItem(IDC_REMOTE_ROUTER_PASSWORD)->EnableWindow(1);
                GetDlgItem(PLAZA2_LOGIN)->EnableWindow(0);
                GetDlgItem(PLAZA2_PASSWORD)->EnableWindow(0);
            }
            else
            {
                GetDlgItem(IDC_PLAZA2_IPADDRESS)->EnableWindow(0);
                GetDlgItem(IDC_EDIT_PLAZA2_PORTNUMBER)->EnableWindow(0);
                GetDlgItem(IDC_REMOTE_ROUTER_PASSWORD)->EnableWindow(0);
                GetDlgItem(PLAZA2_LOGIN)->EnableWindow(1);
                GetDlgItem(PLAZA2_PASSWORD)->EnableWindow(1);
            }	
            GetDlgItem(BUTTON_PLAZA2_CONNECT)->SetWindowTextW(_T("Connect"));
        }
        break;
    case WAIT:
    case OPEN:
        GetDlgItem(IDC_PLAZA2_IPADDRESS)->EnableWindow(0);
        GetDlgItem(IDC_EDIT_PLAZA2_PORTNUMBER)->EnableWindow(0);
        GetDlgItem(IDC_REMOTE_ROUTER_PASSWORD)->EnableWindow(0);
        GetDlgItem(PLAZA2_LOGIN)->EnableWindow(0);
        GetDlgItem(PLAZA2_PASSWORD)->EnableWindow(0);
        GetDlgItem(BUTTON_PLAZA2_CONNECT)->SetWindowTextW(_T("Disconnect"));
        break;
    }
}

void Plaza2Dialog::OnBnClickedCheckPlaza2RemoteRouter()
{
    UpdateControls();
}
