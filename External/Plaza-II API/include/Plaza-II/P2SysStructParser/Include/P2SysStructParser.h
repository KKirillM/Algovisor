#ifndef _P2SYSSTRUCTPARSER_H_
#define _P2SYSSTRUCTPARSER_H_

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef P2SYS_EXPORTS
#define P2SYSSTRUCTPARSER_API P2_API_EXPORTS
#else
#define P2SYSSTRUCTPARSER_API P2_API_IMPORTS
#endif

#include "../../P2SysParser/Include/P2SysParser.h"
#include "../../P2SysScheme/Include/P2SysScheme.h"

#ifdef P2SYS_EXPORTS
  namespace NS_P2StructParser
  {
    class CStructComposer;
    class CStructParser;
  }
  using NS_P2StructParser::CStructComposer;
  using NS_P2StructParser::CStructParser;
#else
  class CStructComposer;
  class CStructParser;
#endif

P2SYSSTRUCTPARSER_API P2ERR P2CALLSPEC P2SysCreateStructComposer(P2SYS_COMPOSER_HANDLE* pComposer, P2DB_TABLE_SCHEME_HANDLE sch);
P2SYSSTRUCTPARSER_API P2ERR P2CALLSPEC P2SysCreateStructParser(P2SYS_PARSER_HANDLE* pParser, P2DB_TABLE_SCHEME_HANDLE sch);

#ifdef  __cplusplus
}
#endif

#endif // _P2SYSSTRUCTPARSER_H_
