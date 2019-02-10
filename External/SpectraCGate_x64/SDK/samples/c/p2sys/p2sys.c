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
	P2sys - пример авторизации роутера из cgate. ѕовтор€ет в цикле следующие действи€:
	i. ѕосылает ошибочный набор (login, pwd), в ответ получает сообщение logon failed;
	ii. ѕосле этого посылает правильный набор (login, pwd);
	iii. Ќа сообщение об успешной авторизации посылаетс€ запрос на logout;
	iv. ¬озврат к пункту 1.

	P2sys is used for authorize router via cgate. Runs the following commands in loop:
	i. Sends erroneous command set ('login', 'pwd'), receives the 'logon failed' message in reply;
	ii. Sends the correct command set ('login', 'pwd');
	iii. Sends the 'logout' request in reply on successful authorisation message;
	iv. Returns to 1.
*/

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif//_WIN32

#define CG_SHORT_NAMES
#include <cgate.h>

const int RouterLogin_msgid = 1;
const int RouterLogout_msgid = 2;

#ifdef _WIN32
static void usleep(uint64_t t)
{
	Sleep((DWORD)t / 1000);
}
#endif //_WIN32
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
unsigned int cntr = 0;

#ifdef _WIN32
BOOL WINAPI InterruptHandler(DWORD reason)
{
	log_info("----BREAK----");
	bExit = 1;
	return 1;
}
#endif//_WIN32

char* userLogin = 0;
char* correctPwd = 0;
char* userLogin2 = 0;
char* correctPwd2 = 0;
char* currentPwd = 0;
char* currentLogin = 0;

const int maxLoginLength = 50;
// publisher pointer
publisher_t* publisher = 0;

uint32_t send_request(cg_publisher_t* pub, char* login)
{
	uint32_t res = 0;
	struct cg_msg_t* msg = 0;
	struct cg_msg_data_t* msgMQ = 0;

	res = pub_msgnew(pub, CG_KEY_ID, login ? &RouterLogin_msgid : &RouterLogout_msgid, &msg);
	if (res != CG_ERR_OK)
	{
		return res; 
	}
	msgMQ = (struct cg_msg_data_t*)msg;
	if (login) 
		strncpy((char*)msgMQ->data, login, maxLoginLength);
	else
		*(int*)msgMQ->data = 1;
	res = pub_post(pub, (struct cg_msg_t *)msg, 0);
	pub_msgfree(pub, (struct cg_msg_t *)msg);
	return res;
}

uint32_t try_to_login()
{
	static char buf[255];
	CG_RESULT res = CG_ERR_INTERNAL;
	if(userLogin2 != 0 && correctPwd2 != 0)
	{
		if((cntr % 3) == 0)
		{
			currentLogin = "wrong login";
			currentPwd = "wrong pwd";
		}
		else if ((cntr % 3) == 1)
		{
			currentLogin = userLogin;
			currentPwd = correctPwd;
		}
		else
		{
			currentLogin = userLogin2;
			currentPwd = correctPwd2;
		}
	}
	else
	{
		if((cntr % 2) == 0)
		{
			currentLogin = "wrong login";
			currentPwd = "wrong pwd";
		}
		else
		{
			currentLogin = userLogin;
			currentPwd = correctPwd;
		}
	}
	sprintf(buf, "USERNAME=%s;PASSWORD=%s", currentLogin, currentPwd);
	res = send_request(publisher, buf);
	if (res == CG_ERR_MORE)
	{
		// router is in intermediate state, try to logon later
		return 0;
	}
	else if (res != CG_ERR_OK)
	{
		log_info("Client gate error: %x", res);
		return 0;
	}
	cntr++;
	usleep(1000000);
	return 1;
}

// This callback is what typically message callback looks like
// Processes messages that are important to Plaza-2 datastream lifecycle
CG_RESULT MessageCallback(cg_conn_t* conn, cg_listener_t* listener, struct cg_msg_t* msg, void* data)
{
	switch (msg->type)
	{
	case CG_MSG_DATA: 
		{
			uint32_t* data = (uint32_t*)msg->data;
			int msg_id = ((struct cg_msg_data_t*)msg)->msg_id;
			if (msg_id == 1)
			{
				log_info("Router Connected");
				send_request(publisher, 0);
				usleep(1000000);
			}
			else if (msg_id == 2)
			{ 
				log_info("Router Disconnected");
				if (try_to_login() == 0)
				{
					return CG_ERR_MORE;
				}
			}
			else if (msg_id == 3)
			{
				log_info("Connection Connected");
			}
			else if (msg_id == 4)
			{
				log_info("Connection Disconnected");
			}
			else if (msg_id == 5)
			{
				log_info("Logon failed");
				if (try_to_login() == 0)
				{
					return CG_ERR_MORE;
				}
			}
		break;
		}
	case CG_MSG_OPEN:
		// stream is opened, its scheme is available
		log_info("OPEN");

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
	default:
		// Other messages get logged but not handled
		log_info("Message 0x%X", msg->type);
	}

	// code returns 0, since there were no errors.
	// any other error code may be return and it will be logged
	return CG_ERR_OK;
}

int main(int argc, char *argv[])
{
	// connection pointer
	conn_t* conn = 0;
	// listener pointer
	listener_t* listener = 0;

	// connection init string

	// Defines connection with P2MQRouter which is located at
	// the same machine and listening on port 4001 using TCP protocol
	// Plaza-2 "application name" will be "test_p2sys"

	const char* conn_str = "p2sys://127.0.0.1:4001;app_name=test_p2sys";

	// listener init string

	// Defines Plaza-2 router state listener
	const char* lsn_str = "p2sys://;name=p2sys_lsn";

	// Defines Plaza-2 publisher which allow to connect router to Plaza-2 network
	const char* pub_str = "p2sys://;name=p2sys_pub";

	
	int i = 0;
	if (argc < 3) {
		printf("Usage: 'user login' 'user pwd' 'user login 2 (optional)' 'user pwd 2 (optional)',\n");
		return 1;
	}
	
	// Init router login, router password
	userLogin = argv[1];
	correctPwd = argv[2];
	if (argc >= 5)
	{
		userLogin2 = argv[3];
		correctPwd2 = argv[4];
	}

#ifdef _WIN32
	SetConsoleCtrlHandler(InterruptHandler, 1);
#endif//_WIN32


	// Environment is initialized using provided INI-file
	CHECK_FAIL(env_open("ini=p2sys.ini;key=11111111"));
	// Connection is initialized using connection init string
	CHECK_FAIL(conn_new(conn_str, &conn));
	// Listener is initialized using listener ini string,
	// message callback pointer and callback data which is 0 for this example.
	CHECK_FAIL(lsn_new(conn, lsn_str, &MessageCallback, 0, &listener));

	// Alternative callback may be supplied to test message dump function.
	//CHECK_FAIL(lsn_new(conn, lsn_str, &MessageCallbackSimple, 0, &listener));

	CHECK_FAIL(pub_new(conn, pub_str, &publisher));

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
			
			
			// query publisher state
			WARN_FAIL(pub_getstate(publisher, &state));

			switch (state)
			{
			case CG_STATE_CLOSED: /* closed */

				// publisher is CLOSED so open it
				WARN_FAIL(pub_open(publisher, 0));
				/**/
				break;
			case CG_STATE_ERROR: /* error */
				// publisher is in ERROR state so close it
				WARN_FAIL(pub_close(publisher));
				break;
			}
			if (state != CG_STATE_ACTIVE)
				continue;
			// query listener state
			WARN_FAIL(lsn_getstate(listener, &state));
			
			switch (state)
			{
			case CG_STATE_CLOSED: /* closed */
				// listener is CLOSED so open it
				WARN_FAIL(lsn_open(listener, 0));
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

	// uninitialize publisher if there is one
	if (publisher != NULL) {
		pub_close(publisher);
		pub_destroy(publisher);
	}

	// destroy connection
	if (conn != NULL)
		conn_destroy(conn);


	// close environment
	CHECK_FAIL(env_close());
	return 0;
}
