#pragma once
#include "resource.h"

class MainWindow;
class CQGTicker;

struct LOGINDATA
{
	wchar_t login[100];
	wchar_t password[100];
	bool demo;
	GUID guID;
};
struct TEXT
{
	wchar_t text[100];
};
struct CONNECTIONSTATUS
{
	BYTE connectionStatus;
	wchar_t text[100];
};
struct SUBSCRIBE
{
	wchar_t ticker[100];
	bool dom;
};
struct INSTRUMENT_PROPERTIES
{
	wchar_t ticker[100];
	//wchar_t fullName[100];
	long instrumentID;
	wchar_t description[100];
	//wchar_t exchange[100];
	wchar_t expirationDate[100];
	int type;
	long exchID;
	int MinTickInc;						//минимальный шаг цены по инструменту
	int precision;						//кол-во значащих знаков после запятой
	//int sessionMask;
};
struct CQG_INSTRUMENT_TRADE
{
	//wchar_t fullName[100];
	long instrumentID;
	SYSTEMTIME timestamp;
	double price;
	long volume;
};
struct CQG_INSTRUMENT_BBO_ASK
{
	//wchar_t fullName[100];
	long instrumentID;
	SYSTEMTIME timestamp;
	double price;
	long size;
};
struct CQG_INSTRUMENT_BBO_BID
{
	//wchar_t fullName[100];
	long instrumentID;
	SYSTEMTIME timestamp;
	double price;
	long size;
};
struct CQG_INSTRUMENT_DOM
{
	//wchar_t fullName[100];
	long instrumentID;
	SYSTEMTIME timestamp;
	double bidPrice[11];
	double askPrice[11];
	long bidSize[11];
	long askSize[11];
	long domLevelCount;
};

// CQGDialog dialog

class CQGDialog : public CDialogEx
{
public:
	enum : UINT { CQG_CONNECT = 1, CQG_TEXT, CQG_CONNECTION_STATUS, CQG_DISCONNECT, CQG_SUBSCRIBE, 
		CQG_UNSUBSCRIBE, CQG_UNSUBSCRIBEALL, CQG_INSTRUMENT_PROPERTIES, CQG_SHUTDOWN, CQG_TRADE, CQG_ASK, CQG_BID, CQG_DOM };

	CQGDialog(MainWindow* pParent = NULL);   // standard constructor
	virtual ~CQGDialog();

// Dialog Data
	enum { IDD = DIALOG_CQG_CONNECTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual void PostNcDestroy();

public:
	void CQGInstrumentProperty(INSTRUMENT_PROPERTIES* ip);
	CQGTicker* GetSelectedInstrument();

	bool waitCursor;

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedConnectCqg();
	afx_msg void OnUpdateConnection(CCmdUI* pCmdUI);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);	
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnClose();
};
