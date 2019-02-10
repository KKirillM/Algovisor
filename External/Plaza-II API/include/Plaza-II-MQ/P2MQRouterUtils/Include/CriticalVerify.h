// CriticalVerify.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRITICALVERIFY_H__90CAC753_7D0A_4858_8697_24EB516EA2B7__INCLUDED_)
#define AFX_CRITICALVERIFY_H__90CAC753_7D0A_4858_8697_24EB516EA2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../Plaza-II/P2SysLog/Include/P2SysLog.h"
#include "../../../Plaza-II/P2Common/Include/P2Errors.h"

#include <cassert>

#if !(defined(__linux__) || defined(__MACH__))
#include <process.h>
#endif

#define CRITICAL_ERROR()																				\
    {																									\
       assert(false);																					\
       P2LogErrorEx("VERIFY", "CRITICAL ERROR in  %s (%d). Program aborted.", __FILE__, __LINE__);  	\
       ::exit(P2ERR_EXIT_CRITICAL_ERROR);																\
    }																									\

#define CRITICAL_VERIFY(expr)																			\
    {																									\
        if (!(expr))																					\
        {																								\
			CRITICAL_ERROR();																			\
        }																								\
    }

#define CRITICAL_VERIFY_SYSCALL(expr)																								\
    {																																\
        if (!(expr))																												\
        {                                                                                       		    						\
            P2LogErrorEx("VERIFY", "critical syscall error %s (%d); %r. Program aborted.", __FILE__, __LINE__, GetLastError());  	\
            assert(false);																											\
            ::exit(P2ERR_EXIT_CRITICAL_ERROR);																						\
        }																															\
    }


#endif // !defined(AFX_CRITICALVERIFY_H__90CAC753_7D0A_4858_8697_24EB516EA2B7__INCLUDED_)
