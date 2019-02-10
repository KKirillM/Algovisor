#include "stdafx.h"

#include "IniFileWorker.h"
#include "sstream"

//проверка файла на открытость
#define CHECK_OPEN_INI_FILE(fileHwnd) \
	if (!fileHwnd.is_open())	\
	{	\
		string err;	\
		err.append("Error, operation with non-open ini file\n");	\
		throw IniFileException(err.c_str(), IniFileException::ERROR_OPEN_FILE);	\
	}

 IniFileWorker::IniFileWorker()
 {
 }

IniFileWorker::~IniFileWorker()
{
	Close();
}

void IniFileWorker::Open(const string& fname)
{
	if (fname.empty())
	{
		string err;
		err.append("File name is empty:\n");
		throw IniFileException(err.c_str(), IniFileException::ERROR_OPEN_FILE);
	}

	m_fs.open(fname, ios::in | ios::out | ios::binary /*| ios::app*/);

	if (!m_fs.is_open())
	{
		string err;
		err.append("Error opening ini file: %s\n", fname.c_str());
		m_fs.close();
		throw IniFileException(err.c_str(), IniFileException::ERROR_OPEN_FILE);		
	}
	else
	{
		m_name = fname;
	}
}

void IniFileWorker::Close()
{
	if (m_fs.is_open())
		m_fs.close();
}

const string& IniFileWorker::GetName() const
{
	return m_name;
}

bool IniFileWorker::IsOpen() const
{
	return m_fs.is_open();
}

void IniFileWorker::WriteInt(const string& sect, const string& key, int val)
{
	CHECK_OPEN_INI_FILE(m_fs)

	string data;
	char keyVal[256];
	sprintf_s(keyVal, sizeof(keyVal), "%s=", key.c_str());				//подготовим строку с ключом

	GetDataFromFile(data);
	size_t sectionPos = FindSection(data, sect);
	if (sectionPos == string::npos)	//если секции нет, добавляем её и ключ
	{
		AddNewSection(data, sect, data.size());
		AddKeyInSection(data, keyVal, FindSection(data, sect)+sect.size()+1);
	}
	else							//секция есть ищем ключ
	{
		size_t sectionEndPos = sectionPos + sect.size() + 1;
		size_t keyPos = FindKeyPosInSection(data, keyVal, sectionEndPos);

		if (keyPos == string::npos)	//если ключа нет, вставляем его в начало секции
			AddKeyInSection(data, keyVal, sectionEndPos);
		else						//ключ есть, удаляем старый и пишем новый
		{
			sprintf_s(keyVal, sizeof(keyVal), "%s%d", keyVal, val);	//подготовим строку с ключом
			ChangeKeyValue(data, keyPos, keyVal);
		}
	}	

	CleanFile();
	m_fs << data;
	m_fs.seekg(0);					//после записи в файл надо обязательно вернуть позицию указателя на данные в файле на прежнее место
}

void IniFileWorker::WriteStr(const string& sect, const string& key, const string& val)
{
	CHECK_OPEN_INI_FILE(m_fs)

	string data;
	string keyVal = key + '=';

	GetDataFromFile(data);
	size_t sectionPos = FindSection(data, sect);
	if (sectionPos == string::npos)		//если секции нет, добавляем её и ключ
	{
		AddNewSection(data, sect, data.size());
		AddKeyInSection(data, keyVal, FindSection(data, sect)+sect.size()+1);
	}
	else								//секция есть ищем ключ
	{
		size_t sectionEndPos = sectionPos + sect.size() + 1;
		size_t keyPos = FindKeyPosInSection(data, keyVal, sectionEndPos);

		if (keyPos == string::npos)		//если ключа нет, вставляем его в начало секции
			AddKeyInSection(data, keyVal, sectionEndPos);
		else							//ключ есть, удаляем старый и пишем новый
		{
			keyVal += val;
			ChangeKeyValue(data, keyPos, keyVal);
		}
	}	

	CleanFile();
	m_fs << data;
	m_fs.seekg(0);
}

int IniFileWorker::ReadInt(const string& sect, const string& key)
{
	CHECK_OPEN_INI_FILE(m_fs)

	string data;
	int value;

	GetDataFromFile(data);
	size_t sectionPos = FindSection(data, sect);
	if (sectionPos == string::npos)		// секции нет
		throw IniFileException("No this section in ini file", IniFileException::NO_SECTION);
	else								// есть ищем ключ
	{
		size_t sectionEndPos = sectionPos + sect.size() + 1;
		size_t keyPos = FindKeyPosInSection(data, key, sectionEndPos);

		if (keyPos == string::npos)		// ключа нет
			throw IniFileException("No this key in ini file", IniFileException::NO_KEY);
		else							// ключ есть
		{
			size_t valPos = keyPos + key.size();
			valPos += 1;				// пропускаем символ '=' и устанавливаем указатель на первый символ значения
			if (!isdigit(data[valPos]))
				throw IniFileException("Unknown format of ini file", IniFileException::BAD_FILE_FORMAT);
			value = atoi(&data[valPos]);
		}	
	}

	m_fs.seekg(0);

	return value;
}

string IniFileWorker::ReadStr(const string& sect, const string& key)
{
	CHECK_OPEN_INI_FILE(m_fs)

	string data;
	string value;

	GetDataFromFile(data);
	size_t sectionPos = FindSection(data, sect);
	if (sectionPos == string::npos)		// секции нет
		throw IniFileException("No section in ini file: " + sect, IniFileException::NO_SECTION);
	else								// секция есть ищем ключ
	{
		size_t sectionEndPos = sectionPos + sect.size() + 1;
		size_t keyPos = FindKeyPosInSection(data, key, sectionEndPos);

		if (keyPos == string::npos)		// ключа нет
			throw IniFileException("No key in ini file:" + key, IniFileException::NO_KEY);
		else							// ключ есть
		{
			size_t valPos = keyPos + key.size();
			valPos += 1;				// пропускаем символ '=' и устанавливаем указатель на первый символ значения
			size_t endValPos = data.find('\r', valPos);
			value.assign(data, valPos, endValPos-valPos);
		}	
	}

	m_fs.seekg(0);

	return value;
}
void IniFileWorker::EnumSections(vector<string>& list)
{
	CHECK_OPEN_INI_FILE(m_fs)

	string data;
	string line;
	size_t pos = 0;

	GetDataFromFile(data);

	while ((line = GetNextString(data, pos)) != "")
	{
		if (IsNewSectionString(line))
		{
			size_t pos = 0;
			pos = line.find('[', pos);
			++pos;
			list.push_back(string(line, pos, line.find(']', pos)-pos));
		}
		
		pos = data.find('\n', pos);	//переходим к след строке
	}

	m_fs.seekg(0);
}

void IniFileWorker::EnumKeys(map<string, string>& list, const string& section)
{
    CHECK_OPEN_INI_FILE(m_fs)

    string data;
    string line;
    size_t pos = 0;

    GetDataFromFile(data);

    while ((line = GetNextString(data, pos)) != "")
    {
        if (IsNewSectionString(line))
        {
            size_t sectionNamePos = line.find(section);
            if (sectionNamePos == std::string::npos)
            {
                pos = data.find('\n', pos);	//переходим к след строке
                continue;
            }
            else
            {
                pos = data.find('\n', pos);	//переходим к след строке
                while ((line = GetNextString(data, pos)) != "" && !IsNewSectionString(line))
                {
                    pos = data.find('\n', pos);	//переходим к след строке
                    if (!IsCommentString(line))
                    {
                        size_t equalPosition = line.find('=', 0);
                        if (equalPosition == std::string::npos)
                        {
                            throw IniFileException("Key string has bad format, section: [" + section + "], line: " + line, IniFileException::BAD_FILE_FORMAT);
                        }
                        list[string(line, 0, equalPosition)] = string(line, equalPosition+1, line.find('\n', 0)-equalPosition);
                    }
                }
				break;
            }
        }
        else
            pos = data.find('\n', pos);	//переходим к след строке
    }

    m_fs.seekg(0);
}

void IniFileWorker::GetDataFromFile(string& data)
{
	data.assign(istreambuf_iterator<char>(m_fs), (istreambuf_iterator<char>()));
}

void IniFileWorker::CleanFile()
{
	if (m_fs.is_open())
		m_fs.close();

	m_fs.open(m_name, ios::in | ios::out | ios::trunc | ios::binary);
}

bool IniFileWorker::IsCommentInString(const string& data, size_t pos) const
{
	size_t semicolonPos = data.find(';', 0);
	if (semicolonPos != string::npos && semicolonPos < pos)
		return true;
	else
		return false;
}

bool IniFileWorker::IsNewSectionString(const string& data) const
{
	size_t squareBracketPos = data.find('[');
	if (squareBracketPos == string::npos)
		return false;

	if (!IsCommentInString(data, squareBracketPos))
		return true;
	else
		return false;
}

void IniFileWorker::ChangeKeyValue(string& data, size_t oldValuePos, string newValue)
{
    // бывает случай этот ключ послединй в файле и не содержит значения, для этого добавим перевод строки, чтобы эта функция отрабатывала правильно
    if (data.back() == '=') 
        data += '\n';
	
	//удаляем старый ключ до символов ';' или '\n' или ' ' или '\r' и записываем на его место новый
	size_t endPos = data.find('\r', oldValuePos);
	if (endPos == string::npos)
	{
		endPos = data.find(' ');
		if (endPos == string::npos)
			endPos = data.find(';');
		if (endPos == string::npos)
			throw IniFileException("Unknown format of ini file", IniFileException::BAD_FILE_FORMAT);
	}

	data.erase(oldValuePos, endPos-oldValuePos);
	AddKeyInSection(data, newValue, oldValuePos);
}

size_t IniFileWorker::FindSection(const string& data, const string& sect) const
{
	return data.find(sect);
}

void IniFileWorker::AddNewSection(string& data, const string& sect, const size_t pos)
{
	data.insert(pos, '\n'+sect+'\n');
}

void IniFileWorker::AddKeyInSection(string& data, const string& keyVal, const size_t pos)
{
	data.insert(pos, keyVal);
}

string IniFileWorker::GetNextString(const string& data, size_t& pos)
{
	string res;
	while ((data[pos] == '\r' || data[pos] == '\n') && pos != data.size()-1)
		++pos;

	size_t end = data.find("\r\n", pos);
	if (pos == string::npos)
		return "";
	
	res = data.substr(pos, end-pos);
	return res;
}

size_t IniFileWorker::FindKeyPosInSection(const string& data, const string& keyVal, const size_t pos)
{
	size_t keyPos = pos;
	string line;
	while ((line = GetNextString(data, keyPos)) != "")
	{
		if (IsNewSectionString(line))
			return string::npos;

		size_t pos = line.find(keyVal);
		if (pos != string::npos && !IsCommentInString(line, pos))
		{
			break;
		}
		keyPos = data.find('\n', keyPos);	//переходим к след строке
	}

	return keyPos;
}