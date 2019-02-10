#ifndef _UDP_PUBLISHER_H_
#define _UDP_PUBLISHER_H_

#include "evqueue.h"
#include "url.h"
#include "internal/UdpSocket.h"
#include "internal/Timer.h"
#include "internal/PubCache.hpp"

namespace transport { 

	class pubqueue_mcast : public pubqueue
	{
		DISABLE_CLASS_COPY(pubqueue_mcast); 
		UdpPubSocket 				_sock;
		PubCache 					_cache; 
		Rate						_rate;
		size_t 		 const			_max_flush;
		uint32_t 	 const			_heartbeat_interval;
		uint32_t 	 const	 	 	_recv_requests_interval;
		session_type const 	 		_session;	
		seq_type		 			_seqnum;  
		InetAddr 	 const 			_multicast_group;
		Timeout	 	 				_heartbeat_time; 
		size_t  					_unflushed_bytes;      
		size_t  					_unflushed_events;
		size_t  					_pushed_bytes;
		size_t  					_pushed_events;
		Ring<Hdr>					_reqs;
		McsTimeout	 	 			_recv_requests;
		void   						post_header(seq_type seq, uint16_t count); 
		size_t 						max_repost(size_t max_events);
		size_t   					repost_messages(size_t max_events); 
		void   						post_cached(seq_type seq, size_t count); 
		void  						post_message(seq_type seq, size_t count, uint16_t *events, size_t* bytes);
		size_t  					post_unflushed_message();

		public: 

		~pubqueue_mcast(); 

		// поддерживаемые параметры: 
		// cache_size 		 : размер исторического буфера (в мегабайтах)
		// ttl 		  		 : значение поля "time to live"
		// heartbeat_timeout : время в миллисекундах 

		pubqueue_mcast(const url& opt, session_type session, seq_type seq);   
		void push(const char* event);
		void flush(); 
		size_t size() const { 		// размер в байтах добавленных, но не отправленных данных 
			return _pushed_bytes; 
		} 			
		size_t max_size() const { 	//  максимальный размер данных, который можно добавить без отправки
			return _cache.max_size(); 
		}  
	};

} // namespace transport 



#endif // _UDP_PUBLISHER_H_



