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

// от кого сообщение. текстовое поле
// тип: строка
#define P2MSG_PROP_FROM "P2_From"
// кому сообщение. текстовое поле
// тип: строка
#define P2MSG_PROP_TO "P2_To"
// тип: UINT32
#define P2MSG_PROP_TYPE "P2_Type"
// тип: P2TIME
#define P2MSG_PROP_TIME "P2_Time"
// ID для посылки сенда
// тип: 8 байт
#define P2MSG_PROP_SEND_ID "P2_SendId"
// ID оригинального сообщения
// тип: 8 байт
#define P2MSG_PROP_REPLY_ID "P2_ReplyId"
// категория сообщения
// тип: строка
#define P2MSG_PROP_CATEGORY "P2_Category"
// тело. переменной длины.
#define P2MSG_PROP_BODY "P2_Body"
// опциональные поля с именем сервиса. первое для сообщений от сервиса, второе - к сервису
// тип: строка
#define P2MSG_PROP_SERVICE_FROM "P2_Service_From"
#define P2MSG_PROP_SERVICE_TO "P2_Service_To"
// приоритет сообщения
// тип: UINT8
#define P2MSG_PROP_PRIORITY "P2_Priority"
// системные флаги сообщения
// тип: UINT32
#define P2MSG_PROP_FLAGS "P2_Flags" 
// все стандартные поля П2 имеют префикс.
#define	P2MSG_P2PREFIX  "P2_"

//////////////////////////////////////////////////////////////////////
// категории сообщений

#define P2MSG_CATEGORY_DEFAULT ""
#define P2MSG_CATEGORY_SYSTEM "P2_System"

//////////////////////////////////////////////////////////////////////
// приоритеты сообщений

enum
{
    P2MSG_PRIORITY_DEFAULT      = 0,    // Псевдо-приоритет, заменяется на P2MSG_PRIORITY_NORMAL
    P2MSG_PRIORITY_LOW          = 2,
    P2MSG_PRIORITY_NORMAL       = 5,    // Базовый приоритет
    P2MSG_PRIORITY_HIGH         = 8,
    P2MSG_PRIORITY_VERY_HIGH    = 11,
    P2MSG_PRIORITY_SYSTEM       = 13    // Только для системных сообщений!
};

#define P2MSG_PRIORITY_MIN P2MSG_PRIORITY_LOW
#define P2MSG_PRIORITY_MAX P2MSG_PRIORITY_SYSTEM

//////////////////////////////////////////////////////////////////////
// системные флаги сообщений

// Требуется гарантированная доставка сообщения
#define P2MSG_FLAG_GRNT                 0x00000001
// Требуется подтверждение о гарантированной доставке
// (по-умолчанию подтверждение не доставляется)
#define P2MSG_FLAG_GRNT_REQ_RECEIPT     0x00000002

//////////////////////////////////////////////////////////////////////

struct P2Message;
typedef struct P2Message *P2MSG_HANDLE;

// общие замечания по стилю
// 1. pbuf - внешний буфер, а ptr - указатель.


// получить указатель на новое пустое сообщение
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCreateMessage(P2MSG_HANDLE *ppmsg);
// освободить память, занятую под сообщение
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgFreeMessage(P2MSG_HANDLE pmsg);

// создать сообщение из плоского представления
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCreateMessageFromBlock(const void *pmem, size_t size, P2MSG_HANDLE *ppmsg, size_t *pRealSize);
// сохранить сообщение в блоке памяти
//TODO: size!!!
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSaveMessageToBlock(P2MSG_HANDLE pmsg, void *pmem, size_t size);

// получить размер блока сообщения
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgGetMessageBlockSize(P2MSG_HANDLE pmsg, size_t *psize);


P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSaveMessageToComposer(P2_ComposeState *pState, P2MSG_HANDLE pmsg);
//сохранить сообщение и записать перед сообщением размер сообщения в блобе
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSaveMessageToComposerWithLength(P2_ComposeState *pState, P2MSG_HANDLE pmsg);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCreateMessageFromParser(P2_ParseState *pState, P2MSG_HANDLE* ppmsg);


// Работа с полями:
// 1. У полей есть строковые имена. 
// 2. Предопределенные имена указаны выше. заводить собственные имена, начинающиеся на P2_ нельзя!
// 3. Допустимо объявлять свои собственные поля.
// 4. Значение поля может быть текстовым или бинарным. В первом случае используется суффикс Str в имени функции,
//	  во-втором длина передается параметром.

// установить бинарное поле из буфера по длине
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetField(P2MSG_HANDLE pmsg, const char *field_name, const void *pbuf, size_t size);
// установить текстовое поле из буфера (длина рассчитывается как strlen()+1)
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetFieldStr(P2MSG_HANDLE pmsg, const char *field_name, const char *pstr);
// установить числовые поля 
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetFieldUINT8(P2MSG_HANDLE pmsg, const char *field_name, UINT8 num);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetFieldINT8(P2MSG_HANDLE pmsg, const char *field_name, INT8 num);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetFieldUINT16(P2MSG_HANDLE pmsg, const char *field_name, UINT16 num);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetFieldINT16(P2MSG_HANDLE pmsg, const char *field_name, INT16 num);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetFieldUINT32(P2MSG_HANDLE pmsg, const char *field_name, UINT32 num);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetFieldINT32(P2MSG_HANDLE pmsg, const char *field_name, INT32 num);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetFieldUINT64(P2MSG_HANDLE pmsg, const char *field_name, UINT64 num);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetFieldINT64(P2MSG_HANDLE pmsg, const char *field_name, INT64 num);


// получить константный указатель на внутренний буфер с полем. только для быстрого чтения!!!
// использовать только для бинарных данных. Для получения строки использовать P2MsgGetStrField
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgGetBinaryField(P2MSG_HANDLE pmsg, const char *field_name, const void **pptr, /*[out]*/size_t *psize);

// FIXME Оставлено для совместимости со старыми версиями. Наличие завершающего 0 не проверяется !!  
// Будет удвлено
#define P2MsgGetFieldStr(pmsg, field_name, pstr) P2MsgGetBinaryField((pmsg), (field_name), ((const void **)pstr), NULL)
// Получить указатель на внутреннюю строку. Проверяет корректность строки.  
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgGetStrField(P2MSG_HANDLE pmsg, const char *field_name, const char **pptr);

#define P2MsgGetFieldSize(pmsg, field_name, psize) P2MsgGetBinaryField((pmsg), (field_name), NULL, (psize))

// скопировать поле в выделенный пользователем буфер.
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyField(P2MSG_HANDLE pmsg, const char *field_name, void *pbuf, /*[in,out]*/ size_t *psize);
// получить числовые поля
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldUINT8(P2MSG_HANDLE pmsg, const char *field_name, UINT8 *pnum);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldINT8(P2MSG_HANDLE pmsg, const char *field_name, INT8 *pnum);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldUINT16(P2MSG_HANDLE pmsg, const char *field_name, UINT16 *pnum);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldINT16(P2MSG_HANDLE pmsg, const char *field_name, INT16 *pnum);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldUINT32(P2MSG_HANDLE pmsg, const char *field_name, UINT32 *pnum);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldINT32(P2MSG_HANDLE pmsg, const char *field_name, INT32 *pnum);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldUINT64(P2MSG_HANDLE pmsg, const char *field_name, UINT64 *pnum);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldINT64(P2MSG_HANDLE pmsg, const char *field_name, INT64 *pnum);



// Ф-ии для установки/взятия стандартных полей в P2SYS_MSG. 
// Работают быстрее, чем установка этих полей по имени через общий интерфейс P2MsgSetField***.  

// установить тело 
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetBody(P2MSG_HANDLE pmsg, const void *pbuf, size_t size); 
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetBodyStr(P2MSG_HANDLE pmsg, const void *pstr); 
// установиь адрес
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetAddrTo(P2MSG_HANDLE pmsg, const char * to); 
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetAddrFrom(P2MSG_HANDLE pmsg, const char *from); 
// установить категорию
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetCategory(P2MSG_HANDLE pmsg, const char *category); 
// установить тип
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetType(P2MSG_HANDLE pmsg, UINT32 type); 
// установить id  сообщения
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetSendId(P2MSG_HANDLE pmsg, UINT64 id);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgSetReplyId(P2MSG_HANDLE pmsg, UINT64 id);

// получить стандартные поля сообщения

P2SYSMSG_API P2ERR P2CALLSPEC P2MsgGetBody(P2MSG_HANDLE pmsg, const void **pptr, size_t *pSize); 
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgGetBodySize(P2MSG_HANDLE pmsg, size_t *pSize); 
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgGetReplyId(P2MSG_HANDLE pmsg, UINT64 *pId); 
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgGetCategory(P2MSG_HANDLE pmsg, const char** ppCategory);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgGetAddrTo(P2MSG_HANDLE pmsg, const char** ppAddrTo);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgGetType(P2MSG_HANDLE pmsg, UINT32* pType);

//  Ф-ии для получения полей сообщения.  

// получить константный указатель на внутренний буфер с телом сообщения. только для быстрого чтения!!!
// создать копию исходного сообщения
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyMessage(P2MSG_HANDLE *pmsg_cpy, const P2MSG_HANDLE pmsg_src );
// скопировать тело в выделенный пользователем буфер
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyBody(const P2MSG_HANDLE pmsg, void *pbuf, size_t *psize);

// копировать значение поля из бинарного представления.
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldFromBlock(const UINT8 *pbinmsg, size_t size, const char *field_name, void *ptr, /*[in,out]*/size_t *psize);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldFromBlockUINT8(const UINT8 *pbinmsg, size_t size, const char *field_name, UINT8 *pnum);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldFromBlockINT8(const UINT8 *pbinmsg, size_t size, const char *field_name, INT8 *pnum);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldFromBlockUINT16(const UINT8 *pbinmsg, size_t size, const char *field_name, UINT16 *pnum);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldFromBlockINT16(const UINT8 *pbinmsg, size_t size, const char *field_name, INT16 *pnum);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldFromBlockUINT32(const UINT8 *pbinmsg, size_t size, const char *field_name, UINT32 *pnum);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldFromBlockINT32(const UINT8 *pbinmsg, size_t size, const char *field_name, INT32 *pnum);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldFromBlockUINT64(const UINT8 *pbinmsg, size_t size, const char *field_name, UINT64 *pnum);
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgCopyFieldFromBlockINT64(const UINT8 *pbinmsg, size_t size, const char *field_name, INT64 *pnum);

// получить стандартные поля из бин. представления (можно передавать нули)
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


// оптимизированная работа с полями сообщения 
#define	P2MSG_PROP_INDEX_TYPE 			0
#define	P2MSG_PROP_INDEX_SEND_ID		1
#define	P2MSG_PROP_INDEX_REPLY_ID		2
#define	P2MSG_PROP_INDEX_ADDR_FROM		3
#define	P2MSG_PROP_INDEX_ADDR_TO		4
#define	P2MSG_PROP_INDEX_BODY			5
#define	P2MSG_PROP_INDEX_CATEGORY		6
#define	P2MSG_PROP_INDEX_LAST			7

// Структура для оптимизированной работы с полями P2SYS_MSG. Для заполнения можно использовать ф-ию P2MsgFieldCreate из P2MsgField.hpp.  
typedef struct _P2MSG_FIELD_ 
{
	union
	{
		// если 0 <= name < P2MSG_PROP_INDEX_LAST, то обрабатывается как индекс стандартного поля
		UINT32 		index;			
		// иначе name обрабатывается как строка, содержащая имя поля.
		const char* str;			
	} name;

	UINT32 		len; 				
	const void* data;

} P2MSG_FIELD; 

// В ф-иях P2MsgField*** массив fields не должен содержать двух полей с одинаковым именем.

// Получить размер буфера, в который можно сохранить P2SYS_MSG, содержащий поля fields.
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgFieldGetMessageBlockSize(P2MSG_FIELD* fields, size_t fieldsCount, size_t *pSize); 
// Сохранить в буфер. Результат идентичен сохранению P2SYS_MSG, содержащего поля fields.
P2SYSMSG_API P2ERR P2CALLSPEC P2MsgFieldSaveMessageToBlock(P2MSG_FIELD* fields, size_t fieldsCount, void *pMem, size_t size); 



#ifdef  __cplusplus
}

#endif


#endif // _P2SYSMSG_H_
