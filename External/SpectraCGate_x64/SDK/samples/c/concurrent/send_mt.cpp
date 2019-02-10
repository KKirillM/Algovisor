/*
        Copyright (c) 2012, MICEX-RTS. All rights reserved.

        Plaza-2 Client Gate API Usage Sample.
        Messages sender.

        All the software and documentation included in this and any
        other MICEX-RTS CGate Releasese is copyrighted by MICEX-RTS.

        Redistribution and use in source and binary forms, with or without
        modification, are permitted only by the terms of a valid
        software license agreement with MICEX-RTS.

        THIS SOFTWARE IS PROVIDED "AS IS" AND MICEX-RTS DISCLAIMS ALL WARRANTIES
        EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY IMPLIED WARRANTIES OF
        NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR
        PURPOSE.  MICEX-RTS DOES NOT WARRANT THAT USE OF THE SOFTWARE WILL BE
        UNINTERRUPTED OR ERROR-FREE.  MICEX-RTS SHALL NOT, UNDER ANY CIRCUMSTANCES, BE
        LIABLE TO LICENSEE FOR LOST PROFITS, CONSEQUENTIAL, INCIDENTAL, SPECIAL OR
        INDIRECT DAMAGES ARISING OUT OF OR RELATED TO THIS AGREEMENT OR THE
        TRANSACTIONS CONTEMPLATED HEREUNDER, EVEN IF MICEX-RTS HAS BEEN APPRISED OF
        THE LIKELIHOOD OF SUCH DAMAGES.
*/

/* 
	Send_mt - многопоточный пример посылки заявки. (Примечание: компилируется только под компиляторами, поддерживающими C+
	+11.). В треде 1 посылаются заявки.В треде 2 обрабатываются reply на посылаемые заявки.

	Send_mt - a multistream example of sending order. (Attention! This example can be compiled only by compilers supporting C++11.)
	Stream 1 contains orders. Stream 2 contains replies for the orders sent.
*/

#include <thread>
#include <memory>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <condition_variable>

#ifdef _WIN32
#include <windows.h>
#endif//_WIN32

//#include <assert.h>
#define CG_SHORT_NAMES

#include <cgate.h>
#include "cgate.hpp"
using namespace cgate; 

#include "forts_messages.h"
#include "send_mt_utils.h"

// helper variable used as a notification flag to exit
static volatile int bExit = 0;
const int numPubs = 5;
static volatile int id = 0;

#ifdef _WIN32
BOOL WINAPI InterruptHandler(DWORD reason)
{
	printf("\n----BREAK----\n");
	bExit = 1;
	return 1;
}
#else
#include <unistd.h>
void Sleep(int msec)
{
	usleep(msec * 1000);
}
#endif//_WIN32

const uint32_t defaultTimeOut = 1;

class MessageReplyListener : public Listener::IEventsListener
{
public:
	MessageReplyListener(const std::string& connSettings, const std::string& lsnSettings) :
	  _connSettings(connSettings),
	  _lsnSettings(lsnSettings),
	  _cnt(0),
	  _tmr(0)
	{
		_conn.reset(new Connection(connSettings));
		_lsn.reset(new Listener(*_conn, lsnSettings, this));
	}

	inline void processListener()
	{
		State state = _lsn->getState();
		if (state == State::Error)
			_lsn->close();
		if (state == State::Closed)
			_lsn->open(_lsnSettings);
	}

	inline void processConnection()
	{
		State state = _conn->getState();
		if (state == State::Error)
			_conn->close();
		if (state == State::Closed)
			_conn->open(_connSettings);
		if (state == State::Active)
		{
			_conn->process(defaultTimeOut);
			processListener();
		}
	}

	void run()
	{
		_tmr = cg_usec_clock();
		while(!bExit)
		{
			try
			{
				processConnection();
			}
			catch(Exception& e)
			{
				if (e.code() != CG_ERR_TIMEOUT)
					CGate::logError("Listener run: cgate exception %d, %s", e.code(), e.what().c_str());
			}
		}
		CGate::logInfo("Listener: finished");
	}

	uint32_t event(cg_conn_t* conn, cg_listener_t* listener, struct cg_msg_t* msg)
	{
		if (msg->type == CG_MSG_DATA)
		{
			cg_msg_data_t* msgData = reinterpret_cast<cg_msg_data_t*>(msg);
			if (cg_usec_clock() - _tmr >= 1000000)
			{
				CGate::logInfo("Receive: Msg per sec %d", _cnt);
				_tmr = cg_usec_clock();
				_cnt = 0;
			}
			++_cnt;
			//CGate::logInfo("Msg received reply for %d", msgData->user_id);
		}
		return CG_ERR_OK;
	}

	const std::shared_ptr<Connection>& getConnection()
	{
		return _conn;
	}

private:
	MessageReplyListener& operator=(const MessageReplyListener&);
	MessageReplyListener(const MessageReplyListener&);

private:
	std::shared_ptr<Connection> _conn;
	std::shared_ptr<Listener> _lsn;
	std::string _connSettings;
	std::string _lsnSettings;
	uint32_t _cnt;
	uint64_t _tmr;

};

class MessagePublisher
{
public:
	MessagePublisher() :
	    _cnt(0),
	    _tmr(0),
	    _userId(0)
	{
	}
	
	MessagePublisher(const std::shared_ptr< Connection>& conn, const std::string& pubSettings) :
	  _pubSettings(pubSettings),
	  _cnt(0),
	  _tmr(0),
	  _userId(0)
	{
		_conn = conn;
		_pub.reset(new Publisher(*_conn.get(), pubSettings));
	}

	void doLogic()
	{
		try
		{
			struct FutAddOrder* ord ;
			auto msg = _pub->newMessageByName("FutAddOrder");
			MessageData* msgData = dynamic_cast<MessageData*>(msg.get());
			if (msgData == nullptr)
				throw Exception(CG_ERR_INTERNAL);
			msgData->setUserId(++_userId);
			ord = (struct FutAddOrder*)msg->getData();
			strcpy(ord->broker_code, "HB00");
			strcpy(ord->isin, "RTS-6.12");
			strcpy(ord->client_code, "000");
			ord->type = 1;
			ord->dir = 1;
			ord->amount = 1;
			strcpy(ord->price, "1700");
			ord->ext_id = 0;
			uint64_t t = cg_usec_clock();
			if ( uint64_t(t - _tmr) >= 1000000LL)
			{
				CGate::logInfo("Send: Msg per sec %d", _cnt);
				_tmr = t;
				_cnt = 0;
			}
			++_cnt;
			_pub->post(msg.get(), CG_PUB_NEEDREPLY);
			//CGate::logInfo("Msg after send %d", _cnt);
		}
		catch(Exception& e)
		{
			CGate::logError("Publisher do logic: cgate exception %d, %s", e.code(), e.what().c_str());
			--_cnt;
		}
	}

	void processPublisher()
	{
		State state = _pub->getState();
		if (state == State::Error)
			_pub->close();
		if (state == State::Closed)
			_pub->open(_pubSettings);
		if (state == State::Active)
			doLogic();
	}

	void run()
	{
		_tmr = cg_usec_clock();
		while(!bExit)
		{
			try
			{
				State state = _conn->getState();
				if (state == State::Active)
				{
					processPublisher();
				}
			}
			catch(Exception& e)
			{
				CGate::logError("Publisher run: cgate exception %d, %s", e.code(), e.what().c_str());
			}
		}
		CGate::logInfo("Publisher: finished");
	}
	
	static bool pred()
	{
		return id == numPubs;
	}

	static void createAndRun(const std::shared_ptr<Connection>& conn, int i, std::unique_ptr<MessagePublisher>* publisher, std::mutex* cv_m, std::condition_variable* cv)
	{
		char settings[256];
		sprintf(settings, "p2mq://FORTS_SRV;category=FORTS_MSG;name=srvlink%d;timeout=5000;scheme=|FILE|forts_messages.ini|message", i);
		{
			{
				std::unique_lock<std::mutex> lk(*cv_m);
				id++;
				(*cv).wait(lk, pred);
				(*cv).notify_all();
			}
		}
		CGate::logInfo("Thread %d is ready to start\n", i);
		publisher->reset(new MessagePublisher(conn, settings));
		publisher->get()->run();
	}

private:
	MessagePublisher& operator=(const MessagePublisher&);
	MessagePublisher(const MessagePublisher&);
private:
	std::shared_ptr<Connection> _conn;
	std::shared_ptr<Publisher> _pub;
	std::string _pubSettings;
	uint32_t _userId;
	uint32_t _cnt;
	uint64_t _tmr;
};


int main()
{	
#ifdef _WIN32
	SetConsoleCtrlHandler(InterruptHandler, 1);
#endif//_WIN32
	try
	{
		CGate env("ini=send_mt.ini;key=11111111;");
		std::unique_ptr<MessageReplyListener> replyListener;
		replyListener.reset(new MessageReplyListener("p2tcp://127.0.0.1:4001;app_name=test_send", "p2mqreply://;ref=srvlink0"));
		std::thread threadListener(&MessageReplyListener::run, replyListener.get());
		Sleep(100);

		std::condition_variable cv;
		std::mutex cv_m;


		std::vector< std::unique_ptr<MessagePublisher> > publishers;
		for(int i = 0; i < numPubs; ++i)
		{
		    publishers.push_back(std::unique_ptr<MessagePublisher>(new MessagePublisher()));
		}

		std::vector<std::thread> threads;
		threads.reserve(numPubs);
		int id = 0;

		for(int i = 0; i < numPubs; ++i)
		{
			threads.push_back(std::thread(MessagePublisher::createAndRun, replyListener->getConnection(), i, &publishers[i], &cv_m, &cv));
		}

		threadListener.join();
		for(int i = 0; i < numPubs; ++i)
		{
			threads[i].join();
		}
		CGate::logInfo("All threads joined");
	}
	catch(Exception& e)
	{
		CGate::logError("Main: cgate exception %d, %s", e.code(), e.what().c_str());
	}
	return 0;
}
