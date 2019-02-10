#ifndef _P2SYSTYPES_SPEC_H_
#define _P2SYSTYPES_SPEC_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include "../../P2Common/Include/P2Types.h"

#ifdef P2SYS_EXPORTS
#define P2SYSTYPES_API P2_API_EXPORTS
#else
#define P2SYSTYPES_API P2_API_IMPORTS
#endif

#include "../../P2Common/Include/P2Errors.h"
#include <stdarg.h>

#ifdef  __cplusplus
}
#endif

#endif // _P2SYSTYPES_SPEC_H_
