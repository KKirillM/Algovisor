#ifndef _P2MQIOCTL_H_
#define _P2MQIOCTL_H_ 


#ifdef  __cplusplus
#include "../../EventQueue/Include/transport_session.h" // for session_type, seq_type
#include "../../../Plaza-II/P2Common/Include/P2Errors.h"
#endif

#define P2MQIOCTL_JOIN_MULTICAST_GROUP     			0x1      /* создать слушающий мультикастовый сокет в P2MQ channel'e */
#define P2MQIOCTL_LEAVE_MULTICAST_GROUP    			0x2      /* удалить мультикастовый сокет */
#define P2MQIOCTL_SET_INCOMPLETE_SEND_HANDLER	 	0x3      

/* описание ioctl кодов.*/
#define P2MQIOCTL_NAMES { "NOIOCTL", "JOIN_MULTICAST_GROUP", "LEAVE_MULTICAST_GROUP", "SET_INCOMPLETE_SEND_HANDLER" } 

/* Все Ioctl's должны содержать поле MQIoctl первым */ 
struct MQIoctl
{
	int code;  
};

#ifdef  __cplusplus
struct Ioctl_JoinMulticastGroup
{
	MQIoctl 				ctl;  				 
	const char* 			opt;
	transport::session_type session;
	transport::seq_type 	seq; 
	unsigned int 			session_timeout;
};
#endif

// при невозможности полной записи LLMsg в сокет вызывается handler
// handler
// @Return Value: если handler возвращает ошибку, P2MQ*** функция отправки llmsg, которая выполняет запись в сокет, завершится с полученной ошибкой.
//                handler не должен возвращать P2ERR_MQ_*** ошибку.

struct Ioctl_SendIncompleteHandler
{
	MQIoctl ctl;  				 
	P2ERR 	(*handler)(void* context);
	void* 	context;	
};

const char* P2MQIoctlToStr(const MQIoctl* ctl);

#endif //_P2MQIOCTL_H_





