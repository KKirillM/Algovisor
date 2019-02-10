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

/* ������� ������� �������:
 * ����� ����� (1) NODE.SERVICE
 * ����������� ����� (2) .SERVICE
 * ����������� ����� (3) NODE
 * ����������� ����� (4) SERVICE
 */

struct ILinkNotifHandler : public IRc
{
    // - There are no OnOutLocalConnectComplete!!!!
    // - ����������� �������� � IMsgConnection
    // - ���� ����������� ���������� �� IMsgConnection (����� ������ ��� ��������� ��������)
    // - ����� ����������� ����� ������ ��� �������� ��������
    // - ������� ������� ������� 
	//	isLocal == true (2) (4)
	//	isLocal == false (1) (3)
//    virtual void OnLoginSucceeded(IMsgConnection* pIMsgConn, bool isLocal,
//                                  bool isOposDefault, const char *oposAddr/*,
//                                  ICs* pICsCaller*/) = 0;

    // 1�� ���� ���������.
    // ���������� �����������������, �� ��� �� ������.
    // ������ ��� ���������� ����� ��� '���������', �� ����� � ���� ���� ������ �����!
    // (����� ����� ����� �� 2-�� ����..)
    virtual void OnLoginSucceededPhase1(IMsgConnection* pIMsgConn, bool isLocal,
                                        bool isOposDefault, const char *oposAddr) = 0;
    // 2�� ���� ���������.
    // ���������� �� ���� �� ������, ��� � ������ ���� ����� �����
    // ����, ���:
    //  - ����� ������� ��� ����������� �� 1�� ����
    //  - �������������� ����� ������� �� ��������� ������ ���� � ������ ���������
    //    ������� � �����.
    // ������ ��� ����� ������������ ���������� �� ������ ���������.
    virtual void OnLoginSucceededPhase2(IMsgConnection* pIMsgConn, bool isLocal,
                                        bool isOposDefault, const char *oposAddr) = 0;

 	virtual void OnLoginFailed(IMsgConnection* pIMsgConn, P2ERR error/*, ICs* pICsCaller*/) = 0;
    virtual void OnClose(IMsgConnection *pIMsgConn, int error, IRouter::ConnectionType connType,const char* connAddress) = 0;

    CHECK_FOR_DELETE__(ILinkNotifHandler)
};

#endif // !defined(AFX_ILINKNOTIFHANDLER_H__53031B2D_1E9E_492F_B592_B7EEF41845A7__INCLUDED_)
