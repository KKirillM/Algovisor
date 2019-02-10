#ifndef _IREPLCLIENTSTORAGE_H
#define _IREPLCLIENTSTORAGE_H

#include "../../../Plaza-II/P2Utils/Include/IRc.h"
#include "../../../Plaza-II/P2Utils/Include/SmartIRc.h"
#include "../../../Plaza-II/P2Utils/Include/ServiceDriver.h"
#include "../../../Plaza-II/P2SysRecord/Include/P2SysRecord.h"
#include "../../../Plaza-II-Repl/P2ReplCommon/Include/P2Repl.h"

/*
 * IStreamTools - интерфейс, содержащий полезные методы стрима для вызова из провайдера
 */
class IStreamTools
{
public:
	// получение текущего состояния канала в виде макроса семейства P2REPL_DATASTREAM_STATE
	virtual UINT32 GetState() = 0;
	// функции вызова пользовательской нотификации. 
	// такой вызов через задницу (полиси вызывает код из стрима, который в свою очередь вызывает код из полиси)
	// сделан с одной целью - разлочить критическую секцию перед вызовом нотификации и залочить её после возврата
	// функции возвращают true, если во время нотификации состояние канала не изменилось и false, если изменилось (например, канал был закрыт или рестартован)
	virtual bool NotifyAndCheckState(UINT32 notif, P2ERR reason = P2ERR_OK) = 0;
	virtual bool NotifyAndCheckStateEx(UINT32 notif, P2ERR reason, size_t tblIdx, const void* pBody = NULL, size_t bodySize = 0, P2DB_RECORD_HANDLE rec = NULL, bool sendNotif = true) = 0;
	// функции возвращают параметры повтора попыток записи в базу
	virtual UINT32 GetRetryCount(void) = 0;
	virtual UINT32 GetRetryTimeout(void) = 0;

	virtual void *GetRecordPtr(P2DB_RECORD_HANDLE rec) = 0;
	virtual P2REPL_REVISION_TYPE GetRecordRev(P2DB_RECORD_HANDLE rec) = 0;
};

class P2ReplCliHostControl
{
public:
	enum { 
		MAJOR_VERSION = 1,
		MINOR_VERSION = 0 
	};
};

const UINT32 cMajorVer = 10;
const UINT32 cMinorVer = 0;

////////
//interface IReplClientStorage declaration
// функционал, реализуемый IReplClientStorage
// 1. обработка клиентской базы: создание, пересоздание и добавление-удаление записей
// 2. поддержка актуальных ревижнов записей в таблицах
// 3. вызов клиентской нотификации
////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct IReplClientStorage : public IRc, public IMQPluginImpl<cMajorVer, cMinorVer, IReplClientStorage>
{
public:
	enum
	{
        MAJOR_VERSION = cMajorVer,
		MINOR_VERSION = cMinorVer 
	};

	/* Этот набор методов должен обрабатывать смену состояний клиента репликации
	 * + OnChannelOpen - сервер подтвердил открытие и прислал свою схему. Вызывается однократно в ответ на 
	 * получение сообщения от сервера. Обычно ничего не делает, ибо клиентская схема ещё неизвестна.
	 * + OnChannelOpenFailed - сервер отказал в доступе к потоку. Вызывается однократно в ответ на 
	 * получение сообщения от сервера. Обычно ничего не делает.
	 * + OnBeforeChangeLifeNum - клиент обнаружил необходимость сменить номер жизни. Вызывается однократно при
	 * сравнении схемы полученной при создании потока или переданной в SetScheme.
	 * + OnSetScheme - сервер согласовал и подтвердил отдачу по клиентской схеме.
	 * + OnSetSchemeFailed - сервер отказал в получении клиентской схемы
	 * + OnChannelClose - канал был закрыт. В этом методе освобождаются ресурсы, связанные с базой
	 * + OnReconnect - от сервера было получено сообщение RECONNECT. Параметр clear - игнорировать!
	 * + OnError - вызывается в случае перехода в состояние ошибки.
	 */
	virtual P2ERR OnChannelOpen(P2DB_SCHEME_HANDLE	hServerScheme) = 0;
	virtual P2ERR OnChannelOpenFailed(UINT32 reason) = 0;
	virtual P2ERR OnBeforeChangeLifeNum(P2DB_SCHEME_HANDLE hServerScheme, P2DB_SCHEME_HANDLE hScheme) = 0;
	virtual P2ERR OnSetScheme(P2DB_SCHEME_HANDLE hScheme) = 0;
	virtual P2ERR OnSetSchemeFailed(UINT32 reason) = 0;
	virtual P2ERR OnChannelClose(P2ERR reason) = 0;
	virtual P2ERR OnReconnect(P2DB_SCHEME_HANDLE hScheme, UINT8 /*clear*/ /*ignore it!*/) = 0;
	virtual P2ERR OnError(P2ERR err) = 0;


	/* Этот набор методов отвечает за поддержку ревизижнов. Возможно два варианта ведения ревижнов:
	 * - провайдер хранилища сохраняет данные в базу и автоматически создает-заполняет-пересоздает её.
	 * - провайдер не поддерживает запоминание ревижнов. В этом случае запрашиваемыми ревижнами управляет пользователь
	 * + HasPersistRevisions - метод возвращает true, если провайдер сохраняет ревижны
	 * + DoInitRevisions
	 * + DoComposeRevisionsToMsg - метод композит тело сообщения текущими ревижнами.
	 */
	virtual bool HasPersistRevisions() = 0;
	virtual P2ERR DoInitRevisions(P2DB_SCHEME_HANDLE hScheme, const P2REPL_REVISION_TYPE* pRevisions, size_t rev_count) = 0;
	virtual P2ERR DoComposeRevisionsToMsg(P2MSG_HANDLE hMsg) = 0;
	virtual P2ERR AdjustRevision(size_t tblIdx, P2REPL_REVISION_TYPE rev, UINT32 flags) = 0;

	/* Набор методов для поддержки транзакции. 
	 * Единственная неочевидная деталь: в DoRollback передается флаг, определяющий нужно ли
	 * делать rollback уровня БД
	 */
	virtual P2ERR DoBeginTransaction() = 0;
	virtual P2ERR DoCommit() = 0;
	virtual P2ERR DoRollback(bool rollbackDb = true) = 0;

	// функция DoProcessRecord - обработчик пришедщих записей.
	virtual P2ERR DoProcessRecord(size_t tblIndex, P2DB_RECORD_HANDLE rec) = 0;
	// DoClearDeleted - урезание ревижнов по таблице до указанного
	virtual P2ERR DoClearDeleted(P2DB_SCHEME_HANDLE hScheme, size_t tblIdx, P2REPL_REVISION_TYPE rev) = 0;
	// DoNotify вызывает клиентскую нотификацию
	virtual void DoNotify(UINT32 notif, P2ERR reason, size_t tblIdx = 0, const void* pBody = NULL, size_t bodySize = 0, 
						  P2DB_RECORD_HANDLE pRec = NULL, bool sendNotif = true, const char *redirectServer = "", 
						  void* pAddData = NULL) = 0;

	virtual P2ERR DoGetDBConn(P2DBConnection** ppDbConn) = 0;

	virtual UINT32 DoDecrementRetryCount() = 0;
	virtual void DoResetRetryCount() = 0;

	virtual void SetIStreamTools(IStreamTools *pStreamTools) = 0;

    /* Методы загрузки/сохранения параметров потока.
     * Реализуются только для базовых клиентов.
     * Для безбазовых клиентов буфер состояния передается в конструктор стрима, а получается отдельной функцией.
     */
    // LoadBinaryState: В случае отсутствия в базе информации о предыдущем состояние НЕОБХОДИМО делать (*pBufferSz = 0)
    virtual P2ERR LoadBinaryState(void * pBuffer, size_t* pBufferSz) = 0;
    virtual P2ERR SaveBinaryState(const void * pBuffer, size_t bufferSz) = 0;
    virtual size_t GetRevisions(const P2REPL_REVISION_TYPE** ppRevisions) = 0;

    /* Команда на удаление предыдущей базы (по предыдущей схеме базы) */
    virtual P2ERR DoClearStorage(P2DB_SCHEME_HANDLE hScheme) = 0; // В этом методе удаляется/очищается старая база.
    /* Команда на создание базы по схеме */
    virtual P2ERR DoCreateStorage(P2DB_SCHEME_HANDLE hScheme) = 0; // Здесь создается новая база по необходимости.
};

typedef CRcPtr< IReplClientStorage > CRcReplClientStorage;

#endif // _IREPLCLIENTSTORAGE_H
