#include "stdafx.h"
#include "IniFileWorker.h"
#include "HelpTools.h"
#include "Plaza2_Tables.h"
#include "Plaza2_defines.h"
#include "Listener.h"

#include "Plaza2_Connector.h"

bool Plaza2Connector::environmentInitFlag = false;
int Plaza2Connector::plaza2ActiveConnetionCounter = 0;

Plaza2Connector::Plaza2Connector() : 
	plaza2ConnectionObject(nullptr),
	bStopRun(false),
	callbackThreadHandle(nullptr),
    callbackEventHandle(nullptr),
	status(NONE)
{
}

Plaza2Connector::~Plaza2Connector()
{
	for_each(listenerlist.begin(), listenerlist.end(), [](Listener* param) 
	{
		Listener* pFD = param;
		if (pFD) 
			delete pFD; 
	});
	listenerlist.clear();
}

void Plaza2Connector::OpenConnectionToRouter(const string& url, const string& environmentString)
{
	if (!environmentInitFlag)
	{
		CHECK_PLAZA2_FAIL(env_open(environmentString.c_str()))
		environmentInitFlag = true;
	}
    ++plaza2ActiveConnetionCounter;
	CHECK_PLAZA2_FAIL(conn_new(url.c_str(), &plaza2ConnectionObject));
}

void Plaza2Connector::CloseConnectionToRouter()
{
	if (plaza2ConnectionObject)
	{
		CHECK_PLAZA2_FAIL(conn_destroy(plaza2ConnectionObject));
		plaza2ConnectionObject = NULL;
		log_info("Connection to router closed");
        --plaza2ActiveConnetionCounter;
	}

    if (environmentInitFlag && !plaza2ActiveConnetionCounter)
    {
        CHECK_PLAZA2_FAIL(env_close());
        environmentInitFlag = false;
    }
}

void Plaza2Connector::RunCallbackThread(CallbackThreadFunc func)
{
    callbackEventHandle = CreateEvent(NULL,                           // default security attributes
                                      TRUE,                           // manual-reset event
                                      FALSE,                          // initial state is nonsignaled
                                      TEXT("Plaza2Connector_Event")   // object name
                                      ); 

    if (callbackEventHandle == NULL)
    {
        std::string errorMessage = "Plaza2Connector_Event creation error";
        log_error(errorMessage.c_str());
        throw Plaza2ConnectorException(errorMessage.c_str(), CG_ERR_INTERNAL);
    }

	bStopRun = true;
	callbackThreadHandle = (HANDLE)_beginthreadex(NULL, 0, func, (void*)this, NULL, NULL);
	if ((long long)callbackThreadHandle == -1L)
	{
		std::string errorMessage = "Plaza2Connector callback thread start error";
		log_error(errorMessage.c_str());
		throw Plaza2ConnectorException(errorMessage.c_str(), CG_ERR_INTERNAL);
	}
}

void Plaza2Connector::StopCallbackThread()
{
	ClearStopFlag();
	WaitUntilCallbackEnd();
}

void Plaza2Connector::WaitUntilCallbackEnd()
{
	if (callbackThreadHandle)
	{
		WaitForSingleObject(callbackThreadHandle, INFINITE);
		CloseHandle(callbackThreadHandle);
		log_info("Callback thread stopped");
		callbackThreadHandle = NULL;
	}	
}

//void Plaza2Connector::OpenListener(const std::string& stream_name, const std::string& settingsString, const std::string& replState, const SystemMessageCallBackFunc func)
//{
//    if (settingsString.empty())
//    {
//        log_info("%s settings string is missing in INI file", stream_name.c_str());
//        return;
//    }
//
//    utils::CSimpleParserA parser;
//    std::vector<std::string> values;
//    size_t tablesPlace = settingsString.find("tables=");
//    bool tablesListExist = tablesPlace != std::string::npos;
//
//    if (tablesListExist)
//    {
//        parser.parse(settingsString, ';', values);
//        if (values.size() < 2)
//        {
//            std::string error = stream_name + "parameter not filled in completely";
//            throw std::exception(error.c_str());
//        }
//        OpenListener(0, stream_name, values[0], values[1], replState, func);
//        log_info("%s Plaza2 replication stream opened", stream_name.c_str());
//    }
//    else
//    {
//        OpenListener(0, stream_name, "", settingsString, replState, func);
//        log_info("%s Plaza2 replication stream opened", stream_name.c_str());
//    }	
//}

//void Plaza2Connector::CloseFlow(int isin)
//{
//	ListenerList::iterator it;
//	while ((it = std::find_if(listenerlist.begin(), listenerlist.end(), std::bind2nd(Listener::find_name(), isin))) != listenerlist.end())
//	{
//		::AcquireSRWLockExclusive(&lock);
//		(*it)->CloseListener();
//		(*it)->DestroyListener();
//		delete (*it);
//		listenerlist.erase(it);
//		::ReleaseSRWLockExclusive(&lock);
//	}
//}

bool Plaza2Connector::CheckConnectionState()
{
	uint32_t state;

	WARN_FAIL(conn_getstate(plaza2ConnectionObject, &state));

	if (state == CG_STATE_ACTIVE)
	{
		status = OPEN;

        if (callbackEventHandle)
        {
            SetEvent(callbackEventHandle);
            Sleep(1); // подождём пока основной поток закроет этот ивент
        }

		return true;
	}
	else if (state == CG_STATE_ERROR)
	{
		status = DISCONNECT;
		log_error("Plaza2 connection state: CG_STATE_ERROR, try to call cg_conn_close()...");
		WARN_FAIL(conn_close(plaza2ConnectionObject));
	}
	else if (state == CG_STATE_CLOSED)
	{
		status = WAIT;
		log_info("Plaza2 connection state: CG_STATE_CLOSED, try to call cg_conn_open()...");
		WARN_FAIL(conn_open(plaza2ConnectionObject, NULL));
	}

	if (state != CG_STATE_ACTIVE)
	{
		auto listenerList = GetListenerList();
		for (Listener* pListener : listenerList)
		{
			pListener->CloseListener();
		}
	}

	return false;
}

void Plaza2Connector::ReadReplstates(const std::string& pathToReplstateDB)
{
	if (pathToReplstateDB.empty())
	{
        std::string errorString = "pathToReplstateDB parameter is empty";
		log_error(errorString.c_str());
		throw Plaza2ConnectorException(errorString.c_str(), CG_ERR_INVALIDARGUMENT);
	}

	TCHAR currDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, currDir);
	std::string fullPathToReplstateDB = currDir + pathToReplstateDB + '\\';

	FILE* replstate_file = NULL;

	for (auto it = listenerlist.begin(), end = listenerlist.end(); it != end; ++it)
	{
        std::string filePath = fullPathToReplstateDB + (*it)->GetStreamName() + REPLSTATEFILE;
        char* replstate = NULL;
        long fsize;

        // возможно здесь нужна проверка даты файла реплстейта и если она вчерашняя, то считаем что файл устарел и не считываем его, а удаляем
        /*HANDLE fH;
        FILETIME creationTime;
        SYSTEMTIME sysFileTime;

        fH = CreateFile(snapshotFilePath.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if(fH != INVALID_HANDLE_VALUE)
        {
            GetFileTime(fH, &creationTime, 0, 0);
            FileTimeToSystemTime(&creationTime, &sysFileTime);
            CloseHandle(fH);
        }
        else
        {
            return;
        }

        SYSTEMTIME sysTime;
        GetSystemTime(&sysTime);
        if (sysTime.wDay == sysFileTime.wDay)
        {
            fsize = HelpTools::ReadFileInBuffer(filePath, replstate);

            if (fsize > 0)
            {
                (*it)->SetReplState(replstate);
                free(replstate);
            }
        }
        else
        {
            DeleteFile(snapshotFilePath.c_str());
        }*/

        // файла может и не быть, тогда считаем что по этому потоку нет сохранённого реплстейта
        fsize = HelpTools::ReadFileInBuffer(filePath, replstate);

        if (fsize > 0)
        {
            (*it)->SetReplState(replstate);
            free(replstate);
        }
	}
}

void Plaza2Connector::WaitUntilCallbackStart()
{
    DWORD dwWaitResult = WaitForSingleObject(callbackEventHandle, 5000); // 5 секунд ждём пока произойдет подключение

    switch (dwWaitResult)
    {  
    case WAIT_OBJECT_0:     // The state of the specified object is signaled
        //log_info("Plaza2 connection event change state");
        break; 
    case WAIT_TIMEOUT:      // The time-out interval elapsed, and the object's state is nonsignaled
        {
            std::string errorMessage = "Plaza2 time-out connection interval elapsed";
            log_error(errorMessage.c_str());
            throw Plaza2ConnectorException(errorMessage.c_str());
        }
        break;
    case WAIT_FAILED:       // The function has failed
        {
            char err_str[100];
            sprintf_s(err_str, 100, "Error ocurred during wait for the connect to Plaza2, error_code=%d", GetLastError());
            log_error(err_str);
            throw Plaza2ConnectorException(err_str);
        }
        break;
    }

    CloseHandle(callbackEventHandle);
    callbackEventHandle = NULL;
}

void MessageLoop(Plaza2Connector* connector)
{
    for (auto it = connector->GetListenerList().begin(), end = connector->GetListenerList().end(); it != end; ++it)
    {
        (*it)->CheckListenerState();
    }

    uint32_t result = CG_ERR_OK;
    //log_debug("Start conn_process() cycle");
    //printf("Start conn_process() cycle\n");
    for (int callidx = 0; callidx < 1000 /*&& (result == CG_ERR_OK || result == CG_ERR_TIMEOUT)*/; ++callidx)
    {
        result = conn_process(connector->GetConnectionObject(), 0, 0);

        if (result == CG_ERR_TIMEOUT) // сообщений нет
        {
            break;
        }
        else if (result != CG_ERR_OK)
        {
            log_error("conn_process() return: %X, try to call cg_conn_close()...", result);
            WARN_FAIL(conn_close(connector->GetConnectionObject()));
            break;
        }
    }

    //printf("End conn_process() cycle\n");
    //log_debug("End conn_process() cycle");
}

Plaza2ConnectorType1::Plaza2ConnectorType1()
{
}

Plaza2ConnectorType1::~Plaza2ConnectorType1()
{
}

unsigned int __stdcall Plaza2ConnectorType1::CallbackThreadFunc(void* ptr)
{
    log_info("Plaza2ConnectorType1 Callback thread started");

    Plaza2Connector* connector = (Plaza2Connector*)ptr;

    while(connector->IsRunCycle())
    {
        if (connector->CheckConnectionState())
        {
            MessageLoop(connector);
			::Sleep(1);
        }
    }

    connector->ChangeConnectionStatus(DISCONNECT);

    return 0;
    /*  Код возврата пользовательского обработчика должен быть установлен в 0 в случае успеха обработки 
        сообщения или в другое значение в случае ошибки. В случае возврата ошибки из callback подписчик 
        будет закрыт, и в callback придут последовательно сообщения: replstate - с состоянием подписчика 
        на последний CG_MSG_TN_COMMIT, CG_MSG_CLOSE.
        Стоит добавить один важный момент, который состоит в том, что после выхода из этого коллбэка 
        сообщения CG_MSG_TN_COMMIT и CG_MSG_CLOSE приходят уже в другом потоке, который видимо плаза2 сама создаёт.
    */
}

void Plaza2ConnectorType1::OpenConnection(const Plaza2InitParameters& initParams, const P2ReplList& streamsParams)
{
    if (!plaza2ConnectionObject)
    {
        status = WAIT;
        OpenConnectionToRouter(initParams.connectionString, initParams.environmentString);
        OpenP2ReplListeners(streamsParams);
        ReadReplstates(initParams.replstateFolderPath);
        RunCallbackThread(CallbackThreadFunc);
        WaitUntilCallbackStart();
    }
}

void Plaza2ConnectorType1::Disconnect()
{
    status = NONE;
    StopCallbackThread();
    CloseP2ReplListeners();
    CloseConnectionToRouter();
}

void Plaza2ConnectorType1::OpenP2ReplListeners(const P2ReplList& streamsParams)
{
    for (auto it = streamsParams.begin(), end = streamsParams.end(); it != end; ++it)
    {
        if (!it->second.func1 || !it->second.func2)
        {
            char err_str[100];
            sprintf_s(err_str, 100, "CallBack function for stream %s is empty", it->first.c_str());
            log_error(err_str);
            throw Plaza2ConnectorException(err_str);
        }

        Listener* listener = new P2Listener(&plaza2ConnectionObject, it->second);
        listener->SetStreamID(it->first);
        listener->OpenListener();

        listenerlist.push_back(listener);
    }
}

void Plaza2ConnectorType1::CloseP2ReplListeners()
{
    for (auto it = GetListenerList().begin(); it != GetListenerList().end(); )
    {
        (*it)->CloseListener();
        (*it)->DestroyListener();
        delete (*it);
        it = GetListenerList().erase(it);
    }
}

Plaza2ConnectorType2::Plaza2ConnectorType2()
{
    ::InitializeSRWLock(&lock);
}

Plaza2ConnectorType2::~Plaza2ConnectorType2()
{
}

unsigned int __stdcall Plaza2ConnectorType2::CallbackThreadFunc(void* ptr)
{
    log_info("Plaza2ConnectorType2 callback thread started");

    Plaza2Connector* connector = (Plaza2Connector*)ptr;

    while(connector->IsRunCycle())
    {
        if (connector->CheckConnectionState())
        {
            ::AcquireSRWLockShared(&connector->GetSRWLOCK());
            MessageLoop(connector);
            ::ReleaseSRWLockShared(&connector->GetSRWLOCK());
			::Sleep(1);
        }
    }

    connector->ChangeConnectionStatus(DISCONNECT);

    return 0;
}

void Plaza2ConnectorType2::OpenConnection(const Plaza2InitParameters& initParams)
{
    if (plaza2ConnectionObject == nullptr)
    {
        status = WAIT;
        OpenConnectionToRouter(initParams.connectionString, initParams.environmentString);
        ReadReplstates(initParams.replstateFolderPath);
        RunCallbackThread(CallbackThreadFunc);
        WaitUntilCallbackStart();
    }
}

void Plaza2ConnectorType2::Disconnect()
{
    status = NONE;
    StopCallbackThread();
    CloseP2ReplListeners();
    CloseConnectionToRouter();
}

void Plaza2ConnectorType2::OpenP2ReplListener(const P2ListnerParameters& streamParams)
{
    if (!streamParams.func1 || !streamParams.func2)
    {
        assert(FALSE);
        char err_str[100];
        sprintf_s(err_str, 100, "CallBack function for stream %s is empty", streamParams.name.c_str());
        log_error(err_str);
        throw Plaza2ConnectorException(err_str);
    }

    Listener* listener = new P2Listener(&plaza2ConnectionObject, streamParams);
    listener->OpenListener();

    ::AcquireSRWLockExclusive(&lock);
    listenerlist.push_back(listener);
    ::ReleaseSRWLockExclusive(&lock);
}

void Plaza2ConnectorType2::CloseP2ReplListener(const int isin)
{
    ::AcquireSRWLockExclusive(&lock);
	for (auto it = GetListenerList().begin(); it != GetListenerList().end(); )
	{
		if ((*it)->GetIsin() == isin)
		{
			(*it)->CloseListener();
			(*it)->DestroyListener();
			delete (*it);
			it = GetListenerList().erase(it);
		}
		else
			++it;
    }
    ::ReleaseSRWLockExclusive(&lock);
}

void Plaza2ConnectorType2::CloseP2ReplListeners()
{
    ::AcquireSRWLockExclusive(&lock);
	for (ListenerList::iterator it = GetListenerList().begin(); it != GetListenerList().end(); )
	{
		(*it)->CloseListener();
        (*it)->DestroyListener();
        delete (*it);
        it = GetListenerList().erase(it);
    }
    ::ReleaseSRWLockExclusive(&lock);
}