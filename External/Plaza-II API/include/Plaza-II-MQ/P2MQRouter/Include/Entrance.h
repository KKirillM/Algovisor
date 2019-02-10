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
//  Задача:
//  -------
//  - Обеспечить упорядоченную передачу извещений и данных между различными
//    уровнями
//  - Не вызывать методы других объектов из критических секций
//
//  Правила реализации и использования:
//  -----------------------------------
//  - Вызывающий класс предоставляет интерфейс ICs* (чтобы освободить его критическую секцию)
//  - Вызываемый класс должен иметь методы Lock()/Unlock()
//  - Методы, предоставляемые вызываемым классом, либо требуют ICs*,
//    либо допускают вызов из критической секции
//  - Если метод, требующий ICs* вызовается не из критической секции,
//    то в кач-ве ICs* можно передать 0
//  - Обеспечивается упорядочивание только для данных (извещений), которые проходят
//    по одному и тому же маршруту
//    (например, если часть данных ходит через аутентификатор, а др. часть из того же 
//     последовательного потока в аутентификатор не заходит, то может возникнуть путаница)
//  - Блокировка вызывающего объекта должна быть, ессна, единственной
//    (не вложенной / последней)
//  - Если в метод передается ICs* pICsCaller, то метод ОБЯЗАН(!) его обработать одним
//    из 3-x способов:
//      - ENTER_LOCK(pICsCaller)
//      - ENTER_NOTNEED(pICsCaller)
//      - передать другой функции, принимающей ICs* pICsCaller
//
//  Пример:
//  -------
//
//  // Обработчик событий
//  class CHandler : private CCS
//  {
//  ...
//  public:
//      // Метод, перед вызовом которого желательно выйти из критической секции
//      void OnRecvData(const char* pData, int len,
//                      ICs* pICsCaller);   // <- требует ICs, если будет вызываться
//                                          //    из другой критической секции
//
//      // Метод, позволяющий вызывать его, находясь в критической секции
//      // (не требует ICs)
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
//  Некоторые недостатки:
//  ---------------------
//
//  - Неочевидный вызов Unlock() в (*)
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
