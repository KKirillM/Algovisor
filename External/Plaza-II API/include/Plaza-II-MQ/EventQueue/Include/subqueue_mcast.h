#ifndef _UDPLISTENER_H_
#define _UDPLISTENER_H_

#include "evqueue.h"
#include "url.h"
#include "internal/UdpSocket.h"
#include "internal/Timer.h"
#include "internal/SubCache.hpp"

namespace transport { 

	class subqueue_mcast : public subqueue
	{
#ifdef WITH_GTEST
		friend struct test_functions;
#endif //WITH_GTEST
		DISABLE_CLASS_COPY(subqueue_mcast);
		SubCache     	_cache;
		UdpSubSocket 	_socket;
		InetAddr     	_server;
		Timeout  	 	_retransmit_time;
		Timeout  	 	_session_time;
		seq_type	 	_retransmit_seq;
		char 		 	_buf[MAX_PAYLOAD]; 
		size_t		 	_buf_size; 
		uint32_t const 	_session_timeout; 
		void 			retransmit_request(seq_type seq);
		void 			repeat_retransmit_request();
		size_t 			recv(char* data, size_t size);    
		protected: 
		void 			heartbeat_request();
		size_t 			recv_packet(Hdr* hdr, char* data, size_t size); 
		session_type 	_session;
		seq_type     	_seqnum; 
		public: 
		subqueue_mcast(const url& u, session_type session, seq_type seq);
		~subqueue_mcast() {} 

		void get(const char** data, size_t* size);
		void pop(); 
		//
		// поддерживаемые параметры url'a.
		//
		static const char* const SOCKET_BUFFER; 	// "subsocket_buffer"   : размер сокетного буфера в мегабайтах
		static const char* const SESSION_TIMEOUT; 	// "session_timeout" 	: допустимое врем€ (в миллисекундах) отсутстви€ пакетов от сервера.
	}; 

	//
	// ѕолучение мультикаста в режиме "подключени€" к текущей сессии сервера.
	//
	class subqueue_mcast_online : public subqueue_mcast
	{
		bool _has_session; 
		bool _has_seqnum;
		Timeout _request_repeat;
		static const size_t _repeat_timeout;
		public: 
		subqueue_mcast_online(const url& u);
		~subqueue_mcast_online() {} 
		void get(const char** data, size_t* size);
		void pop(); 
	};


} //namespace transport

#endif //_UDPLISTENER_H_

