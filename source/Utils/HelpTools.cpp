#include "stdafx.h"
#include "Shlobj.h" // SHCreateDirectoryEx() function use this include
#include "HelpTools.h"


HelpTools::HelpTools()
{
}


HelpTools::~HelpTools()
{
}


bool HelpTools::IsProcessNameInMemory(const std::string& processName)
{
	PROCESSENTRY32 peProcessEntry;
	HANDLE CONST hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(INVALID_HANDLE_VALUE == hSnapshot) 
		return false;

	peProcessEntry.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapshot, &peProcessEntry);		
	while(Process32Next(hSnapshot, &peProcessEntry))
	{
		if (std::string(processName.begin(), processName.end()).compare(peProcessEntry.szExeFile) == 0)
		{
			CloseHandle(hSnapshot);
			return true;
		}
	}

	CloseHandle(hSnapshot);

	return false;
}

HANDLE HelpTools::GetProcessHandle(const std::string& processName)
{
	PROCESSENTRY32 peProcessEntry;
	HANDLE CONST hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(INVALID_HANDLE_VALUE == hSnapshot) 
		return NULL;

	peProcessEntry.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapshot, &peProcessEntry);		
	while(Process32Next(hSnapshot, &peProcessEntry))
	{
		if (std::string(processName.begin(), processName.end()).compare(peProcessEntry.szExeFile) == 0)
		{
			CloseHandle(hSnapshot);
			return OpenProcess(PROCESS_TERMINATE, NULL, peProcessEntry.th32ProcessID);
		}
	}

	CloseHandle(hSnapshot);

	return NULL;
}

//void HelpTools::UpdatePrivilege()
//{
//    HANDLE hToken;
//    TOKEN_PRIVILEGES tp;
//    LUID luid;
//
//    if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,&hToken))
//    {
//       LookupPrivilegeValue(NULL,SE_DEBUG_NAME, &luid);
//
//       tp.PrivilegeCount = 1;
//       tp.Privileges[0].Luid = luid;
//       tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
//
//       AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
//    }
//}

void HelpTools::GetLastErrorString(TCHAR* str)
{
	::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_ALLOCATE_BUFFER 
    |FORMAT_MESSAGE_IGNORE_INSERTS,NULL,GetLastError(), 
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
    (LPTSTR)&str, 0, NULL); 
}

int HelpTools::ReadFileInBuffer(const std::string& filePath, char*& buffer)
{
    if (filePath.empty())
    {
        throw std::exception("filePath parameter is empty");
    }

    int fileLen = 0;
    FILE* pFile = NULL;

    if (NULL != (pFile = fopen(filePath.c_str(), "rb")))
    {
        fseek(pFile, 0, SEEK_END);
        fileLen = ftell(pFile);
        if (fileLen < 0)
        {
            throw std::exception(("Failed to read file size: " + filePath).c_str());
        }
        fseek(pFile, 0, SEEK_SET);
        buffer = (char*)malloc(fileLen);
        if (fread(buffer, 1, fileLen, pFile) != fileLen)
        {
            free(buffer);
            buffer = NULL;
            fclose(pFile);
            throw std::exception(("Failed to read previous state from file: " + filePath).c_str());
        }
        fclose(pFile);
    }
    return fileLen;
}

void HelpTools::SaveBinaryToFile(const std::string& filePath, char* buffer, int bufferLen)
{
    std::string folderPath = std::string(filePath.c_str(), filePath.rfind('\\'));

    DWORD dwAttrs = GetFileAttributes(folderPath.c_str());
    if(dwAttrs == INVALID_FILE_ATTRIBUTES)
    {  
        if (SHCreateDirectoryEx( NULL, folderPath.c_str(), NULL ) != ERROR_SUCCESS)
        {
            char errorNum[100] = {0};
            _itoa_s(GetLastError(), errorNum, sizeof(errorNum), 10);
            std::string errorString = "Error create directory: " + folderPath + " Error number: ";
            errorString += errorNum;
            throw std::exception(errorString.c_str());
        }
    }

    FILE* pFile;
    char errMsg[512];
    pFile = std::fopen(filePath.c_str(), "wb");
    if (pFile)
    {
        fwrite(buffer, 1, bufferLen, pFile);
        fclose(pFile);
    } 
    else
    {
        sprintf_s(errMsg, sizeof(errMsg), "FAILED to store data to file: %s", filePath.c_str());
        throw std::exception(errMsg);
    }
}

void HelpTools::WriteToFile(const std::string& stream, const char* name, bool exist /*= false*/)
{
    FILE* f = NULL;
    char errMsg[512];
    errno_t  error = 0;

    // THREAD_TRACER("Open file");
    if (exist)	//создавать каждый раз заново файл или дописывать в существующий
    {
        error = ::fopen_s(&f, name, "ab");
    }
    else
    {
        error = ::fopen_s(&f, name, "wb");
    }

    if (f)
    {
        setvbuf(f, NULL, _IONBF, 0);	//выключаем буферизацию для файла
        ::fprintf_s(f, stream.c_str());
        ::fclose(f);
    }
    else
    {
        sprintf_s(errMsg, sizeof(errMsg), "File creation error: %d", error);
        throw std::exception(errMsg);
    }
    // THREAD_TRACER("Close file");
}
