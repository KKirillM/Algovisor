
#ifndef P2LRPCQ_H_HEADER_
#define P2LRPCQ_H_HEADER_

#include "../../P2Common/Include/P2Types.h"

#ifndef __MACH__

#ifdef P2SYS_EXPORTS
#define P2LRPCQ_API P2_API_EXPORTS
#else
#define P2LRPCQ_API P2_API_IMPORTS
#endif

#include "../../../Plaza-II/P2Common/Include/P2Windows.h" 
#include "../../../Plaza-II/P2Common/Include/P2Errors.h"

#if defined(__linux__)
#include <semaphore.h>
#endif

struct P2LRPCQChannel;
struct P2LRPCQServer;

typedef P2LRPCQChannel* P2LRPCQ_HANDLE;
typedef P2LRPCQServer*	P2LRPCQ_SERVER_HANDLE;

/* ������ ��������, ������������ � ������� P2LRPCQConnect */
#define P2LRPCQ_WRITE_MODE  	0x1 /* lrpcq-channel ����������� ������ ��� ������ */
#define P2LRPCQ_READ_MODE   	0x2 /* lrpcq-channel ����������� ������ ��� ������ */
#define P2LRPCQ_READWRITE_MODE  (P2LRPCQ_WRITE_MODE|P2LRPCQ_READ_MODE)

/* ������ ����������� ������ �� ��������� */
#define P2LRPCQ_DEFAULT_BUFSIZE (10 * 1024 * 1024)

#ifdef  __cplusplus
extern "C" {
#endif

P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQCreate(P2LRPCQ_HANDLE* hClient);
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQFree(P2LRPCQ_HANDLE hClient);

P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQDisconnect(P2LRPCQ_HANDLE hClient);

/* P2LRPCQConnect ������������� ���������� � ���������
 * @server  ��� �������
 * @bufsize ������ ����������� ������ � ������. ������ ��� ����� 1kb 
 * @mode    ����� ��������: ��� ������ (P2LRPCQ_READ_MODE), ��� ������ (P2LRPCQ_WRITE_MODE), �����������
 * @timeout ����� �������� ������ �� �������. ������ ���� ������ 100ms
*/ 
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQConnect(P2LRPCQ_HANDLE hClient, const char* server, UINT32 bufsize, UINT32 mode, UINT32 timeout);

P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQStartWriting(P2LRPCQ_HANDLE hClient);
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQFinishWriting(P2LRPCQ_HANDLE hClient);
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQCancelWriting(P2LRPCQ_HANDLE hClient);
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQWrite(P2LRPCQ_HANDLE hClient, const char* buf, UINT32 sz, UINT32 timeout);
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQAllocate(P2LRPCQ_HANDLE hClient, char** ppBuf, UINT32 size, UINT32 timeout);


/* ���������-����� ����� 'WriterNeedReadEventSignal'. ����� ������ � ��� ������, ���� ������������ �-�� P2LRPCQGetWriteWaitHandle */
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQWriterNeedReadEventSignal(P2LRPCQ_HANDLE pChannel,BOOL bNeed);

/* ������ */

/* �������� ��������� �� ����� � ������ ������. 
 * ����� ������� ��������� ������ �-�� P2LRPCQRead  ���������� �������� �-��
 * P2LRPCQFinishReading. ��� finish redaing'a ��������� ����� P2LRPCQRead
 * ������ ��������� �� ��� �� ����� �����.
 * ����� P2LRPCQFinishReading ����������, ���������� �� P2LRPCQRead ������������ ������ */

P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQRead(P2LRPCQ_HANDLE hClient, char const** pBuf,UINT32* pSize,UINT32 timeout);

/* ����� �� ��������� ����� � ������ */
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQFinishReading(P2LRPCQ_HANDLE hClient);

/*��������� ����� �� ������ � ���������������� ����� */
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQReadBuffer(P2LRPCQ_HANDLE hClient,char* buf, UINT32* psz,UINT32 timeout);

/* ������ */
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQCreateServer(const char* serverName,P2LRPCQ_SERVER_HANDLE* hServer);
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQFreeServer(P2LRPCQ_SERVER_HANDLE hServer);
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQListen(P2LRPCQ_SERVER_HANDLE hServer, P2LRPCQ_HANDLE* hClient, UINT32 timeout);
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQGetServerName(P2LRPCQ_SERVER_HANDLE hServer,char* serverName, UINT32 sz);

#ifdef _WIN32
/* ����� ��� ������ �� ������ (auto-reset). ��� ������ � ����� �������� ������������� ���� ����� */
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQGetReadWaitHandle(P2LRPCQ_HANDLE hChannel,HANDLE*  pEventHandle);

/* ����� ��� ������ � ����� (auto-reset). '��������' ��������� ���� ����� ���
 * ������, �� ������ � ��� ������, ����  ���� ������� �-�� WriterNeedReadEventSignal(TRUE).
 * ���� 'WriterNeedReadEventSignal' ����������� � FALSE ��� ������ ������ � ����� � ��������� ����������, ��� ����, ����� '��������' �� ����� ������� 
 * system call'a (SetEvent)��� ������ ������.
 * ������ �������������, ���� ����� ������� ����������� ������ � ��� ����-������:
 * ///////////////////////////////////////////////////////////////////////////
 * P2LRPCQGetWriteWaitHandle(hChannel,hReadyForSend);
 * P2LRPCQWriterNeedReadEventSignal(TRUE);
 * err = P2LRPCQWrite(hChannel,pBuf,szBuf,0);
 * if(err != P2ERR_LRPCQ_TIMEOUT)
 * 		//handle successfull write or write error 
 * else	
 * 	   //wait for send ready or another event
 * 	   handlesToWait[] = {hReadyForSend,hAnotherEvent};
 * 	   waitRes = WaitForMultipleObjects(2,handlesToWait,FALSE,timeout);
 *	   //handle waitRes: if hReadyForSend signaled, try to write and so on.
 * endif
 * P2LRPCQWriterNeedReadEventSignal(FALSE);  
 * */  
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQGetWriteWaitHandle(P2LRPCQ_HANDLE hChannel,HANDLE* pEventHandle);

/* ������ peer'�� */
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQGetClosedWaitHandle(P2LRPCQ_HANDLE hChannel,HANDLE* pHandle);
#else

P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQGetReadWaitHandle(P2LRPCQ_HANDLE hChannel,  sem_t** pEventHandle); 
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQGetWriteWaitHandle(P2LRPCQ_HANDLE hChannel, sem_t** pEventHandle);

#endif

/* �������� ����� ������.
 ���� � ������� peerAliveTimeout ��� ������ �� ������, �� ������� ������ ����� ������ P2ERR_LRPCQ_PEER_NOT_ALIVE
 ������� ��� ��������� ������� �  ������� peer ��� terminated.  ��� timeout == 0 -- ��������� */ 
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQSetPeerAliveTimeout(P2LRPCQ_HANDLE hChannel, UINT32 timeout);


#ifdef  __cplusplus
}
#endif

#endif //__MACH__

#endif // P2LRPCQ_H_HEADER








