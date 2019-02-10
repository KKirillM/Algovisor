#include "../../../Plaza-II-eRepl/P2eReplServer/Include/P2eReplServer.hpp" 
#include "../../../Plaza-II-MQ/P2MQ/Include/P2MQ.hpp"

#include "evqueue.h"

#include <memory>


namespace transport { 

	class pubqueue_erepl: public pubqueue
	{
		AutoEReplServerHandle 	 _server;
		P2EREPL_STORAGE_HANDLE 	 _storage; 
		bool					 _transaction; 
		std::auto_ptr<pubqueue>  _multicast;
		P2AutoMQConnHandle		 _mq;				// соединентие с роутером
		DWORD 					 _tick; 			
		class ACLs;
		std::auto_ptr<ACLs>		 _acls;				// клиентская репликация аклов
		public: 
		pubqueue_erepl(P2EREPL_STORAGE_HANDLE storage, const char* section);
		~pubqueue_erepl(); 
		void push(const char* event); 
		void flush(); 

		P2EREPL_STORAGE_HANDLE storage() const 
		{ return _storage; }
	}; 

} // namespace transport















