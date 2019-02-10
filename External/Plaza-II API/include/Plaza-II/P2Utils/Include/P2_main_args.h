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

// правильная C++ байда для ToUpper 
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
// <string> -> строкой считается всё (лексема), что находится между токенами-разделителями
			// если в строке встречаются кавычки, то они опускаются, а содержимое,
			// находящееся в них, не анализируется на наличие токенов
			// например, (argName == `""rrr "123"continue') будет оттранслирован
			// в `rrr 123continue'
// Пример1:
// `-arg1"/eee:":val1,"   val2",val3",,,"xxx' будет оттранслировано в:
// <argName> -> `arg1/eee:'
// <list_of_vals> -> `val1', `   val2', `val3,,,xxx',
// при условии, что префиксы и сепараторы равны значениям по-умолчанию (c_p2*)
//
// Пример2:
// `arg1:::val1' будет оттранслировано в:
// <argName> -> `arg1'
// <list_of_vals> -> `::val1',
// при условии, что префиксы и сепараторы равны значениям по-умолчанию (c_p2*)

// Пример3 (несбалансированная кавычка):
// `"arg1:::val1,val2' будет оттранслировано в:
// <argName> -> `arg1:::val1,val2'
// <list_of_vals> -> e,
// при условии, что префиксы и сепараторы равны значениям по-умолчанию (c_p2*)

class CP2_main_args
{
private:
	typedef vector<string> values_t;
	typedef struct arg_tuple_t_
	{
		size_t pos;
		bool unbalanced;
		values_t vals;
		// для итерации по значениям аргумента с помощью функций ArgumentValGet(First|Next)
		values_t::iterator curVal;
	} arg_tuple_t;
	typedef string arg_t;
	typedef map<arg_t, arg_tuple_t> argsmap_t;
	argsmap_t m_argsmap;

	bool m_eventMode;

	// за токен считается строка либо начинающаяся с *p и заканчивающаяся одним из
	// разделителей, переданным в delim
	// если в строке встречается '"', то ищется пАрная ей и содержимое между ними
	// не анализируется
	// если после выхода inQuotes == true, то это означает, что в строке есть
	// незакрытые кавычки
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
					str++; // один из разделителей
					l = 0;
					break;
				}
			}
			else if (str[l] == '"')
			{
				s.append(str, l);
				str += l;
				str++; // кавычки
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
	// события, возникающие при парсинге аргумента
	// индуцируются только в том случае, если был вызван
	// CP2_main_args::CP2_main_args(int argc, char* argv[], ...) или Init(...)
	// Очевидно, что при добавлении аргументов с помощью AppendArgument, данные
	// события не вызываются
	// параметр unbalanced равный true означает, что в аргументе была обнаружена
	// незакрытая двойная кавычка
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

//nest. в VS 2003 старый метод не работает
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
			// 0-й аргумент добавляем без анализа
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
			return;	// объект не инициализирован (не вызван Init())

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

	// После вызова Init()|Deinit()|AppendArgument(), все указатели,
	// полученные методами Argument*, неопределены!
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
