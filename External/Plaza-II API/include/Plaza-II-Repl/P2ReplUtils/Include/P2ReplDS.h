#ifndef P2REPL_DS_H_
#define P2REPL_DS_H_
//////////////////////////////////////////////////////////////////////////
#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#include "../../../Plaza-II-DB/P2DB/Include/P2DB.h"
#include "../../../Plaza-II-Repl/P2ReplCommon/Include/P2Repl.h"

//////////////////////////////////////////////////////////////////////////
// строка дублируетс€ в $\Projects\Units\Plaza-II-Repl\P2ReplClient2\Include\P2ReplClient.h »змен€ть одновременно!
// ‘лаг дл€ прозрачного пробрасывани€ записей
#define P2REPL_POST_CHECK	0
#define P2REPL_POST_INS		1
#define P2REPL_POST_UPD		2
#define P2REPL_POST_MASK	0x3
//////////////////////////////////////////////////////////////////////////
typedef void* P2REPL_DS_CONN_HANDLE;
class IP2ReplDS;
typedef class IP2ReplDS* P2REPL_DS_HANDLE;

typedef void* P2REPL_DS_RECORD_HANDLE;
//////////////////////////////////////////////////////////////////////////
typedef P2ERR (*P2REPL_DS_ENUM_FUNC)(P2DB_RECORD_HANDLE rec, void* cbParam);
//////////////////////////////////////////////////////////////////////////

P2ERR P2ReplDSCreateCustomDbDS(PCSTR dllName, PCSTR sectionName, P2DB_CONN_HANDLE hDbConn, P2REPL_DS_HANDLE* ppDs);

P2ERR P2ReplDSDuplicateDSHandle(P2REPL_DS_HANDLE src, P2REPL_DS_HANDLE* pDest);
P2ERR P2ReplDSFreeDS(P2REPL_DS_HANDLE ds);

#endif //P2REPL_DS_H_

