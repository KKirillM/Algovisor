#pragma once

/// @brief Базовый класс для запуска инфраструктурных единиц
class BaseApp
{
    HANDLE hExitEvent;

public:
    BaseApp();
    ~BaseApp();

    void SetEvent() { ::SetEvent(hExitEvent); }
    bool CheckCommandLineArg(int argc, _TCHAR* argv[], char* usingString);
    bool OpenLogs();
    bool OpenIni();
    bool Init(int argc, _TCHAR* argv[], char* usingString);

    /// @brief   Основная процедура работы приложения. Наследник должен в реализации возвращать значение вызовом процедуры базового класса BaseApp::Run()
    virtual int Run() = 0;

    static BOOL WINAPI CtrlHandler(DWORD dwCtrlType);

    /// @brief   Чтение параметров из ini-файла
    virtual void ReadConfigParameters() = 0;

    /// @brief   Дополнительные инициализационные процедуры выполняемые в функции Init в наследнике класса
    virtual void AdditionalInitializations() = 0;

    /// @brief   Дополнительные инициализационные процедуры выполняемые в функции Init в наследнике класса
    // From MSDN: 
    // If the function handles the control signal, it should return TRUE. If it returns FALSE, the next handler function in the list of handlers for this process is used.
    virtual BOOL CtrlHandlerCallback(DWORD dwCtrlType) = 0;

private:
    void PrintPlaza2Version();
};