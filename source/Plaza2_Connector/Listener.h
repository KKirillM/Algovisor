#pragma once

//#include "P2OrderBook.h"
#include "IPlaza2_Connector.h"

class Listener : public IListener
{
public:
	enum FlowState { CLOSE, WAIT_TO_CLOSE, OPEN };

	Listener(conn_t** conn, const P2ListnerParameters& streamParams);
	virtual ~Listener();

    virtual void OpenListener() {}
    void CloseListener();
    void DestroyListener();
    bool CheckListenerState();

    static CG_RESULT MessageCallback(conn_t* conn, listener_t* listener, struct msg_t* message, void* data);

    void SetStreamID(const std::string& stream_ID_string)
    {
#undef PLAZA2_STREAM_NAME
#define PLAZA2_STREAM_NAME(x) \
         if (stream_ID_string == #x) \
        { \
            stream_id = P2StreamID::##x; \
        }

        PLAZA2_STREAM_LIST
    }

    // IListener
	virtual int GetIsin() const { return isin; }
    virtual int GetPrecision() const { return precision; }

	virtual bool IsCurrentSession(int sessionID) 
    { 
        if (sessionID != session_id)
        {
            session_id = sessionID;
            return false;
        }
            
        return true; 
    }

    virtual const std::string& GetStreamName() const { return stream_name; }
    virtual P2StreamID GetStreamID() const { return stream_id; }
	virtual const std::string& GetReplState() const { return replstate; }
    virtual void SaveReplstateToFile(const std::string& replFilePath, const char* data, const size_t size);
    virtual void SetReplState(const std::string& _replstate) 
    { 
        if (!_replstate.empty()) 
        {
            replstate = "replstate=" + _replstate; 
        }
    }
	
	struct find_name : public std::binary_function<Listener*, int, bool>
	{
		bool operator()(const Listener* flow, int isin) const
		{
			return flow->isin == isin;
		}
	};

protected:
	conn_t**		            plaza2ConnectionObject;
	listener_t*		            plaza2Listener;
    int				            isin;					// ���� �����������, ��� �������� ��������� ������ ����� ���������� (����� �� ��������������)
    int                         precision;		        // ���������� ������ ����� �������  (����� �� ��������������)
    int				            session_id;				// ���� ������� �������� ������
    P2StreamID                  stream_id;              // id ������ ���������� (����� ��� ���������� ����������� ������ ������ ������������� ������� � �������� ������� ������ ����������)
    P2STREAM_PREFIX             prefix;                 // ������� ������ ����������
    std::string		            stream_name;			// ��� ������ ����������
    std::string                 posfix;
    std::string		            tables;					// ������ � �������� ������, ������� �� ���������� ��� �������� ������ ����������
    std::string		            mode;					// ������ ��� ������� lsn_open, � ������� ���������� mode=(snapshot..online)
    std::string		            replstate;				// ��������� ������ ������, ����������� ����� ��������� ������, ����� ����� � ������ ����� ����� ���� ������ �������� ������

    SystemMessageCallBackFunc   func1;                  // �-��� �������� ��� ���� ��������� ��������� ������ ����������
    StreamMessageCallBackFunc   func2;                  // �-��� �������� ��� ��������� � ������� ������ ����������

    //P2OrderBook		    book;					// ����� ������ ��� �����������, ������ �� �������� ��������� ����� ������� ��������� ������ Listener
    //P2Trade			    trade;					// ������ ���������� ����������������� ������, ������� ��������� �� �����������, ��� �������� ���������� ������ Listener
    //Order			        incomingOrder;			// ��������� ��������� ����� ������������ � ������ (��� ������� ������� �������������� �����)

};	//Listener

class P2Listener : public Listener
{
public:
    P2Listener(conn_t** conn, const P2ListnerParameters& streamParams);
    virtual ~P2Listener();

    virtual void OpenListener();
};