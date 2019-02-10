#pragma once
#include "MatchingData.h"
#include "IPlaza2_Connector.h"
#include "Plaza2_Tables.h"
#include "Plaza2Objects.h"

class Plaza2Callbacks
{
public:
	Plaza2Callbacks() {}

    static CG_RESULT SystemCallback(const int type, msg_t* data, IListener* listener);
	static CG_RESULT OrderBook(msg_streamdata_t* data, IListener* listener);
	static CG_RESULT Trade(msg_streamdata_t* data, IListener* listener);
	static CG_RESULT BBO(msg_streamdata_t* data, IListener* listener);

	static void ClearOrderBookBuffers(int isin_id);
	static void ClearDealsBuffers(int isin_id);

    static CMatchingData mMatchingData;

	static void ClearAll()
	{
		mMatchingData.ClearAllBooks();
		mTradeList.clear();
		mOppositeOrders.clear();
	}

    typedef std::map<int/*InstrumentID*/, std::vector<FORTS_DEALS_REPL::deal>> DealsMap;
    typedef std::map<int/*InstrumentID*/, P2Order> OppositeOrderMap;

	//typedef std::map<int/*InstrumentID*/, bool> StateMap;
    
    static DealsMap mTradeList;                 // прихраниваем здесь трейды потока FORTS_DEALS_REPL, которые обгоняют поток FORTS_ORDLOG_REPL
    static OppositeOrderMap mOppositeOrders;    // мапа прихраненых ордеров, которые привели к трейдам и возможно остаток от них пойдёт в стакан
	//static StateMap isOnlineFlags;    			// флаг отражающий текущее состояние потока стакана: true - online, false - snapshot

};