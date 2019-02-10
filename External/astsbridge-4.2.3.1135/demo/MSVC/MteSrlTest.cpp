// MteSrlTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "mtesrl.h"
#include "mteerr.h"
#include "DataTypes.h"

// "HOST=127.0.0.1:15005\nSERVER=UAT_GATEWAY\nUSERID=MU9009000001\nPASSWORD=1136\nINTERFACE=IFCBroker_26\nLOGFOLDER=.\\log\n"
// IFCBroker - 'Интерфейс для брокерских фирм'
// IFCBroker15A - 'Интерфейс для брокерских фирм, v.15'
// IFCBroker12A - 'Интерфейс для брокерских фирм'
// IFCBroker15B - 'Интерфейс для брокерских фирм, v.15B'
// IFCBrokerRPS - 'Интерфейс для брокерских фирм'
// IFCBroker_03 - 'Интерфейс для брокерских фирм'
// IFCBroker_10 - 'Интерфейс для брокерских фирм'
// IFCBroker_11 - 'Интерфейс для брокерских фирм'
// IFCBroker_12 - 'Интерфейс для брокерских фирм'
// IFCBroker_13 - 'Интерфейс для брокерских фирм'
// IFCBroker_14 - 'Интерфейс для брокерских фирм'
// IFCBroker_17 - 'Интерфейс для брокерских фирм, v.17'
// IFCBroker_26 -                                           - этот использовать
// IFC_Analytic - 'Интерфейс для MICEX Trade SE, v.235'

// "HOST=127.0.0.1:15005\nSERVER=UATCUR_GATEWAY\nUSERID=MD9021000001\nPASSWORD=7235\nINTERFACE=IFC_Broker26\nLOGFOLDER=.\\log\n"
// IFC_BROKER11 - 'Интерфейс для участника торгов'
// IFC_BROKER12 - 'Интерфейс для ВПТС банка v.12'
// IFC_BROKER16 - 'Интерфейс для ВПТС банка v.15'
// IFCBroker15A - 'Интерфейс для брокерских фирм, v.15'
// IFCBroker15B - 'Интерфейс для брокерских фирм, v.15B'
// IFC_Broker18 - 'Интерфейс для брокерских фирм, v.18'
// IFC_Broker21 - 'Интерфейс для брокерских фирм, v.21'
// IFC_Broker22 - 'Интерфейс для брокерских фирм, v.22'
// IFC_Broker24 - 'Интерфейс для брокерских фирм, v.24'
// IFC_Broker26 - 'Интерфейс для брокерских фирм, v.26'     - этот использовать
// IFC_Analytic - 'Интерфейс для MICEX Trade Currency'

// Эти интерфейсы были в документации, но не было в списке возврата
//  'Интерфейс для информ-агентств v.24'
//  'Currency18' - 'Интерфейс для Валютного рынка, v.18'
//  'Информационный интерфейс, v.19'
//  'Equities23' - 'Интерфейс для рынка НГЦБ, v.23'
//  'Info' - 'Информационный интерфейс'

bool g_isVersion2 = true;

template <typename Type>
void printfObjectInfo(char*& ptr)
{
    Type t;
    t.SetValues(ptr);
    printf("%.*s\t\n", t.name.length, t.name.data);
    //printf("%.*s\t%.*s\t%.*s\n", t.name.length, t.name.data, t.header.length, t.header.data, t.description.length, t.description.data);
}

void PrintInterfaceInfo(TInterface& data)
{
    char* oldLocale = setlocale(LC_ALL,"Russian");
    if (!oldLocale)
    {
        printf("ERROR: Locale Russian is not set\n");
        return;
    }

    printf("\n******************************************************************\n\n");
    printf("Interface name: %.*s\n", data.interfaceName.length, data.interfaceName.data);
    printf("HeaderName name: %.*s\n", data.headerName.length, data.headerName.data);
    printf("Description name: %.*s\n", data.description.length, data.description.data);

    //выводим перечень типов, таблиц и транзакций
    printf("\nТипы enum'ов:\n");
    char* ptr = (char*)data.enumTypes.type;
    for (int i = 0; i < data.enumTypes.numberOfTypes; ++i)
    {
        printfObjectInfo<TEnumType>(ptr);
    }

    printf("\nТипы таблиц:\n");
    ptr = (char*)data.tables.tables;
    for (int i = 0; i < data.tables.tablesCount; ++i)
    {
        printfObjectInfo<TTable>(ptr);
    }

    printf("\nТипы транзакций:\n");
    ptr = (char*)data.transactions.transactions;
    for (int i = 0; i < data.transactions.numberOfTransations; ++i)
    {
        printfObjectInfo<TTransaction>(ptr);
    }
    printf("\n******************************************************************\n\n");

    setlocale(LC_ALL, oldLocale);
}

void PrintFieldData(TFieldType type, char* data, const int size)
{
    switch(type)
    {
    case ftChar:        printf("%.*s\t", size, data);                         break;
    case ftInteger:     printf("%.*s\t", size, data);                         break;
    case ftFixed:       printf("%.*s\t", size, data);                         break;
    case ftFloat:       printf("%.*s\t", size, data);                         break;
    case ftDate:        printf("%.*s/%.*s/%.*s\t", 4, data, 2, data+4, 2, data+6);  break;
    case ftTime:        printf("%.*s:%.*s:%.*s\t", 2, data, 2, data+2, 2, data+4);  break;
    case ftFloatPoint:  printf("%.*s\t", size, data);                         break;
    }
}

void PrintTableInfo(std::string& orderbookTable, TMTETable& table, TInterface& iFace)
{
    char* oldLocale = setlocale(LC_ALL,"Russian");
    if (!oldLocale)
    {
        printf("ERROR: Locale Russian is not set\n");
        return;
    }

    printf("Number of rows: %d\n", table.rowsCount);

    char* ptr = (char*)table.rows;
    TTable tableInterface = iFace.tables.GetTableByName(orderbookTable);
    for (int r = 0; r < table.rowsCount; ++r)
    {
        TMTERow row;
        row.SetValues(ptr);
        char* data = row.fieldData;
        int fieldCounter = 0;
        char* _ptr = (char*)tableInterface.outputFiedls.fields;
        printf("\n******************************************************************\n\n");
        for (int fieldNum = 0; fieldNum < tableInterface.outputFiedls.numberOfFields; ++fieldNum)
        {
            TFieldOut field;
            field.SetValues(_ptr);

            if (row.fieldCount)
            {
                if (fieldCounter == row.fieldCount)
                    break;

                char num = *(row.fieldNumbers);
                if (num == fieldNum)
                {
                    row.fieldNumbers++;
                    fieldCounter++;
                }
                else
                    continue;
            }

            printf("%.*s:\t", field.name.length, field.name.data);

            PrintFieldData(field.type, data, field.size);
            data += field.size;
        }
    }

    printf("\n******************************************************************\n\n");

    setlocale(LC_ALL, oldLocale);
}

void PrintOrderBookForInstrument(TTable& tableInterface, TMTETable& table)
{
    char* ptr = (char*)table.rows;
    for (int r = 0; r < table.rowsCount; ++r)
    {
        TMTERow row;
        row.SetValues(ptr);
        char* data = row.fieldData;
        int fieldCounter = 0;
        char* _ptr = (char*)tableInterface.outputFiedls.fields;

        for (int fieldNum = 0; fieldNum < tableInterface.outputFiedls.numberOfFields; ++fieldNum)
        {
            TFieldOut field;
            field.SetValues(_ptr);

            if (row.fieldCount)
            {
                if (fieldCounter == row.fieldCount)
                    break;

                char num = *(row.fieldNumbers);
                if (num == fieldNum)
                {
                    row.fieldNumbers++;
                    fieldCounter++;
                }
                else
                {
                    continue;
                }
            }

            std::string fieldName(field.name.data, field.name.length);
            if (fieldName == "BUYSELL" || fieldName == "PRICE" || fieldName == "QUANTITY")
            {
                PrintFieldData(field.type, data, field.size);
            }
            data += field.size;

            if ((!row.fieldCount && fieldNum == tableInterface.outputFiedls.numberOfFields-1) ||
                (row.fieldCount && fieldCounter == row.fieldCount-1))
            {
                printf("\n");
            }
        }
    }
}

void PrintTime(TTable& tableInterface, TMTETable& table)
{
    char* ptr = (char*)table.rows;
    for (int r = 0; r < table.rowsCount; ++r)
    {
        TMTERow row;
        row.SetValues(ptr);
        char* data = row.fieldData;
        int fieldCounter = 0;
        char* _ptr = (char*)tableInterface.outputFiedls.fields;

        for (int fieldNum = 0; fieldNum < tableInterface.outputFiedls.numberOfFields; ++fieldNum)
        {
            TFieldOut field;
            field.SetValues(_ptr);

            if (row.fieldCount)
            {
                if (fieldCounter == row.fieldCount)
                    break;

                char num = *(row.fieldNumbers);
                if (num == fieldNum)
                {
                    row.fieldNumbers++;
                    fieldCounter++;
                }
                else
                {
                    continue;
                }
            }

            std::string fieldName(field.name.data, field.name.length);
            if (fieldName == "TIME" || fieldName == "MICROSECONDS")
            {
                PrintFieldData(field.type, data, field.size);
            }
            data += field.size;

            if ((!row.fieldCount && fieldNum == tableInterface.outputFiedls.numberOfFields-1) ||
                (row.fieldCount && fieldCounter == row.fieldCount-1))
            {
                printf("\n");
            }
        }
    }
}

void PrintAllTradesForInstrument(TTable& tableInterface, TMTETable& table, std::string& instrumentName)
{
    char* ptr = (char*)table.rows;
    for (int r = 0; r < table.rowsCount; ++r)
    {
        TMTERow row;
        row.SetValues(ptr);
        char* data = row.fieldData;
        int fieldCounter = 0;
        char* _ptr = (char*)tableInterface.outputFiedls.fields;
        
        for (int fieldNum = 0; fieldNum < tableInterface.outputFiedls.numberOfFields; ++fieldNum)
        {
            TFieldOut field;
            field.SetValues(_ptr);

            if (row.fieldCount)
            {
                if (fieldCounter == row.fieldCount)
                    break;

                char num = *(row.fieldNumbers);
                if (num == fieldNum)
                {
                    row.fieldNumbers++;
                    fieldCounter++;
                }
                else
                {
                    continue;
                }
            }

            std::string fieldName(field.name.data, field.name.length);
            if (fieldName == "SECCODE")
            {
                std::string name(data, field.size);
                if (name != instrumentName)
                    break;
                else
                {
                    //печатаем время сделки с микросекундами
                    PrintFieldData(TFieldType::ftTime, row.fieldData+12, 6);
                    PrintFieldData(TFieldType::ftInteger, row.fieldData+153, 6);
                }
            }

            if (fieldName == "PRICE" || fieldName == "QUANTITY" || fieldName == "BUYSELL" /*|| fieldName == "MICROSECONDS"*/)
            {
                PrintFieldData(field.type, data, field.size);
            }
            data += field.size;

            if ((!row.fieldCount && fieldNum == tableInterface.outputFiedls.numberOfFields-1) ||
                (row.fieldCount && fieldCounter == row.fieldCount-1))
            {
                printf("\n");
            }
        }
  
        //if (!row.fieldCount)
        //{
        //    if (std::string(data[22], 12) == instrumentName)
        //    {
        //        printf("%.*s\t", 1, data[103]); // К/П
        //        printf("%.*s\t", 9, data[34]);  // цена
        //        printf("%.*s\n", 10, data[43]); // лоты
        //    }
        //}
        //else
        //{
        //    for (int i = 0; i < row.fieldCount; ++i)
        //    {
        //        char rowNumber = *row.fieldNumbers;
        //        ++row.fieldNumbers;

        //        if (rowNumber == 4 && std::string(data[22], 12) == instrumentName)
        //        {

        //        }
        //        switch(rowNumber)
        //        {
        //        case 3: printf("%.*s\t", 1, data); data += 1; break;    // К/П
        //        case 4: printf("%.*s\t", 9, data); data += 9; break;    // цена
        //        case 6: printf("%.*s\t", 10, data); data += 10; break;  // лоты
        //        }
        //    }
        //}
    }
}

int main(int argc, char** argv)
{
	char ErrMsg[256], /*FileName[MAX_PATH],*/ ConnectParams[1024]/*, *p*/;
    strcpy_s(ConnectParams, sizeof(ConnectParams), "HOST=127.0.0.1:15005\nSERVER=UATCUR_GATEWAY\nUSERID=MD9021000001\nPASSWORD=7235\nINTERFACE=IFC_Broker26\nLOGFOLDER=.\\log\n");
	int32_t connectionHandle, err;
    int32_t tableHandles[3] = {-1};
    MTEMSG* mteMsg;
    char *snapshotBuf;  // указатель на буфер для резервного сохранения
    int snapshotLen;    // длина буфера для резервного сохранения

	// change executable filename extension to .INI and read connect parameters from INI file
	/*p = FileName + GetModuleFileNameA(NULL, FileName, sizeof(FileName)); 
	while ((p > FileName) && (*p != '.')) p--;
	*p = '\0';
	strcat_s(FileName, ".ini");
	GetPrivateProfileSectionA("MTEConnect", ConnectParams, sizeof(ConnectParams), FileName);
	// replace \0 with \n
	p = ConnectParams;
	while (*p) {
		while (*p) p++;
		*p = '\n';
		p++;
	}*/
	printf(ConnectParams);
	printf("Connecting to localhost...\n");
	connectionHandle = MTEConnect(ConnectParams, ErrMsg);
	if (connectionHandle < MTE_OK) 
    {
		CharToOemA(ErrMsg, ErrMsg);
		printf("Error %d: '%s'\n", connectionHandle, ErrMsg);
		return connectionHandle;
	}
	printf("Connected\n");
    printf("Version: %s\n", MTEGetVersion());

    char* info;
    int infoLen;
    err =  MTEGetServInfo(connectionHandle, &info, &infoLen);
    if (err != MTE_OK) 
    {
        printf("MTEGetServInfo(): %d error\n", err);
    } 

    g_isVersion2 = true;

    if (!g_isVersion2)
    {
        err = MTEStructure(connectionHandle, &mteMsg);
        if (err != MTE_OK) {
            CharToOemA(MTEErrorMsg(err), ErrMsg);
            printf("MTEStructure(): %d: '%s'\n", err, ErrMsg);
        } 
        else printf("MTEStructure() returned %d bytes\n", mteMsg->DataLen);
    }
    else
    {
        err = MTEStructureEx(connectionHandle, 3, &mteMsg);
        if (err != MTE_OK) {
            CharToOemA(MTEErrorMsg(err), ErrMsg);
            printf("MTEStructure(): %d: '%s'\n", err, ErrMsg);
        } 
        else 
            printf("MTEStructure() returned %d bytes\n", mteMsg->DataLen);
    }

    TInterface iFace;
    iFace.setData(mteMsg->Data, mteMsg->DataLen);
    
    PrintInterfaceInfo(iFace); return 0;

    std::string timeTable("TESYSTIME");
    std::string orderbookTable("EXT_ORDERBOOK");
    std::string orderbookTableParams("TQBRMOEX        20");
    //std::string orderbookTableParams("CETSUSD000000TOD20");
    std::string allTradesTable("ALL_TRADES");
    MTEMessage msg[3];

 	tableHandles[0] = MTEOpenTable(connectionHandle, orderbookTable.c_str(), orderbookTableParams.c_str(), FALSE, &mteMsg);
    msg[0].set(mteMsg);
    tableHandles[1] = MTEOpenTable(connectionHandle, allTradesTable.c_str(), "", FALSE, &mteMsg);
    msg[1].set(mteMsg);
    tableHandles[2] = MTEOpenTable(connectionHandle, timeTable.c_str(), "", FALSE, &mteMsg);
    msg[2].set(mteMsg);

	if (tableHandles[0] >= MTE_OK && tableHandles[1] >= MTE_OK && tableHandles[2] >= MTE_OK)
    {
        printf("%s table opened, %d bytes received\n", orderbookTable.c_str(), msg[0].dataLen);
        printf("%s table opened, %d bytes received\n", allTradesTable.c_str(), msg[1].dataLen);
        printf("%s table opened, %d bytes received\n", timeTable.c_str(), msg[2].dataLen);

        TMTETable table;
        char* ptr;

        if (msg[0].dataLen)
        {
            ptr = msg[0].data;
            table.SetValues(ptr);
            //PrintTableInfo(orderbookTable, table, iFace);
        }
        
        if (msg[1].dataLen)
        {
            ptr = msg[1].data;
            table.SetValues(ptr);
            //PrintTableInfo(allTradesTable, table, iFace);
        }

        if (msg[2].dataLen)
        {
            ptr = msg[2].data;
            table.SetValues(ptr);
            //PrintTableInfo(allTradesTable, table, iFace);
        }
        
		while(!_kbhit())
        {
            /*char ch = getchar();
            if(ch == 0x0a)
            {

            }*/

			if (MTE_OK == (err = MTEAddTable(connectionHandle, tableHandles[0], 0x12345678)) &&
                MTE_OK == (err = MTEAddTable(connectionHandle, tableHandles[1], 0x12345677)) &&
                MTE_OK == (err = MTEAddTable(connectionHandle, tableHandles[2], 0x12345666)))
            {
                if (MTE_OK == (err = MTERefresh(connectionHandle, &mteMsg)))
                {
                    //printf("MTERefresh() returned %d bytes\n", MTETableMsg->DataLen);

                    TMTETables tables;
                    ptr = mteMsg->Data;
                    tables.SetValues(ptr);

                    //printf("Begin of tables block\n");

                    for (int i = 0; i < tables.tablesCount; ++i)
                    {
                        ptr = tables.SetPointerToTable(i);
                        table.SetValues(ptr);
                        
                        if (table.ref == 0x12345677)
                        {
                            TTable tableInterface = iFace.tables.GetTableByName(allTradesTable);
                            PrintAllTradesForInstrument(tableInterface, table, std::string(/*"USD000000TOD"*/"MOEX        "));
                            //PrintTableInfo(allTradesTable, table, iFace);
                        }

                        if (table.ref == 0x12345678)
                        {
                            TTable tableInterface = iFace.tables.GetTableByName(orderbookTable);
                            printf("\n");
                            PrintOrderBookForInstrument(tableInterface, table);
                            printf("\n");
                            //PrintTableInfo(orderbookTable, table, iFace);
                        }
                        
                        if (table.ref == 0x12345666)
                        {
                            TTable tableInterface = iFace.tables.GetTableByName(timeTable);
                            PrintTime(tableInterface, table);
                            //PrintTableInfo(allTradesTable, table, iFace);
                        }
                    }

                    //printf("End of tables block\n");

                    // получения буфера состояний внутренних структур шлюза 
                    MTEGetSnapshot(connectionHandle, &snapshotBuf, &snapshotLen);

                    if (mteMsg->DataLen < 30720) // если объем полученного пакета данных превышает 30 кбайт, немедленно отправить новый запрос на обновление
                    {
                        /* Если размер ответа на запрос не превышает 30 килобайт, то обязательна пауза не менее
                           100 миллисекунд перед отправкой следующего запроса. Пауза может отсчитываться от
                           времени отправки предыдущего запроса, а не от времени получения ответа.                         */
                        //Sleep(100);
                    }
                }
                else
                {
                    printf("MTERefresh() ERROR, return value=%d", err);
                }
            }
            else
            {
                printf("MTEAddTable() ERROR, return value=%d", err);
            }
		}

        for (int i = 0; i < _countof(tableHandles); ++i)
        {
            if (tableHandles[i] >= MTE_OK)
		        MTECloseTable(connectionHandle, tableHandles[i]);
        }
	}
    else if (tableHandles[0] == MTE_TSMR)
    {
        setlocale(LC_ALL,"Russian");
        printf("MTEOpenTable() ERROR, ErrorMessage=%.*s\n", msg[0].dataLen, msg[0].data);
    }
    else if (tableHandles[1] == MTE_TSMR)
    {
        setlocale(LC_ALL,"Russian");
        printf("MTEOpenTable() ERROR, ErrorMessage=%.*s\n", msg[1].dataLen, msg[1].data);
    }
    else if (tableHandles[2] == MTE_TSMR)
    {
        setlocale(LC_ALL,"Russian");
        printf("MTEOpenTable() ERROR, ErrorMessage=%.*s\n", msg[2].dataLen, msg[2].data);
    }
    else
        printf("MTEOpenTable() ERROR\n");


    std::string transactionName("ORDER"/*"CHANGE_PASSWORD"*/);
    //std::string transactionParam("S01-00001F00BLS P  TQBRMOEX        00001109900000000550000000000                                                 "); 
    //OrderTransaction* order = (OrderTransaction*)transactionParam.data();
    OrderTransaction order;
    memcpy(order.account, "A01-00001F00", _countof(order.account));
    order.buySell = 'B';
    order.mktLimit = 'L'; 
    order.splitFlag = 'S';
    order.immCancel = ' ';
    order.priceYieldEnterType = 'P';
    order.mmOrder = ' ';
    order.activationType = ' ';
    memcpy(order.secboard, "TQBR", _countof(order.secboard));
    memcpy(order.seccode, "MOEX        ", _countof(order.seccode));
    memcpy(order.price, "000011099", _countof(order.price));
    memcpy(order.quantity, "0000000055", _countof(order.quantity));
    memcpy(order.hidden, "0000000000", _countof(order.hidden));
    memcpy(order.brokerref, "                    ", _countof(order.brokerref));
    memcpy(order.orderValue, "                ", _countof(order.orderValue));
    memcpy(order.extRef, "            ", _countof(order.extRef));
    memcpy(order.activationTime, "      ", _countof(order.activationTime));
    memcpy(order.clientCode, "            ", _countof(order.clientCode));
    
	/*if (MTE_OK == (err = MTEExecTrans(connectionHandle, transactionName.c_str(), (const char*)&order, ErrMsg)))
    {
        printf("Order accepted!\n");
    }
    else
    {
	    CharToOemA(ErrMsg, ErrMsg);
	    printf("Error %d: '%s'\n", err, ErrMsg);
    }*/

	MTEDisconnect(connectionHandle);
	printf("Disconnected");

	return 0;
}

