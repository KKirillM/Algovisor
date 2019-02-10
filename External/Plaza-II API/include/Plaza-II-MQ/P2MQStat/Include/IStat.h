// IStat.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMQSTAT_H__A42228DF_29AF_4CE6_B755_18517B55A156__INCLUDED_)
#define AFX_IMQSTAT_H__A42228DF_29AF_4CE6_B755_18517B55A156__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../Plaza-II/P2Utils/Include/ServiceDriver.h"
#include "../../../Plaza-II/P2Common/Include/P2Types.h"
#include "../../P2MQRouter/Include/IModuleServices.h"
#define STATFUNCTOR 
#include "StatType.h"

struct IRouter;

// 
// IStat - ��������� ��������������� ������ (�������).
// ������������ ��� ����� ���������� ���������� �������.
// ������, ����������� IStat ������:
//	- ���� �������� � �� ����������� ������� ���������
//	  � ������� ������ ������������������ ������� (P2StatisticData::data).
//	- ������������ ������ � P2StatisticData::data ������ �� ������ � 
//	  ������������ � �������������� ������ (��. IStat::AnnounceStatisticData)
//
// � 6 ������ ���������� ������ ���� �������� ��������� ����� ��� ������ ������� ���������� 
// � ���������� ��� �� ������������. ��� ��� ����� �����.
//
struct IStat 
{
	// ����������� �������� ������ � ������� ����������
	// pModule - ���������� ��������� �� ������ (������)
	//			 {���� ��������� P2STAT_OVERALL_MODULE,
	//			 �� ������ ������������ P2ERR_OK}.
	// moduleName - ������������ ������
	// moduleIndex - ��������� �� �����������, � �������, � ������ ������, �����
	//				 ������� ������ (handle) ������� ������ � ������� ����������.
	//				 ��� ����������� �������� ������������ � ������� ����� �������
	virtual P2ERR RegisterModule(const void *pModule, const char *moduleName,
		P2STAT_MODULE_INDEX *moduleIndex /*out*/) = 0;
	// ���������� ������������������� ������ �� ������� ����������
	// ����� ������ ����� ������, moduleIndex ����� ����������
	virtual P2ERR UnRegisterModule(P2STAT_MODULE_INDEX moduleIndex) = 0;

	
	// ����������� ��������������� ��������� � ������� ����������
	// moduleIndex - ��. RegisterModule
	// statName - ������������ ��������������� ���������
	// dataType - ��� ������ ��������������� ���������
	// pStatData - out-��������, ���������� ��������� �� ������������������
	//			��������� P2StatisticData
	// dataSize - ���� dataType == P2STAT_BUF/P2STAT_BUF0, �� ����� ��������� ����������
	//			��������� � �������, ����� - �� �����, ����� 0
	// synchro - ����� ������ ������� � ������ (������������/��������������)
	//			� ��������, � ����� ������ ������� ����� ���������� � P2StatisticData::data
	//			������������� (�� �������� LockStatisticData/UnLockStatisticData)
	virtual P2ERR AnnounceStatisticForUserData(P2STAT_MODULE_INDEX moduleIndex,
		const char *statName, P2STAT_DATA_TYPE dataType, void *pUserData,
		size_t dataSize, bool synchro, P2StatisticData **pStatData) = 0;

	virtual P2ERR AnnounceFunctor(P2STAT_MODULE_INDEX moduleIndex, const char *statName,
		IStatFunctor *pStatFunctor, bool synchro, bool isCommand, P2StatisticData **pStatData) = 0;

	// ��... ����� ��... �����
	virtual P2ERR LockStatisticData(P2STAT_MODULE_INDEX moduleIndex, P2StatisticData *pStatData) = 0;
	// ��... ����� ��... ������� )))
	virtual P2ERR UnLockStatisticData(P2STAT_MODULE_INDEX moduleIndex, P2StatisticData *pStatData) = 0;

	// ���������� ���������� ��������������� ��������� 
	// ����� ������ RevokeStatisticData, pStatData ���������.
	virtual P2ERR RevokeStatisticData(P2STAT_MODULE_INDEX moduleIndex,
		P2StatisticData *pStatData) = 0;
	// ���������� ���������� ���� �������������� ���������� ��� ������� ������
	virtual P2ERR PurgeStatistics(P2STAT_MODULE_INDEX moduleIndex) = 0;

	// ���������� ����� ����, ��� IRouter �������� �������������.
	virtual P2ERR OnRouterStarted(IRouter *pIRouter) = 0;

	virtual void PrintVersionModules() = 0;

	CHECK_FOR_DELETE__(IStat)
    
    virtual ~IStat() { }
};


#endif // !defined(AFX_IMQSTAT_H__A42228DF_29AF_4CE6_B755_18517B55A156__INCLUDED_)
