  
// AA.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CAAApp:
// See AA.cpp for the implementation of this class
//

class CAAApp : public CWinApp
{
public:
	CAAApp();
	~CAAApp();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	//переопределение этих двух методов рекомендовано для подключения CrushReport http://crashrpt.sourceforge.net/docs/html/enabling_crash_reporting.html
	virtual LRESULT ProcessWndProcException(CException* e, const MSG* pMsg);
    virtual int Run();
						
	int InitCrashRpt();
	

	DECLARE_MESSAGE_MAP()
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);

};  // CAAApp 

extern CAAApp theApp;