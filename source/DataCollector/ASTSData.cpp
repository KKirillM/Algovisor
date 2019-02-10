#include "stdafx.h"
#include "ConnectionManager.h"
#include "ASTSData.h"

void ASTSData::ASTS_CallbackStock(TMTETable* tableData, TTable* /*tableInterface*/)
{
    static CReplStruct eReplMessage;
    char* ptr = (char*)tableData->rows;
    for (int i = 0; i < tableData->rowsCount; ++i)
    {
        TMTERow row;
        row.SetValues(ptr);
        UINT32 messageSize = (UINT32)row.GetSize();
        void* message = (void*)(ptr-messageSize);

        try
        {
            eReplMessage.Init(message, messageSize, tableData->ref);
        }
        catch (fo2base::CP2Error& err)
        {
            P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), 
                P2LOG_SEVERITY_ERROR, 
                "ASTS_CallbackStock", "error_message: %s",
                err.GetMessageA().c_str()));
            return;
        }

        WriteDataToeShmem(eReplMessage, ASTS_Stock);
    }
}

void ASTSData::ASTS_CallbackCurrency(TMTETable* tableData, TTable* /*tableInterface*/)
{
    static CReplStruct eReplMessage;
    char* ptr = (char*)tableData->rows;
    for (int i = 0; i < tableData->rowsCount; ++i)
    {
        TMTERow row;
        row.SetValues(ptr);
        UINT32 messageSize = (UINT32)row.GetSize();
        void* message = (void*)(ptr-messageSize);

        try
        {
            eReplMessage.Init(message, messageSize, tableData->ref);
        }
        catch (fo2base::CP2Error& err)
        {
            P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), 
                P2LOG_SEVERITY_ERROR, 
                "ASTS_CallbackCurrency", "error_message: %s",
                err.GetMessageA().c_str()));
            return;
        }

        WriteDataToeShmem(eReplMessage, ASTS_Currency);
    }
}