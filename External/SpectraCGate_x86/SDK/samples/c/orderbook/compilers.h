#ifndef __CGUTILS_COMPILERS_H__
#define __CGUTILS_COMPILERS_H__

#ifdef __GNUC__
#define _GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#else
#define _GCC_VERSION 0
#endif

#if defined(__clang__)
# undef CGUTILS_HAS_CXX_UNIQUE_PTR
# if defined(__APPLE__)
#  undef CGUTILS_HAS_CXX_TO_STRING
# elif defined(__clang_major__) && (__clang_major__ * 100 + __clang_minor__ > 301)
#  define CGUTILS_HAS_CXX_TO_STRING 1
# else
#  undef CGUTILS_HAS_CXX_TO_STRING
# endif
#elif (_GCC_VERSION > 405)
# define CGUTILS_HAS_CXX_UNIQUE_PTR 1
# define CGUTILS_HAS_CXX_TO_STRING 1
#elif (_GCC_VERSION > 401)
# define CGUTILS_HAS_CXX_UNIQUE_PTR 1
# undef CGUTILS_HAS_CXX_TO_STRING
#else
# undef CGUTILS_HAS_CXX_UNIQUE_PTR
# undef CGUTILS_HAS_CXX_TO_STRING
#endif

#endif//__CGUTILS_COMPILERS_H__
