#ifndef _IPREPLDB_H_
#define _IPREPLDB_H_

#include "../../../Plaza-II/P2Utils/Include/IRc.h"

#include "../../../Plaza-II/P2SysScheme/Include/P2SysScheme.h"
#include "../../../Plaza-II-Repl/P2ReplCommon/Include/P2Repl.h"
#include "../../../Plaza-II/P2Utils/Include/AutoHandle.hpp"
#include "../../../Plaza-II-DB/P2DB/Include/P2DB.h"
#include "../../../Plaza-II/P2ACL/Include/P2ACL.h"
#include "../../../Plaza-II-Repl/P2ReplUtils/Include/P2ReplDS.h"
#include "../../../Plaza-II-Repl/P2ReplUtils/Include/P2ReplDSDriver.h"

//////////////////////////////////////////////////////////////////////////
// IP2ReplDS
/*
Интерфейс IP2ReplDS используется сервером репликации для абстрагирования от конкретного хранилища данных
Это позволяет серверу репликации работать с P2DB, с P2Tbl и даже с кастомным хранилищем данных, реализованным в P2ReplPlayer.
Интерфейс реализует только методы для чтения из хранилища. Все изменения данных в нём проходят мимо IP2ReplDS. Обычно внесение
изменений реализуется с помощью функций P2ReplServerPostOnlineFromRevRangeInt, которые регистрируют изменения в 
таблицах, дабы во время вызова P2ReplServerCommitInt запросить из хранилища изменившиеся данные.

Таким образом интерфейс реализует функции чтения информации из хранилища. Каждый зарегистрированный сервер репликации имеет свой
интерфейс IP2ReplDS, хотя на одном объекте может висеть несколько серверов. К каждому объекту, реализующему IP2ReplDS жестко привязан один коннекшн к базе. 
Коннекшн скрывается под типом P2REPL_DS_CONN_HANDLE и должен быть уникальным в пределах приложения. Сейчас в качестве коннекшна используется приведенный 
к типу P2REPL_DS_CONN_HANDLE коннекшн к P2DB или P2Tbl базе. Сервер репликации использует метод CheckMyConn для проверки своей привязанности
к данному коннекшну.

Один объект, реализующий IP2ReplDS, может быть привязан к нескольким серверам репликации. Для оптимизации работы конкретного сервера
можно создать предкомпилированные объекты, привязанные к каждому серверу. Это делается в методе InitServer, который получает фильтрационную
схему сервера. Метод может вытащить из неё необходимые данные (имена полей, смещения в структуре итд) и вернуть указатель на объект с данными в параметре
ppObj. Этот параметр хранится как указатель в сервере репликации и должен быть разрушен в методе DeinitServer. Указатель на обхект передается во 
все методы интерфейса IP2ReplDS и может быть использован для ускорения выборки данных из хранилища.

Главный метод интерфейса - это метод EnumRecords. Метод вызывает переданную ему функцию cb для всех записей, присутствующих в таблице со схемой tblSch в диапазоне ревижнов
от lowRev до последней записи. Если методу передан флаг REPLDS_ENUM_FLAGS_HAS_UPPER, то диапазон выдаваемых записей меняется на диапазон от lowRev до highRev. 
Флаг REPLDS_ENUM_FLAGS_SKIP_DELETED требует пропускать удаленные записи (TBD: этот флаг может подниматься только для выдачи снапшота клиенту, не получающему удаленные записи). 
Для оптимизации выдачи снапшота в метод передается адрес клиента в параметре from (в онлайне там NULL) и текущий акл в параметре hAcl. Как и все енумы, этот должен
уметь обрабатывать P2ERR_COMMON_CANCEL_ENUM, возвращаемый из коллбэка

Четыре метода интерфейса служат задачам выборки ревижнов из таблиц. Методы принимают на вход хэндл схемы таблицы и возвращают требуемый ревижн.
Метод GetFirstTableRevision возвращает первый ревижн, присутствующий в таблице.
Метод GetFirstNonDeletedTableRevision возвращает первый ревижн, присутствующий в таблице, у которого в поле act находится ноль. Метод аналогичен предыдущему, но служит для пропуска удаленных записей.
Метод GetFirstTableRevForOnlineServer возвращает первый ревижн, присутствующий в таблице, но возможно отсутствующий в данной таблице. Этот метод актуален для онлайн серверов.
Метод GetNextTableRevision возвращает ревижн, который будет назначен следующей записи в таблице.

Для проверки поддержки онлайн-сервера для таблицы есть метод IsOnlineServerSupported.
Информационный метод GetScheme возвращает схему хранилища.

TBD GetHost

Карта вызовов
DeinitServer вызывается из P2ReplServer::Deconfig
InitServer вызывается из P2ReplServer::ConfigDB
GetScheme вызывается из P2ReplServer::ConfigDB
CheckVersion вызывается из P2ReplServer::ConfigDB

CheckMyConn вызывается из методов, которые осуществляют глобальные операции типа выдачи онлайна.
	P2ReplServer::InitComposeOnlineX
	P2ReplServer::FinishComposeOnlineX
	P2ReplServer::PostOnlineFromChangedTablesX
	P2ReplServer::PostOnlineClearOldRevX
	P2ReplServer::HaveOnlineChannels

EnumRecords логично вызывается строго из двух мест: из композа снапшота и онлайна.
	P2ReplServer::ComposeTableOnlineX
	ReplChannel::ComposeOneTableSnapshot

GetNextTableRevision вызывается там, где нужно передать клиенту последние ревижны или сравнить их с клиентскими
	ReplChannel::ComposeOneTableSnapshot для передачи клиенту последнего ревижна, лежащего в базе
	ReplChannel::CalcDataSizeFromRevs оценка объема недостающих у пользователя данных

Получения первых ревижнов в основном производятся из двух функций: CheckClientRevisonsX, где проверяется, можно ли 
пускать клиента на онлайн-сервер и GetClearDeletedRevisionsX, где по базе определяется ревижн, ниже которого
нужно зачищать клиента. В этих же функциях используется IsOnlineServerSupported для выбора метода.

GetFirstTableRevision
	ReplChannel::CheckClientRevisonsX
	ReplChannel::GetClearDeletedRevisionsX
GetFirstNonDeletedTableRevision
	ReplChannel::GetClearDeletedRevisionsX
GetFirstTableRevForOnlineServer
	ReplChannel::CheckClientRevisonsX
	ReplChannel::GetClearDeletedRevisionsX
IsOnlineServerSupported
	ReplChannel::CheckClientRevisonsX
	ReplChannel::GetClearDeletedRevisionsX

*/
//////////////////////////////////////////////////////////////////////////
#define REPLDS_ENUM_FLAGS_HAS_UPPER 1
#define REPLDS_ENUM_FLAGS_SKIP_DELETED 2
//////////////////////////////////////////////////////////////////////////

class IP2ReplDSVersion
{
public:
	enum { 
		MAJOR_VERSION = 3,
		MINOR_VERSION = 0 
	};
};

class IP2ReplDS : public IRc, public IP2ReplDSVersion 
{
public:
	P2ERR CheckVersion(UINT32 reqMajorVersion, UINT32 reqMinorVersion, char reqBuild);

	virtual ~IP2ReplDS() { };
	virtual P2ERR InitServer(P2DB_SCHEME_HANDLE hFilterScheme, void** ppObj) = 0;
	virtual P2ERR DeinitServer(void* pObj) = 0;
	virtual P2ERR GetHost(P2DSHost**) = 0;
	virtual P2ERR GetScheme(P2DB_SCHEME_HANDLE* pSch) = 0;
	virtual bool  CheckMyConn(P2REPL_DS_CONN_HANDLE conn) = 0;

	virtual P2ERR EnumRecords(void* pObj, P2ACL_HANDLE hAcl, PCSTR from, P2DB_TABLE_SCHEME_HANDLE tblSch, P2REPL_REVISION_TYPE lowRev, P2REPL_REVISION_TYPE highRev, UINT32 flags, P2REPL_DS_ENUM_FUNC cb, void* cbParam) = 0;

	virtual P2ERR GetFirstTableRevision(void* pObj, P2DB_TABLE_SCHEME_HANDLE tblSch, P2REPL_REVISION_TYPE *pRev) = 0;
	virtual P2ERR GetFirstNonDeletedTableRevision(void* pObj, P2DB_TABLE_SCHEME_HANDLE tblSch, P2REPL_REVISION_TYPE *pRev) = 0;
	virtual P2ERR IsOnlineServerSupported(void* pObj, P2DB_TABLE_SCHEME_HANDLE tblSch, bool* pSupported) = 0;
	virtual P2ERR GetFirstTableRevForOnlineServer(void* pObj, P2DB_TABLE_SCHEME_HANDLE tblSch, P2REPL_REVISION_TYPE *pRev) = 0;
	virtual P2ERR GetNextTableRevision(void* pObj, P2DB_TABLE_SCHEME_HANDLE tblSch, P2REPL_REVISION_TYPE *pRev) = 0; 
	virtual P2ERR SetNextTableRevision(void* pObj,P2DB_TABLE_SCHEME_HANDLE tblSch,P2REPL_REVISION_TYPE rev) = 0;

	// Функция должна возвращать уникальный идентификатор коннекшена к БД
	// Либо P2DB_CONN_HANDLE, либо this для ISAM-подобных коннекшенов
	virtual void* GetUniqueID() = 0;
    virtual UINT32 GetCapabilities() = 0;
};

#define IREPLDS_CAPABILITY__NULL_FIELDS         0x0001 // драйвер поддерживает nulls
#define IREPLDS_CAPABILITY__DEFVALUES_IN_FSCH   0x0002 // драйвер поддерживает default поля в фильтр-схеме


inline P2ERR IP2ReplDS::CheckVersion(UINT32 reqMajorVersion, UINT32 reqMinorVersion, char reqBuild)
{
	#ifdef _DEBUG
		char build = 'd';
	#else
		char build = 'r';
	#endif
	if (reqBuild != build)
	{
		P2LogError("Build mismatch: we want build '%c',was loaded '%c'", reqBuild, build);
		_ASSERTE(("Вадим, ты опять смешал релизные и дебажные модули?",0));
		return P2ERR_COMMON_WRONG_BUILD;
	}
	if ((reqMajorVersion == MAJOR_VERSION) && (reqMinorVersion <= MINOR_VERSION))
	{
		return P2ERR_OK;
	}
	else
	{
		P2LogError("Version mismatch: we want module v.%02d.%02d,was loaded v.%02d.%02d", MAJOR_VERSION, MINOR_VERSION, reqMajorVersion, reqMinorVersion);
		_ASSERTE(("Вадим, ты опять забыл перекомпилировать _ВСЕ_ модули?",0));
		return P2ERR_COMMON_INVALID_INTERFACE_VERSION;
	}
}

#endif  // _IPREPLDB_H_
