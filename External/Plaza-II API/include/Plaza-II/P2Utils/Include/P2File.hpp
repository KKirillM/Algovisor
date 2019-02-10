#ifndef _P2FILE_HPP_
#define _P2FILE_HPP_


#ifdef _WIN32 
#include "../../P2Common/Include/P2Windows.h"
#elif defined(__linux__) || defined (__MACH__)
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#endif 
#include "AutoHandleEx.hpp"


#ifdef _WIN32

#define P2FILE_READ 			GENERIC_READ
#define P2FILE_WRITE 			GENERIC_WRITE
#define P2FILE_READ_WRITE		(GENERIC_READ|GENERIC_WRITE)

#define P2FILE_CREATE_ALWAYS 	CREATE_ALWAYS
#define P2FILE_OPEN_EXISTING 	OPEN_EXISTING
#define P2FILE_OPEN_ALWAYS      OPEN_ALWAYS
#define P2FILE_CREATE_NEW       CREATE_NEW

#define P2FILE_BEGIN 			FILE_BEGIN
#define P2FILE_CURRENT			FILE_CURRENT 
#define P2FILE_END				FILE_END

#define P2FILE_ATTRIBUTE_NORMAL FILE_ATTRIBUTE_NORMAL

#define P2FILE_FLAG_WRITE_THROUGH FILE_FLAG_WRITE_THROUGH

#define P2FILE_INVALID_HANDLE 	INVALID_HANDLE_VALUE

#define P2FILE_HANDLE_SIZE		(sizeof(HANDLE))

typedef HANDLE P2FILE_HANDLE;

#elif defined(__linux__) || defined (__MACH__)

#define P2FILE_READ 			O_RDONLY
#define P2FILE_WRITE 			O_WRONLY
#define P2FILE_READ_WRITE		O_RDWR

#define P2FILE_CREATE_ALWAYS 	(O_CREAT|O_TRUNC)
#define P2FILE_OPEN_EXISTING 	0x0 
#define P2FILE_OPEN_ALWAYS      (O_CREAT) 
#define P2FILE_CREATE_NEW       (O_CREAT|O_EXCL|O_TRUNC)

#define P2FILE_BEGIN 			SEEK_SET
#define P2FILE_CURRENT			SEEK_CUR
#define P2FILE_END				SEEK_END

#define P2FILE_ATTRIBUTE_NORMAL 0x0

// для MacOS это сложнее, так что пока не поддерживается
//(см. http://www.humboldt.co.uk/2009/03/fsync-across-platforms.html)
#ifdef __linux__
#define P2FILE_FLAG_WRITE_THROUGH O_SYNC
#endif // __linux__

#define P2FILE_INVALID_HANDLE (-1) 
#define P2FILE_HANDLE_SIZE	  (sizeof(int))

typedef int P2FILE_HANDLE; 

#endif	//defined(__linux__) || defined (__MACH__)

inline P2ERR P2FileCreate(P2FILE_HANDLE* hFile, const char* pFileName, UINT32 dwAccess, UINT32 dwDisposition, UINT32 dwFlag = 0x0)
{
	if(!hFile)
		return P2ERR_COMMON_WRONG_ARGUMENTS;

	if(dwAccess != P2FILE_READ 	&& 
	   dwAccess != P2FILE_WRITE && 
	   dwAccess != P2FILE_READ_WRITE)
	{ 
		return P2ERR_COMMON_WRONG_ARGUMENTS; 
	}

    switch (dwDisposition)
    {
        case P2FILE_CREATE_ALWAYS:
        case P2FILE_OPEN_EXISTING:
        case P2FILE_OPEN_ALWAYS:
        case P2FILE_CREATE_NEW:
            break;
        default:
            return P2ERR_COMMON_WRONG_ARGUMENTS;
    }

#ifdef _WIN32
	DWORD dwShareMode(0); 
	dwShareMode	|= ((dwAccess & P2FILE_READ)  ? FILE_SHARE_READ  : 0); 
	dwShareMode	|= ((dwAccess & P2FILE_WRITE) ? FILE_SHARE_READ | FILE_SHARE_WRITE : 0); 

	*hFile = ::CreateFile(pFileName, dwAccess, dwShareMode, NULL, dwDisposition, FILE_ATTRIBUTE_NORMAL|dwFlag, NULL); 
#elif defined(__linux__) || defined (__MACH__)
	int flags(0);
	mode_t mode = (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);

	flags |= dwAccess;
	flags |= dwDisposition;
	flags |= dwFlag;

	*hFile = ::open(pFileName, flags, mode);
#endif //defined(__linux__) || defined (__MACH__)

	if(*hFile == P2FILE_INVALID_HANDLE)
	{
		return P2ERR_COMMON_SYSCALL_FAILED;
	}

	return P2ERR_OK;
}


inline P2ERR P2FileRead(P2FILE_HANDLE hFile, void* pBuffer, size_t bytesToRead, size_t* pBytesRead)
{
#ifdef _WIN32
	DWORD bytes; 
	if(!::ReadFile(hFile, pBuffer, static_cast<DWORD>(bytesToRead), &bytes, NULL))
		return P2ERR_COMMON_SYSCALL_FAILED;
#elif defined(__linux__) || defined (__MACH__)
	ssize_t bytes = ::read(hFile, pBuffer, bytesToRead); 
	if(bytes == -1)
		return  P2ERR_COMMON_SYSCALL_FAILED;
#endif 

	if (pBytesRead)
        *pBytesRead = bytes; 
	return P2ERR_OK; 
}

inline P2ERR P2FileCheckedRead(P2FILE_HANDLE hFile, void* pBuffer, size_t bytesToRead, size_t* pBytesRead = NULL)
{ 
	P2ERR res(P2ERR_OK);
	size_t bytes = 0;

	if (P2SUCCEEDEDV(P2FileRead(hFile, pBuffer, bytesToRead, &bytes), res) && bytes != bytesToRead)
	{
		res = P2ERR_BUFFER_SIZES_MISMATCH; 
	}

	if (pBytesRead)
        *pBytesRead = bytes;

	return res; 
}


inline P2ERR P2FileWrite(P2FILE_HANDLE hFile, const void* pBuffer, size_t bufferSize, size_t *pBytesWritten = NULL)
{
	P2ERR res(P2ERR_OK);

#ifdef _WIN32

	DWORD bytes; 
	if(!::WriteFile(hFile, pBuffer, static_cast<DWORD>(bufferSize), &bytes, NULL))
		res = P2ERR_COMMON_SYSCALL_FAILED;

#elif defined(__linux__) || defined (__MACH__)

	ssize_t bytes = ::write(hFile, pBuffer, bufferSize); 
	if(-1 == bytes) 
		res = P2ERR_COMMON_SYSCALL_FAILED;

#endif //defined(__linux__) || defined (__MACH__)

	if(pBytesWritten) *pBytesWritten = bytes;

	// todo: EINTR?
	if(P2SUCCEEDED(res) && pBytesWritten && *pBytesWritten != bufferSize)
		return P2ERR_COMMON_SYSCALL_FAILED;

	return res; 
}

inline P2ERR P2FileClose(P2FILE_HANDLE hFile)
{
#ifdef _WIN32
	if(!::CloseHandle(hFile))
		return P2ERR_COMMON_SYSCALL_FAILED;
#elif defined(__linux__) || defined (__MACH__)
	if(-1 == ::close(hFile)) 
		return P2ERR_COMMON_SYSCALL_FAILED;
#endif //defined(__linux__) || defined (__MACH__)

	return P2ERR_OK; 
}

inline P2ERR P2FileSetPointer(P2FILE_HANDLE hFile, INT64 distance, INT64* pNewPointer, UINT32 whence)
{
#ifdef _WIN32  
	LARGE_INTEGER liNewPointer;
	LARGE_INTEGER liDistance;
	liDistance.QuadPart = distance;

	if (FALSE == ::SetFilePointerEx(hFile, liDistance, &liNewPointer, whence))
    {
		return P2ERR_COMMON_SYSCALL_FAILED;
    }
	if (pNewPointer)
        *pNewPointer = liNewPointer.QuadPart;

#elif defined(__linux__) || defined (__MACH__)

    #if defined(__linux__)
	_ASSERT( sizeof(off64_t) == sizeof(INT64) );
	off64_t offset = ::lseek64(hFile, distance, whence);
    
    #elif defined(__MACH__)
	_ASSERT( sizeof(off_t) == sizeof(INT64) );
	off_t offset = ::lseek(hFile, distance, whence);
    
    #endif
	if (-1 == offset)
    {
		return P2ERR_COMMON_SYSCALL_FAILED;
    }
	if (pNewPointer)
        *pNewPointer = static_cast< INT64 >(offset);
#endif // defined(__linux__) || defined (__MACH__)
	return P2ERR_OK; 
}

inline P2ERR P2FileGetSize(P2FILE_HANDLE hFile, UINT64* pSize)
{ 
#ifdef _WIN32  
	LARGE_INTEGER liSize;
	if (!::GetFileSizeEx(hFile, &liSize))
		return P2ERR_COMMON_SYSCALL_FAILED;
	*pSize = static_cast< UINT64 >(liSize.QuadPart);
#elif defined(__linux__) || defined (__MACH__)
    struct stat64 st;
	if (-1 == fstat64(hFile, &st))
		return P2ERR_COMMON_SYSCALL_FAILED;
	*pSize = st.st_size; 
#endif // defined(__linux__) || defined (__MACH__)

	return P2ERR_OK; 
}

inline P2ERR P2FileFlushBuffers(P2FILE_HANDLE hFile)
{ 
#ifdef _WIN32  
	if (!::FlushFileBuffers(hFile))
		return P2ERR_COMMON_SYSCALL_FAILED; 
#elif defined(__linux__) || defined (__MACH__)
	if (-1 == ::fsync(hFile)) 
		return P2ERR_COMMON_SYSCALL_FAILED; 
#endif // defined(__linux__) || defined (__MACH__)
	return P2ERR_OK; 
}	

#ifdef _WIN32
inline HANDLE P2FileGetNativeHandle(P2FILE_HANDLE hFile) { return hFile; }
#elif defined(__linux__) || defined (__MACH__)
inline int P2FileGetNativeHandle(P2FILE_HANDLE hFile) { return hFile; }
#endif //defined(__linux__) || defined (__MACH__)

inline P2ERR P2FileCheckExisting(const char* filename, bool* pExist)
{
	if(!pExist)
		return P2ERR_COMMON_WRONG_ARGUMENTS; 
#ifdef _WIN32
	*pExist = (-1 != GetFileAttributes(filename));
#elif defined(__linux__) || defined (__MACH__)
	struct stat st;
	int err = stat(filename, &st);
	if(err == -1 && errno != ENOENT)
		return P2ERR_COMMON_SYSCALL_FAILED;
	else if(err == -1)
		*pExist = false;
	else
		*pExist = true; 
#endif //defined(__linux__) || defined (__MACH__)
	return P2ERR_OK; 
}

// Sets new file size (allows to short or extend file)
// File pointer will be moved at the end of file
inline P2ERR P2FileSetSize(P2FILE_HANDLE hFile, UINT64 newSize)
{
#ifdef _WIN32  
	LARGE_INTEGER liDistance;
	liDistance.QuadPart = static_cast< LONGLONG >(newSize);
	if (FALSE == ::SetFilePointerEx(hFile, liDistance, NULL, P2FILE_BEGIN) ||
        FALSE == ::SetEndOfFile(hFile))
    {
		return P2ERR_COMMON_SYSCALL_FAILED;
    }
#elif defined(__linux__)
	if (-1 == ::ftruncate64(hFile, static_cast< off64_t >(newSize)) ||
        -1 == ::lseek64(hFile, 0, SEEK_END))
    {
		return P2ERR_COMMON_SYSCALL_FAILED;
    }
#elif defined (__MACH__)
	if (-1 == ::ftruncate(hFile, static_cast< off_t >(newSize)) ||
        -1 == ::lseek(hFile, 0, SEEK_END))
    {
		return P2ERR_COMMON_SYSCALL_FAILED;
    }
#endif // defined(__linux__) || defined (__MACH__)
	return P2ERR_OK; 
}


namespace p2file_impl {
	struct handle_functions 
	{
		typedef P2FILE_HANDLE handle_type;
		static  handle_type   invalid_handle() { return P2FILE_INVALID_HANDLE; }
		static  void close_handle(handle_type h) { _VERIFY_P2ERR(P2FileClose(h)); }
	};
};




typedef AutoClose<p2file_impl::handle_functions> P2AutoFile;


#endif // _P2FILE_HPP_
