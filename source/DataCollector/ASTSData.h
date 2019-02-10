#pragma once

#include "ASTS_Connector.h"

class ASTSData
{
public:
    static void ASTS_CallbackStock(TMTETable* tableData, TTable* tableInterface);
    static void ASTS_CallbackCurrency(TMTETable* tableData, TTable* tableInterface);
};