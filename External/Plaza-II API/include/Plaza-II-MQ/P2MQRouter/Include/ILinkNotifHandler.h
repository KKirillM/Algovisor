// interface ILinkNotifHandler
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ILINKNOTIFHANDLER_H__53031B2D_1E9E_492F_B592_B7EEF41845A7__INCLUDED_)
#define AFX_ILINKNOTIFHANDLER_H__53031B2D_1E9E_492F_B592_B7EEF41845A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II/P2Utils/Include/IRc.h"
#include "../../../Plaza-II/P2Utils/Include/ICs.h"
#include "IMsgConnection.h"
#include "IModuleServices.h"
#include "IRouter.h"

/* Способы задания адресов:
 * Общая форма (1) NODE.SERVICE
 * Сокращенная форма (2) .SERVICE
 * Сокращенная форма (3) NODE
 * Сокращенная форма (4) SERVICE
 */

struct ILinkNotifHandler : public IRc
{
    // - There are no OnOutLocalConnectComplete!!!!
    // - направление коннекта в IMsgConnection
    // - своя дефолтность получается из IMsgConnection (нужна только для исходящих ремутных)
    // - чужая дефолтность нужна только для входящих ремутных
    // - Способы задания адресов 
	//	isLocal == true (2) (4)
	//	isLocal == false (1) (3)
//    virtual void OnLoginSucceeded(IMsgConnection* pIMsgConn, bool isLocal,
//                                  bool isOposDefault, const char *oposAddr/*,
//                                  ICs* pICsCaller*/) = 0;

    // 1ая фаза обработки.
    // Соединение аутентифицировано, но еще не готово.
    // Сейчас это соединение можно уже 'посчитать', но слать в него пока ничего НИЗЗЯ!
    // (слать можно будет во 2-ой фазе..)
    virtual void OnLoginSucceededPhase1(IMsgConnection* pIMsgConn, bool isLocal,
                                        bool isOposDefault, const char *oposAddr) = 0;
    // 2ая фаза обработки.
    // Вызывается из того же потока, что и первая фаза сразу после
    // того, как:
    //  - будут вызваны все обработчики по 1ой фазе
    //  - аутентификатор будет извещен об окончании первой фазы и сможет завершить
    //    общение с пиром.
    // Теперь уже можно использовать соединение по полной программе.
    virtual void OnLoginSucceededPhase2(IMsgConnection* pIMsgConn, bool isLocal,
                                        bool isOposDefault, const char *oposAddr) = 0;

 	virtual void OnLoginFailed(IMsgConnection* pIMsgConn, P2ERR error/*, ICs* pICsCaller*/) = 0;
    virtual void OnClose(IMsgConnection *pIMsgConn, int error, IRouter::ConnectionType connType,const char* connAddress) = 0;

    CHECK_FOR_DELETE__(ILinkNotifHandler)
};

#endif // !defined(AFX_ILINKNOTIFHANDLER_H__53031B2D_1E9E_492F_B592_B7EEF41845A7__INCLUDED_)
