#ifndef __RTS_Q_P2EXECLOG_H__
#define __RTS_Q_P2EXECLOG_H__

#ifdef FASTCALL
#undef FASTCALL
#endif

#define EVENT_BUFFER_SIZE 1024*10   // 10 Кбайт

#include "P2eSharedMem.h"
#include "SharedMemoryCriticalSectionLock.h"
#include "p2time.h"

typedef fo2base::CHandle<P2EREPL_STORAGE_HANDLE, P2eShReplStorageClose> CP2eConnectionHandle;

namespace
{
    struct P2eShReplStorageBeginTransactionSafe
    {
        P2eShReplStorageBeginTransactionSafe(P2EREPL_STORAGE_HANDLE hStorage) : hStorage_(hStorage) {}
        P2ERR operator()() {return P2eShReplStorageBeginTransaction(hStorage_);}
        ~P2eShReplStorageBeginTransactionSafe(){}
    private:
        P2EREPL_STORAGE_HANDLE hStorage_;
    };

    struct P2eShReplStorageCommitSafe
    {
        P2eShReplStorageCommitSafe(P2EREPL_STORAGE_HANDLE hStorage) : hStorage_(hStorage) {}
        P2ERR operator()() {return P2eShReplStorageCommit(hStorage_);}
        ~P2eShReplStorageCommitSafe(){}
    private:
        P2EREPL_STORAGE_HANDLE hStorage_;
    };

    struct P2eShReplStorageRollbackSafe
    {
        P2eShReplStorageRollbackSafe(P2EREPL_STORAGE_HANDLE hStorage) : hStorage_(hStorage) {}
        P2ERR operator()() {return P2eShReplStorageRollback(hStorage_);}
        ~P2eShReplStorageRollbackSafe(){}
    private:
        P2EREPL_STORAGE_HANDLE hStorage_;
    };

    struct P2eShReplStorageInsertSafe
    {
        P2eShReplStorageInsertSafe(P2EREPL_STORAGE_HANDLE hStorage, TEReplRec* pRec) : hStorage_(hStorage),pRec_(pRec) {}
        P2ERR operator()() {return P2eShReplStorageInsert(hStorage_,pRec_);}
        ~P2eShReplStorageInsertSafe(){}
    private:
        P2EREPL_STORAGE_HANDLE hStorage_;
        TEReplRec* pRec_;
    };

    struct P2eShReplStorageClearOldRevSafe
    {
        P2eShReplStorageClearOldRevSafe(P2EREPL_STORAGE_HANDLE hStorage, P2EREPL_REVISION_TYPE maxRev, UINT32 flag) : hStorage_(hStorage),maxRev_(maxRev),flag_(flag) {}
        P2ERR operator()() {return P2eShReplStorageClearOldRev(hStorage_,maxRev_,flag_);}
        ~P2eShReplStorageClearOldRevSafe(){}
    private:
        P2EREPL_STORAGE_HANDLE hStorage_;
        P2EREPL_REVISION_TYPE maxRev_;
        UINT32 flag_;
    };

    bool ReportEvent(LPCSTR message)
    {
        char	path[MAX_PATH];
        HMODULE thisModule = 0;
        ZeroMemory(path, MAX_PATH);
        char *pFileName = "FO2SRV64.EXE";
        HANDLE event_log;

        if(GetModuleFileName(thisModule, path, MAX_PATH))
        {
            char* pSlashPos;
            if((pSlashPos = strrchr( path, '\\' )) != NULL)
            {
                pFileName = pSlashPos + 1;
            }
        }

        if( (event_log = ::RegisterEventSource(NULL, pFileName)) == NULL )
            return false;

        if(::ReportEvent(event_log, EVENTLOG_ERROR_TYPE, 0, 0, NULL, 1, 0, &message,NULL)==0)
            return false;

        return true;
    }

    template <class F>
    void performEReplSafe(F f)
    {
        P2ERR err = P2ERR_OK;
        do 
        {
            err = f();
            if (err==P2ERR_OK)
            {
                break;
            }
            else if (err==P2ERR_COMMON_TIMEOUT)
            {
                continue;
            }
            else
            {
                char message[512];
                sprintf_s(message, _countof(message), "ErrorCode (%d) from Plaza II received. Exit.", err);

                P2TRACE_ERROR((message))

                ReportEvent(message);

                //Создаем минидамп
                RaiseException(EXCEPTION_ACCESS_VIOLATION, 0, 0, NULL);
            }                     
        }
        while (1);
    }
}


//----------------------------------------------------------------------------
//  class CP2eConnection
/// Базовый класс соединения с eRepl
//----------------------------------------------------------------------------
class CP2eConnection : public CP2eConnectionHandle
{
public:
    CP2eConnection() : CP2eConnectionHandle() 
    {
        mRefCount = 0;
        m_LastSentRev = 0;
    }

    ~CP2eConnection() 
    {
        P2eReplStorageCleanup();
    }

    void Create(const char* pSectionName)
    {
        P2EREPL_STORAGE_HANDLE  h;
        P2ERR                   rc;
        Release();
        if(P2FAILEDV(P2eShReplStorageOpen(pSectionName, &h), rc))
        {
            throw fo2base::CP2Error("Create erepl connection failed", rc);
        }
        Attach(h);
    }

    void Disconnect()
    {
        P2ERR                   rc;
        if(P2FAILEDV(P2eShReplStorageClose(GetHandle()), rc))
        {
            throw fo2base::CP2Error("Disconnect erepl failed", rc);
        }

		Detach();
    }

    void BeginTransaction()
    {
        //EnterCriticalSection(&mCS);
        if (mRefCount==0)
        {
            performEReplSafe(P2eShReplStorageBeginTransactionSafe(GetHandle()));
        }
        mRefCount++;
    }

    void Commit()
    {
        if (mRefCount==1)
        {
            performEReplSafe(P2eShReplStorageCommitSafe(GetHandle()));
        }
        mRefCount = max(0, mRefCount-1);
        //LeaveCriticalSection(&mCS);
    }

    void Rollback()
    {
        if (mRefCount == 1)
        {
            performEReplSafe(P2eShReplStorageRollbackSafe(GetHandle()));
        }
        mRefCount = max(0, mRefCount-1);
        //LeaveCriticalSection(&mCS);
    }

    void Insert(TEReplRec* rec)
    {
        performEReplSafe(P2eShReplStorageInsertSafe(GetHandle(), rec));
        m_LastSentRev = rec->rev;
    }

    P2EREPL_REVISION_TYPE GetNextRev()
    {
        P2ERR rc;
        P2EREPL_REVISION_TYPE rev;

        if(P2FAILEDV(P2eShReplStorageGetNextRev(GetHandle(), &rev), rc))
        {
            throw fo2base::CP2Error(rc);
        }
        return rev;
    }

    void Clear(P2EREPL_REVISION_TYPE rev = 0)
    {
        fo2base::CCritA guard(GetLock());
        P2ERR rc;
        if (rev==0)
        {
            if(P2FAILEDV(P2eShReplStorageGetNextRev(GetHandle(), &rev), rc))
            {
                throw fo2base::CP2Error(rc);
            }
            rev++;
        }
        
        do
        {
            rc = P2eShReplStorageClearOldRev(GetHandle(), rev, P2EREPL_STORAGE_CLEAR_AND_CLEAN );
        }
        while (rc==P2ERR_COMMON_TIMEOUT);
        if (rc!=P2ERR_OK) throw fo2base::CP2Error(rc);
    }

    fo2base::CCritSection& GetLock() { return mCS; }

    P2EREPL_REVISION_TYPE GetLastSentRev()
    {
        if (m_LastSentRev == 0)
        {
            P2ERR rc;
            P2EREPL_REVISION_TYPE pRev;

            if (P2FAILEDV(P2eReplStorageGetNextRev(GetHandle(), &pRev), rc))
            {
                throw fo2base::CP2Error(rc);
            }

            m_LastSentRev = pRev-1;
        }

        return m_LastSentRev;   
    }

private:
    //CRITICAL_SECTION mCS;
    fo2base::CCritSection   mCS;
    int                     mRefCount;
    P2EREPL_REVISION_TYPE   m_LastSentRev;
};  // CP2eConnection

class CReplStruct
{
public:
    CReplStruct() 
    {
        ZeroMemory(m_data, EVENT_BUFFER_SIZE);
    }
        
    void Init(void* message, UINT32 messageSize, UINT messageType)
	{
        //static int dataSize = 0;    // максимальный размер в байтах, который записывается в рёплику за транзакцию

		TEReplRec tRec;
		tRec.eventType = messageType;
		
        /*int64_t currentTime = fo2base::CP2Time::Now().ToInt64();
        LARGE_INTEGER perfCounter;
        QueryPerformanceCounter(&perfCounter);*/

        fo2base::UnixNanosecondsTime currentTime = fo2base::NowHighResolutionUnixNanosecondsTime();

		tRec.dataSize = messageSize + sizeof(currentTime);

        if (tRec.dataSize > EVENT_BUFFER_SIZE)
        {
            throw fo2base::CP2Error(P2ERR_COMMON_BUFFER_TOO_SMALL, "Size of eRepl message (%d bytes) is bigger than CReplStruct buffer (%d bytes)", tRec.dataSize, EVENT_BUFFER_SIZE);
        }
		
		memcpy(m_data, &tRec, sizeof(tRec));
		memcpy(m_data + sizeof(tRec), &currentTime, sizeof(currentTime));
		memcpy(m_data + sizeof(tRec) + sizeof(currentTime), message, messageSize);

        /*if (dataSize < messageSize)
        {
            dataSize = messageSize;
            printf("%d bytes\n", dataSize);
        }*/ 
	}
	~CReplStruct()
	{
		//delete[] m_data;
	};
protected:
	char m_data[EVENT_BUFFER_SIZE];

public:
	TEReplRec*  getData() { return reinterpret_cast<TEReplRec*>(m_data); };
	void setRev(P2EREPL_REVISION_TYPE value)
	{
		reinterpret_cast<TEReplRec*>(m_data)->rev = value;
	}
};

#endif//__RTS_Q_P2EXECLOG_H__
