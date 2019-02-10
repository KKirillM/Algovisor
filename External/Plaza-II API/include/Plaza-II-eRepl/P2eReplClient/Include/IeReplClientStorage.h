#ifndef _IREPLCLIENTSTORAGE_H
#define _IREPLCLIENTSTORAGE_H

#include "../../../Plaza-II/P2Utils/Include/IRc.h"
#include "../../../Plaza-II/P2Utils/Include/ServiceDriver.h"
#include "../../../Plaza-II/P2SysRecord/Include/P2SysRecord.h"
#include "../../P2eReplCommon/Include/P2eReplTypes.h"

/*
 * IStreamTools - интерфейс, содержащий полезные методы стрима для вызова из провайдера
 */
class IStreamTools
{
public:
	// получение текущего состояния канала в виде макроса семейства P2EREPL_DATASTREAM_STATE
	virtual UINT32 GetState() = 0;
};

class P2eReplCliHostControl
{
public:
	enum { 
		MAJOR_VERSION = 1,
		MINOR_VERSION = 0 
	};
};

const UINT32 cMajorVer = 8;
const UINT32 cMinorVer = 0;

////////
//interface IReplClientStorage declaration
// функционал, реализуемый IReplClientStorage
// 1. обработка клиентской базы: создание, пересоздание и добавление-удаление записей
// 2. поддержка актуальных ревижнов записей в таблицах
// 3. вызов клиентской нотификации
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Внимание! При изменении структуры необходимо поднимать версию интерфейса IeReplClientStorage
struct TNotifData
{
    P2ERR                    m_reason;
    PCSTR                    m_pRedirectServer;
    UINT32                   m_lifeNum;
    P2EREPL_REVISION_TYPE    m_clearRev;
    UINT32                   m_clearRevFlags;

    TNotifData() : m_reason(P2ERR_OK), m_pRedirectServer(NULL), m_lifeNum(0), m_clearRev(P2EREPL_MIN_REVISION), m_clearRevFlags(0)
    {
    };
};

struct IeReplClientStorage : public IRc, public IMQPluginImpl<cMajorVer, cMinorVer, IeReplClientStorage>
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
	 * + OnChannelClose - канал был закрыт. В этом методе освобождаются ресурсы, связанные с базой
	 * + OnReconnect - от сервера было получено сообщение RECONNECT. Параметр clear - игнорировать!
	 * + OnError - вызывается в случае перехода в состояние ошибки.
	 */
	virtual P2ERR OnChannelOpen() = 0;
	virtual P2ERR OnChannelOpenFailed(UINT32 reason) = 0;
	virtual P2ERR OnChannelClose(P2ERR reason) = 0;
	virtual P2ERR OnReconnect() = 0;
	virtual P2ERR OnError(P2ERR err) = 0;

    virtual P2ERR OnLifeNumChanged(UINT32 serverLifeNum) = 0;

	/* Этот набор методов отвечает за поддержку ревизижнов. Возможно два варианта ведения ревижнов:
	 * - провайдер хранилища сохраняет данные в базу и автоматически создает-заполняет-пересоздает её.
	 * - провайдер не поддерживает запоминание ревижнов. В этом случае запрашиваемыми ревижнами управляет пользователь
	 * + HasPersistRevisions - метод возвращает true, если провайдер сохраняет ревижны
	 * + DoInitRevisions
	 * + DoComposeRevisionsToMsg - метод композит тело сообщения текущими ревижнами.
	 */
	/*virtual bool HasPersistRevisions() = 0;
	virtual P2ERR DoInitRevisions(P2DB_SCHEME_HANDLE hScheme, const P2REPL_REVISION_TYPE* pRevisions, size_t rev_count) = 0;
	*/

    virtual P2ERR DoInitRevision(P2EREPL_REVISION_TYPE rev) = 0;
    virtual P2ERR DoComposeRevisionToMsg(P2MSG_HANDLE hMsg) = 0;
    virtual P2ERR AdjustRevision(P2EREPL_REVISION_TYPE lastRev, UINT32 flags) = 0;

    /* Набор методов для поддержки транзакции. 
	 * Единственная неочевидная деталь: в DoRollback передается флаг, определяющий нужно ли
	 * делать rollback уровня БД
	 */
	virtual P2ERR DoBeginTransaction() = 0;
	virtual P2ERR DoCommit() = 0;
	virtual P2ERR DoRollback() = 0;

	// функция DoProcessEvent - обработчик пришедших событий.
	virtual P2ERR DoProcessEvent(const TEReplRec* pEvent) = 0;
	// DoClearOldRev - урезание ревижнов по таблице до указанного
	virtual P2ERR DoClearOldRev(P2EREPL_REVISION_TYPE rev, UINT32 flags) = 0;
	// DoNotify вызывает клиентскую нотификацию с нотификационным сообщением
	virtual void DoNotify(UINT32 notif, const TNotifData* pNotifData) = 0;
    // DoFastNotify вызывает клиентскую нотификацию для часто возникающих нотификаций (без композинга нотификационного сообщения)
    virtual void DoFastNotify(UINT32 notif, const TEReplRec* pRec = NULL) = 0;

    /* Команда на удаление предыдущей базы (по предыдущей схеме базы) */
    //virtual P2ERR DoClearStorage(P2DB_SCHEME_HANDLE hScheme) = 0; // В этом методе удаляется/очищается старая база.
    /* Команда на создание базы по схеме */
    //virtual P2ERR DoCreateStorage(P2DB_SCHEME_HANDLE hScheme) = 0; // Здесь создается новая база по необходимости.

    virtual P2ERR DoClearStorage() = 0; // В этом методе удаляется/очищается старая база.

    virtual void SetIStreamTools(IStreamTools *pStreamTools) = 0;
};

#endif