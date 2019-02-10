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
    // ������� ��� ������� ������ ���������� �� 256 ������
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
	std::string replstateFolderPath;		// ���� � ������, � ������� ����� ������������ ��������� ������� ����� �� ���������

};	// Plaza2InitParameters

// ��� ������� ����������� IPlaza2ConnectorType1(2) � ������������ ������� ����� ����������� � ���������� ����� ���� ����������
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
    P2STREAM_PREFIX prefix;             // ������� ������ ����������
    std::string name;                   // ��� ������ ����������
    std::string postfixParams;          // ��������� ���������� ��� �������� ����������
    std::string tables;                 // ������ ������, ������� ������ �������������� � ������ ������ ����������
    std::string mode;                   // ����� �������� �� ����� ���������� online ��� snapshot
    int isin;                           // ���� �����������, ���� ����� ��������� ���������� �������� � ����������� �����������
    int precision;		                // ���������� ������ ����� �������
	int	session_id;
    SystemMessageCallBackFunc func1;
    StreamMessageCallBackFunc func2;

    P2ListnerParameters() : isin(0), precision(0), func1(NULL), func2(NULL) {}

}; // P2RelpStreamParameters

typedef std::string ReplStreamName;
typedef std::map<ReplStreamName, P2ListnerParameters> P2ReplList;

// ����� �����������, ��� ������������� �������� ������ ������������ ��������� ������ ������� ����������,
// � ����� ����� ����������� ���������� ������ ������ �� ����� ������, ��������� ����� ������ ������
class IPlaza2ConnectorType1
{
public:
	virtual void OpenConnection(const Plaza2InitParameters& initParams, const P2ReplList& streamsParams) = 0;
	virtual void Disconnect() = 0;
	virtual Plaza2ConnectionStatus GetConnectionStatus() const = 0;
	virtual bool IsConnected() const = 0;

};	// IPlaza2ConnectorType1

// ����� �����������, ��� ������������� �������� ������ �������� ����������� � �������,
// � ����� ������ ���������� ����� �����������/����������� � �������� ������ � �����������
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

// �������-�������, ��������� ���������� �������� IPlaza2ConnectorType1(2)
// ������������� ����������� ������������ �������� ���� ������� � ��������������� ��������� (��� ���� ������� ������������ ��������� �� ��������� ������)
DLL_FUNCTION IPlaza2ConnectorType1* APIENTRY GetPlaza2ConnectorType1();
DLL_FUNCTION IPlaza2ConnectorType2* APIENTRY GetPlaza2ConnectorType2();

/* ������� ��� ������������� �������� Plaza2InitParameters � P2ReplList ������� �� INI-�����
   ������ ���������� INI-�����:

   [Plaza2]
   ;������ ����������� � �������, ��� �����, ����, ��� ���������� �� �������� ������������, ������� �������� ����������� � �������
   connectionString=p2lrpcq://127.0.0.1:4001;app_name=Application_name;local_timeout=5000;name=Plaza2Connector;
   ;������ ���������
   environmentString=ini=.\ini\Plaza2Connector.ini;minloglevel=debug;key=11111111;
   ;���� � ������ replstate, ������� ������ ��������� ������ ����� ��� ���������
   dbPath=\DB\Plaza2

   [Plaza2:p2repl]
   ;��������� ������� ����������
   FORTS_DEALS_REPL=tables=deal;mode=snapshot+online
   FORTS_ORDLOG_REPL=mode=snapshot+online
   */
DLL_FUNCTION void APIENTRY GetPlaza2InitParametersFromINI(const std::string& pathToINI, Plaza2InitParameters& params);
DLL_FUNCTION void APIENTRY GetP2ReplListParametersFromINI(const std::string& pathToINI, P2ReplList& params);

// ������/������� �������� ������� Plaza2
/* � ������ p2lrpcq ���������� ������ ������ ����������� ���������, �.�. � ��������� ������ � ������� �����-
   ����� �� ������������������, ������� ��������� ���������� � ������� ��� ���� �� ������ ����� �� �������� ��� ����-
   ������� �������.
   ������������ ��������:
   0  - ������ �������
   1  - �������� �������� ��� ������� �������
   2  - �������� �������� ��������� � �������
   -1 - ������ ��� ��� ������� �� ������ ������ �������
   */
DLL_FUNCTION int APIENTRY StartPlaza2RouterProcess(const std::string& pathToRouter, const std::string& pathToINI, bool needConsoleWindow = false);
/* ������������ ��������:
    0  - ������ ����������
    1  - �������� �������� ��� ��������� �������
    2  - �������� �������� ��������� � �������
    */
DLL_FUNCTION int APIENTRY StopPlaza2RouterProcess(const std::string& routerProcessName);

// ������ ������ � ������ � INI-���� �������
DLL_FUNCTION void APIENTRY SetPlaza2RouterLoginPass(const std::string& pathToINI, const std::string& login, const std::string& password);

// ��������� isin_id �� ���������� isin (�������� RTS-3.17) �����������
// ���� ������ isin �� ����������, �� ������� ����� 0
DLL_FUNCTION int APIENTRY GetIsinIdFromIsin(IPlaza2ConnectorType2* connector, const std::string& isin);

// ��������� isin (�������� RTS-3.17) ����������� �� ���������� isin_id
// ���� ������ isin_id �� ����������, �� ������� ����� ������� ���������
DLL_FUNCTION const char* APIENTRY GettIsinFromIsinId(IPlaza2ConnectorType2* connector, int isin_id);

// ���������� ��������� �� ������ ���� FORTS_FUTINFO_REPL::fut_sess_contents, � ������� ���������� �������� ��������� �� �����������
// ��������� �� ���� ������ ���������� ������ � ������ (isin) ����������� (�������� RTS-3.17)
// ������������� ����������� ������������ �������� ���� ������� � ��������������� ��������� (��� ���� ������� ������������ ��������� �� ��������� ������)
DLL_FUNCTION void* APIENTRY GetInstrumentContents(IPlaza2ConnectorType2* connector, const std::string& isin);

// ��������� ���������� ���� �������� ���� ��������� ������������ � ��������� ����������� ���� ������������ ���� FORTS_FUTINFO_REPL::fut_sess_contents
DLL_FUNCTION void APIENTRY GetAllInstrumentsContents(IPlaza2ConnectorType2* connector, void** data, int& count);

#ifdef  __cplusplus
}
#endif