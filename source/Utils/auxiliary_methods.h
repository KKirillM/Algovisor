#pragma once

#include <string>

// создаёт папку со всеми промежуточными подпапками и возвращает полный путь к новой папке
std::string CreateFolder(const std::string& folderPath);

// копирование файлов из одной папки в другую
bool CopyFile(const std::string& sourceFilePath, const std::string& destinationFilePath);