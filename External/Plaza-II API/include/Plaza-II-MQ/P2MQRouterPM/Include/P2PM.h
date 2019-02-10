#ifndef _P2PM_H_
#define _P2PM_H_

#include "../../../Plaza-II/P2Common/Include/P2Errors.h"

//////////////////////////////////////////////////////////////////////

#define P2PROGMON_INI_SECT      "ProgMon"
#define P2PROGMON_INI_SECT_HDR  P2PROGMON_INI_SECT ":"
#define P2PROGMON_INI_COMMON_EXITTIMEOUT        "EXITTIMEOUT"
#define P2PROGMON_INI_COMMON_EXITTIMEOUT_DEF    30
#define P2PROGMON_INI_COMMON_PROG_MASTER        "MASTER"
#define P2PROGMON_INI_COMMON_PROG_SLAVE         "SLAVE"
#define P2PROGMON_INI_COMMON_RESTART_TIMEOUT	"RESTART_TIMEOUT"

#define P2PROGMON_INI_PROG_SUSPENDED            "SUSPEND"
#define P2PROGMON_INI_PROG_EXITEVENT            "EXITEVENT"
//#define P2PROGMON_INI_PROG_DESCRIPTION          "DESCRIPTION"
//#define P2PROGMON_INI_PROG_HINT                 "HINT"
#define P2PROGMON_INI_PROG_IMAGENAME            "IMAGENAME"
#define P2PROGMON_INI_PROG_COMMANDLINE          "COMMANDLINE"
#define P2PROGMON_INI_PROG_CURRENTDIR           "CURRENTDIR"
#define P2PROGMON_INI_PROG_DETACHED             "DETACHED"
#define P2PROGMON_INI_PROG_SAMECONSOLE          "SAMECONSOLE"
#define P2PROGMON_INI_PROG_SEPARATEMEMORYSPACE  "SEPARATEMEMORYSPACE"
#define P2PROGMON_INI_PROG_PRIORITYCLASS        "PRIORITYCLASS"
#define P2PROGMON_INI_PROG_TITLE                "TITLE"
#define P2PROGMON_INI_PROG_DESKTOP              "DESKTOP"
#define P2PROGMON_INI_PROG_PATH					"PATH"
#define P2PROGMON_INI_PROG_X                    "X"
#define P2PROGMON_INI_PROG_Y                    "Y"
#define P2PROGMON_INI_PROG_XPIXELSIZE           "XPIXELSIZE"
#define P2PROGMON_INI_PROG_YPIXELSIZE           "YPIXELSIZE"
#define P2PROGMON_INI_PROG_XCHARSIZE            "XCHARSIZE"
#define P2PROGMON_INI_PROG_YCHARSIZE            "YCHARSIZE"
//#define P2PROGMON_INI_PROG_FOREGROUND           "FOREGROUND"
//#define P2PROGMON_INI_PROG_BACKGROUND           "BACKGROUND"
#define P2PROGMON_INI_PROG_SHOWWINDOW           "SHOWWINDOW"
#define P2PROGMON_INI_PROG_FORCEFEEDBACK        "FORCEFEEDBACK"
#define P2PROGMON_INI_PROG_AFFINITYMASK			"AFFINITY"

//////////////////////////////////////////////////////////////////////

#ifndef RTS_ENV_EXITEVENT
 #define RTS_ENV_EXITEVENT      "RTSExitEvent"
#endif //RTS_ENV_EXITEVENT

#ifndef RTS_EXITNORMAL
 #define RTS_EXITNORMAL         0
#endif // RTS_EXITNORMAL

#ifndef RTS_EXITFATAL
 #define RTS_EXITFATAL          13
#endif // RTS_EXITFATAL

#define P2PROGMON_ENV_EXITEVENT     RTS_ENV_EXITEVENT

//////////////////////////////////////////////////////////////////////
// Пользовательский протокол управления PM

/*

  Для выполнения PM-команды на узле <node>, необходимо send-ом
  послать сообщение вида:

    To:         <node>.P2PROGMON_APP_NAME
    Category:   P2MSG_CATEGORY_PROGMON
    Type:       <commad id (one of P2MSG_TYPE_PROGMON_..)>
    Body:       [string: command parameter(s)]

  В ответ приходит реплай вида:

    Category:   P2MSG_CATEGORY_PROGMON
//    Type:       P2MSG_TYPE_PROGMON_REPLY
    PropResult: <P2ERR>
    Body:       [опциональные доп. данные]

*/

#define P2PROGMON_APP_NAME      "P2ProgMon"
#define P2MSG_CATEGORY_PROGMON  "P2_ProgMon"

// Additional field(s):
//  - результат исполнения команды
//    [P2ERR]
#define P2PROGMON_PROP_RESULT   "P2PROGMON_P2ERR"

enum
{
    P2MSG_TYPE_PROGMON_LIST,        // Body: -
    P2MSG_TYPE_PROGMON_SUSPEND,     // Body: <program name>
    P2MSG_TYPE_PROGMON_TERMINATE,   // Body: <program name>
    P2MSG_TYPE_PROGMON_RESUME,      // Body: <program name>
    P2MSG_TYPE_PROGMON_EXIT,        // Body: -
    P2MSG_TYPE_PROGMON_DISCONNECT,  // Body: login to disconnect
//    P2MSG_TYPE_PROGMON_REPLY

    P2MSG_TYPE_PROGMON_LAST_
};

//////////////////////////////////////////////////////////////////////

#endif // _P2PM_H_
