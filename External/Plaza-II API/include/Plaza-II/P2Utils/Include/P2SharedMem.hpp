
#ifndef _P2SHARED_MEM_HPP_
#define _P2SHARED_MEM_HPP_

#include "P2File.hpp"

#ifdef _WIN32

typedef HANDLE P2SHARED_MEM_HANDLE;
#define P2SHARED_MEM_INVALID_HANDLE NULL

#elif defined(__linux__) || defined (__MACH__) 

#include <sys/mman.h>

typedef int P2SHARED_MEM_HANDLE;
#define P2SHARED_MEM_INVALID_HANDLE (-1)

#endif

// todo: в юниксе игнорируется name; то есть  не поддерживаются именованные отбражения без файла

inline P2ERR P2SharedMemCreate(P2SHARED_MEM_HANDLE* pShMemHandle, P2FILE_HANDLE hFile, UINT32 size, const char* name, void* pInheritedMemory = NULL)
{
	if(!pShMemHandle)
		return P2ERR_COMMON_WRONG_ARGUMENTS;

#ifdef _WIN32 

	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE; 

	*pShMemHandle = CreateFileMapping(P2FileGetNativeHandle(hFile), pInheritedMemory? &sa : NULL, PAGE_READWRITE, 0, size, name);   

#elif defined(__linux__) || defined (__MACH__) 

	if(hFile == P2FILE_INVALID_HANDLE)
	{ 
		_ASSERT(pInheritedMemory);		
		char tmpname[] = "/p2shmemXXXXXX";
		if(*mktemp(tmpname) == 0)
			return P2ERR_COMMON_SYSCALL_FAILED;

		if(-1 == (*pShMemHandle = shm_open(tmpname, O_RDWR|O_CREAT|O_EXCL, S_IRUSR|S_IWUSR)))
			return P2ERR_COMMON_SYSCALL_FAILED;

		shm_unlink(tmpname);
		int flags = fcntl(*pShMemHandle, F_GETFD);
		flags &= ~FD_CLOEXEC;
		if(fcntl(*pShMemHandle, F_SETFD, flags) != 0) 
        {
            close(*pShMemHandle);
            return P2ERR_COMMON_SYSCALL_FAILED;
        }
	}
	else 
	{
		*pShMemHandle = dup(P2FileGetNativeHandle(hFile));
        if(*pShMemHandle == -1)
        {
            return P2ERR_COMMON_SYSCALL_FAILED;
        }
	}

	if(-1 == ftruncate(*pShMemHandle, size))
	{
		close(*pShMemHandle);
		*pShMemHandle = P2SHARED_MEM_INVALID_HANDLE;
		return P2ERR_COMMON_SYSCALL_FAILED;
	}

#endif 

	if(pInheritedMemory && pInheritedMemory != pShMemHandle)
	{
		memcpy(pInheritedMemory, pShMemHandle, sizeof(P2SHARED_MEM_HANDLE));
	}

	return P2ERR_OK; 
}

inline P2ERR P2SharedMemGet(P2SHARED_MEM_HANDLE* pHandle, void* pInheritedMemmory)
{
	if(!pHandle)
		return P2ERR_COMMON_WRONG_ARGUMENTS;
		
	memcpy(pHandle, pInheritedMemmory, sizeof(P2SHARED_MEM_HANDLE)); 
	return P2ERR_OK;
}

inline P2ERR P2SharedMemClose(P2SHARED_MEM_HANDLE shmem)
{
#ifdef _WIN32
	CloseHandle(shmem);
#elif defined(__linux__) || defined (__MACH__) 
	close(shmem);
#endif 
	return P2ERR_OK;
}

inline void* P2SharedMemMap(P2SHARED_MEM_HANDLE shmem, UINT32 size)
{ 
#ifdef _WIN32
	return MapViewOfFile(shmem, FILE_MAP_ALL_ACCESS, 0, 0, size);
#elif defined(__linux__) || defined (__MACH__) 
	return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shmem, 0);
#endif 
}

inline void* P2SharedMemMapInPrivateMode(P2SHARED_MEM_HANDLE shmem, UINT32 size)
{
#ifdef _WIN32 
	return MapViewOfFile(shmem, FILE_MAP_COPY, 0, 0, size);
#elif defined(__linux__) || defined (__MACH__)
	return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, shmem, 0);
#endif
}


inline P2ERR P2SharedMemUnMap(void* pMappedMem, UINT32 size)
{
#ifdef _WIN32
    UNUSED(size);
	if(0 == UnmapViewOfFile(pMappedMem))
		return P2ERR_COMMON_SYSCALL_FAILED;
#elif defined(__linux__) || defined (__MACH__) 
	if(-1 == munmap(pMappedMem, size))
		return P2ERR_COMMON_SYSCALL_FAILED;
#endif 
	return P2ERR_OK; 
}

inline P2ERR P2SharedMemFlush(void *pAddress, size_t bytes)
{
#ifdef _WIN32
	if(0 == FlushViewOfFile(pAddress, bytes))
		return P2ERR_COMMON_SYSCALL_FAILED;
#elif defined(__linux__) || defined (__MACH__) 
	if(-1 == msync(pAddress, bytes, MS_SYNC))
		return P2ERR_COMMON_SYSCALL_FAILED; 
#endif
	return P2ERR_OK; 
}


namespace p2shmem_impl {
	struct handle_functions 
	{
		typedef P2SHARED_MEM_HANDLE  handle_type;
		static  handle_type   invalid_handle() { return P2SHARED_MEM_INVALID_HANDLE; }
		static  void close_handle(handle_type h) { _VERIFY_P2ERR(P2SharedMemClose(h)); }
	};
};



typedef AutoClose<p2shmem_impl::handle_functions> P2AutoSharedMem;

class AutoMappedBuffer
{
	void*  m_h;
	UINT32 m_size;

	DISABLE_CLASS_COPY_PB(AutoMappedBuffer);

	public:

	AutoMappedBuffer():m_h(NULL), m_size(0) {}
	void  reset(void* h, UINT32 sz) { m_h = h; m_size = sz; } 
	void* operator()() { return m_h;}
	const void* operator()() const { return m_h;}
	size_t size() const { return m_size; }

	~AutoMappedBuffer()
	{
		if(m_h != NULL)
		{
			_VERIFY_P2ERR(P2SharedMemUnMap(m_h, m_size));
		}
	}
};



















#endif  // _P2SHARED_MEM_HPP_
