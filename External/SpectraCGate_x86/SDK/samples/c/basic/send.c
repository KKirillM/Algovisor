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
	Send - выставл€ет за€вку на FORTS. ¬ыводит в лог ответ торговой системы. ¬ходных аргументов нет.

	Send adds order to FORTS. Saves reply from the trading system into log file. The example does not have input parameters.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif//_WIN32
#include <assert.h>

#define CG_SHORT_NAMES
#include <cgate.h>

#include "forts_messages.h"

// helper macro to call CGate function
// and exit if there was error

#define CHECK_FAIL(f, msg) \
	{ \
	uint32_t cf_res = f; \
	if (cf_res != CG_ERR_OK) \
		{ \
		log_error("Client gate error (%s): %x\n", msg, cf_res); \
		exit(1); \
		} \
	} 

// helper macro to call CGate function
// and print warning if there was error

#define WARN_FAIL(f, msg) \
	{ \
		uint32_t wf_res = f; \
		if (wf_res != CG_ERR_OK) \
			log_error("Error (%s): %x\n", msg, wf_res); \
	} 

#define BREAK_FAIL(f, msg) \
	{ \
	uint32_t res = f; \
	if (res != CG_ERR_OK) \
		{ \
		log_error("Client gate error (%s): %x\n", msg, res); \
		continue; \
		} \
	} 

// helper variable used as a notification flag to exit
static volatile int bExit = 0;

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


const char* cfg_cli_conn = {"p2tcp://127.0.0.1:4001;app_name=test_send"};

const char* cfg_cli_publisher = "p2mq://FORTS_SRV;category=FORTS_MSG;name=srvlink;timeout=5000;scheme=|FILE|forts_messages.ini|message";

const char* cfg_cli_listener = "p2mqreply://;ref=srvlink";

cg_conn_t* cli_conn = 0;
cg_listener_t *cli_listener = 0;
cg_publisher_t *cli_publisher = 0;

CG_RESULT ClientMessageCallback(cg_conn_t* conn, cg_listener_t* listener, struct cg_msg_t* msg, void* data)
{
	switch (msg->type)
	{
	case CG_MSG_DATA: 
		{
		uint32_t* data = (uint32_t*)msg->data;
		log_info("Client received reply [id:%d, data: %d, user-id: %d, name: %s]\n", 
			((struct cg_msg_data_t*)msg)->msg_id, 
			*((uint32_t*)msg->data), 
			((struct cg_msg_data_t*)msg)->user_id,
			((struct cg_msg_data_t*)msg)->msg_name);

		{
			struct scheme_desc_t* scheme;
			size_t bufSize;
			
			if (cg_lsn_getscheme(listener, &scheme) != CG_ERR_OK)
				scheme = 0;
				
			if (cg_msg_dump(msg, scheme, 0, &bufSize) == CG_ERR_BUFFERTOOSMALL)
			{
				char* buffer = (char*)malloc(bufSize+1);
				
				bufSize++;
				if (cg_msg_dump(msg, scheme, buffer, &bufSize) == CG_ERR_OK)
					log_info("client dump: %s\n", buffer);
				free(buffer);
				
			}
		}
		break;
		}
	case CG_MSG_P2MQ_TIMEOUT: 
		{
		log_info("Client reply TIMEOUT\n");
		break;
		}
	default:
		log_info("Message 0x%X\n", msg->type);
	}
	return CG_ERR_OK;
}

CG_RESULT send_request(cg_publisher_t* pub, uint32_t counter)
{
	struct FutAddOrder* ord ;
	struct cg_msg_data_t* msg;
	struct cg_msg_data_t *hb;

	CG_RESULT res = CG_ERR_INTERNAL;
	res = pub_msgnew(pub, CG_KEY_NAME, "FutAddOrder", (struct cg_msg_t**)&msg);
	WARN_FAIL(res, "msgnew");
	if (res != CG_ERR_OK)
	{
		return res;
	}

	memcpy(msg->data, &counter, sizeof(counter));
	msg->user_id = counter;
	
	ord = (struct FutAddOrder*)msg->data;
	memset(ord, 0, sizeof(struct FutAddOrder));
	strcpy(ord->broker_code, "7500");
	strcpy(ord->isin, "CHF-6.13");
	strcpy(ord->client_code, "000");
	ord->type = 1;
	ord->dir = 2;
	ord->amount = 5;
	strcpy(ord->price, "149947");
	ord->ext_id = 1;
	ord->du = 0;
	ord->dont_check_money = 1;
		
	WARN_FAIL(pub_post(pub, (struct cg_msg_t *)msg, CG_PUB_NEEDREPLY), "pub_post");
	WARN_FAIL(pub_msgfree(pub, (struct cg_msg_t *)msg), "pub_msgfree");

	res = pub_msgnew(pub, CG_KEY_NAME, "Heartbeat", (struct cg_msg_t**)&hb);
	WARN_FAIL(res, "msgnew");
	if (res != CG_ERR_OK)
	{
		return res;
	}
	WARN_FAIL(pub_post(pub, (struct cg_msg_t *)hb, 0), "pub_post");
	WARN_FAIL(pub_msgfree(pub, (struct cg_msg_t *)hb), "pub_msgfree");

	return CG_ERR_OK;
}

int main()
{
	int counter = 0;
#ifdef _WIN32
	SetConsoleCtrlHandler(InterruptHandler, 1);
#endif//_WIN32

	CHECK_FAIL(env_open("ini=send02.ini;key=11111111"), "env_open");

	CHECK_FAIL(conn_new(cfg_cli_conn, &cli_conn), "conn_new");
	CHECK_FAIL(pub_new(cli_conn, cfg_cli_publisher, &cli_publisher), "pub_new");
	CHECK_FAIL(lsn_new(cli_conn, cfg_cli_listener, &ClientMessageCallback, 0, &cli_listener), "lsn_new");

	while (!bExit)
	{
		uint32_t state;

		// query connection state
		WARN_FAIL(conn_getstate(cli_conn, &state), "conn_getstate");

		if (state == CG_STATE_ERROR)
		{
			// if connection is in ERROR state
			// then close connection
			WARN_FAIL(conn_close(cli_conn), "conn_close");
		}
		else if (state == CG_STATE_CLOSED)
		{
			// if connection is in CLOSED state
			// then open connection
			WARN_FAIL(conn_open(cli_conn, 0), "conn_open");
		}
		else if (state == CG_STATE_ACTIVE)
		{
			// if connection is ACTIVE then
			// process with connection messages

			uint32_t result = conn_process(cli_conn, 1, 0);
			if (result != CG_ERR_OK && result != CG_ERR_TIMEOUT) 
			{
				log_info("Warning: connection state request failed: %X", result);
			}

			// query publisher state
			WARN_FAIL(pub_getstate(cli_publisher, &state), "pub_getstate");

			switch (state)
			{
			case CG_STATE_CLOSED: /* closed */

				// publisher is CLOSED so open it
				WARN_FAIL(pub_open(cli_publisher, ""), "pub_open");
				/**/
				break;
			case CG_STATE_ERROR: /* error */
				// publisher is in ERROR state so close it
				WARN_FAIL(pub_close(cli_publisher), "pub_close");
				break;
			}
			if (state != CG_STATE_ACTIVE)
				continue;

			// query listener state
			WARN_FAIL(lsn_getstate(cli_listener, &state), "lsn_getstate");

			switch (state)
			{
			case CG_STATE_CLOSED: /* closed */

				// listener is CLOSED so open it
				WARN_FAIL(lsn_open(cli_listener, ""), "lsn_open");
				/**/
				break;
			case CG_STATE_ERROR: /* error */
				// listener is in ERROR state so close it
				WARN_FAIL(lsn_close(cli_listener), "lsn_close");
				break;
			}
			if (state != CG_STATE_ACTIVE)
				continue;

			log_info("Sending message to server\n");
			if(send_request(cli_publisher, counter++) != CG_ERR_OK)
			{
				break;
			}
			Sleep(1000);
		}
	}

	CHECK_FAIL(conn_close(cli_conn), "conn_close");
	CHECK_FAIL(lsn_destroy(cli_listener), "lsn_destroy");
	CHECK_FAIL(pub_destroy(cli_publisher), "pub_destroy");
	CHECK_FAIL(conn_destroy(cli_conn), "conn_destroy");

	CHECK_FAIL(env_close(), "env_close");
	return 0;
}
