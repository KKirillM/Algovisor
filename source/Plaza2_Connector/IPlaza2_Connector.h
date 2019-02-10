#pragma once

//Plaza2 API header
#define CG_SHORT_NAMES
#include "cgate.h"

#ifdef _USRDLL
#define DLL_FUNCTION __declspec(dllexport)
#else
#define DLL_FUNCTION __declspec(dllimport)
#endif

#define PLAZA2_STREAM_LIST \
    PLAZA2_STREAM_NAME(p2repl_FORTS_DEALS_REPL) \
    PLAZA2_STREAM_NAME(p2repl_FORTS_ORDLOG_REPL) \
    PLAZA2_STREAM_NAME(p2repl_FORTS_FEE_REPL) \
    PLAZA2_STREAM_NAME(p2repl_FORTS_FUTAGGR20_REPL) \
    PLAZA2_STREAM_NAME(p2repl_FORTS_FUTAGGR50_REPL) \
    PLAZA2_STREAM_NAME(p2repl_FORTS_FUTAGGR5_REPL) \
    PLAZA2_STREAM_NAME(p2repl_FORTS_FUTCOMMON_REPL) \
    PLAZA2_STREAM_NAME(p2repl_FORTS_FUTINFO_REPL) \
    PLAZA2_STREAM_NAME(p2repl_FORTS_FUTORDERBOOK_REPL) \
    PLAZA2_STREAM_NAME(p2repl_FORTS_FUTTRADE_REPL) \
    PLAZA2_STREAM_NAME(p2repl_FORTS_INFO_REPL) \
    PLAZA2_STREAM_NAME(p2repl_FORTS_MISCINFO_REPL) \
    PLAZA2_STREAM_NAME(p2repl_FORTS_MM_REPL) \
    PLAZA2_STREAM_NAME(p2repl_FORTS_OPTCOMMON_REPL) \
    PLAZA2_STREAM_NAME(p2repl_FORTS_OPTINFO_REPL) \
    PLAZA2_STREAM_NAME(p2repl_FORTS_OPTORDERBOOK_REPL) \
    PLAZA2_STREAM_NAME(p2repl_FORTS_OPTTRADE_REPL) \
    PLAZA2_STREAM_NAME(p2repl_FORTS_PART_REPL) \
    PLAZA2_STREAM_NAME(p2repl_FORTS_POS_REPL) \
    PLAZA2_STREAM_NAME(p2repl_FORTS_TNPENALTY_REPL) \
    PLAZA2_STREAM_NAME(p2repl_FORTS_VM_REPL) \
    PLAZA2_STREAM_NAME(p2repl_FORTS_VOLAT_REPL) \
    PLAZA2_STREAM_NAME(p2repl_MOEX_RATES_REPL) \
    PLAZA2_STREAM_NAME(p2repl_FORTS_CLR_REPL) \
    PLAZA2_STREAM_NAME(p2repl_MCXCC_USERDATA_REPL) \
    PLAZA2_STREAM_NAME(p2repl_MCXCC_AGGR_REPL) \
    PLAZA2_STREAM_NAME(p2repl_MCXCC_MDCOMMON_REPL) \
    PLAZA2_STREAM_NAME(p2repl_MCXCC_INFO_REPL) \
    PLAZA2_STREAM_NAME(p2repl_MCXCC_MDTRADE_REPL) \
    PLAZA2_STREAM_NAME(p2repl_RTS_INDEX_REPL)

enum P2StreamID
{
    // отводим для каждого потока репликации по 256 таблиц
    p2repl_FORTS_UNKNOWN,
    p2repl_FORTS_DEALS_REPL        = 0x0100,
    p2repl_FORTS_ORDLOG_REPL       = 0x0200,
    p2repl_FORTS_FEE_REPL          = 0x0300,
    p2repl_FORTS_FUTAGGR20_REPL    = 0x0400,
    p2repl_FORTS_FUTAGGR50_REPL    = 0x0500,
    p2repl_FORTS_FUTAGGR5_REPL     = 0x0600,
    p2repl_FORTS_FUTCOMMON_REPL    = 0x0700,
    p2repl_FORTS_FUTINFO_REPL      = 0x0800,
    p2repl_FORTS_FUTORDERBOOK_REPL = 0x0900,
    p2repl_FORTS_FUTTRADE_REPL     = 0x0A00,
    p2repl_FORTS_INFO_REPL         = 0x0B00,
    p2repl_FORTS_MISCINFO_REPL     = 0x0C00,
    p2repl_FORTS_MM_REPL           = 0x0D00,
    p2repl_FORTS_OPTCOMMON_REPL    = 0x0E00,
    p2repl_FORTS_OPTINFO_REPL      = 0x0F00,
    p2repl_FORTS_OPTORDERBOOK_REPL = 0x1000,
    p2repl_FORTS_OPTTRADE_REPL     = 0x1100,
    p2repl_FORTS_PART_REPL         = 0x1200,
    p2repl_FORTS_POS_REPL          = 0x1300,
    p2repl_FORTS_TNPENALTY_REPL    = 0x1400,
    p2repl_FORTS_VM_REPL           = 0x1500,
    p2repl_FORTS_VOLAT_REPL        = 0x1600,
    p2repl_MOEX_RATES_REPL         = 0x1700,
    p2repl_FORTS_CLR_REPL          = 0x1800,
    p2repl_MCXCC_USERDATA_REPL     = 0x1900,
    p2repl_MCXCC_AGGR_REPL         = 0x0100,
    p2repl_MCXCC_MDCOMMON_REPL     = 0x1A00,
    p2repl_MCXCC_INFO_REPL         = 0x1B00,
    p2repl_MCXCC_MDTRADE_REPL      = 0x1C00,
    p2repl_RTS_INDEX_REPL          = 0x1D00
};

struct Plaza2InitParameters
{    
	std::string connectionString;
	std::string environmentString;
	std::string replstateFolderPath;		// путь к файлам, в которые будут записываться состояния потоков перед их закрытием

};	// Plaza2InitParameters

// все функции интерфейсов IPlaza2ConnectorType1(2) и внеклассовых функций нужно оборачивать в обработчик этого типа исключения
class Plaza2ConnectorException : public std::exception 
{
public:
	Plaza2ConnectorException(const char* str, CG_RESULT code = 0) : std::exception(str), errorCode(code) {}
	CG_RESULT GetErrorCode() { return errorCode; }
    std::string ErrorCodeToStr() { char code[30]; sprintf_s(code, 30, "%d", errorCode); return std::string(code); }

private:
	CG_RESULT errorCode;

};	// Plaza2ConnectorException

enum Plaza2ConnectionStatus 
{
    NONE, 
    DISCONNECT, 
    WAIT, 
    OPEN 
};		

class IListener
{
public:
    virtual int GetIsin() const = 0;
    virtual int GetPrecision() const = 0;
    virtual bool IsCurrentSession(int sessionID) = 0;
    virtual const std::string& GetStreamName() const = 0;
    virtual P2StreamID GetStreamID() const = 0;
	virtual	const std::string& GetReplState() const = 0;
    virtual void SaveReplstateToFile(const std::string& replFilePath, const char* data, const size_t size) = 0;
    virtual void SetReplState(const std::string& _replstate) = 0;

};	// IListener

typedef CG_RESULT (*SystemMessageCallBackFunc)(const int type, msg_t* data, IListener* listener);
typedef CG_RESULT (*StreamMessageCallBackFunc)(msg_streamdata_t* data, IListener* listener);

enum P2STREAM_PREFIX
{
    P2REPL,
    P2ORDBOOK,
    P2SYS,
    P2MQREPLY,
    P2EMB
};

struct P2ListnerParameters
{
    P2STREAM_PREFIX prefix;             // префикс потока репликации
    std::string name;                   // имя потока репликации
    std::string postfixParams;          // параметры задаваемые при создании подписчика
    std::string tables;                 // список таблиц, которые должны траслироваться в данном потоке репликации
    std::string mode;                   // режим подписки на поток репликации online или snapshot
    int isin;                           // исин инструмента, если нужно привязать конкретный листенер к конкретному инструменту
    int precision;		                // количество знаков после запятой
	int	session_id;
    SystemMessageCallBackFunc func1;
    StreamMessageCallBackFunc func2;

    P2ListnerParameters() : isin(0), precision(0), func1(NULL), func2(NULL) {}

}; // P2RelpStreamParameters

typedef std::string ReplStreamName;
typedef std::map<ReplStreamName, P2ListnerParameters> P2ReplList;

// Класс подключения, для использования которого сперва определяется требуемый список потоков репликации,
// а затем после подключения получаются данные только из этого списка, добавлять новые потоки нельзя
class IPlaza2ConnectorType1
{
public:
	virtual void OpenConnection(const Plaza2InitParameters& initParams, const P2ReplList& streamsParams) = 0;
	virtual void Disconnect() = 0;
	virtual Plaza2ConnectionStatus GetConnectionStatus() const = 0;
	virtual bool IsConnected() const = 0;

};	// IPlaza2ConnectorType1

// Класс подключения, при использовании которого сперва создаётся подключение к роутеру,
// а затем потоки репликации могут добавляться/отключаться в процессе работы с соединением
class IPlaza2ConnectorType2
{
public:
    virtual void OpenConnection(const Plaza2InitParameters& initParams) = 0;
    virtual void Disconnect() = 0;
    virtual void OpenP2ReplListener(const P2ListnerParameters& streamParams) = 0;
    virtual void CloseP2ReplListener(const int isin) = 0;	                           
    virtual void CloseP2ReplListeners() = 0;	
    virtual Plaza2ConnectionStatus GetConnectionStatus() const = 0;
    virtual bool IsConnected() const = 0;

};	// IPlaza2ConnectorType2


#ifdef  __cplusplus
extern "C" {
#endif

// функции-фабрики, создающие экземпляры объектов IPlaza2ConnectorType1(2)
// рекомендуется оборачивать возвращаемое значение этих функций в интелектуальный указатель (или явно удалять возвращённый указатель по окончанию работы)
DLL_FUNCTION IPlaza2ConnectorType1* APIENTRY GetPlaza2ConnectorType1();
DLL_FUNCTION IPlaza2ConnectorType2* APIENTRY GetPlaza2ConnectorType2();

/* Функции для инициализации структур Plaza2InitParameters и P2ReplList данными из INI-файла
   Пример содержания INI-файла:

   [Plaza2]
   ;строка подключения к роутеру, его адрес, порт, имя приложения от которого подключаемся, таймаут ожидания подключения к роутеру
   connectionString=p2lrpcq://127.0.0.1:4001;app_name=Application_name;local_timeout=5000;name=Plaza2Connector;
   ;строка окружения
   environmentString=ini=.\ini\Plaza2Connector.ini;minloglevel=debug;key=11111111;
   ;путь к файлам replstate, которые хранят состояние потока перед его закрытием
   dbPath=\DB\Plaza2

   [Plaza2:p2repl]
   ;настройки потоков репликации
   FORTS_DEALS_REPL=tables=deal;mode=snapshot+online
   FORTS_ORDLOG_REPL=mode=snapshot+online
   */
DLL_FUNCTION void APIENTRY GetPlaza2InitParametersFromINI(const std::string& pathToINI, Plaza2InitParameters& params);
DLL_FUNCTION void APIENTRY GetP2ReplListParametersFromINI(const std::string& pathToINI, P2ReplList& params);

// запуск/останов процесса роутера Plaza2
/* В случае p2lrpcq соединение должно всегда закрываться корректно, т.к. в противном случае в роутере соеди-
   нение не деинициализируется, поэтому повторное соединение к роутеру под этим же именем будет не возможно без пере-
   запуска роутера.
   Возвращаемое значение:
   0  - роутер запущен
   1  - возникли проблемы при запуске роутера
   2  - переданы неверные аргументы в функцию
   -1 - роутер уже был запущен до вызова данной функции
   */
DLL_FUNCTION int APIENTRY StartPlaza2RouterProcess(const std::string& pathToRouter, const std::string& pathToINI, bool needConsoleWindow = false);
/* Возвращаемое значение:
    0  - роутер остановлен
    1  - возникли проблемы при остановке роутера
    2  - переданы неверные аргументы в функцию
    */
DLL_FUNCTION int APIENTRY StopPlaza2RouterProcess(const std::string& routerProcessName);

// запись логина и пароля в INI-файл роутера
DLL_FUNCTION void APIENTRY SetPlaza2RouterLoginPass(const std::string& pathToINI, const std::string& login, const std::string& password);

// получение isin_id по имеющемуся isin (например RTS-3.17) инструмента
// если такого isin не существует, то функция вернёт 0
DLL_FUNCTION int APIENTRY GetIsinIdFromIsin(IPlaza2ConnectorType2* connector, const std::string& isin);

// получение isin (например RTS-3.17) инструмента по имеющемуся isin_id
// если такого isin_id не существует, то функция вернёт нулевой указатель
DLL_FUNCTION const char* APIENTRY GettIsinFromIsinId(IPlaza2ConnectorType2* connector, int isin_id);

// возвращает указатель на данные типа FORTS_FUTINFO_REPL::fut_sess_contents, в которых содержится основные параметры по инструменту
// принимает на вход вторым параметром строку с именем (isin) инструмента (например RTS-3.17)
// рекомендуется оборачивать возвращаемое значение этой функции в интелектуальный указатель (или явно удалять возвращённый указатель по окончанию работы)
DLL_FUNCTION void* APIENTRY GetInstrumentContents(IPlaza2ConnectorType2* connector, const std::string& isin);

// заполняет переданную мапу тикерами всех доступных инструментов и основными параметрами всех инструментов типа FORTS_FUTINFO_REPL::fut_sess_contents
DLL_FUNCTION void APIENTRY GetAllInstrumentsContents(IPlaza2ConnectorType2* connector, void** data, int& count);

#ifdef  __cplusplus
}
#endif