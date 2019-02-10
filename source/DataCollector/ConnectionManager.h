#pragma once

#include "p2execlog.h"
#include "IPlaza2_Connector.h"
#include "ASTS_Connector.h"


enum ExternalStoragesType
{
	Plaza2_Trades,
	Plaza2_AllData,
	Plaza2_Info,
	ASTS_Stock,
    ASTS_Currency,
    Rithmic,
	ExternalStoragesCount
};

void WriteDataToeShmem(CReplStruct& rec, ExternalStoragesType extType);

class ConnectionManager
{
public:
	ConnectionManager();
	~ConnectionManager();

    void InitFlags();
	void Init();
	void DeInit();
	void ConnectToDataProviders();
	void Disconnect();

	CP2eConnection* GetExtStorage() { return externalStorages; }

private:
	void ConnectToPlaza2(P2ReplList& streamParams);
	void DisconnectFromPlaza2();
    void ConnectToASTS(const MarketType marketType);
    void DisconnectFromASTS(const MarketType marketType);
	
private:
    int startNewRouterProcess;

	int ignorePlaza2;
    int ignoreASTSStock;
    int ignoreASTSCurrency;
    int ignoreRithmic;

    Plaza2InitParameters params;

	std::unique_ptr<IPlaza2ConnectorType1> plaza2Connector;
    std::unique_ptr<ASTS_ConnectorType1> astsStockConnector;
    std::unique_ptr<ASTS_ConnectorType1> astsCurrencyConnector;

	CP2eConnection externalStorages[ExternalStoragesCount];

}; // ConnectionManager

typedef CSingletonHolder<ConnectionManager>  ConnectionManagerHolder;

