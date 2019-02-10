#pragma once
#include "resource.h"
#include "afxwin.h"

// PrivateKey dialog

class PrivateKey : public CDialogEx
{
	DECLARE_DYNAMIC(PrivateKey)

public:
	PrivateKey(CWnd* pParent = NULL);   // standard constructor
	virtual ~PrivateKey();
	virtual BOOL OnInitDialog();
// Dialog Data
	enum { IDD = DIALOG_PRIVATE_KEY };

	CStatic logo;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedFindFilePath();
	afx_msg void OnBnClickedOk();
};
