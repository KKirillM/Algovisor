#ifndef _REPLLRPCQLINK_H__INCLUDED_
#define _REPLLRPCQLINK_H__INCLUDED_

#include "../../../Plaza-II/P2Common/Include/P2Windows.h"
#include "../../../Plaza-II/P2Common/Include/P2Types.h"
#include "../../../Plaza-II-Repl/P2ReplUtils/Include/P2ReplComm.h"
#include "../../../Plaza-II-eRepl/P2eReplCommon/Include/P2eReplProtocol.h"

typedef enum
{
    // стандартная реплика
    P2ReplLinkField_EndMarker = 0,
    P2ReplLinkField_AddrTo = 1,
    P2ReplLinkField_Type = 2,
    P2ReplLinkField_Body = 3,
    P2ReplLinkField_ChannelInfo = 4,
    P2ReplLinkField_DataInfo = 5,
    P2ReplLinkField_DataInfo2 = 6,
    P2ReplLinkField_NullFields = 7,
    P2ReplLinkField_RedirectAddr = 8,
    P2ReplLinkField_Version = 9,
    P2ReplLinkField_ChannelId = 10,
    P2ReplLinkField_SessionId = 11,
    P2ReplLinkField_SequenceId = 12,
    P2ReplLinkField_TokenId = 13,
	P2ReplLinkField_Flags = 14,
    // 15-19 reserved
    // event-реплика
    P2ReplLinkField_eVersion = 20,
    P2ReplLinkField_eChannelInfo = 21,
    P2ReplLinkField_eDataInfo = 22,
    P2ReplLinkField_eFlags = 23,
    P2ReplLinkField_eRedirectAddr = 24,
    P2ReplLinkField_eTokenId = 25
} EReplLinkFieldId;


enum 
{	
	// диапазон EReplLinkFieldId для стандартной реплики ( должен начинаться с 0)
	// должен соответствовать EReplLinkFieldId
	P2ReplLinkField_Max_ReplField = P2ReplLinkField_Flags 
};

enum 
{
	// диапазон EReplLinkFieldId для e-реплики 
	// должен соответствовать EReplLinkFieldId
	P2ReplLinkField_Min_eReplField = P2ReplLinkField_eVersion,
	P2ReplLinkField_Max_eReplField = P2ReplLinkField_eTokenId
};

typedef struct
{
    UINT16 fieldId; // EReplLinkFieldId
    const char *fieldName;
    size_t fieldNameSize; // including zero
    UINT32 fieldDataSize;
} TReplKnownField;

#define FIELD_LEN_IS_VARIABLE ((UINT32)~0)

const TReplKnownField knownFieldsInfo[] = {
    // Порядок и количество элементов в EReplLinkFieldId и здесь должны совпадать!
    // стандартная реплика
    { P2ReplLinkField_EndMarker,    NULL,                           0,                                  0 },
    { P2ReplLinkField_AddrTo,       P2MSG_PROP_TO,                  sizeof(P2MSG_PROP_TO),              FIELD_LEN_IS_VARIABLE }, // 1 // Должно быть первым в "переменном блоке"
    { P2ReplLinkField_Type,         P2MSG_PROP_TYPE,                sizeof(P2MSG_PROP_TYPE),            sizeof(UINT32) },
    { P2ReplLinkField_Body,         P2MSG_PROP_BODY,                sizeof(P2MSG_PROP_BODY),            FIELD_LEN_IS_VARIABLE }, // 3
    { P2ReplLinkField_ChannelInfo,  P2REPL_PROP_CHANNEL_INFO,       sizeof(P2REPL_PROP_CHANNEL_INFO),   sizeof(P2REPL_CHANNEL_INFO) },
    { P2ReplLinkField_DataInfo,     P2REPL_PROP_DATA_INFO,          sizeof(P2REPL_PROP_DATA_INFO),      FIELD_LEN_IS_VARIABLE }, // 5
    { P2ReplLinkField_DataInfo2,    P2REPL_PROP_DATA_INFO_2,        sizeof(P2REPL_PROP_DATA_INFO_2),    FIELD_LEN_IS_VARIABLE },
    { P2ReplLinkField_NullFields,   P2REPL_PROP_NULL_FIELDS,        sizeof(P2REPL_PROP_NULL_FIELDS),    FIELD_LEN_IS_VARIABLE }, // 7
    { P2ReplLinkField_RedirectAddr, P2REPL_PROP_REDIRECT_ADDRESS,   sizeof(P2REPL_PROP_REDIRECT_ADDRESS), FIELD_LEN_IS_VARIABLE },
    { P2ReplLinkField_Version,      P2REPL_PROP_VERSION,            sizeof(P2REPL_PROP_VERSION),        sizeof(UINT32) },        // 9
    { P2ReplLinkField_ChannelId,    P2REPL_PROP_CHANNEL_ID,         sizeof(P2REPL_PROP_CHANNEL_ID),     sizeof(UINT32) },
    { P2ReplLinkField_SessionId,    P2REPL_PROP_SESSION_ID,         sizeof(P2REPL_PROP_SESSION_ID),     sizeof(UINT32) },        // 11
    { P2ReplLinkField_SequenceId,   P2REPL_PROP_ORDER_COUNT,        sizeof(P2REPL_PROP_ORDER_COUNT),    sizeof(UINT32) },
    { P2ReplLinkField_TokenId,      P2REPL_PROP_TOKEN,              sizeof(P2REPL_PROP_TOKEN),          FIELD_LEN_IS_VARIABLE }, // 13
    { P2ReplLinkField_Flags,		P2REPL_PROP_START_FLAGS,        sizeof(P2REPL_PROP_START_FLAGS),    sizeof(UINT32) },		 // 14 
	// 15-19 reserved
    { P2ReplLinkField_EndMarker,    NULL,                           0,                                  0 },                     // 15 reserved
    { P2ReplLinkField_EndMarker,    NULL,                           0,                                  0 },                     // 16 reserved
    { P2ReplLinkField_EndMarker,    NULL,                           0,                                  0 },                     // 17 reserved
    { P2ReplLinkField_EndMarker,    NULL,                           0,                                  0 },                     // 18 reserved
    { P2ReplLinkField_EndMarker,    NULL,                           0,                                  0 },                     // 19 reserved
    // event-реплика
    { P2ReplLinkField_eVersion,     P2EREPL_PROP_VERSION,           sizeof(P2EREPL_PROP_VERSION),       sizeof(UINT32) },        // 20
    { P2ReplLinkField_eChannelInfo, P2EREPL_PROP_CHANNEL_INFO,      sizeof(P2EREPL_PROP_CHANNEL_INFO),  sizeof(P2EREPL_CHANNEL_INFO) },
    { P2ReplLinkField_eDataInfo,    P2EREPL_PROP_DATA_INFO,         sizeof(P2EREPL_PROP_DATA_INFO),     sizeof(P2EREPL_DATA_INFO) }, // 22
    { P2ReplLinkField_eFlags,       P2EREPL_PROP_FLAGS,             sizeof(P2EREPL_PROP_FLAGS),         sizeof(UINT32) },
    { P2ReplLinkField_eRedirectAddr, P2EREPL_PROP_REDIRECT_ADDRESS, sizeof(P2EREPL_PROP_REDIRECT_ADDRESS), FIELD_LEN_IS_VARIABLE }, // 24
    { P2ReplLinkField_eTokenId,     P2EREPL_PROP_TOKEN,             sizeof(P2EREPL_PROP_TOKEN),         FIELD_LEN_IS_VARIABLE }     // 25
};

#define REPL_LINK_NAMED_FIELD           (static_cast< UINT16 >(~0))

#pragma pack(push, 1)
typedef struct 
{
    UINT16 IdOrFlag; // EReplLinkFieldId or predefined flag value
    union
    {
        UINT8 constSizeData[1];
        P2ZBLOCK variableSizeData[1];
    };
} TReplLinkField;
#pragma pack(pop)

#define P2REPL_LRPCQ_LINK_NAME_PREFIX "P2Repl_FastLink_" // + NodeName

#endif // _REPLLRPCQLINK_H__INCLUDED_
