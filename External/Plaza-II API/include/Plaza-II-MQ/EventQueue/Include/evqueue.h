#ifndef _EVQUEUE_H_
#define _EVQUEUE_H_

#include <cstdint>
#include <cstddef>
#include <system_error>


#include "transport_session.h"
#include "p2err.h"

namespace transport { 

#pragma pack(push, 4)
	struct Event
	{
		uint64_t  	rev;
		uint32_t  	type;
		uint32_t	data_size;
	}; 
#pragma pack(pop)

	inline size_t event_size(const void* ev)
	{
		return sizeof(Event) + static_cast<const Event*>(ev)->data_size; 
	} 

	struct pubqueue
	{ 
		virtual void push(const char* event) = 0;   
		virtual void flush() = 0; 
		virtual ~pubqueue() {} 
		FD event; 
		pubqueue(): event(INVALID_FD) {} 
	}; 

	struct subqueue
	{ 
		FD event; 	// Программа должна прочитать все данные перед тем как снова ждать на этом эвенте.(edge trigger)
		subqueue(): event(INVALID_FD) {}

		virtual void get(const char** data, size_t* size) = 0;
		virtual void pop() = 0;
		virtual ~subqueue() {} 
	}; 
	
} //namespace transport

	inline P2ERR pubqueue_push(transport::pubqueue* pub, const char* event)
	{
	    if (!pub) return P2ERR_COMMON_WRONG_ARGUMENTS; 
		try 
		{ 
			pub->push(event); 
		} 
		catch (const std::system_error& ex) 
		{ 
			p2err::trace_exception("transport", ex);
			return p2err::code(ex);
		} 
		catch (const std::bad_alloc&)		
		{ 
			return P2ERR_COMMON_NO_MEMORY;
		}
		return P2ERR_OK;
	}

	inline P2ERR pubqueue_flush(transport::pubqueue* pub)
	{
	    if (!pub) return P2ERR_COMMON_WRONG_ARGUMENTS; 
		try 
		{ 
			pub->flush(); 
		} 
		catch (const std::system_error& ex) 
		{ 
			p2err::trace_exception("transport", ex);
			return p2err::code(ex);
		} 
		catch (const std::bad_alloc&)		
		{ 
			return P2ERR_COMMON_NO_MEMORY;
		}
		return P2ERR_OK; 
	}

	// [data, data + size) должно содержать целое число эвентов
	inline P2ERR pubqueue_post(transport::pubqueue* pub, const char* data, size_t size)
	{
		while (size != 0)
		{
			RETURN_P2ERR(pubqueue_push(pub, data));
			size -= transport::event_size(data);
			data += transport::event_size(data);
		} 
		return pubqueue_flush(pub);
	} 

	inline P2ERR subqueue_get(transport::subqueue* sub, const char** data, size_t* size)
	{
		if (!sub || !data || !size) return P2ERR_COMMON_WRONG_ARGUMENTS;
		try
		{
			sub->get(data, size);
			return P2ERR_OK; 
		}
		catch (const std::system_error& ex) 
		{
			p2err::trace_exception("transport", ex);
			return p2err::code(ex);
		}
	}

	inline P2ERR subqueue_pop(transport::subqueue* sub)
	{
		if (!sub) return P2ERR_COMMON_WRONG_ARGUMENTS;
		sub->pop();
		return P2ERR_OK;
	}

	inline P2ERR subqueue_recv(transport::subqueue* sub, char* data, size_t* size)
	{
		if (!sub || !data || !size) return P2ERR_COMMON_WRONG_ARGUMENTS;
		const char* buf;
		size_t real_size;
		RETURN_P2ERR(subqueue_get(sub, &buf, &real_size));
		if (real_size > *size) 
		{
			*size = real_size;
			return P2ERR_COMMON_REAL_SIZE_RETURNED;
		}
		*size = real_size;
		memcpy(data, buf, real_size);
		subqueue_pop(sub);
		return P2ERR_OK;
	} 




#endif //_EVQUEUE_H_


