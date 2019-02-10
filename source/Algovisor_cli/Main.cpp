#include <windows.h>
#include <TChar.h>
#include <stdio.h>
#include <string>
#include <Shlwapi.h>

void InstallNinjaIndicator(std::wstring path, bool install)
{
	HKEY key;
	LONG nResult;
	wchar_t pszNinjaFolder[512] = {0};
	wchar_t pszAlgovisorFolder[512] = {0};
	wchar_t log_str[512];
	DWORD len = 512;
	//FILE* f;	
	BOOL res = 0;
	DWORD err = 0;
	wchar_t pszNinjaKey[] = L"\\Software\\NinjaTrader\\NinjaTrader 7\\{A579A0D1-B30E-0DD9-3269-2C1C2282AEDA}";
	DWORD dwNinjaKeyLen = wcslen(pszNinjaKey);

	//::fopen_s(&f, "c:\\Algovisor_installer_LOG.txt", "wb");

	DWORD maxSubKeyLen;
	//определяем длину (в символах) самого длинного названия подраздела
	::RegQueryInfoKeyW(HKEY_USERS, NULL, NULL, NULL, NULL, &maxSubKeyLen, NULL, NULL, NULL, NULL, NULL, NULL);

	//добавляем один символ для оканчивающего строку нуля
	maxSubKeyLen++;

	//буфер для имени подраздела
	wchar_t* pszKeyName = new wchar_t[maxSubKeyLen + dwNinjaKeyLen];

	DWORD dwKeyNameLen = maxSubKeyLen;
	DWORD dwIndex = 0;
	FILETIME ftLastWritten;

	//LOG
	//wsprintf(log_str, L"start enum register \n");
	//fwrite(log_str, sizeof(wchar_t), wcslen(log_str), f);
	
	//цикл до появления ошибки
	while (ERROR_SUCCESS == RegEnumKeyEx(HKEY_USERS, dwIndex++, pszKeyName, &dwKeyNameLen, NULL, NULL, NULL, &ftLastWritten)) 
	{
		wcscat(pszKeyName, pszNinjaKey);
		nResult = ::RegOpenKeyExW(/*HKEY_CURRENT_USER*/HKEY_USERS, pszKeyName, NULL, KEY_READ|/*KEY_WOW64_32KEY*/KEY_WOW64_64KEY, &key);
		if (nResult == ERROR_SUCCESS) 
		{
			nResult = RegGetValue(key, NULL, L"_D99DD24B0760493C81035A2B061DCF85", RRF_RT_REG_SZ, 0, pszNinjaFolder, &len); 
			if (nResult == ERROR_SUCCESS)
			{
				wcscat(pszNinjaFolder, L"Algovisor_NinjaTrader.dll");

				if (install)
				{
					wcscpy(pszAlgovisorFolder, path.c_str());
					wcscat(pszAlgovisorFolder, L"NinjaTrader\\Algovisor_NinjaTrader.dll");

					res = CopyFileW(pszAlgovisorFolder, pszNinjaFolder, FALSE);

					/*if (!res) {
						err = ::GetLastError();
						printf("GetLastError: %d\n", err);
					}*/
				}
				else
					::DeleteFile(pszNinjaFolder);
			}
		}
		dwKeyNameLen = maxSubKeyLen;

		RegCloseKey(key);
	}	

	

	/*if (nResult != ERROR_SUCCESS) {
			MessageBox(NULL, L"Ninjatrader not installed", L"Error", MB_OK);
			wsprintf(log_str, L"ERROR_SUCCESS when RegOpenKeyExW, Result = %d \n", nResult);
			fwrite(log_str, sizeof(wchar_t), wcslen(log_str), f);
			fclose(f);
			return -1; 
		} */

	/*nResult = RegGetValue(key, NULL, L"_D99DD24B0760493C81035A2B061DCF85", RRF_RT_REG_SZ, 0, str, &len); 
    if (nResult != ERROR_SUCCESS) { 
		MessageBox(NULL, L"Ninjatrader not installed", L"Error", MB_OK);
		wsprintf(log_str, L"ERROR_SUCCESS when RegGetValue \n");
		fwrite(log_str, sizeof(wchar_t), wcslen(log_str), f);
		fclose(f);
        return -1; 
    }

	wsprintf(log_str, L"open path: %s \n", str);
	fwrite(log_str, sizeof(wchar_t), wcslen(log_str), f);

	wcscat(str, L"Algovisor_NinjaTrader.dll");

	wsprintf(log_str, L"open path: %s \n", str);
	fwrite(log_str, sizeof(wchar_t), wcslen(log_str), f);

	wsprintf(log_str, L"commandLine path: %s \n", path);
	fwrite(log_str, sizeof(wchar_t), wcslen(log_str), f);

	CopyFileW(path,str,FALSE);

	MessageBox(NULL, L"Indicator installed to Ninjatrader", L"Complete", MB_OK);*/

	//fclose(f);
}

void CleanRegisterLicenseKey()
{
	HKEY key;
	LONG nResult;
	wchar_t pszKey[] = L"\\Software\\AlgoVisor\\AlgoVisor\\Keys";
	DWORD dwKeyLen = wcslen(pszKey);

	DWORD maxSubKeyLen;
	//определяем длину (в символах) самого длинного названия подраздела
	::RegQueryInfoKeyW(HKEY_USERS, NULL, NULL, NULL, NULL, &maxSubKeyLen, NULL, NULL, NULL, NULL, NULL, NULL);

	//добавляем один символ для оканчивающего строку нуля
	maxSubKeyLen++;

	//буфер для имени подраздела
	wchar_t* pszKeyName = new wchar_t[maxSubKeyLen + dwKeyLen];

	DWORD dwKeyNameLen = maxSubKeyLen;
	DWORD dwIndex = 0;
	FILETIME ftLastWritten;
	//DWORD err;

	//цикл до появления ошибки
	while (ERROR_SUCCESS == RegEnumKeyEx(HKEY_USERS, dwIndex++, pszKeyName, &dwKeyNameLen, NULL, NULL, NULL, &ftLastWritten))
	{
		wcscat(pszKeyName, pszKey);
		nResult = ::RegOpenKeyExW(HKEY_USERS, pszKeyName, NULL, KEY_ALL_ACCESS|KEY_WOW64_64KEY, &key);
		if (nResult == ERROR_SUCCESS) 
		{
			nResult = RegDeleteTreeW(key, 0);
			//nResult = SHDeleteKeyW(key, pszKeyName);
			//nResult = ::RegDeleteKeyW(key, pszKeyName);
			//err = ::GetLastError();
		}
		dwKeyNameLen = maxSubKeyLen;

		RegCloseKey(key);
	}
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE , LPTSTR commandLine, int nCmdShow)
{
	std::wstring cmd = commandLine;
	//cmd = L"/uninstall C:\\Program Files\\Algovisor platform\\";
	size_t n = 0;

	if ((n = cmd.find(L"/install")) != std::wstring::npos)
	{
		n += wcslen(L"/install");
		InstallNinjaIndicator(cmd.substr(n+1), true);
	}
	else if (cmd.find(L"/uninstall") != std::wstring::npos)
	{
		InstallNinjaIndicator(L"", false);
		CleanRegisterLicenseKey();
	}

	return 0;
}