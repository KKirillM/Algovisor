// interface IMsgConnection
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMSGCONNECTION_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_)
#define AFX_IMSGCONNECTION_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II/P2Common/Include/P2Types.h"
#include "../../../Plaza-II/P2Utils/Include/IRc.h"
#include "../../../Plaza-II/P2Utils/Include/ICs.h"
#include "ConnLUID.h"
#include "IBytesWLen.h"
#include "LLMsg.h"

//////////////////////////////////////////////////////////////////////
// IMsgConnection

struct ILoginCallback;

struct IMsgConnection : public IRc
{
    typedef conn::LUID LUID;

    enum TIsLocal
    {
        LOCAL_YES,
        LOCAL_NO,
        LOCAL_UNKNOWN
    };

    virtual LUID GetLUID() const = 0;
	virtual UINT64 GetConnectionKey() /*const*/ = 0;  //TODO: добавить const убрать UINT64
	virtual bool IsOutgoing() const = 0;
	virtual const char* GetHost(char *host,size_t sz) const = 0;
	virtual UINT GetPort() const = 0;
    virtual bool IsDefUpLink() const = 0;

// <- Transferred to handler(s) after an establishment of connection
//    virtual TIsLocal IsLocal() = 0;

    // - Заполняется сразу после завершения аутентификации. TODO: check this  comment
    // - До заполнения - пустая строка ("")
    virtual void GetName(PSTR name, UINT32 size) const = 0;


    virtual P2ERR Connect(ICs* pICsCaller) = 0;
	virtual void Disconnect(bool setDead, bool graceful, ICs* pICsCaller) = 0;
	/*
		Мы таки их переименовали! 
		PostAsync - методы, которые осуществляют скорее всего асинхронную посылку LLMsg. Они дедлоко-безопасны для вызова из любого числа захваченных CS, 
					но увеличивают latency на проброс вызова в соседний поток пула. Ответственность за дедлок на реализзаторе метода
		PostSync - методы, которые скорее всего выполняют синхронную посылку в потоке, откуда метод был вызван. Методы показывают минимальный latency, 
					но могут увеньшить capacity. Ответственность за дедлок на вызывающем.
	*/
	virtual P2ERR PostSync(IBytesWLenPriority* pIBytes,ICs* pICsCaller) = 0;
	virtual P2ERR PostSync(UINT32 system_type, const void* pBody, size_t len, ICs* pICsCaller, UINT32 flags) = 0;
    virtual P2ERR PostAsync(UINT32 system_type, const void* pBody, size_t len, UINT32 flags) = 0;
	virtual P2ERR PostAsync(IBytesWLenPriority* pIBytes) = 0;

    // Асинхронный дисконнект
    // (этот вызов не блокирует CS, а постит реальный вызов Disconnect()-a
    //  в другой поток из пула)
	virtual void DisconnectPost(bool setDead, bool graceful) = 0;

    // Вызывается когда данное соединение пытается залогинить роутер.
    // Внимание! Несмотря на то, что метод не содержит параметра ICs*, внутри происходит
    // блокирование критической секции - метод не должен вызываться из под другой CS!
    virtual void OnLoginFrom_(ILoginCallback* pILoginCB, IBytesWLen* pIAuthData) = 0;

    // Проверка валидности соединения в настоящий момент.
    // Внимание! После вызова функции состояние соединения может измениться!
    virtual BOOL IsValid() const = 0;

    CHECK_FOR_DELETE__(IMsgConnection)
};

#endif // !defined(AFX_IMSGCONNECTION_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_)
