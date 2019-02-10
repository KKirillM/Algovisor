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
    int				            isin;					// исин инструмента, для которого открываем данный поток репликации (может не использоваться)
    int                         precision;		        // количество знаков после запятой  (может не использоваться)
    int				            session_id;				// айди текущей торговой сессии
    P2StreamID                  stream_id;              // id потока репликации (нужен для вычисления уникального номера каждой транслируемой таблицы в пределах данного потока репликации)
    P2STREAM_PREFIX             prefix;                 // префикс потока репликации
    std::string		            stream_name;			// имя потока репликации
    std::string                 posfix;
    std::string		            tables;					// строка с перечнем таблиц, которые мы подключаем для текущего потока репликации
    std::string		            mode;					// строка для функции lsn_open, в которой содержится mode=(snapshot..online)
    std::string		            replstate;				// состояние потока данных, присылается перед закрытием потока, чтобы потом с тогоже места можно было начать получать данные

    SystemMessageCallBackFunc   func1;                  // ф-ция коллбэка для всех служебных сообщений потока репликации
    StreamMessageCallBackFunc   func2;                  // ф-ция коллбэка для сообщений с данными потока репликации

    //P2OrderBook		    book;					// книга заявок для инструмента, данные по которому слушаются через текущий экземпляр класса Listener
    //P2Trade			    trade;					// данные последнего неагрегированного трейда, который произошёл по инструменту, для текущего экземпляра класса Listener
    //Order			        incomingOrder;			// последний встречный ордер выставленный в стакан (или другими словами агрегированный трейд)

};	//Listener

class P2Listener : public Listener
{
public:
    P2Listener(conn_t** conn, const P2ListnerParameters& streamParams);
    virtual ~P2Listener();

    virtual void OpenListener();
};