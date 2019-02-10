#pragma once
#include "..\Plaza2Utils\BaseApp.h"
#include "..\Plaza2Utils\EDBStorageReader.h"
#include "Plaza2Callbacks.h"
#include "Logger.h"


class DataAnalyzerApp : public BaseApp
{
    std::string pathToDB;
    fo2base::CP2Time startDate;
    fo2base::CP2Time endDate;

    bool stopFlag;
    Plaza2Callbacks callbacks;
    EDBStorageReader edbReader;

public:
    DataAnalyzerApp();
    ~DataAnalyzerApp();

    void StopApp() { stopFlag = true; }

    virtual int Run();
    virtual void ReadConfigParameters();
    virtual void AdditionalInitializations();
    virtual BOOL CtrlHandlerCallback(DWORD dwCtrlType);

};  // DataAnalyzerApp

