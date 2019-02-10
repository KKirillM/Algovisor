#include "stdafx.h"
#include "resource.h"
#include "DEFINE.h"
#include "Algovisor.h"
#include "LoginDlg.h"
#include "Auxiliary_methods.h"
#include "Logger.h"


//#define RITHMIC_PROTECT

extern CAAApp theApp;

static std::string header;
size_t curl_header_function(char* ptr, size_t size, size_t nmemb, std::string* data)
{
    header.append(ptr, size*nmemb);
    return size*nmemb;
}

size_t curl_write_function(char* ptr, size_t size, size_t nmemb, std::string* data)
{
	if (data)
	{
		data->append(ptr, size*nmemb);
		return size*nmemb;
	}
	else return 0;
	/*static int cur_buf_index = 0;

	if (userdata == NULL || cur_buf_index + size*nmemb > 2048) 
		return 0;

	memcpy(&userdata[cur_buf_index], ptr, size*nmemb);
	cur_buf_index += size*nmemb;

	return size*nmemb;*/
}

PROTECT_FUNC tstring SFLB_0_SendCurlRequest(std::string& req, std::string& answer)
{
#ifdef DEBUG
	FileLogger logger(".\\log", "server_transaction.txt", std::ios_base::app, LOGGER_PARAM_DATE | LOGGER_PARAM_TIME);
#define DEBUG_LOG(message, ...) PRINT_TO_LOG(logger, LOG_DEBUG, message, __VA_ARGS__)
#else
#define DEBUG_LOG(message, ...)
#endif

	CURL* curl;
	CURLcode res;
	//char errorString[CURL_ERROR_SIZE];

	curl = curl_easy_init();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_HEADER, 0); //1 - отображать заголовок, 0 - нет
		curl_easy_setopt(curl, CURLOPT_URL, "http://algovisor.com/user/sslauth");
		//curl_easy_setopt(curl, CURLOPT_URL, "http://algovisor.com/decryptssl2.php");

		curl_easy_setopt(curl, CURLOPT_PORT, 80);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

		//создаем md5 хэш от клиентского файла ключей
		unsigned char md5[MD5_DIGEST_LENGTH];
		char slistData[130] = {0};

#ifdef TRIAL_VERSION
		
		char client_private_demo_key[] = "-----BEGIN PRIVATE KEY-----\n"
							"MIICdQIBADANBgkqhkiG9w0BAQEFAASCAl8wggJbAgEAAoGBAK/HBbN92KNDGy91\n"
							"AJLHFofUeCgeUS8fk2HWVzqUrs4bpGHHZ7w9F8YaTAyV7FpS5WOFfVWKAMTxVv7p\n"
							"42RMmXxlDqSuMY2Xo8ednkZbkugcBTGYTggu2NM7EZemdxpj1tXQlbRyT3LyAY26\n"
							"/b/CqvRLfUx+lwdMwlR50tPtzdpTAgMBAAECgYB1WmBaB0jdJu49btXlKHnIUXev\n"
							"AR6eYWxaQ2mVOLLD8/x5M1H0gK0gm2nudSGI5BM1sGeO9kWxNpcoNSqVSfal1OoG\n"
							"0g+YqePtnJtIfhwTGjNCPStQeeiNhj2lyy1KyypFrliFsX2t34qqG6OGJKRjCJAZ\n"
							"ANrk/n2fuZNs6nWfQQJBANTFbBQay7JhFGWo7wPUEmMgHEC6KKWSR865+9nKG5qI\n"
							"VWliQVHlG0ypFXw7T070OBxk9whBfNK4fN7pVm71qZECQQDTfX6Z+ywLo5IEehpt\n"
							"Qv/b2JJTjgk797CJYRM8HgLkGPYAojT3dnsYW/t3m+UVLwUIouiwRTHE4SRinqeH\n"
							"azOjAkBaSmkB4xkb2myQc27yXfVrkT3MDfOfx4KnbG8+zbCHFGHCdcqoWSQLlRkj\n"
							"/fs60xJYolPecgPpXBbJdZWTQraxAkAZAZynjkhN9AA+4UUgqCxijT/3+zJiUc4g\n"
							"4EaK9C15VO45cvR6UhsJ7U5VfSxgDdg1hjWnKfdRhSFtO78pRuarAkBy6GKKGuw5\n"
							"PxEbHvdMACzenMCy0sD1fdqtbeWJHHjqhQiYm9l50VDLUbeRt3ADUpaRPoDmO4cb\n"
							"gzbh5QwyOrdx\n"
							"-----END PRIVATE KEY-----\n";

		size_t len = strlen(client_private_demo_key);
		MD5((const unsigned char*)client_private_demo_key, len, md5);

#else
		//переписать этот кусок кода чтобы файл 1 раз открывался и сделать это с помощью fstream
		size_t file_len;
		std::ifstream keyFile("key.cpr", std::ios::binary | std::ios::ate);
		if (!keyFile.is_open())
			return _T("ERROR: file key.cpr open error");
		else
			file_len = keyFile.tellg();

		keyFile.close();
	
		unsigned char* clientKey = new unsigned char[file_len];

		FILE* key;
		if (fopen_s(&key, "key.cpr", "r"))
		{
			delete[] clientKey;
			return _T("ERROR: file key.cpr not found");
		}
		fread(clientKey, 1, file_len, key);
		fclose(key);
		MD5((const unsigned char*)clientKey, file_len, md5);
		delete[] clientKey;

#endif	

		StringCchPrintfA(slistData, 100, "X-cpr: ");
		for (int i = 0; i < MD5_DIGEST_LENGTH; ++i)
			StringCchPrintfA(slistData, 100, "%s%02x", slistData, md5[i]);

		curl_slist *headers=NULL; // init to NULL is important 
		headers=curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded; charset=UTF-8"); 
		headers=curl_slist_append(headers, "charset=UTF-8");  // 2 раза charset=UTF-8 ???
		headers=curl_slist_append(headers, slistData); 

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, req.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, req.length());
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &answer);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, curl_header_function);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_function);
		curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "cookie");
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "cookie");

		//curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorString);
        DEBUG_LOG("REQUEST HEADER: %s%s%s%s", headers->data, headers->next->data, headers->next->next->data, req.c_str());
		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
		{
			std::string error(curl_easy_strerror(res));
			return std::wstring(error.begin(), error.end());
		}
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);

        DEBUG_LOG("ANSWER HEADER: %s", header.c_str())

		const int ret = SFLB_0_decryptRSA(answer);
		
		if (ret == 0)
			return _T("key.cpr error");

		return _T("Please wait...");
	}
	else
	{
		return _T("cURL ERROR");
	}
}

LoginDlg::LoginDlg(CWnd* pParent /*=NULL*/) : CDialogEx(LoginDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIconW(IDR_MAINFRAME);
	//SetBackgroundColor(BGRNCOLOR, false);
	m_logonState = NOSTATUS;
	waitCursor = false;
	
	srand((unsigned int)time(0));
	timerMaxValue = (rand() % 15) + 5;
}

void LoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(LoginDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &LoginDlg::OnBnClickedOk)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

// LoginDlg message handlers

BOOL LoginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	//иконка
 	//HICON m_hIconSmall = (HICON) ::LoadImage(theApp.m_hInstance, MAKEINTRESOURCEW(IDR_MAINFRAME), IMAGE_ICON, 16, 16, 0/*LR_DEFAULTCOLOR*/); // Загрузить иконку 16х16
 	//HICON m_hIconBig = (HICON) ::LoadImage(theApp.m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 32, 32, 0/*LR_DEFAULTCOLOR*/); 	// Загрузить иконку 32x32
	//HICON m_hIconSmall = LoadIcon(hInst, MAKEINTRESOURCE(IDR_MAINFRAME));
	//HICON m_hIconBig = LoadIcon(hInst, MAKEINTRESOURCE(IDR_MAINFRAME));
	//SetIcon(m_hIconBig, TRUE);			// Set big icon
	//SetIcon(m_hIconSmall, FALSE);		// Set small icon

	//номер версии
	TCHAR version[32];
	LoadString(GetModuleHandle(NULL), ALGOVISOR_VERSION, version, 32);
	GetDlgItem(MAIN_WINDOW_VERSION_NUMBER)->SetWindowTextW(version);

	//проверяем наличие ключа в реестре в ID и заносим его значение в с поле редактирования
	GetDlgItem(IDC_EDIT_LOGIN)->SetWindowTextW(SFLB_0_CryptString(theApp.GetProfileString(APPLICATION_NAME, _T("Login"))));
	GetDlgItem(IDC_EDIT_PASSWORD)->SetWindowTextW(SFLB_0_CryptString(theApp.GetProfileString(APPLICATION_NAME, _T("Password"))));
	CheckDlgButton(IDC_CHECK_SAVELOGPASS, theApp.GetProfileInt(APPLICATION_NAME, _T("SaveID"), 0));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void LoginDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}
HCURSOR LoginDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

tstring SendLoginData(CStringA& log, CStringA& pass, computerInfo& hardw, std::string& answer)
{
#ifdef DEBUG
	FileLogger logger(".\\log", "server_transaction.txt", std::ios_base::app, LOGGER_PARAM_DATE | LOGGER_PARAM_TIME);
#define DEBUG_LOG(message, ...) PRINT_TO_LOG(logger, LOG_DEBUG, message, __VA_ARGS__)
#else
#define DEBUG_LOG(message, ...)
#endif

	using namespace rapidjson;

	StringBuffer s;
    Writer<StringBuffer> writer(s);
    
	writer.StartObject();
	writer.String("action");				 writer.String("connect");
	writer.String("login");					 writer.String((char*)log.GetString());
	writer.String("password");				 writer.String(MD5_String((log+"<d209jnk37"+MD5_String(pass).c_str()+"@@#@$m298j").GetString()).c_str());
	//writer.String("password");			 writer.String(MD5_String((log+pass).GetString()).c_str());
	writer.String("user name");				 writer.String(MD5_String(::CW2A(hardw.userName)).c_str());
	writer.String("computer name");			 writer.String(MD5_String(::CW2A(hardw.computerName)).c_str());
	TCHAR version[32];	//версия софта (берем из таблицы в ресурсах)
	LoadString(GetModuleHandle(NULL), ALGOVISOR_VERSION, version, 32);
    writer.String("algovisor version");		 writer.String(::CW2A(version));
    writer.String("disk mask");				 writer.Uint(hardw.discMask);
    writer.String("disk serial number");	 writer.Uint(hardw.driveSerial);
    writer.String("HDD serial number");		 writer.String(MD5_String(::CW2A(hardw.HDD_HardwareSerial)).c_str());
    writer.String("memory size");			 writer.Uint64(hardw.memorySize);
    writer.String("processor architecture"); writer.Uint(hardw.processorArch);
    writer.String("number of cores");		 writer.Uint(hardw.numbersOfProc);
    writer.String("processor revision");	 writer.Uint(hardw.processorRevision);
    writer.String("OS major version");		 writer.Uint(hardw.majorVerOs);
    writer.String("OS minor version");		 writer.Uint(hardw.minorVerOs);
    writer.String("OS build");				 writer.Uint(hardw.bildNumberOs);
    writer.String("machine GUID");			 writer.String(MD5_String(::CW2A(hardw.machineGUID)).c_str());
    writer.String("screen X resolution");	 writer.Uint(hardw.cxScreen);
    writer.String("screen Y resolution");	 writer.Uint(hardw.cyScreen);
	writer.String("mac adresses");
	writer.StartArray();
    for (size_t i = 0; i < hardw.macs.size(); i++)
    {
			writer.String(MD5_String(::CW2A(hardw.macs[i])).c_str());
    }
	writer.EndArray();
	writer.String("bios version");			writer.String(MD5_String(::CW2A(hardw.biosVer)).c_str());
	writer.String("bios date");				writer.String(MD5_String(::CW2A(hardw.biosDate)).c_str());
	writer.String("bios info");				writer.String(MD5_String(::CW2A((wchar_t*)hardw.multiBiosVer)).c_str());
    writer.EndObject();

	//очищаем память строк выделенных для данных о системе
	delete[] hardw.machineGUID;
	delete[] hardw.biosDate;
	delete[] hardw.biosVer;
	delete[] hardw.HDD_HardwareSerial;
	for (size_t i = 0; i < hardw.macs.size(); ++i)
    {
		delete[] hardw.macs[i];
    }

	std::string request("");
    DEBUG_LOG("COMPUTER INFO REQUEST: %.*s", s.GetSize(), s.GetString())
    const wchar_t* errorWStr = SFLB_0_encryptRSA((char*)s.GetString(), request);
	if (!request.compare(""))
    {
		return errorWStr;
    }

    tstring ret = SFLB_0_SendCurlRequest(request, answer);
	DEBUG_LOG("DECRYPT ANSWER: %s", answer.c_str())
	return ret;
}

bool CorrectAndCheckLoginAndPassword(CString& login, CString& password)
{
    // убираем пробелы табы перед и после для строк логина и пароля если они есть
    int nPos;
    while ((nPos = login.Find(' ', 0)) != -1 || 
           (nPos = login.Find('\t', 0)) != -1 ||
           (nPos = login.Find('\n', 0)) != -1 ||
           (nPos = login.Find('\v', 0)) != -1 ||
           (nPos = login.Find('\r', 0)) != -1)
    {
        login.Delete(nPos);
    }

    while ((nPos = password.Find(' ', 0)) != -1 || 
        (nPos = password.Find('\t', 0)) != -1 ||
        (nPos = password.Find('\n', 0)) != -1 ||
        (nPos = password.Find('\v', 0)) != -1 ||
        (nPos = password.Find('\r', 0)) != -1)
    {
        password.Delete(nPos);
    }

    // предварительная проверка логина и пароля перед отправкой на сервер
    if (login.IsEmpty() || password.IsEmpty() || 
        login.GetLength() < 6 || password.GetLength() < 8 || 
        login.Find('@', 0) == -1 || login.Find('.', 0) == -1)
    {
        return false;
    }

    return true;
}

void LoginDlg::OnBnClickedOk()
{
	waitCursor = true;
	BeginWaitCursor();

	//выключаем кнопку
	m_logonState = WAIT;
	EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC)->SetWindowTextW(_T("Please wait..."));
	
	CString wLogin, wPassword;
	GetDlgItem(IDC_EDIT_LOGIN)->GetWindowText(login);
	GetDlgItem(IDC_EDIT_PASSWORD)->GetWindowText(wPassword);

    if (!CorrectAndCheckLoginAndPassword(login, wPassword))
    {
        Sleep(3000); //искусственная задерка чтобы казалось что запрос обрабатывается и в этом случае
        GetDlgItem(IDC_STATIC)->SetWindowTextW(L"Login/Password incorrect");
        EnableWindow(TRUE);
        EndWaitCursor();
        waitCursor = false;
        return;
    }

	wLogin = login;

	//записываем в реестр введенные данные
	if (IsDlgButtonChecked(IDC_CHECK_SAVELOGPASS))
	{
		theApp.WriteProfileString(APPLICATION_NAME, _T("Login"), SFLB_0_CryptString(wLogin));
		theApp.WriteProfileString(APPLICATION_NAME, _T("Password"), SFLB_0_CryptString(wPassword));
		theApp.WriteProfileInt(APPLICATION_NAME, _T("SaveID"), 1);
	}
	else
	{
		theApp.WriteProfileString(APPLICATION_NAME, _T("Login"), _T(""));
		theApp.WriteProfileString(APPLICATION_NAME, _T("Password"), _T(""));
		theApp.WriteProfileInt(APPLICATION_NAME, _T("SaveID"), 0);
	}

#ifndef CONNECT_THROUGH_SERVER
	{
		Sleep(3000);
		EnableWindow(TRUE);
		EndWaitCursor();
		waitCursor = false;
		CDialogOK();
		return;
	}
#endif

	//собираем данные о компе и отправляем запрос серверу
	GetDlgItem(IDC_STATIC)->SetWindowTextW(SendLoginData(CStringA(wLogin), CStringA(wPassword), SFLB_0_GetComputerInfo(), result).c_str());

	if (!result.compare(""))
	{
		EnableWindow(TRUE);
		EndWaitCursor();
		waitCursor = false;
		return;
	}

#ifdef RITHMIC_PROTECT
	SYSTEMTIME st;
	::GetLocalTime(&st);
	CString rProtect = theApp.GetProfileStringW(APPLICATION_NAME, _T("R_PROTECT"));

	if (st.wYear == 2015 && rProtect == L"")
	{
		result.append("Login/Password incorrect");
		theApp.WriteProfileStringW(APPLICATION_NAME, _T("R_PROTECT"), L"-");
	}
	else if (st.wYear == 2015 || rProtect == L"-")
	{
		result.append("Login/Password incorrect");
	}
#endif

	result = SFLB_0_CryptString(result);
	SetTimer(1, 100, NULL);
}

HBRUSH LoginDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	/*if (pWnd->GetDlgCtrlID() == IDC_STATIC && m_logonState == WAIT) {
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(255, 0, 0));
	}*/

	return hbr;
}

void LoginDlg::OnTimer(UINT_PTR nIDEvent)
{
	SFLB_0_PushOk(this, timerMaxValue, result);

	/*static PsProtUInt32_1 timerCounter = 0;

	if (timerCounter++ > timerMaxValue)
	{
		if (!result.compare(SFLB_1_CryptString(std::string("ERROR10407"))))
		{	
			EnableWindow(1);
			EndWaitCursor();
			SetWaitCursor(FALSE);
			KillTimer(1);

			CDialogEx::OnOK();
		}

		result.clear();

		GetDlgItem(IDC_STATIC)->SetWindowTextW(L"Login/Password incorrect");
		
		timerCounter = 0;
		
		KillTimer(1);

		EnableWindow(1);
		EndWaitCursor();
		SetWaitCursor(FALSE);
	}*/	

	CDialogEx::OnTimer(nIDEvent);
}

BOOL LoginDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (waitCursor)
	{
		RestoreWaitCursor();
		return TRUE;
	}

	return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}
