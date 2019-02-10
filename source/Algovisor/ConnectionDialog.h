#pragma once


class ConnectionException : public std::exception 
{
public:
	ConnectionException(const char* message) : std::exception(message) {}
};

class ConnectionDialog : public CDialogEx
{
public:
	ConnectionDialog(CWnd* pParent, tstring name, UINT _connectButtonID, UINT _loginFieldID, UINT _passwordFieldID, UINT _saveCheckID);
	virtual ~ConnectionDialog();

protected:
	virtual void Connect() = 0;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual void PostNcDestroy();

protected:
	void GetLoginAndPassword();
	void StartWaitCursor();
	void StopWaitCursor();
	void SaveConnectionDataToRegistry(bool save);

protected:
	bool waitCursor;

	tstring connection_name;
	tstring login;
	tstring password;
	
	tstring saveToRegistryParamName;
	tstring loginRegistryParamName;
	tstring passwordRegistryParamName;
	
	UINT loginFieldID;
	UINT passwordFieldID; 
	UINT saveCheckID;
	
public:	
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()
};

