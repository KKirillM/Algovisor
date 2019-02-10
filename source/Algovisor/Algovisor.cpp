#include "stdafx.h"
#include "Algovisor.h"
#include "LoginDlg.h"
#include "MainWindow.h"
#include "PrivateKey.h"
#include "Auxiliary_methods.h"
#include <timeapi.h> // for timeGetDevCaps
#pragma comment(lib, "winmm.lib")  /* include winmm.lib */ // for timeGetDevCaps

// #ifdef _DEBUG
// #define new DEBUG_NEW
// #endif

// CAAApp

BEGIN_MESSAGE_MAP(CAAApp, CWinApp)
//	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


CAAApp theApp;

CAAApp::CAAApp()
{
	// Place all significant initialization in InitInstance
}

CAAApp::~CAAApp()
{
}



extern std::vector<RTicker*> rithmic_data;
extern std::vector<CQGTicker*> cqg_data;
extern std::vector<CQGTicker*> ninja_data;

// CAAApp initialization

LRESULT CAAApp::ProcessWndProcException(CException* e, const MSG* pMsg)
{
	// This is where we land with some MFC exceptions.
	// If we needed to show a message or something, we could do that here.
	// However, in most cases, we just want to cause MFC to throw the 
	// exception out to CrashRpt.
 
	// Make MFC throw (and not catch) this exception so that CrashRpt can catch it.
	THROW_LAST();
 
	return 0;
	//return CWinApp::ProcessWndProcException(e, pMsg);
}

BOOL CAAApp::InitInstance()
{
#ifdef ASPROTECT
	::PSA_DummyFunction();	//функция заглушка для подключения DLL защиты
#endif

#ifdef _DEBUG
	//выводит отчет об утечке памяти по закрытию приложения
	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_DELAY_FREE_MEM_DF );
	//_CrtSetBreakAlloc(21374);
#endif

	//защита от запуска двух копий приложения
    //if (CheckProcessInMemory(_T("Algovisor.exe")) > 1)
    //{
        HWND hwn1 = ::FindWindow(NULL, _T("Algovisor account authentication"));
	    HWND hwn2 = ::FindWindow(NULL, _T("Algovisor"));
        HWND hwn3 = ::FindWindow(NULL, _T("Checking the licence"));
	
    if (hwn1 || hwn2 || hwn3)
    {
	    if (hwn1)
	    {
		    if (::IsIconic(hwn1))
			    ::ShowWindow(hwn1, SW_RESTORE);
		
		    ::SetForegroundWindow(hwn1);
	    }
	    
        if (hwn2)
	    {
		    if (::IsIconic(hwn2))
			    ::ShowWindow(hwn2, SW_RESTORE);
		
		    ::SetForegroundWindow(hwn2);
	    }

        if (hwn3)
        {
            if (::IsIconic(hwn3))
                ::ShowWindow(hwn3, SW_RESTORE);

            ::SetForegroundWindow(hwn3);
        }

		return FALSE;
    }

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;    // Set this to include all the common control classes you want to use in your application.
	InitCommonControlsEx(&InitCtrls);

	// Initializes the COM library
	//CoInitialize(NULL);

	CWinApp::InitInstance();

	//AfxEnableControlContainer();

	//инициализируем тонкие блокировки
	::InitializeSRWLock(&SRWLockRithmic);
	::InitializeSRWLock(&srwLockNinja);

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	//CShellManager *pShellManager = new CShellManager;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(APPLICATION_NAME);

	//удаление cookie файла, который мог остаться с прошлой сессии
	DeleteFile(_T("cookie"));

#if !defined(TRIAL_VERSION) /*&& defined(CONNECT_THROUGH_SERVER)*/ //проверка наличия файла ключа файла (только для платной версии)
	/*OFSTRUCT reOpenBuff;
	HFILE hf = OpenFile("key.cpr", &reOpenBuff, OF_EXIST);
	if (hf == HFILE_ERROR)
	{
		PrivateKey keyDlg;
		INT_PTR nResponse = keyDlg.DoModal();
		if (nResponse != IDOK)
        {
			return FALSE;
        }
	}*/
#endif

	// это защита для платного пользователя делалась (версия без привязки к серверу)
	//{
	//	SYSTEMTIME st;
	//	::GetLocalTime(&st);
	//	if (st.wYear != 2018 ||
	//		((st.wMonth <= 3 /*&& st.wDay < 11*/) || (st.wMonth > 5) || (st.wMonth == 5 && st.wDay > 18)))
	//	{
	//		MessageBox(NULL, L"The paid period has expired", L"Algovisor", MB_ICONEXCLAMATION);
	//		return FALSE;
	//	}
	//}
//#ifdef CONNECT_THROUGH_SERVER
    LoginDlg dlg;
    INT_PTR nResponse = dlg.DoModal();

    if (nResponse == IDOK)
    {

    }
    else if (nResponse == IDCANCEL)
    {
        return FALSE;
    }
//#endif    

    // Parse command line for standard shell commands, DDE, file open
    //CCommandLineInfo cmdInfo;
    //ParseCommandLine(cmdInfo);
    //if (!ProcessShellCommand(cmdInfo))
    //    return FALSE;

	// Delete the shell manager created above.
	/*if (pShellManager != NULL)
	{
		delete pShellManager;
	}*/

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.

	TIMECAPS tc;
    MMRESULT mmErr;
    mmErr = timeGetDevCaps(&tc, sizeof(tc));
	//if (mmErr == MMSYSERR_NOERROR)
	mmErr = timeBeginPeriod(tc.wPeriodMin);
	//if (mmErr == MMSYSERR_NOERROR)

	return TRUE;
}

 int CALLBACK CallBackForCrashReport(CR_CRASH_CALLBACK_INFO* pInfo)
 {   
	CString sExeptionTypeStr;

	//добавим имя пользователя к xml-репорту
	crAddProperty(login.GetBuffer(), _T("User name: "));

	sExeptionTypeStr.Format(_T("Exception details:\nType: %d\ncode: 0x%x\nexpression: %s\nfunction: %s\nfile: %s\nline: %s"), 
								pInfo->pExceptionInfo->exctype, 
								pInfo->pExceptionInfo->code,
								pInfo->pExceptionInfo->expression,
								pInfo->pExceptionInfo->function, 
								pInfo->pExceptionInfo->file, 
								pInfo->pExceptionInfo->line);

	::MessageBox(NULL, sExeptionTypeStr.GetString(), _T("Exception"), MB_OK | MB_ICONERROR);

	return CR_CB_DODEFAULT;
 }

int CAAApp::InitCrashRpt()
{
	// Install crash reporting
	CR_INSTALL_INFO info;
	TCHAR version[32];
	int nResult;

	memset(&info, 0, sizeof(CR_INSTALL_INFO));
	info.cb = sizeof(CR_INSTALL_INFO);  
	std::wstring wstr(APPLICATION_NAME);
	wstr += ' ' + login;
	info.pszAppName = wstr.c_str();										// Имя приложения.
	LoadString(GetModuleHandle(NULL), ALGOVISOR_VERSION, version, 32);
	info.pszAppVersion = version;										// Версия приложения.
	//info.pszEmailTo = L"kkm-biz@mail.ru";								// E-mail адрес для отправки отчетов об ошибке.
	//info.pszSmtpProxy = L"algovisor.com";
	//info.pszSmtpLogin = L"dump@algovisor.com";
	//info.pszSmtpPassword = L"m32D94b234FFdub82";
	//info.pszEmailSubject = L"123";									// заголовок в письме	
	//info.pfnCrashCallback = CallBackForCrashReport;					// коллбэк функция которая вызывается при падении
	info.pszUrl = _T("http://algovisor.com/reportdump");				// URL скрипта (используется для доставки отчета через соединение HTTP).                   
	//info.pszCrashSenderPath =											// Имя каталога, где находится CrashSender.exe.
	//info.pfnCrashCallback = NULL;										// Пользовательская callback-функция (будет вызвана при сбое).
	//info.uPriorities;													// Массив приоритетов отправки отчета.
	info.dwFlags |= CR_INST_ALL_POSSIBLE_HANDLERS | 
					CR_INST_ALLOW_ATTACH_MORE_FILES | 
					CR_INST_SEND_QUEUED_REPORTS |
					/*CR_INST_AUTO_THREAD_HANDLERS |*/
					CR_INST_SEND_MANDATORY |
					CR_INST_NO_GUI;										// Набор флагов.				
	info.pszPrivacyPolicyURL = _T("http://algovisor.com/privacy");		// URL для политики конфиденциальности.
	info.uMiniDumpType = /*_MINIDUMP_TYPE::*/MiniDumpNormal;								// Тип минидампа (влияет на размер отчета).

	nResult = crInstall(&info);
	
	if(nResult!=0)
	{
		TCHAR buff[256];
		crGetLastErrorMsg(buff, 256);
        TRACE1("%s", buff);
		MessageBox(NULL, buff, _T("crInstall error"), MB_OK);
		return 1;
	}

	// Set crash callback function
	crSetCrashCallback(CallBackForCrashReport, NULL);

	// Take screenshot of the app window at the moment of crash
	crAddScreenshot2(CR_AS_VIRTUAL_SCREEN|CR_AS_USE_JPEG_FORMAT, 95);

	return 0;
}

int CAAApp::ExitInstance()
{
	// Close the COM library and unload all DLLs
	//CoUninitialize();

#ifdef ASPROTECT
	PSA_Uninitialize(); //удаление ресурсов (потоки, динамическая память) и выгрузка dll защиты из памяти процесса
#endif

	//удаление из контейнеров всех инструментов
	for (size_t i = 0; i < rithmic_data.size(); ++i) 
		delete rithmic_data[i];
	rithmic_data.clear();

	for (size_t i = 0; i < cqg_data.size(); ++i) 
		delete cqg_data[i];
	cqg_data.clear();

	for (size_t i = 0; i < ninja_data.size(); ++i) 
		delete ninja_data[i];
	ninja_data.clear();

	//удаление cookie файла
	DeleteFile(L"cookie");

	//::CoUninitialize();

	return CWinApp::ExitInstance();;
}

BOOL CAAApp::PreTranslateMessage(MSG* pMsg)
{
	//генерация крэш-репорта при нажатии комбинации клавиш Q+W+Ctrl+Shift
	if (pMsg->message == WM_KEYDOWN && 
		(GetKeyState(VK_LCONTROL) & 0x1000) && 
		(GetKeyState(VK_SHIFT) & 0x1000) && 
		(GetKeyState(0x51) & 0x1000) &&
		(GetKeyState(0x57) & 0x1000))
	{
		CR_EXCEPTION_INFO ei;
		memset(&ei, 0, sizeof(CR_EXCEPTION_INFO));
		ei.cb = sizeof(CR_EXCEPTION_INFO);
		ei.exctype = CR_SEH_EXCEPTION;
		ei.code = 12345;
		ei.pexcptrs = NULL;

		int result = crGenerateErrorReport(&ei);

		if(result!=0)
		{
			// If goes here, crGenerateErrorReport() has failed
			// Get the last error message
			TCHAR szErrorMsg[256];
			crGetLastErrorMsg(szErrorMsg, 256);
			::MessageBox(NULL, szErrorMsg, _T("Manual CrashReport generation error"), MB_OK);
		}
   
		// Manually terminate program
		ExitProcess(0);
	}

	return CWinApp::PreTranslateMessage(pMsg);
}


int CAAApp::Run()
{
    // инициализация Crash Report
    if (InitCrashRpt())
    {
        return FALSE;
    }

    MainWindow* dlgMain = MainWnd::GetInstance();
    dlgMain->DoModal();

    BOOL bRun = CWinApp::Run();

    // Uninstall crash reporting
    // в этом месте обязательно надо чтобы уже были завершены все потоки
    crUninstall();

    return bRun;
}
