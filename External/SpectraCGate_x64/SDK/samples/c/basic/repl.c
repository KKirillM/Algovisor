/*
        Copyright (c) 2012, MICEX-RTS. All rights reserved.

        Plaza-2 Client Gate API Usage Sample.
        Replication DataStream Client.

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
	Repl - получение реплики данных по потоку. ѕример печатает все получаемые сообщени€ в log. ѕри разрыве соединени€ реплика
	начинаетс€ сначала. ¬ходных аргументов нет.

	Repl allows to receive data replica for a stream and saves all incoming messages into log file. 
	When disconnected, the replica starts over. The example does not have input parameters.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif//_WIN32

#define CG_SHORT_NAMES
#include ".\..\..\..\include\cgate.h"
#include <assert.h>
#include <string.h>

// helper macro to call CGate function
// and exit if there was error

#define CHECK_FAIL(f) \
	{ \
		uint32_t res = f; \
		if (res != CG_ERR_OK) \
		{ \
			log_info("Client gate error: %x", res); \
			exit(1); \
		} \
	} 

// helper macro to call CGate function
// and print warning if there was error

#define WARN_FAIL(f) \
	{ \
		uint32_t res = f; \
		if (res != CG_ERR_OK) \
			log_info("Error: %x", res); \
	} 


// helper variable used as a notification flag to exit
static volatile int bExit = 0;

#ifdef _WIN32
BOOL WINAPI InterruptHandler(DWORD reason)
{
	log_info("----BREAK----");
	bExit = 1;
	return 1;
}
#endif//_WIN32


// This callback may be used to test cg_msg_dump function
// Dumps all the messages it receives 
CG_RESULT MessageCallbackSimple(cg_conn_t* conn, cg_listener_t* listener, struct cg_msg_t* msg, void* data)
{
	struct scheme_desc_t* scheme;
	size_t bufSize = 0;
	
	// get stream scheme
	if (cg_lsn_getscheme(listener, &scheme) != CG_ERR_OK)
		scheme = 0;
		
	// get buffer needed to dump message
	if (cg_msg_dump(msg, scheme, 0, &bufSize) == CG_ERR_BUFFERTOOSMALL)
	{	
				
		char* buffer = (char*)malloc(bufSize);
		
		// dump message to buffer and print its content to stdout
		if (cg_msg_dump(msg, scheme, buffer, &bufSize) == CG_ERR_OK)
			log_info("message: %s\n", buffer);
		free(buffer);
		
	}
	return CG_ERR_OK;
}

// This callback is what typically message callback looks like
// Processes messages that are important to Plaza-2 datastream lifecycle
CG_RESULT MessageCallback(cg_conn_t* conn, cg_listener_t* listener, struct cg_msg_t* msg, void* data)
{
	switch (msg->type)
	{
	case CG_MSG_STREAM_DATA: 
		{
		// data recevied. print data properties (message name, index etc)

		struct cg_msg_streamdata_t* replmsg = (struct cg_msg_streamdata_t* )msg;
		if(replmsg->msg_index == 0)
		{
			int64_t *revision = (int64_t *)data;
			if (replmsg->rev < *revision)
			{
				cg_log_error("Incorrect revison in replica server = %lld, client = %lld.", replmsg->rev, *revision);
				exit(1);
			}
			assert(replmsg->rev > *revision);
			*revision = replmsg->rev;
		}
		log_info("DATA message SEQ=%lld [table:[idx=%llu, id=%X, name=%s], dataSize:%llu]\n", 
			(long long)replmsg->rev,
			(unsigned long long)replmsg->msg_index,
			replmsg->msg_id, 
			replmsg->msg_name, 
			(unsigned long long)replmsg->data_size);
		break;
		}
	case CG_MSG_P2REPL_ONLINE:
		// datastream is in ONLINE state
		log_info("ONLINE");
		break;
	case CG_MSG_TN_BEGIN:
		// next data block is about to be received
		log_info("TN BEGIN");
		break;
	case CG_MSG_TN_COMMIT:
		// data block finished. data is consistent now
		log_info("TN COMMIT");
		break;
	case CG_MSG_OPEN:
		// stream is opened, its scheme is available
		log_info("OPEN");

		{
			uint64_t *revision = (uint64_t *)data;
			// this is how scheme is handled
			struct cg_scheme_desc_t* schemedesc = 0;
			*revision = 0;
			WARN_FAIL(cg_lsn_getscheme(listener, &schemedesc));
			if (schemedesc != 0)
			{
				struct cg_message_desc_t* msgdesc = schemedesc->messages;
				while (msgdesc)
				{
					struct cg_field_desc_t* fielddesc = msgdesc->fields;
					log_info("Message %s, block size = %d", msgdesc->name, msgdesc->size);

					while (fielddesc)
					{
						log_info("\tField %s = %s [size=%d, offset=%d]", fielddesc->name, fielddesc->type, fielddesc->size, fielddesc->offset);
						fielddesc = fielddesc->next;
					}
					
					msgdesc = msgdesc->next;
				}
			}
		}
		break;
	case CG_MSG_CLOSE:
		// Stream is closed, no more data will be received
		log_info("CLOSE");
		break;
	case CG_MSG_P2REPL_LIFENUM:
		// Stream data scheme's life number was changed.
		// complete data snapshot re-replication will occur.
		log_info("Life number changed to: %u, flags: %u",
			((struct cg_data_lifenum_t*)msg->data)->life_number, ((struct cg_data_lifenum_t*)msg->data)->flags);
		break;
	case CG_MSG_P2REPL_CLEARDELETED:
		{
			struct cg_data_cleardeleted_t * msg_clear_deleted = (struct cg_data_cleardeleted_t *)msg->data;
			log_info("Clear deleted table_index: %d, table_rev: %lld.", msg_clear_deleted->table_idx, (long long)msg_clear_deleted->table_rev);
			if ( msg_clear_deleted->table_rev == CG_MAX_REVISON )
			{
				log_info("table_index: %d, rev start from 1.", msg_clear_deleted->table_idx);
			}
		}
		break;
	case CG_MSG_P2REPL_REPLSTATE:
		// Datastream state is recevied to be used for resume later.
		// Message content may be stored anywhere as a string
		// and then used in cg_lsn_open(..) call as value for
		// parameter "replstate="
		log_info("Replica state: %s", msg->data);
		break;
	default:
		// Other messages get logged but not handled
		log_info("Message 0x%X", msg->type);
	}

	// code returns 0, since there were no errors.
	// any other error code may be return and it will be logged
	return CG_ERR_OK;
}

int main(int argc, char* argv[])
{
	// connection pointer
	conn_t* conn = 0;
	// listener pointer
	listener_t* listener = 0;

	int64_t revision = 0;
	int lifenum = 0;
	size_t port = 4001;
	// connection init string

	// Defines connection with P2MQRouter which is located at
	// the same machine and listening on port 4001 using TCP protocol
	// Plaza-2 "application name" will be "test1"
	char conn_str[100] = "p2tcp://127.0.0.1:4001;app_name=test_repl";

	// listener init string (you can uncomment any line
	// to experiment with various settings)

	const char* lsn_str_0 = "p2ordbook://FORTS_ORDLOG_REPL;snapshot=FORTS_FUTORDERBOOK_REPL";

	// Defines Plaza-2 replication datastream listener
	// of stream named FORTS_FUTINFO_REPL
	const char* lsn_str_1 = "p2repl://FORTS_FUTINFO_REPL;scheme=|FILE|ini/fut_info.ini|CustReplScheme";

	// Defines Plaza-2 replication datastream listener
	// of stream FORTS_FUTTRADE using custom data scheme
	const char* lsn_str_2 = "p2repl://FORTS_FUTTRADE_REPL";
	const char* lsn_str_3 = "p2repl://FORTS_FUTTRADE_REPL;scheme=|FILE|ini/fut_trades.ini|CustReplScheme";

	// Defines Plaza-2 datastream listener, which provides
	// helper functions to receive order books. See documentation for
	// detailed description of this type of listener.
	const char* lsn_str_4 = "p2ordbook://FORTS_FUTTRADE_REPL;snapshot=FORTS_FUTORDERBOOK_REPL;scheme=|FILE|ini/fut_trades.ini|CustReplScheme";
	const char* lsn_str_5 = "p2ordbook://FORTS_ORDLOG_REPL;snapshot=FORTS_FUTORDERBOOK_REPL;scheme=|FILE|ini/ordLog_trades.ini|CustReplScheme";
	const char* lsn_str_6 = "p2ordbook://FORTS_FUTTRADE_REPL;snapshot=FORTS_FUTORDERBOOK_REPL";
	const char* lsn_str_7 = "p2repl://MCXCC_USERDATA_REPL;repl_params=\"login=MD9007500024;pwd=;\"";

	const char* lsn_strs[] = {lsn_str_0, lsn_str_1, lsn_str_2, lsn_str_3, lsn_str_4, lsn_str_5, lsn_str_6, lsn_str_7};
	const char* lsn_str = lsn_str_7;
	srand((unsigned)time(0)); 

#ifdef _WIN32
	SetConsoleCtrlHandler(InterruptHandler, 1);
#endif//_WIN32

	if (argc > 1)
	{
		size_t idx = atoi(argv[1]);
		if (idx < 8 )
			lsn_str = lsn_strs[idx];
		if (argc > 2)
		{
			port = atoi(argv[2]);
			if (port)
				sprintf(conn_str, "p2tcp://127.0.0.1:%d;app_name=test_repl_%d", port, rand() % 1000);
		}
		if (argc > 3)
		{
			lifenum = atoi(argv[3]);
		}
		if (argc > 4)
		{
			sprintf(conn_str, "p2tcp://%s:%d;app_name=test_repl_%d", argv[4], port, rand() % 1000);
		}
	}
	else
	{
		int i = 0;
		for (i = 0; i < 8; ++i)
		{
			printf("Lsn %d: %s\n", i , lsn_strs[i]);
		}
	}
	// Environment is initialized using provided INI-file
	CHECK_FAIL(env_open("ini=repl.ini;key=11111111"));
	// Connection is initialized using connection init string
	CHECK_FAIL(conn_new(conn_str, &conn));
	// Listener is initialized using listener ini string,
	// message callback pointer and callback data which is 0 for this example.
	CHECK_FAIL(lsn_new(conn, lsn_str, &MessageCallback, &revision, &listener));

	// Alternative callback may be supplied to test message dump function.
	//CHECK_FAIL(lsn_new(conn, lsn_str, &MessageCallbackSimple, 0, &listener));


	// Application main loop, which polls connection state,
	// handles connection and listener life cycles and
	// exits when break event happens.
	while (!bExit) {
	
		uint32_t state;

		// query connection state
		WARN_FAIL(conn_getstate(conn, &state));

		if (state == CG_STATE_ERROR)
		{
			// if connection is in ERROR state
			// then close connection
			WARN_FAIL(conn_close(conn));
		}
		else
		if (state == CG_STATE_CLOSED)
		{
			// if connection is in CLOSED state
			// then open connection
			WARN_FAIL(conn_open(conn, 0));
		}
		else
		if (state == CG_STATE_ACTIVE)
		{
			// if connection is ACTIVE then
			// process with connection messages
			
			uint32_t state;
			uint32_t result = conn_process(conn, 1, 0);
			if (result != CG_ERR_OK && result != CG_ERR_TIMEOUT) 
			{
				log_info("Warning: connection state request failed: %X", result);
			}
		
			// query listener state
			WARN_FAIL(lsn_getstate(listener, &state));
			
			switch (state)
			{
			case CG_STATE_CLOSED: /* closed */
				{
					// listener is CLOSED so open it
					char open_str[255];
					if ( lifenum > 0 )
						sprintf(open_str, "lifenum=%d", lifenum);
					else
						sprintf(open_str, "");

					WARN_FAIL(lsn_open(listener, open_str));
					/**/
				}
					break;
			case CG_STATE_ERROR: /* error */
					// listener is in ERROR state so close it
					WARN_FAIL(lsn_close(listener));
					break;
			}
		}
	}

	// break event occured
	// destroy objects

	// uninitialize listener if there is one
	if (listener != NULL) {
		lsn_close(listener);
		lsn_destroy(listener);
	}

	// destroy connection
	if (conn != NULL)
		conn_destroy(conn);


	// close environment
	CHECK_FAIL(env_close());
	return 0;
}
