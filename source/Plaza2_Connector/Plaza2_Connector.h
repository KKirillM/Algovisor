#pragma once

// ���������� ���������� ������ ����������� ����������� ��� Plaza2
// ����������� ���������� �������� �� ������ ��������� � ����� ������:
// http://www.cyberguru.ru/cpp/cpp-exporting-cpp-classes-from-dll.html?showall=1
// ������� ������������� ��������������� ������ ������������ ����, ��� ������ ������������ �����
// ��� ��������� ���������� ������ �� �������������


#include "Plaza2_defines.h"
#include "IPlaza2_Connector.h"

#define REPLSTATEFILE "_replstate.rpst"

class Listener;

class Plaza2Connector
{
public:
	 Plaza2Connector();
	 virtual ~Plaza2Connector();

	 BAN_COPY_CONSTRUCTOR(Plaza2Connector)					        // ����������� ���������� ������� ����������

public:
    typedef std::vector<Listener*> ListenerList;

    Plaza2ConnectionStatus GetConnectionStatus() const { return status; }
    bool IsConnected() const { return status == OPEN; }
    ListenerList& GetListenerList() { return listenerlist; }
    conn_t* GetConnectionObject() { return plaza2ConnectionObject; }

    typedef unsigned (__stdcall * CallbackThreadFunc) (void *);

    bool CheckConnectionState();
    void ChangeConnectionStatus(Plaza2ConnectionStatus _status) { status = _status; }
    bool IsRunCycle() { return bStopRun; }
    SRWLOCK& GetSRWLOCK() { return lock; }

protected:
	void SetRouterLoginPass(const std::string& pathToINI, const std::string& login, const std::string& pass);
	void OpenConnectionToRouter(const std::string& url, const std::string& environmentString);
	void CloseConnectionToRouter();
    void ClearStopFlag() { bStopRun = false; }				// ����� ����� ��������� ������ ���������
	void RunCallbackThread(CallbackThreadFunc func);		// ������ ������ ������ ��������� ��� ���� ������� ������
	void StopCallbackThread();				                // ������� ������ ���������
	void ReadReplstates(const std::string& pathToReplstateDB);
    void WaitUntilCallbackStart();                          // �������� ������ ������ ��������
    void WaitUntilCallbackEnd();		                    // �������� ���������� ������ ��������    
    
protected:
    static int plaza2ActiveConnetionCounter;
	static bool environmentInitFlag;				// ���� true, �� ������� env_open() ���������� � ��������� ���������������                     
	conn_t* plaza2ConnectionObject;
	volatile Plaza2ConnectionStatus status;
	volatile bool bStopRun;							// ���� ������ �� ����� ��������� ��������� ������� ����������
	ListenerList listenerlist;						
	HANDLE callbackThreadHandle;	
    HANDLE callbackEventHandle;                     // ����� ��� ��������
    SRWLOCK lock;                                   // ������ ������������� ��� ������� � ������� ��������� ListenerList

	friend class Listener;

};	//Plaza2Connector

class Plaza2ConnectorType1 : public IPlaza2ConnectorType1, public Plaza2Connector
{
public:
    Plaza2ConnectorType1();
    virtual ~Plaza2ConnectorType1();

    BAN_COPY_CONSTRUCTOR(Plaza2ConnectorType1)

public:
    static unsigned int __stdcall CallbackThreadFunc(void* ptr);    // �-��� ��� ������ � ������� ���������� ��������� ���������� ���������

    virtual void OpenConnection(const Plaza2InitParameters& initParams, const P2ReplList& streamsParams);
    virtual void Disconnect();
    virtual Plaza2ConnectionStatus GetConnectionStatus() const { return Plaza2Connector::GetConnectionStatus(); }
    virtual bool IsConnected() const { return Plaza2Connector::IsConnected(); }

private:
    void OpenP2ReplListeners(const P2ReplList& streamsParams);
    void CloseP2ReplListeners();

};	// Plaza2ConnectorType1

class Plaza2ConnectorType2 : public IPlaza2ConnectorType2, public Plaza2Connector
{
public:
    Plaza2ConnectorType2();
    virtual ~Plaza2ConnectorType2();

    BAN_COPY_CONSTRUCTOR(Plaza2ConnectorType2)

public:
    static unsigned int __stdcall CallbackThreadFunc(void* ptr);    // �-��� ��� ������ � ������� ���������� ��������� ���������� ���������

    virtual void OpenConnection(const Plaza2InitParameters& initParams);
    virtual void Disconnect();
    virtual void OpenP2ReplListener(const P2ListnerParameters& streamParams);
    virtual void CloseP2ReplListener(const int isin);                           
    virtual void CloseP2ReplListeners();					               
    virtual Plaza2ConnectionStatus GetConnectionStatus() const { return Plaza2Connector::GetConnectionStatus(); }
    virtual bool IsConnected() const { return Plaza2Connector::IsConnected(); }

};	// Plaza2ConnectorType2