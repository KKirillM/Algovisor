#ifndef _P2SYSP1PARSER_H_
#define _P2SYSP1PARSER_H_

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef P2SYSEXT_EXPORTS
#define P2SYSP1PARSER_API __declspec(dllexport)
#else
#define P2SYSP1PARSER_API __declspec(dllimport)
#endif

#include "../../P2SysParser/Include/P2SysParser.h"

P2SYSP1PARSER_API P2ERR P2SysCreateP1Composer( P2SYS_COMPOSER_HANDLE* pComposer );
P2SYSP1PARSER_API P2ERR P2SysCreateP1Parser( P2SYS_PARSER_HANDLE* pParser );

#ifdef  __cplusplus
}
#endif

#define P1PARSEROPT_COMPOSEI8AS4 1

#endif _P2SYSP1PARSER_H_
