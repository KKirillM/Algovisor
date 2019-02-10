#pragma once

#include <string>

// ������ ����� �� ����� �������������� ���������� � ���������� ������ ���� � ����� �����
std::string CreateFolder(const std::string& folderPath);

// ����������� ������ �� ����� ����� � ������
bool CopyFile(const std::string& sourceFilePath, const std::string& destinationFilePath);