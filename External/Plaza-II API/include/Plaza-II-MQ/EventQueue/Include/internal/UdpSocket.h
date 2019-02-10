#ifndef _UDP_SOCKET_H_
#define _UDP_SOCKET_H_

#if defined(_WIN32)
#   include "../../../../Plaza-II/P2Common/Include/P2Windows.h"
#	include <winsock2.h>
#	include <Ws2tcpip.h> 
#	undef min
#else
#	include <netinet/ip.h>
#	include <arpa/inet.h>
#endif

#include <algorithm>
#include <cstdint>
#include <cstring>  

#include "../../Include/transport_session.h"

namespace transport  { 

	// constnt for socket option means that we don't modify it
	const uint32_t SO_SYSTEM_VALUE = (~0u);

#pragma pack(push, 1)
	struct Hdr { 
		session_type session;
		seq_type	 seqnum;
		uint16_t	 count; 
		Hdr(): session(0), seqnum(0), count(0) {}
		Hdr(session_type session_, seq_type seqnum_, uint16_t count_): session(session_), seqnum(seqnum_), count(count_) {} 
		static const uint16_t END_OF_SESSION = 0xFFFF; 
	};
#pragma pack(pop)

	const size_t MAX_PAYLOAD = (1472 - sizeof(Hdr));  // 1472 == <(Ethernet frame> - <IP header> - <UDP Header>)

#if defined(_WIN32)
		typedef WSABUF IOVEC;
#else
		typedef struct iovec IOVEC;
#endif

	// Message class encapsulates data buffers for sending/receiving via sock.
	class Message	{
		Message(const Message&); 

		mutable IOVEC _buf[3];
		size_t  _count;
		size_t  _bytes; 

		void append(const void* data, size_t size) {
#if defined(_WIN32)
			_buf[_count].buf = const_cast<char*>(static_cast<const char*>(data));  // TODO: comment about const cast
			_buf[_count].len = (uint32_t)size; 
#else
			_buf[_count].iov_base = const_cast<void*>(data);  
			_buf[_count].iov_len = size; 
#endif
			++_count;
			_bytes += size;
		} 
		public: 
		Message(): _count(0), _bytes(0) {} 

		Message(const void* data, size_t size): _count(0), _bytes(0) {
			append(data, size);
		}

		Message(const void* data_1, size_t size_1, const void* data_2, size_t size_2): _count(0), _bytes(0) {
			append(data_1, size_1);
			append(data_2, size_2);
		} 

		Message(const void* data_1, size_t size_1, const void* data_2, size_t size_2, const void* data_3, size_t size_3): _count(0), _bytes(0) {
			append(data_1, size_1);
			append(data_2, size_2);
			append(data_3, size_3);
		} 

		IOVEC* bufs() const { 
			return &_buf[0]; 
		}

		uint32_t bytes() const { 
			return static_cast<uint32_t>(_bytes); 
		} 

		uint32_t bufs_count() const { 
			return static_cast<uint32_t>(_count);
		} 

	};

	struct InetAddr: public sockaddr_in {
		InetAddr() {}
		// @port (network byte order)
		InetAddr(uint32_t ip, uint16_t port) { 
			sin_family = AF_INET;
			sin_addr.s_addr = ip;
			sin_port = port; 
			memset(sin_zero, 0x0, sizeof(sin_zero));
		} 
		uint32_t ip() const { return sin_addr.s_addr; }
		uint16_t port() const { return sin_port; }
	};

	class UdpSocket { 
		protected: 
#if defined(_WIN32)  
		SOCKET _socket; 
#endif 
		// @snd/rcv_buf socket buffer size
		UdpSocket(uint32_t snd_buf, uint32_t rcv_buf); 
		public: 

		// descriptor wait for socket readiness (poll/WaitForSingleObject).
		FD fd;  

		int  recvfrom(const Message& buf, sockaddr_in* from);
		void sendto(const Message& buf, const sockaddr_in& to); 

		~UdpSocket(); 
	};

	class UdpSubSocket : public UdpSocket { 
		public: 
		// @port			multicast group port
		// @ip				multicast group ip (host byte order)
		// @snd/rcv_buf 	socket buffer size
		UdpSubSocket(uint32_t snd_buf, uint32_t rcv_buf, uint32_t ip, uint32_t port);
	}; 

	class UdpPubSocket : public UdpSocket { 
		public: 
		// @iface 		interface ip (network byte order)
		// @ttl    		time-to-live socket option
		// @snd/rcv_buf socket buffer size
		UdpPubSocket(uint32_t snd_buf, uint32_t rcv_buf, uint32_t ttl, uint32_t iface); 
	};

}  

#endif // _UDP_SOCKET_H_




