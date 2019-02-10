#ifndef pcharhash_h__
#define pcharhash_h__

#include <stdint.h>
#include <cstring>

#if defined(_WIN32)
extern "C" void * __cdecl memcpy(void * dst, const void * src, size_t count);
#pragma intrinsic( memcpy )
#endif

//-----------------------------------------------------------------------------
// Platform-specific functions and macros
//
#ifdef EIF_LEAN 
#ifndef WIN32
inline char* strncpy_x(char* strDest, const char* strSource, size_t destBufSize)
{
    strncpy(strDest, strSource, destBufSize);
    return strDest;
};
#endif
#endif



// Microsoft Visual Studio


#if defined(_MSC_VER)


#define FORCE_INLINE    __forceinline


#include <stdlib.h>

// вернул обратно FOSRV-2448 (FOSRV-2449) count - размер буфера.
inline char* strncpy_x(char* strDest, const char* strSource, size_t destBufSize)
{
    strncpy_s(strDest, destBufSize, strSource, _TRUNCATE);
    return strDest;
};


#define ROTL32(x,y)     _rotl(x,y)
#define ROTL64(x,y)     _rotl64(x,y)


#define BIG_CONSTANT(x) (x)


// Other compilers


#else   // defined(_MSC_VER)

#ifdef _MSC_VER
#define FORCE_INLINE __attribute__((always_inline))
#else
#define FORCE_INLINE inline __attribute__((always_inline))
#endif


inline uint32_t rotl32 ( uint32_t x, int8_t r )
{
    return (x << r) | (x >> (32 - r));
}


inline uint64_t rotl64 ( uint64_t x, int8_t r )
{
    return (x << r) | (x >> (64 - r));
}


#define ROTL32(x,y)     rotl32(x,y)
#define ROTL64(x,y)     rotl64(x,y)


#define BIG_CONSTANT(x) (x##LLU)


#endif // !defined(_MSC_VER)



//-----------------------------------------------------------------------------
// Block read - if your platform needs to do endian-swapping or can only
// handle aligned reads, do the conversion here


FORCE_INLINE uint32_t getblock ( const uint32_t * p, int i )
{
    return p[i];
}


FORCE_INLINE uint64_t getblock ( const uint64_t * p, int i )
{
    return p[i];
}


//-----------------------------------------------------------------------------
// Finalization mix - force all bits of a hash block to avalanche


FORCE_INLINE uint32_t fmix ( uint32_t h )
{
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;


    return h;
}


//----------


FORCE_INLINE uint64_t fmix ( uint64_t k )
{
    k ^= k >> 33;
    k *= BIG_CONSTANT(0xff51afd7ed558ccd);
    k ^= k >> 33;
    k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
    k ^= k >> 33;


    return k;
}

FORCE_INLINE void MurmurHash3_x86_32 ( const void * key, int len,
                         uint32_t seed, void * out )
{
    const uint8_t * data = (const uint8_t*)key;
    const int nblocks = len / 4;


    uint32_t h1 = seed;


    uint32_t c1 = 0xcc9e2d51;
    uint32_t c2 = 0x1b873593;


    //----------
    // body


    const uint32_t * blocks = (const uint32_t *)(data + nblocks*4);


    for(int i = -nblocks; i; i++)
    {
        uint32_t k1 = getblock(blocks,i);


        k1 *= c1;
        k1 = ROTL32(k1,15);
        k1 *= c2;

        h1 ^= k1;
        h1 = ROTL32(h1,13); 
        h1 = h1*5+0xe6546b64;
    }


    //----------
    // tail


    const uint8_t * tail = (const uint8_t*)(data + nblocks*4);


    uint32_t k1 = 0;


    switch(len & 3)
    {
    case 3: k1 ^= tail[2] << 16;
    case 2: k1 ^= tail[1] << 8;
    case 1: k1 ^= tail[0];
        k1 *= c1; k1 = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
    };


    //----------
    // finalization


    h1 ^= len;


    h1 = fmix(h1);


    *(uint32_t*)out = h1;
} 


template <int n>
FORCE_INLINE size_t hashString(const char* str,uint32_t seed = 0)
{
    uint32_t result;
    MurmurHash3_x86_32(&str[0],n,seed,&result);
    return result;          
}

FORCE_INLINE size_t hashLPCString(const char* str, uint32_t seed = 0)
{
    uint32_t result;
    MurmurHash3_x86_32(&str[0],(int)strlen(str),seed,&result);
    return result;          
}

FORCE_INLINE size_t hashStdString(const std::string& str, uint32_t seed = 0)
{
    uint32_t result;
    MurmurHash3_x86_32(str.c_str(),(int)str.length(),seed,&result);
    return result;          
}

struct StdStringHasher 
{
    size_t operator()(const std::string& str) const {return hashStdString(str);}
};

template <typename T>
struct PartHasher 
{
    size_t operator()(const T& partCode) const {return hashString<sizeof(T)>((const char*)&partCode);}
};

FORCE_INLINE size_t hashCombine(uint32_t x, uint32_t y)
{
    uint32_t result;
    MurmurHash3_x86_32(&x,sizeof(x),y,&result);
    return result;          
} 

FORCE_INLINE size_t hashCombine(uint32_t x, uint32_t y, uint32_t z)
{
    uint32_t data[2]={x,y};
    uint32_t result;
    MurmurHash3_x86_32(&data,sizeof(data),z,&result);
    return result;          
}

FORCE_INLINE size_t hashCombine(uint32_t w, uint32_t x, uint32_t y, uint32_t z)
{
    uint32_t data[3]={w,x,y};
    uint32_t result;
    MurmurHash3_x86_32(&data,sizeof(data),z,&result);
    return result;          
}

FORCE_INLINE size_t hashCombine(uint32_t v, uint32_t w, uint32_t x, uint32_t y, uint32_t z)
{
    uint32_t data[4]={v,w,x,y};
    uint32_t result;
    MurmurHash3_x86_32(&data,sizeof(data),z,&result);
    return result;          
} 

FORCE_INLINE size_t hashCombine64(uint64_t x, uint32_t z)
{
    uint32_t result;
    MurmurHash3_x86_32(&x,sizeof(x),z,&result);
    return result;          
}



#endif // pcharhash_h__
