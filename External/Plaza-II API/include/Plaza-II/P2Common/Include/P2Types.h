#ifndef _P2TYPES_H_
#define _P2TYPES_H_

// Основные типы платформы. Также включает слой совместимости для разных платформ

#if defined(_MSC_VER)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // _CRT_SECURE_NO_WARNINGS
#endif // defined(_MSC_VER)

#include <stddef.h>

// мак тоже юникс
// Оставлено для сборки библиотеки P2Crypt
#ifdef __MACH__
#define __unix__ 1
#endif


#if defined(__linux__) || defined(__MACH__)
#include <errno.h>
#endif



// 32битная или 64битная платформа
// TODO: нужно использовать какой то другой макрос не от архитектуры системы
#if defined(_M_IX86) || defined(__i386__)
#define  _P2_PLATFORM_32_
#elif defined(_M_X64) || defined(__x86_64__)
#define  _P2_PLATFORM_64_
#else
#error "Unknown platform"
#endif // 32битная или 64битная платформа

// Для совместимости с "корными" типами
// При необходимости использования типов Плаза-1,
// файл "Core\corebase\Include\core.h" надо включать
// до P2Types.h, ессно... ;)
#ifdef __coretype_h

#pragma message("********** Does programm crash?.. Maybe problem is here :\"" __FILE__ "\"? **********")

#else // __coretype_h

////////////////////////////////////////////////////////////////////////////////
// типы данных
// 1. типы, привязанные к размеру. 
typedef unsigned char UINT8;
typedef signed char INT8;
typedef unsigned short int UINT16;
typedef short int INT16;
typedef unsigned int UINT32;
typedef int INT32;
#if defined(_MSC_VER)
typedef unsigned __int64 UINT64;
typedef __int64 INT64;
#elif defined(__GNUC__)
typedef unsigned long long UINT64;
typedef long long INT64;
#else
#error Unsupported compiler
#endif

// 2. типы, нужные для Windows. Желательно их изжить

// Тип LONG в винде имеет размер 32бита. В линуксе оставляем тот же самый размер во избежание возможных проблем при портировании.

#if defined(_WIN32)
typedef unsigned long ULONG;
typedef long LONG;
#elif defined(__linux__) || defined(__MACH__)
typedef unsigned int ULONG;
typedef int LONG;
#endif

typedef unsigned char BYTE;
typedef unsigned short int WORD;
typedef unsigned int UINT;
typedef const char* LPCSTR;
typedef char* PSTR;
typedef const char* PCSTR;
typedef void* PVOID;
typedef const void* PCVOID;
typedef int BOOL;
typedef wchar_t WCHAR;
#if defined(__linux__) || defined(__MACH__)
typedef long long LONGLONG;
typedef UINT64 ULONGLONG;
typedef LONG* LPLONG;
typedef unsigned int DWORD;
typedef DWORD* LPDWORD;
typedef char TCHAR;

// Тип xxx_PTR должен вмещать указатель.
#if defined(_P2_PLATFORM_64_)
typedef unsigned long long  UINT_PTR, *PUINT_PTR;
typedef long long    		INT_PTR,  *PINT_PTR;
#elif defined(_P2_PLATFORM_32_)
typedef unsigned int UINT_PTR, *PUINT_PTR;
typedef int 		 INT_PTR, *PINT_PTR;
#endif  //__i386__
#endif  // defined(__linux__) || defined(__MACH__)

#endif //__coretype_h

#ifdef UNUSED  // may be already defined in MFC, afx.h
#undef UNUSED
#endif // UNUSED
// затыкание компилятора на предмет неиспользуемых переменных
#define UNUSED(expr) ((void)(expr))

// импорт/экспорт функций
#if defined(__GNUC__)
#define P2_API_EXPORTS	__attribute__ ((visibility("default")))
#define P2_API_IMPORTS  __attribute__ ((visibility("default"))) 
#elif defined(_MSC_VER)
#define P2_API_EXPORTS __declspec(dllexport)
#define P2_API_IMPORTS __declspec(dllimport)
#endif

// разные опции компилятора
// Выравнивание
#if defined(__GNUC__)
#define __ALIGNED__(n)     __attribute__((aligned(n)))
#elif defined(_MSC_VER) 
#define __ALIGNED__(n)     __declspec(align(n)) 
#endif // defined(__GNUC__)

#if defined(_MSC_VER)
typedef __int64 int64_t;
#define	PLATFORM_S64(x)		x##i64
#define	PLATFORM_U64(x)		x##ui64
#define FASTCALL __fastcall
#elif defined(__GNUC__)
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS 1
#endif
#include <stdint.h>
#define	PLATFORM_S64(x)		x##ll
#define	PLATFORM_U64(x)		x##ull

#undef FASTCALL
#ifdef __LP64__
#define FASTCALL
#else
#ifdef __MACH__
#define FASTCALL
#else
#define FASTCALL  __attribute__((fastcall))
#endif // __MACH__
#endif // __LP64__

#ifndef MAXLONGLONG
#define MAXLONGLONG     (PLATFORM_S64(0x7fffffffffffffff))
#endif // MAXLONGLONG

#endif // defined(_MSC_VER)

#ifdef _WIN32
	typedef unsigned long LAST_ERROR; /*DWORD*/
#elif defined(__linux__) || defined(__MACH__)
	typedef int LAST_ERROR;
#endif


#if defined(__linux__) || defined(__MACH__)

#ifdef __cplusplus
typedef bool BOOLEAN;
#endif

#define GetLastError() errno

inline void SetLastError(int errnum)
{
	errno = errnum;
}

#define Sleep(x) usleep((x)*1000)

#ifdef __GNUC__
#define InterlockedIncrement(p)	__sync_add_and_fetch((p), 1)
#define InterlockedDecrement(p)	__sync_sub_and_fetch((p), 1)
#define InterlockedCompareExchange(p, Exchange, Comperand) __sync_val_compare_and_swap(p, Comperand, Exchange)
#define _InterlockedCompareExchange64(p, Exchange, Comperand) __sync_val_compare_and_swap(p, Comperand, Exchange)
#define InterlockedExchange(p, val1) __sync_lock_test_and_set(p, val1)
#define InterlockedExchange64(p, val1) __sync_lock_test_and_set(p, val1)
#define InterlockedExchangeAdd(dst, addval) __sync_fetch_and_add(dst, addval)
#define InterlockedExchangeAdd_ InterlockedExchangeAdd
#define _InterlockedAnd(p, val1)	__sync_fetch_and_and((p), val1)
#define _InterlockedOr(p, val1)	__sync_fetch_and_or((p), val1)
#endif

#define TlsGetValue pthread_getspecific
#define TlsSetValue !pthread_setspecific
#define TlsAlloc pthread_key_create
#define TlsFree pthread_key_delete

#define GetCurrentThreadId pthread_self

//typedef unsigned char BYTE;
typedef BYTE *PBYTE;

#define CONST const

// Unix branch gets INT64 counts in microseconds.
#include <sys/socket.h>
typedef int SOCKET;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
typedef struct sockaddr SOCKADDR;
#include <netinet/in.h>
typedef struct sockaddr_in SOCKADDR_IN;
#include <arpa/inet.h>
#include <sys/types.h>
//typedef struct iovec WSABUF;
typedef struct _WSABUF {
    u_long      len;     /* the length of the buffer */
    char*       buf;     /* the pointer to the buffer */
} WSABUF, *LPWSABUF;
#define SD_SEND SHUT_RD
#define SD_RECEIVE SHUT_WR
#define SD_BOTH SHUT_RDWR
#define LPHOSTENT hostent*
#define LPIN_ADDR in_addr*

#define __stdcall 
#define CALLBACK

#define LPCTSTR const char*
#define LPTSTR char*
#define LPSTR char*
#define PTSTR char*

typedef void VOID;

#define WM_QUIT 0x0012

//typedef unsigned long long UINT64; // defined in P2Types.h
//typedef UINT64 ULONGLONG;
//typedef long* LPLONG;

#include <aio.h>
typedef struct aiocb OVERLAPPED;
typedef OVERLAPPED* LPOVERLAPPED;

#define _THROW0()	throw ()

#define INVALID_HANDLE_VALUE NULL


#define INFINITE            0xFFFFFFFF  // Infinite timeout


// from windef.h
// Тип, который вмещает указатель.
typedef unsigned long ULONG_PTR, *PULONG_PTR;
typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;

#define MAKEWORD(a, b)      ((WORD)(((BYTE)((DWORD_PTR)(a) & 0xff)) | ((WORD)((BYTE)((DWORD_PTR)(b) & 0xff))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)((DWORD_PTR)(a) & 0xffff)) | ((DWORD)((WORD)((DWORD_PTR)(b) & 0xffff))) << 16))
#define LOWORD(l)           ((WORD)((DWORD_PTR)(l) & 0xffff))
#define HIWORD(l)           ((WORD)((DWORD_PTR)(l) >> 16))
#define LOBYTE(w)           ((BYTE)((DWORD_PTR)(w) & 0xff))
#define HIBYTE(w)           ((BYTE)((DWORD_PTR)(w) >> 8))

#define __cdecl

#define FAILED(err) ((err) != P2ERR_OK)

#define UNUSED_FUNCTION __attribute__ ((unused))


#include <errno.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#define WSAGetLastError() errno
#define GetLastError() errno
#define WSAEWOULDBLOCK EAGAIN
#include <stdio.h>

/* Minimum and maximum macros */

#define __max(a,b)  (((a) > (b)) ? (a) : (b))
#define __min(a,b)  (((a) < (b)) ? (a) : (b))

// typedef const char* PCSTR;

#define IntToPtr( i )    ((void *)(INT_PTR)((int)i))

typedef void* HINSTANCE;

#define APIENTRY

#include <sys/time.h>
#include <unistd.h>
static inline unsigned GetTickCount()
{
    //get the current number of microseconds since january 1st 1970
    struct timeval ts;
    gettimeofday(&ts,0);
    return (unsigned int)(ts.tv_sec * 1000 + (ts.tv_usec / 1000));
}

#if defined(_P2_PLATFORM_64_)
typedef long long WPARAM;
typedef long long LPARAM;
#elif defined(_P2_PLATFORM_32_)
typedef unsigned int WPARAM;
typedef unsigned int LPARAM;
#endif 

typedef struct tagMSG {
    unsigned int        hwnd;
    unsigned int        message;
    WPARAM             wParam;
    LPARAM             lParam;
    unsigned int       time;
} MSG, *PMSG;

#define _snprintf snprintf

typedef union _LARGE_INTEGER {
    struct {
        DWORD LowPart;
        LONG HighPart;
    };
    struct {
        DWORD LowPart;
        LONG HighPart;
    } u;
	LONGLONG QuadPart;
} LARGE_INTEGER;


#endif // defined(__linux__) || defined(__MACH__)


// типы для перехода на 64 битовую архитектуру
typedef size_t P2SIZE_T;
typedef ptrdiff_t P2PTRDIFF_T;

#ifdef __GNUC__
#include <assert.h>
#define _ASSERT(x) assert(x)
#define _ASSERTE2(m,x) assert(x)
#define _ASSERTE(x) _ASSERTE2 x
#endif // __GNUC__

#ifdef __GNUC__
#define P2CALLSPEC
#else
#define P2CALLSPEC __cdecl
#endif // __GNUC__

#pragma pack(push,4)

typedef struct
{
    UINT16      year;       //ex.: 1998
    UINT8       month;      // 1 - 12
    UINT8       day;        // 1 - 31 
    UINT8       hour;       // 0 - 23
    UINT8       minute;     // 0 - 59
    UINT8       second;     // 0 - 59
	UINT8		padding;	// используется для выравнивания, всегда 0
    UINT16      msec;    	// 0 - 999
    UINT32      nanoseconds;// 0 - 999999
} P2TIME;

#pragma pack(pop)

#define P2TIME_SEC_DECIMAL_DIGITS   3

#pragma pack(push,4)
typedef struct
{
	UINT32 size;
	UINT8 data[1];
} P2ZBLOCK;
#pragma pack(pop)

#define P2ZBLOCKN(N) struct{ UINT32 size; UINT8 data[N]; }


// константа обозначает гарантированную длину буфера, в которую поместится транспортное имя
// любоая подсистема вправе наложить свое ограничение на длину имени
#define P2_MAX_POSSIBLE_ADDRESS_SIZE 1024

#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    1
#endif

#if defined(__linux__) || defined(__MACH__)
#define MAX_PATH PATH_MAX
#endif // defined(__linux__) || defined(__MACH__)

#ifdef _WIN32
typedef wchar_t P2WCHAR_T;
#elif defined(__linux__) || defined(__MACH__)
typedef INT16 P2WCHAR_T;
#endif

#endif // _P2TYPES_H_
