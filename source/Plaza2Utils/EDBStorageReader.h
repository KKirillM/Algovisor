#pragma once

#include "..\..\External\Plaza-II API\include\Plaza-II-eRepl\P2eReplStorage\Include\P2eReplStorage.h"
#include "..\..\External\Plaza-II API\include\Plaza-II-eRepl\P2eReplStorage\Include\P2eReplStorage.hpp"

typedef P2ERR (*EDBStorabeCallback)(const TEReplRec* pRec, void* pParam);

/// @brief Класс EDBReader реализаует функционал чтения данных из edb-файлов
/// реализация использует библиотеку P2eReplStorage
class EDBStorageReader
{
public:
    EDBStorageReader() : startupInit(false) {}
    ~EDBStorageReader()
    {
        if (storageIn.m_h != NULL)
        {
            P2eReplStorageCleanup();
        }
    }

    /// @brief Открывает файл-хранилище для работы с ним
    /// @param [in] pathToEDBFile строка, указывающая путь к edb-файлу базы
    /// @param [out] результат операции открытия файла-хранилища: 0 - открытие прошло успешно, 1 - произошла ошибка во время открытия (причина в логе)
    int Open(const std::string& pathToEDBFile)
    {
        P2ERR res;

        if (!startupInit && P2FAILEDV(P2eReplStorageStartup(), res))
        {
            P2LogErrorWithCodeEx(res, "EDBStorageReader", "P2eReplStorageStartup failed");
            return 1;
        }

        startupInit = true;

        std::string storageConnStr = "cache_type=Last1;flush_mode=PerTrans;file=";
        storageConnStr += pathToEDBFile;
        if (P2FAILEDV(P2eReplStorageOpen(storageConnStr.c_str(), &storageIn), res))
        {
            P2LogErrorWithCodeEx(res, "EDBStorageReader", "P2eReplStorageOpen failed");
            return 1;
        }

        if (P2FAILEDV(P2eReplStorageLoad(storageIn), res))
        {
            P2LogErrorWithCodeEx(res, "EDBStorageReader", "P2eReplStorageLoad failed");
            return 1;
        }

        return 0;
    }

    int Close()
    {
        P2ERR res;

        if (P2FAILEDV(P2eReplStorageClose(storageIn), res))
        {
            P2LogErrorWithCodeEx(res, "EDBStorageReader", "P2eReplStorageClose failed");
            return 1;
        }
        
        storageIn.Detach();

        return 0;
    }

    /// @brief Возвращает последнее значение ревижена содержащееся в файле-хранилище
    INT64 GetLastRevision()
    {
        P2ERR res;
        P2EREPL_REVISION_TYPE pRev;

        if (P2FAILEDV(P2eReplStorageGetNextRev(storageIn, &pRev), res))
        {
            //P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_ERROR, "EDBStorageReader", "GetLastRevision failed"))
            return 0;
        }

        return pRev-1;
    }

    /// @brief Запускает получение данных из edb-файла
    void StartRead(EDBStorabeCallback& callbackFunc, void* callbackParam, const INT64 minRevision = P2EREPL_MIN_REVISION, const INT64 maxRevision = P2EREPL_MAX_REVISION)
    {
        P2ERR res;
        //for( ; ; )
        //{
            //INT64 lastRev = GetLastRevision();
            if (P2FAILEDV(P2eReplStorageEnumRecords(storageIn, minRevision, maxRevision, callbackFunc, callbackParam), res))
            {
                if (P2ERR_COMMON_EXITCODE_TERMINATED == res)
                {
                    //P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, "EDBStorageReader", "Enum records was terminated"))
                    return;
                }

                /*if (P2ERR_NOT_OK == res)
                {
                    continue;
                }*/

                if (P2ERR_COMMON_CANCEL_ENUM == res)
                {
                    //P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, "EDBStorageReader", "All ranges were dumped"))
                    return;
                }

                throw std::exception("P2eReplStorageEnumRecords failed");
            }

            //P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, "EDBStorageReader", "Reading EDB-file complete"))
            //break;
        //} 
    }

private:
    P2AutoEReplStorage storageIn;
    bool startupInit;

};  // EDBStorageReader

