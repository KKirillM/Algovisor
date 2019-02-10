// stuff.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STUFF_H__2B7CAB93_190D_4196_844E_C1DB396D62CF__INCLUDED_)
#define AFX_STUFF_H__2B7CAB93_190D_4196_844E_C1DB396D62CF__INCLUDED_

#if defined(_WIN32)
#pragma warning( push )
#pragma warning( disable : 4996 )   
#endif // defined(_WIN32)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stddef.h>
#include <stdlib.h>

#if defined(_MSC_VER)
#include <crtdbg.h>
#endif // defined(_MSC_VER)

#include <string.h>
#include "../../P2Common/Include/P2Errors.h"

#if defined(__linux__) || defined(__MACH__)
#include <limits.h>
#include <ctype.h>
#endif

//////////////////////////////////////////////////////////////////////
// #pragma reminder by Jeffrey Richter
// Usage: #pragma _pm(<message>), where <message> - text without "

#ifndef NO_PM_PRAGMA
#define _pm_str_(x)	#x
#define _pm_str(x)	_pm_str_(x)
#define _pm(text)	message(__FILE__ "(" _pm_str(__LINE__) "):      >>> " #text " <<<")
#else
#define _pm(text)
#endif
//////////////////////////////////////////////////////////////////////
// _VERIFY macro

#ifdef _DEBUG
#if defined(__linux__) || defined(__MACH__)
#include <assert.h>
#define _VERIFY(expr)	assert(expr)
#else
#define _VERIFY(expr)   _ASSERT(expr)
#endif //defined(__linux__) || defined(__MACH__)
#else // !_DEBUG
#define _VERIFY(expr)   (expr)
#endif // _DEBUG

//////////////////////////////////////////////////////////////////////
// Disable copying class

#define DISABLE_CLASS_COPY_ACCESS(class, access)    \
	private:                                        \
	class(const class&);                        \
	class& operator=(const class&);             \
	access:

#define DISABLE_CLASS_COPY_PB(class) DISABLE_CLASS_COPY_ACCESS(class, public)
#define DISABLE_CLASS_COPY_PV(class) DISABLE_CLASS_COPY_ACCESS(class, private)
#define DISABLE_CLASS_COPY_PT(class) DISABLE_CLASS_COPY_ACCESS(class, protected)

//////////////////////////////////////////////////////////////////////
// P2ERR macroses

// классический макрос для возврата из функции по ошибке. 
// использовать только, если не нужно ничего очищать
// или очистка производится деструкторами
#define RETURN_P2ERR(x) { P2ERR err_____ = x; if (err_____ != P2ERR_OK) { return err_____; } }
// проверка на пустой указатель из аллоков
#define RETURN_P2ERR_MEM(ptr) if (ptr == NULL) return P2ERR_COMMON_NO_MEMORY
// если выражение ложно, то вернуться с кодом ошибки
#define RETURN_P2ERR_COND(expr, err_code) if(!(expr)) return err_code
// если выражение ложно, то вернуться с кодом ошибки
//#define RETURN_P2ERR_COND_ASS(expr, err_code) _ASSERTE(expr); if(!(expr)) return err_code
#if defined(__linux__) || defined(__MACH__)
#define RETURN_P2ERR_COND_ASS(expr, err_code) { bool bRes = expr; assert(bRes); if(!bRes) return err_code; }
#else
#define RETURN_P2ERR_COND_ASS(expr, err_code) { bool bRes = expr; _ASSERTE(bRes); if(!bRes) return err_code; }
#endif
// если аргумент NULL, вернуть ошибку
#define RETURN_NULL_ARG(arg) if (arg == NULL) return P2ERR_COMMON_WRONG_ARGUMENTS

#define RETURN_P2ERR_GET_SIZE(x) { P2ERR err_____=x; if (err_____ != P2ERR_COMMON_REAL_SIZE_RETURNED){ return err_____;} }

#define RETURN_UINT32_MAX_ARG_ASS(arg) RETURN_P2ERR_COND_ASS(arg < UINT_MAX, P2ERR_COMMON_BUFFER_TOO_LARGE)
//////////////////////////////////////////////////////////////////////////
// нужно определить макрос TRACE_LOG_CAT() выдающий категорию для сообщений макросов типа RETURN_P2ERR_TRACE
//////////////////////////////////////////////////////////////////////////
#define RETURN_P2ERR_TRACE(x, s) { P2ERR err____ = x; if(P2FAILED(err____)) { P2TRACE_ERROR_WITH_CODE_EX((err____, TRACE_LOG_CAT(), s)); return err____; } }
#define RETURN_P2ERR_TRACE_1(x, s, a) { P2ERR err____ = x; if(P2FAILED(err____)) { P2TRACE_ERROR_WITH_CODE_EX((err____, TRACE_LOG_CAT(), s, a)); return err____; } }
#define RETURN_P2ERR_TRACE_2(x, s, a, b) { P2ERR err____ = x; if(P2FAILED(err____)) { P2TRACE_ERROR_WITH_CODE_EX((err____, TRACE_LOG_CAT(), s, a, b)); return err____; } }
#define RETURN_P2ERR_TRACE_3(x, s, a, b, c) { P2ERR err____ = x; if(P2FAILED(err____)) { P2TRACE_ERROR_WITH_CODE_EX((err____, TRACE_LOG_CAT(), s, a, b, c)); return err____; } }
#define RETURN_P2ERR_TRACE_4(x, s, a, b, c, d) { P2ERR err____ = x; if(P2FAILED(err____)) { P2TRACE_ERROR_WITH_CODE_EX((err____, TRACE_LOG_CAT(), s, a, b, c, d)); return err____; } }
#define RETURN_P2ERR_TRACE_5(x, s, a, b, c, d, e) { P2ERR err____ = x; if(P2FAILED(err____)) { P2TRACE_ERROR_WITH_CODE_EX((err____, TRACE_LOG_CAT(), s, a, b, c, d, e)); return err____; } }
#if _MSC_VER >= 1400 || defined(__GNUC__)
#define RETURN_P2ERR_TRC(x, s, ...) { P2ERR err____ = x; if(P2FAILED(err____)) { P2LogErrorWithCodeEx(err____, TRACE_LOG_CAT(), s, __VA_ARGS__); return err____; } }
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define _VERIFY_P2ERR(expr)   _VERIFY(P2SUCCEEDED(expr))
#else // !_DEBUG
#define _VERIFY_P2ERR(expr)   (expr)
#endif // _DEBUG

//////////////////////////////////////////////////////////////////////
// TRACE(s)

#ifdef _DEBUG

/*inline void TRACE0th__(char* str)
{
    char out[200];
    sprintf(out, "0x%08X %s", GetCurrentThreadId(), str);
    OutputDebugString(out);
}*/

#else // !_DEBUG

//#define TRACE0th__(str)

#endif // _DEBUG

//////////////////////////////////////////////////////////////////////
// 'C(ritical) S(ection) M(ust) B(e) F(ree)'
// (Текущий поток не должен находиться в этой критической секции)
//
// В DEBUG-версии проекта срабатывает _ASSERT, если данная критическая
// секция не освобождена текущим потоком.
// Может быть использован с CCS и CCSI.

#if defined(__linux__) || defined(__MACH__)
#define CSMBF__(cs)
#else
#ifdef _DEBUG
#define CSMBF__(cs) (cs)->MBF__()
#else // !_DEBUG
#define CSMBF__(cs)
#endif // _DEBUG
#endif // defined(__linux__) || defined(__MACH__)

//////////////////////////////////////////////////////////////////////
// 'C(ritical) S(ection) M(ust) B(e) L(ocked)' in this point by current thread
//
// В DEBUG-версии проекта срабатывает _ASSERT, если текущий поток
// не вошел в эту критическую секцию.
// Может быть использован с CCS и CCSI.

#if defined(__linux__) || defined(__MACH__)
#define CSMBL__(cs)
#else
#ifdef _DEBUG
#define CSMBL__(cs) (cs)->MBL__()
#else // !_DEBUG
#define CSMBL__(cs)
#endif // _DEBUG
#endif // defined(__linux__) || defined(__MACH__)

//////////////////////////////////////////////////////////////////////
// Find deleting of interface(s)

//#define IFACE_CHECK_FOR_DELETE__

#if defined(_DEBUG) && defined(IFACE_CHECK_FOR_DELETE__)
#define CHECK_FOR_DELETE__(IFace)   \
	protected:                      \
	~IFace();
#else
#define CHECK_FOR_DELETE__(IFace)
#endif

////////////////////////////////////////////////////////////////////////////////
// занудная функция копирования строки. проверяет на валидность параметры,
// если указан недефолтный maxPossibleSize, то строка проверяется на наличие нуля в конце
// также проверяется и корректируется параметр pSize
// заключающий ноль учитывается в размере!!!!
#define IMPOSSIBLE_STRING_SIZE ~0
inline P2ERR SafeStrNCopy(char *dest, const char *src, /*[in, out]*/ size_t *pSize, size_t maxPossibleSize = IMPOSSIBLE_STRING_SIZE)
{
	if ((src == NULL) || (pSize == NULL)) {
		return P2ERR_COMMON_WRONG_ARGUMENTS;
	}
	// рассчет длины входной строки
	size_t len;
	if (maxPossibleSize == (size_t)IMPOSSIBLE_STRING_SIZE) {
		len = strlen(src) + 1;
	} else {
		len = 0;
		const char *srcp = src;
		for(; (srcp - src < static_cast<ptrdiff_t>(maxPossibleSize)) && (*srcp != 0); srcp++) {
			len++;
		}
		if ( static_cast<size_t>(srcp-src) < maxPossibleSize) {
			len++;
		} else {
			return P2ERR_COMMON_BAD_STRING;
		}
	}
    if (dest == NULL) {
        *pSize = len;
        return P2ERR_COMMON_REAL_SIZE_RETURNED;
    }
	if (len > *pSize) {
        *pSize = len;
		return P2ERR_COMMON_BUFFER_TOO_SMALL;
	}
	memcpy(dest, src, len);
	*pSize = len;
	return P2ERR_OK;
}

// новая однопроходная версия. не поддерживает максимальную длину.
inline P2ERR SafeStrNCopy2(char *dest, const char *src, /*[in, out]*/ size_t *pSize)
{
	if ((src == NULL) || (pSize == NULL)) {
		return P2ERR_COMMON_WRONG_ARGUMENTS;
	}
	if (dest != NULL) {
		size_t sz = *pSize - 1;
		// тупо копируем сколько можем
		size_t rem = sz; // 1 Для нуля
		while (rem-- != 0) {
			char ch = (*dest++ = *src++);
			if (ch == 0) {
				*pSize -= rem + 1;
				return P2ERR_OK;
			}
		}
		while (*src++ != 0) ++sz;
		*pSize = sz + 1;
		return P2ERR_COMMON_BUFFER_TOO_SMALL;
	}
	*pSize = strlen(src) + 1;
	return P2ERR_COMMON_REAL_SIZE_RETURNED;
}
//////////////////////////////////////////////////////////////////////
// кошерное копирование блока в буфер пользователя
inline P2ERR SafeMemCopy(void *pDestData, size_t *pSize, const void *pSrcData, size_t realSize)
{
	RETURN_NULL_ARG(pSize);
	RETURN_NULL_ARG(pSrcData);
	size_t userSize = *pSize;
	*pSize = realSize;
	if (pDestData == NULL) return P2ERR_COMMON_REAL_SIZE_RETURNED;
	RETURN_P2ERR_COND((userSize >= realSize), P2ERR_COMMON_BUFFER_TOO_SMALL);
	memcpy(pDestData, pSrcData, realSize);
	return P2ERR_OK;
}

//////////////////////////////////////////////////////////////////////
inline P2ERR SimpleSafeStrCopy(char* pDestData, size_t *pSize, const char* pSrcData)
{
	RETURN_NULL_ARG(pSize);
	RETURN_NULL_ARG(pSrcData);
	size_t userSize = *pSize;
	*pSize = strlen(pSrcData) + 1;
	if (pDestData == NULL) return P2ERR_COMMON_REAL_SIZE_RETURNED;
	RETURN_P2ERR_COND((userSize >= *pSize), P2ERR_COMMON_BUFFER_TOO_SMALL);
	strcpy(pDestData, pSrcData);
	return P2ERR_OK;
}

//////////////////////////////////////////////////////////////////////
// Safe c-array from STL vector

template <class Vec>
inline const typename Vec::value_type* VEC2PTR(const Vec& v)
{
	return v.empty() ? 0 : &v[0];
}

template <class Vec>
inline typename Vec::value_type* VEC2PTR(Vec& v)
{
	return v.empty() ? 0 : &v[0];
}

//////////////////////////////////////////////////////////////////////
// Не совсем атомарное, но вполне корректное interlocked-изменение
// INT64-переменной
// (Для целей статистики вполне годится)

#ifdef _WINDOWS_ // Признак включения файла windows.h
#ifdef _WIN64
#define InterlockedExchangeAdd_ InterlockedExchangeAdd64
#else
inline void InterlockedExchangeAdd_(/*volatile*/ INT64* pVal, INT64 add)
{
	_ASSERT(sizeof(INT64) == sizeof(LONG) * 2);
	LONG highAdd = (LONG)((add + (UINT32)InterlockedExchangeAdd(reinterpret_cast<LONG*>(pVal), static_cast<LONG>(add))) >> 32);
	if (highAdd) {
		InterlockedExchangeAdd(reinterpret_cast<LONG*>(pVal) + 1, highAdd);
	}
}
#endif // _WIN64
#endif // _WINDOWS_



// compile-time assertion
#define STATIC_ASSERT(condition, text)\
    typedef char assert_failed_##text [(condition) ? 1 : -1];

#if defined(_WIN32)
#pragma warning( pop )
#endif 

//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_STUFF_H__2B7CAB93_190D_4196_844E_C1DB396D62CF__INCLUDED_)
