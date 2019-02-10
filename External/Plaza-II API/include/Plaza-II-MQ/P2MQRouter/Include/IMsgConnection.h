// interface IMsgConnection
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMSGCONNECTION_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_)
#define AFX_IMSGCONNECTION_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II/P2Common/Include/P2Types.h"
#include "../../../Plaza-II/P2Utils/Include/IRc.h"
#include "../../../Plaza-II/P2Utils/Include/ICs.h"
#include "ConnLUID.h"
#include "IBytesWLen.h"
#include "LLMsg.h"

//////////////////////////////////////////////////////////////////////
// IMsgConnection

struct ILoginCallback;

struct IMsgConnection : public IRc
{
    typedef conn::LUID LUID;

    enum TIsLocal
    {
        LOCAL_YES,
        LOCAL_NO,
        LOCAL_UNKNOWN
    };

    virtual LUID GetLUID() const = 0;
	virtual UINT64 GetConnectionKey() /*const*/ = 0;  //TODO: �������� const ������ UINT64
	virtual bool IsOutgoing() const = 0;
	virtual const char* GetHost(char *host,size_t sz) const = 0;
	virtual UINT GetPort() const = 0;
    virtual bool IsDefUpLink() const = 0;

// <- Transferred to handler(s) after an establishment of connection
//    virtual TIsLocal IsLocal() = 0;

    // - ����������� ����� ����� ���������� ��������������. TODO: check this  comment
    // - �� ���������� - ������ ������ ("")
    virtual void GetName(PSTR name, UINT32 size) const = 0;


    virtual P2ERR Connect(ICs* pICsCaller) = 0;
	virtual void Disconnect(bool setDead, bool graceful, ICs* pICsCaller) = 0;
	/*
		�� ���� �� �������������! 
		PostAsync - ������, ������� ������������ ������ ����� ����������� ������� LLMsg. ��� �������-��������� ��� ������ �� ������ ����� ����������� CS, 
					�� ����������� latency �� ������� ������ � �������� ����� ����. ��������������� �� ������ �� ������������ ������
		PostSync - ������, ������� ������ ����� ��������� ���������� ������� � ������, ������ ����� ��� ������. ������ ���������� ����������� latency, 
					�� ����� ��������� capacity. ��������������� �� ������ �� ����������.
	*/
	virtual P2ERR PostSync(IBytesWLenPriority* pIBytes,ICs* pICsCaller) = 0;
	virtual P2ERR PostSync(UINT32 system_type, const void* pBody, size_t len, ICs* pICsCaller, UINT32 flags) = 0;
    virtual P2ERR PostAsync(UINT32 system_type, const void* pBody, size_t len, UINT32 flags) = 0;
	virtual P2ERR PostAsync(IBytesWLenPriority* pIBytes) = 0;

    // ����������� ����������
    // (���� ����� �� ��������� CS, � ������ �������� ����� Disconnect()-a
    //  � ������ ����� �� ����)
	virtual void DisconnectPost(bool setDead, bool graceful) = 0;

    // ���������� ����� ������ ���������� �������� ���������� ������.
    // ��������! �������� �� ��, ��� ����� �� �������� ��������� ICs*, ������ ����������
    // ������������ ����������� ������ - ����� �� ������ ���������� �� ��� ������ CS!
    virtual void OnLoginFrom_(ILoginCallback* pILoginCB, IBytesWLen* pIAuthData) = 0;

    // �������� ���������� ���������� � ��������� ������.
    // ��������! ����� ������ ������� ��������� ���������� ����� ����������!
    virtual BOOL IsValid() const = 0;

    CHECK_FOR_DELETE__(IMsgConnection)
};

#endif // !defined(AFX_IMSGCONNECTION_H__EEEED7F7_F03D_4A39_81EF_16B404FD8A98__INCLUDED_)
