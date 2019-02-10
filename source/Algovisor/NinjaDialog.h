#pragma once
#include "resource.h"

class MainWindow;

struct Instrument_DATA
{
	long value;
	double tickSize;
	wchar_t ID[50];
	wchar_t ticker[50];	
	wchar_t description[50];
	wchar_t expiration[50];
	wchar_t exchange[50];
	wchar_t securityStr[50];
	bool type;	
};

struct Trade_DATA
{
	long value;
	unsigned short hour;
	unsigned short minute;
	unsigned short second;
	unsigned short millisecond;
	wchar_t ticker[50];
	double Price;
	long long Volume;	
};
		
struct BBO_DATA
{
	long value;
	double Price;
	wchar_t ticker[50];
};

struct DOM_DATA
{
	long value;
	wchar_t ticker[50];
	unsigned short hour;
	unsigned short minute;
	unsigned short second;
	unsigned short millisecond;
	double Price;
	long long Volume;	
	int pos;	
	wchar_t type;
	wchar_t agressor;
};

struct DELETE_INSTRUMENT
{
	long value;
	wchar_t ticker[50];
};

// NinjaDialog dialog

class NinjaDialog : public CDialogEx
{
public:
	enum : int { NINJA_INSTRUMENT_DATA = 101, NINJA_TRADE, NINJA_BBO_ASK, NINJA_BBO_BID, NINJA_DOM, INSTRUMENT_DELETE };

	NinjaDialog(MainWindow* pParent = NULL);   // standard constructor
	virtual ~NinjaDialog();

	void RunPipeServer();
	void StopPipeServer();

// Dialog Data
	enum { IDD = DIALOG_NINJATRADER_CONNECTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual void PostNcDestroy();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedConnect();
	afx_msg void OnUpdateConnection(CCmdUI* pCmdUI);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);	
	afx_msg void OnClose();
};
