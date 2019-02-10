#include "stdafx.h"
#include "DataAnalyzerApp.h"

#define USING_STRING "\nUsing: DataAnalyzer /INI:[path_to_ini_file]\n\n"

DataAnalyzerApp app;

int _tmain(int argc, _TCHAR* argv[])
{
    try
    {
        if (app.Init(argc, argv, USING_STRING))
        {
            return app.Run();
        }
    }
    catch (std::exception& err)
    {
        P2TRACECUST_EX((fo2base::gGetLog()->GetHandle(), P2LOG_SEVERITY_ERROR, "std::exception", "Unhalted exception: %s", err.what()))
        printf(err.what());
    }   

	return 1;
}

