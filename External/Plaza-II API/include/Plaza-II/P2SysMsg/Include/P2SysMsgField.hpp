
#ifndef _P2SYS_MSG_FIELD_HPP_
#define _P2SYS_MSG_FIELD_HPP_

#include "P2SysMsg.h"


// �-�� ���������� P2MSG_FIELD. 


// ��� ���� ����� ������ ������� ������������ ���� ��� ��������� ������.  
// ��� ������ �� ������������ ������ ����� ������������ ���������� �� ������, �-�� P2MsgFieldxxx ����� �������� ������� � ����� �����. 
//
inline void P2MsgFieldCreate(P2MSG_FIELD* field, int fieldIndex, const char* str) 
{
	field->name.index 	= fieldIndex;
	field->len 			= static_cast<UINT32>(str ? strlen(str) + 1: 0);
	field->data 		= str; 
}

inline void P2MsgFieldCreate(P2MSG_FIELD* field, const char* fieldName, const char* str)
{ 
	field->name.str 	= fieldName;
	field->len 			= static_cast<UINT32>(str ? strlen(str) + 1: 0);
	field->data 		= str; 
}

inline void P2MsgFieldCreate(P2MSG_FIELD* field, int fieldIndex, const void* data, size_t dataSz) 
{ 
	field->name.index 	= fieldIndex;
	field->len 			= static_cast<UINT32>(dataSz);
	field->data 		= data;
}

inline void P2MsgFieldCreate(P2MSG_FIELD* field, const char* fieldName, const void* data, size_t dataSz) 
{ 
	field->name.str = 	fieldName;
	field->len 		= 	static_cast<UINT32>(dataSz);
	field->data 	= 	data; 
}


#endif // _P2SYS_MSG_FIELD_HPP_







