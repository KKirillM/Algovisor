#pragma once

#include "../../../Plaza-II-eRepl/P2eReplCommon/Include/P2eReplTypes.h"


#pragma pack(push, 1)

typedef struct
{
    UINT32 blockSignature;
    UINT32 blockComment;
	UINT64 counter;
	P2TIME t; // TODO FILETIME
	UINT32 dataSize;  
	UINT8 type;
} eFileBlock;

struct EventSt: public eFileBlock
{
	TEReplRec replEvent;
};

struct ClearOldRevsSt : public eFileBlock
{
	P2EREPL_REVISION_TYPE rev;
    UINT32 flags;
};

struct StreamOpenedSt : public eFileBlock
{
	UINT32 fileFormatVersion;
    UINT32 serverLifeNum;
	char streamName[1];
};

struct ExitSt : public eFileBlock
{
	UINT32 fileFormatVersion;
    P2EREPL_REVISION_TYPE revision;
	char strSequence[20];
};

#pragma pack(pop)


enum eFileBlockType
{
    FBT_STREAM_OPENED = 0,
	FBT_CLEAR_OLD_REVS,
	FBT_EVENT,
	FBT_BEGIN_TRANS,
	FBT_COMMIT_TRANS,
	FBT_EXIT
};
