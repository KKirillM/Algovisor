#pragma once

#ifndef FO2_GLOBAL
#define FO2_GLOBAL
#endif

#if defined(_WIN32)
#include <windows.h>
#include <objbase.h>
#endif
#include <assert.h>


#ifndef BYTE_DEFINED
#define BYTE_DEFINED
typedef unsigned char   byte;
typedef unsigned short  word;
typedef unsigned int    dword;
#endif

#pragma warning(disable: 4127)

#ifndef null
#define null NULL
#endif

/*
#ifndef zero
#define zero ( 0 )
#endif
*/

#ifndef DO_NOT_USE
#define DO_NOT_USE( x ) 
#endif

#ifndef USE_IN_DEBUG
#ifdef DEBUG
#define USE_IN_DEBUG( x )	x
#else
#define USE_IN_DEBUG( x ) 
#endif
#endif

//--- number of items in array -----
#define GetItemsNumber( Array ) ( sizeof( Array ) / sizeof( Array[ 0 ] ) )

//--- debug break -----
#ifdef _MSC_VER
#ifdef _X86_
#define DebugBreak()    _asm{ int 3 }
#endif
#endif


namespace fo2base
{

//--- ASSERTS -----
extern bool g_use_asserts;

#if defined(_WIN32)

//--- Quick MessageBox -----
inline void quickMessageBox( PCSTR s )
{
    char str[ 128 ];
    GetModuleFileNameA( NULL, str, GetItemsNumber( str ) );
    MessageBoxA( GetActiveWindow(), s, str, MB_OK );
}

inline void chFail( PSTR msg )
{
    fo2base::quickMessageBox( msg );
    DebugBreak();
}

inline void chAssertFail( LPCSTR file, int line, PCSTR expr )
{
    char str[ 128 ];
    if( fo2base::g_use_asserts )
    {
        wsprintfA( str, "File %s, line %d : %s", file, line, expr );
        fo2base::chFail( str );
    }
}

#endif
}


#ifndef chASSERT

#ifdef _DEBUG
#if defined(_WIN32)
#define chASSERT( x ) if( !( x ) ) fo2base::chAssertFail( __FILE__, __LINE__, #x )
#else
#include <cassert>
#define chASSERT( x ) if( fo2base::g_use_asserts ) assert( x );
#endif
#define chASSERT_TURN_ON()  fo2base::g_use_asserts = true
#define chASSERT_TURN_OFF() fo2base::g_use_asserts = false
#else
#define chASSERT( x )
#define chASSERT_TURN_ON()
#define chASSERT_TURN_OFF()
#endif

#ifdef assert
#undef assert
#endif

#ifdef ASSERT
#undef ASSERT
#endif

#endif

#define assert( x )         chASSERT( x )
#define ASSERT( x )         chASSERT( x )

// you can use these macros to turn on/off assertions in your TEST code,
// so it is possible to test your code in incorrect situations
#define assert_turn_on()    chASSERT_TURN_ON()
#define assert_turn_off()   chASSERT_TURN_OFF()

#pragma warning ( push, 3 )
#pragma warning(disable: 4701)
#pragma warning(disable: 4702)
#pragma warning(disable: 4530)
#include <iomanip>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>
#include <iosfwd>
#include <algorithm>
#include <set>
#include <map>
using namespace std;
#pragma warning(default: 4530)
#pragma warning(default: 4702)
#pragma warning(default: 4701)
#pragma warning ( pop )

// disable incorrect class usage
#ifndef BAN_COPY_CONSTRUCTOR
#if (__cplusplus >= 201103L)
#define BAN_COPY_CONSTRUCTOR(class_name) \
    class_name(const class_name& ) = delete;\
    class_name& operator=(const class_name& ) = delete;
#else
#define BAN_COPY_CONSTRUCTOR(class_name) \
    private:\
    class_name(const class_name& );\
    class_name& operator=(const class_name& );\
    public:
#endif //(__cplusplus >= 201103L)
#endif
