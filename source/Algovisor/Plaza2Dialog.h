#pragma once
#include "resource.h"

struct Plaza2DialogParameters
{
    bool saveLoginPass;
    bool useRemoteRouter;
    std::string login;
    std::string password;
    std::string ip;
    std::string port;
    std::string remoteRouterPassword;
};

class Plaza2Dialog : public CDialogEx
{
public:

	enum { IDD = DIALOG_PLAZA2_CONNECTION };

	Plaza2Dialog();
	virtual ~Plaza2Dialog();

    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
    void Connect();
    Plaza2ConnectionStatus GetConnectionStatus();
    void CheckConnectionParameters();
    void ReadDataFromINI();
    void SaveDataToINI();
	void StartWaitCursor();
    void StopWaitCursor();
    void UpdateControls();

private:
    Plaza2DialogParameters params;
	std::string routerPath;
	std::string iniPath;
	std::string connectionString;
	std::string environmentString;

private:
	afx_msg void OnBnClickedConnect();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnBnClickedCheckPlaza2RemoteRouter();
    afx_msg void OnClose();
    
    DECLARE_MESSAGE_MAP()

};  // Plaza2Dialog

