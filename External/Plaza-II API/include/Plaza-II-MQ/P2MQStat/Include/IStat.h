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
// IStat - интерфейс статистического модуля (сервиса).
// Предназначен для сбора статистики работающих модулей.
// Объект, реализующий IStat должен:
//	- быть пассивен и не производить никаких изменений
//	  в рабочих данных зарегистрированных модулей (P2StatisticData::data).
//	- осуществлять доступ к P2StatisticData::data только на чтение в 
//	  эксклюзивном и неэксклюзивном режиме (см. IStat::AnnounceStatisticData)
//
// с 6 версии интерфейса решено было выкинуть выделение места под данные модулем статистики 
// и переложить это на пользователя. ему это проще будет.
//
struct IStat 
{
	// Регистрация рабочего модуля в сервисе статистики
	// pModule - уникальный указатель на модуль (объект)
	//			 {если передаётся P2STAT_OVERALL_MODULE,
	//			 то всегда возвращается P2ERR_OK}.
	// moduleName - наименование модуля
	// moduleIndex - указатель на переменнную, в которую, в случае успеха, будет
	//				 записан индекс (handle) данного модуля в сервисе статистики.
	//				 Все последующие операции производятся с помощью этого индекса
	virtual P2ERR RegisterModule(const void *pModule, const char *moduleName,
		P2STAT_MODULE_INDEX *moduleIndex /*out*/) = 0;
	// Исключение зарегистрированного модуля из сервиса статистики
	// после вызова этого метода, moduleIndex будет невалидным
	virtual P2ERR UnRegisterModule(P2STAT_MODULE_INDEX moduleIndex) = 0;

	
	// Регистрация статистического параметра в сервисе статистики
	// moduleIndex - см. RegisterModule
	// statName - наименование статистического параметра
	// dataType - тип данных статистического параметра
	// pStatData - out-параметр, возвращает указатель на инициализированную
	//			структуру P2StatisticData
	// dataSize - если dataType == P2STAT_BUF/P2STAT_BUF0, то здесь передаётся количество
	//			элементов в массиве, иначе - не нужен, можно 0
	// synchro - задаёт способ доступа к данным (эксклюзивный/неэксклюзивный)
	//			в принципе, в любой момент времени можно обратиться к P2StatisticData::data
	//			неэксклюзивно (не вызывать LockStatisticData/UnLockStatisticData)
	virtual P2ERR AnnounceStatisticForUserData(P2STAT_MODULE_INDEX moduleIndex,
		const char *statName, P2STAT_DATA_TYPE dataType, void *pUserData,
		size_t dataSize, bool synchro, P2StatisticData **pStatData) = 0;

	virtual P2ERR AnnounceFunctor(P2STAT_MODULE_INDEX moduleIndex, const char *statName,
		IStatFunctor *pStatFunctor, bool synchro, bool isCommand, P2StatisticData **pStatData) = 0;

	// Ну... вощем та... лочит
	virtual P2ERR LockStatisticData(P2STAT_MODULE_INDEX moduleIndex, P2StatisticData *pStatData) = 0;
	// Ну... вощем та... анлочит )))
	virtual P2ERR UnLockStatisticData(P2STAT_MODULE_INDEX moduleIndex, P2StatisticData *pStatData) = 0;

	// Прекращает мониторинг статистического параметра 
	// после вызова RevokeStatisticData, pStatData невалиден.
	virtual P2ERR RevokeStatisticData(P2STAT_MODULE_INDEX moduleIndex,
		P2StatisticData *pStatData) = 0;
	// Прекращает мониторинг всех статистических параметров для данного модуля
	virtual P2ERR PurgeStatistics(P2STAT_MODULE_INDEX moduleIndex) = 0;

	// Вызывается после того, как IRouter закончит инициализацию.
	virtual P2ERR OnRouterStarted(IRouter *pIRouter) = 0;

	virtual void PrintVersionModules() = 0;

	CHECK_FOR_DELETE__(IStat)
    
    virtual ~IStat() { }
};


#endif // !defined(AFX_IMQSTAT_H__A42228DF_29AF_4CE6_B755_18517B55A156__INCLUDED_)
