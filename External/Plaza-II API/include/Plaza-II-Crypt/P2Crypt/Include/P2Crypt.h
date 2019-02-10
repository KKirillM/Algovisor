#ifndef _P2CRYPT_H_
#define _P2CRYPT_H_

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the P2CRYPT_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// P2CRYPT_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

#include "../../../Plaza-II/P2Common/Include/P2Types.h"

#ifdef P2CRYPT_EXPORTS
#define P2CRYPT_API //P2_API_EXPORTS /* build as static lib */
#else
#define P2CRYPT_API //P2_API_IMPORTS
#endif


#ifndef __unix__
#include <windows.h>
#endif

#include "../../../Plaza-II/P2Common/Include/P2Errors.h"

#ifdef __unix__
#define zeroise( memory, size )		memset( memory, 0, size )
#define CRYPT_ERROR				-1
#include <time.h>
time_t getTime( void );
#endif
#ifdef __cplusplus
extern "C"
{
#endif

typedef PVOID P2CRYPT_CTX_HANDLE;

/****************************************************************************
*																			*
*								General Constants							*
*																			*
****************************************************************************/

/* The maximum user key size - 2048 bits */

#define P2CRYPT_MAX_KEYSIZE		256U

/* The maximum IV size - 256 bits */

#define P2CRYPT_MAX_IVSIZE		32U

/* The maximum public-key component size - 4096 bits */

#define P2CRYPT_MAX_PKCSIZE		512U

/* The maximum hash size - 256 bits */

#define P2CRYPT_MAX_HASHSIZE		32U

/* The maximum size of a text string (e.g.key owner name) */

#define P2CRYPT_MAX_TEXTSIZE		64U

/* The minimum and maximum conventional key size in bits.*/

#define MIN_KEYSIZE_BITS		40U
#define MAX_KEYSIZE_BITS		256U

#define MAX_VERSION_LABELSIZE	64U + 1U

#define P2CRYPT_MAX_CTX_EXPORTSIZE	2048U


#define bitsToBytes( bits )			( ( ( bits ) + 7 ) >> 3 )
#define bytesToBits( bytes )		( ( bytes ) << 3 )

/****************************************************************************
*																			*
*							Algorithm and Object Types						*
*																			*
****************************************************************************/

/* Algorithm and mode types */

typedef enum {						/* Algorithms */
	/* No encryption */
	P2CRYPT_ALGO_NONE,				/* No encryption */

	/* Conventional encryption */
	P2CRYPT_ALGO_DES,					/* DES */
	P2CRYPT_ALGO_3DES,				/* Triple DES */
	P2CRYPT_ALGO_CAST,				/* CAST-128 */
	P2CRYPT_ALGO_RC2,					/* RC2 */
	P2CRYPT_ALGO_RC4,					/* RC4 */
	P2CRYPT_ALGO_RC5,					/* RC5 */
	P2CRYPT_ALGO_AES,					/* AES */
	P2CRYPT_ALGO_BLOWFISH,			/* Blowfish */

	/* Hash algorithms */
	P2CRYPT_ALGO_MD2 = 100,			/* MD2 */
	P2CRYPT_ALGO_MD4,					/* MD4 */
	P2CRYPT_ALGO_MD5,					/* MD5 */
	P2CRYPT_ALGO_SHA,					/* SHA/SHA1 */
	P2CRYPT_ALGO_RIPEMD160,			/* RIPE-MD 160 */
	P2CRYPT_ALGO_SHA2,				/* SHA2 placeholder */

	/* MAC's */
	P2CRYPT_ALGO_HMAC_MD5 = 200,		/* HMAC-MD5 */
	P2CRYPT_ALGO_HMAC_SHA,			/* HMAC-SHA */
	P2CRYPT_ALGO_HMAC_RIPEMD160,		/* HMAC-RIPEMD-160 */


	P2CRYPT_ALGO_LAST,				/* Last possible crypt algo value */

	/* In order that we can scan through a range of algorithms with
	   cryptQueryCapability(), we define the following boundary points for
	   each algorithm class */
	P2CRYPT_ALGO_FIRST_CONVENTIONAL = P2CRYPT_ALGO_DES,
	P2CRYPT_ALGO_LAST_CONVENTIONAL = P2CRYPT_ALGO_MD2 - 1,
	P2CRYPT_ALGO_FIRST_HASH = P2CRYPT_ALGO_MD2,
	P2CRYPT_ALGO_LAST_HASH = P2CRYPT_ALGO_HMAC_MD5 - 1,
	P2CRYPT_ALGO_FIRST_MAC = P2CRYPT_ALGO_HMAC_MD5,
	P2CRYPT_ALGO_LAST_MAC = P2CRYPT_ALGO_HMAC_MD5 + 99	/* End of mac algo.range */
	} P2CRYPT_ALGO_TYPE;

typedef enum {						/* Block cipher modes */
	P2CRYPT_MODE_NONE,				/* No encryption mode */
	P2CRYPT_MODE_ECB,					/* ECB */
	P2CRYPT_MODE_CBC,					/* CBC */
	P2CRYPT_MODE_CFB,					/* CFB */
	P2CRYPT_MODE_OFB,					/* OFB */
	P2CRYPT_MODE_LAST					/* Last possible crypt mode value */
	} P2CRYPT_MODE_TYPE;

typedef enum {
	P2CRYPT_ATTR_ALGO,
	P2CRYPT_ATTR_IVSIZE,
	P2CRYPT_ATTR_BLOCKSIZE,
	P2CRYPT_ATTR_KEYSIZE,
	P2CRYPT_ATTR_KEY,
	P2CRYPT_ATTR_ALGO_MODE,
	P2CRYPT_ATTR_IV,
	P2CRYPT_ATTR_FIRST = P2CRYPT_ATTR_ALGO,
	P2CRYPT_ATTR_LAST = P2CRYPT_ATTR_IV,
	P2CRYPT_ATTR_FIRST_RW = P2CRYPT_ATTR_KEY, /* First attribute that can be set and get */
	
} P2CRYPT_ATTR_TYPE;

typedef struct {
	/* Algorithm information */
	char algoName[ P2CRYPT_MAX_TEXTSIZE ];/* Algorithm name */
	int blockSize;					/* Block size of the algorithm */
	int minKeySize;					/* Minimum key size in bytes */
	int keySize;					/* Recommended key size in bytes */
	int maxKeySize;					/* Maximum key size in bytes */
	} P2CRYPT_QUERY_INFO;

typedef enum
{
	P2CRYPT_EXPORT_NATIVE,
} P2CRYPT_CTX_EXPORT_TYPE;


// flags for P2CryptContextCreate
// generate initialization vector (ignoring for hash and MAC algorithms)
#define P2CRYPT_CTX_GEN_IV		0x01
// generate secret key (ignoring for hash algorithms)
#define P2CRYPT_CTX_GEN_KEY		0x02

// generate all needed stuff for ctx
#define P2CRYPT_CTX_GEN_ALL		P2CRYPT_CTX_GEN_IV | P2CRYPT_CTX_GEN_KEY

typedef enum { HASH_START, HASH_CONTINUE, HASH_END, HASH_ALL } HASH_STATE;

extern BOOL cl_isW95;

/*
	P2CryptStartup initializes P2Crypt library;
	must be called once from any thread of application
*/
P2CRYPT_API P2ERR P2CryptStartup(void);

/*
	P2CryptCleanup deinitializes P2Crypt library;
	must be called from the same thread P2CryptStartup called.
	All other threads must be deinited by calling P2CryptThreadCleanup (vide infra)
*/
P2CRYPT_API void P2CryptCleanup(void);

/*
	Any new thread uses P2Crypt must call P2CryptThreadStartup before using any function.
	!!! Not Realized Yet !!!
*/
P2CRYPT_API P2ERR P2CryptThreadStartup(void);

/*
	Any thread that called P2CryptThreadStartup must call P2CryptThreadCleanup on shutdown.
	!!! Not Realized Yet !!!
*/
P2CRYPT_API void P2CryptThreadCleanup(void);

/*
	P2CryptContextCreate builds the crypto environment for crypto operation.
	It also generates secret key and IV if needed/
	- keyLength ignored for algo == [P2CRYPT_ALGO_FIRST_HASH .. P2CRYPT_ALGO_LAST_HASH]
	  if keyLength == 0 used the best key length for this algorithm.
	  keyLength value must be in bits;
	- mode used only with algo == [P2CRYPT_ALGO_FIRST_CONVENTIONAL .. P2CRYPT_ALGO_LAST_CONVENTIONAL]
	- flags - see P2CRYPT_CTX_GEN_*;
*/
P2CRYPT_API P2ERR P2CryptContextCreate(P2CRYPT_CTX_HANDLE *pctx, P2CRYPT_ALGO_TYPE algo, const size_t keyLength,
						 const P2CRYPT_MODE_TYPE mode, int flags);

/*
	P2CryptContextClone makes copy of context. States of contexts are equal.
*/
P2CRYPT_API P2ERR P2CryptContextClone(P2CRYPT_CTX_HANDLE ctx, P2CRYPT_CTX_HANDLE *pctxClone);

/*
	P2CryptContextSetAttr allow to store attributes of P2CRYPT_CTX_HANDLE.
	Attributes described in P2CRYPT_ATTR_TYPE.
	!!! Not Realized Yet !!!
*/
P2CRYPT_API P2ERR P2CryptContextSetAttr(P2CRYPT_CTX_HANDLE ctx, P2CRYPT_ATTR_TYPE attr, const void *value,
										size_t valueSize);

/*
	P2CryptContextGetAttr allow to get attributes from P2CRYPT_CTX_HANDLE.
	Attributes described in P2CRYPT_ATTR_TYPE.
	!!! Not Realized Yet !!!
*/
P2CRYPT_API P2ERR P2CryptContextGetAttr(P2CRYPT_CTX_HANDLE ctx, P2CRYPT_ATTR_TYPE attr, void *value, size_t *pvalueLength);

/*
	P2CryptContextExport exports context into blob.
	!!! Not Realized Yet !!!
*/
P2CRYPT_API P2ERR P2CryptContextExport(P2CRYPT_CTX_HANDLE ctx,
									   P2CRYPT_CTX_EXPORT_TYPE exportType, UINT32 version,
									   void *poutBuffer, size_t *poutLength);

/*
	P2CryptContextImport imports context from blob.
	!!! Not Realized Yet !!!
*/
P2CRYPT_API P2ERR P2CryptContextImport(P2CRYPT_CTX_HANDLE *pctx, void *pinBuffer, size_t inLength);

/*
	P2CryptContextReset resets current context to state when it was initially created
	- newIV must be FALSE for context created with
	  algo == [P2CRYPT_ALGO_FIRST_HASH .. P2CRYPT_ALGO_LAST_HASH] ||
			  [P2CRYPT_ALGO_FIRST_MAC .. P2CRYPT_ALGO_LAST_MAC]
	- newKey must be FALSE for context created with
	  algo == [P2CRYPT_ALGO_FIRST_CONVENTIONAL .. P2CRYPT_ALGO_LAST_CONVENTIONAL]
*/
P2CRYPT_API P2ERR P2CryptContextReset(P2CRYPT_CTX_HANDLE ctx, BOOLEAN newIV, BOOLEAN newKey);

/*
	P2CryptContextFree releases ctx.

*/
P2CRYPT_API P2ERR P2CryptContextFree(P2CRYPT_CTX_HANDLE ctx);


P2CRYPT_API P2ERR P2CryptGetRandomBuf(void *poutBuffer, const size_t bufferLength);

P2CRYPT_API P2ERR P2CryptEncrypt(P2CRYPT_CTX_HANDLE ctx, void *pData, size_t *pDataLength, size_t bufferLength);
P2CRYPT_API P2ERR P2CryptDecrypt(P2CRYPT_CTX_HANDLE ctx, void *pData, size_t *pDataLength);

P2CRYPT_API P2ERR P2CryptHash(P2CRYPT_CTX_HANDLE ctx, void *poutBuffer, size_t *poutLength,
				const void *pinBuffer, const size_t inLength,
				const HASH_STATE hashState );

P2CRYPT_API P2ERR P2CryptMemAlloc( void **pointer, size_t size );
P2CRYPT_API P2ERR P2CryptMemFree( void *pointer );

#ifdef __cplusplus
}
#endif


#endif //_P2CRYPT_H_
