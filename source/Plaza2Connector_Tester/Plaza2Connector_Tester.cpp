// Plaza2Connector_Tester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <conio.h>
#include <process.h>
#include "Logger.h"
#include "IPlaza2_Connector.h"
#include "Plaza2_Tables.h"
//#include "P2OrderBook.h"


#define APPLICATION_NAME	"Plaza2Connector_Tester"

const char* routerPath			=	".\\Plaza2\\P2MQRouter64.exe";
const char* iniPath				=	".\\ini\\plaza2_router.ini";


CG_RESULT SystemMessagesForData(const int type, msg_t* data, IListener* listener)
{
    switch(type)
    {
    case CG_MSG_OPEN:
        {
            log_debug("CG_MSG_OPEN message, stream: %s", listener->GetStreamName().c_str());
        }
        break;

    case CG_MSG_CLOSE:
        {
            log_debug("CG_MSG_CLOSE message, stream: %s, reason: %d", listener->GetStreamName().c_str(), (int)data->data);
        }
        break;

    case CG_MSG_TN_BEGIN:
		{
			log_debug("CG_MSG_TN_BEGIN message, stream: %s", listener->GetStreamName().c_str());
		}
		break;

	case CG_MSG_TN_COMMIT:
        {
            log_debug("CG_MSG_TN_COMMIT message, stream: %s", listener->GetStreamName().c_str());
        }
        break;

    case CG_MSG_P2REPL_ONLINE:
        {
            log_debug("CG_MSG_P2REPL_ONLINE message, stream: %s", listener->GetStreamName().c_str());
        }
        break;

    case CG_MSG_P2REPL_REPLSTATE:
        {
            log_debug("CG_MSG_P2REPL_REPLSTATE message, stream: %s", listener->GetStreamName().c_str());
        }
        break;
    case CG_MSG_P2REPL_LIFENUM:
        {
            log_debug("CG_MSG_P2REPL_LIFENUM message, lifenum: %d, stream: %s", (int)data->data, listener->GetStreamName().c_str());
        }
        break;

    case CG_MSG_P2REPL_CLEARDELETED:
        {
            log_debug("CG_MSG_P2REPL_CLEARDELETED message, table_idx %d, table_rev %lld, stream: %s", 
                ((cg_data_cleardeleted_t*)data)->table_idx, ((cg_data_cleardeleted_t*)data)->table_rev, listener->GetStreamName().c_str());
        }
        break;
    }

    return CG_ERR_OK;
}

CG_RESULT Data(msg_streamdata_t* data, IListener* listener)
{
    log_debug("CG_MSG_STREAM_DATA message, stream: %s", listener->GetStreamName().c_str());
    printf("Stream_name='%s'\n", listener->GetStreamName().c_str());

	using namespace FORTS_DEALS_REPL;
	
	//const int deal_index = 0;
	//const int sys_events_index = 1;

    //printf("name:%s, index:%d\n", data->msg_name, data->msg_index);

	switch (data->msg_index)
	{
	case deal_index:
		{
			deal& d = *(deal*)data->data;
			/*for (auto it = instruments.begin(); it != instruments.end(); ++it)
			{
				if (d.isin_id == it->second.isin_id)
				{
					char timestamp[100];
					sprintf_s(timestamp, 100, "%02d:%02d:%02d.%03d", d.moment.hour, d.moment.minute, d.moment.second, d.moment.msec);

					P2Price price = GetValuesHelper::GetP2Price(d.price);

					fstream m_fs;
					const char* fileName = "DOM_LOG.txt";
					m_fs.open(fileName, ios::in | ios::out | ios::app);
					m_fs << timestamp << "\t\t" << price.price_double << '\t' << d.amount << '\t' << d.pos << endl;
					m_fs.close();

					cout << timestamp << "\t\t" << price.price_double << '\t' << d.amount << '\t' << d.pos << endl;
				}
			}*/
		}
		break;

	case sys_events_index:
		{
			sys_events& d = *(sys_events*)data->data;
		}
		break;
	}

	/*switch (data->msg_index)
	{
	case deal_index:
		{
			deal& d = *(deal*)data->data;
		}
		break;
	case multileg_deal_index:
		{
			multileg_deal& md = *(multileg_deal*)data->data;
		}
		break;
	case heartbeat_index:
		{
			heartbeat& md = *(heartbeat*)data->data;
		}
		break;
	case sys_events_index:
		{
			sys_events& md = *(sys_events*)data->data;
		}
		break;
	}*/

	return CG_ERR_OK;
}

//CG_RESULT FortsFutAggr5Func(int type, msg_streamdata_t* data, Listener* flow)
//{
//	using namespace FORTS_FUTAGGR5_REPL;
//
//	orders_aggr& d = *(orders_aggr*)data->data;
//
//	//for (auto it = instruments.begin(); it != instruments.end(); ++it)
//	//{
//	//	if (d.isin_id == it->second.isin_id)
//	//	{
//	//		double price = GetValuesHelper::GetDoubleFromStringBCD(d.price);
//
//	//		Order ord(price, d.volume);
//
//	//		P2OrderBook* pBook = P2OrderBook::GetP2OrderBookInstance();
//	//		pBook->Time(d.moment);
//	//		pBook->SetPriceLevel(&ord, (Dir)d.dir);
//
//	//		char str[100];
//	//		sprintf_s(str, 100, "\t%02d:%02d:%02d.%03d\t%.lf (%d)\t%s\n", 
//	//					d.moment.hour, d.moment.minute, d.moment.second, d.moment.msec,
//	//					//DOM.GetBuyPrice(0), DOM.GetBuyVolume(0), DOM.GetSellPrice(0), DOM.GetSellVolume(0));
//	//			price, d.volume, (d.dir-1 ? "Ask" : "Bid"));
//
//	//		fstream m_fs;
//	//		const char* fileName = "DOM_LOG.txt";
//	//		m_fs.open(fileName, ios::in | ios::out | ios::app);
//	//		m_fs << str;
//	//		m_fs.close();
//
//	//		pBook->PrintToFile(fileName);
//
//	//		//cout << str;
//	//	}
//	//}	
//
//	return CG_ERR_OK;
//}
//
//CG_RESULT FortsFutInfoFunc(int type, msg_streamdata_t* data, Listener* flow)
//{
//
//	return CG_ERR_OK;
//}
//
//CG_RESULT FortsOrdLogFunc(int type, msg_streamdata_t* data, Listener* flow)
//{
//	using namespace FORTS_ORDLOG_REPL;
//
//	switch(type)
//	{
//	case CG_MSG_STREAM_DATA:
//		{
//			orders_log& d = *(orders_log*)data->data;
//	
//
//			for (auto it = instruments.begin(); it != instruments.end(); ++it)
//			{
//				signed int instrumentIsinId = it->second.isin_id;
//
//				if (d.isin_id == instrumentIsinId)
//				{
//					static P2Trade trade;
//					P2Price price = GetValuesHelper::GetP2Price(d.price);
//					P2Price deal_price = GetValuesHelper::GetP2Price(d.deal_price);
//					Order order(price, d.amount_rest, d.status);
//					static P2OrderBook pBook;
//
//					pBook.ClearPreviousInfo();							//чистим данные о трейдах и удаляем нулевые значения в стакане если есть
//
//					pBook.Time(d.moment);
//
//					pBook.Time(d.moment);
//
//					if (order.volume || price.price_double == deal_price.price_double)			//добавляем обычные ордера и TRADE, которые обнуляют цену
//						pBook.SetPriceLevel(&order, (Dir)d.dir);
//					else if (price.price_double != deal_price.price_double)						
//						pBook.RemovePriceLevel(&order, (Dir)d.dir);
//
//					if (d.status == 0x401 || d.status == 0x402)
//					{
//						trade.price = price;
//						trade.size = d.amount;
//					}			
//
//					if (d.action == TRADE)								//нарезанные данные трейда				
//					{
//						if (trade.CheckId(d.id_deal))
//						{
//							trade.agressorSide = (d.dir == Dir::Bid) ? SELL : BUY;
//							trade.trades.push_back(SliceTrades(deal_price, d.amount));
//							pBook.Trade(trade);
//						}
//					}
//
//					/*char str[256];
//					sprintf_s(str, 256, "\t%02d:%02d:%02d.%03d\t%.lf (%d)\t%s\t%s\tdeal_price:%.lf\torder_size:%d\tid_deal:%d\tstatus:0x%X\n", 
//								d.moment.hour, d.moment.minute, d.moment.second, d.moment.msec,
//								price, d.amount_rest, (d.dir-1 ? "Ask" : "Bid"), (d.action ? ((d.action == 1) ? "ADD" : "TRADE") : "REMOVE"),
//								deal_price, d.amount, d.id_deal, d.status);
//
//					fstream m_fs;
//					const char* fileName = "DOM_LOG.txt";
//					m_fs.open(fileName, ios::in | ios::out | ios::app);
//					m_fs << str;
//					m_fs.close();
//
//					pBook.PrintToFile(std::string(fileName));*/
//				}
//			}
//		}
//		break;
//	}
//
//	return CG_ERR_OK;
//}

// подключаемся сперва к потоку FORTS_DEALS_REPL получаем трейды онлайн в течении 2 секунд и отключаемся
void Test1(const _TCHAR* argv)
{
    // ************Test1 Begin************

    Plaza2InitParameters params;
    P2ReplList streamParams;
    std::unique_ptr<IPlaza2ConnectorType1> pC;

	try 
	{
        GetPlaza2InitParametersFromINI(argv, params);
        GetP2ReplListParametersFromINI(argv, streamParams);

        for (auto it = streamParams.begin(), end = streamParams.end(); it != end; ++it)
        {
            it->second.func1 = SystemMessagesForData;
            it->second.func2 = Data;
        }

        printf("Starting Plaza2 router process...\n");
        StartPlaza2RouterProcess(routerPath, iniPath, true);
        Sleep(1000);
        pC.reset(GetPlaza2ConnectorType1());
        printf("Getting data...\n");
		pC->OpenConnection(params, streamParams);

        Sleep(20000);	//получаем данные

        pC->Disconnect();

        printf("Stop Plaza2 router process...\n");
        StopPlaza2RouterProcess("P2MQRouter64.exe");
	}
	catch (std::bad_alloc& e)
	{
		std::cout << e.what() << std::endl;
	}
	catch(Plaza2ConnectorException& e)
	{
        std::cout << "Plaza2ConnectorException occurred!" << std::endl;
        std::cout << "Error code: " << e.ErrorCodeToStr() << std::endl;
        std::cout << "Error message : " << e.what() << std::endl;
	}
	catch(std::exception& e)
	{
		pC->Disconnect();
		std::cout << e.what() << std::endl;
	}

    // ************Test1 End**************
}

// подключаемся сперва к потоку FORTS_DEALS_REPL получаем трейды онлайн в течении 2 секунд и отключаемся
// затем подключаемся к потоку FORTS_ORDLOG_REPL получаем данные полного ордерлога в течении 2 секунд и отключаемся
void Test2(const _TCHAR* argv)
{
    // ************Test2 Begin************

    Plaza2InitParameters params;
    P2RelpStreamParameters streamParams;
    std::unique_ptr<IPlaza2ConnectorType2> pC;

    try
    {
        GetPlaza2InitParametersFromINI(argv, params);

        printf("Starting Plaza2 router process\n");
        StartPlaza2RouterProcess(routerPath, iniPath, true);

        pC.reset(GetPlaza2ConnectorType2());
        pC->OpenConnection(params);

        streamParams.replStreamName = "FORTS_DEALS_REPL";
        streamParams.replStreamMode = "snapshot";
        streamParams.func1 = SystemMessagesForData;
        streamParams.func2 = Data;
        streamParams.isin = GetIsinIdFromShortIsin(pC.get(), "SiZ6");

        pC->OpenP2ReplListener(streamParams);

        printf("Getting data...\n");
        Sleep(2000);	//получаем данные

        pC->CloseP2ReplListener(streamParams.isin);

        streamParams.replStreamName = "FORTS_ORDLOG_REPL";
        streamParams.replStreamMode = "online";
        streamParams.func1 = SystemMessagesForData;
        streamParams.func2 = Data;
        streamParams.isin = GetIsinIdFromShortIsin(pC.get(), "RIZ6");
        
        const char* ticker = GetShortIsinFromIsinId(pC.get(), streamParams.isin);
       
        std::unique_ptr<FORTS_FUTINFO_REPL::fut_sess_contents> content;
        content.reset((FORTS_FUTINFO_REPL::fut_sess_contents*)GetInstrumentContents(pC.get(), ticker));
        
        std::map<std::string, void*> contents;
        GetAllInstrumentsContents(pC.get(), contents);

        pC->OpenP2ReplListener(streamParams);

        printf("Getting data...\n");
        Sleep(2000);	//получаем данные

        pC->CloseP2ReplListener(streamParams.isin);

        pC->Disconnect();

        printf("Stop Plaza2 router process\n");
        StopPlaza2RouterProcess("P2MQRouter64.exe");
    }
    catch (std::bad_alloc& e)
    {
        std::cout << e.what() << std::endl;
    }
    catch(Plaza2ConnectorException& e)
    {
        std::cout << "Plaza2ConnectorException occurred!" << std::endl;
        std::cout << "Error code: " << e.ErrorCodeToStr() << std::endl;
        std::cout << "Error message : " << e.what() << std::endl;
    }
    catch(std::exception& e)
    {
        pC->Disconnect();
        std::cout << e.what() << std::endl;
    }

    // ************Test2 End**************
}

int _tmain(int argc, _TCHAR* argv[])
{
    if (argc < 2)
    {
        printf("Second parameter is empty, please specify path to INI-file");
        return 1;
    }

    Test1(argv[1]);
    Test2(argv[1]);

	return 0;
}


