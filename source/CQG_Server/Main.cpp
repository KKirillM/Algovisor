#include "stdafx.h"
#include "CQG_Server.h"

BOOL InitWindow(int nCmdShow, HINSTANCE hInstance);
LONG APIENTRY MainWndProc(HWND hWnd,  UINT message, UINT wParam, LONG lParam);
HWND hwnMain;

CQGServer cqg;

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) 
{
	UNREFERENCED_PARAMETER(hPrevInstance);

	//защита от запуска двух копий приложения
	if (::FindWindow(L"CQG_Algovisor_Server", NULL))
		return 0;

	//убираем возможность запуска без алговизора
#ifndef _DEBUG
	if (!::FindWindow(NULL, AlgovisorStr))
		return 0;
#endif	

	CoInitialize(NULL);	//Initialize the COM Library

	InitWindow(nCmdShow, hInstance);
		
	MSG msg ;
	while (::GetMessage(&msg, 0, 0, 0))
	{

		::DispatchMessage(&msg) ;
	}

	CoUninitialize();	//Uninitialize the COM Library
  
   return(0);
}

BOOL InitWindow(int nCmdShow, HINSTANCE hInstance) 
{
	// Fill in window class structure with parameters
	// that describe the main window.
	WNDCLASS wcListview;
	wcListview.style = CS_HREDRAW | CS_VREDRAW;                     
	wcListview.lpfnWndProc = (WNDPROC)MainWndProc; 
	wcListview.cbClsExtra = 0;              
	wcListview.cbWndExtra = 0;              
	wcListview.hInstance = hInstance;
	wcListview.hIcon = ::LoadIcon(NULL, IDI_APPLICATION); 
	wcListview.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcListview.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH); 
	wcListview.lpszMenuName = NULL;  
	wcListview.lpszClassName = L"CQG_Algovisor_Server";

	::RegisterClass(&wcListview);
	
	hwnMain = CreateWindowW(
		L"CQG_Algovisor_Server",
		L"CQG_Algovisor_Server", 
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,               
		NULL,               
		hInstance,          
		NULL);

	return TRUE ;
}

LONG APIENTRY MainWndProc(HWND hWnd, UINT message, UINT wParam, LONG lParam)
{
	PCOPYDATASTRUCT pCDS;

	switch (message) 
	{
	case WM_CREATE:
		
		break ;

	case WM_SIZE:
		
		break;

	case WM_DESTROY:
		cqg.Disconnect();
		::PostQuitMessage(0);
		break ;

	case WM_CLOSE:
		
		break;

	case WM_COPYDATA:
		pCDS = (PCOPYDATASTRUCT)lParam;
		switch(pCDS->dwData)
		{
		case CQGServer::CQG_CONNECT:
			cqg.Connect((wchar_t*)((LOGINDATA*)(pCDS->lpData))->login, (wchar_t*)((LOGINDATA*)(pCDS->lpData))->password, 
				(bool)((LOGINDATA*)(pCDS->lpData))->demo, (GUID)((LOGINDATA*)(pCDS->lpData))->guID);
			break;
		case CQGServer::CQG_DISCONNECT:
			cqg.Disconnect();
			break;
		case CQGServer::CQG_SUBSCRIBE:
			cqg.Subscribe((wchar_t*)((SUBSCRIBE*)(pCDS->lpData))->ticker, (bool)((SUBSCRIBE*)(pCDS->lpData))->dom);
			break;
		case CQGServer::CQG_UNSUBSCRIBE:
			cqg.Unsubscribe((wchar_t*)((TEXT*)(pCDS->lpData))->text);
			break;
		case CQGServer::CQG_UNSUBSCRIBEALL:
			cqg.UnsubscribeAll();
			break;
		case CQGServer::CQG_SHUTDOWN:
			cqg.Disconnect();
			::PostQuitMessage(0);
			break;
		case WM_APP+1:
			cqg.SendInstrumentTrade(cqg.itVec[0]);
			cqg.itVec.erase(cqg.itVec.begin());
			break;
		}
		break;
	default:
		return (DefWindowProc(hWnd, message, wParam, lParam)) ;
	}
	return 0 ;
}