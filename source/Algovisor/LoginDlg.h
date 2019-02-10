#pragma once


struct computerInfo;
// LoginDlg dialog
class LoginDlg : public CDialogEx
{
// Construction
public:
	LoginDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_LOGIN };
	enum LogonState { NOSTATUS, WAIT, LOGCOPMLETE, LOGERROR };

	BOOL GetWaitCursor() { return waitCursor; }
	VOID SetWaitCursor(BOOL b) { waitCursor=b; }
	VOID CDialogOK() { CDialogEx::OnOK(); }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support	

// Implementation
protected:
	HICON m_hIcon;
	LogonState m_logonState;
	std::string result;
	BOOL waitCursor;
	PsProtUInt32_1 timerMaxValue;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};
