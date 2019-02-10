
// LightInstaller.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "LightInstaller.h"

using namespace std;

#define BUFFERSIZE /*16384*/ 1024
#define WM_UPDATE_TEXT WM_APP+1
#ifdef TRIAL_VERSION
#define U_MSI_FILE_NAME L"algovisor_demo_setup.msi"
#define A_MSI_FILE_NAME "algovisor_demo_setup.msi"
#else
#define U_MSI_FILE_NAME L"algovisor_setup.msi"
#define A_MSI_FILE_NAME "algovisor_setup.msi"
#endif

HANDLE hThread = INVALID_HANDLE_VALUE;
HWND hDlgWnd;
FILE* f;
//FILE* log_file;

// The normal HANDLE_MSG macro in WindowsX.h does not work properly for dialog
// boxes because DlgProc return a BOOL instead of an LRESULT (like
// WndProcs). This chHANDLE_DLGMSG macro corrects the problem:
#define chHANDLE_DLGMSG(hwnd, message, fn)                 \
   case (message): return (SetDlgMsgResult(hwnd, uMsg,     \
      HANDLE_##message((hwnd), (wParam), (lParam), (fn))))

// Sets the dialog box icons
inline void chSETDLGICONS(HWND hwnd, int idi) {
   SendMessage(hwnd, WM_SETICON, TRUE,  (LPARAM) 
      LoadIcon((HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 
         MAKEINTRESOURCE(idi)));
   SendMessage(hwnd, WM_SETICON, FALSE, (LPARAM) 
      LoadIcon((HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 
      MAKEINTRESOURCE(idi)));
}

INT_PTR CALLBACK DialogFunc(HWND, UINT, WPARAM, LPARAM);
BOOL Dlg_OnInitDialog(HWND, HWND, LPARAM);
void Dlg_OnCommand(HWND, int, HWND, UINT);
unsigned int WINAPI DownloadFunction(void*);
void Dlg_UpdateText();
void die_with_error(TCHAR *errorMessage);
void die_with_wserror(TCHAR *errorMessage);
string CryptString(string str);
BOOL Is64BitOS();

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	unsigned int threadID;

	//создаем поток для скачивания инсталятора
	hThread = (HANDLE)_beginthreadex(NULL, 0, DownloadFunction, NULL, 0, &threadID);
	CloseHandle(hThread);

	//InitCommonControls(); 
	DialogBoxW(hInstance, MAKEINTRESOURCE(IDD_LIGHTINSTALLER_DIALOG), NULL, DialogFunc);

	return 0;
}

INT_PTR CALLBACK DialogFunc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	chHANDLE_DLGMSG(hwnd, WM_INITDIALOG, Dlg_OnInitDialog);
	chHANDLE_DLGMSG(hwnd, WM_COMMAND, Dlg_OnCommand);
	case WM_UPDATE_TEXT:
		{
			TCHAR str[124];
			SetDlgItemText(hwnd, IDC_INSTALLER_TEXT, str);
		}
		break;
	}
   
	return 0;
}

BOOL Dlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) 
{
	hDlgWnd = hwnd;

	//вывод диалогового окна в центре экрана
	RECT Rect;
	::GetWindowRect(hDlgWnd, &Rect);
	::SetWindowPos(hDlgWnd, HWND_TOPMOST, (::GetSystemMetrics(SM_CXSCREEN)/2 - ((Rect.right - Rect.left)/2)),
					   (::GetSystemMetrics(SM_CYSCREEN)/2 - ((Rect.bottom - Rect.top)/2)),
					   (Rect.right - Rect.left), (Rect.bottom - Rect.top), SWP_SHOWWINDOW);

	chSETDLGICONS(hwnd, IDR_MAINFRAME);

	return(TRUE);
}

void Dlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) 
{
	switch (id) {
	case IDCANCEL:
		if (hThread != INVALID_HANDLE_VALUE)
			TerminateThread(hThread, 1);
		fclose(f);
		DeleteFileW(U_MSI_FILE_NAME);
		EndDialog(hwnd, id);
		break;

	case IDOK:
		break;
	}
}

unsigned int WINAPI DownloadFunction(void* arg)
{
	if (!Is64BitOS())
	{
		die_with_wserror(L"Runtime error: available on 64-bit Windows only");
		return 1;
	}

    string response;
    int resp_leng;
 
	char buffer[BUFFERSIZE] = {0};
    struct sockaddr_in serveraddr;
    int sock;
 
    WSADATA wsaData;
	hostent* addr;
   // char *ipaddress = "5.45.119.245";  //использовать имя домена
    int port = 80;
#ifdef TRIAL_VERSION 
	//расшифровываем зашифрованный запрос
    //request+="GET /soft/algovisor_demo_setup.msi HTTP/1.0\r\n";
    //request+="Host: algovisor.com\r\n";
	//request+="X-installer:AlgovisorDemoSetup\r\n";
    //request+="\r\n";
	string request = CryptString("FGW$*uhn}%j`jayyb}aKqszwFi~hhn1MRKlqrw!'f@CEUYQXNPITN]P-LHi,(4<(&')?t<6=%=&9%<748**lhnn");
#else
	string request;
	/*request += "GET /soft/algovisor_setup.msi HTTP/1.0\r\n";
	request += "Host: algovisor.com\r\n";
	request += "X-installer:AlgovisorRealSetup\r\n";
	request += "\r\n";
	int size_req = request.size();
	request = CryptString(request);*/

	char data[96] = {
	0x46, 0x47, 0x57, 0x24, 0x2A, 0x75, 0x68, 0x6E, 0x7D, 0x25, 0x6A, 0x60, 0x6A, 0x61, 0x79, 0x79, 
	0x62, 0x7D, 0x61, 0x4B, 0x66, 0x73, 0x63, 0x6D, 0x69, 0x34, 0x76, 0x6F, 0x74, 0x3E, 0x57, 0x74, 
	0x75, 0x72, 0x0C, 0x15, 0x0B, 0x16, 0x2A, 0x22, 0x61, 0x45, 0x58, 0x58, 0x17, 0x0E, 0x4E, 0x5C, 
	0x56, 0x5D, 0x45, 0x5D, 0x46, 0x59, 0x45, 0x16, 0x5A, 0x55, 0x56, 0x31, 0x37, 0x66, 0x12, 0x29, 
	0x2F, 0x31, 0x37, 0x25, 0x29, 0x2A, 0x22, 0x3A, 0x73, 0x0B, 0x27, 0x2B, 0x22, 0x38, 0x26, 0x23, 
	0x3E, 0x20, 0x01, 0x31, 0x34, 0x3A, 0x04, 0x3D, 0x2D, 0x2F, 0x2B, 0x51, 0x57, 0x53, 0x55, 0x00
	};
	request.append(data);
	request = CryptString(request);
	/*string*/ //request = CryptString("FGW$*uhn}%j`jayyb}aKfscmi4vot>Wtur*\x22""aEXXN\V]E]FYEZUV17f)/17%)*\x22"":s'+\x22""8&#> 14:=-/+QWSU");
								   // FGW$*uhn}%j`jayyb}aKfscmi4vot>Wtur*"     aEXXN\V]E]FYEZUV17f)/17%)*"     :s'+"     8&#> 14:=-/+QWSU
	
#endif
    //init winsock
    if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
	{
        die_with_wserror(L"WSAStartup() failed");
		return 1;
	}
 
    //open socket
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		WSACleanup();
        die_with_wserror(L"socket failed");
		return 1;
	}

	addr = ::gethostbyname("algovisor.com");
	if (addr == NULL)
	{
		closesocket(sock);
		WSACleanup();
		die_with_wserror(L"server not found");
		return 1;
	}
 
    //connect
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family      = AF_INET;
	/*serveraddr.sin_addr.s_addr*/serveraddr.sin_addr.S_un.S_addr = *(ULONG*)(*(addr->h_addr_list));//inet_addr(ipaddress);
    serveraddr.sin_port        = htons((unsigned short) port);
    if (connect(sock, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
	{
		closesocket(sock);
		WSACleanup();
        die_with_wserror(L"connect failed");
		return 1;
	}
 
    //send request
    if (send(sock, request.c_str(), request.length(), 0) != request.length())
	{
        die_with_wserror(L"send() sent a different number of bytes than expected");
		return 1;
	}
 
    //get response
	int fileSize = 0;
	int writtenLength = 0;
	bool firstTransaction = false;
	TCHAR procent[15] = {0};
    response = "";
    resp_leng = BUFFERSIZE;
	BOOL res = DeleteFileW(U_MSI_FILE_NAME);
	::fopen_s(&f, A_MSI_FILE_NAME, "wb");
	//::fopen_s(&log_file, "answer.txt", "wb");
	//::setbuf(f, NULL);	//выключаем буферизацию для файла

    while (resp_leng/* == BUFFERSIZE*/)
    {
        resp_leng = recv(sock, (char*)&buffer, BUFFERSIZE-1, 0);

		//поправить этот кусок, т.к. первый пакет может быть 250 байт всего, а может и меньше
		if (!firstTransaction)
		{
			string str = (string)buffer;
			char* content_length = "Content-Length: ";
			char* pattern = "\x0D\x0A\x0D\x0A";
			size_t start_of_content_length = str.find(content_length);
			string length_of_file_string = str.c_str()+start_of_content_length+strlen(content_length);
			size_t end_of_content_length = length_of_file_string.find("\x0D");
			fileSize = atoi(string(length_of_file_string.c_str(), length_of_file_string.c_str()+end_of_content_length).c_str());
			SendMessageW(GetDlgItem(hDlgWnd, IDC_PROGRESS_BAR), PBM_SETRANGE32, 0, fileSize);

			size_t start_of_pattern = str.find(pattern);
			int len = 4/*strlen(str.c_str()+start_of_pattern)*/;
			writtenLength += resp_leng-start_of_pattern-len;
			fwrite(buffer+start_of_pattern+len, sizeof(char), writtenLength, f);
			firstTransaction = true;

			//char log_str[100];
			//sprintf(log_str, "resp_leng = %d,\n writtenLength = %d,\n start_of_pattern = %d \n", resp_leng, writtenLength, start_of_pattern);
			//fwrite(log_str, sizeof(char), strlen(log_str), log_file);
			continue;
		}

		writtenLength += resp_leng;
		
		wsprintf(procent, L"%d%%", (writtenLength*100)/fileSize);
		SetWindowText(GetDlgItem(hDlgWnd, IDC_PROCENT_TEXT), (LPCWSTR)procent);
		SendMessage(GetDlgItem(hDlgWnd, IDC_PROGRESS_BAR), PBM_SETPOS, writtenLength, 0);

        if (resp_leng > 0)
			 fwrite(buffer, sizeof(char), resp_leng, f);
    }
	::fclose(f);
 
    //disconnect
    closesocket(sock);
 
    //cleanup
    WSACleanup();

	//ShellExecute(0,L"open",U_MSI_FILE_NAME,NULL,NULL,SW_HIDE);
	
	//STARTUPINFO cif;
	//ZeroMemory(&cif,sizeof(STARTUPINFO));
	//PROCESS_INFORMATION pi;
	//if (CreateProcess(L"msiexec /i "U_MSI_FILE_NAME,NULL,NULL,NULL,FALSE,NULL,NULL,NULL,&cif,&pi)==TRUE)
	//{
	//	Sleep(1000);				// подождать
	//	TerminateProcess(pi.hProcess,NO_ERROR);	// убрать процесс
	//}

	//запускаем инсталятор в зависимости от того установлена текущая версия или нет
	HKEY key;
	LONG nResult =::RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{E696270D-0C33-4847-AA54-506976D9A09C}", NULL, KEY_READ, &key);
	if (nResult == ERROR_SUCCESS)
		WinExec("msiexec /i "A_MSI_FILE_NAME, SW_SHOW);
	else
	{
		//путь к установленному приложению
		/*wchar_t lpValueBuf[512] = {0};
		DWORD pcchValueBuf = 512;

		UINT res = MsiGetProductInfoW(L"{E696270D-0C33-4847-AA54-506976D9A09C}", INSTALLPROPERTY_INSTALLLOCATION, lpValueBuf, &pcchValueBuf);*/
		/*ERROR_BAD_CONFIGURATION
		ERROR_INVALID_PARAMETER
		ERROR_MORE_DATA
		ERROR_SUCCESS
		ERROR_UNKNOWN_PRODUCT
		ERROR_UNKNOWN_PROPERTY*/
        


		/*STARTUPINFO si = { sizeof(si) };
		PROCESS_INFORMATION pi;

		if(::CreateProcessW(L"msiexec.exe", L" /x {E696270D-0C33-4847-AA54-506976D9A09C} /quiet", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		{
			WaitForSingleObject(pi.hProcess, INFINITE);
			WinExec("msiexec /i "A_MSI_FILE_NAME, SW_SHOW);
		}
		else
			die_with_wserror(L"msiexec run error");*/

		SHELLEXECUTEINFO ExecuteInfo;
    
		memset(&ExecuteInfo, 0, sizeof(ExecuteInfo));
    
		ExecuteInfo.cbSize       = sizeof(ExecuteInfo);
		ExecuteInfo.fMask        = 0;                
		ExecuteInfo.hwnd         = 0;                
		ExecuteInfo.lpVerb       = L"open";														// Operation to perform
		ExecuteInfo.lpFile       = L"msiexec.exe";												// Application name
		ExecuteInfo.lpParameters = L"/x {E696270D-0C33-4847-AA54-506976D9A09C} /quiet";         // Additional parameters
		ExecuteInfo.lpDirectory  = 0;															// Default directory
		ExecuteInfo.nShow        = SW_SHOW;
		ExecuteInfo.hInstApp     = 0;
    
		if(ShellExecuteEx(&ExecuteInfo))
		{
			WaitForSingleObject(ExecuteInfo.hProcess, INFINITE);
			Sleep(5000);
			WinExec("msiexec /i "A_MSI_FILE_NAME, SW_SHOW);
		}
		else
			die_with_wserror(L"msiexec run error");
		
		//if (MsiInstallProduct(NULL, L"/x {E696270D-0C33-4847-AA54-506976D9A09C} /quiet") == ERROR_SUCCESS)
		//{
		//	//WaitForSingleObject(pi.hProcess, INFINITE);
		//	WinExec("msiexec /i "A_MSI_FILE_NAME, SW_SHOW);
		//}
		//else
		//	die_with_wserror(L"msiexec run error");

		//WinExec("start /wait msiexec.exe /x {E696270D-0C33-4847-AA54-506976D9A09C} /quiet", SW_SHOW);
		//WinExec("msiexec /i "A_MSI_FILE_NAME, SW_SHOW);
		//WinExec("msiexec /fa "A_MSI_FILE_NAME, SW_SHOW);
		//WinExec("msiexec /i {E696270D-0C33-4847-AA54-506976D9A09C} REINSTALL=ALL REINSTALLMODE=omus", SW_SHOW);
	}

	//завершаем лайтинсталлер
	PostMessage(hDlgWnd, WM_QUIT, 0, 0);

	return 0;
}

void die_with_error(TCHAR *errorMessage)
{
	MessageBox(hDlgWnd, errorMessage, L"Error", MB_ICONERROR|MB_TOPMOST);
    exit(1);
}
 
void die_with_wserror(TCHAR *errorMessage)
{
	MessageBoxW(hDlgWnd, errorMessage, L"Error", MB_ICONERROR|MB_TOPMOST);
    exit(1);
}

string CryptString(string str) 
{
	size_t len = str.size();
	string out(str);

	for (size_t i = 0; i < len; i++)
		out.at(i) = str[i] ^ (i+1);

	return out;
}

BOOL Is64BitOS()
{
	BOOL bIs64Bit = FALSE;
 
	#if defined(_WIN64)

	bIs64Bit = TRUE;  // 64-bit programs run only on Win64

	#elif defined(_WIN32)

	// Note that 32-bit programs run on both 32-bit and 64-bit Windows

	typedef BOOL (WINAPI *LPFNISWOW64PROCESS) (HANDLE, PBOOL);
	LPFNISWOW64PROCESS pfnIsWow64Process = (LPFNISWOW64PROCESS)GetProcAddress(GetModuleHandle(_T("kernel32")), "IsWow64Process");
 
	if (pfnIsWow64Process)
		pfnIsWow64Process(GetCurrentProcess(), &bIs64Bit);
 
	#endif

	return bIs64Bit;
}