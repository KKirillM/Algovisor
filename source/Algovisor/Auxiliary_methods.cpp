#include "stdafx.h"

#include "DEFINE.h"
#include "Algovisor.h"
#include "Instrument.h"
#include "MainWindow.h"
#include "LoginDlg.h"
#include "NinjaDialog.h"
#include "BaseChart.h"
#include "Quant_Chart.h"
#include "HardDriveSerialNumer.h"
#include "Plaza2Instruments.h"

#include "Auxiliary_methods.h"

#define BUFSIZE	512	//размер принимающиего буфера для пайп-сервера

extern CAAApp theApp;
DWORD iNumberOfProcessors = 1;		//количество процессоров(ядер) в системе
CString login;						//имя пользователя (глобальное потому что для крэшрепорта нужно)

std::list<HANDLE> hPipeServer, hPipeThreads;		//хэндлы для пайп-сервера для связи с нинзей и мьютекса для синхронизации
HANDLE hGlobalMutex = INVALID_HANDLE_VALUE; 
SRWLOCK srwLockNinja;				//лок для сообщений от нинзи
SRWLOCK SRWLockRithmic;				//лок для ритмика
 
//void StatusBar::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
//{
//    switch(lpDrawItemStruct->itemID)
//    {
//    case 1:
//
//		/*HDC memDC = ::CreateCompatibleDC(lpDrawItemStruct->hDC);
//		HBITMAP memBitmap = ::CreateCompatibleBitmap(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom);
//		::SelectObject(memDC, memBitmap);
//         
//		RECT rt = { 0,0,200,200 };
//		DrawRithmicCopyright(memDC, rt);
//
//		::BitBlt(lpDrawItemStruct->hDC, 0, 0, lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom, memDC, 0, 0, SRCCOPY);
//
//		::DeleteObject(memBitmap);
//		::DeleteDC(memDC);*/
//			
//
//		// Attach to a CDC object
//        CDC dc;
//        dc.Attach(lpDrawItemStruct->hDC);
//
//        // Get the pane rectangle and calculate text coordinates
//        CRect rect(&lpDrawItemStruct->rcItem);
//
//		HICON rithmicIcon = (HICON)LoadImageW(AfxGetInstanceHandle(), MAKEINTRESOURCE(/*IDI_RITHMIC_LICENSE_ICON_2*/IDI_RITHMIC_TRADING_PLATFORM), IMAGE_ICON, /*134*/161, /*28*/15, 0);
//		HICON omneIcond = (HICON)LoadImageW(AfxGetInstanceHandle(), MAKEINTRESOURCE(/*IDI_RITHMIC_LICENSE_ICON_3*/IDI_RITHMIC_OMNE), IMAGE_ICON, 122, /*28*/15, 0);
//
//		CBitmap btmp; 
//		btmp.LoadBitmapW(IDB_RITHMIC_TRADING_PLATFORM);        
//        CDC srcDC; // select current bitmap into a compatible CDC
//        srcDC.CreateCompatibleDC(NULL);
//        CBitmap* pOldBitmap = srcDC.SelectObject(&btmp);
//
//		//DrawRithmicCopyright(srcDC, rect);
//
//        dc.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &srcDC, 0, 0, SRCCOPY); // BitBlt to pane rect
//		
//		srcDC.SelectObject(pOldBitmap);
//
//        // Detach from the CDC object, otherwise the hDC will be
//        // destroyed when the CDC object goes out of scope
//        dc.Detach();
//
//        return;
//    }
//
//    CStatusBar::DrawItem(lpDrawItemStruct);
//}

const char HEX2DEC[256] = 
{
    /*       0  1  2  3   4  5  6  7   8  9  A  B   C  D  E  F */
    /* 0 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 1 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 2 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 3 */  0, 1, 2, 3,  4, 5, 6, 7,  8, 9,-1,-1, -1,-1,-1,-1,
    
    /* 4 */ -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 5 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 6 */ -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 7 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    
    /* 8 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 9 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* A */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* B */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    
    /* C */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* D */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* E */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* F */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1
};

// Only alphanum is safe.
const char SAFE[256] =
{
    /*      0 1 2 3  4 5 6 7  8 9 A B  C D E F */
    /* 0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 1 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 2 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 3 */ 1,1,1,1, 1,1,1,1, 1,1,0,0, 0,0,0,0,
    
    /* 4 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
    /* 5 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
    /* 6 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
    /* 7 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
    
    /* 8 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 9 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* A */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* B */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    
    /* C */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* D */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* E */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* F */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};
    
std::string UrlDecode(const std::string & sSrc)
{
    // Note from RFC1630:  "Sequences which start with a percent sign
    // but are not followed by two hexadecimal characters (0-9, A-F) are reserved
    // for future extension"
    
    const unsigned char * pSrc = (const unsigned char *)sSrc.c_str();
	const size_t SRC_LEN = sSrc.length();
    const unsigned char * const SRC_END = pSrc + SRC_LEN;
    const unsigned char * const SRC_LAST_DEC = SRC_END - 2;   // last decodable '%' 

    char * const pStart = new char[SRC_LEN];
    char * pEnd = pStart;

    while (pSrc < SRC_LAST_DEC)
	{
		if (*pSrc == '%')
        {
            char dec1, dec2;
            if (-1 != (dec1 = HEX2DEC[*(pSrc + 1)])
                && -1 != (dec2 = HEX2DEC[*(pSrc + 2)]))
            {
                *pEnd++ = (dec1 << 4) + dec2;
                pSrc += 3;
                continue;
            }
        }

        *pEnd++ = *pSrc++;
	}

    // the last 2- chars
    while (pSrc < SRC_END)
        *pEnd++ = *pSrc++;

    std::string sResult(pStart, pEnd);
    delete [] pStart;
	return sResult;
}
std::string UrlEncode(const std::string & sSrc)
{
    const char DEC2HEX[16 + 1] = "0123456789ABCDEF";
    const unsigned char * pSrc = (const unsigned char*)sSrc.c_str();
    const size_t SRC_LEN = sSrc.length();
    unsigned char * const pStart = new unsigned char[SRC_LEN * 3];
    unsigned char * pEnd = pStart;
    const unsigned char * const SRC_END = pSrc + SRC_LEN;

    for (; pSrc < SRC_END; ++pSrc)
	{
		if (SAFE[*pSrc]) 
            *pEnd++ = *pSrc;
        else
        {
            // escape this char
            *pEnd++ = '%';
            *pEnd++ = DEC2HEX[*pSrc >> 4];
            *pEnd++ = DEC2HEX[*pSrc & 0x0F];
        }
	}

    std::string sResult((char *)pStart, (char *)pEnd);
    delete [] pStart;
    return sResult;
}

//вспомогательная функция для определения серийного номера HDD
wchar_t* GetWText(std::vector<char> &v)
{
	size_t w_num = v.size() + 1;
	wchar_t* w_str = new wchar_t[w_num];	w_str[w_num - 1] = 0;
	MultiByteToWideChar(CP_UTF8, 0, (char *) &v[0], (int)v.size(), w_str, (int)w_num);
	return w_str;
}

void GetHDDSerial(wchar_t** str)
{
	// Get Hard Drive Serial Number
	std::vector<char> hwId;
	MasterHardDiskSerial diskSerial;
    diskSerial.GetSerialNo(hwId);
   
	if (hwId.empty())
    {
		return;
        //throw std::runtime_error("Can't retrieve hardware serial number");
    }

	*str = GetWText(hwId);
}

/*PROTECT_FUNC*/ computerInfo SFLB_0_GetComputerInfo()
{
	computerInfo data;

	//имя пользователя
	DWORD size = (DWORD)sizeof(data.userName);
	::GetUserName(data.userName, &size);

	//имя компьютера
	DWORD sizeC = (DWORD)sizeof(data.userName);
	::GetComputerName(data.computerName, &sizeC);
		
	//имена присутствующих дисков в системе
	data.discMask = GetLogicalDrives();
	/*char dd[4];
	for( int i = 0; i < 26; i++ )
		if( ((data.discMask>>i)&0x00000001) == 1 ) dd[0] = char(65+i); dd[1] = ':'; dd[2] = '\\'; dd[3] = 0;		*/	

	//серийный номер диска с которого запущена программа
	wchar_t VolumeNameBuffer[64];
	wchar_t FileSystemNameBuffer[64];
	DWORD sz,fs;
	
	::GetVolumeInformation(NULL, VolumeNameBuffer, 64, &data.driveSerial, &sz, &fs, FileSystemNameBuffer, 100);

	//ID жесткого диска
	data.HDD_HardwareSerial = NULL;
	GetHDDSerial(&data.HDD_HardwareSerial);

	//размер оперативной памяти
	MEMORYSTATUS stat;

	::GlobalMemoryStatus (&stat);
	data.memorySize = stat.dwTotalPhys;

	//информация о процессоре
	SYSTEM_INFO sysInfo;
 
	::GetSystemInfo(&sysInfo);
	data.processorArch = sysInfo.dwOemId;
	data.numbersOfProc = sysInfo.dwNumberOfProcessors;
	data.processorRevision = sysInfo.wProcessorRevision;

	//версия ОС
	OSVERSIONINFO osvi;

    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    ::GetVersionEx(&osvi);
	data.majorVerOs = osvi.dwMajorVersion;
	data.minorVerOs = osvi.dwMinorVersion;
	data.bildNumberOs = osvi.dwBuildNumber;

	//guid компьютера
	HKEY guid;
	DWORD sizeR = 64;
	data.machineGUID = new wchar_t[sizeR];
	::RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Cryptography", NULL, KEY_READ|KEY_WOW64_64KEY, &guid);
	while (::RegQueryValueExW(guid, L"MachineGuid", 0, 0, (PBYTE)data.machineGUID, &sizeR) == ERROR_MORE_DATA)
	{
		delete[] data.machineGUID;
		sizeR += 64;
		data.machineGUID = new wchar_t[sizeR];
	}
	
	//мультистрока версии и даты биоса
	sizeR = 1;
	DWORD type = REG_MULTI_SZ;
	::RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System", NULL, KEY_READ|KEY_WOW64_64KEY, &guid);
	data.multiBiosVer = new BYTE[sizeR]; data.multiBiosVer[0] = 0;
	while (::RegQueryValueExW(guid, L"SystemBiosVersion", 0, &type, (PBYTE)data.multiBiosVer, &sizeR) == ERROR_MORE_DATA)
	{
		delete[] data.multiBiosVer;
		data.multiBiosVer = new BYTE[sizeR]; data.multiBiosVer[0] = 0;
	}
	//делаем это в одну строку (вместо переводов строк ставим пробелы)
	wchar_t* s = (wchar_t*)data.multiBiosVer;
	for (DWORD i = 0; i < sizeR; ++i)
	{
		if (s[i] == 0 && s[i+1] != 0)
			s[i] = ' ';
		else if (s[i] == 0 && s[i+1] == 0)
			break;
	}

	//версия и дата биоса раздельные
	sizeR = 64;
	data.biosVer = new wchar_t[sizeR]; data.biosVer[0] = 0;
	while (::RegQueryValueExW(guid, L"SystemBiosVersion", 0, 0, (PBYTE)data.biosVer, &sizeR) == ERROR_MORE_DATA)
	{
		delete[] data.biosVer;
		sizeR += 64;
		data.biosVer = new wchar_t[sizeR]; data.biosVer[0] = 0;
	}	
	::RegCloseKey(guid);

	::RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\BIOS", NULL, KEY_READ|KEY_WOW64_64KEY, &guid);
	sizeR = 64;
	data.biosDate = new wchar_t[sizeR]; data.biosDate[0] = 0;
	while (::RegQueryValueExW(guid, L"BIOSReleaseDate", 0, 0, (PBYTE)data.biosDate, &sizeR) == ERROR_MORE_DATA)
	{
		delete[] data.biosDate;
		sizeR += 64;
		data.biosDate = new wchar_t[sizeR]; data.biosDate[0] = 0;
	}
	::RegCloseKey(guid);

	//разрешение экрана
	data.cxScreen = ::GetSystemMetrics(SM_CXSCREEN);
	data.cyScreen = ::GetSystemMetrics(SM_CYSCREEN);

	//mac-адрес
	IP_ADAPTER_INFO* AdapterInfo = (IP_ADAPTER_INFO*)new BYTE[sizeof(IP_ADAPTER_INFO)];
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	PIP_ADAPTER_INFO pNext;

	if(ERROR_BUFFER_OVERFLOW == GetAdaptersInfo(AdapterInfo, &ulOutBufLen)) 
	{
		delete[] AdapterInfo;
		AdapterInfo = (IP_ADAPTER_INFO*)new BYTE[ulOutBufLen];
	}

    if (!GetAdaptersInfo(AdapterInfo, &ulOutBufLen))
	{
		pNext = AdapterInfo;
		
		do {
			data.macs.push_back(new wchar_t[100]);
			StringCchPrintf(data.macs[data.macs.size()-1], 100, L"%02x-%02x-%02x-%02x-%02x-%02x", 
				pNext->Address[0], pNext->Address[1], pNext->Address[2], pNext->Address[3], pNext->Address[4], pNext->Address[5]);
			pNext = pNext->Next;
		} while (pNext);		
	}
	delete[] AdapterInfo;

	return data;
}

/*PROTECT_FUNC*/ CString SFLB_0_CryptString(const CString& _str) 
{
	CString str = _str;
	if (!str.IsEmpty()) 
	{
		for (int i = 0, len = str.GetLength(); i < len; i++)
        {
            unsigned char c1 = str[i];
            unsigned char c2 = (i+1) & 0xFF;
			str.SetAt(i, c1 ^ c2);
        }
	}

	return str;
}

/*PROTECT_FUNC*/ std::string SFLB_0_CryptString(const std::string& _str)
{
	std::string str = _str;
	if (!str.empty())
	{
		for (size_t i = 0, len = str.length(); i < len; i++)
        {
		    unsigned char c1 = str[i];
            unsigned char c2 = (i+1) & 0xFF;
            str[i] = c1 ^ c2;
        }
	}

	return str;
}

/*PROTECT_FUNC*/ tstring& SFLB_0_CryptStringW(tstring& str)
{
	if (!str.empty())
	{
		for (size_t i = 0, len = str.length(); i < len; i++)
        {
            unsigned char c1 = str[i];
            unsigned char c2 = (i+1) & 0xFF;
			str[i] = c1 ^ c2;
        }
	}

	return str;
}

//шифрование сырой строки
PROTECT_FUNC wchar_t* SFLB_0_encryptRSA(char* sourceString, std::string& encryptString)
{
	//считываем открытый ключ
	RSA* rsa = NULL;
	BIO *bufio = NULL;
	
	/*FILE* openKey;

	if (fopen_s(&openKey, "server_public.spu", "r"))
		return _T("ERROR: file server_public.spu not found");
	
	if (!(rsa = PEM_read_RSA_PUBKEY(openKey, 0, 0, 0))) 
	{
		
		fclose(openKey);
		return _T("ERROR: bad format of server_public.spu");
	}
	
	fclose(openKey);*/

	char server_public_key[] = "-----BEGIN PUBLIC KEY-----\n"
							"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDDiD3rjs3+/apJ/FgQyvsbqing\n"
							"J/6tQHuvzyqur08e5Z9IkcbcK97p235LfP/URxdgp8iltp96uAWs8meJq3pEXeyw\n"
							"RmKdIy7PllFF9wiPUsMgkKLBji5Li8frvU6R2/yPUL7KLP6ayw6egnUB+BUzvQCL\n"
							"+lx60pcR3Xc+TC6pkQIDAQAB\n"
							"-----END PUBLIC KEY-----";

	bufio = BIO_new_mem_buf((void*)server_public_key, -1/*(int)strlen(open_key)*/);
	rsa = PEM_read_bio_RSA_PUBKEY(bufio, 0, 0, 0);
	
	size_t keySize = RSA_size(rsa);
	size_t buffSize = strlen(sourceString);
	size_t cryptSize;
	char* encryptBuff = NULL;	

	//непосредственно шифрование буфера
	if (keySize-11 >= buffSize)
	{
		cryptSize = keySize;
		encryptBuff = new char[cryptSize];		
		RSA_public_encrypt((int)buffSize/*keySize-11*/, (unsigned char*)sourceString, (unsigned char*)encryptBuff, rsa, /*RSA_NO_PADDING*/RSA_PKCS1_PADDING);
	}
	else
	{
		size_t source_iterator = 0, dest_iterator = 0;
		size_t rowSize = keySize-11;
		cryptSize = buffSize/rowSize * keySize + keySize;
		
		encryptBuff = new char[cryptSize];

		while (source_iterator < buffSize)
		{
			RSA_public_encrypt(rowSize, (unsigned char*)(sourceString+source_iterator), (unsigned char*)(encryptBuff+dest_iterator), rsa, /*RSA_NO_PADDING*/RSA_PKCS1_PADDING); //здесь при keySize-11 кодируется все равно кол-во символов равное keySize
			source_iterator += rowSize;
			dest_iterator += keySize;
			if ((buffSize - source_iterator) < rowSize)
				rowSize = buffSize - source_iterator;
		}
	}

	BIO_free_all(bufio);
	RSA_free(rsa);

	//расшифровка
	//FILE* privKey;
	//char* decryptBuff;

	//if (fopen_s(&privKey, "server_private.spr", "r"))
	//{
	//	fclose(privKey);
	//}

	//rsa = PEM_read_RSAPrivateKey(privKey, 0, 0, 0); //для PKCS#1-формата
	//
	//fclose(privKey);

	//keySize = RSA_size(rsa);	

	//непосредственно дешифрование буфера
	//if (keySize-11 >= buffSize)
	//{
	//	decryptBuff = new char[keySize];
	//	RSA_private_decrypt(keySize, (unsigned char*)encryptBuff, (unsigned char*)decryptBuff, rsa, /*RSA_NO_PADDING*/RSA_PKCS1_PADDING);
	//}
	//else
	//{
	//	int source_iterator = 0,
	//		dest_iterator = 0;
	//	int rowSize = keySize;
	//	int size = buffSize/rowSize * keySize + keySize;
	//	
	//	decryptBuff = new char[size];
	//	
	//	while (source_iterator < buffSize)
	//	{
	//		RSA_private_decrypt(rowSize, (unsigned char*)(encryptBuff+source_iterator), (unsigned char*)(decryptBuff+dest_iterator), rsa, /*RSA_NO_PADDING*/RSA_PKCS1_PADDING); //здесь при keySize-11 кодируется все равно кол-во символов равное keySize
	//		source_iterator += keySize;
	//		dest_iterator += keySize-11;
	//		/*if ((buffSize - iterator) < rowSize)
	//			rowSize = buffSize - iterator;*/
	//	}
	//}
	//FILE* RSA_str;
	//RSA_str = fopen("RSA_string.txt", "w");
	//setvbuf(RSA_str, NULL, _IONBF, 0);		
	//fwrite(decryptBuff, sizeof(char), buffSize, RSA_str);
	//fclose(RSA_str);

	//делаем base64_encode для шифрованных данных
	char* base64encode;
	int base64Size = Base64EncodeGetRequiredLength((int)cryptSize); 
	base64encode = new char[base64Size];
	Base64Encode((BYTE*)encryptBuff, (int)cryptSize, base64encode, &base64Size, ATL_BASE64_FLAG_NOCRLF);
	delete[] encryptBuff;

	std::string encodeCryptBuffer(base64encode, base64Size);
	encodeCryptBuffer = UrlEncode(encodeCryptBuffer);
	delete[] base64encode;

	//собираем строку запроса + данные
	encryptString.append("data=");
	encryptString += encodeCryptBuffer;

	return _T("request to server ready");
}
//расшифровка зашифрованной строки
PROTECT_FUNC const int SFLB_0_decryptRSA(std::string& answ)
{
	RSA* rsa = NULL;
	BIO* bufio = NULL;

	//получаем ключ для дешифрации
#ifndef TRIAL_VERSION

	FILE* privKey;
		
	//открываем файл ключа
	if (fopen_s(&privKey, "key.cpr", "r"))
		return 0;

	rsa = PEM_read_RSAPrivateKey(privKey, 0, 0, 0); //для PKCS#1-формата

	//проверяем на валидность файл ключа
	if (!rsa)
		return 0;
	
	fclose(privKey);

#else
	
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
						"-----END PRIVATE KEY-----";

	bufio = BIO_new_mem_buf((void*)client_private_demo_key, -1/*(int)strlen(open_key)*/);
	rsa = PEM_read_bio_RSAPrivateKey(bufio, 0, 0, 0);
	
#endif

	char* decryptBuff = NULL;
	size_t keySize = RSA_size(rsa);
	int decryptChars = 0;
		
	if (keySize >= answ.size())
	{
		decryptBuff = new char[keySize];
		decryptChars += RSA_private_decrypt(keySize, (unsigned char*)answ.data(), (unsigned char*)decryptBuff, rsa, /*RSA_NO_PADDING*/RSA_PKCS1_PADDING);
	}
	else
	{
		size_t source_iterator = 0, dest_iterator = 0;
		size_t rowSize = keySize;
		int size = answ.size()/rowSize * keySize + keySize;
			
		decryptBuff = new char[size];
			
		while (source_iterator < answ.size())
		{
			decryptChars += RSA_private_decrypt(rowSize, (unsigned char*)(answ.c_str()+source_iterator), (unsigned char*)(decryptBuff+dest_iterator), rsa, /*RSA_NO_PADDING*/RSA_PKCS1_PADDING); //здесь при keySize-11 кодируется все равно кол-во символов равное keySize
			source_iterator += keySize;
			dest_iterator += keySize-11;
			/*if ((buffSize - iterator) < rowSize)
				rowSize = buffSize - iterator;*/
		}
	}

	RSA_free(rsa);
	BIO_free_all(bufio);

	answ.resize(0);
	answ.append(decryptBuff);

	delete[] decryptBuff;
	
	return decryptChars;

	//расшифрованный ответ от сервера в файл
	/*FILE* RSA_str;
	RSA_str = fopen("RSA_string.txt", "w");
	setvbuf(RSA_str, NULL, _IONBF, 0);		
	fwrite(decryptBuff, sizeof(char), strlen(decryptBuff), RSA_str);
	fclose(RSA_str);*/

	//зашифрованный ответ от сервера в файл
	/*FILE* srv_answ;
	srv_answ = fopen("server_answer.txt", "w");
	setvbuf(srv_answ, NULL, _IONBF, 0);		
	fwrite(answ.c_str(), sizeof(char), answ.length(), srv_answ);
	fclose(srv_answ);*/
}

/*PROTECT_FUNC*/ VOID SFLB_0_PushOk(LoginDlg* dlg, PsProtUInt32_1 i, std::string& result)
{
	static PsProtUInt32_1 timerCounter = 0;

	//здесь извращения с исключениями нужны чтобы компилятор создал более сложный код, для защиты этих проверок
	try {
		throw i;
	}
	catch(PsProtUInt32_1)
	{
		if (timerCounter++ > i)
		{
			if (!result.compare(SFLB_0_CryptString(std::string("ERROR10407")))) //проверка на верность ключа
			{	
				//запишем в реестр метку времени о последней проверке лицензии
				theApp.WriteProfileInt(APPLICATION_NAME, _T("License_check_time"), time(0));

				dlg->EnableWindow(1);
				dlg->EndWaitCursor();
				dlg->SetWaitCursor(FALSE);
				dlg->KillTimer(1);
				dlg->CDialogOK();
			}
			else if (!result.compare(SFLB_0_CryptString(std::string("ERROR2893273"))))	//проверка на верность номера версии
			{
				dlg->KillTimer(1);
				MessageBox(NULL, L"You use old version of software.\n Please download the latest version \nof Algovisor from www.algovisor.com", L"Algovisor old version", MB_ICONEXCLAMATION);
				SetForegroundWindow(dlg->m_hWnd);
				dlg->GetDlgItem(IDC_STATIC)->SetWindowTextW(L"Old version");
				dlg->EnableWindow(1);
				dlg->EndWaitCursor();
				dlg->SetWaitCursor(FALSE);
				result.clear();
			}
			else
			{
				result.clear();

				dlg->GetDlgItem(IDC_STATIC)->SetWindowTextW(L"Login/Password incorrect");
		
				timerCounter = 0;
		
				dlg->KillTimer(1);

				dlg->EnableWindow(1);
				dlg->EndWaitCursor();
				dlg->SetWaitCursor(FALSE);
			}
		}
	}
}

std::string MD5_String(const char* str)
{
	ASSERT(str);

	unsigned char md5[MD5_DIGEST_LENGTH];
	char result[MD5_DIGEST_LENGTH*2+1] = {0};
	
	if (str)
	{
		MD5((const unsigned char*)str, strlen(str), md5);

		for (int i = 0; i < MD5_DIGEST_LENGTH; ++i)
			StringCchPrintfA(result, 100, "%s%02x", result, md5[i]);
	}

	return std::string(result);
}

void Disconnect_Algovisor_Server()
{
	using namespace rapidjson;

	StringBuffer s;
	Writer<StringBuffer> writer(s);
    
	writer.StartObject();
	writer.String("action");				writer.String("disconnect");
	writer.EndObject();

	std::string request(""), answer("");
	/*const wchar_t* errorWStr =*/ SFLB_0_encryptRSA((char*)s.GetString(), request);
	if (!request.compare(""))
		return;

	SFLB_0_SendCurlRequest(request, answer);
}

int CheckProcessInMemory(const wchar_t* process_name)
{
	HANDLE PHandle;
	PROCESSENTRY32 ProcEntry;
	int result = 0;
 
	//проверяем запущен ли процесс NinjaTrader.exe в системе
	PHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	ProcEntry.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(PHandle, &ProcEntry))
	{
		if (!wcscmp(ProcEntry.szExeFile, process_name))
			++result;
	}
	while(Process32Next(PHandle, &ProcEntry))
	{
		if (!wcscmp(ProcEntry.szExeFile, process_name))
			++result;
	}
	
	return result;
}

//записываем из вектора _i_old в вектор _i только трейды
void GetTradesOnlyInVector(IInstrument*& source, IInstrument*& dest, size_t& dest_index)
{
	static int counter = 0;
	BYTE hour = 99, minute, second;

	size_t bookSize = source->GetAggregatedOrderbookSize();
	if (dest_index > bookSize)
	{
		dest_index = 0;
		delete dest;
		dest = reinterpret_cast<IInstrument*>(source->create_object());
		dest->CopyAllParameters(source);
	}

	while (dest_index < bookSize)
	{				
		if (source->Get_tradeSize(dest_index)) 	
		{
			//записываем трейд
			dest->push_back(source->get_data(dest_index));
			//обнуляем у него маркер оси
			size_t lastIndx = dest->get_size()-1;
			dest->Set_yAxis(lastIndx, false);
			if (++counter >= 100 && (hour != dest->Get_hour(lastIndx) || minute != dest->Get_minute(lastIndx) || second != dest->Get_second(lastIndx)))
			{		
				hour = dest->Get_hour(lastIndx);
				minute = dest->Get_minute(lastIndx);
				second = dest->Get_second(lastIndx);
		
				dest->Set_yAxis(lastIndx, true);
				counter = 0;
			}
		}

		++dest_index;
	}
}

//если от инструмента за которым было закреплено окно отписались рисуем уведомление в центре
void DrawUnsubscribeString(HDC dc, const RECT& rt, const FeedType ft, const char* name)
{
	//черный фон
	FillRect(dc, &rt, (HBRUSH)GetStockObject(BLACK_BRUSH));

	//выводим текст
	::SetTextColor(dc, RGB(0,0,0));		//черный текст
	::SetBkColor(dc, RGB(255,255,0));	//желтый фон текста
	::SetBkMode(dc, OPAQUE);
	::SetTextAlign(dc, TA_NOUPDATECP | TA_CENTER | TA_BASELINE);
	::SelectObject(dc, (HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	
	TCHAR ticker[64];
	
	switch (ft)
	{
	case RITHMIC:		StringCchPrintf(ticker, 64, L" Waiting for subscribe instrument: Rithmic:%S ", name); break;
	case CQG:			StringCchPrintf(ticker, 64, L" Waiting for subscribe instrument: CQG:%S ", name); break;
	case NINJATRADER:	StringCchPrintf(ticker, 64, L" Waiting for subscribe instrument: NinjaTrader:%S ", name); break;
    default:            StringCchPrintf(ticker, 64, L" Waiting for subscribe instrument: %S ", name); break;
	}

	::TextOut(dc, rt.right/2, rt.bottom/2, ticker, (int)_tcslen(ticker));
}

//рисование кнопки с заданным текстом и в заданных координатах
void DrawButton(HDC dc, RECT& rt, const RECT& rt2, const RECT& rt3, const wchar_t* name, const COLORREF color1, const COLORREF color2)
{
	SIZE sSize;
	::GetTextExtentPoint32W(dc, name, 1, &sSize);

	int xCoord = rt2.right - sSize.cx/2 - 1,
		yCoord = rt2.bottom;		
	rt.left = rt3.right - sSize.cx;
	rt.top = rt3.bottom - sSize.cy;
	rt.right = rt3.right;
	rt.bottom = rt3.bottom;

	::SetDCBrushColor(dc, RGB(255,128,64));
	::SetDCPenColor(dc, RGB(255,128,64));
	::Rectangle(dc, xCoord - sSize.cx/2 -3, yCoord - sSize.cy, xCoord + sSize.cx/2 +2, yCoord);
	::SetTextColor(dc, color1);
	::TextOut(dc, xCoord, yCoord, name, 1);
	::SetDCPenColor(dc, color2);
}

void DrawHistoryLoad(HDC dc, const RECT& rt)
{
	BITMAPINFO bi;
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = rt.right;
	bi.bmiHeader.biHeight = rt.bottom;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = 0;
	bi.bmiHeader.biXPelsPerMeter = 0;
	bi.bmiHeader.biYPelsPerMeter = 0;
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;
	void* pixelsBuf;

	HBITMAP hBuf = CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, &pixelsBuf, NULL, 0);
	HDC hBufDC = CreateCompatibleDC(dc);

	SelectObject(hBufDC, hBuf);
	FillRect(hBufDC, &rt, (HBRUSH)GetStockObject(BLACK_BRUSH));

	//рисуем содержимое буфера в указанный DC, со смешиванием (blending) цветов
	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 200, 0/*AC_SRC_ALPHA*/};
	AlphaBlend(dc, 0, 0, rt.right, rt.bottom, hBufDC, 0, 0, rt.right, rt.bottom, bf);

	//выводим текст
	::SetTextColor(dc, RGB(0,0,0));	//черный текст
	::SetBkColor(dc, RGB(255,255,0));	//желтый фон
	::SetBkMode(dc, OPAQUE);
	::SetTextAlign(dc, TA_NOUPDATECP | TA_CENTER | TA_BASELINE);
	::SelectObject(dc, (HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	::TextOut(dc, rt.right/2, rt.bottom/2, L" Downloading historical data... ", 32);

	DeleteObject(hBuf);
	DeleteDC(hBufDC);
}
void DrawCopyright(HDC dc, const RECT& rt)
{
	TCHAR* str = L"\u00A9 Algovisor, 2015";

	//шрифт
	CFont font;
	font.CreateFont(14, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, VARIABLE_PITCH|FF_SWISS, L"Arial");

	::SetTextColor(dc, RGB(0,0,0));			//черный текст
	::SetBkColor(dc, RGB(192,192,192));		//серый фон
	::SetBkMode(dc, OPAQUE);
	::SetTextAlign(dc, TA_NOUPDATECP | TA_LEFT | TA_BASELINE);
	::SelectObject(dc, (HFONT)font/*::GetStockObject(SYSTEM_FONT)*/);
	::TextOutW(dc, rt.left + 10, rt.bottom - 35, str, (int)wcslen(str));
}

//logo for rithmic and omne	
void DrawRithmicCopyright(HDC dc, const RECT& rt)
{
	HBRUSH grey_brush = CreateSolidBrush(RGB(192,192,192));
	HICON rithmicIcon = (HICON)LoadImageW(AfxGetInstanceHandle(), MAKEINTRESOURCE(/*IDI_RITHMIC_LICENSE_ICON_2*/IDI_RITHMIC_TRADING_PLATFORM), IMAGE_ICON, /*134*/161, /*28*/15, 0);
	HICON omneIcond = (HICON)LoadImageW(AfxGetInstanceHandle(), MAKEINTRESOURCE(/*IDI_RITHMIC_LICENSE_ICON_3*/IDI_RITHMIC_OMNE), IMAGE_ICON, 122, /*28*/15, 0);

	::DrawIconEx(dc, rt.left /*+ 105*/, rt.bottom /*- 60*/, rithmicIcon, 0, 0, 0, grey_brush, DI_NORMAL);
	::DrawIconEx(dc, rt.left /*+ 250*/, rt.bottom /*- 60*/, omneIcond, 0, 0, 0, grey_brush, DI_NORMAL);

	DestroyIcon(rithmicIcon);
	DestroyIcon(omneIcond);
	DeleteObject(grey_brush);
}

UINT GetRangeIndexes(HWND hwn, IInstrument* _i, size_t& start, size_t& end, RECT& rt)
{
	SCROLLINFO info = { sizeof(SCROLLINFO), SIF_POS, 0, 0, 0, 0, 0 };
	::GetScrollInfo(hwn, SB_HORZ, &info);
	
	start = info.nPos;
	
	size_t size = _i->GetAggregatedOrderbookSize();
	
	if (size)
	{
		if ((start + rt.right-1) < size)
			end = start + (rt.right-1);
		else 
		{
			end = size-1;
			if (start)
			{
				int _start = (int)end - rt.right;
				if (_start < 0)
					start = 0;
				else
					start = (size_t)_start;				
			}
		}	
	}
	else return 1;

	return 0;
}



void BaseChartData::SetData(BaseChart* base, const size_t _start, const size_t _end, std::vector<std::vector<std::pair<size_t,size_t>>>* pTimeLabelsVector, 
				 std::vector<Price>* maxPoolPrice,std::vector<Price>* minPoolPrice,std::vector<Price>* lastBidPoolPrice,std::vector<Price>* lastAskPoolPrice, volatile LONG* numOfPulThread)
{
	numberOfPoolThread = numOfPulThread;
	_i =	base->instrumentPtr;
	start =	_start;
	end =	_end;
	scale_type = base->scale_type;
	maxPrice =	  maxPoolPrice;
	minPrice =	  minPoolPrice;
	lastBidPrice = lastBidPoolPrice;
	lastAskPrice = lastAskPoolPrice;
	timeLabelsVector = pTimeLabelsVector;
}

void QuantChartData::SetData(QuantChart* _base, const size_t _start, const size_t _end, Price* _scaleMaxPrice, Price* _p1, Price* _p2, const double _maxDomValue, const double _count,
							 std::vector<std::vector<Coordinate>>* _drawVector, std::vector<LabelsCoordinate>* _labelsVector, volatile LONG* numOfPulThread)
{
	numberOfPoolThread = numOfPulThread;
	base =		_base;
	start =		_start;
	end =		_end;
	scaleMaxPrice = _scaleMaxPrice;
	p1 = _p1;
	p2 = _p2; 
	maxDomValue = _maxDomValue;
	count = _count;
	drawVector = _drawVector;
	labelsVector = _labelsVector;
}

//функция рабочего элемента для пула потоков
VOID CALLBACK FindMaxMinPrices(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WORK Work)
{
	//данные для текущего потока
	BaseChartData pc = *(BaseChartData*)Context;
	
	const int indx = InterlockedIncrement(pc.numberOfPoolThread);	//номер потока

	//вычисляем диапазон для текущего потока
	size_t diff = pc.end - pc.start;
	size_t distance = double(diff / iNumberOfProcessors) + 0.5;
	size_t _start;
	if (!indx)
		_start = pc.start;	//если это нулевой поток то старт равен началу
	else 
		_start = pc.start + distance * indx;
	size_t _end = _start + distance;

	//готовим переменные для значений цен
	Price maxP((int)INT_MAX, (UINT)0, true), minP((int)INT_MAX, (UINT)0), lastBidP, lastAskP;	 

	//находим максимальные и минимальные цены
	FindMaxMinPriceInRange(_start, _end, distance * indx, pc._i, maxP, minP, pc.scale_type, lastBidP, lastAskP, NULL, pc.timeLabelsVector, indx);

	//записываем полученные значения цен в массив для текущего потока
	pc.maxPrice->at(indx) = maxP;
	pc.minPrice->at(indx) = minP;
	pc.lastBidPrice->at(indx) = lastBidP;
	pc.lastAskPrice->at(indx) = lastAskP;
}

void FindMaxMinPriceInRange(size_t _start, size_t _end, size_t x, IInstrument* _i, Price& maxP, Price& minP, 
							int scale_type, Price& lastBidP, Price& lastAskP, Price* lastP, std::vector<std::vector<std::pair<size_t,size_t>>>* pTimeLabelsVector, size_t indx)
{
	int bidLevel = -1, askLevel = -1;

	for (size_t y = _start; y <= _end; y++, x++)
	{
		Price tradePrice, bboPrice;

		//ищем самые дальние уровни с объемом
		for (int k = _i->GetDomLevelsCount()-1; k >= 0; k--)
		{		
			if (_i->Get_bidSize(y, k) && bidLevel == -1)
				bidLevel = k;

			if (_i->Get_askSize(y, k) && askLevel == -1)
				askLevel = k;
			
			if (bidLevel >= 0 && askLevel >= 0)
				break;
		}

		//ищем максимальную цену
		if (askLevel != -1)
		{
			bboPrice = _i->Get_askPrice(y, askLevel);
			if (maxP < bboPrice)
				maxP = bboPrice;
		}
		tradePrice = _i->Get_tradePrice(y);
		if (tradePrice.IsPrice() && maxP < tradePrice)
			maxP = tradePrice;
		
		//ищем минимальную цену
		if (bidLevel != -1)
		{
			bboPrice = _i->Get_bidPrice(y, bidLevel);
			if (minP > bboPrice)
				minP = bboPrice;
		}
		if (tradePrice.IsPrice() &&  minP > tradePrice)
			minP = tradePrice;

		if (scale_type == BaseChart::VISIBLE_RANGE)
		{
			//ищем максимальную цену лучшего офера
			bboPrice = _i->Get_askPrice(y, 0);
			if (lastAskP < bboPrice || !lastAskP.IsPrice())
				lastAskP = bboPrice;
			if (tradePrice.IsPrice() && lastAskP < tradePrice)
				lastAskP = tradePrice;
		
			//ищем минимальную цену лучшего бида
			bboPrice = _i->Get_bidPrice(y, 0);
			if (lastBidP > bboPrice || !lastBidP.IsPrice())
				lastBidP = bboPrice;
			if (tradePrice.IsPrice() && lastBidP > tradePrice)
				lastBidP = tradePrice;
		}
		
		//рисуем вертикальные оси с метками и надписями
		if (_i->Get_yAxis(y))
			(*pTimeLabelsVector).at(indx).push_back(std::pair<size_t,size_t>(y,x));

		bidLevel = -1, askLevel = -1;	//сбрасываем для следующей итерации

		//запоминаем последнюю цену трейда для метки
		if (lastP && _i->Get_tradePrice(y).IsPrice())
			*lastP = _i->Get_tradePrice(y);
	}
}

//функция рабочего элемента для пула потоков
VOID CALLBACK CalculateCoordinates(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WORK Work)
{
	//данные для текущего потока
	QuantChartData pd = *(QuantChartData*)Context;
	IInstrument* _i = pd.base->instrumentPtr;

	const int indx = InterlockedIncrement(pd.numberOfPoolThread);	//номер потока

	//вычисляем диапазон для текущего потока
	size_t diff = pd.end - pd.start;
	size_t distance = double(diff / iNumberOfProcessors) + 0.5;
	size_t _start;
	if (!indx)
		_start = pd.start;	//если это нулевой поток то старт равен началу
	else 
		_start = pd.start + distance * indx;
	size_t _end = _start + distance;

	//вычисляем координаты для рисования
	GetDOMCoordinates(pd.base, _start, _end, _i, *pd.scaleMaxPrice, *pd.p1, *pd.p2, pd.maxDomValue, pd.count, indx, distance, pd.drawVector, pd.labelsVector);
}

void GetDOMCoordinates(const QuantChart* base, const size_t _start, const size_t _end, IInstrument* _i, const Price& scaleMaxPrice, Price& p1, Price& p2, const double maxDomValue, 
					   const double count, const int indx, const size_t distance, std::vector<std::vector<Coordinate>>* drawVector, std::vector<LabelsCoordinate>* labelsVector)
{
	Coordinate coord;
	LabelsCoordinate lbCoord;
	double bar;
	int bidTradeFromX = 0, bidTradeToX = 0, askTradeFromX = 0, askTradeToX = 0, inspredTradeFromX = 0, inspredTradeToX = 0, limitsToX = 0;
	
	for (size_t j = indx * distance, y = _start; y <= _end; j++, y++)
	{
		for (Price k = p2; k <= p1; _i->IncPrice(k))
		{		
			if (k == _i->Get_tradePrice(y))
			{
                if (base->showOpenInterestChange == TRUE)
                {
                    Plaza2Instrument* instrument = ((Plaza2TickerAdapterClass*)base->instrumentPtr)->GetInstrument();
                    Plaza2Trade* trade = instrument->GetBook(y).trade;

                    switch (trade->interestDir)
                    {
                    case UNDEFINED: coord.color = base->OINotChangeBackground; break;
                    case OPEN_POSITION: coord.color = base->OIIncreaseBackground; break;
                    case CLOSE_POSITION: coord.color = base->OIDecreaseBackground; break;
                    }
                }

				//трейд по биду
				if (_i->Get_agressorSide(y) == 'S' || _i->Get_impliedSide(y) == 'S')
				{
                    if (base->showOpenInterestChange == FALSE)
                    {
					    if (_i->Get_agressorSide(y) == '?')
						    coord.color = base->noagressorColor;
					    else coord.color = base->sellTradeColor;
                    }
					if (!base->bEventAutoScale)	
						bar = (double)_i->Get_tradeSizeQuant(y) / maxDomValue * (count - 1);
					else	//если ручное масштабирование событий
					{
						bar = (double)_i->Get_tradeSizeQuant(y) / maxDomValue * (count/2 - 1);
						if (bar >= count/2) bar = count/2;
					}
					if (bar < 1.) ++bar;
					bidTradeFromX = (double)_i->PriceDifference(scaleMaxPrice, k) * count - 1;
					bidTradeToX = (double)_i->PriceDifference(scaleMaxPrice, k) * count - bar - 1;
					coord.From.first = (int)j, coord.From.second = bidTradeFromX;
					coord.To.first = (int)j, coord.To.second = bidTradeToX;
					(*drawVector).at(indx).push_back(coord);
				}
				//трейд по аску
				else if (_i->Get_agressorSide(y) == 'B' || _i->Get_impliedSide(y) == 'B')
				{
                    if (base->showOpenInterestChange == FALSE)
                    {
                        if (_i->Get_agressorSide(y) == '?') 
                            coord.color = base->noagressorColor;
                        else coord.color = base->buyTradeColor;
                    }
					if (!base->bEventAutoScale)
						bar = (double)_i->Get_tradeSizeQuant(y) / maxDomValue * (count - 1);
					else
					{
						bar = (double)_i->Get_tradeSizeQuant(y) / maxDomValue * (count/2 - 1);
						if (bar >= count/2) bar = count/2;
					}
					if (bar < 1.) ++bar;
					askTradeFromX = (double)_i->PriceDifference(scaleMaxPrice, k) * count + 1;
					askTradeToX = (double)_i->PriceDifference(scaleMaxPrice, k) * count + bar + 1;
					coord.From.first = (int)j, coord.From.second = askTradeFromX;
					coord.To.first = (int)j, coord.To.second = askTradeToX;
					(*drawVector).at(indx).push_back(coord);
				}
				//трейд внутри спрэда
				else if (_i->Get_agressorSide(y) == '?' || _i->Get_impliedSide(y) == 'M')
				{
                    if (base->showOpenInterestChange == FALSE)
                    {
					    coord.color = base->noagressorColor;
                    }
					if (!base->bEventAutoScale)
						bar = (double)_i->Get_tradeSizeQuant(y) / maxDomValue * (count - 1);
					else
					{
						bar = (double)_i->Get_tradeSizeQuant(y) / maxDomValue * (count/2 - 1);
						if (bar >= count/2) bar = count/2;
					}
					if (bar < 1.) ++bar;
					inspredTradeFromX = (double)_i->PriceDifference(scaleMaxPrice, k) * count;
					inspredTradeToX = (double)_i->PriceDifference(scaleMaxPrice, k) * count + bar + 1;
					coord.From.first = (int)j, coord.From.second = inspredTradeFromX;
					coord.To.first = (int)j, coord.To.second = inspredTradeToX;
					(*drawVector).at(indx).push_back(coord);
				}
			}
							
			//все уровни в стакане
			for (int x = _i->GetDomLevelsCount()-1; x >= 0; x--)
			{
				//бид лимиты
				if (k == _i->Get_bidPrice(y, x) && _i->Get_bidSize(y, x))
				{
					if ((_i->Get_agressorSide(y) == 'b' || _i->Get_agressorSide(y) == 'x') && _i->Get_eventBidLevel(y) & (1 << x)) 
						coord.color = base->newBidLimitColor;
					else coord.color = base->bidLimitColor;
					if (!base->bEventAutoScale)
						bar = (double)_i->Get_bidSize(y, x) / maxDomValue * (count - 1);
					else
					{
						bar = (double)_i->Get_bidSize(y, x) / maxDomValue * (count/2 - 1);
						if (bar >= count/2) bar = count/2;
					}
					limitsToX = (double)_i->PriceDifference(scaleMaxPrice, k) * count - bar - 1;
					if (bidTradeToX == limitsToX) ++limitsToX;
					coord.From.first = (int)j, coord.From.second = (double)_i->PriceDifference(scaleMaxPrice, k) * count - 1;
					coord.To.first = (int)j, coord.To.second = limitsToX;
					(*drawVector).at(indx).push_back(coord);

					//метки
					if (base->limitsLabels && y == _end && indx == iNumberOfProcessors-1)
					{
						lbCoord.color1 = base->newBidLimitColor;
						lbCoord.color2 = base->bidLimitColor;
						lbCoord.Ycoord = limitsToX;
						lbCoord.size = _i->Get_bidSize(_end, x);
						(*labelsVector).push_back(lbCoord);
					}
				}

				//бид ордера 
				if (base->showOrders && k == _i->Get_bidPrice(y, x) && _i->Get_bidOrders(y, x))
				{
					if ((_i->Get_agressorSide(y) == 'b' || _i->Get_agressorSide(y) == 'x') && _i->Get_eventBidLevel(y) & (1 << x)) 
						coord.color = base->newBidOrdersColor;
					else coord.color = base->bidOrdersColor;				
					bar = (double)_i->Get_bidOrders(y, x) / maxDomValue * (count - 1);
					if (bar >= count) bar = count-1; //это на случай если установлено ручное масштабирование событий и текущее значение события больше установленного вручную
					limitsToX = (double)_i->PriceDifference(scaleMaxPrice, k) * count - bar - 1;
					/*if (bidTradeToX && bidTradeToX == limitsToX)
						++limitsToX;*/
					coord.From.first = (int)j, coord.From.second = (double)_i->PriceDifference(scaleMaxPrice, k) * count - 1;
					coord.To.first = (int)j, coord.To.second = limitsToX;
					(*drawVector).at(indx).push_back(coord);

					//метка
					if (base->ordersLabels && y == _end && indx == iNumberOfProcessors-1)
					{
						lbCoord.color1 = base->newBidOrdersColor;
						lbCoord.color2 = base->bidOrdersColor;
						lbCoord.Ycoord = limitsToX;
						lbCoord.size = _i->Get_bidOrders(_end, x);
						(*labelsVector).push_back(lbCoord);
					}
				}
			
				//аск лимиты
				if (k == _i->Get_askPrice(y, x) && _i->Get_askSize(y, x))
				{
					if ((_i->Get_agressorSide(y) == 'a' || _i->Get_agressorSide(y) == 'x') && _i->Get_eventAskLevel(y) & (1 << x)) 
						coord.color = base->newAskLimitColor;
					else coord.color = base->askLimitColor;
					if (!base->bEventAutoScale)
						bar = (double)_i->Get_askSize(y, x) / maxDomValue * (count - 1);
					else
					{
						bar = (double)_i->Get_askSize(y, x) / maxDomValue * (count/2 - 1);
						if (bar >= count/2) bar = count/2;
					}
					limitsToX = (double)_i->PriceDifference(scaleMaxPrice, k) * count + bar + 1;
					if (askTradeToX == limitsToX) --limitsToX;
					if (limitsToX <= base->plotZone.bottom)
					{
						coord.From.first = (int)j, coord.From.second = (double)_i->PriceDifference(scaleMaxPrice, k) * count + 1;
						coord.To.first = (int)j, coord.To.second = limitsToX;
						(*drawVector).at(indx).push_back(coord);
					}

					//метки
					if (base->limitsLabels && y == _end && indx == iNumberOfProcessors-1)
					{
						lbCoord.color1 = base->newAskLimitColor;
						lbCoord.color2 = base->askLimitColor;
						lbCoord.Ycoord = limitsToX;
						lbCoord.size = _i->Get_askSize(_end, x);
						(*labelsVector).push_back(lbCoord);
					}					
				}				

				//аск ордера 
				if (base->showOrders && k == _i->Get_askPrice(y, x) && _i->Get_askOrders(y, x))
				{
					if ((_i->Get_agressorSide(y) == 'a' || _i->Get_agressorSide(y) == 'x') && _i->Get_eventAskLevel(y) & (1 << x)) 
						coord.color = base->newAskOrdersColor;
					else coord.color = base->askOrdersColor;
					//if (bar >= count) bar = count-1; //это на случай если установлено ручное масштабирование событий и текущее значение события больше установленного вручную
					bar = (double)_i->Get_askOrders(y, x) / maxDomValue * (count - 1);
					limitsToX = (double)_i->PriceDifference(scaleMaxPrice, k) * count + bar + 1;
					/*if (bidTradeToX && bidTradeToX == limitsToX)
						++limitsToX;*/
					if (limitsToX <= base->plotZone.bottom)
					{
						coord.From.first = (int)j, coord.From.second = (double)_i->PriceDifference(scaleMaxPrice, k) * count + 1;
						coord.To.first = (int)j, coord.To.second = limitsToX;
						(*drawVector).at(indx).push_back(coord);
					}

					//метка
					if (base->ordersLabels && y == _end && indx == iNumberOfProcessors-1)
					{
						lbCoord.color1 = base->newAskOrdersColor;
						lbCoord.color2 = base->askOrdersColor;
						lbCoord.Ycoord = limitsToX;
						lbCoord.size = _i->Get_askOrders(_end, x);
						(*labelsVector).push_back(lbCoord);
					}
				}
			}			
		}
	}
}


int StartPipeServerForNinja()
{
	/*PSECURITY_DESCRIPTOR psd;
	SECURITY_ATTRIBUTES sa;
	psd=(PSECURITY_DESCRIPTOR)LocalAlloc(LPTR,SECURITY_DESCRIPTOR_MIN_LENGTH);
	InitializeSecurityDescriptor(psd,SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(psd,true,NULL,false);
	sa.nLength=sizeof(sa);
	sa.bInheritHandle=true;
	sa.lpSecurityDescriptor=psd;*/

	unsigned int threadID;
	HANDLE hPipe = INVALID_HANDLE_VALUE, hPipeThread = INVALID_HANDLE_VALUE;

	//открываем новый канал
	hPipe = CreateNamedPipe(SERVER_PIPE_NAME, PIPE_ACCESS_INBOUND /*| WRITE_OWNER*/,/* PIPE_READMODE_MESSAGE | PIPE_TYPE_MESSAGE|*/ PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 
							PIPE_UNLIMITED_INSTANCES, 0, 0, INFINITE, NULL/*&sa*/);

	if (hPipe == INVALID_HANDLE_VALUE)
	{
		DWORD err = GetLastError();
		pMainWnd->UpdateActivityWindow(L"Algovisor Pipe-Server run error: Create pipe");
		return 1; //L"Algovisor Pipe-Server run error: Create pipe";
	}

	//добавляем новый канал к общему списку каналов
	hPipeServer.push_back(hPipe);

	//создаем новый поток для созданного канала
	hPipeThread = (HANDLE)_beginthreadex(NULL, 0, PipeThreadFunction, hPipe, 0, &threadID);

	if (hPipeThread == 0)
	{
		hPipeServer.pop_back();
		CloseHandle(hPipe);
		pMainWnd->UpdateActivityWindow(L"Algovisor Pipe-Server run error: Create thread");
		return 2; //L"Algovisor Pipe-Server run error: Create thread";
	}
	
	CloseHandle(hPipeThread);
	hPipeThreads.push_back(hPipeThread);

	//создаем мьютекс для синхронизации процессов
	if (hGlobalMutex == INVALID_HANDLE_VALUE)
		hGlobalMutex = CreateMutex(NULL, FALSE, L"Algovisor-Ninjatrader");
	
	return 0;
}

static unsigned int WINAPI PipeThreadFunction(void* arg)
{
	BOOL bConnect;
	HANDLE hPipe = (HANDLE)arg;

	//ждем подключения нового клиента
	bConnect = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
	
    if (bConnect)
    {      
		StartPipeServerForNinja();

		//HANDLE hHeap      = GetProcessHeap();
		//TCHAR* pchRequest = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE*sizeof(TCHAR));
		BYTE* pchRequest = new BYTE[1024];
		
		DWORD cbBytesRead = 0; 
		BOOL fSuccess = FALSE;

		while (true) 
		{ 
			fSuccess = ReadFile( 
				hPipe,			// handle to pipe 
				pchRequest,		// buffer to receive data 
				1024,			// size of buffer 
				&cbBytesRead,	// number of bytes read 
				NULL);			// not overlapped I/O 

			if (!fSuccess || cbBytesRead == 0)
			{   
				 if (GetLastError() == ERROR_BROKEN_PIPE)
				 {
					 goto end;
					  //_tprintf(TEXT("InstanceThread: client disconnected.\n"), GetLastError());
					  //CloseHandle(hPipe); 
				 }
				 else
				 {
					 goto end;
					  //_tprintf(TEXT("InstanceThread ReadFile failed, GLE=%d.\n"), GetLastError()); 
					  //CloseHandle(hPipe); 
				 }
				 break;
			} 	

			long value = *((long*)pchRequest); //первые 4 байта в каждом пакете от клиента пайпа сожержит тип сообщения

			switch (value)
			{
			case NinjaDialog::NINJA_INSTRUMENT_DATA:	
				if (pMainWnd->ninjaStatus)
				{
					pMainWnd->ninjaIdata.push_back(*((Instrument_DATA*)pchRequest));
					PostMessageW(pMainWnd->m_hWnd, WM_APP+6, 0, 0);
					//pMainWnd->ninjaIdata = (Instrument_DATA*)pchRequest;
					
					/*if (pMainWnd->CheckForNinjaInstrument())
						break;
					
					::AcquireSRWLockExclusive(&srwLockNinja);
					pMainWnd->NinjaInstrumentData(0, 0);
					PostMessageW(pMainWnd->m_hWnd, WM_APP+14, (WPARAM)(new Instrument_DATA(*pMainWnd->ninjaIdata)), 0);
					::ReleaseSRWLockExclusive(&srwLockNinja);*/					
					
					////добавляем инструмент в список
					//pMainWnd->instrumentList.InsertItem(0, _T(""));
					//pMainWnd->instrumentList.SetItemText(0, 1, L"NinjaTrader");
					//pMainWnd->instrumentList.SetItemText(0, 2, pMainWnd->ninjaIdata->ticker);
					//StringCchPrintfW(wstr, 100, L"%s", pMainWnd->ninjaIdata->ID); 
					//pMainWnd->instrumentList.SetItemText(0, 3, wstr);
					//pMainWnd->instrumentList.SetItemText(0, 4, L"Getting data");
					//pMainWnd->instrumentList.SetItemText(0, 5, pMainWnd->ninjaIdata->exchange);
					//pMainWnd->instrumentList.SetItemText(0, 6, pMainWnd->ninjaIdata->expiration);
					//pMainWnd->instrumentList.SetItemText(0, 7, pMainWnd->ninjaIdata->description);
					//
					//pMainWnd->instrumentList.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);	
				}
				break;
			case NinjaDialog::NINJA_TRADE:
				if (pMainWnd->ninjaStatus)
				{
					pMainWnd->ninjaTrade.push_back(*((Trade_DATA*)pchRequest));
					PostMessageW(pMainWnd->m_hWnd, WM_APP+7, 0, 0);
					//::AcquireSRWLockExclusive(&srwLockNinja);
					//pMainWnd->ninjaTrade.push_back(*((Trade_DATA*)pchRequest));
					//pMainWnd->NinjaInstrumentTrade(0, 0);
					////PostMessage(pMainWnd->m_hWnd, WM_APP+7, 0, 0);
					//::ReleaseSRWLockExclusive(&srwLockNinja);
				}
				break;
			case NinjaDialog::NINJA_BBO_ASK:
				if (pMainWnd->ninjaStatus)
				{
					pMainWnd->ninja_BBO_ASK.push_back(*((BBO_DATA*)pchRequest));
					PostMessageW(pMainWnd->m_hWnd, WM_APP+8, 0, 0);
					//::AcquireSRWLockExclusive(&srwLockNinja);
					//pMainWnd->ninja_BBO_ASK.push_back(*((BBO_DATA*)pchRequest));
					//pMainWnd->NinjaInstrumentAsk(0, 0);	
					////PostMessage(pMainWnd->m_hWnd, WM_APP+8, 0, 0);
					//::ReleaseSRWLockExclusive(&srwLockNinja);
				}
				break;

			case NinjaDialog::NINJA_BBO_BID:
				if (pMainWnd->ninjaStatus)
				{
					pMainWnd->ninja_BBO_BID.push_back(*((BBO_DATA*)pchRequest));
					PostMessageW(pMainWnd->m_hWnd, WM_APP+9, 0, 0);
					//::AcquireSRWLockExclusive(&srwLockNinja);
					//pMainWnd->ninja_BBO_BID.push_back(*((BBO_DATA*)pchRequest));
					//pMainWnd->NinjaInstrumentBid(0, 0);	
					////PostMessage(pMainWnd->m_hWnd, WM_APP+9, 0, 0);
					//::ReleaseSRWLockExclusive(&srwLockNinja);
				}
				break;
			case NinjaDialog::NINJA_DOM:
				if (pMainWnd->ninjaStatus)
				{
					pMainWnd->ninja_DOM.push_back(*((DOM_DATA*)pchRequest));
					PostMessageW(pMainWnd->m_hWnd, WM_APP+10, 0, 0);
					//::AcquireSRWLockExclusive(&srwLockNinja);
					//pMainWnd->ninja_DOM.push_back(*((DOM_DATA*)pchRequest));
					//pMainWnd->NinjaInstrumentDOM(0, 0);	
					////PostMessage(pMainWnd->m_hWnd, WM_APP+10, 0, 0);
					//::ReleaseSRWLockExclusive(&srwLockNinja);
				}
				break;
			case NinjaDialog::INSTRUMENT_DELETE:
				if (pMainWnd->ninjaStatus)
				{
					pMainWnd->ninjaDelete.push_back(*((DELETE_INSTRUMENT*)pchRequest));
					PostMessageW(pMainWnd->m_hWnd, WM_APP+11, 0, 0);
					//::AcquireSRWLockExclusive(&srwLockNinja);
					//pMainWnd->ninjaDelete.push_back(*((DELETE_INSTRUMENT*)pchRequest));
					//pMainWnd->NinjaInstrumentDELETE(0, 0);
					////PostMessage(pMainWnd->m_hWnd, WM_APP+11, 0, 0);
					//::ReleaseSRWLockExclusive(&srwLockNinja);
					goto Stop;
				}
				break;
			}
		}
Stop:
		
		if (hPipe != INVALID_HANDLE_VALUE)
		{
			hPipeServer.remove(hPipe);
			bConnect = DisconnectNamedPipe(hPipe);
			CloseHandle(hPipe);
			hPipe = INVALID_HANDLE_VALUE;
		}
end:
		delete[] pchRequest;
		//HeapFree(hHeap, 0, pchRequest);
	}

	return 0;
}

UINT TypeTextToActivMon(LPVOID p)
{
	wchar_t * str = (wchar_t*)p;
	size_t len = wcslen(str);
	wchar_t c1, c2;
	wchar_t underline = '_';

	for (size_t i = 0; i < len; ++i)
	{
		c1 = str[i]; str[i] = underline;
		c2 = str[i+1]; str[i+1] = 0;

		pMainWnd->activityMonitor.SetSel(0, -1);
		pMainWnd->activityMonitor.ReplaceSel(str);
		::Sleep(50);

		str[i] = c1; str[i+1] = c2;
	}

	pMainWnd->activityMonitor.SetSel(0, -1);
	pMainWnd->activityMonitor.ReplaceSel(str);

	delete[] str;

	SendMessage(pMainWnd->m_hWnd, WM_SYSCOMMAND, SC_KEYMENU, NULL); // равносильно нажатию кнопки ALT
	//SendMessage(pMainWnd->m_hWnd, VK_RETURN, VK_DOWN, 0);

	/*HMENU hMainMenu = GetMenu(pMainWnd->m_hWnd);
    HMENU hMenu = GetSubMenu(hMainMenu, 0);
    int itemID2 = GetMenuItemID(hMenu, 0);
 
    HiliteMenuItem(pMainWnd->m_hWnd, hMenu, itemID2, MF_HILITE);*/

	/*CWnd* pButtonWnd = pMainWnd->GetDlgItem(IDR_MAINMENU);
	pMainWnd->SendMessage(WM_COMMAND, MAKELONG(IDR_MAINMENU, BN_CLICKED), (LPARAM)pButtonWnd->GetSafeHwnd());*/
	//pMainWnd->PostMessage(WM_COMMAND, MAKELONG(nButtonID, BN_CLICKED), pButtonWnd->GetSafeHwnd());

	//HiliteMenuItem(pMainWnd->m_hWnd, pMainWnd->GetMenu(), 
	//HWND hwdMenu = pMainWnd->GetDlgItem(IDR_MAINMENU)->m_hWnd;
	//WPARAM WParam = MAKEWPARAM(IDR_MAINMENU, BN_CLICKED);
	//::PostMessage(pMainWnd->m_hWnd, WM_COMMAND, WParam, 0);

	/*RECT rt;

	if (!GetWindowRect(pMainWnd->m_hWnd, &rt))
    {
        return 0;
    }

    int x = rt.left+10;
    int y = rt.top+50;*/ 
	//PostMessage(pMainWnd->m_hWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(x, y));
	//PostMessage(pMainWnd->m_hWnd, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(x, y));

	return 0;
}

std::wstring wide_string(std::string const &s, std::locale const &loc)
{
    if (s.empty())
        return std::wstring();
    std::ctype<wchar_t> const &facet = std::use_facet<std::ctype<wchar_t> >(loc);
    char const *first = s.c_str();
    char const *last = first + s.size();
    std::vector<wchar_t> result(s.size());
    
    facet.widen(first, last, &result[0]);
    
    return std::wstring(result.begin(), result.end());
}

std::string narrow_string(std::wstring const &s, std::locale const &loc, char default_char)
{
    if (s.empty())
        return std::string();
    std::ctype<wchar_t> const &facet = std::use_facet<std::ctype<wchar_t> >(loc);
    wchar_t const *first = s.c_str();
    wchar_t const *last = first + s.size();
    std::vector<char> result(s.size());
    
    facet.narrow(first, last, default_char, &result[0]);
    
    return std::string(result.begin(), result.end());
}


//CString ConnectToAlgovisorServer(SOCKET& hSocket)
//{
//	WSADATA wsaData;
//
//	if (!::WSAStartup(MAKEWORD(2,0), &wsaData))
//	{
//		if (LOBYTE(wsaData.wVersion) < 2)
//		{
//			::WSACleanup();
//			return _T("WinSocket version not supported");
//		}
//	}
//
//	hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//	
//	if (hSocket == INVALID_SOCKET)
//	{
//		closesocket(hSocket);
//		::WSACleanup();
//		hSocket = INVALID_SOCKET;
//		return _T("Socket create ERROR");
//	}
//
//	hostent* addr;
//	addr = ::gethostbyname("algovisor.com");
//	if (!addr)
//	{
//		closesocket(hSocket);
//		::WSACleanup();
//		hSocket = INVALID_SOCKET;
//		return _T("DNS name ERROR");
//	}
//	
//	sockaddr_in sockAddr = {0};
//	sockAddr.sin_family = AF_INET;
//	sockAddr.sin_port = htons(80);
//	sockAddr.sin_addr.S_un.S_addr = *(ULONG*)(*(addr->h_addr_list));
//	//sockAddr.sin_addr.S_un.S_addr = inet_addr("5.45.119.245");
// 
//	// Подключаемся к серверу
//	if (connect(hSocket, (sockaddr*)(&sockAddr), sizeof(sockAddr)) != 0)
//	{
//		closesocket(hSocket);
//		::WSACleanup();
//		hSocket = INVALID_SOCKET;
//		return _T("Connection to server ERROR");
//	}
//	/* Замечение: приведение (sockaddr*) необхадимо, т.к. соединение требует переменную типа
//	   sockaddr, а тип переменной sockAddr sockaddr_in. Преобразование безопасно, т.к. оба типа имеют схожую структуру, но компилятор видит их как два разных типа*/
//
//
//	//считываем открытый ключ
//	FILE* openKey;
//	char sourceBuff[128] = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456";
//	sourceBuff[127] = '7';
//	char* encryptBuff;	
//
//	if (fopen_s(&openKey, "server_public.spu", "r"))
//	{
//		closesocket(hSocket);
//		::WSACleanup();
//		return _T("ERROR: file server_public.spu not found");
//	}
//	
//	RSA* rsa = NULL;
//	//rsa = PEM_read_RSA_PUBKEY(openKey, 0, 0, 0); //для pem-формата
//	//rsa = PEM_read_RSAPublicKey(openKey, 0, 0, 0) //для PKCS#1-формата
//	if (!(rsa = PEM_read_RSA_PUBKEY(openKey, 0, 0, 0))) 
//	{
//		fclose(openKey);
//		closesocket(hSocket);
//		::WSACleanup();
//		return _T("ERROR: bad format of server_public.spu");
//	}
//	fclose(openKey);
//
//	int keySize = RSA_size(rsa);
//	int buffSize = sizeof(sourceBuff);
//	int cryptSize;
//
//	//непосредственно шифрование буфера
//	if (keySize >= buffSize)
//	{
//		encryptBuff = new char[keySize];
//		cryptSize = keySize;
//		RSA_public_encrypt(keySize/*-11*/, (unsigned char*)sourceBuff, (unsigned char*)encryptBuff, rsa, RSA_NO_PADDING/*RSA_PKCS1_PADDING*/);
//	}
//	else
//	{
//		int iterator = 0;
//		int rowSize = keySize/*-11*/;
//		cryptSize = buffSize/keySize * keySize + keySize;
//		
//		encryptBuff = new char[cryptSize];
//		
//		while (iterator < buffSize)
//		{
//			RSA_public_encrypt(rowSize, (unsigned char*)(sourceBuff+iterator), (unsigned char*)(encryptBuff+iterator), rsa, RSA_NO_PADDING/*RSA_PKCS1_PADDING*/); //здесь при keySize-11 кодируется все равно кол-во символов равное keySize
//			iterator += keySize;
//			if ((buffSize - iterator) < keySize)
//				rowSize = buffSize - iterator;
//		}
//	}
//
//	//считываем закрытый ключ
//	//FILE* privKey;
//	//char* decryptBuff;
//
//	//if (fopen_s(&privKey, "server_private.spr", "r"))
//	//{
//	//	fclose(privKey);
//	//	GetDlgItem(IDC_STATIC)->SetWindowTextW(_T("ERROR: file public.key not found"));
//	//	closesocket(hSocket);
//	//	::WSACleanup();
//	//	return 1;
//	//}
//
//	//if (!(rsa = PEM_read_RSAPrivateKey(openKey, 0, 0, 0))) //для PKCS#1-формата
//	//{
//	//	GetDlgItem(IDC_STATIC)->SetWindowTextW(_T("ERROR: bad format of public.key"));
//	//	closesocket(hSocket);
//	//	::WSACleanup();
//	//	return 1;
//	//}
//	//fclose(privKey);
//
//	//keySize = RSA_size(rsa);	
//
//	////непосредственно дешифрование буфера
//	//if (keySize >= cryptSize)
//	//{
//	//	decryptBuff = new char[keySize];
//	//	RSA_private_decrypt(keySize, (unsigned char*)encryptBuff, (unsigned char*)decryptBuff, rsa, RSA_NO_PADDING/*RSA_PKCS1_PADDING*/);
//	//}
//	//else
//	//{
//	//	int iterator = 0;
//	//	int rowSize = keySize;
//	//	//int size = buffSize/keySize * keySize + keySize;
//	//	
//	//	decryptBuff = new char[cryptSize];
//	//	
//	//	while (iterator < cryptSize)
//	//	{
//	//		RSA_private_decrypt(rowSize, (unsigned char*)(encryptBuff+iterator), (unsigned char*)(decryptBuff+iterator), rsa, RSA_NO_PADDING/*RSA_PKCS1_PADDING*/); //здесь при keySize-11 кодируется все равно кол-во символов равное keySize
//	//		iterator += keySize;
//	//		if ((cryptSize - iterator) < keySize)
//	//			rowSize = cryptSize - iterator;
//	//	}
//	//}
//	
//
//
//	//пример использования функций send и recv
//	//char* get = "GET /echoport.php?data=KIRILL123 HTTP/1.1\r\nHOST: algovisor.com\r\n\r\n";
//	char* post_request = "POST /decryptssl.php HTTP/1.1\r\nHOST: algovisor.com\r\nAccept: */*\r\nContent-Type: application/x-www-form-urlencoded; charset=UTF-8\r\nContent-Length: "; //12\r\n\r\ndata=";
//	char* post_request2 = "POST /echoportssl.php HTTP/1.1\r\nHOST: algovisor.com\r\nAccept: */*\r\nContent-Type: application/x-www-form-urlencoded; charset=UTF-8\r\nContent-Length: "; //12\r\n\r\ndata=";
//	char requestToServer[800];
//	char dataSize[10];
//
//	//делаем base64_encode для шифрованных данных
//	char* base64encode;
//	int base64Size = Base64EncodeGetRequiredLength(cryptSize);
//	base64encode = new char[base64Size];
//	Base64Encode((BYTE*)encryptBuff, cryptSize, base64encode, &base64Size, ATL_BASE64_FLAG_NOCRLF);
//	delete[] encryptBuff;
//
//	std::string encodeCryptBuffer(base64encode, base64Size);
//	encodeCryptBuffer = UrlEncode(encodeCryptBuffer);
//	delete[] base64encode;
//
//	//собираем строку запроса + данные
//	strcpy(requestToServer, post_request);
//	itoa((int)encodeCryptBuffer.size()+5, dataSize, 10);
//	strcpy(requestToServer+strlen(requestToServer), dataSize);
//	strcpy(requestToServer+strlen(requestToServer), "\r\n\r\ndata=");
//	int request_size = strlen(requestToServer) + encodeCryptBuffer.size();
//	memcpy((void*)(requestToServer+strlen(requestToServer)), encodeCryptBuffer.data(), encodeCryptBuffer.size());
//
//	/*FILE* zapros;
//	zapros = fopen("zapros.bin", "wb");
//	fwrite(requestToServer, sizeof(char), request_size, zapros);
//	fclose(zapros);*/
//	
//	// Отправляем данные
//	int curPos = 0;
//	while (request_size > 0)
//	{
//		int ret = send(hSocket, &requestToServer[curPos], request_size, 0);
//		if (ret == 0)
//			break;
//		else if (ret == SOCKET_ERROR)
//		{
//			shutdown(hSocket, SD_BOTH);
//			closesocket(hSocket);
//			::WSACleanup();
//			return _T("ERROR: server is not responding");
//		}
//		request_size -= ret;
//		curPos += ret;
//	}
//
//	//ответ от сервера
//	//FILE* srv_answ;
//	//srv_answ = fopen("server_answer.bin", "w");
//	//setvbuf(srv_answ, NULL, _IONBF, 0);
//
//	//char answerFromServer[1024] = {0};
//	//int answSize = 1024;
//	//curPos = 0;
//
//	//while (answSize > 0)
//	//{
//	//	int ret = recv(hSocket, &answerFromServer[curPos], 1024, 0);
//	//	if (ret == 0)	//если соединение закрыто
//	//		break;
//	//	else if (ret == SOCKET_ERROR) // ошибка
//	//	{
//	//		shutdown(hSocket, SD_BOTH);
//	//		closesocket(hSocket);
//	//		::WSACleanup();
//	//		return _T("ERROR: data from server is incorrect");
//	//	}
//	//	fwrite(&answerFromServer[curPos], sizeof(char), ret, srv_answ);
//
//	//	answSize -= ret;
//	//	curPos += ret;		
//	//}
//	
//	//fclose(srv_answ);
//
//	shutdown(hSocket, SD_BOTH);
//	closesocket(hSocket);
//	::WSACleanup();
//
//	return 0;
//}