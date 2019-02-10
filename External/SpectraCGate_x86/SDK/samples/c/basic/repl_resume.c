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
	Repl_resume - пример идентичиен repl. Отличие заключается в том, что после разрыва соединения repl_resume продолжает реплику
	с последнего сообщения TN_COMMIT. Входных аргументов нет.

	Repl_resume is an example similar to the 'repl' one. The difference is that when disconnected, the replica starts from the last 'TN_COMMIT'
	message. The example does not have input parameters.
*/

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif//_WIN32

#define CG_SHORT_NAMES
#include <cgate.h>


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


static const char* REPLSTATE_FNAME = "replstate.dat";


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

CG_RESULT MessageCallback(cg_conn_t* conn, cg_listener_t* listener, struct cg_msg_t* msg, void* data)
{
	switch (msg->type)
	{
	case CG_MSG_STREAM_DATA: 
		{
		// data recevied. print data properties (message name, index etc)

		struct cg_msg_streamdata_t* replmsg = (struct cg_msg_streamdata_t* )msg;
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
		log_info("ONLINE\n");
		break;
	case CG_MSG_TN_BEGIN:
		// next data block is about to be received
		log_info("TN BEGIN\n");
		break;
	case CG_MSG_TN_COMMIT:
		// data block finished. data is consistent now
		log_info("TN COMMIT\n");
		break;
	case CG_MSG_OPEN:
		// stream is opened, its scheme is available
		log_info("OPEN\n");

		{
			// this is how scheme is handled
			struct cg_scheme_desc_t* schemedesc = 0;
			WARN_FAIL(cg_lsn_getscheme(listener, &schemedesc));
			if (schemedesc != 0)
			{
				struct cg_message_desc_t* msgdesc = schemedesc->messages;
				while (msgdesc)
				{
					struct cg_field_desc_t* fielddesc = msgdesc->fields;
					log_info("Message %s, block size = %llu\n", msgdesc->name, (unsigned long long)msgdesc->size);

					while (fielddesc)
					{
						log_info("\tField %s = %s [size=%llu, offset=%llu]\n", fielddesc->name, fielddesc->type, (unsigned long long)fielddesc->size, (unsigned long long)fielddesc->offset);
						fielddesc = fielddesc->next;
					}
					
					msgdesc = msgdesc->next;
				}
			}
		}
		break;
	case CG_MSG_CLOSE:
		// Stream is closed, no more data will be received
		log_info("CLOSE\n");
		break;
	case CG_MSG_P2REPL_LIFENUM:
		// Stream data scheme's life number was changed.
		// complete data snapshot re-replication will occur.
		log_info("Life number changed to: %d\n", *((uint32_t*)msg->data));
		break;
	case CG_MSG_P2REPL_REPLSTATE:
		{
			// Datastream state is recevied to be used for resume later.
			FILE* state_file;
			log_info("Replica state message received\n");

			// store state in file
			state_file = fopen(REPLSTATE_FNAME, "wb");
			if (state_file)
			{
				fwrite(msg->data, 1, msg->data_size, state_file);
				fclose(state_file);
				log_info("Replica state stored to %s\n", REPLSTATE_FNAME);
			} else
			{
				log_error("FAILED to store replica state to %s\n", REPLSTATE_FNAME);
			}
			

			break;
		}
	case CG_MSG_P2REPL_CLEARDELETED:
		{
			struct cg_data_cleardeleted_t * msg_clear_deleted = (struct cg_data_cleardeleted_t *)msg->data;
			log_info("Clear deleted table_index: %d, table_rev: %lld.", msg_clear_deleted->table_idx, (long long)msg_clear_deleted->table_rev);
			if ( msg_clear_deleted->table_rev == CG_MAX_REVISON )
			{
				log_info("table_index: %d, rev start from 1.");
			}
		}
		break;
	default:
		// Other messages get logged but not handled
		log_info("Message 0x%X", msg->type);
	}

	// code returns 0, since there were no errors.
	// any other error code may be return and it will be logged
	return CG_ERR_OK;
}

int main()
{
	// connection pointer
	conn_t* conn = 0;
	// listener pointer
	listener_t* listener = 0;

	// connection init string
	const char* conn_str = "p2tcp://127.0.0.1:4001;app_name=test_repl_resume";

	// listener init string
	const char* lsn_str = "p2repl://FORTS_FUTTRADE_REPL";
	// listener open string buffer
	char* lsn_openstr = NULL;

	// listener open string template
	const char lsn_openstr_template[] = "replstate=%s";

	// replstate file variable
	FILE* replstate_file = NULL;

#ifdef _WIN32
	SetConsoleCtrlHandler(InterruptHandler, 1);
#endif//_WIN32

	if ((replstate_file = fopen(REPLSTATE_FNAME, "rb")))
	{
		char* replstate = NULL;
		long fsize;

		fseek(replstate_file, 0, SEEK_END);
		fsize = ftell(replstate_file);
		if (fsize < 0)
		{
			fclose(replstate_file);
			fprintf(stderr, "Failed to read file state size\n");
			return 3;
		}
		fseek(replstate_file, 0, SEEK_SET);
		replstate = (char*)malloc(fsize+1);
		if (fread(replstate, 1, fsize, replstate_file) != fsize)
		{
			free(replstate);
			fclose(replstate_file);
			fprintf(stderr, "Failed to read previous state from file\n");
			return 3;
		}
		fclose(replstate_file);
		replstate[fsize] = 0;
		lsn_openstr = (char*)malloc(sizeof(lsn_openstr_template) + fsize + 1);
		sprintf(lsn_openstr, lsn_openstr_template, replstate);
		free(replstate);
	}
	
	// Environment is initialized using provided INI-file
	CHECK_FAIL(env_open("ini=repl_resume.ini;key=11111111"));
	// Connection is initialized using connection init string
	CHECK_FAIL(conn_new(conn_str, &conn));
	// Listener is initialized using listener ini string
	CHECK_FAIL(lsn_new(conn, lsn_str, &MessageCallback, 0, &listener));

	// Application main loop
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
				log_error("Warning: connection state request failed: %X", result);
			}
		
			// query listener state
			WARN_FAIL(lsn_getstate(listener, &state));
			
			switch (state)
			{
			case CG_STATE_CLOSED: /* closed */

					// listener is CLOSED so open it
				WARN_FAIL(lsn_open(listener, lsn_openstr));
					/**/
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
