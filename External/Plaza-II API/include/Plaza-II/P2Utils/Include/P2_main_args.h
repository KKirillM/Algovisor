#ifndef __P2_MAIN_ARGS__
#define __P2_MAIN_ARGS__

#ifdef _WIN32
#pragma warning (disable: 4786)
#pragma warning (disable: 4503)
#endif //_WIN32

#include <stdio.h>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cctype>
#include <locale>

#include "P2Str.hpp"

using namespace std;

static const char * const c_p2argPrefixes = "/-";
static const char * const c_p2arg_valSeparators = ":";
static const char * const c_p2valsSeparators = ",;";

// ���������� C++ ����� ��� ToUpper 
   struct StructToUpper
   {
       StructToUpper(std::locale const& l) : loc(l) {;}
       char operator() (char c) const  { return std::toupper(c,loc); }
   private:
       std::locale const& loc;
   };
   
   struct StructToLower
   {
       StructToLower(std::locale const& l) : loc(l) {;}
       char operator() (char c) const  { return std::tolower(c,loc); }
   private:
       std::locale const& loc;
   };
   

// <argument> -> [<p2argPrefixe>]<argName>[<p2arg_valSeparator><list_of_vals>]
// <argName> -> <string>
// <list_of_vals> -> <list_of_vals><string><p2valsSeparators> | e
// <string> -> ������� ��������� �� (�������), ��� ��������� ����� ��������-�������������
			// ���� � ������ ����������� �������, �� ��� ����������, � ����������,
			// ����������� � ���, �� ������������� �� ������� �������
			// ��������, (argName == `""rrr "123"continue') ����� ��������������
			// � `rrr 123continue'
// ������1:
// `-arg1"/eee:":val1,"   val2",val3",,,"xxx' ����� ��������������� �:
// <argName> -> `arg1/eee:'
// <list_of_vals> -> `val1', `   val2', `val3,,,xxx',
// ��� �������, ��� �������� � ���������� ����� ��������� ��-��������� (c_p2*)
//
// ������2:
// `arg1:::val1' ����� ��������������� �:
// <argName> -> `arg1'
// <list_of_vals> -> `::val1',
// ��� �������, ��� �������� � ���������� ����� ��������� ��-��������� (c_p2*)

// ������3 (������������������ �������):
// `"arg1:::val1,val2' ����� ��������������� �:
// <argName> -> `arg1:::val1,val2'
// <list_of_vals> -> e,
// ��� �������, ��� �������� � ���������� ����� ��������� ��-��������� (c_p2*)

class CP2_main_args
{
private:
	typedef vector<string> values_t;
	typedef struct arg_tuple_t_
	{
		size_t pos;
		bool unbalanced;
		values_t vals;
		// ��� �������� �� ��������� ��������� � ������� ������� ArgumentValGet(First|Next)
		values_t::iterator curVal;
	} arg_tuple_t;
	typedef string arg_t;
	typedef map<arg_t, arg_tuple_t> argsmap_t;
	argsmap_t m_argsmap;

	bool m_eventMode;

	// �� ����� ��������� ������ ���� ������������ � *p � ��������������� ����� ��
	// ������������, ���������� � delim
	// ���� � ������ ����������� '"', �� ������ ������ �� � ���������� ����� ����
	// �� �������������
	// ���� ����� ������ inQuotes == true, �� ��� ��������, ��� � ������ ����
	// ���������� �������
	void get_tok(char **p, string &s, bool &inQuotes, const char *delim)
	{
		char *str = *p;
		int l;
		
		s.erase();
		
		if (str == NULL || *str == '\0')
			return;

		l = 0;
		inQuotes = false;
		while (str[l] != '\0')
		{
			if (strchr(delim, str[l]) != NULL)
			{
				if (inQuotes)
					l++;
				else
				{
					s.append(str, l);
					str += l;
					str++; // ���� �� ������������
					l = 0;
					break;
				}
			}
			else if (str[l] == '"')
			{
				s.append(str, l);
				str += l;
				str++; // �������
				l = 0;				

				inQuotes = !inQuotes;
			}
			else
				l++;
		}

		s.append(str, l);
		*p = str + l;
	}

protected:
	int m_argc;
	char **m_argv;
	char *m_argPrefixes;
	char *m_arg_valSeparators;
	char *m_valsSeparators;

protected:
	// �������, ����������� ��� �������� ���������
	// ������������ ������ � ��� ������, ���� ��� ������
	// CP2_main_args::CP2_main_args(int argc, char* argv[], ...) ��� Init(...)
	// ��������, ��� ��� ���������� ���������� � ������� AppendArgument, ������
	// ������� �� ����������
	// �������� unbalanced ������ true ��������, ��� � ��������� ���� ����������
	// ���������� ������� �������
	virtual void OnNewArgument(const char *argName, int argNum, bool unbalanced) {};
	virtual void OnNewArgumentVal(const char *argName, const char *argVal, bool unbalanced) {};

public:
	CP2_main_args()
	{
		m_eventMode = false;
		m_argc = 0;
		m_argv = NULL;
		m_argPrefixes = m_arg_valSeparators = m_valsSeparators = NULL;
	}

	CP2_main_args(int argc, char* argv[],
		const char *argPrefixes = c_p2argPrefixes,
		const char *arg_valSeparators = c_p2arg_valSeparators,
		const char *valsSeparators = c_p2valsSeparators)
	{
		CP2_main_args();
		Init(argc, argv, argPrefixes, arg_valSeparators, valsSeparators);
	}

	virtual ~CP2_main_args()
	{
		Deinit();
	}

//nest. � VS 2003 ������ ����� �� ��������
#if _MSC_VER >= 1310 || defined(__GNUC__)
	void ToUpper(string &str)
	{
		StructToUpper up(std::locale::classic());
		std::transform(str.begin(), str.end(), str.begin(), up);
	}
#else
	void ToUpper(string &str)
	{
		ctype<char> o;
		o.toupper(str.begin(), str.end());
	}
#endif

	void Init(int argc, char* argv[],
		const char *argPrefixes = c_p2argPrefixes,
		const char *arg_valSeparators = c_p2arg_valSeparators,
		const char *valsSeparators = c_p2valsSeparators)
	{
		Deinit();
		m_argc = argc;
		m_argv = argv;
		m_argPrefixes = P2StrDup(argPrefixes);
		m_arg_valSeparators = P2StrDup(arg_valSeparators);
		m_valsSeparators = P2StrDup(valsSeparators);
		if (m_argc > 0)
		{
			// 0-� �������� ��������� ��� �������
			arg_tuple_t &t = m_argsmap[m_argv[0]];
			t.pos = 0;
			t.unbalanced = false;
			t.curVal = t.vals.begin();
			OnNewArgument(m_argv[0], 0, false);

			int i = 1;
			m_eventMode = true;
			while (i < argc)
			{
				char *p = argv[i];
				AppendArgument(p, argPrefixes, arg_valSeparators, valsSeparators);
				i++;
			}
			m_eventMode = false;
		}
	}

	void AppendArgument(char *arg,
		const char *argPrefixes = NULL,
		const char *arg_valSeparators = NULL,
		const char *valsSeparators = NULL)
	{
		string a, v;
		bool inQuotes = false;
		static int args_count = 1;

		if (m_argc < 1)
			return;	// ������ �� ��������������� (�� ������ Init())

		if (argPrefixes == NULL)
			argPrefixes = m_argPrefixes;
		if (arg_valSeparators == NULL)
			arg_valSeparators = m_arg_valSeparators;
		if (valsSeparators == NULL)
			valsSeparators = m_valsSeparators;

		if (strchr(argPrefixes, *arg) != NULL)
			arg++;
		
		get_tok(&arg, a, inQuotes, arg_valSeparators);
		if (!a.empty())
		{
		    ToUpper(a);
			arg_tuple_t &t = m_argsmap[a];
			if (m_eventMode)
				OnNewArgument(a.c_str(), args_count, inQuotes);
			while ('\0' != *arg)
			{
				get_tok(&arg, v, inQuotes, valsSeparators);
				if (!v.empty())
				{
					t.vals.push_back(v);
					if (m_eventMode)
						OnNewArgumentVal(a.c_str(), v.c_str(), inQuotes);
				}
			}
			t.pos = args_count;
			t.unbalanced = inQuotes;
			t.curVal = t.vals.begin();
			
			args_count++;
		};
	}

	void Deinit()
	{
		m_argc = 0;
		m_argv = NULL;
		m_argsmap.clear();
		if (m_argPrefixes != NULL)
		{
			free(m_argPrefixes);
			m_argPrefixes = NULL;
		}
		if (m_arg_valSeparators != NULL)
		{
			free(m_arg_valSeparators);
			m_arg_valSeparators = NULL;
		}
		if (m_valsSeparators != NULL)
		{
			free(m_valsSeparators);
			m_valsSeparators = NULL;
		}
	}

	bool ArgumentNameCheck(const char *argName)
	{
		if (argName != NULL)
		{
			string a(argName);
			ToUpper(a);
			argsmap_t::iterator i = m_argsmap.find(a);
			if (i != m_argsmap.end())
				return true;
		}
		return false;
	}

	int ArgumentsCount()
	{
		return static_cast<int> (m_argsmap.size());
	}

	// ����� ������ Init()|Deinit()|AppendArgument(), ��� ���������,
	// ���������� �������� Argument*, ������������!
	const char *ArgumentNameGet(int argNum)
	{
		if (argNum >= 0 && argNum < ArgumentsCount())
		{
			argsmap_t::iterator i = m_argsmap.begin();
			for (; i != m_argsmap.end(); i++)
			{
				if (i->second.pos == static_cast<size_t>(argNum))
				{
					return i->first.c_str();
				}
			}
		}
		return NULL;
	}

	const char *ArgumentValGetFirst(const char *argName)
	{
		if (argName != NULL)
		{
			string a(argName);
			ToUpper(a);
			argsmap_t::iterator i = m_argsmap.find(a);
			if (i != m_argsmap.end())
			{
				i->second.curVal = i->second.vals.begin();
				if (i->second.curVal != i->second.vals.end())
				{
					return i->second.curVal->c_str();
				}
			}
		}
		return NULL;
	}

	const char *ArgumentValGetNext(const char *argName)
	{
		if (argName != NULL)
		{
			string a(argName);
			ToUpper(a);
			argsmap_t::iterator i = m_argsmap.find(a);
			if (i != m_argsmap.end())
			{
				if (i->second.curVal != i->second.vals.end())
				{
					i->second.curVal++;
					return i->second.curVal->c_str();
				}
			}
		}
		return NULL;
	}

	const char *ArgumentValGet(const char *argName, size_t valNum)
	{
		if (argName != NULL)
		{
			string a(argName);
			ToUpper(a);
			argsmap_t::iterator i = m_argsmap.find(a);
			if (i != m_argsmap.end())
			{
				if (valNum < i->second.vals.size())
				{
					return i->second.vals[valNum].c_str();
				}
			}
		}
		return NULL;
	}

	const char *ArgumentValGet(int argNum, size_t valNum)
	{
		const char *argName = ArgumentNameGet(argNum);
		return ArgumentValGet(argName, valNum);
	}
};

#endif	//__P2_MAIN_ARGS__
