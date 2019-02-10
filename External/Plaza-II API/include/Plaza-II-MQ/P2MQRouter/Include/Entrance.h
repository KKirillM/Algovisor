// Entrance.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENTRANCE_H__C3E3368F_7765_41AD_B14D_8B491AA972F0__INCLUDED_)
#define AFX_ENTRANCE_H__C3E3368F_7765_41AD_B14D_8B491AA972F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../Plaza-II/P2Utils/Include/CS.h"

//////////////////////////////////////////////////////////////////////
//
//  ������:
//  -------
//  - ���������� ������������� �������� ��������� � ������ ����� ����������
//    ��������
//  - �� �������� ������ ������ �������� �� ����������� ������
//
//  ������� ���������� � �������������:
//  -----------------------------------
//  - ���������� ����� ������������� ��������� ICs* (����� ���������� ��� ����������� ������)
//  - ���������� ����� ������ ����� ������ Lock()/Unlock()
//  - ������, ��������������� ���������� �������, ���� ������� ICs*,
//    ���� ��������� ����� �� ����������� ������
//  - ���� �����, ��������� ICs* ���������� �� �� ����������� ������,
//    �� � ���-�� ICs* ����� �������� 0
//  - �������������� �������������� ������ ��� ������ (���������), ������� ��������
//    �� ������ � ���� �� ��������
//    (��������, ���� ����� ������ ����� ����� ��������������, � ��. ����� �� ���� �� 
//     ����������������� ������ � �������������� �� �������, �� ����� ���������� ��������)
//  - ���������� ����������� ������� ������ ����, �����, ������������
//    (�� ��������� / ���������)
//  - ���� � ����� ���������� ICs* pICsCaller, �� ����� ������(!) ��� ���������� �����
//    �� 3-x ��������:
//      - ENTER_LOCK(pICsCaller)
//      - ENTER_NOTNEED(pICsCaller)
//      - �������� ������ �������, ����������� ICs* pICsCaller
//
//  ������:
//  -------
//
//  // ���������� �������
//  class CHandler : private CCS
//  {
//  ...
//  public:
//      // �����, ����� ������� �������� ���������� ����� �� ����������� ������
//      void OnRecvData(const char* pData, int len,
//                      ICs* pICsCaller);   // <- ������� ICs, ���� ����� ����������
//                                          //    �� ������ ����������� ������
//
//      // �����, ����������� �������� ���, �������� � ����������� ������
//      // (�� ������� ICs)
//      bool Encrypt(const char* pFrom, char* pTo);
//  ...
//  private:
//      ENTER_DECLARE()
//  };
//
//  CHandler::OnRecvData(const char* pData, int len, ICs* pICsCaller)
//  {
//      ENTER_LOCK(pICsCaller)
//
//      if (m_ready)
//      {
//      ...
//      }
//
//      ENTER_UNLOCK()
//  }
//
//  class CConnection : private ICs
//                      private CSock
//  {
//  ...
//  private:
//      // ICs
//      void Lock();
//      void Unlock();
//
//      void OnSockRecv(const char* pData, int len);
//  };
//
//  void CConnection::OnSockRecv(const char* pData, int len)
//  {
//      Lock();
//      ...
//      m_pHandler->OnRecvData(pData, len, this);   // <- (*)
//  }
//
//
//  ��������� ����������:
//  ---------------------
//
//  - ����������� ����� Unlock() � (*)
//
//////////////////////////////////////////////////////////////////////

#define ENTER_DECLARE_NAME(name) CCS m_Enter_##name##__;

#define ENTER_LOCK_NAME(pICsCaller, name)   \
    if (pICsCaller)                         \
    {                                       \
        m_Enter_##name##__.Lock();          \
        (pICsCaller)->Unlock();             \
        Lock();                             \
        m_Enter_##name##__.Unlock();        \
    }                                       \
    else                                    \
    {                                       \
        Lock();                             \
    }

// <- just use Unlock() instead
//#define ENTER_UNLOCK() Unlock();

#define ENTER_LOCK(pICsCaller) ENTER_LOCK_NAME(pICsCaller, Default)

#define ENTER_DECLARE() ENTER_DECLARE_NAME(Default)

#define ENTER_NOTNEED(pICsCaller)   if (pICsCaller) { (pICsCaller)->Unlock(); }

#endif // !defined(AFX_ENTRANCE_H__C3E3368F_7765_41AD_B14D_8B491AA972F0__INCLUDED_)
