// interface IConnCrypto
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICONNCRYPTO_H__B5D645EA_CEAC_4351_BE51_AD7315F843E6__INCLUDED_)
#define AFX_ICONNCRYPTO_H__B5D645EA_CEAC_4351_BE51_AD7315F843E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II/P2Utils/Include/IRc.h"
#include "../../../Plaza-II/P2Utils/Include/ICs.h"
#include "ConnLUID.h"
#include "IBytesProtPtrList.h"

//////////////////////////////////////////////////////////////////////
// IConnCrypto
struct IConnectionHostsInfo;
struct IConnCrypto : public IRc
{
    typedef conn::LUID LUID;

    virtual LUID GetLUID() const = 0;
	virtual bool IsOutgoing() const = 0;

    virtual P2ERR Connect(ICs* pICsCaller) = 0;
	virtual void Disconnect(bool setDead, bool graceful, ICs* pICsCaller) = 0;

    // Send copy of data
	virtual P2ERR PostSync(const void* pData, size_t len, BYTE priority, ICs* pICsCaller) = 0;
	// Send copy of data
	/* метод исключен из интерфейса по причине невостребованности и неэффективности
    virtual P2ERR PostCopySync(const IBytesWLenPriority* pIBytes, ICs* pICsCaller) = 0;
	*/
    // Send this instance (don't use pIBytes after call, only Release1()!)
    virtual P2ERR PostThisSync(IBytesWLenPriority* pIBytes, ICs* pICsCaller) = 0;

    virtual P2ERR PostThisSync(IBytesPriorProtPtrList* pIDL, ICs* pICsCaller) = 0;

    virtual bool IsDefUpLink() const = 0;

	virtual IConnectionHostsInfo* GetConnectionHostsInfo() = 0;

    CHECK_FOR_DELETE__(IConnCrypto)
};

/*struct IConnCrypto : public IRc
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
    //  - Если возникла какая-то ошибка, то шифровальщик сам закрывает соединение.
    //    Т.е. если какой-либо из методов шифровальщика вернул P2ERR-ошибку, то
    //    вызывающая сторона ничего не делает, просто прекращает обработку в надежде
    //    позже получить извещение OnClose(). Все данные, посутпающие из этого соединения
    //    будут проигнорированы (и не переданы шифровальщику!) вплоть до реконнекта.
    //    (См. также комментарии к IConnCryptoContainer - в случае LoginFailed
    //     шифровальщик закрывать соединение не должен)
    //

    // - *pPopLen - после вызова содержит длину данных, которые можно извлечь через Pop()
    virtual P2ERR Push(const void* pData, UINT32 len, UINT32* pPopLen, ICs* pICsCaller) = 0;
    // - maxLen - размер буффера pData
    // - *pRealLen - после вызова содержит длину данных, записанных в pData
    // - *pRemLen - после вызова содержит длину данных, которые можно еще извлечь через Pop()
    // (Надо/можно вызывать до тех пор, пока вызов не вернет *pRemLen == 0)
    virtual P2ERR Pop(void* pData, UINT32 maxLen, UINT32* pRealLen, UINT32* pRemLen,
                      ICs* pICsCaller) = 0;
    // (После вызова длина может увеличиться)
    virtual UINT32 GetPopLen() = 0;

    CHECK_FOR_DELETE__(IConnCrypto)
};*/

#endif // !defined(AFX_ICONNCRYPTO_H__B5D645EA_CEAC_4351_BE51_AD7315F843E6__INCLUDED_)
