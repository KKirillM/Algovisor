#ifndef _MURMURHASH_H_
#define _MURMURHASH_H_

#include "../../P2Common/Include/P2Types.h"
#include "../../../Plaza-II/P2Common/Include/P2Windows.h"

// http://sites.google.com/site/murmurhash/
// http://ru.wikipedia.org/wiki/Murmur2
class MurmurHash
{
public:

//     static uint64 hash64( const void * key, int len, unsigned int seed_ )
//     {
// 	    //unsigned int murmurhash2 ( char * key, unsigned int len) {
// 		    const unsigned int m = 0x5bd1e995;
// 		    const unsigned int seed = seed_;
// 		    const int r = 24;
// 		    unsigned int h = seed ^ len;
// 		    const unsigned char * data = (const unsigned char *)key;
// 		    while (len >= 4) {
// 			    unsigned int k;
// 			    k  = data[0];
// 			    k |= data[1] << 8;
// 			    k |= data[2] << 16;
// 			    k |= data[3] << 24;
// 			    k *= m;
// 			    k ^= k >> r;
// 			    k *= m;
// 			    h *= m;
// 			    h ^= k;
// 			    data += 4;
// 			    len -= 4;
// 		    }
// 		    switch (len) {
// 		    case 3:
// 			    h ^= data[2] << 16;
// 		    case 2:
// 			    h ^= data[1] << 8;
// 		    case 1:
// 			    h ^= data[0];
// 			    h *= m;
// 		    };
// 		    h ^= h >> 13;
// 		    h *= m;
// 		    h ^= h >> 15;
// 		    return h;
//     }

    static UINT64 Hash64( const void * key, int len, unsigned int seed )
    {
        // 'm' and 'r' are mixing constants generated offline.
        // They're not really 'magic', they just happen to work well.
        const UINT64 m = PLATFORM_U64(0xc6a4a7935bd1e995);
        const int r = 47;

        UINT64 h = seed ^ (len * m);

        const UINT64 * data = (const UINT64 *)key;
        const UINT64 * end = data + (len/8);

        while(data != end)
        {
            UINT64 k = *data++;

            k *= m; 
            k ^= k >> r; 
            k *= m; 

            h ^= k;
            h *= m; 
        }

        const unsigned char * data2 = (const unsigned char*)data;

        switch(len & 7)
        {
            case 7: h ^= UINT64(data2[6]) << 48;
            case 6: h ^= UINT64(data2[5]) << 40;
            case 5: h ^= UINT64(data2[4]) << 32;
            case 4: h ^= UINT64(data2[3]) << 24;
            case 3: h ^= UINT64(data2[2]) << 16;
            case 2: h ^= UINT64(data2[1]) << 8;
            case 1: h ^= UINT64(data2[0]);
                h *= m;
        };

        h ^= h >> r;
        h *= m;
        h ^= h >> r;

        return h;
    }

    static UINT32 Hash32 ( const void * key, int len, unsigned int seed )
    {
        // 'm' and 'r' are mixing constants generated offline.
        // They're not really 'magic', they just happen to work well.
        const unsigned int m = 0x5bd1e995;
        const int r = 24;

        // Initialize the hash to a 'random' value
        unsigned int h = seed ^ len;

        // Mix 4 bytes at a time into the hash
        const unsigned char * data = (const unsigned char *)key;

        while(len >= 4)
        {
            unsigned int k = *(unsigned int *)data;

            k *= m; 
            k ^= k >> r; 
            k *= m; 

            h *= m; 
            h ^= k;

            data += 4;
            len -= 4;
        }

        // Handle the last few bytes of the input array
        switch(len)
        {
            case 3: h ^= data[2] << 16;
            case 2: h ^= data[1] << 8;
            case 1: h ^= data[0];
                h *= m;
        };

        // Do a few final mixes of the hash to ensure the last few
        // bytes are well-incorporated.
        h ^= h >> 13;
        h *= m;
        h ^= h >> 15;

        return h;
    } 		
private:
    MurmurHash();
    MurmurHash(const MurmurHash &obj);
    MurmurHash& operator=(const MurmurHash& _Right);
};

#endif // _MURMURHASH_H_
