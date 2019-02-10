
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

/* режимы открытия, передаваемые в функцию P2LRPCQConnect */
#define P2LRPCQ_WRITE_MODE  	0x1 /* lrpcq-channel открывается только для записи */
#define P2LRPCQ_READ_MODE   	0x2 /* lrpcq-channel открывается только для чтения */
#define P2LRPCQ_READWRITE_MODE  (P2LRPCQ_WRITE_MODE|P2LRPCQ_READ_MODE)

/* размер разделяемой памяти по умолчанию */
#define P2LRPCQ_DEFAULT_BUFSIZE (10 * 1024 * 1024)

#ifdef  __cplusplus
extern "C" {
#endif

P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQCreate(P2LRPCQ_HANDLE* hClient);
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQFree(P2LRPCQ_HANDLE hClient);

P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQDisconnect(P2LRPCQ_HANDLE hClient);

/* P2LRPCQConnect Устанавливает соединение с серврером
 * @server  имя сервера
 * @bufsize размер разделяемой памяти в байтах. Должен быт бльше 1kb 
 * @mode    режим открытия: для чтения (P2LRPCQ_READ_MODE), для записи (P2LRPCQ_WRITE_MODE), объединённый
 * @timeout время ожидания ответа от сервера. Должен быть больше 100ms
*/ 
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQConnect(P2LRPCQ_HANDLE hClient, const char* server, UINT32 bufsize, UINT32 mode, UINT32 timeout);

P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQStartWriting(P2LRPCQ_HANDLE hClient);
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQFinishWriting(P2LRPCQ_HANDLE hClient);
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQCancelWriting(P2LRPCQ_HANDLE hClient);
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQWrite(P2LRPCQ_HANDLE hClient, const char* buf, UINT32 sz, UINT32 timeout);
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQAllocate(P2LRPCQ_HANDLE hClient, char** ppBuf, UINT32 size, UINT32 timeout);


/* установка-сброс флага 'WriterNeedReadEventSignal'. Нужна только в том случае, если используется ф-ия P2LRPCQGetWriteWaitHandle */
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQWriterNeedReadEventSignal(P2LRPCQ_HANDLE pChannel,BOOL bNeed);

/* чтение */

/* получить указатель на пакет и размер пакета. 
 * После каждого успешного вызова ф-ии P2LRPCQRead  необходимо вызывать ф-ию
 * P2LRPCQFinishReading. Без finish redaing'a повторный вызов P2LRPCQRead
 * вернет указатель на тот же самый пакет.
 * После P2LRPCQFinishReading указателем, полученным из P2LRPCQRead пользоваться нельзя */

P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQRead(P2LRPCQ_HANDLE hClient, char const** pBuf,UINT32* pSize,UINT32 timeout);

/* сдвиг на следующий пакет в канале */
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQFinishReading(P2LRPCQ_HANDLE hClient);

/*прочитать пакет из канала в пользовательский буфер */
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQReadBuffer(P2LRPCQ_HANDLE hClient,char* buf, UINT32* psz,UINT32 timeout);

/* сервер */
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQCreateServer(const char* serverName,P2LRPCQ_SERVER_HANDLE* hServer);
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQFreeServer(P2LRPCQ_SERVER_HANDLE hServer);
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQListen(P2LRPCQ_SERVER_HANDLE hServer, P2LRPCQ_HANDLE* hClient, UINT32 timeout);
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQGetServerName(P2LRPCQ_SERVER_HANDLE hServer,char* serverName, UINT32 sz);

#ifdef _WIN32
/* эвент для чтения из канала (auto-reset). При записи в канал писатель устанавливает этот эвент */
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQGetReadWaitHandle(P2LRPCQ_HANDLE hChannel,HANDLE*  pEventHandle);

/* эвент для записи в канал (auto-reset). 'Читатель' установит этот эвент при
 * чтении, но только в том случае, если  была вызвана ф-ия WriterNeedReadEventSignal(TRUE).
 * Флаг 'WriterNeedReadEventSignal' сбрасываетя в FALSE при каждой записи в канал с ненулевым ттаймаутом, для того, чтобы 'читатель' не делал лишнего 
 * system call'a (SetEvent)при каждом чтении.
 * Пример использования, если нужно ожидать возможности записи и еще чего-нибудь:
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

/* закрыт peer'ом */
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQGetClosedWaitHandle(P2LRPCQ_HANDLE hChannel,HANDLE* pHandle);
#else

P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQGetReadWaitHandle(P2LRPCQ_HANDLE hChannel,  sem_t** pEventHandle); 
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQGetWriteWaitHandle(P2LRPCQ_HANDLE hChannel, sem_t** pEventHandle);

#endif

/* Контроль жизни буфера.
 если в течении peerAliveTimeout нет чтения из буфера, то попытка записи вернёт ошибку P2ERR_LRPCQ_PEER_NOT_ALIVE
 сделано для отсекания каналов у  которых peer был terminated.  при timeout == 0 -- выключено */ 
P2LRPCQ_API P2ERR P2CALLSPEC P2LRPCQSetPeerAliveTimeout(P2LRPCQ_HANDLE hChannel, UINT32 timeout);


#ifdef  __cplusplus
}
#endif

#endif //__MACH__

#endif // P2LRPCQ_H_HEADER








