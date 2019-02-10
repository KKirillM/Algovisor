#if !defined(AFX_COMMON_H__57977908_3F40_4C81_A8CB_D34B1512A639__INCLUDED_)
#define AFX_COMMON_H__57977908_3F40_4C81_A8CB_D34B1512A639__INCLUDED_

#include "../../../Plaza-II-eRepl/P2eReplCommon/Include/P2eReplProtocol.h"
#include "../../../Plaza-II/P2SysTypes/Include/P2SysTypes.hpp"

#define INI_SETTINGS_SECTION          "p2esharedmem"
//#define INI_COMMIT_EVENT_NAME         "commitevent"
//#define INI_FREE_MEM_EVENT_NAME       "freememevent"
//#define INI_FILE_NAME                 "filename"
#define INI_FILE_MAPPING_NAME         "filemapping"
#define INI_FILE_SIZE                 "filesize"
#define INI_P2REPL_SHARED_MEM         "p2ereplsharedmem"
#define INI_P2REPL_SHARED_MEM_INI     "p2ereplsharedmemini"
#define INI_REPL_TIMEOUT              "repltimeout"
#define INI_FREE_MEM_TIMEOUT          "freememtimeout"

#define INHERITED_EVENT_EREPL_STARTED            "ereplshmemstartedevent"
#define INHERITED_ESHARED_MEM_NAME               "eSharedMemory"
#define COMMIT_EVENT_ENV_VAR_NAME                "commitevent"
#define FREE_MEM_EVENT_ENV_VAR_NAME              "freememevent"
#define FILE_SIZE_ENV_VAR_NAME                   "filesize"
#define PID_ENV_VAR_NAME                         "processid"

#define DEFAULT_FILE_NAME             "P2eSharedMem.dat"
#define DEFAULT_FILE_MAPPING_NAME     "P2_E_SHARED_MEMORY"
#define DEFAULT_P2REPL_SHARED_MEM     "P2eReplSharedMem"
#define DEFAULT_P2REPL_SHARED_MEM_INI "P2eReplSharedMem.ini"
#define DEFAULT_REPL_TIMEOUT          60000
#define DEFAULT_FREE_MEM_TIMEOUT      60000

#define DEFAULT_MAX_TRANS_CNT         1
#define DEFAULT_MAX_TRANS_TIME        60000         

/****************!!!!!!!!!!!!****************
Размер файла должен быть не меньше, чем
(sizeof(TFileHeader) + размер самой длинной транзакции + размер самой толстой записи + EMERGENCY_BLOCK_SIZE)

Размер самой толстой записи нужно прибавлять из-за того, что возможен переход через конец файла,
если эта самая толстая запись не помещается в оставшееся место, и т.о. место в конце файла останется неиспользованным,
но будет приплюсовано к длине транзакции
*/


#pragma pack(push,4)

#define CURRENT_FILE_VERSION 1

namespace NS_P2eSharedMem {

struct TFileHeader
{
    UINT32    version;          // 0
    UINT32    fileSize;         // 4
    UINT32    headerSize;       // 8
    UINT32    replBeginOffset;  // 12
    UINT32    replEndOffset;    // 16
    LONG      waitForFreeMem;   // 20 
    struct TFileHeaderOutData
    {
        P2EREPL_REVISION_TYPE nextRev; // передается один раз при старте P2eReplSharedMem
    } outData;                  // 24
};

#define EMERGENCY_BLOCK_SIZE          4

/*#define NOT_INITED_MAGIC_WORD    ":-)"
#define TRANS_MAGIC_WORD         "t:)"
#define SAVE_TBL_MAGIC_WORD      "s:)"
#define SAVE_DB_MAGIC_WORD       "S:)"
#define NOP_MAGIC_WORD           "NOP" //no operation
*/

enum 
{
    P2OP_CODE_NOT_INITED = 0,
    P2OP_CODE_NO_OPERATION_1,
    P2OP_CODE_NO_OPERATION,
    P2OP_CODE_BEGIN_TRANSACTION,
    P2OP_CODE_COMMIT,
    //P2OP_CODE_SAVE_DB,
    //P2OP_CODE_SAVE_TBL,
    P2OP_CODE_POST_REC,
    //P2OP_CODE_FAST_DELETE_REC,
    P2OP_CODE_CLEAR_OLD_REV,
    //P2OP_CODE_CLEAR_DELETED,
    P2OP_CODE_COMMIT_WITH_EVENT
};

#define SHMEM_BLOCK_HEADER_MAGIC 0x00452938 // 'E)8'

struct TDataBlockHdr
{
    const UINT64    counter;
    const UINT32    opCode;
    UINT32          magic;

    explicit TDataBlockHdr(UINT64& _counter, UINT32 oper = P2OP_CODE_NOT_INITED) : counter(++_counter), opCode(oper)
    {
        magic = SHMEM_BLOCK_HEADER_MAGIC;
    }
private:
    TDataBlockHdr& operator =(const TDataBlockHdr& rhs);
};
/*
на размеры этого блока завязаны тесты, которые проверяют кольцевой буфер на вщивость. 
поэтому лучше его размер не менять, а если изменение необходимо, то согласовать с Аней

struct TOldDataBlockHdr
{
    const UINT32     opCode;
    const P2TIME     timestamp;
};
*/

struct TNoOperDataBlockHdr : public TDataBlockHdr
{
    UINT32    blockSize;

    explicit TNoOperDataBlockHdr(UINT64& _counter) : TDataBlockHdr(_counter, P2OP_CODE_NO_OPERATION), blockSize(0) { };

private:
    TNoOperDataBlockHdr& operator =(const TNoOperDataBlockHdr& rhs);
};

enum TRecOper
{
    roInsert = 0,
    //roUpdate,
    //roDelete,
    roCheck,
    //roFastDelete
};

struct TRecDataBlockHdr : public TDataBlockHdr
{
    UINT32      recSize;
    TRecOper    recOperType;

    explicit TRecDataBlockHdr(UINT64& _counter, TRecOper recOper, UINT32 oper = P2OP_CODE_POST_REC) : TDataBlockHdr(_counter, oper), recSize(0), recOperType(recOper) { };

private:
    TRecDataBlockHdr& operator =(const TRecDataBlockHdr& rhs);
};

struct TClearRevDataBlockHdr : public TDataBlockHdr
{
    P2EREPL_REVISION_TYPE   rev;
    UINT32                  flags;

    explicit TClearRevDataBlockHdr(UINT64& _counter, P2EREPL_REVISION_TYPE in_rev, UINT32 in_flags) : TDataBlockHdr(_counter, P2OP_CODE_CLEAR_OLD_REV), rev(in_rev), flags(in_flags) { };
private:
    TClearRevDataBlockHdr(); // конструктор по умолчанию не должен вызываться
    TClearRevDataBlockHdr& operator =(const TClearRevDataBlockHdr& rhs);
};

struct TCommitWithEventBlockHdr : public TDataBlockHdr
{
    UINT32                  length;
    explicit TCommitWithEventBlockHdr(UINT64& _counter, UINT32 in_length) : TDataBlockHdr(_counter, P2OP_CODE_COMMIT_WITH_EVENT), length(in_length) { };
private:
    TCommitWithEventBlockHdr(); // конструктор по умолчанию не должен вызываться
    TCommitWithEventBlockHdr& operator =(const TCommitWithEventBlockHdr& rhs);
};

#pragma pack(pop)

} // end of namespace

#endif // !defined(AFX_COMMON_H__57977908_3F40_4C81_A8CB_D34B1512A639__INCLUDED_)
