#ifndef _CGATE_HPP_
#define _CGATE_HPP_

namespace cgate
{

enum State
{
	Closed = CG_STATE_CLOSED,
	Opening = CG_STATE_OPENING,
	Error = CG_STATE_ERROR,
	Active = CG_STATE_ACTIVE
};
	
class Exception
{
public:
	Exception(uint32_t code) :
		_code(code),
		_what(std::string(cg_err_getstr(code)))
	{

	}
	inline const std::string& what()
	{
		return _what;
	}

	inline uint32_t code()
	{
		return _code;
	}
private:
	std::string _what;
	uint32_t _code;
};

class CGate
{
public:
	CGate(const std::string& settings)
	{
		uint32_t res = cg_env_open(settings.c_str());
		if (res != CG_ERR_OK)
			throw Exception(res);
	}

	~CGate()
	{
		cg_env_close();
	}

	void open(const std::string& settings)
	{
		uint32_t res = cg_env_open(settings.c_str());
		if (res != CG_ERR_OK)
			throw Exception(res);
	}

	void close()
	{
		cg_env_close();
	}

	static void logInfoStr(const std::string& str)
	{
		cg_log_infostr(str.c_str());
	}

	static void logInfo(const char* str,...)
	{
		va_list va;
		va_start(va, str);
		cg_log_va_info(str, va);
		va_end(va);
	}

	static void logErrorStr(const std::string& str)
	{
		cg_log_errorstr(str.c_str());
	}

	static void logError(const char* str,...)
	{
		va_list va;
		va_start(va, str);
		cg_log_va_error(str, va);
		va_end(va);
	}

	static void logDebugStr(const std::string& str)
	{
		cg_log_debugstr(str.c_str());
	}

	static void logDebug(const char* str,...)
	{
		va_list va;
		va_start(va, str);
		cg_log_va_debug(str, va);
		va_end(va);
	}

	static void logTraceStr(const std::string& str)
	{
		cg_log_tracestr(str.c_str());
	}

	static void logTrace(const std::string& str,...)
	{
		va_list args;
		va_start(args, str);
		cg_log_va_trace(str.c_str(), args);
		va_end(args);
	}
private:
	CGate& operator=(const CGate&);
	CGate(const CGate&);
};

class Connection
{
public:
	
	Connection(const std::string& settings)
	{
		uint32_t res = cg_conn_new(settings.c_str(), &_conn);
		if ( res != CG_ERR_OK)
			throw Exception(res);
	}

	~Connection()
	{
		cg_conn_destroy(_conn);
		_conn = 0;
	}

	void open(const std::string& settings)
	{
		uint32_t res = cg_conn_open(_conn, settings.c_str());
		if (res != CG_ERR_OK)
			throw Exception(res);
	}

	void close()
	{
		uint32_t res = cg_conn_close(_conn);
		if (res != CG_ERR_OK)
			throw Exception(res);
	}

	bool process(uint32_t timeout)
	{
		uint32_t res = cg_conn_process(_conn, timeout, 0);
		if (res == CG_ERR_TIMEOUT)
			return false;
		else if (res != CG_ERR_OK)
			throw Exception(res);
		else
			return true;
	}

	State getState()
	{
		uint32_t state;
		uint32_t res = cg_conn_getstate(_conn, &state);
		if (res != CG_ERR_OK)
			throw Exception(res);
		return static_cast<State>(state);
	}
private:
	Connection& operator=(const Connection&);
	Connection(const Connection&);
	friend class Listener;
	friend class Publisher;
private:
	cg_conn_t* _conn;
};

class Listener
{
public:
	struct IEventsListener
	{
		virtual uint32_t event(cg_conn_t* conn, cg_listener_t* listener, struct cg_msg_t* msg) = 0;
	};

	Listener(const Connection& conn, const std::string& settings, IEventsListener* evntLsn) :
		_evntLsn(evntLsn)
	{
		uint32_t res = cg_lsn_new(conn._conn, settings.c_str(), Listener::_callback, this, &_lsn);
		if ( res != CG_ERR_OK)
			throw Exception(res);
	}

	~Listener()
	{
		cg_lsn_destroy(_lsn);
		_lsn = 0;
	}

	void open(const std::string& settings)
	{
		uint32_t res = cg_lsn_open(_lsn, settings.c_str());
		if ( res != CG_ERR_OK)
			throw Exception(res);
	}

	void close()
	{
		uint32_t res = cg_lsn_close(_lsn);
		if ( res != CG_ERR_OK)
			throw Exception(res);
	}

	static CG_RESULT _callback(cg_conn_t* conn, cg_listener_t* listener, struct cg_msg_t* msg, void* data)
	{
		Listener* _this = static_cast<Listener*>(data);
		return _this->_evntLsn->event(conn, listener, msg);
	}

	State getState()
	{
		uint32_t state;
		uint32_t res = cg_lsn_getstate(_lsn, &state);
		if (res != CG_ERR_OK)
			throw Exception(res);
		return static_cast<State>(state);
	}
private:
	Listener& operator=(const Listener&);
	Listener(const Listener&);

private:
	cg_listener_t* _lsn;
	IEventsListener* _evntLsn;

};

class Publisher;

class Message
{
public:
	Message(cg_msg_t* msg, Publisher* producer) :
		_msg(msg),
		_producer(producer)
	{
	}

	virtual ~Message();

	void* getData()
	{
		return _msg->data;
	}

private:
	friend class Publisher;

	Message& operator=(const Message&);
	Message(const Message&);
protected:
	cg_msg_t* _msg;
private:
	Publisher* _producer;
};

class MessageData : public Message
{
public:
	MessageData(cg_msg_t* msg, Publisher* producer) : Message(msg, producer)
	{
	}

	void setUserId(uint32_t userId)
	{
		cg_msg_data_t*  data = reinterpret_cast<cg_msg_data_t*>(_msg);
		data->user_id = userId;
	}
};

class Publisher
{
public:
	enum Key
	{
		Index = CG_KEY_INDEX,
		Id = CG_KEY_ID
	};

	Publisher(const Connection& conn, const std::string& settings)
	{
		uint32_t res = cg_pub_new(conn._conn, settings.c_str(), &_pub);
		if ( res != CG_ERR_OK)
			throw Exception(res);
	}

	~Publisher()
	{
		cg_pub_destroy(_pub);
		_pub = 0;
	}

	uint32_t open(const std::string& settings)
	{
		uint32_t res = cg_pub_open(_pub, settings.c_str());
		if ( res != CG_ERR_OK)
			throw Exception(res);
		return res;
	}

	uint32_t close()
	{
		uint32_t res = cg_pub_close(_pub);
		if ( res != CG_ERR_OK)
			throw Exception(res);
		return res;
	}

	State getState()
	{
		uint32_t state;
		uint32_t res = cg_pub_getstate(_pub, &state);
		if (res != CG_ERR_OK)
			throw Exception(res);
		return static_cast<State>(state);
	}

	std::shared_ptr<Message> newMessageByName(const std::string& name)
	{
		cg_msg_t* msg(0); 
		uint32_t res = cg_pub_msgnew(_pub, CG_KEY_NAME, name.c_str(), &msg);
		if (res != CG_ERR_OK)
			throw Exception(res);
		std::shared_ptr<Message> msgOut;
		if (msg->type == CG_MSG_DATA)
		{
			msgOut.reset(new MessageData(msg, this));
		}
		return msgOut;
	}

	std::shared_ptr<Message> newMessageById(Key key, uint32_t id)
	{
		cg_msg_t* msg(0); 
		uint32_t res = cg_pub_msgnew(_pub, key, &id, &msg);
		if (res != CG_ERR_OK)
			throw Exception(res);
		std::shared_ptr<Message> msgOut;
		if (msg->type == CG_MSG_DATA)
		{
			msgOut.reset(new MessageData(msg, this));
		}
		return msgOut;
	}

	void post(Message* msg, uint32_t flag)
	{
		uint32_t res = cg_pub_post( _pub, msg->_msg, flag);
		if (res != CG_ERR_OK)
			throw Exception(res);
	}
private:
	friend class Message;
	void freeMessage(cg_msg_t* _msg)
	{
		cg_pub_msgfree(_pub, _msg);
	}

	Publisher& operator=(const Publisher&);
	Publisher(const Publisher&);

private:
	cg_publisher_t* _pub;
};


Message::~Message()
{
	_producer->freeMessage(_msg);
}

};

#endif //_CGATE_HPP_