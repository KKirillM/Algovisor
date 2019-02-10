#ifndef _P2SYSBINPARSER_H_
#define _P2SYSBINPARSER_H_

#include <stdarg.h>

#include "P2SysParser.h"

#ifdef  __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////// 
#define	NULL_POOL_OFFSET 0xFFFFFFFF 
////////////////////////////////////////////////////////////////////////////////
enum ParseMode
{
	PARSE_SMART,
	PARSE_GET_POINTER,
	PARSE_COPY,
	PARSE_ALLOC_COPY,
	PARSE_PLAIN_COPY
};

//
// ����� ������� ��� ����������� ������. ������������� ������������ ������������ � ������� ����
/*
���������� ����:
i1 INT8
i2 INT16
i4 INT32
i8 INT64
u1 UINT8
u2 UINT16
u4 UINT32
u8 UINT64
c[size] ������ size = strlen(str) + 1
b ���� ������ ptr, size

dN.M  UBCD
sN.M  SBCD

t - P2TIME
*/

// �������� ��������� ��� ���������� ���������
typedef struct{
	// ��������� �� ���� ����������� ������
	// ���� ���������� � ������������� ������ ����� ������� API!
	void * buf;
	// ������ �����
	size_t size;
	// ���������� ����������. �� ��������!
	size_t alloc_size;
	// 
	char dontCreateReally;
	size_t alignment;
}P2_ComposeState;

// �������� ��������� ��� ������������ ���������
typedef struct{
	// ���������� ����������. �� ��������!
	const void * buf;
	// ���������� ����������. �� ��������!
	size_t size;
	// ���������� ����������. �� ��������!
	size_t curIndex;
	size_t alignment;
}P2_ParseState;

// ������������� ���������
P2SYSPARSER_API P2ERR P2CALLSPEC P2MsgInitCompose(P2_ComposeState *pState);
P2SYSPARSER_API P2ERR P2CALLSPEC P2MsgInitComposeForAppend(P2_ComposeState *pState, const void *p, size_t size);
P2SYSPARSER_API P2ERR P2CALLSPEC P2MsgInitComposeForAppend2(P2_ComposeState *pState, const void* p, size_t size,size_t szPreallocateHint);
// �������� ��������� ����� ������
P2SYSPARSER_API P2ERR P2CALLSPEC P2MsgCompose(P2_ComposeState *pState, const char *format, ...);
// ���������� (����� ���������� ;-)) ������� ������ �� ������� � �������� � ��������
P2SYSPARSER_API P2ERR P2CALLSPEC P2MsgComposeFromParser(P2_ComposeState *pStateComp, P2_ParseState *pStateParse);
// ��������������� ���������
P2SYSPARSER_API P2ERR P2CALLSPEC P2MsgFreeCompose(P2_ComposeState *pState);
P2SYSPARSER_API P2ERR P2CALLSPEC P2MsgResetCompose(P2_ComposeState *pState);
P2SYSPARSER_API P2ERR P2CALLSPEC P2MsgResetComposeForAppend(P2_ComposeState* pState,const void* pBuf,size_t size);

typedef void * (P2CALLSPEC *pfnAlloc)(size_t size);

// ������������� ���������
P2SYSPARSER_API P2ERR P2CALLSPEC P2MsgInitParse(P2_ParseState *pState, const void *p, size_t size);
// ��������������� ��������� 
P2SYSPARSER_API P2ERR P2CALLSPEC P2MsgResetParse(P2_ParseState *pState);
// ����� ��������� ����� ������
// ������� � ����������� ���������� �� ������������ �����
P2SYSPARSER_API P2ERR P2CALLSPEC P2MsgParseGetPointer(P2_ParseState *pState, const char *format, ...);
// ������� � ������������ � �������������� �����
P2SYSPARSER_API P2ERR P2CALLSPEC P2MsgParseCopy(P2_ParseState *pState, const char *format, ...);
// ������� � ���������� ������ �����������
P2SYSPARSER_API P2ERR P2CALLSPEC P2MsgParseAllocCopy(P2_ParseState *pState, pfnAlloc alloc, const char *format, ...);

// ������������� ������. ������ ������ ����������-���������� ������������ �� ����������
P2SYSPARSER_API P2ERR P2CALLSPEC P2MsgParse(P2_ParseState *pState, pfnAlloc alloc, const char *format, ...);
// ��������������� ���������
P2SYSPARSER_API P2ERR P2CALLSPEC P2MsgFreeParse(P2_ParseState *pState);

// �������� ������� �������
P2SYSPARSER_API P2ERR P2CALLSPEC P2MsgParseGetCurrentIndex(P2_ParseState *pState, size_t *pCurIndex);
// ���������� ������� �������
P2SYSPARSER_API P2ERR P2CALLSPEC P2MsgParseSetCurrentIndex(P2_ParseState *pState, size_t curIndex);

// ��������� ��������� �� ���� �� �����
#define P2MsgParseIsFinished(pState) ((pState)->curIndex >= (pState)->size)

#ifdef  __cplusplus

class P2MsgComposer : public P2_ComposeState
{
public:
	P2MsgComposer()
	{ 
		err = P2MsgInitCompose(this); 
	}
	P2MsgComposer(const void* p, size_t size)
	{ 
		err = P2MsgInitComposeForAppend(this, p, size); 
	}
	P2MsgComposer(const void* p, size_t size, size_t szPreallocateHint)
	{
		err = P2MsgInitComposeForAppend2(this,p,size,szPreallocateHint);
	}

	~P2MsgComposer(){ P2MsgFreeCompose(this); }

	P2ERR err;
};

class P2MsgParser : public P2_ParseState
{
public:
    P2MsgParser(const void *p, size_t size){ err = P2MsgInitParse(this, p, size); }
	~P2MsgParser(){P2MsgFreeParse(this);}

	P2ERR err;
};

#endif

P2SYSPARSER_API P2ERR P2MsgVParse(P2_ParseState *pState, enum ParseMode mode, pfnAlloc alloc, const char *format, va_list args);
P2SYSPARSER_API P2ERR P2MsgVCompose(P2_ComposeState *pState, const char *format, va_list args);

#ifdef  __cplusplus
}

#endif


#endif
