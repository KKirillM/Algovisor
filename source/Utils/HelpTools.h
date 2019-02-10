#pragma once

class HelpTools
{
public:
	HelpTools();
	~HelpTools();

	static bool IsProcessNameInMemory(const std::string& processName);	//�������� ������� ����������� �������� � ������ ������
	static HANDLE GetProcessHandle(const std::string& processName);		//��������� ������ ������ �������� �� ��� ����� 
	static void GetLastErrorString(TCHAR* str);							//��������� GetLastError ������	

    // ������� ��������� �������� ������ �� ����� �� ���������� ���� � ���������� �� � �����
    // ������ ��� ����� ���������� ����� ��������
    // ������� ���������: ���� � ������, ��������� �� �����
    // ������������ ��������: ������ ����������� ������, ���� ����� �� ���������� ������� ���������� 0
    static int ReadFileInBuffer(const std::string& filePath, char*& buffer);

    // ������� ���������� �������� ������ � ����, ���� ����� �� ����������, �� ��� ��������
    static void SaveBinaryToFile(const std::string& filePath, char* buffer, int bufferLen);

    // ������ ������ � ����
    static void WriteToFile(const std::string& stream, const char* name, bool exist = false);

}; //HelpTools

//������ ����� ��� ����� ���� ���� � ��� ���
//		DWORD dwAttrs;
//		LPCWSTR aa=L"..\\..\\temp\\logs";
//		dwAttrs = GetFileAttributes(aa);
//		if(dwAttrs == INVALID_FILE_ATTRIBUTES)
//		{  
//			LPCTSTR lpPathName = L"..\\..\\temp\\logs";
//			CreateDirectory(lpPathName, NULL);
//		}

