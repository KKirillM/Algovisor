#include "stdafx.h"
#include "ConnectionManager.h"
#include "Plaza2_Connector.h"
#include "auxiliary_methods.h"

#include "Plaza2Callbacks.h"


const char* Plaza2Callbacks::moduleName = "Plaza2Callbacks";
int Plaza2Callbacks::sess_id = 0;

void WriteDataToeShmem(CReplStruct& rec, ExternalStoragesType extType)
{
    ConnectionManager* connMngr = ConnectionManagerHolder::GetInstance();
    CP2eConnection* externalStorage = connMngr->GetExtStorage();

    rec.setRev(externalStorage[extType].GetLastSentRev());

    externalStorage[extType].BeginTransaction();
    externalStorage[extType].Insert(rec.getData());
    externalStorage[extType].Commit();
}

Plaza2Callbacks::Plaza2Callbacks()
{
}


Plaza2Callbacks::~Plaza2Callbacks()
{
}

std::string GetCloseStreamReason(int* reason)
{
	std::string sReason;

	switch (*reason)
	{
	case CG_REASON_UNDEFINED:		// - не определена;
		sReason += "unknown";
		break;

	case CG_REASON_USER:			// - пользователь вернул ошибку в callback подписчика;
		sReason += "Callback function return error";
		break;

	case CG_REASON_ERROR:			// - внутренняя ошибка;
		sReason += "internal error";
		break;

	case CG_REASON_DONE:			// - вызван метод cg_lsn_destroy;
		sReason += "cg_lsn_destroy call";
		break;

	case CG_REASON_SNAPSHOT_DONE:	// - снэпшот получен
		sReason += "snapshot received";
		break;
	}

	return sReason;
}

CG_RESULT Plaza2Callbacks::MsgTypeCallback(const int type, msg_t* data, IListener* listener)
{
	switch(type)
	{
	case CG_MSG_OPEN:
		{
			P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, moduleName, "%s replication stream is opened", listener->GetStreamName().c_str()))
		}
		break;

	case CG_MSG_CLOSE:
		{   // проверить здесь что содержится в поле data
			P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, moduleName, "%s replication stream is closed, reason: %s", listener->GetStreamName().c_str(),
                GetCloseStreamReason((int*)(data->data)).c_str()))
		}
		break;	

	case CG_MSG_P2REPL_ONLINE:
		{
			P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, moduleName, "%s replication stream in online mode started", listener->GetStreamName().c_str()))
		}
		break;

	case CG_MSG_P2REPL_REPLSTATE:
		{
			listener->SaveReplstateToFile(fo2base::gGetCP2Ini()->ReadStr("Plaza2", "replstateFolderPath"), (char*)data->data, data->data_size);
			listener->SetReplState(std::string((char*)data->data, data->data_size));
			P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, moduleName, "%s replstate was saved", listener->GetStreamName().c_str()))
		}
		break;
	case CG_MSG_P2REPL_LIFENUM:
		{
			P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, moduleName, "%s lifenum changed message recieved, lifenum= %d", 
				listener->GetStreamName().c_str(), data->data))
		}
		break;

	case CG_MSG_P2REPL_CLEARDELETED:
		{
			//P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, moduleName, "%s cleardeleted message recieved, table_idx %d, table_rev %lld",
			//	listener->GetStreamName().c_str(), ((cg_data_cleardeleted_t*)data)->table_idx, ((cg_data_cleardeleted_t*)data)->table_rev))
			
			if (((cg_data_cleardeleted_t*)data)->table_rev == CG_MAX_REVISON)
			{
				P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, moduleName, "%s table_idx %d, rev start from 1",
					listener->GetStreamName().c_str(), ((cg_data_cleardeleted_t*)data)->table_idx))
			}
		}
		break;
	}

	return CG_ERR_OK;
}

bool CheckNewSession(const int event_type)
{
#define SESSION_DATA_READY              1      // message = "session_data_ready",          Закончена загрузка данных из клиринговой системы в торговую перед началом новой торговой сессии
#define INTRADAY_CLEARING_FINISHED      2      // message = "intraday_clearing_finished",  Все расчетные процедуры в промклиринге закончены
#define INTRADAY_CLEARING_STARTED       4      // message = "intraday_clearing_started",   Начало промклиринга
#define CLEARING_STARTED                5      // message = "clearing_started",            Начало основного клиринга
#define EXTENSION_OF_LIMITS_FINISHED    6      // message = "extension_of_limits_finished" Раздвижка лимитов закончена
#define BROKER_RECALC_FINISHED          8      // message = "broker_recalc_finished"       Денежные средства после промклиринга пересчитаны
#define POSTLS_RECALC_STARTED          11 
#define SESSION_DATA_READY_FOR_CORE    12
#define BROKER_RECALC_STARTED          13

    if (event_type == CLEARING_STARTED)
    {
        return true;
    }
    return false;
}

void CopyEDBFilesToNewFolder()
{
    ConnectionManager* connMngr = ConnectionManagerHolder::GetInstance();
    connMngr->DeInit();

    fo2base::CP2Time date = fo2base::CP2Time::Today();
    std::string fromFolder = ".\\DB\\";
	char buf[30];
	sprintf_s(buf, "%02d-%02d-%04d", date.day, date.month, date.year);
	std::string folderPath = fromFolder + buf;

	P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, "Plaza2Callbacks", "Create folder: %s", folderPath.c_str()))

    std::string toFolder = CreateFolder(folderPath);

    char fileName[6][30] = { "Plaza2_AllData.edb", "Plaza2_AllData.edb-idx", 
                             "Plaza2_Info.edb", "Plaza2_Info.edb-idx",
                             "Plaza2_Trades.edb", "Plaza2_Trades.edb-idx" };

	P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, "Plaza2Callbacks", "Move files"))

    for (int i = 0; i < 6; ++i)
    {
        if (!MoveFile((fromFolder + fileName[i]).c_str(), (toFolder + '\\' + fileName[i]).c_str()))
        {
            P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_WARNING, "Plaza2Callbacks", "Can't move file: %s", fileName[i]))
        }
    }

    P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, "Plaza2Callbacks", "All EDB files was move to folder: %s", toFolder.c_str()))
    
    connMngr->Init();
}

CG_RESULT Plaza2Callbacks::StreamDataCallback(msg_streamdata_t* data, IListener* listener)
{
    static CReplStruct eReplMessage;
    CG_RESULT res = CG_ERR_OK;
    P2StreamID type = listener->GetStreamID();
    UINT messageType = (UINT)type + (UINT)data->msg_index; // data->msg_index - это номер таблицы внутри потока репликации

    try
    {
        eReplMessage.Init(data->data, (UINT32)data->data_size, messageType); 
    }
    catch (fo2base::CP2Error& err)
    {
        P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), 
                        P2LOG_SEVERITY_ERROR, 
                        "Plaza2_UniversalCallback", "%s error_message: %s",
                        listener->GetStreamName().c_str(),
                        err.GetMessageA().c_str()))
        res = CG_ERR_INTERNAL;
    }

    switch (type)
    {
    case P2StreamID::p2repl_FORTS_UNKNOWN:
        P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), 
                        P2LOG_SEVERITY_ERROR, 
                        "Plaza2_UniversalCallback", "%s table_idx: %d, message type: FORTS_UNKNOWN is wrong", 
                        listener->GetStreamName().c_str(), 
                        data->msg_index));
        res = CG_ERR_INTERNAL;
        break;

    case P2StreamID::p2repl_FORTS_DEALS_REPL:
        WriteDataToeShmem(eReplMessage, Plaza2_AllData);
        //WriteDataToeShmem(eReplMessage, Plaza2_Trades);
        break;

    case P2StreamID::p2repl_FORTS_ORDLOG_REPL:
        WriteDataToeShmem(eReplMessage, Plaza2_AllData);
        if (data->msg_index == FORTS_ORDLOG_REPL::sys_events_index && 
			CheckNewSession(static_cast<FORTS_ORDLOG_REPL::sys_events*>(data->data)->event_type))
        {
            CopyEDBFilesToNewFolder();
        }
        break;

    case P2StreamID::p2repl_FORTS_FUTINFO_REPL:
    case P2StreamID::p2repl_FORTS_OPTINFO_REPL:
    case P2StreamID::p2repl_FORTS_FUTCOMMON_REPL:
    case P2StreamID::p2repl_FORTS_OPTCOMMON_REPL:
    case P2StreamID::p2repl_FORTS_MM_REPL:
    case P2StreamID::p2repl_RTS_INDEX_REPL:
    case P2StreamID::p2repl_FORTS_MISCINFO_REPL:
    case P2StreamID::p2repl_FORTS_VOLAT_REPL:
        WriteDataToeShmem(eReplMessage, Plaza2_AllData);
        //WriteDataToeShmem(eReplMessage, Plaza2_Info);
        break;
    }

    return res;
}