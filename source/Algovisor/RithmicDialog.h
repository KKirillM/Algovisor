#pragma once
#include "resource.h"
#include "afxwin.h"
#include "afxcmn.h"

class RithmicCallbacks;
class RTicker;
class MainWindow;

struct RithmicDialogParameters
{
	bool saveLoginPass;
	std::string login;
	std::string password;
	int connectionType;
};

class RithmicDialog : public CDialogEx
{
	friend UINT AFX_CDECL RithmicConnectionThread(LPVOID pointer);
public:
	enum ConnectionType : int { Rithmic_Paper_Trading, Rithmic_01_Test, Rithmic_01_Aggregated, Rithmic_01 };

	RithmicDialog(MainWindow* pParent = NULL);
	virtual ~RithmicDialog();

	enum { IDD = DIALOG_RITHMIC_CONNECTION };

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnOK();
	virtual void OnCancel();
	virtual void PostNcDestroy();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	void ReadDataFromINI();
	void SaveDataToINI();
	void StartWaitCursor();
	void StopWaitCursor();
	bool CheckConnectionParameters();

private:
	RithmicDialogParameters params;
	CComboBox connectionTypeCombo;
	CStatic m_marketDataConnectionStatus;
	CEdit rLogin;
	CEdit rPassword;
	BOOL waitCursor;

	CStatic rithmicLicense;

	//CBrush backgroundBrush;

public:
	afx_msg void ConnectButtonPush();
	afx_msg void OnUpdateConnection(CCmdUI* pCmdUI);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT RepaintCursor(WPARAM, LPARAM);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnClose();

};