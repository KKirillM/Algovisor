#include "stdafx.h"

#include "Algovisor.h"
#include "Auxiliary_methods.h"
#include "ConnectionDialog.h"

using namespace std;

BEGIN_MESSAGE_MAP(ConnectionDialog, CDialogEx)
	ON_WM_SETCURSOR()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

ConnectionDialog::ConnectionDialog(CWnd* pParent, tstring name, UINT _windowID, UINT _loginFieldID, 
								   UINT _passwordFieldID, UINT _saveCheckID) : 
	CDialogEx(_windowID, pParent),
	loginFieldID(_loginFieldID),
	passwordFieldID(_passwordFieldID),
	saveCheckID(_saveCheckID),
	connection_name(name),
	waitCursor(false)
{
	saveToRegistryParamName.assign(_T("NeedToSave"));
	saveToRegistryParamName.insert(saveToRegistryParamName.end(), connection_name.begin(), connection_name.end());
	saveToRegistryParamName.append(_T("Auth"));

	loginRegistryParamName.assign(_T("Login"));
	loginRegistryParamName.insert(loginRegistryParamName.end(), connection_name.begin(), connection_name.end());

	passwordRegistryParamName.assign(_T("Password"));
	passwordRegistryParamName.insert(passwordRegistryParamName.end(), connection_name.begin(), connection_name.end());
}

ConnectionDialog::~ConnectionDialog()
{
}

void ConnectionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL ConnectionDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

	//проверяем наличие ключа в реестре в ID и заносим его значение в поле редактирования
	if (theApp.GetProfileInt(APPLICATION_NAME, saveToRegistryParamName.c_str(), 0)) 
	{
		CheckDlgButton(saveCheckID, 1);
		GetDlgItem(loginFieldID)->SetWindowTextW(SFLB_0_CryptString(theApp.GetProfileString(APPLICATION_NAME, loginRegistryParamName.c_str())));
		GetDlgItem(passwordFieldID)->SetWindowTextW(SFLB_0_CryptString(theApp.GetProfileString(APPLICATION_NAME, passwordRegistryParamName.c_str())));
	}

    return TRUE;  // return TRUE  unless you set the focus to a control

}

void ConnectionDialog::GetLoginAndPassword()
{
	CString wLogin, wPassword;

	GetDlgItem(loginFieldID)->GetWindowText(wLogin);
	if (wLogin.IsEmpty()) 
	{
		throw ConnectionException("Plaza2: Please enter login");
	}

	GetDlgItem(passwordFieldID)->GetWindowTextW(wPassword);
	if (wPassword.IsEmpty()) 
	{
		throw ConnectionException("Plaza2: Please enter password");
	}

	login = wLogin;
	password = wPassword;
}

void ConnectionDialog::StartWaitCursor()
{
	//замораживаем окно на время подключения или отключения
	waitCursor = true;
	BeginWaitCursor();
	EnableWindow(0);
}

void ConnectionDialog::StopWaitCursor()
{
	waitCursor = false;
	EndWaitCursor();
	EnableWindow(1);
	Invalidate(0);
}

void ConnectionDialog::SaveConnectionDataToRegistry(bool save)
{
	tstring _login = login;
	tstring _password = password;

	if (save) 
	{
		theApp.WriteProfileString(APPLICATION_NAME, loginRegistryParamName.c_str(), SFLB_0_CryptStringW(_login).c_str());
		theApp.WriteProfileString(APPLICATION_NAME, passwordRegistryParamName.c_str(), SFLB_0_CryptStringW(_password).c_str());
		theApp.WriteProfileInt(APPLICATION_NAME, saveToRegistryParamName.c_str(), 1);
	} else {
		theApp.WriteProfileString(APPLICATION_NAME, loginRegistryParamName.c_str(), _T(""));
		theApp.WriteProfileString(APPLICATION_NAME, passwordRegistryParamName.c_str(),  _T(""));
		theApp.WriteProfileInt(APPLICATION_NAME, saveToRegistryParamName.c_str(), 0);
	}
}

BOOL ConnectionDialog::PreTranslateMessage(MSG* pMsg)
{
	UpdateDialogControls(this, FALSE);
	return CDialogEx::PreTranslateMessage(pMsg);
}

void ConnectionDialog::OnOK() {}
void ConnectionDialog::OnCancel() { DestroyWindow(); }
void ConnectionDialog::PostNcDestroy()
{
	CDialogEx::PostNcDestroy();
	delete this;
}

BOOL ConnectionDialog::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (waitCursor)
	{
		RestoreWaitCursor();
		return TRUE;
	}

	return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}


void ConnectionDialog::OnClose()
{
	/*if (pMainWnd->cqgStatus == 0)
	{
		pMainWnd->cqgStatus = -1;
		pMainWnd->GetDlgItem(IDC_CONNECTION_STATUS_FEED2)->Invalidate(0);
	}*/

	CDialogEx::OnClose();
}