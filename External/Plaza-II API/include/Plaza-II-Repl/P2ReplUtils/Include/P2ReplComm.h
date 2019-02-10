#ifndef P2REPL_COMM_H_
#define P2REPL_COMM_H_

#include "../../P2ReplCommon/Include/P2Repl.h"

// элементы протокола репликации

/* 

  Версии протокола документируются в этом комментарии.
  Версии, поддерживаемые клиентом и сервером объявлены в виде #define в 
  соответствующих публичных хидерах 

  Соглашение о версиях протекает следующим образом:
	
	Cli---Svr
	->OPEN(cli_ver)
	<-OPEN_ANS(ser_ver) или OPEN_FAIL(incompatible version)
	->SCHEME... или CLOSE(incompatible version)

  Сервер отказывается на втором шаге, а клиент на третьем.

  Примеры:
	->OPEN(1)
	<-OPEN_ANS(1) // даже если текущая версия у сервера 3, а версия 1 поддерживается сервером
	->SCHEME..

    ->OPEN(1)
    <-OPEN_FAIL(incompatible version) // В том случае, если сервер поддерживает версии начания со 2-ой

	->OPEN(3)
	<-OPEN_ANS(1) // если сервер старее, то он может понадеяться на поддержку в клиенте
	->CLOSE(incompatible version) // если клиент не разумеет старой версии

	->OPEN(3)
	<-OPEN_ANS(1) // если сервер старее, то он может понадеяться на поддержку в клиенте
	->SHEME... // если клиент разумеет старую версию

  14.01.04 v1
    - начальная версия
  02.04.04 v2
    - добавлена компрессия данных
    - версия 1 больше не поддерживается
  18.05.04 v3
    - добавлена очистка удаленных данных с младшими ревижнами
	- остальное совпадает
	- версия 2 не поддерживается
  16.11.04 v4
	- передача данных из нескольких таблиц одним пакетом
	- версия 3 не поддерживается
  24.11.04 v5
    - добавлена фильтрация записей на сервере
	- сервер работает с версиями 4 и 5
	- клиент работает только с версией 5

  23.05.2005 v6
	- добавлена команда реконнекта при изменении номера жизни схемы
	- версия 5 не поддерживается
  25.05.2007 v10
    - добалено в сообщениях сервера поле P2_Repl_Chan для хранения channel, session, order
	- сервер работает с версиями 9 и 10
	- клиент работает только с версией 10
  30.09.2009
    - добвлено сообщение P2REPL_PKT_START_ONLINE для получения только ONLINE данных
	- сервер работает с версиями 9 и 10
	- клиент работает только с версией 10
  26.02.2010
	- в пакет с данными добавлено новое поле c последними просмотренными ревижнами по таблицам
  ~  05.2011 v12 (с Plaza-2 v187)
    - передача реплики в плагин RouterReplica
  06.02.2013 v13 (с Plaza-2 v196)
    - в пакет  CLEAR_DELETED добавлено поле флагов
  28.05.2013 v13 (с Plaza-2 v197)
    - в пакет OPEN добавлено поле с дополнительными данными клиента
  01.10.2013 v13 (с Plaza-2 v197.4)
    - в пакет OPEN_ANS добавлено поле с флагами сервера
  11.04.2014 v14 (с Plaza-2 v199)
    - поддержка FullRecComposing
    - удаленные записи всегда отдаются полностью
  10.09.2015 v14 (с Plaza-2 v204)
    - пакет P2REPL_PKT_RECONNECT не формируется на сервере. Заменен на CLOSE с кодом P2ERR_REPL_RECONNECT
  02.11.2015 v14 (с Plaza-2 v204) флаг P2REPL_START_FLAG_SNAPSHOT_SERVER_SUPPORTED удален на клиенте и сервере
  29.07.2016 v15 (Plaza-2   v207) Версия протокола изменяется с каждой версией плазы2. 
*/

/*

Общие поля (выставлены в каждом сообщении):
===============================================
category = P2REPL_CATEGORY_SYSTEM

-----------------------------------------------
OPEN (cli, send)
-----------------------------------------------
type = P2REPL_PKT_OPEN
version = [u4]<client version>
service_to = [c]<stream name>
[P2_Repl_Extra] = [c]<client extra info>

-----------------------------------------------
OPEN_ANS (srv, reply)
-----------------------------------------------
type = P2REPL_PKT_OPEN_ANS
version = [u4]<server version>
channel = [u8]<new channel id>
body = [b]<server scheme>
token = [s]<server token>
flags = [u4]<server flags>

-----------------------------------------------
OPEN_ANS (srv, reply) VERSION 10
-----------------------------------------------
type = P2REPL_PKT_OPEN_ANS
version = [u4]<server version>
chan = [u8]<new channel id>[u4]<session id = 0>[u4]<session order num = 0>
body = [b]<server scheme>

-----------------------------------------------
OPEN_FAIL_ANS (srv, reply)
-----------------------------------------------
type = P2REPL_PKT_OPEN_FAIL_ANS
body = [p2err]<reason>

///////////////////////////////////////////////

-----------------------------------------------
SCHEME (cli, send)
-----------------------------------------------
type = P2REPL_PKT_SCHEME
channel = [u8]<channel id>
body = [b]<client scheme>
filter = [composer:"u4c..."]<номер таблицы, фильтр для таблицы>

-----------------------------------------------
SCHEME_ANS (srv, reply)
-----------------------------------------------
type = P2REPL_PKT_SCHEME_ANS
channel = [u8]<channel id>
body = [p2err]<result>

-----------------------------------------------
SCHEME_ANS (srv, reply) VERSION 10
-----------------------------------------------
type = P2REPL_PKT_SCHEME_ANS
chan = [u8]<new channel id>[u4]<session id = 0>[u4]<session order num = 0>
body = [p2err]<result>

///////////////////////////////////////////////

-----------------------------------------------
START_ONLINE (cli, post)
-----------------------------------------------
type = P2REPL_PKT_START_ONLINE
channel = [u8]<channel id>
session = [u4]<new session id>

-----------------------------------------------
START_COMBINE (cli, post)
-----------------------------------------------
type = P2REPL_PKT_START_COMBINE
channel = [u8]<channel id>
session = [u4]<new session id>
body = [composer:"u4..."]<массив текущих ревижнов клиента + 1>
       (если клиент хочет получать данные с начала, то можно указать пустой composer)
start_flag = u4 опциональный! пока только флаг HANDLE_SNAPSHOT_SESSION

-----------------------------------------------
DATA (srv, post)
-----------------------------------------------
type = P2REPL_PKT_DATA
channel = [u8]<channel id>
session = [u4]<session id>
count = [u4]<++session order num = 0>
data_info=[composer:u4u4u4][composer:"u4u4u4u4u1..."]<P2REPL_FORMAT_DATA_HEADER><P2REPL_FORMAT_TBL_HEADER>
null_fields=[composer:"u1..."]
body=<compressed records>


-----------------------------------------------
DATA (srv, post) VERSION 10
-----------------------------------------------
type = P2REPL_PKT_DATA
chan = [u8]<channel id>[u4]<session id>[u4]<++session order num = 0>
data_info=[composer:u4u4u4][composer:"u4u4u4u4u1..."]<P2REPL_FORMAT_DATA_HEADER><P2REPL_FORMAT_TBL_HEADER>
data_info_2=[composer:"u4u8..."]<P2REPL_FORMAT_DATA_2_TBL_INFO>
null_fields=[composer:"u1..."]
body=<compressed records>

-----------------------------------------------
ACK_DATA  (cli, post)
-----------------------------------------------
type = P2REPL_PKT_ACK_DATA
channel = [u8]<channel id>
session = [u4]<session id>
count = [u4]<session order num of DATA with ans_req>
body = [composer:"u4..."]<массив текущих ревижнов клиента + 1>
       (если клиент хочет получать данные с начала, то можно указать пустой composer)

///////////////////////////////////////////////

-----------------------------------------------
PING  (cli, post)
// используется клиентом с момента окрытия потока (получения channel_id)
-----------------------------------------------
type = P2REPL_PKT_PING
channel = [u8]<channel id>

-----------------------------------------------
PING  (cli, post) VERSION 11
// используется клиентом с момента окрытия потока
-----------------------------------------------
type = P2REPL_PKT_PING
channel = [u8]<channel id>
session = [u4]<session id>

-----------------------------------------------
PING  (srv, post)
// используется сервером с момента открытия потока до запроса данных,
// после запроса данных (открытия сессии) используется P2REPL_PKT_COUNTED_PING
пинг со счетчиком необходим клиенту для контроля пропущенных данных (особенно онлайна), 
который может быть покрыт серией пингов
-----------------------------------------------
type = P2REPL_PKT_PING
channel = [u8]<channel id>

-----------------------------------------------
PING  (srv, post) VERSION 10
-----------------------------------------------
type = P2REPL_PKT_PING
chan = [u8]<channel id>[u4]<session id = 0>[u4]<session order num = 0>

-----------------------------------------------
P2REPL_PKT_COUNTED_PING  (srv, post)
-----------------------------------------------
type = P2REPL_PKT_COUNTED_PING
channel = [u8]<channel id>
session = [u4]<session id>
count = [u4]<session order num of DATA with ans_req>

-----------------------------------------------
P2REPL_PKT_COUNTED_PING  (srv, post) VERSION 10
-----------------------------------------------
type = P2REPL_PKT_COUNTED_PING
chan = [u8]<channel id>[u4]<session id>[u4]<session order num of DATA with ans_req>

-----------------------------------------------
CLOSE (cli/srv, post)
-----------------------------------------------
type = P2REPL_PKT_CLOSE
channel = [u8]<channel id>
[body] = [p2err]<reason>

-----------------------------------------------
CLOSE (srv, post) VERSION 10
-----------------------------------------------
type = P2REPL_PKT_CLOSE
chan = [u8]<channel id>[u4]<session id = 0>[u4]<session order num = 0>
[body] = [p2err]<reason>

-----------------------------------------------
P2REPL_PKT_CLEAR_DELETED (srv, post) VERSION 12
-----------------------------------------------
type = P2REPL_PKT_CLEAR_DELETED
channel = [u8]<channel id>
session = [u4]<session id>
count = [u4]<++session order num = 0>
body = [composer:"u4(u4u8)..."]<счетчик и сами пары, <номер таблицы, ревижн первой записи>>

-----------------------------------------------
P2REPL_PKT_CLEAR_DELETED (srv, post) 
-----------------------------------------------
type = P2REPL_PKT_CLEAR_DELETED
channel = [u8]<channel id>
session = [u4]<session id>
count = [u4]<++session order num = 0>
flags = [u4]<clear deleted flag>
body  = [composer:"u4(u4u8)..."]<счетчик и сами пары, <номер таблицы, ревижн первой записи>>

-----------------------------------------------
P2REPL_PKT_CLEAR_DELETED (srv, post) VERSION 10
-----------------------------------------------
type = P2REPL_PKT_CLEAR_DELETED
chan = [u8]<channel id>[u4]<session id>[u4]<session order num = 0>
body = [composer:"u4(u4u8)..."]<счетчик и сами пары, <номер таблицы, ревижн первой записи>>

// С Plaza-2 v204 пакет RECONNECT не формируется сервером
// -----------------------------------------------
// RECONNECT (srv, post)
// -----------------------------------------------
// type = P2REPL_PKT_RECONNECT
// channel = [u8]<channel id>
// [body] = [u1]<clear db flag>
// 
// -----------------------------------------------
// RECONNECT (srv, post) VERSION 10
// -----------------------------------------------
// type = P2REPL_PKT_RECONNECT
// chan = [u8]<channel id>[u4]<session id = 0>[u4]<session order num = 0>
// [body] = [u1]<clear db flag>
*/

// наименование категории сообщений
#define P2REPL_CLIENT_TO_SERVER_CATEGORY_SYSTEM "P2_Replication_C2S"
#define P2REPL_SERVER_TO_CLIENT_CATEGORY_SYSTEM "P2_Replication_S2C"

// наименование полей

// версия протокола. присутствует в P2REPL_FORMAT_OPEN/P2REPL_FORMAT_OPEN_ANS
#define P2REPL_PROP_VERSION "P2_Repl_Ver"
// channel id. выставляется сервером в P2REPL_FORMAT_OPEN_ANS, дальше должен присутствовать во всех сообщениях
#define P2REPL_PROP_CHANNEL_ID "P2_Repl_Channel"
// session id. выставляется клиентом в START и поддерживается в DATA/ACK_DATA
#define P2REPL_PROP_SESSION_ID "P2_Repl_Session"
// order count. выставляется сервером в DATA и автоинкрементируется в последующих DATA в сессии и отдельная нумерация в онлайне
#define P2REPL_PROP_ORDER_COUNT "P2_Repl_Count"
// answer required. выставляется сервером в DATA 
//#define P2REPL_PROP_ANS_REQ "P2_Repl_Ans_Req" <- вместо этого поля выставляется флаг в body: P2REPL_DATA_FLAG_ANSREQ
#define P2REPL_PROP_DATA_INFO "P2_Repl_DataInfo"
// список последних обработанных ревижнов
#define P2REPL_PROP_DATA_INFO_2 "P2RDI2"
// объединяет поля channel id, session id, order count 
#define P2REPL_PROP_CHANNEL_INFO "P2_Repl_Chan"
#define P2REPL_CHANNEL_INFO_FORMAT "u8u4u4"

// флаги, передаваемые в START_COMBINE и CLEAR_DELETED
#define P2REPL_PROP_START_FLAGS "P2_Repl_Flags"

// флаги сервера, передаваемые клиенту
#define P2REPL_PROP_SERVER_FLAGS P2REPL_PROP_START_FLAGS

// filters, задается клиентом при отсылке схемы
#define P2REPL_PROP_FILTER "P2_Repl_Filter" 
// null fields, нулевые поля записи, P2ReplNullFields.hpp
#define P2REPL_PROP_NULL_FIELDS "P2_Repl_Null_Fields"

#define P2REPL_PROP_REDIRECT_ADDRESS "P2_Redirect_Address"

#define P2REPL_PROP_TOKEN "P2_Repl_Token"

#define P2REPL_EXTRA_INFO "P2_Repl_Extra"

// интервал пингов 
#define P2REPL_PROP_PING_INTERVAL "P2_Repl_Ping_Interval"

// типы пакетов протокола
enum P2REPL_PKT
{
	P2REPL_PKT_OPEN,
	P2REPL_PKT_OPEN_ANS,
	P2REPL_PKT_OPEN_FAIL_ANS,
	P2REPL_PKT_SCHEME,
	P2REPL_PKT_SCHEME_ANS,
	P2REPL_PKT_START_SNAPSHOT_OBSOLETE, // устар.
	P2REPL_PKT_START_ONLINE,
	P2REPL_PKT_START_COMBINE,
	P2REPL_PKT_START_ANS,       // ?
	P2REPL_PKT_DATA,
	//P2REPL_PKT_ONLINE,
	P2REPL_PKT_ACK_DATA,
	P2REPL_PKT_PING,
	P2REPL_PKT_COUNTED_PING,
	P2REPL_PKT_CLOSE,
	P2REPL_PKT_CLEAR_DELETED, // добавлено в версии 3. очистка старых удаленных сообщений
	P2REPL_PKT_RECONNECT // не посылается сервером, оставлен для совместимости со старыми серверами
};

typedef UINT32 P2REPL_VERSION;
typedef UINT64 P2REPL_CHANNEL_HANDLE;
typedef UINT32 P2REPL_SESSION_HANDLE;
typedef UINT32 P2REPL_ORDER_NUM;

//#define P2REPL_FORMAT_OPEN никаких данных не шлет

// схема сервиса
#define P2REPL_FORMAT_OPEN_ANS "b"

// код ошибки открытия
#define P2REPL_FORMAT_OPEN_FAIL_ANS "u4"

// схема, выбранная клиентом
#define P2REPL_FORMAT_SCHEME "b"

// фильтры для схемы
#define P2REPL_FORMAT_FILTER "u4b"

// код ошибки установки схемы
#define P2REPL_FORMAT_SCHEME_ANS "u4"

// номер номера последних ревижнов + 1 всех таблиц в схеме
#define P2REPL_FORMAT_START_SNAPSHOT_PART   P2REPL_REVISION_FIELD_FORMAT
#define P2REPL_FORMAT_START_COMBINE_PART    P2REPL_REVISION_FIELD_FORMAT

// номера последних ревижнов на сервере
#define P2REPL_FORMAT_START_SNAPSHOT_ANS_PART   P2REPL_REVISION_FIELD_FORMAT
#define P2REPL_FORMAT_START_COMBINE_ANS_PART    P2REPL_REVISION_FIELD_FORMAT

//#define P2REPL_DATA_FLAG_BOT	1
#define P2REPL_DATA_FLAG_EOT	2
#define P2REPL_DATA_FLAG_EOALL	4
#define P2REPL_DATA_FLAG_ANSREQ	8   // Необходимо подтвердить получение данных через P2REPL_PKT_ACK_DATA
#define P2REPL_DATA_FLAG_ONLINE	0x10
// 0x20 устарел с V14 сервера. Он отдавался сервером в ответ на установку флага REMOVE_DELETED_EX от клиента, а такого теперь нет
// От клиентов предыдущих версий флаг REMOVE_DELETED_EX игнорируется - сервер всегда отдает полную запись
//#define P2REPL_DATA_FLAG_SHORT_DELETED 0x20
// Флаг, отмечающий, что пачка данных содержит только часть онлайн-транзакции (из-за превышения размера пачки), и,
// возможно, далее будут еще пачки с данными, относящимися к этой транзакции
// Появился в протоколе 14, пока на клиенте не анализируется.
#define P2REPL_DATA_FLAG_PART_OF_TRANS_ONLINE 0x40

//#define P2REPL_START_FLAG_SNAPSHOT_SERVER_SUPPORTED 1 // удален с версии Plaza-2 204
//#define P2REPL_START_FLAG_REMOVE_DELETED 2 // deprecated with V14

#define P2REPL_TABLE_FLAG_EOT	1
#define P2REPL_TABLE_FLAG_FRC	2 // данные по таблице закомпожены структурами с серверной схемой

// DATA и ONLINE записи: 
/* 4 - флаги
 * 4 - size of decompressed data
 * 4 - тип компрессии

 * 4 - номер таблицы
 * 4 - кол-во записей
 * 4 - индекс начала таблицы в парсере
 * 4 - индекс начала NULL'ов таблицы в парсере
 * 1 - признак последней записи снепшота
 */
#define P2REPL_FORMAT_DATA_HEADER "u4u4u4"
#define P2REPL_FORMAT_TBL_HEADER  "u4u4u4u4u1"
#define P2REPL_FORMAT_DATA_2_TBL_INFO "u4u8"

// номера последних ревижнов + 1 всех таблиц в схеме
#define P2REPL_FORMAT_ACK_DATA_PART P2REPL_REVISION_FIELD_FORMAT

// Причина закрытия соединения (P2ERR)
#define P2REPL_FORMAT_CLOSE "u4"

// Очищать базу при переподключении
#define P2REPL_FORMAT_RECONNECT "u1"
 
//#define P2REPL_PING_FLAG_DATA 1
//#define P2REPL_PING_FLAG_ONLINE 2

#define P2REPL_FORMAT_CLEAR_HEADER "u4"
#define P2REPL_FORMAT_CLEAR_PART "u4" P2REPL_REVISION_FIELD_FORMAT

#define STR_SIZE(x) ((x) + 1)

#define P2REPL_SERVER_TO_CLIENT_DEFAULT_PING_INTERVAL			60000     // значение по умолчанию интервала пингов "сервер --> клиент"
#define P2REPL_CLIENT_TO_SERVER_PING_INTERVAL					10000     // интервал пингов "клиент --> сервер"
#define P2REPL_SKIPPED_PINGS        							3

#define P2REPL_FORMAT(a,b) ""#a#b""

/*
	Поскольку железно мы зашиты на бинарный композер можно использовать структуру
	с выравниванием 1 вместо последовательного композинга полей.
*/
#pragma pack(push, 1)
typedef struct P2REPL_CHANNEL_INFO_TAG
{
	P2REPL_CHANNEL_HANDLE	m_channelId;
	P2REPL_SESSION_HANDLE	m_session;
	P2REPL_ORDER_NUM		m_order;
} P2REPL_CHANNEL_INFO;
#pragma pack (pop)

#endif

