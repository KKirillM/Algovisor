#ifndef _TRANSPORT_SESSION_H_
#define _TRANSPORT_SESSION_H_ 

namespace transport { 
	typedef unsigned int session_type;
#if defined(_MSC_VER)
	typedef unsigned __int64 seq_type;
#else 
	typedef unsigned long long seq_type; 
#endif 

// waitable descriptor for WaitForMultipleObjects or for polling
#if defined(_WIN32)
	typedef void* FD; // HANDLE is void*
#define INVALID_FD NULL
#else
	typedef int FD; 
#define INVALID_FD (-1)
#endif
} // namespace transport 

#endif //_TRANSPORT_SESSION_H_

