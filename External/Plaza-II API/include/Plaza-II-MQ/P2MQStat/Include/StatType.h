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

// ������� �� �����������!

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
	// ������� ���� ������ �������� � ���������������� ������������, � m_data ��������� �� ��� ����������. � ���.
	P2STAT_UINT8,
	P2STAT_INT8,
	P2STAT_UINT16,
	P2STAT_INT16,
	P2STAT_UINT32,
	P2STAT_INT32,
	P2STAT_UINT64,
	P2STAT_INT64,
	P2STAT_WCHAR,
	// �������� ����� � ���������������� ������������. m_data ��������� �� ����. ���������� ��� �������� ����.
	P2STAT_BUF,
	// ������, ��������������� �����. �������� ��� ����� � ������������. 
	// ������ ������������� ������� 0 � ��������� �������. ��� ���� ������ ��������, � ������ �� ��������, �� ��� �����
	// ��������� ������� - ��� dataSize-1, �� ���� 0 ����������� � ������� ������
	P2STAT_BUF0,

	// P2STAT_CSTR - ��������� �� ����������� ������ (const char*)
	// ������������ �������� ��������� �� �������� ���������. 
	// ��� ����� �������� �� ����� ������ ������ �������� ���������, ��� ����������, ��� ������ ������
	P2STAT_CSTR,
	// ������������ ������ ��������� P2TIME
	P2STAT_TIME,
	P2STAT_FUNCTOR,
	P2STAT_FUNCTOR_WITH_EFFECT
};


#define P2STAT_NAME_LEN			255

// ��������� � �������� ������������ ������� �� ��������������� �������
struct P2StatisticData
{
	// ����� �������� ��������� �� ���������� � ������������
	union
	{
		void *m_VPtr;
		char *m_CPtr;
		IStatFunctor *m_IPtr;
	}m_data;
	P2STAT_DATA_TYPE m_dataType;
	// ������ ������ � ����� ������ ������ ���������� � ������
	INT32 m_dataSize;
	UINT32 m_idx;
	char m_name[P2STAT_NAME_LEN + 1];
};

namespace NS_Stat
{

// ����� �������� ����������, ��� void *
class ModuleInfo;
}
typedef NS_Stat::ModuleInfo* P2STAT_MODULE_INDEX;
#endif // _STATTYPE_H_
