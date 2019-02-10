#ifndef _COMMAND_LINE_PARSER_
#define _COMMAND_LINE_PARSER_


#include <vector>
#include <algorithm>


namespace utils
{
	class CCommandLineParser
	{
	public:
		// for <argName>[<sep>[<value>]]:
		//  - check existence
		//  - get value to pArgValue
		// (uprArgName - in upper case!)
		// перед argName обязательно ставится символ '/' или '-'
		static bool CLGetArgValue(const char* uprArgName, TCHAR sep = 0, std::string* pArgValue = 0);
		static LPCTSTR CLGetFirstArg(std::string* pArg);
		static LPCTSTR CLGetNextArg(LPCTSTR pPrevRet, std::string* pArg);
		static void MakeUpperStr(const char* src, size_t len, std::string& dest);
		static void MakeUpperStr(std::string& str);
	};

	class CSimpleParserA
	{
	public:
		CSimpleParserA();
		~CSimpleParserA();

		static bool parse(const std::string& data, char sep, std::vector<std::string>& values);
	};

}	// namespace utils


#endif // _COMMAND_LINE_PARSER_ 