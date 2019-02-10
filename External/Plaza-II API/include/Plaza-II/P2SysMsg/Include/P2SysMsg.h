#ifndef _P2SYSMSG_H_
#define _P2SYSMSG_H_

#include "../../P2Common/Include/P2Types.h"
#include "../../P2Common/Include/P2Errors.h"
#include "../../P2SysParser/Include/P2SysBinParser.h"


#ifdef  __cplusplus
extern "C" {
#endif

#ifdef P2SYS_EXPORTS
#define P2SYSMSG_API P2_API_EXPORTS
#else
#define P2SYSMSG_API P2_API_IMPORTS
#endif

//////////////////////////////////////////////////////////////////////
// std fields

// �� ���� ���������. ��������� ����
// ���: ������
#define P2MSG_PROP_FROM "P2_From"
// ���� ���������. ��������� ����
// ���: ������
#define P2MSG_PROP_TO "P2_To"
// ���: UINT32
#define P2MSG_PROP_TYPE "P2_Type"
// ���: P2TIME
#define P2MSG_PROP_TIME "P2_Time"
// ID ��� ������� �����
// ���: 8 ����
#define P2MSG_PROP_SEND_ID "P2_SendId"
// ID ������������� ���������
// ���: 8 ����
#define P2MSG_PROP_REPLY_ID "P2_ReplyId"
// ��������� ���������
// ���: ������
#define P2MSG_PROP_CATEGORY "P2_Category"
// ����. ���������� �����.
#define P2MSG_PROP_BODY "P2_Body"
// ������������ ���� � ������ �������. ������ ��� ��������� �� �������, ������ - � �������
// ���: ������
#define P2MSG_PROP_SERVICE_FROM "P2_Service_From"
#define P2MSG_PROP_SERVICE_TO "P2_Service_To"
// ��������� ���������
// ���: UINT8
#define P2MSG_PROP_PRIORITY "P2_Priority"
// ��������� ����� ���������
// ���: UINT32
#define P2MSG_PROP_FLAGS "P2_Flags" 
// ��� ����������� ���� �2 ����� �������.
#define	P2MSG_P2PREFIX  "P2_"

//////////////////////////////////////////////////////////////////////
// ��������� ���������

#define P2MSG_CATEGORY_DEFAULT ""
#define P2MSG_CATEGORY_SYSTEM "P2_System"

//////////////////////////////////////////////////////////////////////
// ���������� ���������

enum
{
    P2MSG_PRIORITY_DEFAULT      = 0,    // ������-���������, ���������� �� P2MSG_PRIORITY_NORMAL
    P2MSG_PRIORITY_LOW          = 2,
    P2MSG_PRIORITY_NORMAL       = 5,    // ������� ���������
    P2MSG_PRIORITY_HIGH         = 8,
    P2MSG_PRIORITY_VERY_HIGH    = 11,
    P2MSG_PRIORITY_SYSTEM       = 13    // ������ ��� ��������� ���������!
};

#define P2MSG_PRIORITY_MIN P2MSG_PRIORITY_LOW
#define P2MSG_PRIORITY_MAX P2MSG_PRIORITY_SYSTEM

//////////////////////////////////////////////////////////////////////
// ��������� ����� ���������

// ��������� ��������������� �������� ���������
#define P2MSG_FLAG_GRNT                 0x00000001
// ��������� ������������� � ��������������� ��������
// (��-��������� ������������� �� ������������)
#define P2MSG_FLAG_GRNT_REQ_RECEIPT     0x00000002

//////////////////////////////////////////////////////////////////////

struct P2Message;
typedef struct P2Message *P2MSG_HANDLE;

// ����� ��������� �� �����
// 1. pbuf - ������� �����, � ptr - ���������.


// �������� ��������� �� ����� ������ ���������
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCreateMessage(P2MSG_HANDLE *ppmsg);
// ���������� ������, ������� ��� ���������
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgFreeMessage(P2MSG_HANDLE pmsg);

// ������� ��������� �� �������� �������������
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCreateMessageFromBlock(const void *pmem, size_t size, P2MSG_HANDLE *ppmsg, size_t *pRealSize);
// ��������� ��������� � ����� ������
//TODO: size!!!
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSaveMessageToBlock(P2MSG_HANDLE pmsg, void *pmem, size_t size);

// �������� ������ ����� ���������
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgGetMessageBlockSize(P2MSG_HANDLE pmsg, size_t *psize);


P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSaveMessageToComposer(P2_ComposeState *pState, P2MSG_HANDLE pmsg);
//��������� ��������� � �������� ����� ���������� ������ ��������� � �����
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSaveMessageToComposerWithLength(P2_ComposeState *pState, P2MSG_HANDLE pmsg);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCreateMessageFromParser(P2_ParseState *pState, P2MSG_HANDLE* ppmsg);


// ������ � ������:
// 1. � ����� ���� ��������� �����. 
// 2. ���������������� ����� ������� ����. �������� ����������� �����, ������������ �� P2_ ������!
// 3. ��������� ��������� ���� ����������� ����.
// 4. �������� ���� ����� ���� ��������� ��� ��������. � ������ ������ ������������ ������� Str � ����� �������,
//	  ��-������ ����� ���������� ����������.

// ���������� �������� ���� �� ������ �� �����
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetField(P2MSG_HANDLE pmsg, const char *field_name, const void *pbuf, size_t size);
// ���������� ��������� ���� �� ������ (����� �������������� ��� strlen()+1)
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetFieldStr(P2MSG_HANDLE pmsg, const char *field_name, const char *pstr);
// ���������� �������� ���� 
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetFieldUINT8(P2MSG_HANDLE pmsg, const char *field_name, UINT8 num);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetFieldINT8(P2MSG_HANDLE pmsg, const char *field_name, INT8 num);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetFieldUINT16(P2MSG_HANDLE pmsg, const char *field_name, UINT16 num);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetFieldINT16(P2MSG_HANDLE pmsg, const char *field_name, INT16 num);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetFieldUINT32(P2MSG_HANDLE pmsg, const char *field_name, UINT32 num);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetFieldINT32(P2MSG_HANDLE pmsg, const char *field_name, INT32 num);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetFieldUINT64(P2MSG_HANDLE pmsg, const char *field_name, UINT64 num);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetFieldINT64(P2MSG_HANDLE pmsg, const char *field_name, INT64 num);


// �������� ����������� ��������� �� ���������� ����� � �����. ������ ��� �������� ������!!!
// ������������ ������ ��� �������� ������. ��� ��������� ������ ������������ P2MsgGetStrField
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgGetBinaryField(P2MSG_HANDLE pmsg, const char *field_name, const void **pptr, /*[out]*/size_t *psize);

// FIXME ��������� ��� ������������� �� ������� ��������. ������� ������������ 0 �� ����������� !!  
// ����� �������
#define P2MsgGetFieldStr(pmsg, field_name, pstr) P2MsgGetBinaryField((pmsg), (field_name), ((const void **)pstr), NULL)
// �������� ��������� �� ���������� ������. ��������� ������������ ������.  
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgGetStrField(P2MSG_HANDLE pmsg, const char *field_name, const char **pptr);

#define P2MsgGetFieldSize(pmsg, field_name, psize) P2MsgGetBinaryField((pmsg), (field_name), NULL, (psize))

// ����������� ���� � ���������� ������������� �����.
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyField(P2MSG_HANDLE pmsg, const char *field_name, void *pbuf, /*[in,out]*/ size_t *psize);
// �������� �������� ����
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldUINT8(P2MSG_HANDLE pmsg, const char *field_name, UINT8 *pnum);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldINT8(P2MSG_HANDLE pmsg, const char *field_name, INT8 *pnum);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldUINT16(P2MSG_HANDLE pmsg, const char *field_name, UINT16 *pnum);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldINT16(P2MSG_HANDLE pmsg, const char *field_name, INT16 *pnum);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldUINT32(P2MSG_HANDLE pmsg, const char *field_name, UINT32 *pnum);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldINT32(P2MSG_HANDLE pmsg, const char *field_name, INT32 *pnum);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldUINT64(P2MSG_HANDLE pmsg, const char *field_name, UINT64 *pnum);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldINT64(P2MSG_HANDLE pmsg, const char *field_name, INT64 *pnum);



// �-�� ��� ���������/������ ����������� ����� � P2SYS_MSG. 
// �������� �������, ��� ��������� ���� ����� �� ����� ����� ����� ��������� P2MsgSetField***.  

// ���������� ���� 
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetBody(P2MSG_HANDLE pmsg, const void *pbuf, size_t size); 
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetBodyStr(P2MSG_HANDLE pmsg, const void *pstr); 
// ��������� �����
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetAddrTo(P2MSG_HANDLE pmsg, const char * to); 
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetAddrFrom(P2MSG_HANDLE pmsg, const char *from); 
// ���������� ���������
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetCategory(P2MSG_HANDLE pmsg, const char *category); 
// ���������� ���
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetType(P2MSG_HANDLE pmsg, UINT32 type); 
// ���������� id  ���������
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetSendId(P2MSG_HANDLE pmsg, UINT64 id);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetReplyId(P2MSG_HANDLE pmsg, UINT64 id);

// �������� ����������� ���� ���������

P2SYSMSG_API P2ERR P2CALLSPEC P2MsgGetBody(P2MSG_HANDLE pmsg, const void **pptr, size_t *pSize); 
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgGetBodySize(P2MSG_HANDLE pmsg, size_t *pSize); 
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgGetReplyId(P2MSG_HANDLE pmsg, UINT64 *pId); 
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgGetCategory(P2MSG_HANDLE pmsg, const char** ppCategory);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgGetAddrTo(P2MSG_HANDLE pmsg, const char** ppAddrTo);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgGetType(P2MSG_HANDLE pmsg, UINT32* pType);

//  �-�� ��� ��������� ����� ���������.  

// �������� ����������� ��������� �� ���������� ����� � ����� ���������. ������ ��� �������� ������!!!
// ������� ����� ��������� ���������
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyMessage(P2MSG_HANDLE *pmsg_cpy, const P2MSG_HANDLE pmsg_src );
// ����������� ���� � ���������� ������������� �����
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyBody(const P2MSG_HANDLE pmsg, void *pbuf, size_t *psize);

// ���������� �������� ���� �� ��������� �������������.
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldFromBlock(const UINT8 *pbinmsg, size_t size, const char *field_name, void *ptr, /*[in,out]*/size_t *psize);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldFromBlockUINT8(const UINT8 *pbinmsg, size_t size, const char *field_name, UINT8 *pnum);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldFromBlockINT8(const UINT8 *pbinmsg, size_t size, const char *field_name, INT8 *pnum);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldFromBlockUINT16(const UINT8 *pbinmsg, size_t size, const char *field_name, UINT16 *pnum);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldFromBlockINT16(const UINT8 *pbinmsg, size_t size, const char *field_name, INT16 *pnum);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldFromBlockUINT32(const UINT8 *pbinmsg, size_t size, const char *field_name, UINT32 *pnum);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldFromBlockINT32(const UINT8 *pbinmsg, size_t size, const char *field_name, INT32 *pnum);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldFromBlockUINT64(const UINT8 *pbinmsg, size_t size, const char *field_name, UINT64 *pnum);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldFromBlockINT64(const UINT8 *pbinmsg, size_t size, const char *field_name, INT64 *pnum);

// �������� ����������� ���� �� ���. ������������� (����� ���������� ����)
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgGetStandartFieldsFromBlock(	const UINT8* pMsgBlock, size_t size,
																UINT32* pType,   				//P2MSG_PROP_TYPE
																UINT64* pSendId, 				//P2MSG_PROP_SEND_ID,
																UINT64* pReplyId,				//P2MSG_PROP_REPLY_ID
																const char** ppAddrFrom, 		//P2MSG_PROP_FROM,
																const char** ppAddrTo,			//P2MSG_PROP_TO
																const char** ppCategory,  		//P2MSG_PROP_CATEGORY
																const void** ppBody,		 	//P2MSG_PROP_BODY 
																size_t*      pBodySz);


P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetStandartFieldToBlock(UINT8* pMsgBlock, 
															size_t		 size,
															const char*  pFieldName,
															const void*  pValue,
															size_t		 szValue);
															  
#if defined(_MSC_VER) && defined(_DEBUG)
#include <crtdbg.h>
	P2SYSMSG_API void P2CALLSPEC P2MsgMemCheckpoint(_CrtMemState *state);
	P2SYSMSG_API int P2CALLSPEC P2MsgMemDiff(_CrtMemState *stateDiff, const _CrtMemState *oldState, const _CrtMemState *newState);
	P2SYSMSG_API void P2CALLSPEC P2MsgMemDump(_CrtMemState *state);
#endif // defined(_MSC_VER) && defined(_DEBUG)

P2SYSMSG_API P2ERR P2CALLSPEC P2MsgParseBodyGetPointer(P2MSG_HANDLE msg, const char *format, ...);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgParseBodyCopy(P2MSG_HANDLE msg, const char *format, ...);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgParseBodyCopyV(P2MSG_HANDLE msg, const char *format, va_list args);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgComposeBody(P2MSG_HANDLE msg, const char *format, ...);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgComposeBodyV(P2MSG_HANDLE msg, const char *format, va_list args);

P2SYSMSG_API P2ERR P2CALLSPEC P2MsgDuplicateMessageHandle(P2MSG_HANDLE src, P2MSG_HANDLE* pDest);


// ���������������� ������ � ������ ��������� 
#define	P2MSG_PROP_INDEX_TYPE 			0
#define	P2MSG_PROP_INDEX_SEND_ID		1
#define	P2MSG_PROP_INDEX_REPLY_ID		2
#define	P2MSG_PROP_INDEX_ADDR_FROM		3
#define	P2MSG_PROP_INDEX_ADDR_TO		4
#define	P2MSG_PROP_INDEX_BODY			5
#define	P2MSG_PROP_INDEX_CATEGORY		6
#define	P2MSG_PROP_INDEX_LAST			7

// ��������� ��� ���������������� ������ � ������ P2SYS_MSG. ��� ���������� ����� ������������ �-�� P2MsgFieldCreate �� P2MsgField.hpp.  
typedef struct _P2MSG_FIELD_ 
{
	union
	{
		// ���� 0 <= name < P2MSG_PROP_INDEX_LAST, �� �������������� ��� ������ ������������ ����
		UINT32 		index;			
		// ����� name �������������� ��� ������, ���������� ��� ����.
		const char* str;			
	} name;

	UINT32 		len; 				
	const void* data;

} P2MSG_FIELD; 

// � �-��� P2MsgField*** ������ fields �� ������ ��������� ���� ����� � ���������� ������.

// �������� ������ ������, � ������� ����� ��������� P2SYS_MSG, ���������� ���� fields.
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgFieldGetMessageBlockSize(P2MSG_FIELD* fields, size_t fieldsCount, size_t *pSize); 
// ��������� � �����. ��������� ��������� ���������� P2SYS_MSG, ����������� ���� fields.
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgFieldSaveMessageToBlock(P2MSG_FIELD* fields, size_t fieldsCount, void *pMem, size_t size); 



#ifdef  __cplusplus
}

#endif


#endif // _P2SYSMSG_H_
