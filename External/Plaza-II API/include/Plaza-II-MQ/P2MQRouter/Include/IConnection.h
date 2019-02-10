// interface IConnection
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICONNECTION_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_)
#define AFX_ICONNECTION_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II/P2Utils/Include/IRc.h"
#include "../../../Plaza-II/P2Utils/Include/ICs.h"
#include "ConnLUID.h"
#include "IBytesProtPtrList.h"

//////////////////////////////////////////////////////////////////////
// IConnection

struct IConnectionHostsInfo : public IRc
{
	virtual const char* GetHost() const = 0;
	virtual UINT32		GetPort() const = 0;
};

struct IConnection : public IRc
{
    typedef conn::LUID LUID;

    virtual LUID GetLUID() const = 0;
	virtual bool IsOutgoing() const = 0;

	// Valid only for incoming connections, for outgoing - always false
    virtual bool FromLocalHost() const = 0;

    virtual P2ERR Connect(ICs* pICsCaller) = 0;
	virtual void Disconnect(bool setDead, bool graceful, ICs* pICsCaller) = 0;

    // Send copy of data
	virtual P2ERR PostSync(const void* pData, size_t len, BYTE priority, ICs* pICsCaller) = 0;
	// Send copy of data
	/* метод исключен из интерфейса по причине невостребованности и неэффективности
    virtual P2ERR PostCopySync(const IBytesWLenPriority* pIBytes, ICs* pICsCaller) = 0;
	*/
    // Send this instance (don't use pIBytes after call, only Release1()!)
    virtual P2ERR PostThisSync(IBytesWLenPriority* pIBytes, ICs* pICsCaller) = 0;

    // Send copy of data
    virtual P2ERR PostAsync(const void* pData, size_t len, BYTE priority) = 0;
    // Send copy of data
	virtual P2ERR PostCopyAsync(const IBytesWLenPriority* pIBytes) = 0;
    // Send this instance (don't use pIBytes after call, only Release1()!)
	virtual P2ERR PostThisAsync(IBytesWLenPriority* pIBytes) = 0;

    virtual P2ERR PostThisSync(IBytesPriorProtPtrList* pIDL, ICs* pICsCaller) = 0;

    // Indirect call (through IOCP)
    virtual void SuspendRecv() = 0;
    virtual void ResumeRecv() = 0;
    // Direct call
    virtual void SuspendRecv(ICs* pICsCaller) = 0;
	virtual IConnectionHostsInfo* GetConnectionHostsInfo()  = 0;

    CHECK_FOR_DELETE__(IConnection)
};

//инициализация из ини файла
//ограничение размера памяти, который может занимать очередь отправки
#define INI_LOCAL_CONN_MAX_BYTES			"LocalConn_MaxQueueBytes"
#define INI_REMOTE_CONN_MAX_BYTES			"RemoteConn_MaxQueueBytes"
#define INI_SOCKET_SNDBUFSZ					"sock_send_bufsize"
#define INI_SOCKET_RCVBUFSZ					"sock_recv_bufsize"
#define INI_DEFAULT_LOCAL_CONN_MAX_BYTES	100*1024*1024	//100мб 
#define INI_DEFAULT_SOCKET_RCVBUFSZ			1024
#define INI_DEFAULT_SOCKET_SNDBUFSZ			1024
#define INI_IP_SOURCE						"IpSource"     // через какой интерфейс устанавливаются исх. соединения
#define INI_QUEUE_STAT_TIMEOUT              "queue_stat_timeout"



#endif // !defined(AFX_ICONNECTION_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_)
