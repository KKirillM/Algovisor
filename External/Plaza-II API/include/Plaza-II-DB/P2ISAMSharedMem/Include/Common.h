#if !defined(AFX_COMMON_H__6E728C1F_9756_4DF2_B54F_815C84D61C47__INCLUDED_)
#define AFX_COMMON_H__6E728C1F_9756_4DF2_B54F_815C84D61C47__INCLUDED_

#include "../../../Plaza-II-Repl/P2ReplUtils/Include/P2ReplComm.h"
#include "../../../Plaza-II/P2SysTypes/Include/P2SysTypes.hpp"
#include "../../../Plaza-II/P2SysScheme/Include/P2SysScheme.hpp"

#define INI_SETTINGS_SECTION          "p2sharedmem"
#define INI_FILE_NAME                 "filename"
#define INI_FILE_SIZE                 "filesize"
#define INI_P2REPL_SHARED_MEM         "p2replsharedmem"
#define INI_P2REPL_SHARED_MEM_INI     "p2replsharedmemini"
#define INI_REPL_TIMEOUT              "repltimeout"
#define INI_FREE_MEM_TIMEOUT          "freememtimeout"
#define INI_SAVE_TIMEOUT              "savetimeout"

#define INHERITED_EVENT_REPL_STARTED     "replshmemstartedevent"
#define COMMIT_EVENT_ENV_VAR_NAME        "commitevent"
#define FREE_MEM_EVENT_ENV_VAR_NAME      "freememevent"
#define FILE_SIZE_ENV_VAR_NAME           "filesize"
#define PID_ENV_VAR_NAME                 "processid"
#define INHERITED_SHARED_MEM_NAME		 "sharedmem"

#define DEFAULT_FILE_NAME             "P2ISAMSharedMem.dat"
#define DEFAULT_FILE_MAPPING_NAME     "P2ISAM_SHARED_MEMORY"
#define DEFAULT_P2REPL_SHARED_MEM     "P2ReplSharedMemD.exe"
#define DEFAULT_P2REPL_SHARED_MEM_INI "P2ReplSharedMem.ini"
#define DEFAULT_REPL_TIMEOUT          60000
#define DEFAULT_FREE_MEM_TIMEOUT      60000
#define DEFAULT_SAVE_TIMEOUT          60000

#define DEFAULT_MAX_TRANS_CNT         1
#define DEFAULT_MAX_TRANS_TIME        60000         

/****************!!!!!!!!!!!!****************
Размер файла должен быть не меньше, чем
(sizeof(TFileHeader) + (scheme block size))
+ размер самой длинной транзакции + размер самой толстой записи + EMERGENCY_BLOCK_SIZE)

Размер самой толстой записи нужно прибавлять из-за того, что возможен переход через конец файла,
если эта самая толстая запись не помещается в оставшееся место, и т.о. место в конце файла останется неиспользованным,
но будет приплюсовано к длине транзакции
*/


#pragma pack(push,4)

#define CURRENT_FILE_VERSION 8 

namespace NS_P2ISAMSharedMem {

struct TFileHeader
{
	UINT32    version;
	UINT32    fileSize;
	UINT32    headerSize;
	UINT32    replBeginOffset;
	UINT32    replEndOffset;
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
	P2OP_CODE_SAVE_DB,
	P2OP_CODE_SAVE_TBL,
	P2OP_CODE_POST_REC,
	P2OP_CODE_FAST_DELETE_REC,
	P2OP_CODE_CLEAR_OLD_REV,
	P2OP_CODE_CLEAR_DELETED,
	P2OP_CODE_COMMIT_WITH_EVENT
};

#define SHMEM_BLOCK_HEADER_MAGIC 0x2938 //')8'

struct TDataBlockHdr
{
	const UINT64	 counter;
    const UINT32     opCode;
	const UINT32	 magic;

	explicit TDataBlockHdr(UINT64& _counter, UINT32 oper = P2OP_CODE_NOT_INITED): 
		counter(++_counter), opCode(oper), magic(SHMEM_BLOCK_HEADER_MAGIC)
	{
	}
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

	explicit TNoOperDataBlockHdr(UINT64& _counter) : TDataBlockHdr(_counter, P2OP_CODE_NO_OPERATION), blockSize(0) {};
};

struct TTblDataBlockHdr : public TDataBlockHdr
{
    INT32    tableIndex;

	explicit TTblDataBlockHdr(UINT64& _counter, UINT32 oper) : TDataBlockHdr(_counter, oper), tableIndex(-1) {};
private:
	TTblDataBlockHdr(); // конструктор по умолчанию не должен вызываться
};

enum TRecOper
{
	roInsert = 0,
	roUpdate,
	roDelete,
	roCheck,
	roFastDelete
};

struct TRecDataBlockHdr : public TTblDataBlockHdr
{
    UINT32      recSize;
	TRecOper    recOperType;

	explicit TRecDataBlockHdr(UINT64& _counter, TRecOper recOper, UINT32 oper = P2OP_CODE_POST_REC) : TTblDataBlockHdr(_counter, oper), recSize(0), recOperType(recOper) {};
};

struct TFastDelRecBlockHdr : public TRecDataBlockHdr
{
	P2REPL_ID_TYPE          id;
	P2REPL_REVISION_TYPE    rev;
		
	TFastDelRecBlockHdr(UINT64& _counter) : TRecDataBlockHdr(_counter, roFastDelete, P2OP_CODE_FAST_DELETE_REC), id(P2REPL_MIN_REVISION), rev(P2REPL_MIN_REVISION) {};
};

struct TRecReplHdr
{
	P2REPL_ID_TYPE			id;
	P2REPL_REVISION_TYPE	rev;
	P2REPL_DELETED_TYPE		deleted;
};

struct TClearRevDataBlockHdr : public TTblDataBlockHdr
{
    P2REPL_REVISION_TYPE    rev;

	explicit TClearRevDataBlockHdr(UINT64& _counter, INT32 in_tableIndex, P2REPL_REVISION_TYPE in_rev) : TTblDataBlockHdr(_counter, P2OP_CODE_CLEAR_OLD_REV), rev(in_rev) {tableIndex = in_tableIndex;};
private:
	TClearRevDataBlockHdr(); // конструктор по умолчанию не должен вызываться
};

struct TCommitWithEventBlockHdr : public TDataBlockHdr
{
	UINT32					length;
	explicit TCommitWithEventBlockHdr(UINT64& _counter, UINT32 in_length) : TDataBlockHdr(_counter, P2OP_CODE_COMMIT_WITH_EVENT), length(in_length){};
private:
	TCommitWithEventBlockHdr(); // конструктор по умолчанию не должен вызываться
};

#pragma pack(pop)


/*inline P2ERR CheckDbSchemeInFile(P2DB_SCHEME_HANDLE hDbScheme, BYTE* fileData, UINT32 fileDataSize, PCSTR logCategory = "P2ISAMSM")
{
	P2ERR res = P2ERR_OK;
	UINT32 tblCount = 0;
    if (P2FAILEDV(P2SchemeGetTableCount(hDbScheme, &tblCount), res))
	{
		P2TRACE_ERROR_WITH_CODE_EX((res, logCategory, "Cannot get table count from scheme, scheme 0x%x", hDbScheme));
		return res;
	}

	BYTE* curPtr = fileData;
	BYTE* endPtr = curPtr + fileDataSize;

	for(UINT32 i = 0; i < tblCount; i++)
	{
		PCSTR tblNameInFile = reinterpret_cast<PCSTR>(curPtr);
		curPtr += strlen(tblNameInFile)+1;
		UINT32 recSizeInFile = *reinterpret_cast<UINT32*>(curPtr);
        curPtr += sizeof(UINT32);

		if (curPtr > endPtr)
		{
            P2TRACE_ERROR_WITH_CODE_EX((P2ERR_ISAMSM_SCHEME_MISMATCH, logCategory, "End of scheme in file, current tbl index %d, total count of tables %d", i, tblCount));
			return P2ERR_ISAMSM_SCHEME_MISMATCH;
		}
			
		AutoTableSchemeHandle hTblSchemeA;
		if (P2FAILEDV(P2SchemeGetTableByIndex(hDbScheme, i, &hTblSchemeA), res))
		{
            P2TRACE_ERROR_WITH_CODE_EX((res, logCategory, "Cannot get table scheme, idx %d", i));
			return res;
		}

		char tblName[P2DB_MAX_TABLE_NAME_SIZE];
		UINT32 tblNameSize = sizeof(tblName);
		if (P2FAILEDV(P2SchemeGetTableName(hTblSchemeA(), tblName, &tblNameSize), res))
		{
			P2TRACE_ERROR_WITH_CODE_EX((res, logCategory, "Cannot get table name, idx %d", i));
			return res;
		}
		
		UINT32 structSize = 0;
		if (P2FAILEDV(P2RecordGetStructSize(hTblSchemeA(), &structSize), res))
		{
            P2TRACE_ERROR_WITH_CODE_EX((res, logCategory, "Cannot get record size, table \'%s\'", tblName));
			return res;
		}

		if ((stricmp(tblNameInFile, tblName) != 0) || (recSizeInFile != structSize)) 
		{
			P2TRACE_ERROR_WITH_CODE_EX((P2ERR_ISAMSM_SCHEME_MISMATCH, logCategory, "Scheme mismatch, table name in file %s, table name in scheme %s, record size in file %d, record size in scheme %d", tblNameInFile, tblName, recSizeInFile, structSize));
			return P2ERR_ISAMSM_SCHEME_MISMATCH;
		}
	}
	curPtr += 1; //trailing 0
	if (curPtr != endPtr)
	{
        P2TRACE_ERROR_WITH_CODE_EX((P2ERR_ISAMSM_SCHEME_MISMATCH, logCategory, "Some data from scheme in file is absent in db scheme"));
		return P2ERR_ISAMSM_SCHEME_MISMATCH;
	}

	return P2ERR_OK;
}*/

} // end of namespace

#endif // !defined(AFX_COMMON_H__6E728C1F_9756_4DF2_B54F_815C84D61C47__INCLUDED_)
