// interface IRouter
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IROUTER_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_)
#define AFX_IROUTER_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II/P2Common/Include/P2Types.h"
#include "../../../Plaza-II/P2Utils/Include/ServiceDriver.h"
#include "IMsgConnection.h"
#include "ILLMsgStackProc.h"
#include <set>
#include "IModuleServices.h"

//////////////////////////////////////////////////////////////////////
// IRouter
/* Способы задания адресов:
 * Общая форма (1) NODE.SERVICE
 * Сокращенная форма (2) .SERVICE
 * Сокращенная форма (3) NODE
 */

// коллбэк для перечисления соединений. если возвращает что-то отличное от P2ERR_OK, то перечисление прекращается
// внимательнее с операциями в коллбэке, возможен дедлок.
typedef P2ERR (P2CALLSPEC *pfnEnumConn)(const char *name, IMsgConnection *pIMsgConn, bool isDefault, bool isLocal, void *pdata);

struct IRouter 
{
	// битовая маска типов адресов
	enum AddressType
	{
		ADDR_TYPE_FULL = 0x01,
		ADDR_TYPE_SERVICE_ONLY = 0x02,
		ADDR_TYPE_NODE_ONLY = 0x04
	};

	enum NodeType
	{
		NODE_TYPE_CLIENT,
		NODE_TYPE_SERVER,
		NODE_TYPE_ROOT

	};

	// ключ коннекшена. По ключу можно получить коннекшен.
	typedef UINT64 ConnectionKey;

    // Используется также комбинация типов, поэтому биты не должны пересекаться.
    enum ConnectionType
    {
		// реальные типы соединений. Используются в GetRoute
        CONN_TYPE_DEF_UP    = 0x01,
        CONN_TYPE_DEF_DOWN  = 0x02,
        CONN_TYPE_DIRECT    = 0x04,
        CONN_TYPE_LOCAL     = 0x08,
		// фиктивный тип отсутствия роутинга
        CONN_TYPE_UNKNOWN   = 0x80,
		//две следующие маски используются только в PostBroadcast
		CONN_TYPE_INCOMING  = 0x100,
		CONN_TYPE_OUTGOING  = 0x200
    };
    enum { CONN_TYPE_ALL = 0xff, CONN_TYPE_ALL_DIRECTIONS = 0x300 };

    // Router service
	// Способы задания адресов (1), (2)
	// disable : запрет роутинга в определённые адреса; маска типов коннешенов (connectionType), куда запрещается роутить 
	// в наст. время поддерживается запрет роутинга на директные линки и на дефолтный аплинк
    virtual P2ERR GetRoute(const char *address, IMsgConnection **ppIMsgConn, ConnectionType *pType = 0, UINT8 disable = 0) const = 0;

	// Способы задания адресов (3)
	virtual void GetMyAddress(char (&address)[P2_MAX_POSSIBLE_ADDRESS_SIZE + 1]) const = 0;
	// получить мой адрес вида <mynodename>.router.  pLength -- размер  получившегося адреса с учётом завершаюего нуля
	virtual P2ERR GetMyFullMqAddr(char* pAddressBuffer, size_t *pLength) const = 0;
	virtual P2ERR CheckAddressType(const char *address, UINT32 typeBitMask)const = 0;

	virtual P2ERR GetRouterByAddress(const char *original_address, char *router_address, size_t len)const = 0;
	virtual P2ERR GetConnectionAddress(IMsgConnection *pIMsgConn, char *address, size_t len,/*hint for searching*/ConnectionType connTypeHint)const = 0;

	virtual enum NodeType GetMyType(void) const = 0;
	// посылка сообщения по адресу. сокращение для GetRoute+Post
	/*
		логика, связанная с введением флагов:
		1. флаги передаются дальше в IMsgConnection::Post
		2. если выставлен P2LLMSG_FLAG_TO_ROUTE _И_ address == NULL, то address извлекается из тела
		3. логика извлечения адреса из тела зависит от флага P2LLMSG_FLAG_P2MSG
		4. если флаг P2LLMSG_FLAG_P2MSG выставлен, то адрес берется из поля to
		5. если флаг не выставлен, то тело парсится по форматной строке "s...."
	*/
    virtual P2ERR Post(const char *address, UINT32 system_type, const void* pBody, size_t len, UINT32 flags)const = 0;
	/* Post функция, аналогичная предыдущей;  в объекте pIBytes хранится LLMsg.  */ 
	virtual P2ERR Post(const char *address, IBytesWLenPriority* pIBytes) const = 0;

	//  Отправить сообщение через соединение, заданное ключом connKey.
	//  Если соединения с таким ключом нет(например, ключ равен нулю), то отправить через роутинг на address. Если address == NULL, то вернуть ошибку NO_ROUTE
	virtual P2ERR PostSync(const char* pAddrTo, IBytesWLenPriority* pIBytes, UINT64 connKey) = 0;
	
	// получить ключ соедиенения
	virtual UINT64 GetConnectionKey(const IMsgConnection* pIMsgConn) const = 0; 

	// сервисная процедура для упрощения роутинга LL-сообщений
	// Проверяется адрес address. Если это наш адрес (точнее его часть "имя узла"), то возвращается P2ERR_OK
	// Если нет, то сообщение посылается по роутингу. В этом случае код возврата  P2ERR_POSTED_TO_FOREIGN_ADDRESS, если все послалось или ошибка от поста
	virtual P2ERR PostIfForeignAddress(const char *address, UINT32 system_type, const void* pBody, size_t len, UINT32 flags)const = 0;

    // Запрос состояния модуля по приему входящих соединений
    virtual bool IsInLocLinksEnabled() 	const = 0;
    virtual bool IsInRemLinksEnabled()	const = 0;

	/*получить количество исходящих соединений, которые должен установить роутер. Читается из инишника */
	virtual UINT32 GetOutgoingLinksCount() const = 0;

    // Изменение (установка(login)/очистка(logout)) имени узла
	// - если имя непустое, то считать секцию из инишника
	//   и запустить на коннект исходящие линки
	// - с пустым именем активизировать закрытие ремутных линков
	virtual P2ERR SetNodeName(const char *address, UINT32 loginId = 0) = 0;

    // Функция для встраивания обработчика LLMsg-ей в стек.
    // Если модуль хочет быть встроен, он должен вернуть адрес интерфейса,
    // если не хочет - возвращает 0.
    virtual ILLMsgStackProc* GetLLMsgStackProc() = 0;

	virtual P2ERR ParseAddress(const char *addr, char *node, size_t *pNodeLen, char *service, size_t *pServiceLen)const = 0;

    /* Блокирует роутерную критическую секцию и из под нее вызывает CallBack-и, что довольно таки опасно...
       Можно переписать с использованием локальной копии необходимой роутерной инфы,
       но пока вместо этого достаточно будет функции PostBroadcast()
	virtual P2ERR EnumConnections(pfnEnumConn fn, void *pdata)const = 0;*/
    virtual P2ERR PostBroadcast(UINT32 system_type, const void* pBody, size_t len, UINT32 flags, ConnectionType connMask)const = 0;

	// сравнение адресов (expandAddr указывает, нужно ли уточнять адреса перед сравнением), выдает равенство каждой части адреса.
	virtual P2ERR CompareAddresses(const char *addr1, const char *addr2, bool expandAddr, bool *nodesEq, bool *appsEq)const = 0;

    virtual P2ERR GetConnectionType(IMsgConnection *pIMsgConn, ConnectionType* pType)const = 0;

	virtual P2ERR RegisterVirtualConnection(IMsgConnection* pIMsgConn, const char* oposSvcAddr) = 0;
	virtual void UnRegisterVirtualConnection(IMsgConnection* pIMsgConn) = 0;

    // ".a" -> "<node>.a"
    virtual P2ERR MakeFullAddress(const char *src, char *dest, /*[in,out]*/ size_t* pDestSize) const = 0;
	virtual void SetRouterAdmins(const char *src) = 0 ;
	virtual P2ERR GetRouterAdminsStr(char* buf, size_t bufsz) const = 0 ;
    virtual bool isAdmins(const std::string& node_name) const = 0 ;
	virtual P2ERR GetSrcDestFromLLMsg(const UINT32 flags, size_t len,const BYTE* pDta, const char** src, const char** dest) const = 0;
    CHECK_FOR_DELETE__(IRouter)
};



bool RouterRoute_GetService(IModuleServices* pIModuleServices, IRouter** ppIRouter); 
void RouterRoute_FreeService(IRouter* pRouter);

P2ERR LLMsgRoutePingReply_SecurityCheck(const void* body, size_t size, const char* pAddrFrom);


#endif // !defined(AFX_IROUTER_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_)

/*

2nd version
-----------

// Общая информация:
// -----------------
// - Успешным логином роутера считается первая успешная аутентификация соединения
//
// Реализация:
// -----------
// Вызовы, производимые из единственного управляющего потока:
//  - IGlobalAuth::
//      ReadConfiguration()
//      SetInfoFromUser()
//  - IRouter::
//      SetNodeName()
//  - методы зарегистрированных в модуле обработчиков ILinkNotifHandler
//
// - При получении в юзеровский модуль запроса на логин из локального соединения
//   создается объект, реализующий ILoginCallback, в котором упаковываются данные:
//      - IMsgConnection* локального соединения
//      - #login-a
//   Затем IRc-указатель на этот объект + аут.инфа постится в управляющий поток,
//   где производится вызов IGlobalAuth::SetInfoFromUser(). В случае ошибки
//   производится вызов ILoginCallback->OnFailure().

----------------------------------------------------------------------------------
- алгоритм ремутного логина
-- by INI
* создание глобального роутера
	* роутер подключается к нотификатору о статусах соединений
* создание глобального аутф
	* аутф подключается к нотификатору о статусах соединений
* вызов TryToConnect
	* .ReadConfiguration
	* если полученное имя непуcтое, то 
		* вызвать IRouter::SetNodeName
			* в случае непустого имени просчитать все линки и создать коннекшны
...
--- by user's login
* вызов TryToConnectWithUserInfo с коллбеком из user 
	* .SetUserInfo с коллбеком из user 
		* если имя уже есть то выдать ошибку
	* если полученное имя непуcтое, то 
		* вызвать IRouter::SetNodeName
			* в случае непустого имени просчитать все линки и создать коннекшны
* по установке первого соединения
	* вызвать коллбэк
----------------------------------------------------------------------------------

struct ILoginCallback : public IRc
{
    virtual void OnSuccess() = 0;
    virtual void OnFailure(P2ERR reason) = 0;
};

struct IAuthService
{
	// - прочитать INI
    // - если удалось причитать аутентификационную информацию, то вызвать
    //   IRouter::SetNodeName(const char* pNodeName)
	virtual P2ERR ReadConfiguration() = 0;
	// Аутентификация по внешней команде
    // - если уже имеется аут.инфа, то вернуть ошибку
    // - разобрать блок аутентификационных данных
    // - в случае успеха вызвать IRouter::SetNodeName(const char* pNodeName)
    // - если эта функция вернет OK и pILoginCb != 0, то будет обязательно
    //   вызван один из методов ILoginCallback!
	virtual P2ERR SetInfoFromUser(void *auth_blob, UINT32 len, ILoginCallback* pILoginCb) = 0;

    // Фабрика пер-коннектных обработчиков
	virtual P2ERR CreateConnectionCryptoLayer(IConnCrypto** ppIConnCrypto,
                                IConnCryptoContainer* pIContainer, IConnection* pIConn) = 0;
};

struct IConnCrypto : public IRc
{
    //
    // Обработка событий соединения
    //

    // - параметры соединения (исходящее/входящее) можно получить из IConnection
    // - признак isDefUpLink передается при создании этого объекта в фабрику
	virtual void OnConnectComplete(ICs* pICsCaller) = 0;
	virtual void OnClose(int error, ICs* pICsCaller) = 0;

    //
    // Шифрование
    //

    virtual P2ERR Push(const char* pData, UINT32 len, ICs* pICsCaller) = 0;
    // (Надо/можно вызывать до тех пор, пока вызов не вернет *pRealLen == 0)
    virtual P2ERR Pop(char* pData, UINT32 maxLen, UINT32 *pRealLen, ICs* pICsCaller) = 0;
    // (Для некоторого увеличения эффективности, после вызова длина может
    //  увеличиться)
    virtual UINT32 GetPopLen() = 0;
};

struct IConnCryptoContainer : public IRc
{
	virtual void OnLoginSucceeded(bool isLocal,
                                  bool isOposDefault, const char *oposAddr,
                                  ICs* pICsCaller) = 0;
    // Вызывается, когда текущая аут.инфа некорректна, при этом соединение шифровальщиком
    // не закрывается (будет закрыто контейнером).
    // В случае других проблем шифровальщик просто закрывает соединение и не вызывает
    // контейнер.
	virtual void OnLoginFailed(P2ERR error, ICs* pICsCaller) = 0;
};

struct IRouter
{
    // ...

    // Запрос состояния модуля по приему входящих соединений
    virtual bool IsInLocLinksEnabled() = 0;
    virtual bool IsInRemLinksEnabled() = 0;

    // Изменение (установка(login)/очистка(logout)) имени узла
	// - если имя непустое, то считать секцию из инишника
	//   и запустить на коннект исходящие линки
	// - с пустым именем активизировать закрытие ремутных линков
	virtual P2ERR SetNodeName(const char *address) = 0;
};

//struct IConnection : ...
//{
//    // ...
//
//    virtual bool IsDefUpLink() = 0;
//};

struct IMsgConnection : ...
{
    // ...

    virtual bool IsDefUpLink() = 0;
};

struct ILinkNotifHandler : public IRc
{
    // - There are no OnOutLocalConnectComplete!!!!
    // - Способы задания адресов (1), (3). Сервис игнорируется.
    // - направление коннекта в IMsgConnection
    // - своя дефолтность получается из IMsgConnection (нужна только для исходящих ремутных)
    // - чужая дефолтность нужна только для входящих ремутных
    virtual void OnLoginSucceeded(IMsgConnection* pIMsgConn, bool isLocal,
                                  bool isOposDefault, const char *oposAddr,
                                  ICs* pICsCaller) = 0;
	virtual void OnLoginFailed(IMsgConnection* pIMsgConn, P2ERR error, ICs* pICsCaller) = 0;
	virtual void OnClose(IMsgConnection *pIMsgConn, int error,
                         ICs* pICsCaller) = 0;
};

struct IModuleServices
{
    // ...

    // Обработчики link-событий
    virtual void RegisterLinkNotifHandler(ILinkNotifHandler* pILinkNotifHandler);
    virtual void UnRegisterLinkNotifHandler(ILinkNotifHandler* pILinkNotifHandler);
};

*/
