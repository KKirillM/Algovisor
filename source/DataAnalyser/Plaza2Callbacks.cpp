#include "stdafx.h"
#include "..\Plaza2Utils\EDBStorageReader.h"
#include "..\Utils\Logger.h"
#include "..\Plaza2_Connector\Plaza2_Tables.h"
#include "..\Plaza2_Connector\IPlaza2_Connector.h"
#include "DataAnalyzerApp.h"
#include "Plaza2Callbacks.h"



#define MODULE_NAME "Plaza2Callbacks"

#define INCOME_DATA_LOG(message, ...) PRINT_TO_LOG((*orderlogLogger.get()), LOG_TRACE, message, __VA_ARGS__)
#define RTT_LOG(message, ...) PRINT_TO_LOG((*roundTripLogger.get()), LOG_TRACE, message, __VA_ARGS__)
#define EDB_CONTENT_LOG(message, ...) PRINT_TO_LOG((*edbContentLogger.get()), LOG_TRACE, message, __VA_ARGS__)

static Plaza2Callbacks* gPtr = NULL;

inline const string getOrderStatusString(int status)
{
    string res;

    if (status & QUOTE) res += "QUOTE ";
    else if (status & OPPOSITE) res += "OPPOSITE ";
    else if (status & FILL_OR_KILL) res += "FILL_OR_KILL ";

    if (status & NON_SYSTEMIC) res += "NON_SYSTEMIC ";
    if (status & LAST_IN_RANSACTION) res += "LAST_IN_RANSACTION ";
    if (status & CROSS_TRADE_DELETE) res += "CROSS_TRADE_DELETE ";
    if (status & MONEY_DONT_CHECK) res += "MONEY_DONT_CHECK ";

    if (status & ORDER_MOVE) res += "ORDER_MOVE ";
    if (status & ORDER_DELETE) res += "ORDER_DELETE ";
    if (status & GROUP_DELETE) res += "GROUP_DELETE ";

    return res;
}

#define SESSION_DATA_READY              1      // message = "session_data_ready",          Закончена загрузка данных из клиринговой системы в торговую перед началом новой торговой сессии
#define INTRADAY_CLEARING_FINISHED      2      // message = "intraday_clearing_finished",  Все расчетные процедуры в промклиринге закончены
#define INTRADAY_CLEARING_STARTED       4      // message = "intraday_clearing_started",   Начало промклиринга
#define CLEARING_STARTED                5      // message = "clearing_started",            Начало основного клиринга
#define EXTENSION_OF_LIMITS_FINISHED    6      // message = "extension_of_limits_finished" Раздвижка лимитов закончена
#define BROKER_RECALC_FINISHED          8      // message = "broker_recalc_finished"       Денежные средства после промклиринга пересчитаны
#define POSTLS_RECALC_STARTED          11 
#define SESSION_DATA_READY_FOR_CORE    12
#define BROKER_RECALC_STARTED          13

std::string GetSysEventString(const int event_type)
{
    std::string str;
    switch (event_type)
    {
    case SESSION_DATA_READY:            str = "SESSION_DATA_READY"; break;
    case INTRADAY_CLEARING_FINISHED:    str = "INTRADAY_CLEARING_FINISHED"; break;
    case INTRADAY_CLEARING_STARTED:     str = "INTRADAY_CLEARING_STARTED"; break;
    case CLEARING_STARTED:              str = "CLEARING_STARTED"; break;
    case EXTENSION_OF_LIMITS_FINISHED:  str = "EXTENSION_OF_LIMITS_FINISHED"; break;
    case BROKER_RECALC_FINISHED:        str = "BROKER_RECALC_FINISHED"; break;
    case POSTLS_RECALC_STARTED:         str = "POSTLS_RECALC_STARTED"; break;
    case SESSION_DATA_READY_FOR_CORE:   str = "SESSION_DATA_READY_FOR_CORE"; break;
    case BROKER_RECALC_STARTED:         str = "BROKER_RECALC_STARTED"; break;
    default:                            str = "UNKNOWN_SYS_EVENT"; break;
    }

    return str;
}


Plaza2Callbacks::Plaza2Callbacks(const bool& abortFlag)  
    : bAbortOperation(abortFlag)
    , isTradingPeriod(false)
    , sessionData(NULL)
	, lastCoreTime(0)
    , revChecherForEDBFile("EDB-file")
    , revChecherForORDLOG("ORDLOG")
    , revChecherForDEALS("DEALS")
{
    gPtr = this;
}


Plaza2Callbacks::~Plaza2Callbacks()
{

}


P2ERR Plaza2Callbacks::MainCallback(const TEReplRec* pRec, void* param)
{
    if (gPtr->AbortCheck())
    {
        return P2ERR_COMMON_EXITCODE_TERMINATED;
    }

    // проверка правильности последовательности ревизий в edb-файле
    if (!gPtr->revChecherForEDBFile.check(pRec->rev))
    {
        P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_ERROR, MODULE_NAME, "Wrong revision sequence in EDB-file"));
        return P2ERR_COMMON_INTERNAL_ERROR;
    }

    /*Plaza2Callbacks* callback = static_cast<Plaza2Callbacks*>(param);*/

    // логирование оглавления содержимого edb-файла
    if (gPtr->logEdbContent())
    {
        gPtr->EdbContentToLog(pRec);
        return P2ERR_OK;
    }

	UINT32 expectedMessageSize = 0;
	Plaza2Callbacks::callbackPtr funcPtr = NULL;
    switch (pRec->eventType & ~(0xFF))
    {
    case P2StreamID::p2repl_FORTS_UNKNOWN:
        P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_ERROR, MODULE_NAME, "message type: FORTS_UNKNOWN is wrong"));
        return P2ERR_NOT_OK;

    case P2StreamID::p2repl_FORTS_DEALS_REPL:
        if (pRec->eventType == P2StreamID::p2repl_FORTS_DEALS_REPL + FORTS_DEALS_REPL::deal_index)
        {
			expectedMessageSize = FORTS_DEALS_REPL::sizeof_deal;
			funcPtr = &Plaza2Callbacks::deal;
        }
        break;

    case P2StreamID::p2repl_FORTS_ORDLOG_REPL:
        if (pRec->eventType == P2StreamID::p2repl_FORTS_ORDLOG_REPL + FORTS_ORDLOG_REPL::orders_log_index)
        { 
			expectedMessageSize = FORTS_ORDLOG_REPL::sizeof_orders_log;
			funcPtr = &Plaza2Callbacks::orders_log;
        }
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_ORDLOG_REPL + FORTS_ORDLOG_REPL::sys_events_index)
        {
			expectedMessageSize = FORTS_ORDLOG_REPL::sizeof_sys_events;
			funcPtr = &Plaza2Callbacks::sys_events;
        }
        break;

    case P2StreamID::p2repl_FORTS_FUTINFO_REPL:
        if (pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::fut_sess_contents_index)
        { 
			expectedMessageSize = FORTS_FUTINFO_REPL::sizeof_fut_sess_contents;
			funcPtr = &Plaza2Callbacks::fut_sess_contents;
        }
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::fut_sess_settl_index)
        {
			expectedMessageSize = FORTS_FUTINFO_REPL::sizeof_fut_sess_settl;
			funcPtr = &Plaza2Callbacks::fut_sess_settl;
        }
        break;

    case P2StreamID::p2repl_FORTS_OPTINFO_REPL:
        if (pRec->eventType == P2StreamID::p2repl_FORTS_OPTINFO_REPL + FORTS_OPTINFO_REPL::opt_sess_contents_index)
        {
			expectedMessageSize = FORTS_OPTINFO_REPL::sizeof_opt_sess_contents;
			funcPtr = &Plaza2Callbacks::opt_sess_contents;
        }
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_OPTINFO_REPL + FORTS_OPTINFO_REPL::opt_sess_settl_index)
        {
			expectedMessageSize = FORTS_OPTINFO_REPL::sizeof_opt_sess_settl;
			funcPtr = &Plaza2Callbacks::opt_sess_settl;
		}
		break;

    case P2StreamID::p2repl_FORTS_FUTCOMMON_REPL:

        break;

    case P2StreamID::p2repl_FORTS_OPTCOMMON_REPL:

        break;

    case P2StreamID::p2repl_FORTS_MM_REPL:

        break;

    case P2StreamID::p2repl_RTS_INDEX_REPL:

        break;

    case P2StreamID::p2repl_FORTS_MISCINFO_REPL:

        break;

    case P2StreamID::p2repl_FORTS_VOLAT_REPL:

        break;

	default:
		P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_ERROR, MODULE_NAME, "message type: unknown message type"));
		return P2ERR_NOT_OK;
    }

	if (expectedMessageSize == 0)
	{
		return P2ERR_OK;
	}

	UINT32 incomeMessageSize = pRec->dataSize - sizeof(fo2base::UnixNanosecondsTime);	// вычтем из размера заголовка, размер поля с временем, чтобы получить чистый размер полезных данных
	if (incomeMessageSize != expectedMessageSize)
	{
		P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_ERROR, MODULE_NAME, "incomeMessageSize error"));
		return P2ERR_NOT_OK;
	}

	int64_t* ptr = (int64_t*)((char*)pRec + sizeof(TEReplRec));
	int64_t localTime = *ptr + fo2base::timezoneShift.get();
	char* beginOfData = (char*)ptr + sizeof(localTime);

	(gPtr->*funcPtr)(beginOfData, localTime);

    return P2ERR_OK;
}

void Plaza2Callbacks::InitParams(const OptionsParams& _params)
{
    params = _params;
}

void Plaza2Callbacks::StartSessionRange()
{
    sessionData = gGetTradeSessionData();

    statistics.Init(sessionData);

	if (NeedBacktest())
	{
		strategyTester.reset(new BackTester());
		strategyTester.get()->CreateStrategies(params.roundTripDelay, *sessionData, params.pathToLogs);
	}
}

void Plaza2Callbacks::StartNewSession(const std::string& sessionName)
{
	if (logORDLOG() || logDEALS() || logOrderBook() || logAggrTrades() || logSplitTrades())
	{
		orderlogLogger.reset(new FileLogger(params.pathToLogs, params.fileNamePrefix + "_OrderLog_(" + sessionName + ").log"));
	}

	if (logEdbContent())
	{
		edbContentLogger.reset(new FileLogger(params.pathToLogs, params.fileNamePrefix + "_EDB_Content_(" + sessionName + ").log"));
	}

	if (logRTT())
	{
		roundTripLogger.reset(new FileLogger(params.pathToLogs, params.fileNamePrefix + "_RTT_(" + sessionName + ").log"));
		RTT_LOG("%s", "LOCAL_TIME; CORE_TIME; EVENT_COUNT; AVG_LATENCY")
	}

    sessionData->ClearAllData();
    statistics.ResetSessionStatistics();
    if (orderlogLogger)
    {
        INCOME_DATA_LOG("ORDLOG; New session started, all order books are empty")
    }

    ResetRevisionCheckers();
}

void Plaza2Callbacks::EndSession()
{
    const std::string dateString = fo2base::CP2Time::FromUnixNanosecondsToDateString(lastCoreTime, '-');

    if (logTradesStatistics())
    {
        for (const int isin_id : params.isinIDList)
        {
            TradesStatisticsToFile(isin_id, dateString);
        }
    }

    if (logSessionStatistics())
    {
        PerSessionStatisticsToFile(dateString);
        statistics.CalculateMultiSessionStatistics(lastCoreTime);
    }

    if (NeedBacktest())
    {
		strategyTester->EndSessionAction(dateString);
        const std::string result = strategyTester->GetStrategyResult();
        P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_INFO, MODULE_NAME, "%s%s ...", "\nBacktesting result:\n\n", result.c_str()))
        
    }

    if (orderlogLogger)
    {
        INCOME_DATA_LOG("ORDLOG; New session started, clean all order books...")
    }
    sessionData->ClearAllData();
    statistics.ResetSessionStatistics();
    //gDelTradeSessionData();
}

void Plaza2Callbacks::EndSessionRange()
{
    if (bAbortOperation)
    {
        return;
    }

    if (logSessionStatistics())
    {
        MultiSessionStatisticsToFile();
    }

	if (NeedBacktest())
	{
		strategyTester->EndSessionRangeAction();
		strategyTester.reset();
	}
}

void Plaza2Callbacks::ResetRevisionCheckers()
{
    revChecherForEDBFile.Reset();
    revChecherForORDLOG.Reset();
    revChecherForDEALS.Reset();
}

void Plaza2Callbacks::fut_sess_contents(void* data, const fo2base::UnixNanosecondsTime localTime)
{
    FORTS_FUTINFO_REPL::fut_sess_contents& info = *reinterpret_cast<FORTS_FUTINFO_REPL::fut_sess_contents*>(data);
    bool goodISIN = logIsinID(info.isin_id);
    if (!goodISIN)
    {
        return;
    }
    
    sessionData->PushFutSessionContents(info);
}

void Plaza2Callbacks::opt_sess_contents(void* data, const fo2base::UnixNanosecondsTime localTime)
{
    FORTS_OPTINFO_REPL::opt_sess_contents& info = *reinterpret_cast<FORTS_OPTINFO_REPL::opt_sess_contents*>(data);
    bool goodISIN = logIsinID(info.isin_id);
    if (!goodISIN)
    {
        return;
    }

    sessionData->PushOptSessionContents(info);
}

void Plaza2Callbacks::fut_sess_settl(void* data, const fo2base::UnixNanosecondsTime localTime)
{
    FORTS_FUTINFO_REPL::fut_sess_settl& info = *reinterpret_cast<FORTS_FUTINFO_REPL::fut_sess_settl*>(data);
    bool goodISIN = logIsinID(info.isin_id);
    if (!goodISIN)
    {
        return;
    }

    sessionData->PushFutInstrument(info);
}

void Plaza2Callbacks::opt_sess_settl(void* data, const fo2base::UnixNanosecondsTime localTime)
{
    FORTS_OPTINFO_REPL::opt_sess_settl& info = *reinterpret_cast<FORTS_OPTINFO_REPL::opt_sess_settl*>(data);
    bool goodISIN = logIsinID(info.isin_id);
    if (!goodISIN)
    {
        return;
    }

    sessionData->PushOptInstrument(info);
}

void Plaza2Callbacks::sys_events(void* data, const fo2base::UnixNanosecondsTime localTime)
{
    FORTS_ORDLOG_REPL::sys_events& info = *reinterpret_cast<FORTS_ORDLOG_REPL::sys_events*>(data);

    if (logORDLOG() || logDEALS() || logOrderBook() || logAggrTrades() || logSplitTrades())
    {
        std::string timeString;
        fo2base::CP2Time::FromUnixNanosecondsToString2(localTime, timeString);
        std::string eventString = GetSysEventString(info.event_type);
        INCOME_DATA_LOG("SYS_EVENT; local_moment=%s; %s", timeString.c_str(), eventString.c_str());
    }

    switch (info.event_type)
    {
    case SESSION_DATA_READY:  
		if (NeedBacktest())
		{
			strategyTester->InitStrategies(lastCoreTime);
		}
		isTradingPeriod = true;
		break;

    case INTRADAY_CLEARING_FINISHED:
        isTradingPeriod = true;
        break;

    case CLEARING_STARTED:
        EndSession();
		isTradingPeriod = false;
		break;

    case INTRADAY_CLEARING_STARTED:
        isTradingPeriod = false;
        break;
    }
}

void Plaza2Callbacks::orders_log(void* data, const fo2base::UnixNanosecondsTime localTime)
{
    FORTS_ORDLOG_REPL::orders_log& info = *reinterpret_cast<FORTS_ORDLOG_REPL::orders_log*>(data);
    info.moment_ns += fo2base::timezoneShift.get();
	lastCoreTime = info.moment_ns;

    // проверка правильности последовательности ревизий в потоке репликации FORTS_ORDLOG_REPL
    if (!revChecherForORDLOG.check(info.replID))
    {
        return;
    }

    if (logRTT())
    {
        RTTToFile(info, localTime);
    }

    int instrumentID = info.isin_id;
    bool goodISIN = logIsinID(instrumentID);
    if (!goodISIN)
    {
        return;
    }

    bool goodTime = true;
    if (params.checkTimeInterval)
    {
        fo2base::CP2Time eventTime(fo2base::CP2Time::FromUnixNanosecondsTime(info.moment_ns));
        goodTime = (eventTime.CompareTime(params.startEventTime) >= 0 && eventTime.CompareTime(params.endEventTime) <= 0) ? true : false;
    }

    if (logORDLOG() && goodTime)
    {
        double dPrice = 0, dDealPrice = 0;
        P2BCDII2Double(&dPrice, info.price);
        P2BCDII2Double(&dDealPrice, info.deal_price);
        
        std::string coreTime/*, localTime*/;
        fo2base::CP2Time::FromUnixNanosecondsToString2(info.moment_ns, coreTime);
        /*fo2base::CP2Time::FromUnixNanosecondsToString2(time, localTime);*/

        INCOME_DATA_LOG("ORDLOG; moment=%s; sess_id=%d; isin_id=%d; id_ord=%lld; action=%6s; dir=%s; "
                        "price=%f; amount=%d; deal_price=%f; amount_rest=%d; id_deal=%lld; xstatus=0x%llx; status=0x%x; Status=%s; replID=%lld; replRev=%lld; replAct=%lld",
                        /*time-info.moment_ns,*/			// дельта между временем события в ядре и временем локальной машине
                        /*localTime.c_str(),*/				// время события на локальной машине
                        coreTime.c_str(),				// время события в ядре
                        info.sess_id,                   // Идентификатор торговой сессии
                        info.isin_id,                   // Уникальный числовой идентификатор инструмента
                        info.id_ord,                    // Номер заявки
                        ((info.action == P2Action::REMOVE) ? "REMOVE" : (info.action == P2Action::ADD) ? "ADD" : "TRADE"),  // Действие с заявкой
                        (info.dir == P2Dir::ASK ? "Ask" : "Bid"),   // Направление
                        dPrice,                         // Цена
                        info.amount,                    // Количество в операции
                        dDealPrice,                     // Цена заключенной сделки
                        info.amount_rest,               // Оставшееся количество в заявке (в книге которая стоит)
                        info.id_deal,                   // Идентификатор сделки по данной записи журнала заявок
                        info.xstatus,                   // Расширенный статус заявки
                        info.status,                    // Статус заявки
                        getOrderStatusString(info.status).c_str(),
                        info.replID,                    // Служебное поле подсистемы репликации
                        info.replRev,                   // Служебное поле подсистемы репликации
                        info.replAct)                   // Служебное поле подсистемы репликации        
    }

    if (NeedBacktest() && isTradingPeriod)
    {
        strategyTester.get()->PreEventActions(info);
    }

    if (info.status & NON_SYSTEMIC)   // внесистемные(адресные) заявки пока не смотрим
    {
        return;
    }

    sessionData->PushOrderLogAction(info);

    if (logOrderBook() && goodTime)
    {
        std::string dump = sessionData->DumpBookToString(instrumentID, params.orderBookLevels);
		INCOME_DATA_LOG("%s", dump.c_str());
    }

	bool logSessionStat = logSessionStatistics();
	if (logSessionStat)
	{
		statistics.AddBBOOrderSize(instrumentID);
		statistics.CalculateSpreadChangesCount(instrumentID);
		statistics.CalculateHourlyStatistics(instrumentID, info.moment.hour);
		statistics.SetTotalTradesCount(instrumentID, sessionData->aggrTradesMap[instrumentID].size());
	}

    if (NeedBacktest() && isTradingPeriod)
    {
        strategyTester.get()->PostEventActions(info);
    }

	bool logTradesStat = logTradesStatistics();
    if (info.action == P2Action::ADD)
    {
        if (logSessionStat)
        {
            statistics.AddOrderStatistic(instrumentID, info.amount);
        }

        if (logSessionStat || logTradesStat)
        {
            if (info.status & ORDER_MOVE)
            {
                statistics.IncMoveOrderCounter(instrumentID);
            }
            else
            {
                statistics.IncAddOrderCounter(instrumentID);
            }
        }
    }
    else if (info.action == P2Action::REMOVE)
    {
        if (logSessionStat || logTradesStat)
        {
            if (info.status & ORDER_DELETE || info.status & GROUP_DELETE)
            {
                statistics.IncDelOrderCounter(instrumentID);
            }
        }
    }
}

void Plaza2Callbacks::deal(void* data, const fo2base::UnixNanosecondsTime localTime)
{
	FORTS_DEALS_REPL::deal& info = *reinterpret_cast<FORTS_DEALS_REPL::deal*>(data);
    info.moment_ns += fo2base::timezoneShift.get();

    // проверка правильности последовательности ревизий в потоке репликации FORTS_DEALS_REPL
    if (!revChecherForDEALS.check(info.replID))
    {
        return;
    }

    int currInstrumentID = info.isin_id;
    bool goodISIN = logIsinID(currInstrumentID);
    if (!goodISIN)
    {
        return;
    }

    bool goodTime = true;
    if (params.checkTimeInterval)
    {
        fo2base::CP2Time eventTime(fo2base::CP2Time::FromUnixNanosecondsTime(info.moment_ns));
        goodTime = (eventTime.CompareTime(params.startEventTime) >= 0 && eventTime.CompareTime(params.endEventTime) <= 0) ? true : false;
    }

    if (logDEALS() && goodTime)
    {
        double dPrice = 0;
        P2BCDII2Double(&dPrice, info.price);

        std::string timeString;
        fo2base::CP2Time::FromUnixNanosecondsToString2(info.moment_ns, timeString);

        INCOME_DATA_LOG("DEAL; moment=%s; sess_id=%d; isin_id=%d; id_deal=%lld; pos=%d; price=%f; amount=%d; id_ord_buy=%lld; id_ord_sell=%lld; nosystem=%d; replID=%lld; replRev=%lld; replAct=%lld",
                        timeString.c_str(),
                        info.sess_id,                   // Идентификатор торговой сессии
                        info.isin_id,                   // Уникальный числовой идентификатор инструмента
                        info.id_deal,                   // Номер сделки
                        info.pos,                       // Кол-во позиций по инструменту на рынке после сделки
                        dPrice,                         // Цена
                        info.amount,                    // Объем, кол-во единиц инструмента
                        info.id_ord_buy,                // Номер заявки покупателя
                        info.id_ord_sell,               // Номер заявки продавца
                        info.nosystem,                  // Признак внесистемной сделки
                        info.replID,                    // Служебное поле подсистемы репликации
                        info.replRev,                   // Служебное поле подсистемы репликации
                        info.replAct)                   // Служебное поле подсистемы репликации
    }

    if (info.nosystem != 0)   // внесистемные(адресные) сделки пока не смотрим
    {
        return;
    }

    sessionData->PushDealAction(info);

    if (NeedBacktest() && isTradingPeriod)
    {
        strategyTester.get()->PostEventActions(info);
    }
}

void Plaza2Callbacks::EdbContentToLog(const TEReplRec* pRec)
{
    int64_t* pTime = (int64_t*)((char*)pRec + sizeof(TEReplRec));
    int64_t time = *pTime + fo2base::timezoneShift.get();
    char* beginOfData = (char*)pTime + sizeof(pTime);

    std::string tableName;
    P2StreamID replicationThreadType = static_cast<P2StreamID>(pRec->eventType & ~(0xFF));

    switch (replicationThreadType)
    {
    case P2StreamID::p2repl_FORTS_UNKNOWN:  tableName = "FORTS_UNKNOWN"; 
        
        break;

    case P2StreamID::p2repl_FORTS_DEALS_REPL:
        if (pRec->eventType == P2StreamID::p2repl_FORTS_DEALS_REPL + FORTS_DEALS_REPL::deal_index)      
        {
            FORTS_DEALS_REPL::deal& info = *reinterpret_cast<FORTS_DEALS_REPL::deal*>(beginOfData);
            tableName = "FORTS_DEALS_REPL::deal" " isin_id=" + to_string(info.isin_id);
        }
        else if (pRec->eventType == P2StreamID::p2repl_FORTS_DEALS_REPL + FORTS_DEALS_REPL::multileg_deal_index)    tableName = "FORTS_DEALS_REPL::multileg_deal";
        else if (pRec->eventType == P2StreamID::p2repl_FORTS_DEALS_REPL + FORTS_DEALS_REPL::heartbeat_index)        tableName = "FORTS_DEALS_REPL::heartbeat"; 
        else if (pRec->eventType == P2StreamID::p2repl_FORTS_DEALS_REPL + FORTS_DEALS_REPL::sys_events_index)       tableName = "FORTS_DEALS_REPL::sys_events";

        break;

    case P2StreamID::p2repl_FORTS_ORDLOG_REPL:
        if (pRec->eventType == P2StreamID::p2repl_FORTS_ORDLOG_REPL + FORTS_ORDLOG_REPL::orders_log_index)  
        {
            FORTS_ORDLOG_REPL::orders_log& info = *reinterpret_cast<FORTS_ORDLOG_REPL::orders_log*>(beginOfData);
            tableName = "FORTS_ORDLOG_REPL::orders_log" " isin_id=" + to_string(info.isin_id);
        }
        else if (pRec->eventType == P2StreamID::p2repl_FORTS_ORDLOG_REPL + FORTS_ORDLOG_REPL::multileg_orders_log_index)    tableName = "FORTS_ORDLOG_REPL::multileg_orders_log";
        else if (pRec->eventType == P2StreamID::p2repl_FORTS_ORDLOG_REPL + FORTS_ORDLOG_REPL::heartbeat_index)              tableName = "FORTS_ORDLOG_REPL::heartbeat";
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_ORDLOG_REPL + FORTS_ORDLOG_REPL::sys_events_index)
        {
            FORTS_ORDLOG_REPL::sys_events& info = *reinterpret_cast<FORTS_ORDLOG_REPL::sys_events*>(beginOfData);
            std::string eventString = GetSysEventString(info.event_type);
            tableName = "FORTS_ORDLOG_REPL::sys_events; event_type: " + eventString;
        }

        break;

    case P2StreamID::p2repl_FORTS_FUTINFO_REPL:
        if (pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::fut_sess_contents_index)
        {
            FORTS_FUTINFO_REPL::fut_sess_contents& info = *reinterpret_cast<FORTS_FUTINFO_REPL::fut_sess_contents*>(beginOfData);
            tableName = "FORTS_FUTINFO_REPL::fut_sess_contents" " isin_id=" + to_string(info.isin_id);
        }
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::rates_index)                     tableName = "FORTS_FUTINFO_REPL::rates";
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::fut_vcb_index)                   tableName = "FORTS_FUTINFO_REPL::fut_vcb";
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::fut_instruments_index)           tableName = "FORTS_FUTINFO_REPL::fut_instruments";
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::fut_bond_registry_index)         tableName = "FORTS_FUTINFO_REPL::fut_bond_registry";
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::diler_index)                     tableName = "FORTS_FUTINFO_REPL::diler";
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::sys_messages_index)              tableName = "FORTS_FUTINFO_REPL::sys_messages";
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::prohibition_index)               tableName = "FORTS_FUTINFO_REPL::prohibition";
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::RF_index)                        tableName = "FORTS_FUTINFO_REPL::RF";
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::trade_modes_index)               tableName = "FORTS_FUTINFO_REPL::trade_modes";
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::money_transfers_index)           tableName = "FORTS_FUTINFO_REPL::money_transfers";
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::trf_accounts_index)              tableName = "FORTS_FUTINFO_REPL::trf_accounts";
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::trf_accounts_map_index)          tableName = "FORTS_FUTINFO_REPL::trf_accounts_map";
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::multileg_dict_index)             tableName = "FORTS_FUTINFO_REPL::multileg_dict";
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::fut_ts_cons_index)               tableName = "FORTS_FUTINFO_REPL::fut_ts_cons";
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::fut_rejected_orders_index)       tableName = "FORTS_FUTINFO_REPL::fut_rejected_orders";
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::fut_intercl_info_index)          tableName = "FORTS_FUTINFO_REPL::fut_intercl_info";
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::fut_bond_nkd_index)              tableName = "FORTS_FUTINFO_REPL::fut_bond_nkd";
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::fut_bond_nominal_index)          tableName = "FORTS_FUTINFO_REPL::fut_bond_nominal";
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::fut_bond_isin_index)             tableName = "FORTS_FUTINFO_REPL::fut_bond_isin";
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::usd_online_index)                tableName = "FORTS_FUTINFO_REPL::usd_online";
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::investr_index)                   tableName = "FORTS_FUTINFO_REPL::investr";
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::fut_sess_settl_index)            tableName = "FORTS_FUTINFO_REPL::fut_sess_settl";
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::fut_margin_type_index)           tableName = "FORTS_FUTINFO_REPL::fut_margin_type";
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::fut_settlement_account_index)    tableName = "FORTS_FUTINFO_REPL::fut_settlement_account";
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::sys_events_index)                tableName = "FORTS_FUTINFO_REPL::sys_events";
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::session_index)                   tableName = "FORTS_FUTINFO_REPL::session";
        else if(pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::sys_events_index)                tableName = "FORTS_FUTINFO_REPL::sys_events";

        break;

    case P2StreamID::p2repl_FORTS_OPTINFO_REPL:
        if (pRec->eventType == P2StreamID::p2repl_FORTS_OPTINFO_REPL + FORTS_OPTINFO_REPL::opt_sess_contents_index)
        {
            FORTS_OPTINFO_REPL::opt_sess_contents& info = *reinterpret_cast<FORTS_OPTINFO_REPL::opt_sess_contents*>(beginOfData);
            tableName = "FORTS_OPTINFO_REPL::opt_sess_contents" " isin_id=" + to_string(info.isin_id);
        }
        else if (pRec->eventType == P2StreamID::p2repl_FORTS_OPTINFO_REPL + FORTS_OPTINFO_REPL::opt_exprule_index)              tableName = "FORTS_OPTINFO_REPL::opt_exprule";
        else if (pRec->eventType == P2StreamID::p2repl_FORTS_OPTINFO_REPL + FORTS_OPTINFO_REPL::opt_vcb_index)                  tableName = "FORTS_OPTINFO_REPL::opt_vcb";
        else if (pRec->eventType == P2StreamID::p2repl_FORTS_OPTINFO_REPL + FORTS_OPTINFO_REPL::opt_rejected_orders_index)      tableName = "FORTS_OPTINFO_REPL::opt_rejected_orders";
        else if (pRec->eventType == P2StreamID::p2repl_FORTS_OPTINFO_REPL + FORTS_OPTINFO_REPL::opt_intercl_info_index)         tableName = "FORTS_OPTINFO_REPL::opt_intercl_info";
        else if (pRec->eventType == P2StreamID::p2repl_FORTS_OPTINFO_REPL + FORTS_OPTINFO_REPL::opt_exp_orders_index)           tableName = "FORTS_OPTINFO_REPL::opt_exp_orders";
        else if (pRec->eventType == P2StreamID::p2repl_FORTS_OPTINFO_REPL + FORTS_OPTINFO_REPL::opt_sess_settl_index)           tableName = "FORTS_OPTINFO_REPL::opt_sess_settl";
        else if (pRec->eventType == P2StreamID::p2repl_FORTS_OPTINFO_REPL + FORTS_OPTINFO_REPL::sys_events_index)               tableName = "FORTS_OPTINFO_REPL::sys_events";

        break;

    case P2StreamID::p2repl_FORTS_FUTCOMMON_REPL:   tableName = "FORTS_FUTCOMMON_REPL::common";

        break;

    case P2StreamID::p2repl_FORTS_OPTCOMMON_REPL:   tableName = "FORTS_OPTCOMMON_REPL::common";

        break;

    case P2StreamID::p2repl_FORTS_MM_REPL:
        if (pRec->eventType == P2StreamID::p2repl_FORTS_MM_REPL + FORTS_MM_REPL::fut_MM_info_index)                 tableName = "FORTS_MM_REPL::fut_MM_info";
        else if (pRec->eventType == P2StreamID::p2repl_FORTS_MM_REPL + FORTS_MM_REPL::opt_MM_info_index)            tableName = "FORTS_MM_REPL::opt_MM_info";
        else if (pRec->eventType == P2StreamID::p2repl_FORTS_MM_REPL + FORTS_MM_REPL::cs_mm_rule_index)             tableName = "FORTS_MM_REPL::cs_mm_rule";
        else if (pRec->eventType == P2StreamID::p2repl_FORTS_MM_REPL + FORTS_MM_REPL::mm_agreement_filter_index)    tableName = "FORTS_MM_REPL::mm_agreement_filter";

        break;

    case P2StreamID::p2repl_RTS_INDEX_REPL:     tableName = "RTS_INDEX_REPL::rts_index";

        break;

    case P2StreamID::p2repl_FORTS_MISCINFO_REPL:    tableName = "FORTS_MISCINFO_REPL::volat_coeff";
        
        break;

    case P2StreamID::p2repl_FORTS_VOLAT_REPL:       tableName = "FORTS_VOLAT_REPL::volat";
        
        break;
    }

    std::string timeString;
    fo2base::CP2Time::FromUnixNanosecondsToString2(time, timeString);

    EDB_CONTENT_LOG("Local_moment: %s; EDB_Revision: %lld; ReplID: %lld; ReplRev: %lld; %s", timeString.c_str(), pRec->rev, *(long long*)(beginOfData), *(long long*)(beginOfData+8), tableName.c_str())
}

void Plaza2Callbacks::RTTToFile(const FORTS_ORDLOG_REPL::orders_log& info, const fo2base::UnixNanosecondsTime time)
{
    long long delta = abs(long long(time-info.moment_ns));
    const long long fiveSecondsInNanosec = 10000000000;
    if (delta < fiveSecondsInNanosec)   // если дельта больше десяти секунд, то не учитвыем эти данные, чтобы слишком большие значения дельт не искажали общую картину
    {
        if (!orderRTT.isNewTimeInterval(time))
        {
            orderRTT.addNewValue(delta);
        }
        else
        {
            std::string coreTime, localTime;
            fo2base::CP2Time::FromUnixNanosecondsToString2(info.moment_ns, coreTime);
            fo2base::CP2Time::FromUnixNanosecondsToString2(time, localTime);

            uint64_t avgLatencyPerInterval = orderRTT.calc();
            uint64_t eventsCountPerInterval = orderRTT.getCounter();
            RTT_LOG("%s; %s; %15llu; %20llu", localTime.c_str(), coreTime.c_str(), eventsCountPerInterval, avgLatencyPerInterval)
            orderRTT.setNewIntervalValues(time, delta);
        }
    }
}

void Plaza2Callbacks::TradesStatisticsToFile(const int isin_id, const std::string& date)
{
#define SPLITTRADE_LOG(message, ...) PRINT_TO_LOG((*tradesStatisticsLogger.get()), LOG_TRACE, message, __VA_ARGS__)

    std::string fileName = params.fileNamePrefix + "_TradesStatistics_" + sessionData->GetIsinFromId(isin_id).c_str() + '_' + '(' + date + ')' + ".csv";
    
    std::unique_ptr<FileLogger> tradesStatisticsLogger;   // файл-логер раздробленных трейдов
    tradesStatisticsLogger.reset(new FileLogger(params.pathToLogs, fileName));

    std::string tableHeader;
    tableHeader = "TIME;"                          // TIME - время трейда в формате ЧЧ:ММ:СС
                  "SPLIT_PRICE;"                   // SPLIT_PRICE - цена раздробленного трейда
                  "SPLIT_SIZE;"                    // SPLIT_SIZE - размер раздробленного трейда
                  "SIDE;"                          // SIDE - направление трейда: B - покупка, S - продажа
                  "INITIAL_PRICE;"                 // INITIAL_PRICE - цена, по которой был выставлен ордер привёдший к трейду
                  "INITIAL_SIZE;"                  // INITIAL_SIZE - размер, который был указан в заявке инициировавшей трейд
                  "TRADE_SIZE;"                    // TRADE_SIZE - размер агрегированного трейда
                  "OI_CHANGE;"                     // OI_CHANGE - направление изменения открытого интереса в трейде: OPEN - поизиция открыта, CLOSE - позиция закрыта, UNKNOWN - не известно
                  "ORDER_TYPE;"                    // ORDER_TYPE - вида заявки, которая породила трейд: FAS - котировочная, IOC - встречная, FAK - Fill-and-Kill, SYS - системная(адресная) сделка
                  "OPPOSITE_ORDER_SIZE;"           // OPPOSITE_ORDER_SIZE - размер заявки, которая была на противоположной стороне для агрегированного трейда
                  "PRICE_LEVEL_ORDERS_COUNT;"      // PRICE_LEVEL_CUMULATIVE_SIZE - суммарный размер всех заявок на ценовом уровне, по которому прошёл агрегированный трейд
                  "PRICE_LEVEL_CUMULATIVE_SIZE;"   // PRICE_LEVEL_ORDERS_COUNT - количество заявок на ценовом уровне, по которому прошёл агрегированный трейд
                  "MAX_OPPOSITE_ORDER_SIZE;"       // MAX_OPPOSITE_ORDER_SIZE - максимальный размер заявки противоположной стороны на ценовом уровне, по которому прошёл агрегированный трейд
                  "EXECUTED_LEVELS;"               // EXECUTED_LEVELS - количество ценовых уровней, которые полностью были "очищены" агрегированным трейдом
                  "OPPOSITE_MAX_ORDER_TIMER;"      // OPPOSITE_MAX_ORDER_TIMER - количество миллисекунд, которое отстояла самая крупная на ценовом уровне заявка до сведения с агрегированным трейдом
                  "TOTAL_BID_SIZE;"                // TOTAL_BID_SIZE - суммарное количество размеров заявок на всех ценовых уровнях бидов
                  "TOTAL_ASK_SIZE;"                // TOTAL_ASK_SIZE - суммарное количество размеров заявок на всех ценовых уровнях асков
                  "TOTAL_BID_ORDERS_COUNT;"        // TOTAL_BID_ORDERS_COUNT - суммарное количество заявок на всех ценовых уровнях бидов
                  "TOTAL_ASK_ORDERS_COUNT;";       // TOTAL_ASK_ORDERS_COUNT - суммарное количество заявок на всех ценовых уровнях асков

    if (NeedBacktest())
    {
        tableHeader += "TRADE_WAPRICE;"            // TRADE_WAPRICE - цена трейда стратегии
                       "POSITION_WAPRICE;"         // POSITION_WAPRICE - средневзвешенная цена текущей позиции
                       "POSITION_SIZE;"            // POSITION_SIZE - накопленная позиция знак + лонг, знак - шорт
                       "REALISED_PNL;"             // PNL - накопленный прибыль/убыток
                       "UNREALISED_PNL;"           // UNREALISED_PNL - нереализованная прибыль/убыток 
					   "INDICATOR_1;"			   // INDICATOR_1 - первый индикатор для торговой системы
				       "INDICATOR_2;";			   // INDICATOR_2 - второй индикатор для торговой системы
    }

    SPLITTRADE_LOG("%s", tableHeader.c_str())

    sessionData->StartEnumAggrTrades(isin_id);
    AggregatedTrade aggrTrade;
    bool endFlag = sessionData->GetNextAggrTrade(aggrTrade, isin_id);

    sessionData->StartEnumSplitTrades(isin_id);
    SplitTrade splitTrade;

    const SOrderEx* prevTrade = NULL;
    while(sessionData->GetNextSplitTrade(splitTrade, isin_id))
    {
        char buf[1000] = {0};
        int size = 0;
        bool goodTime = true;
        if (params.checkTimeInterval)
        {
			fo2base::CP2Time eventTime(fo2base::CP2Time::FromUnixNanosecondsTime(splitTrade.time));
            goodTime = (eventTime.CompareTime(params.startEventTime) >= 0 && eventTime.CompareTime(params.endEventTime) <= 0) ? true : false;
        }

        if (goodTime)
        {
			//std::string stringTime;
			//fo2base::CP2Time::FromUnixNanosecondsToString3(splitTrade.time, stringTime);
			DateFormatParser parser(params.dateFormat, splitTrade.time);
            size = sprintf_s(buf, "%s;%s;%lld;%c;", (const char*)parser, splitTrade.price.GetString(Price::COMMA).c_str(), splitTrade.size, splitTrade.agressorSide);

            while (endFlag) 
            {
                if (aggrTrade.time < splitTrade.time)
                {
                    endFlag = sessionData->GetNextAggrTrade(aggrTrade, isin_id);
                    continue;
                }
                else if (aggrTrade.time > splitTrade.time)
                {
                    size += sprintf_s(buf+size, 1000-size, ";;;;;;;;;;;;;;;");
                    break;
                } 
                else if (aggrTrade.time == splitTrade.time)
                {
                    size += sprintf_s(buf+size, 1000-size, "%s;%d;%d;%s;%s;", aggrTrade.initialPrice.GetString(Price::COMMA).c_str(), aggrTrade.initialSize, aggrTrade.matchSize, 
                        aggrTrade.IODir == OPEN_POSITION ? "OPEN" : aggrTrade.IODir == CLOSE_POSITION ? "CLOSE" : "UNKNOWN", 
                        (aggrTrade.status & QUOTE) ? "FAS" : (aggrTrade.status & OPPOSITE) ? "IOC" : (aggrTrade.status & FILL_OR_KILL) ? "FOK" : (aggrTrade.status & NON_SYSTEMIC) ? "SYS" : "");
                    size += sprintf_s(buf+size, 1000-size, "%lld;%lld;%lld;%lld;%d;%lld;", aggrTrade.oppositeOrderSize, aggrTrade.priceLevelOrdersCount, aggrTrade.priceLevelCumulativeSize,
                        aggrTrade.maxPriceLevelOrderSize, aggrTrade.executedPriceLevelsCount, aggrTrade.oppositeMaxOrderDeltaTime/1000000);
                    size += sprintf_s(buf+size, 1000-size, "%lld;%lld;%lld;%lld;", aggrTrade.totalBidSize, aggrTrade.totalAskSize, aggrTrade.totalBidOrdersCount, aggrTrade.totalAskOrdersCount);
                    endFlag = sessionData->GetNextAggrTrade(aggrTrade, isin_id);
                }

                break;
            }

            // данные о результатах торговой стратегии
            if (NeedBacktest())
            {
                const SOrderEx* currTrade = strategyTester.get()->GetTrade(isin_id, splitTrade.time);
                if (currTrade && prevTrade != currTrade)
                {
                    size += sprintf_s(buf+size, 1000-size, "%f;", currTrade->realPrice);
                    prevTrade = currTrade;
                }
                else
                {
                    size += sprintf_s(buf+size, 1000-size, ";");
                }

                const Position* posPtr = strategyTester.get()->GetIntermediatePositionResult(isin_id, splitTrade.time);
                if (posPtr)
                {
                    const int pos = posPtr->GetPos();
                    if (pos == 0)
                    {
                        size += sprintf_s(buf+size, 1000-size, ";0;");
                    }
                    else
                    {
                        const double WAPrice = posPtr->GetWAPrice();
                        size += sprintf_s(buf+size, 1000-size, "%f;%d;", WAPrice, pos);
                    }

                    const double PnL = posPtr->GetClosedPnL();
                    const double unPnL = posPtr->GetUnrealisedPnL(splitTrade.price.ToDouble());
					size += sprintf_s(buf+size, 1000-size, "%f;%f;", PnL, unPnL);
                }
                else
                {
					size += sprintf_s(buf+size, 1000-size, ";;;;");
                }

				const double* indicator1 = strategyTester.get()->GetIndicator1(isin_id, splitTrade.time);
				if (indicator1)
				{
					size += sprintf_s(buf + size, 1000 - size, "%f;", *indicator1);
				}
				else
				{
					size += sprintf_s(buf + size, 1000 - size, ";");
				}

				const double* indicator2 = strategyTester.get()->GetIndicator2(isin_id, splitTrade.time);
				if (indicator2)
				{
					size += sprintf_s(buf + size, 1000 - size, "%f;", *indicator2);
				}
				else
				{
					size += sprintf_s(buf + size, 1000 - size, ";");
				}

                // заменяем в результирующей строке все точки на запятые (это для экселя нужно)
                char* pch;
                while ((pch=strchr(buf,'.')) != NULL)
                {
                    *pch = ',';
                }
            }
        }

        SPLITTRADE_LOG("%s", buf)
    }
}

void Plaza2Callbacks::PerSessionStatisticsToFile(const std::string& date)
{
#define SESSION_STATISTICS_LOG(message, ...) PRINT_TO_LOG((*sessionStatisticsLogger.get()), LOG_TRACE, message, __VA_ARGS__)

    std::string fileName = params.fileNamePrefix + "_SessionStatistics_" + '(' + date + ')' + ".csv";

    std::unique_ptr<FileLogger> sessionStatisticsLogger;
    sessionStatisticsLogger.reset(new FileLogger(params.pathToLogs, fileName));

    std::map<InstrumentID, std::map<TradeSize, Frequency>> frequencyTradeStatistic;
    GetFrequencyTradeStatistic(frequencyTradeStatistic);

    std::map<InstrumentID, std::map<PriceLevelCount, Frequency>> depthOfPenetrationStatistic;
    GetDepthOfPenetrationStatistic(depthOfPenetrationStatistic);

    statistics.PrepareHourlyStatistics();

    std::string tableString, tableString1, tableString2, tableString3;
    for (const std::pair<InstrumentID, SessionStatistics>& mainPair : statistics.GetSessionStatisticsData())
    {
        const InstrumentID isin_id = mainPair.first;
        if (!sessionData->isFutures(isin_id))	// без опционов, только фьючерсы
        {
            continue;
        }

        std::string isin = sessionData->GetIsinFromId(isin_id);
        tableString.clear();
        tableString = isin + '(' + std::to_string(isin_id) + ')' + ";";
        SESSION_STATISTICS_LOG("%s", tableString.c_str())

        tableString = "Add orders count;"; // строка со значением общего количества добавленных заявок
        tableString += std::to_string(mainPair.second.addOrderEventCount);
        SESSION_STATISTICS_LOG("%s", tableString.c_str())

        tableString = "Del orders count;"; // строка со значением общего количества удалённых заявок
        tableString += std::to_string(mainPair.second.delOrderEventCount);
        SESSION_STATISTICS_LOG("%s", tableString.c_str())

        tableString = "Move orders count;"; // строка со значением общего количества перемещённых заявок
        tableString += std::to_string(mainPair.second.moveOrderEventCount);
        SESSION_STATISTICS_LOG("%s", tableString.c_str())

        tableString = "Events count per session;"; // строка со значением общего количества событий с заявками
        tableString += std::to_string(mainPair.second.eventsCountPerSession);
        SESSION_STATISTICS_LOG("%s", tableString.c_str())

        tableString = "Order size;"; // строка содержащая размеры трейдов, которые встречались в течении сессии
        tableString1 = "Orders count;"; // строка со значениями количеств ордеров, определённого размера
        for (const std::pair<uint64_t, uint64_t>& valuePair : mainPair.second.ordersSizeCounter)
        {
            tableString += std::to_string(valuePair.first) + ";";
            tableString1 += std::to_string(valuePair.second) + ";";
        }
        SESSION_STATISTICS_LOG("%s", tableString.c_str())
        SESSION_STATISTICS_LOG("%s", tableString1.c_str())
            
        tableString = "Min size of 1% top big orders;";  // строка со значением минимального размера ордера из 1% диапазона самых крупных ордеров
        size_t counter = 0;
        size_t allAddOrdersCount = mainPair.second.addOrderEventCount + mainPair.second.moveOrderEventCount;
        for (auto& it = mainPair.second.ordersSizeCounter.rbegin(), end = mainPair.second.ordersSizeCounter.rend(); it != end; ++it)
        {
            counter += it->second;
            if (counter > allAddOrdersCount * 0.01)
            {
                tableString += to_string(it->first);
                break;
            }
        }
        SESSION_STATISTICS_LOG("%s", tableString.c_str())

        tableString = "Order size in BBO;";  // строка со значениями размера ордеров встречающихся на BBO
        for (const uint64_t bboOrderSize : mainPair.second.ordersSizeOnBBO)
        {
            tableString += std::to_string(bboOrderSize) + ";";
        }
        SESSION_STATISTICS_LOG("%s", tableString.c_str())

        tableString = "Total trades count;"; // строка со значением общего количества сделок по инструменту
        const size_t allTradesCount = sessionData->GetAggrTradesCount(isin_id);
        tableString += std::to_string(allTradesCount);
        SESSION_STATISTICS_LOG("%s", tableString.c_str())

        tableString = "Trades size;";   // строка со значениями размеров сделок, которые встречались
        tableString1 = "Trades count;"; // строка со значениями количеств сделок, определённого размера
        for (const std::pair<int, int>& valuePair : frequencyTradeStatistic[isin_id])
        {
            tableString += std::to_string(valuePair.first) + ";";
            tableString1 += std::to_string(valuePair.second) + ";";
        }
        SESSION_STATISTICS_LOG("%s", tableString.c_str())
        SESSION_STATISTICS_LOG("%s", tableString1.c_str())            

        tableString = "Min size of 5% top trades;";  // строка со значением минимального размера сделки из 5% диапазона самых крупных сделок
        counter = 0;
        for (auto& it = frequencyTradeStatistic[isin_id].rbegin(), end = frequencyTradeStatistic[isin_id].rend(); it != end; ++it)
        {
            counter += it->second;
            if (counter > allTradesCount * 0.05)
            {
                tableString += to_string(it->first);
                break;
            }
        }
        SESSION_STATISTICS_LOG("%s", tableString.c_str())

        tableString = "Breaked levels;";    // строка со значениями количества пробитых ценовых уровней трейдом
        tableString1 = "Trades count;";     // строка со значениями количеств сделок, которые пробили ценовые уровни
        for (const std::pair<int, int>& valuePair : depthOfPenetrationStatistic[isin_id])
        {
            tableString += std::to_string(valuePair.first) + ';';
            tableString1 += std::to_string(valuePair.second) + ';';
        }
        SESSION_STATISTICS_LOG("%s", tableString.c_str())
        SESSION_STATISTICS_LOG("%s", tableString1.c_str())

        tableString = "Hour;";                  // строка со значениями часа сбора статистики
        tableString1 = "CummTrades;";           // строка со значениями суммарных количеств трейдов за данный час
        tableString2 = "CummSpreadChanges;";    // строка со значениями количества изменений спрэда
        tableString3 = "CummEvents;";           // строка со значениями суммарного количества событий за данный час
        for (const std::pair<Hour, HourlyStatistics>& pair : mainPair.second.hourlyStatistics)
        {
            tableString += std::to_string(pair.first) + ";";
            tableString1 += std::to_string(pair.second.tradesCount) + ";";
            tableString2 += std::to_string(pair.second.spreadChangesCount) + ";";
            tableString3 += std::to_string(pair.second.eventCount) + ";";

        }
        SESSION_STATISTICS_LOG("%s", tableString.c_str())
        SESSION_STATISTICS_LOG("%s", tableString1.c_str())
        SESSION_STATISTICS_LOG("%s", tableString2.c_str())
        SESSION_STATISTICS_LOG("%s", tableString3.c_str())

        //tableString = "Min size of 1% top big orders in BBO;";  // строка со значением минимального размера ордера из 1% диапазона самых крупных ордеров встречающихся на BBO
        //counter = 0;
        //for (auto& it = mainPair.second.ordersSizeOnBBO.rbegin(), end = mainPair.second.ordersSizeOnBBO.rend(); it != end; ++it)
        //{
        //    ++counter;
        //    if (counter > mainPair.second.ordersSizeOnBBO.size() * 0.01)
        //    {
        //        tableString += to_string(*it);
        //        break;
        //    }
        //}
        //INSTRUMENTS_LOG("%s", tableString.c_str())

        //tableString = "Cumulative size in BBO;";  // строка со значенями кумулятивных размеров встречающихся на ценовом BBO
        //for (const int bboCumulativeSize : mainPair.second.cumulativeSizeOnBBO)
        //{
        //    tableString += std::to_string(bboCumulativeSize) + ";";
        //}
        //INSTRUMENTS_LOG("%s", tableString.c_str())

        //tableString = "Min size of 1% top cumulative sizes in BBO;";  // строка со значением минимального кумулятивного размера из 1% диапазона самых крупных кумулятивных размеров ордеров встречающихся на BBO
        //counter = 0;
        //for (auto& it = mainPair.second.cumulativeSizeOnBBO.rbegin(), end = mainPair.second.cumulativeSizeOnBBO.rend(); it != end; ++it)
        //{
        //    ++counter;
        //    if (counter > mainPair.second.cumulativeSizeOnBBO.size() * 0.01)
        //    {
        //        tableString += to_string(*it);
        //        break;
        //    }
        //}
        //INSTRUMENTS_LOG("%s", tableString.c_str())

        SESSION_STATISTICS_LOG("%s", "\n");
    }
}

void Plaza2Callbacks::GetFrequencyTradeStatistic(std::map<InstrumentID, std::map<TradeSize, Frequency>>& massiv)
{
    for (const std::pair<int, std::vector<AggregatedTrade>>& mainPair : sessionData->aggrTradesMap)
    {
        const InstrumentID isin_id = mainPair.first;
        if (sessionData->isFutures(isin_id)) // только для фьючей
        {
            for (const AggregatedTrade& trade : mainPair.second)
            {
                std::map<TradeSize, Frequency>& mapValue = massiv[mainPair.first];
                ++mapValue[(int)trade.matchSize];
            }
        }
    }
}

void Plaza2Callbacks::GetDepthOfPenetrationStatistic(std::map<InstrumentID, std::map<PriceLevelCount, Frequency>>& massiv)
{
    for (const std::pair<int, std::vector<AggregatedTrade>>& mainPair : sessionData->aggrTradesMap)
    {
        const InstrumentID isin_id = mainPair.first;
        if (sessionData->isFutures(isin_id)) // только для фьючей
        {
            for (const AggregatedTrade& trade : mainPair.second)
            {
                std::map<PriceLevelCount, Frequency>& mapValue = massiv[mainPair.first];
                ++mapValue[trade.executedPriceLevelsCount];
            }
        }
    }
}

void Plaza2Callbacks::MultiSessionStatisticsToFile()
{
#define MULTISESSION_STATISTICS_LOG(message, ...) PRINT_TO_LOG((*multisessionStatisticsLogger.get()), LOG_TRACE, message, __VA_ARGS__)

    std::string fileName = params.fileNamePrefix + "_MultiSessionStatistics" + ".csv";

    std::unique_ptr<FileLogger> multisessionStatisticsLogger;
    multisessionStatisticsLogger.reset(new FileLogger(params.pathToLogs, fileName));

    std::string tableHeader("Isin;Isin_ID;");
    std::set<fo2base::UnixNanosecondsTime> dates;
    for (const auto& pair : statistics.GetTradesMultiSessionStatisticsData())
    {
        for (const auto& _pair : pair.second)
        {
            dates.insert(_pair.first);
        }
    }

    for (const fo2base::UnixNanosecondsTime& date : dates)
    {
        tableHeader += fo2base::CP2Time::FromUnixNanosecondsToDateString(date, '-') + ';';
    }
    

    MULTISESSION_STATISTICS_LOG("%s", tableHeader.c_str());
    
    std::string tableString;
    for (const auto& pair : statistics.GetTradesMultiSessionStatisticsData())
    {
        const InstrumentID isin_id = pair.first;
        const std::string isin = sessionData->GetIsinFromId(isin_id);
        
        tableString = isin + ';';
        tableString += to_string(isin_id) + ';';

        for (const fo2base::UnixNanosecondsTime& date : dates)
        {
            if (pair.second.find(date) == pair.second.end())
            {
                tableString += to_string(0) + ';';
            }
            else
            {
                const size_t tradesCountPerSession = pair.second.at(date);
                tableString += to_string(tradesCountPerSession) + ';';
            }
        }

        MULTISESSION_STATISTICS_LOG("%s", tableString.c_str());
        tableString.clear();
    }

    MULTISESSION_STATISTICS_LOG("%s", "\n\n");
    MULTISESSION_STATISTICS_LOG("%s", tableHeader.c_str());

    for (const auto& pair : statistics.GetSpreadMultiSessionStatisticsData())
    {
        const InstrumentID isin_id = pair.first;
        const std::string isin = sessionData->GetIsinFromId(isin_id);

        tableString = isin + ';';
        tableString += to_string(isin_id) + ';';

        for (const fo2base::UnixNanosecondsTime& date : dates)
        {
            if (pair.second.find(date) == pair.second.end())
            {
                tableString += to_string(0) + ';';
            }
            else
            {
                const size_t spreadchangesCountPerSession = pair.second.at(date);
                tableString += to_string(spreadchangesCountPerSession) + ';';
            }
        }

        MULTISESSION_STATISTICS_LOG("%s", tableString.c_str());
        tableString.clear();
    }
}

StatisticsWrapper::StatisticsWrapper() : pSessionData(NULL)
{   
}

void StatisticsWrapper::Init(TradeSessionData* ptr)
{
     pSessionData = ptr;
}

void StatisticsWrapper::CalculateSpreadChangesCount(const InstrumentID& ID)
{
    COrdersBookBase* book = pSessionData->ordersBooks.getOrdersBook(ID);
    COrdersSetBase* bidSet = book->GetBidSet(); bidSet->Begin();
    COrdersSetBase* askSet = book->GetAskSet(); askSet->Begin();

    Price currBestBidPrice, currBestAskPrice;

    SOrder* firstBidOrder = bidSet->GetOrder();
    SOrder* firstAskOrder = askSet->GetOrder();

    if (firstBidOrder != NULL)
    {
        currBestBidPrice = firstBidOrder->price;
    }
    else
    {
        return;
    }

    if (firstAskOrder != NULL)
    {
        currBestAskPrice = firstAskOrder->price;
    }
    else
    {
        return;
    }

    Price prevBidPrice = sessionStatistics[ID].lastBestBidPrice;
    Price prevAskPrice = sessionStatistics[ID].lastBestAskPrice;

    if (prevBidPrice.IsPrice() && prevAskPrice.IsPrice())
    {
        if (prevBidPrice != currBestBidPrice || prevAskPrice != currBestAskPrice)
        {
            ++sessionStatistics[ID].spreadChangesCount;
        }
    }

	if (!pSessionData->aggrTradesMap.at(ID).empty())
	{
		pSessionData->aggrTradesMap.at(ID).back().spreadChangesCount = sessionStatistics[ID].spreadChangesCount;
	}

    sessionStatistics[ID].lastBestBidPrice = currBestBidPrice;
    sessionStatistics[ID].lastBestAskPrice = currBestAskPrice;
}

void StatisticsWrapper::PrepareHourlyStatistics()
{
    TradeSessionData* sessionData = gGetTradeSessionData();
    for (auto& pair : sessionStatistics)
    {
        const int isin_id = pair.first;
        if (!sessionData->isFutures(isin_id))
        {
            continue;
        }

        SessionStatistics& params = pair.second;
        for (auto& it = params.hourlyStatistics.rbegin(), end = params.hourlyStatistics.rend()-1; it != end; ++it)
        {
            HourlyStatistics& h1 = it->second;
            HourlyStatistics& h2 = (it+1)->second;

            h1.tradesCount = h1.tradesCount - h2.tradesCount;
            h1.eventCount = h1.eventCount - h2.eventCount;
            h1.spreadChangesCount = h1.spreadChangesCount - h2.spreadChangesCount;
        }
    }
}

void StatisticsWrapper::AddOrderStatistic(const InstrumentID& ID, const StatParameter orderSize)
{
    std::map<uint64_t, uint64_t>& mapValue = sessionStatistics[ID].ordersSizeCounter;
    ++mapValue[orderSize];
}

void StatisticsWrapper::AddBBOOrderSize(const InstrumentID& ID)
{
    COrdersSetBase* bidSet = pSessionData->ordersBooks.getOrdersBook(ID)->GetBidSet();
    COrdersSetBase* askSet = pSessionData->ordersBooks.getOrdersBook(ID)->GetAskSet();
    bidSet->Begin(); askSet->Begin();

    SOrder* order = bidSet->GetOrder();
    Price firstPrice;
    uint64_t cumulativeSize = 0;
    if (order != NULL)
    {
        firstPrice = order->price;

        while (order != NULL && firstPrice == order->price)
        {
            cumulativeSize += order->mRemainedQuantity;
            sessionStatistics[ID].ordersSizeOnBBO.insert(order->mInitialQuantity); 
            bidSet->Next();
            order = bidSet->GetOrder();
        }
        //statData[ID].cumulativeSizeOnBBO.insert(cumulativeSize);
    }

    cumulativeSize = 0;
    order = askSet->GetOrder();
    if (order != NULL)
    {
        firstPrice = order->price;

        while (order != NULL && firstPrice == order->price)
        {
            cumulativeSize += order->mRemainedQuantity;
            sessionStatistics[ID].ordersSizeOnBBO.insert(order->mInitialQuantity); 
            askSet->Next();
            order = askSet->GetOrder();
        }
        //sessionStatistics[ID].cumulativeSizeOnBBO.insert(cumulativeSize);
    }
}

void StatisticsWrapper::CalculateHourlyStatistics(const InstrumentID& currInstrumentID, const Hour hour)
{
    TradeSessionData& sessionData = *gGetTradeSessionData();
    if (!sessionData.isFutures(currInstrumentID))
    {
        return;
    }

    SessionStatistics& params = sessionStatistics[currInstrumentID];

    if (params.hourlyStatistics.empty() || params.hourlyStatistics.back().first != hour)
    {
        params.hourlyStatistics.push_back(std::make_pair(hour, HourlyStatistics()));
    }

    params.hourlyStatistics.back().second.tradesCount = sessionData.GetAggrTradesCount(currInstrumentID);
    params.hourlyStatistics.back().second.eventCount = params.eventsCountPerSession;
    params.hourlyStatistics.back().second.spreadChangesCount = params.spreadChangesCount;
}

void StatisticsWrapper::CalculateMultiSessionStatistics(const fo2base::UnixNanosecondsTime localTime)
{
    for (const std::pair<InstrumentID, SessionStatistics>& pair : sessionStatistics)
    {
        const InstrumentID isin_id = pair.first;
        const SessionStatistics& sessionStatistics = pair.second;

        const StatParameter tradesCountPerSession = sessionStatistics.totalTradesCount;
        //if (tradesCountPerSession > 1000)
        {
            tradesMultiSessionStatistics[isin_id].insert(std::make_pair(localTime, tradesCountPerSession));

            const StatParameter spreadChangesPerSession = sessionStatistics.spreadChangesCount;
            spreadMultiSessionStatistics[isin_id].insert(std::make_pair(localTime, spreadChangesPerSession));
        }
    }
}