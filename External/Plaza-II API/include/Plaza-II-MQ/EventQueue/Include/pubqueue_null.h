#ifndef _PUBQUEUE_NULL_
#define _PUBQUEUE_NULL_

#include "../../../Plaza-II/P2Utils/Include/P2Event.hpp"

#include "evqueue.h" 
namespace transport { 
	class pubqueue_null: public pubqueue 
	{
		public:
			pubqueue_null() 
			{ 
#if defined(WIN32)
				THROW_IFERR(P2EventCreate(&(pubqueue::event), false, false)); 
#else
				pubqueue::event = -1;
#endif

			} 

			~pubqueue_null() 
			{ 
#if defined(WIN32)
				P2EventClose(pubqueue::event); 
#endif
			} 

			void push(const char*) {} 
			void flush() {} 
	}; 
}  // namespace transport
#endif //_PUBQUEUE_NULL_

