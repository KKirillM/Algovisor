#pragma once

class HelpTools
{
public:
	HelpTools();
	~HelpTools();

	static bool IsProcessNameInMemory(const std::string& processName);	//проверка наличия запущенного процесса с данным именем
	static HANDLE GetProcessHandle(const std::string& processName);		//получение хэндла чужого процесса по его имени 
	static void GetLastErrorString(TCHAR* str);							//получение GetLastError строки	

    // функция считывает бинарные данные из файла по указанному пути и записывает их в буфер
    // память под буфер выделяется самой функцией
    // входные параметры: путь к буферу, указатель на буфер
    // возвращаемое значение: размер записанного буфера, если файла не существует функция возвращает 0
    static int ReadFileInBuffer(const std::string& filePath, char*& buffer);

    // функция записывает бинарные данные в файл, если папки не существует, то она создаётся
    static void SaveBinaryToFile(const std::string& filePath, char* buffer, int bufferLen);

    // запись данных в файл
    static void WriteToFile(const std::string& stream, const char* name, bool exist = false);

}; //HelpTools

//создаём папку для файла лога если её нет ещё
//		DWORD dwAttrs;
//		LPCWSTR aa=L"..\\..\\temp\\logs";
//		dwAttrs = GetFileAttributes(aa);
//		if(dwAttrs == INVALID_FILE_ATTRIBUTES)
//		{  
//			LPCTSTR lpPathName = L"..\\..\\temp\\logs";
//			CreateDirectory(lpPathName, NULL);
//		}

