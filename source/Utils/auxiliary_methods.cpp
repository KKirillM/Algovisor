#include "stdafx.h"

#include "auxiliary_methods.h"
#include "Shlobj.h" // SHCreateDirectoryEx() function use this include
#include <fstream>

std::string CreateFolder(const std::string& folderPath)
{
    TCHAR currDir[MAX_PATH];
    ::GetCurrentDirectory(MAX_PATH, currDir);

    std::string fullFolderPath(currDir+folderPath);

    DWORD dwAttrs;
    dwAttrs = ::GetFileAttributes(fullFolderPath.c_str());
    if(dwAttrs == INVALID_FILE_ATTRIBUTES)
    {  
        if (::SHCreateDirectoryEx( NULL, fullFolderPath.c_str(), NULL ) != ERROR_SUCCESS)
        {
            char errorNum[100] = {0};
            _itoa_s(GetLastError(), errorNum, sizeof(errorNum), 10);
            std::string errorString = "Error create directory: " + folderPath + " Error number: ";
            errorString += errorNum;
            throw std::exception(errorString.c_str());
        }
    }

    return fullFolderPath;
}


bool CopyFile(const std::string& sourceFilePath, const std::string& destinationFilePath)
{
    std::ifstream src(sourceFilePath, std::ios::binary);
    std::ofstream dest(destinationFilePath, std::ios::binary);
    dest << src.rdbuf();

    return src && dest;
}