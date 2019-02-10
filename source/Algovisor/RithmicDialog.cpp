#include "stdafx.h"
#include "RithmicCallbacks.h"
#include "MainWindow.h"
#include "Algovisor.h"
#include "Instrument.h"
#include "Auxiliary_methods.h"
#include "IniFileWorker.h"
#include "RithmicDialog.h"



RithmicDialog::RithmicDialog(MainWindow* pParent /*=NULL*/) : CDialogEx(RithmicDialog::IDD, pParent)
{
	waitCursor = false;
	//SetBackgroundColor(BGRNCOLOR, false);
}

RithmicDialog::~RithmicDialog() 
{ 
    MainWindow* pMainWnd = MainWnd::GetInstance();
    if (pMainWnd)
    {
        pMainWnd->rithmicDlg = NULL;  
    }
}

void RithmicDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, RITHMIC_COMBOBOX, connectionTypeCombo);
	DDX_Control(pDX, RITHMIC_CONNECTION_STATUS, m_marketDataConnectionStatus);
	DDX_Control(pDX, RITHMIC_LOGIN, rLogin);
	DDX_Control(pDX, RITHMIC_PASSWORD, rPassword);
	DDX_Control(pDX, IDC_RITHMIC_LICENSE, rithmicLicense);
}

BEGIN_MESSAGE_MAP(RithmicDialog, CDialogEx)
	ON_BN_CLICKED(BUTTON_RITHIMC_CONNECT, &RithmicDialog::ConnectButtonPush)
	ON_WM_CTLCOLOR()
	ON_UPDATE_COMMAND_UI(BUTTON_RITHIMC_CONNECT, &RithmicDialog::OnUpdateConnection)
	ON_WM_SETCURSOR()
	ON_MESSAGE(WM_USER+1, &RithmicDialog::RepaintCursor)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL RithmicDialog::OnInitDialog()
{
	ReadDataFromINI();

	//backgroundBrush.CreateSolidBrush(BGRNCOLOR);
	
	CDialogEx::OnInitDialog();	// эту функцию нельзя помещать позже connectionTypeCombo.SetCurSel(params.connectionType); иначе будет эксепшен

	GetDlgItem(RITHMIC_LOGIN)->SetWindowTextW(std::wstring(params.login.begin(), params.login.end()).c_str());
	GetDlgItem(RITHMIC_PASSWORD)->SetWindowTextW(std::wstring(params.password.begin(), params.password.end()).c_str());
	if (params.saveLoginPass)
	{
		CheckDlgButton(RITHIMC_SAVE_LOGIN_PASSWORD, 1);
	}	
	connectionTypeCombo.SetCurSel(params.connectionType);

	// данные о правах ритмика
	HICON hBitmap = (HICON)LoadImageW(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_RITHMIC_LICENSE_ICON), IMAGE_ICON, 191, 152, 0);
	rithmicLicense.SetIcon(hBitmap);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void RithmicDialog::OnOK() {}
void RithmicDialog::OnCancel() { DestroyWindow(); }
void RithmicDialog::PostNcDestroy()
{
	CDialogEx::PostNcDestroy();
	delete this;
}

LRESULT RithmicDialog::RepaintCursor(WPARAM, LPARAM)
{
	EndWaitCursor();
	waitCursor = false;
	return 0;
}


UINT AFX_CDECL RithmicConnectionThread(LPVOID pointer)
{
	RithmicDialog* rithmicDlg = static_cast<RithmicDialog*>(pointer);

	char* envp[11];
	REngineParams   rEngineParams;
	LoginParams     oLoginParams;	

#ifndef CONNECT_THROUGH_SERVER
	switch (rithmicDlg->connectionTypeCombo.GetCurSel())
	{
	case 0:	// Rithmic 01
		envp[0] = "MML_DMN_SRVR_ADDR=ritpz01001.01.rithmic.com:65000~ritpz01000.01.rithmic.com:65000~ritpz01001.01.rithmic.net:65000~ritpz01000.01.rithmic.net:65000~ritpz01001.01.theomne.net:65000~ritpz01000.01.theomne.net:65000~ritpz01001.01.theomne.com:65000~ritpz01000.01.theomne.com:65000";
		envp[1] = "MML_DOMAIN_NAME=rithmic_prod_01_dmz_domain";
		envp[2] = "MML_LIC_SRVR_ADDR=ritpz01000.01.rithmic.com:56000~ritpz01001.01.rithmic.com:56000~ritpz01000.01.rithmic.net:56000~ritpz01001.01.rithmic.net:56000~ritpz01000.01.theomne.net:56000~ritpz01001.01.theomne.net:56000~ritpz01000.01.theomne.com:56000~ritpz01001.01.theomne.com:56000";
		envp[3] = "MML_LOC_BROK_ADDR=ritpz01000.01.rithmic.com:64100";
		envp[4] = "MML_LOGGER_ADDR=ritpz01000.01.rithmic.com:45454~ritpz01000.01.rithmic.net:45454~ritpz01000.01.theomne.com:45454~ritpz01000.01.theomne.net:45454";
		oLoginParams.sMdCnnctPt.pData		= "login_agent_tp_r01c";
		oLoginParams.sMdCnnctPt.iDataLen	= (int)strlen(oLoginParams.sMdCnnctPt.pData);
		//oLoginParams.sTsCnnctPt.pData		= "login_agent_prodc";
		//oLoginParams.sTsCnnctPt.iDataLen	= (int)strlen(oLoginParams.sTsCnnctPt.pData);
		oLoginParams.sIhCnnctPt.pData		= "login_agent_historyc";
		oLoginParams.sIhCnnctPt.iDataLen	= (int)strlen(oLoginParams.sIhCnnctPt.pData);		
		// * LoginParams.sPnLCnnctPt : "login_agent_pnl_sslc"  
		break;

	case 1: // Rithmic 01 Test
	case 2:
		envp[0] = "MML_DMN_SRVR_ADDR=rituz00100.00.rithmic.com:65000~rituz00100.00.rithmic.net:65000~rituz00100.00.theomne.net:65000~rituz00100.00.theomne.com:65000";
		envp[1] = "MML_DOMAIN_NAME=rithmic_uat_dmz_domain";							
		envp[2] = "MML_LIC_SRVR_ADDR=rituz00100.00.rithmic.com:56000~rituz00100.00.rithmic.net:56000~rituz00100.00.theomne.net:56000~rituz00100.00.theomne.com:56000";
		envp[3] = "MML_LOC_BROK_ADDR=rituz00100.00.rithmic.com:64100";							
		envp[4] = "MML_LOGGER_ADDR=rituz00100.00.rithmic.com:45454~rituz00100.00.rithmic.net:45454~rituz00100.00.theomne.com:45454~rituz00100.00.theomne.net:45454";
		oLoginParams.sMdCnnctPt.pData		= "login_agent_tpc";
		oLoginParams.sMdCnnctPt.iDataLen	= (int)strlen(oLoginParams.sMdCnnctPt.pData);
		//oLoginParams.sTsCnnctPt.pData		= "login_agent_opc";
		//oLoginParams.sTsCnnctPt.iDataLen	= (int)strlen(oLoginParams.sTsCnnctPt.pData);
		oLoginParams.sIhCnnctPt.pData		= "login_agent_historyc";
		oLoginParams.sIhCnnctPt.iDataLen	= (int)strlen(oLoginParams.sIhCnnctPt.pData);	
		//LoginParams.sPnLCnnctPt: "login_agent_pnlc"
		//LoginParams.sOrCnnctPt : "Not currently available"
		break;

	case 3: // Rithmic Paper Trading
		envp[0] = "MML_DMN_SRVR_ADDR=ritpa11120.11.rithmic.com:65000~ritpa11120.11.rithmic.net:65000~ritpa11120.11.theomne.net:65000~ritpa11120.11.theomne.com:65000";
		envp[1] = "MML_DOMAIN_NAME=rithmic_paper_prod_domain";
		envp[2] = "MML_LIC_SRVR_ADDR=ritpa11120.11.rithmic.com:56000~ritpa11120.11.rithmic.net:56000~ritpa11120.11.theomne.net:56000~ritpa11120.11.theomne.com:56000";
		envp[3] = "MML_LOC_BROK_ADDR=ritpa11120.11.rithmic.com:64100";
		envp[4] = "MML_LOGGER_ADDR=ritpa11120.11.rithmic.com:45454~ritpa11120.11.rithmic.net:45454~ritpa11120.11.theomne.net:45454~ritpa11120.11.theomne.com:45454";
		oLoginParams.sMdCnnctPt.pData		= "login_agent_tp_paperc";
		oLoginParams.sMdCnnctPt.iDataLen	= (int)strlen(oLoginParams.sMdCnnctPt.pData);
		oLoginParams.sIhCnnctPt.pData		= "login_agent_history_paperc";
		oLoginParams.sIhCnnctPt.iDataLen	= (int)strlen(oLoginParams.sIhCnnctPt.pData);
		//oLoginParams.sPnLCnnctPt.pData	= "login_agent_pnl_paperc";
		//oLoginParams.sTsCnnctPt.pData		= "login_agent_op_paperc";
		//oLoginParams.sTsCnnctPt.iDataLen	= (int)strlen(oLoginParams.sTsCnnctPt.pData);
		break;

	case 4: // Rithmic Paper Trading Europe
		envp[0] = "MML_DMN_SRVR_ADDR=ritpa11120.11.rithmic.com:65000~ritpa11120.11.rithmic.net:65000~ritpa11120.11.theomne.net:65000~ritpa11120.11.theomne.com:65000";
		envp[1] = "MML_DOMAIN_NAME=rithmic_paper_prod_domain";
		envp[2] = "MML_LIC_SRVR_ADDR=ritpa11120.11.rithmic.com:56000~ritpa11120.11.rithmic.net:56000~ritpa11120.11.theomne.net:56000~ritpa11120.11.theomne.com:56000";
		envp[3] = "MML_LOC_BROK_ADDR=ritpa11120.11.rithmic.com:64100";
		envp[4] = "MML_LOGGER_ADDR=ritpa11120.11.rithmic.com:45454~ritpa11120.11.rithmic.net:45454~ritpa11120.11.theomne.net:45454~ritpa11120.11.theomne.com:45454";
		oLoginParams.sMdCnnctPt.pData    = "login_agent_tp_aws_europec";
		oLoginParams.sMdCnnctPt.iDataLen = (int)strlen(oLoginParams.sMdCnnctPt.pData);
		oLoginParams.sIhCnnctPt.pData	 = "login_agent_history_paperc";
		oLoginParams.sIhCnnctPt.iDataLen = (int)strlen(oLoginParams.sIhCnnctPt.pData);
		//oLoginParams.sPnLCnnctPt.pData	= "login_agent_pnl_paperc"
		//oLoginParams.sTsCnnctPt.pData		= "login_agent_op_aws_europec";
		//oLoginParams.sTsCnnctPt.iDataLen	= (int)strlen(oLoginParams.sTsCnnctPt.pData);
		break;
	}	

#else

	using namespace rapidjson;

	StringBuffer s;
	Writer<StringBuffer> writer(s);

	writer.StartObject();
	writer.String("action");
	writer.String("getfeed");
	writer.String("feedname");

	switch (rithmicDlg->connectionTypeCombo.GetCurSel())
	{
	case 0:
		writer.String("Rithmic 01");
		break;					   
								   
	case 1:						   
	case 2:						   
		writer.String("Rithmic 01 Test");
		break;	

	case 3:						   
		writer.String("Rithmic Paper Trading");
		break;	

	case 4:						   
		writer.String("Rithmic Paper Trading Europe");
		break;
	}
	
	writer.EndObject();

	std::string request(""), answer("");
	const wchar_t* errorWStr = SFLB_0_encryptRSA((char*)s.GetString(), request);
	if (!request.compare(""))
	{
		pMainWnd->UpdateActivityWindow(errorWStr);
		pMainWnd->EnableWindow(1);
		::SendMessage(rithmicDlg->m_hWnd, WM_USER + 1, 0, 0);
		return 1;
	}

	tstring curlRequest = SFLB_0_SendCurlRequest(request, answer);
	if (!answer.compare(""))
	{
		const wchar_t* error = _T("Algovisor: ERROR: ");
		curlRequest.insert(0, error, wcslen(error));
		pMainWnd->UpdateActivityWindow(curlRequest.c_str());
	}

	MyHandler handler;
	Reader reader;
	StringStream ss(answer.c_str());
	reader.Parse(ss, handler);

	envp[0] = envp[1] = envp[2] = envp[3] = envp[4] = NULL;

	if (handler.strings.size() == 9)
	{
		envp[0] = (char*)handler.strings[0].c_str();
		envp[1] = (char*)handler.strings[1].c_str();
		envp[2] = (char*)handler.strings[2].c_str();
		envp[3] = (char*)handler.strings[3].c_str();
		envp[4] = (char*)handler.strings[4].c_str();
		oLoginParams.sMdCnnctPt.pData = (char*)handler.strings[5].c_str();
		oLoginParams.sMdCnnctPt.iDataLen = (int)strlen(oLoginParams.sMdCnnctPt.pData);
		oLoginParams.sIhCnnctPt.pData = (char*)handler.strings[6].c_str();
		oLoginParams.sIhCnnctPt.iDataLen = (int)strlen(oLoginParams.sIhCnnctPt.pData);
		//oLoginParams.sTsCnnctPt.pData    = (char*)handler.strings[7].c_str();
		//oLoginParams.sTsCnnctPt.iDataLen = (int)strlen(oLoginParams.sTsCnnctPt.pData);
		//oLoginParams.sPnlCnnctPt.pData = (char*)handler.strings[8].c_str();
		//oLoginParams.sPnlCnnctPt.iDataLen = (int)strlen(oLoginParams.sPnlCnnctPt.pData);
	}

#endif

	envp[5] = "MML_LOG_TYPE=log_net";
	envp[6] = "MML_SSL_CLNT_AUTH_FILE=Rithmic\\rithmic_ssl_cert_auth_params";
	envp[7] = "MML_SSL_CLNT_CERT_FILE=Rithmic\\rithmic_ssl_client_params";
	envp[8] = "MML_SSL_CLNT_KEY_FILE=Rithmic\\rithmic_ssl_client_private_key";
	CStringA userLogin;
	userLogin.Format("USER=%s", rithmicDlg->params.login.c_str());
	envp[9] = (char*)userLogin.GetBuffer();
	envp[10] = NULL;

	//rEngineParams.envp				= envp;
	//EngineParams.pContext				= this;
	rEngineParams.sAppName.pData        = "Algovisor"; /*"ur:Algo-Graphic FLOW";*/
	rEngineParams.sAppName.iDataLen     = (int)strlen(rEngineParams.sAppName.pData);
	rEngineParams.sAppVersion.pData     = "1.0"; /*"2.0";*/
	rEngineParams.sAppVersion.iDataLen  = (int)strlen(rEngineParams.sAppVersion.pData);
	rEngineParams.sAdmCnnctPt.pData     = "dd_admin_sslc";
	rEngineParams.sAdmCnnctPt.iDataLen  = (int)strlen(rEngineParams.sAdmCnnctPt.pData);
#ifdef _DEBUG
	rEngineParams.sLogFilePath.pData	= "log\\rithmic.log";
#else
	rEngineParams.sLogFilePath.pData	= "";
#endif
	rEngineParams.sLogFilePath.iDataLen = (int)strlen(rEngineParams.sLogFilePath.pData);

	std::string login(rithmicDlg->params.login), password(rithmicDlg->params.password);
	try
	{
		if (pMainWnd->rithmicCallbacks == NULL)
		{
			pMainWnd->rithmicCallbacks = new RithmicCallbacks(envp, rEngineParams, oLoginParams, login, password);
		}
		else
		{
			pMainWnd->rithmicCallbacks->SetValues(envp, rEngineParams, oLoginParams, login, password);
		}
	}
	catch (OmneException& oEx)
	{
		char errstr[100];
		StringCchPrintfA(errstr, 100, "Rithmic: REngine Error : %d, %s", oEx.getErrorCode(), oEx.getErrorString());
		wchar_t errwstr[100];
		StringCchPrintfW(errwstr, 100, L"%S", errstr);
		pMainWnd->UpdateActivityWindow(errwstr);
		rithmicDlg->m_marketDataConnectionStatus.SetWindowText(_T("Connection failed"));
		rithmicDlg->GetDlgItem(BUTTON_RITHIMC_CONNECT)->EnableWindow(1);		//включаем обратно кнопку Connect
		rithmicDlg->StopWaitCursor();
		::SendMessage(rithmicDlg->m_hWnd, WM_USER+1, 0,0);
		return 1;
	}
	
	if (pMainWnd->rithmicCallbacks->Connect())
	{
		rithmicDlg->m_marketDataConnectionStatus.SetWindowText(_T("Connection failed"));
		pMainWnd->rithmicStatus = 0;
		rithmicDlg->GetDlgItem(RITHMIC_CONNECTION_STATUS)->RedrawWindow();
	} 	
	
	wchar_t wstr[100];
	StringCchPrintf(wstr, 100, L"%S", pMainWnd->rithmicCallbacks->errstr);
	pMainWnd->UpdateActivityWindow(wstr);

	rithmicDlg->StopWaitCursor();
	::SendMessage(rithmicDlg->m_hWnd, WM_USER+1, 0,0);

	pMainWnd->OnSelchangeDatafeedCombobox();
    pMainWnd->RedrawWindow();

	return 0;
}

void RithmicDialog::ConnectButtonPush()
{
	StartWaitCursor();

	if (!CheckConnectionParameters())
	{
		StopWaitCursor();
		return;
	}

	SaveDataToINI();

	CString str;
	GetDlgItem(BUTTON_RITHIMC_CONNECT)->GetWindowTextW(str);
	
	if (str == _T("Connect")) 
	{
		m_marketDataConnectionStatus.SetWindowText(_T("Trying to logon ..."));
		pMainWnd->rithmicStatus = 1;
		pMainWnd->Invalidate(0);
		m_marketDataConnectionStatus.RedrawWindow();	
		AfxBeginThread(RithmicConnectionThread, this);
	}
	else
	{
		pMainWnd->rithmicCallbacks->Disconnect();
		StopWaitCursor();
		pMainWnd->DisconnectFromFeed(RITHMIC);
	}
}

HBRUSH RithmicDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if (pWnd == &m_marketDataConnectionStatus) {
		pDC->SetBkMode(TRANSPARENT);
		switch (pMainWnd->rithmicStatus) 
		{
		case 0:	hbr = ::CreateSolidBrush(RGB(255, 0, 0)); break;		//disconnect красный
		case 1:	hbr = ::CreateSolidBrush(RGB(255, 155, 55)); break;		//trying to connect коричневый
		case 2:	hbr = ::CreateSolidBrush(RGB(0, 255, 128));	break;		//connect ok зеленый
		}
	}

	/*switch (pWnd->GetDlgCtrlID())
	{
	case IDC_STATIC:
		pDC->SetBkMode(TRANSPARENT);
		SetTextColor(pDC->m_hDC, RGB(255,255,255));
		SetBkColor(pDC->m_hDC, BGRNCOLOR);
		return (HBRUSH)(backgroundBrush.GetSafeHandle());	 
		break;
	}*/


	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

BOOL RithmicDialog::PreTranslateMessage(MSG* pMsg)
{
	UpdateDialogControls(this, FALSE);
	return CDialogEx::PreTranslateMessage(pMsg);
}
void RithmicDialog::OnUpdateConnection(CCmdUI* pCmdUI)
{
	if (pMainWnd->rithmicStatus == 2) {
		GetDlgItem(RITHMIC_LOGIN)->EnableWindow(0);
		GetDlgItem(RITHMIC_PASSWORD)->EnableWindow(0);
		pCmdUI->SetText(L"Disconnect");
	} else {
		GetDlgItem(RITHMIC_LOGIN)->EnableWindow(1);
		GetDlgItem(RITHMIC_PASSWORD)->EnableWindow(1);
		pCmdUI->SetText(L"Connect");
	}
}

BOOL RithmicDialog::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (waitCursor)
	{
		RestoreWaitCursor();
		return TRUE;
	}

	return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}


void RithmicDialog::OnClose()
{
	if (pMainWnd->rithmicStatus == 0)
	{
		pMainWnd->rithmicStatus = -1;
		pMainWnd->GetDlgItem(IDC_CONNECTION_STATUS_FEED1)->Invalidate(0);
	}

	CDialogEx::OnClose();
}

void RithmicDialog::ReadDataFromINI()
{
	IniFileWorker ini;

	try
	{
		ini.Open(".\\ini\\Algovisor.ini");

		params.saveLoginPass = ini.ReadInt("Rithmic", "saveLoginPass") == 1 ? true : false;
		params.login = ini.ReadStr("Rithmic", "login");
		params.password = ini.ReadStr("Rithmic", "password");
		params.connectionType = ini.ReadInt("Rithmic", "connectionType");
	}
	catch (IniFileException&)
	{
		static_cast<MainWindow*>(GetParent())->UpdateActivityWindow(_T("ERROR: Algovisor.ini has not the correct format"));
	}

	ini.Close();
}

void RithmicDialog::SaveDataToINI()
{
	IniFileWorker ini;

	try
	{
		if (params.saveLoginPass)
		{
			ini.Open(".\\ini\\Algovisor.ini");
			ini.WriteInt("Rithmic", "saveLoginPass", params.saveLoginPass == true ? 1 : 0);
			ini.WriteStr("Rithmic", "login", params.login);
			ini.WriteStr("Rithmic", "password", params.password);
			ini.WriteInt("Rithmic", "connectionType", params.connectionType);
		}
	}
	catch (IniFileException&)
	{
		static_cast<MainWindow*>(GetParent())->UpdateActivityWindow(_T("ERROR: Algovisor.ini has not the correct format"));
	}

	ini.Close();
}
void RithmicDialog::StartWaitCursor()
{
	BeginWaitCursor();
	EnableWindow(0);
	waitCursor = true;
}

void RithmicDialog::StopWaitCursor()
{
	EnableWindow(1);
	EndWaitCursor();
	waitCursor = false;
	Invalidate(0);
}

bool RithmicDialog::CheckConnectionParameters()
{
	if (IsDlgButtonChecked(RITHIMC_SAVE_LOGIN_PASSWORD) == BST_CHECKED)
	{
		params.saveLoginPass = 1;

		CString str;

		rLogin.GetWindowTextW(str);
		if (str.IsEmpty())
		{
			pMainWnd->UpdateActivityWindow(_T("Rithmic: Please enter login"));
			return false;
		}
		params.login = CStringA(str);

		rPassword.GetWindowTextW(str);
		if (str.IsEmpty())
		{
			pMainWnd->UpdateActivityWindow(_T("Rithmic: Please enter password"));
			return false;
		}
		params.password = CStringA(str);

		params.connectionType = connectionTypeCombo.GetCurSel();
	}
	else
	{
		params.saveLoginPass = 1;
		params.login = "";
		params.password = "";
		params.connectionType = 0;
	}

	return true;
}
