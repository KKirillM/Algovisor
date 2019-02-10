#ifndef _STATTYPE_H_
#define _STATTYPE_H_
#include <set>
#include <string>

typedef std::set<std::string>  sSet;

enum t_loadFrom
{
	NOT_LOAD,
	LOAD_FROM_ROUTER,
	LOAD_FROM_P2MQAPP
};

#include "../../../Plaza-II/P2Common/Include/P2Types.h"

struct P2StatisticData;

// ћј——»¬џ Ќ≈ –≈јЋ»«ќ¬јЌџ!

#ifdef STATFUNCTOR
#include "../../../Plaza-II/P2Utils/Include/IRc.h"
struct IStatFunctor : public IRc
{
	virtual char *Get() = 0;
	virtual void Free(char *str) = 0;
};
#else
struct IStatFunctor;
#endif


enum P2STAT_DATA_TYPE
{
	P2STAT_NONE = 0,
	// простые типы просто хран€тс€ в пользовательском пространстве, а m_data указывает на эту переменную. и все.
	P2STAT_UINT8,
	P2STAT_INT8,
	P2STAT_UINT16,
	P2STAT_INT16,
	P2STAT_UINT32,
	P2STAT_INT32,
	P2STAT_UINT64,
	P2STAT_INT64,
	P2STAT_WCHAR,
	// бинарный буфер в пользовательском пространстве. m_data указывает на него. печатаетс€ как бинарный блок.
	P2STAT_BUF,
	// строка, заканчивающа€с€ нулем. хранитс€ как буфер у пользовател€. 
	// крайне рекомендуетс€ держать 0 в последней позиции. ибо если строка мен€етс€, а данные не залочены, то все плохо
	// последн€€ позици€ - это dataSize-1, то есть 0 учитываетс€ в размере буфера
	P2STAT_BUF0,

	// P2STAT_CSTR - указатель на константную строку (const char*)
	// пользователь передает указатель на подобный указатель. 
	// при таком хранении он может просто мен€ть значение указател€, это безопаснее, чем мен€ть строку
	P2STAT_CSTR,
	// пользователь хранит структуру P2TIME
	P2STAT_TIME,
	P2STAT_FUNCTOR,
	P2STAT_FUNCTOR_WITH_EFFECT
};


#define P2STAT_NAME_LEN			255

// структура с видимыми пользователю данными по статистическому объекту
struct P2StatisticData
{
	// здесь хранитс€ указатель на статданные у пользовател€
	union
	{
		void *m_VPtr;
		char *m_CPtr;
		IStatFunctor *m_IPtr;
	}m_data;
	P2STAT_DATA_TYPE m_dataType;
	// размер буфера с точки зрени€ модул€ статистики в байтах
	INT32 m_dataSize;
	UINT32 m_idx;
	char m_name[P2STAT_NAME_LEN + 1];
};

namespace NS_Stat
{

// лучше неполное объ€вление, чем void *
class ModuleInfo;
}
typedef NS_Stat::ModuleInfo* P2STAT_MODULE_INDEX;
#endif // _STATTYPE_H_
