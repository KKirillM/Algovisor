#include "stdafx.h"
#include "CommandLineParser.h"

//////////////////////////////////////////////////////////////////////////

static const char* SkipSpaces(const char* pStr)
{
	_ASSERT(pStr);
	static char spaces[] = " \t\n\r";
	return pStr + strspn(pStr, spaces);
}

static const char* GetTocken(const char* pStr, std::string* pRes)
{
	_ASSERT(pStr);
	_ASSERT(pRes);

	pRes->erase();

	// Skip left spaces
	pStr = SkipSpaces(pStr);
	if (*pStr != '\0')
	{
		// In quotes
		// (don't skip any spaces in quotes!)
		if (*pStr == '"')
		{
			pStr++;
			if (*pStr != '\0')
			{
				// Find closing quotation
				LPCTSTR p;
				for (p = pStr; *p != '\0'; p++)
				{
					if (*p == '"')
					{
						if (*(p + 1) != '"')
						{
							p++;
							break;
						}
						p++;
					}
					*pRes += *p;
				}
				return p;
			}
		}
		// Without quotes
		else
		{
			// Find begin of the next tocken
			static char delimiters[] = " \t\n\r\"";    // Spaces & quotation
			size_t len = strcspn(pStr, delimiters);
			pRes->assign(pStr, len);
			return pStr + len;
		}
	}

	return 0;
}

LPCTSTR utils::CCommandLineParser::CLGetFirstArg(std::string* pArg)
{
	_ASSERT(pArg);
	if (!pArg) 
		return 0;

	// Skip first argument - module name
	LPCTSTR pNext = GetTocken(GetCommandLine(), pArg);

	if (pNext) 
		pNext = GetTocken(pNext, pArg);

	return pNext;
}

const char* utils::CCommandLineParser::CLGetNextArg(LPCTSTR pPrevRet, std::string* pArg)
{
	_ASSERT(pArg);
	_ASSERT(pPrevRet);
	if (!pArg || !pPrevRet) return 0;

	return GetTocken(pPrevRet, pArg);
}

bool utils::CCommandLineParser::CLGetArgValue(const char* uprArgName, TCHAR sep, std::string* pArgValue)
{
	if (pArgValue) pArgValue->erase();

	std::string clArg, clPARAM;
	LPCTSTR pNext = CLGetFirstArg(&clArg);
	while (pNext)
	{
		if (clArg.size() > 0 &&
			(clArg[0] == '-' || clArg[0] == '/'))
		{
			MakeUpperStr(clArg.c_str() + 1, clArg.size() - 1, clPARAM);

			if (!sep)
			{
				if (clPARAM == uprArgName)
				{
					return true;
				}
			}
			else
			{
				std::string::size_type sepPos = clPARAM.find(sep);
				if (sepPos != std::string::npos && sepPos > 0)
				{
					std::string tocken(clPARAM, 0, sepPos++);
					std::string value;
					if (clPARAM.size() > sepPos) value.assign(clPARAM, sepPos, clPARAM.size() - sepPos);

					if (tocken == uprArgName)
					{
						if (pArgValue) pArgValue->assign(value);
						return true;
					}
				}
			}
		}

		pNext = CLGetNextArg(pNext, &clArg);
	}

	return false;
}

inline char ToUpper(char c)
{
	return static_cast<char>(toupper(c));
}

void utils::CCommandLineParser::MakeUpperStr(const char* src, size_t len, std::string& dest)
{
	dest.resize(len);
	std::string::iterator to = dest.begin();
	PCSTR end = src + len;
	for (; src != end; src++, to++)
		*to = ToUpper(*src);
}

void utils::CCommandLineParser::MakeUpperStr(std::string& str)
{
	for (std::string::iterator from = str.begin(); from != str.end(); from++)
		*from = ToUpper(*from);
}

//////////////////////////////////////////////////////////////////////////

utils::CSimpleParserA::CSimpleParserA()
{

}

utils::CSimpleParserA::~CSimpleParserA()
{

}

bool utils::CSimpleParserA::parse(const std::string& data, char sep, std::vector<std::string>& values)
{
	size_t pos = 0;
	size_t prev_pos = 0;

	do
	{
		pos = data.find(sep, prev_pos);

		std::string val;
		if( pos != std::string::npos ){
			val = data.substr(prev_pos, pos - prev_pos);  
		}
		else{
			val = data.substr(prev_pos, data.size() -  prev_pos);
		}

		prev_pos = pos + 1;

		if( val.size() > 0 )
			values.push_back(val);
	}
	while( pos != std::string::npos );

	return true;
}