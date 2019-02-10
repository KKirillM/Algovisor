#pragma once
#pragma warning(disable: 4251)

#include <vector>
#include <map>
#include <fstream>

using namespace std;

class IniFileException : public std::exception
{
public:
	enum EXEPTION_TYPE { NO_SECTION, NO_KEY, BAD_FILE_FORMAT, ERROR_OPEN_FILE };

	IniFileException(const std::string s, EXEPTION_TYPE t) : exception(s.c_str()), type(t) {}
	EXEPTION_TYPE GetExceptionType() { return type; }

private:
	EXEPTION_TYPE type;
};

class IniFileWorker
{
public:
    IniFileWorker();
    ~IniFileWorker();

    void Open(const string& fname);
    void Close();
	const string& GetName() const;

    bool IsOpen() const;

    void WriteInt(const string& sect, const string& key, int val);
    void WriteStr(const string& sect, const string& key, const string& val);
    
    // секция, ключ и значение ключа обязательно должены быть в файле иначе вернётся исключение
    int ReadInt(const string& sect, const string& key);

    // секция и ключ обязательно должены быть в файле иначе вернётся исключение, а значение ключа может отсутствовать в этом случае вернётся пустая строка
    string ReadStr(const string& sect, const string& key);

	void EnumSections(vector<string>& list);
    void EnumKeys(map<string, string>& list, const string& section);

private:
    IniFileWorker(const IniFileWorker& );
    IniFileWorker& operator=(const IniFileWorker& );

private:
	void GetDataFromFile(string& data);
	string GetNextString(const string& data, size_t& pos);
	bool IsCommentInString(const string& data, size_t pos) const;
    bool IsCommentString(const string& data) const { return data[0] == ';'; }
	bool IsNewSectionString(const string& data) const;
	void ChangeKeyValue(string& data, size_t oldValuePos, string newValue);
	size_t FindSection(const string& data, const string& sect) const;
	void AddNewSection(string& data, const string& sect, const size_t pos);
	void AddKeyInSection(string& data, const string& keyVal, const size_t pos);
	size_t FindKeyPosInSection(const string& data, const string& keyVal, const size_t pos);
	void CleanFile();

private:
	string m_name;
    fstream m_fs;

};  // IniFileWorker