#ifndef _P2ERRORS_H_
#define _P2ERRORS_H_

#include "P2Types.h"

//
// При добавлении нового кода ошибки не забудь обновить описание!!!
// (Plaza-II\P2SysTypes\Source\P2ErrGetDescr.cpp):44
//

typedef UINT32 P2ERR;

// error ranges

#define P2ERR_RANGE_SIZE    0x1000

#define P2ERR_COMMON_BEGIN  0x0000
#define P2ERR_COMMON_END    (P2ERR_COMMON_BEGIN + P2ERR_RANGE_SIZE - 1)

#define P2ERR_MSG_BEGIN     0x1000
#define P2ERR_MSG_END       (P2ERR_MSG_BEGIN + P2ERR_RANGE_SIZE - 1)

#define P2ERR_CONN_BEGIN    0x2000
#define P2ERR_CONN_END      (P2ERR_CONN_BEGIN + P2ERR_RANGE_SIZE - 1)

#define P2ERR_ROUTE_BEGIN   0x3000
#define P2ERR_ROUTE_END     (P2ERR_ROUTE_BEGIN + P2ERR_RANGE_SIZE - 1)

#define P2ERR_INI_BEGIN     0x4000
#define P2ERR_INI_END       (P2ERR_INI_BEGIN + P2ERR_RANGE_SIZE - 1)

#define P2ERR_LOG_BEGIN     0x5000
#define P2ERR_LOG_END       (P2ERR_LOG_BEGIN + P2ERR_RANGE_SIZE - 1)

#define P2ERR_MQ_BEGIN      0x6000
#define P2ERR_MQ_END        (P2ERR_MQ_BEGIN + P2ERR_RANGE_SIZE - 1)

#define P2ERR_MQCRYPT_BEGIN 0x7000
#define P2ERR_MQCRYPT_END   (P2ERR_MQCRYPT_BEGIN + P2ERR_RANGE_SIZE - 1)

#define P2ERR_DB_BEGIN      0x8000
#define P2ERR_DB_END        (P2ERR_DB_BEGIN + P2ERR_RANGE_SIZE - 1)

#define P2ERR_SERV_BEGIN    0x9000
#define P2ERR_SERV_END      (P2ERR_SERV_BEGIN + P2ERR_RANGE_SIZE - 1)

#define P2ERR_REPL_BEGIN    0xA000
#define P2ERR_REPL_END      (P2ERR_REPL_BEGIN + P2ERR_RANGE_SIZE - 1)

#define P2ERR_CRYPT_BEGIN   0xB000
#define P2ERR_CRYPT_END     (P2ERR_CRYPT_BEGIN + P2ERR_RANGE_SIZE - 1)

#define P2ERR_ASNS_BEGIN    0xC000
#define P2ERR_ASNS_END      (P2ERR_ASNS_BEGIN + P2ERR_RANGE_SIZE - 1)

#define P2ERR_VM_BEGIN      0xD000
#define P2ERR_VM_END        (P2ERR_VM_BEGIN + P2ERR_RANGE_SIZE - 1)

#define P2ERR_MQASNS_BEGIN  0xE000
#define P2ERR_MQASNS_END    (P2ERR_MQASNS_BEGIN + P2ERR_RANGE_SIZE - 1)

#define P2ERR_ACL_BEGIN     0xF000
#define P2ERR_ACL_END       (P2ERR_ACL_BEGIN + P2ERR_RANGE_SIZE - 1)

#define P2ERR_MQSTAT_BEGIN  0x10000
#define P2ERR_MQSTAT_END    (P2ERR_MQSTAT_BEGIN + P2ERR_RANGE_SIZE - 1)

#define P2ERR_BL_BEGIN      0x11000
#define P2ERR_BL_END        (P2ERR_BL_BEGIN + P2ERR_RANGE_SIZE - 1)

//#define P2ERR_UNUSEDRANGE1_BEGIN 0x12000
//#define P2ERR_UNUSEDRANGE2_BEGIN 0x13000

#define P2ERR_TEST_BEGIN    0x14000
#define P2ERR_TEST_END      (P2ERR_TEST_BEGIN + P2ERR_RANGE_SIZE - 1)

#define P2ERR_ISAMSM_BEGIN  0x15000
#define P2ERR_ISAMSM_END    (P2ERR_ISAMSM_BEGIN + P2ERR_RANGE_SIZE - 1)

#define P2ERR_LRPCQ_BEGIN   0x16000
#define P2ERR_LRPCQ_END     (P2ERR_LRPCQ_BEGIN + P2ERR_RANGE_SIZE-1)

#define P2ERR_CORE_BEGIN    0x17000
#define P2ERR_CORE_END      (P2ERR_CORE_BEGIN + P2ERR_RANGE_SIZE-1)

#define P2ERR_USER_DEFINED_BEGIN    0x90000
#define P2ERR_USER_DEFINED_END      (P2ERR_USER_DEFINED_BEGIN + P2ERR_RANGE_SIZE-1)

// see "P2ErrorsBL.h"

// P2ERR_COMMON
enum{
    // возвращается в случае успешного выполнения
    P2ERR_OK = P2ERR_COMMON_BEGIN,
    // ошибка выделения памяти
    P2ERR_COMMON_NO_MEMORY,
    // некорректные значения аргументов. если возможно, то 
    // стоит передавать более конкретную информацию.
    // например, для передачи NULL можно вернуть и этот код,
    // а обнаружение битого сообщения лучше обозначать отдельным кодом
    P2ERR_COMMON_WRONG_ARGUMENTS,
    // Неожиданная ошибка
    // (подробности см. в логе)
    P2ERR_COMMON_INTERNAL_ERROR,
    // Неожиданная ошибка в системном вызове
    P2ERR_COMMON_SYSCALL_FAILED,
    // (Пока) не реализовано
    P2ERR_COMMON_NOT_IMPLEMENTED,
    // объект не проинициализирован до конца. 
    // инициализация зависит от внешних факторов,
    // на которые вызывающий повлиять не может
    P2ERR_COMMON_OBJECT_NOT_INITED,
    // переданный для заполнения буфер меньше, чем нужно
    P2ERR_COMMON_BUFFER_TOO_SMALL,
    // переданный для копирования буфер больше, чем нужно
    P2ERR_COMMON_BUFFER_TOO_LARGE,
    // ошибка возвращается в цепочечном обработчике и
    // показывает, что дальнейшая обработка не нужна
    P2ERR_COMMON_MESSAGE_WAS_INTERCEPTED,
    // конфликт версий разных модулей
    P2ERR_COMMON_INVALID_INTERFACE_VERSION,
    // в строке нет ожидаемого завершающего нуля
    P2ERR_COMMON_BAD_STRING,
    // используется как стандартное неаварийное завершение енумератора с коллбэком
    P2ERR_COMMON_CANCEL_ENUM,
    // Код завершенея процесса в случае фатальной ошибки
    // (для совместимости с плазой-1 значение должно равняться 13!)
    P2ERR_COMMON_EXITCODE_FATAL,
    P2ERR_COMMON_OVERFLOW,
    P2ERR_COMMON_REAL_SIZE_RETURNED,
    P2ERR_COMMON_NOT_SUPPORTED,
    P2ERR_COMMON_ALREADY_INITIALIZED,
    // смешивание отладочных и релизных версий модулей в одной программе
    P2ERR_COMMON_WRONG_BUILD,
    // универсальный код для ошибки - повторного определения чего-либо
    P2ERR_COMMON_ALREADY_DEFINED,
    // Код завершения процесса, убитого снаружи
    P2ERR_COMMON_EXITCODE_TERMINATED,
    P2ERR_NOT_OK, // аналог false
    P2ERR_BUFFER_SIZES_MISMATCH, //размер внутреннего буфера записи не равен размеру буфера, переданному в параметрах
    P2ERR_COMMON_WRONG_INI_PARAMETER,
    P2ERR_COMMON_NOT_INITIALIZED,
    P2ERR_COMMON_TIMEOUT,
    P2ERR_COMMON_OPEN_FILE_MAPPING_FAILED,
    P2ERR_COMMON_INVALID_OBJECT_STATE,
    P2ERR_COMMON_NO_FREE_OBJECTS,
    P2ERR_COMMON_OBJECT_BUSY,
    P2ERR_COMMON_EXIT_EVENT_RECEIVED,
    P2ERR_COMMON_HANDLE_EOF,
    P2ERR_COMMON_NO_SUCH_ITEM,
    P2ERR_COMMON_EXITCODE_RESTART,
    P2ERR_COMMON_LAST
};

// P2ERR_MSG
enum{
    // запрошенного поля в сообщении не существует
    P2ERR_MSG_FIELD_NOT_FOUND = P2ERR_MSG_BEGIN,
    // ???? две следующие ошибки можно объединить
    // фиксированный заголовок сообщения испорчен
    P2ERR_MSG_BAD_MESSAGE_SIGNATURE,
    // переменный заголовок сообщения испорчен
    P2ERR_MSG_WRONG_OFFSETS_IN_MESSAGE,
    // возвращается из P2MsgGetFieldFromBlock если нельзя извлечь поле
//    P2ERR_MSG_FIELD_NOT_AVAILABLE, <- use P2ERR_MSG_FIELD_NOT_FOUND
    // блок не может быть разобран в функциях *Parse*
    P2ERR_MSG_BAD_PARSE_BLOCK,
    P2ERR_MSG_LAST
};

// P2ERR_CONN
enum{
    // Common socket errors
    // (use WSAGetLastError() to get extended information)
    P2ERR_CONN_SOCK_CREATE = P2ERR_CONN_BEGIN,  // Couldn't create socket
    P2ERR_CONN_SOCK_BIND,                // Couldn't bind socket
    P2ERR_CONN_SOCK_MAKEADDR,           // Couldn't prepare socket address
    P2ERR_CONN_SOCK_CONNECT,            // Couldn't connect socket
    P2ERR_CONN_SOCK_EVTSELECT,          // Error in WSAEventSelect()
    P2ERR_CONN_SOCK_LISTEN,                // Couldn't listen socket
    P2ERR_CONN_SOCK_ERROR,                // Error in socket operation
    P2ERR_CONN_WINSOCK_INIT,            // Couldn't initialize WinSock
    P2ERR_CONN_SOCK_CLOSED,             // Socket closed
    P2ERR_CONN_SOCK_TIMEOUT,            // Timeout in socket operation
    // Common thread errors
    // (use GetLastError() to get extended information)
    P2ERR_CONN_THREAD_START,            // Couldn't start thread
    // Common IOCP errors
    // (use GetLastError() to get extended information)
    P2ERR_CONN_IOCP_CREATE,                // Couldn't create IOCP
    P2ERR_CONN_IOCP_ASSOCDEV,           // Couldn't associate device with IOCP
    P2ERR_CONN_IOCP_STATPOST,           // Couldn't post status through IOCP
    P2ERR_CONN_IOCP_STATGET,            // Couldn't get status from IOCP
    // Invalid object state for this operation
    P2ERR_CONN_OBJ_IS_DEAD,                // Object is dead (can be released)
    P2ERR_CONN_INVSTATE_LISTENER,        // Invalid state of listener
    P2ERR_CONN_INVSTATE_CONNECTION,     // Invalid state of connection
    P2ERR_CONN_INVSTATE_ENCRYPTOR,      // Invalid state of encryptor
    P2ERR_CONN_INVSTATE_MSGCONNECTION,     // Invalid state of msg-connection
    // No free keys in the specified range
    P2ERR_CONN_NO_FREE_KEYS,
    // Invalid IOCP key
    P2ERR_CONN_INVALID_KEY,
    // Couldn't start listener thread
    // (use GetLastError() to get extended information)
    P2ERR_CONN_THSTART_LISTENER,
    // Low level messages (LL)
    P2ERR_CONN_LL_INVALID_SYS_TYPE,     // Invalid system type
    P2ERR_CONN_LL_SIZE_TOOBIG,          // 
    P2ERR_CONN_LL_BAD_SIGNATURE,        //
    P2ERR_CONN_LL_WITHOUT_BODY,         //
//    P2ERR_CONN_LL_SIZE_INVALID,
    // Errors of MQ.dll
//    P2ERR_CONN_MQ_INCORRECT_CALL,
//    P2ERR_CONN_MQ_NOT_READY,
    // Reading INI settings
    P2ERR_CONN_INI_NO_REQ_VALUE,        // Couldn't read required value
    P2ERR_CONN_INI_INVALID_VALUE,
    P2ERR_CONN_LAST
};

// P2ERR_ROUTE
enum
{
    // роутер не может выдать маршрут до узла
    P2ERR_ROUTE_NO_ROUTE = P2ERR_ROUTE_BEGIN,
    // неправильно сформированный адрес
    P2ERR_ROUTE_INVALID_ADDRESS,
    // возвращает PostIfForeignAddress, если пакет предназначался 
    // не нам. 
    P2ERR_ROUTE_POSTED_TO_FOREIGN_ADDRESS,
    // возвращает GetRoute если адрес принадлежит нам
    P2ERR_ROUTE_ME_ITSELF,
    // возвращает SetNodeName при попытке присвоить имя не сбросив предыдущее
    P2ERR_ROUTE_NODE_NAME_YET_ASSIGNED,
    P2ERR_ROUTE_NO_FLAG_TO_ROUTE,
    P2ERR_ROUTE_LAST
};

// P2ERR_INI
enum
{
    P2ERR_INI_ALREADY_STARTED = P2ERR_INI_BEGIN,
    P2ERR_INI_NOT_STARTED,
    P2ERR_INI_WRITE_ERROR,
    // Полученное значение обрезано до заданных размеров
    P2ERR_INI_VALUE_TRUNCATED,
    P2ERR_INI_FILE_NOT_FOUND,
    P2ERR_INI_INVALID_INI_HANDLE,   // Invalid INI handle specified
//    P2ERR_INI_CANCEL_ENUM,          // Cancel enum call from callback function
//    <- use P2ERR_COMMON_CANCEL_ENUM instead
    P2ERR_INI_LAST
};

// P2ERR_LOG
enum
{
    P2ERR_LOG_ALREADY_OPENED = P2ERR_LOG_BEGIN,
    P2ERR_LOG_NOT_OPENED_YET,
    P2ERR_LOG_INVALID_INI_VALUE,
    P2ERR_LOG_INVALID_LOG_HANDLE,
    P2ERR_LOG_LAST
};

// P2ERR_MQ
enum
{
    P2ERR_MQ_ALREADY_INITIALIZED = P2ERR_MQ_BEGIN,   // MQ library already initialized
    P2ERR_MQ_NOT_INITIALIZED_YET,   // MQ library not initialized yet
    P2ERR_MQ_ALREADY_CONNECTED,     // Connection already connected
    P2ERR_MQ_NOT_CONNECTED_YET,
    P2ERR_MQ_TIMEOUT,
    P2ERR_MQ_INCORRECT_LLMSG,       // Received incorrect LL-message
    P2ERR_MQ_PROTOCOL_ERROR,        // Protocol MQ-P2router is broken
    P2ERR_MQ_ORIGMSG_NOT_SEND,      // Original message in the PostReply not a send-message
    P2ERR_MQ_ORIGMSG_BAD_ADDR_FROM, // Incorrect value in the field Addr_from of original message
    P2ERR_MQ_INVALID_CONNECTION,    // Connection must be reopened (Disconnect() and then Connect())
    P2ERR_MQ_INVALID_CONN_HANDLE,   // Invalid connection handle specified (by example, handle used after free)
    // Socket errors
    P2ERR_MQ_SOCK_CREATE,           // Couldn't create socket
    P2ERR_MQ_SOCK_BIND,                // Couldn't bind socket
    P2ERR_MQ_SOCK_MAKEADDR,           // Couldn't prepare socket address
    P2ERR_MQ_SOCK_CONNECT,            // Couldn't connect socket
    P2ERR_MQ_SOCK_ERROR,            // Error in socket operation
    P2ERR_MQ_WINSOCK_INIT,          // Couldn't initialize WinSock
    P2ERR_MQ_SOCK_CLOSED,           // Socket closed
    // Low level messages (LL)
    P2ERR_MQ_LL_INVALID_SYS_TYPE,   // Invalid system type
    P2ERR_MQ_LL_SIZE_TOOBIG,
    P2ERR_MQ_LL_BAD_SIGNATURE,
    // Other
    P2ERR_MQ_INVALID_DESTINATION,   // Invalid destination address
    P2ERR_MQ_CONNECTION_BUSY,       // 
    P2ERR_MQ_MULTGET_ABORTED,       // 
    P2ERR_MQ_ORIG_MSG_NOT_FOUND,    // Original(send) message not found for post reply
    P2ERR_MQ_NODE_NAME_CHANGED,
    P2ERR_MQ_UNEXPECTED_GRNT_CONF,
    P2ERR_MQ_GRNT_NOT_EXISTS,
    P2ERR_MQ_ACCESS_DENY,           //попытка несанкционированного доступа 
    P2ERR_MQ_NOT_ALL_POSTED,        //P2MsgPostAll : не все пакеты обработаны роутером
    P2ERR_MQ_LAST
};

// P2ERR_MQCRYPT
enum
{
    P2ERR_MQCRYPT_ALREADY_CONNECTED = P2ERR_MQCRYPT_BEGIN,
    P2ERR_MQCRYPT_CONN_NOT_READY,   // Connection not ready for this operation
    P2ERR_MQCRYPT_BAD_AUTH_INFO,    // Bad name or wrong password...
    P2ERR_MQCRYPT_NO_AUTH_INFO,
    P2ERR_MQCRYPT_ALREADY_LOGGED_IN, // Make logout before login
    P2ERR_MQCRYPT_LOGIN_ABORTED,    // Current login aborted by logout
    P2ERR_MQCRYPT_AS_NAME_NOT_SET,  // Name of AS not set in the INI
    P2ERR_MQCRYPT_ASDLL_NAME_NOT_SET,// Not set name of DLL for specified AS driver
    P2ERR_MQCRYPT_LOAD_ASDLL,       // Couldn't load DLL of AS driver
    P2ERR_MQCRYPT_NOT_ENCRYPTED_OR_BAD, // Data not encrypted or something wrong
    P2ERR_MQCRYPT_NOT_HEX_STRING,   // String is not hex representation of data
    P2ERR_MQCRYPT_LOGOUT_NOT_BY_OWNER,// Router already logged in by another application and can be logged out only by it
    P2ERR_MQCRYPT_LOGIN_NOT_PERMITTED,// Login not permitted for this application
    P2ERR_MQCRYPT_LAST
};

// P2ERR_DB
enum
{
    // ошибка открытия соединения
    P2ERR_DB_OPEN_ERROR = P2ERR_DB_BEGIN,
    P2ERR_DB_DRIVER_ERROR,
    // указано несуществующее имя поля
    P2ERR_DB_NO_SUCH_FIELD,
    P2ERR_DB_NO_SUCH_TABLE,
    P2ERR_DB_NOT_SUPPORTED_BY_DRIVER,
    P2ERR_DB_BAD_RECORDSET,
    P2ERR_DB_EOF,
    P2ERR_DB_EMPTY_SCHEME, // deprecated, use P2ERR_DB_WRONG_DB_SCHEME instead
    P2ERR_DB_WRONG_DB_SCHEME,
    P2ERR_DB_CANT_BE_DONE_WITHOUT_SCHEME,
    P2ERR_DB_INVALID_TYPE,
    // исполнение команды не привело к созданию рекордсета
    P2ERR_DB_NO_RECORDSET_RETURNED,
    P2ERR_DB_CANT_INSERT_EMPTY_RECORD,
    P2ERR_DB_CANT_CLOSE_WITH_ACTIVE_OBJECTS,
    P2ERR_DB_RECORD_NOT_FETCHED,
    P2ERR_DB_CANT_BE_DONE_WITH_SCHEME,
    P2ERR_DB_NO_RECORD,
    P2ERR_DB_COMMIT_WITHOUT_TRANSACTION,
    P2ERR_DB_COMMIT_AFTER_ROLLBACK,
    P2ERR_DB_ROLLBACK_WITHOUT_TRANSACTION,
    P2ERR_DB_NESTED_TRANSACTION_AFTER_ROLLBACK,
    P2ERR_DB_READONLY_RECORD,
    P2ERR_DB_KEY_FIELD_NOT_SET,
    P2ERR_DB_FIELD_NOT_SET,
    P2ERR_DB_DUPLICATE_KEYS,
    P2ERR_DB_FIELD_IS_NULL,
    P2ERR_DB_BUSY,
    P2ERR_DB_NOT_CONNECTED,
    P2ERR_DB_ALREADY_CONNECTED,
    P2ERR_DB_NO_SUCH_ITEM,
    P2ERR_DB_WRONG_ITEM_TYPE,
    P2ERR_DB_NO_SUCH_VIEW,
    P2ERR_DB_NO_SUCH_INDEX,
    P2ERR_DB_DATABASE_IS_CORRUPTED,
    P2ERR_DB_NON_UNIQUE_INDEX_VALUE,
    P2ERR_DB_UNIQUE_INDEX_REQUIRED,
    P2ERR_DB_CANT_TRUNCATE_IN_TRANSACTION,
    P2ERR_DB_CANT_CHANGE_UNDER_ENUM,
    P2ERR_DB_CANT_BE_DONE_WITHOUT_INDEX,
    P2ERR_DB_CANT_SAVE_IN_TRANSACTION,
    P2ERR_DB_WRONG_P2TIME_DATA_FORMAT,
    P2ERR_DB_CANT_LOAD_IN_TRANSACTION,
    P2ERR_DB_BUSY_NO_ROLLBACK,
    P2ERR_DB_ROLLBACK_REQUIRED,
    P2ERR_DB_NESTED_TRANSACTION,
    P2ERR_DB_CANT_SET_OPTIONS_IN_TRANSACTION,
    P2ERR_DB_TRANSACTION_REQUIRED,
    P2ERR_DB_INVALID_FIELD_DEFAULT_VALUE,
    P2ERR_DB_INVALID_SCHEME_URL, // ошибка разбора url в P2SchemeCreateSchemeFromUrl()
    P2ERR_DB_TABLE_ALREADY_EXIST,
    P2ERR_DB_DATABASE_ALREADY_EXIST,
    P2ERR_DB_NO_NATIVE_BUFFER,
    P2ERR_DB_LOAD_TRUNCATE_REQUIRED,
    P2ERR_DB_CANT_PROCESS_IN_TRANSACTION,
    P2ERR_DB_LAST
};

// P2ERR_SERV
enum
{
    // Specified service already registered for specified address
    P2ERR_SERV_ALREADY_REGISTERED = P2ERR_SERV_BEGIN,
    P2ERR_SERV_INCORRECT_EXCLUDE_ADDR,  // Incorrect exclude address specified for resolve
    P2ERR_SERV_NO_SERVICE,              // 
    P2ERR_SERV_LAST
};

// P2ERR_REPL
enum
{
    P2ERR_REPL_WRONG_STATE = P2ERR_REPL_BEGIN,
    P2ERR_REPL_SCHEME_ALREADY_SET,
    P2ERR_REPL_OBJECT_CHANGED__,                // Not error, for internal use only!
    P2ERR_REPL_SERVICE_WAS_UNREGISTERED,
    P2ERR_REPL_PROTOCOL_ERROR,
    P2ERR_REPL_TOO_MANY_CONNECTIONS,            // Превышено кол-во одновременных подключений от одного логина
    P2ERR_REPL_CHANNEL_DISCONNECT_ON_TIMEOUT,
    P2ERR_REPL_PROTOCOL_VERSION_MISMATCH,
    P2ERR_REPL_SERVER_SHUTDOWN,                 // Посылается клиентам при закрытии сервера
    P2ERR_REPL_ACCESS_DENIED,
    P2ERR_REPL_LIFE_NUMBER_MISMATCH,
    P2ERR_REPL_RECONNECT,
    P2ERR_REPL_WRONG_SCHEME,
    P2ERR_REPL_NO_SERVICE,
    P2ERR_REPL_TIMEOUT,
    P2ERR_REPL_REDIRECT_TO_SNAPSHOT_SERVER,
    P2ERR_REPL_INCORRECT_REVISION,
    P2ERR_REPL_CLIENT_INCORRECT_REVISION,
    P2ERR_REPL_LAST
};

enum
{
    P2ERR_CRYPT_NOTAVAIL = P2ERR_CRYPT_BEGIN,
    P2ERR_CRYPT_NOT_INITED,
    P2ERR_CRYPT_NOT_EXITING,
    P2ERR_CRYPT_POINTER,
    P2ERR_CRYPT_ERROR,
    P2ERR_CRYPT_RANDOM,
    P2ERR_CRYPT_UNKNOWN_CTX_TYPE,
    P2ERR_CRYPT_UNSUPPORTED_CTX_VERSION,
    P2ERR_CRYPT_INVALID_CTX,
    P2ERR_CRYPT_OKSPECIAL,
};

enum
{
    // Все ошибки P2ERR*, которые происходят на сервере
    // и никак не связаны с корректностью аутентификационной
    // информации, будут переданы удалённому узлу как
    // P2ERR_ASNS_INTERNAL. Дополнительная информация будет
    // содержаться в логах AS
    P2ERR_ASNS_INTERNAL = P2ERR_ASNS_BEGIN,
    P2ERR_ASNS_PROTOCOL_VERSION_MISMATCH,
    P2ERR_ASNS_CRYPT_ALGO,
    P2ERR_ASNS_CRYPT_MODE,
    // Неверный формат аутентификационного запроса
    P2ERR_ASNS_BAD_AUTHREQUEST,
    // Неизвестный инициатор (по нему нет информации на сервере)
    P2ERR_ASNS_INITIATOR_UNKNOWN,
    // некорректный пароль инициатора
    P2ERR_ASNS_INITIATOR_BAD,
    // Неизвестный акцептор (по нему нет информации на сервере)
    P2ERR_ASNS_ACCEPTOR_UNKNOWN,
    // некорректный пароль акцептора
    P2ERR_ASNS_ACCEPTOR_BAD,
    P2ERR_ASNS_IP_BLOCKED,
    P2ERR_ASNS_TOO_LONG_KEY,
    P2ERR_ASNS_TOO_SHORT_KEY,
    P2ERR_ASNS_CORRUPTED_KEY,
    P2ERR_ASNS_LAST,
};

// P2ERR_VM
enum
{
    P2ERR_VM_WRONG_LEXEME = P2ERR_VM_BEGIN,
    P2ERR_VM_SYNTAX_ERROR,
    P2ERR_VM_NUMERIC_OVERFLOW,
    P2ERR_VM_ILLEGAL_OPCODE,
    P2ERR_VM_VARIABLE_NOT_SET,
    P2ERR_VM_STACK_UNDERFLOW,
    P2ERR_VM_WRONG_VARIABLE_TYPE,
    P2ERR_VM_WRONG_BLOCK_FORMAT,
    P2ERR_VM_OPERATION_NOT_ALLOWED,
    P2ERR_VM_VAR_ZERO,
    P2ERR_VM_DLL_NOT_FOUND,
    P2ERR_VM_FUNCTION_NOT_FOUND,
    P2ERR_VM_DUPLICATED_EXT_FUNC,
    P2ERR_VM_STACK_BOUNDS_EXCEEDED,
    P2ERR_VM_FUNCTION_EXEC_ERROR,
    P2ERR_VM_LAST
};

enum
{
    P2ERR_MQASNS_PROTOCOL_VERSION_MISMATCH = P2ERR_MQASNS_BEGIN,
    P2ERR_MQASNS_CRYPT_POLITICS,
    P2ERR_MQASNS_CRYPT_ALGO,
    P2ERR_MQASNS_CRYPT_MODE,
    P2ERR_MQASNS_RANDOM_NUMBER,    // Random number not valid. Intrusion warning!
    P2ERR_MQASNS_TIME_MARK,        // Time mark is not valid. Intrusion warning!
    P2ERR_MQASNS_TOKEN,            // Invalid token format or broken token
    P2ERR_MQASNS_CRYPTO_PACKET,
    P2ERR_MQASNS_STATE_CHANGED,
    P2ERR_MQASNS_STATE_INVALID,
    P2ERR_MQASNS_CRC,
    P2ERR_MQASNS_TOKEN_EXPIRED,
    P2ERR_MQASNS_INVALID_IP,
    P2ERR_MQASNS_LAST
};

// P2ERR_ACL
enum
{
    P2ERR_ACL_ROLE_ALREADY_EXISTS = P2ERR_ACL_BEGIN,
    P2ERR_ACL_USER_ALREADY_EXISTS,
    P2ERR_ACL_UNKNOWN_ROLE,
    P2ERR_ACL_UNKNOWN_RIGHT,
    P2ERR_ACL_UNKNOWN_USER,
    P2ERR_ACL_BAD_BLOCK,
    P2ERR_ACL_UNUSED, // unused code
    P2ERR_ACL_COMPRESS,
    P2ERR_ACL_VERSION_MISMATCH,
    P2ERR_ACL_BAD_SIGNATURE,
    P2ERR_ACL_LAST
};

enum
{
    P2ERR_MQSTAT_ALREADY_REGISTERED = P2ERR_MQSTAT_BEGIN,
    P2ERR_MQSTAT_NOT_REGISTERED,
    P2ERR_MQSTAT_ALREADY_ANNOUNCED,
    P2ERR_MQSTAT_NOT_ANNOUNCED,
    P2ERR_MQSTAT_LAME_STATISTIC,
    P2ERR_MQSTAT_INVALID_QUERY,
    P2ERR_MQSTAT_INVALID_NAME,
    P2ERR_MQSTAT_INVALID_CATEGORY,
    P2ERR_MQSTAT_LAST
};

enum
{
    P2ERR_TEST_TIMEOUT = P2ERR_TEST_BEGIN,
    P2ERR_TEST_ERROR,
    P2ERR_TEST_LAST
};

enum
{
    P2ERR_ISAMSM_CANNOT_CREATE_FILE_TO_MAP = P2ERR_ISAMSM_BEGIN,
    P2ERR_ISAMSM_FILE_MAPPING_FAILED,
    P2ERR_ISAMSM_CANNOT_MAP_VIEW,
    P2ERR_ISAMSM_NESTED_TRANSACTIONS,
    P2ERR_ISAMSM__UNUSED_1,             // was P2ERR_ISAMSM_FREE_MEM_TIMEOUT
    P2ERR_ISAMSM_CREATE_EVENT_FAILED,
    P2ERR_ISAMSM__UNUSED_2,             // was P2ERR_ISAMSM_OPEN_EVENT_FAILED
    P2ERR_ISAMSM_CANNOT_START_THREAD,
    P2ERR_ISAMSM_REPL_PROCESS_FAILED,
    P2ERR_ISAMSM_EOF, //используется restorer'ом, когда он при чтении доходит до конца mapped-файла
    P2ERR_ISAMSM_ROLLBACK_REQUIRED, //сигнализирует об ошибочном состоянии БД
    P2ERR_ISAMSM_TRANSACTION_REQUIRED, //любое изменение БД необходимо осуществлять в транзакции
    P2ERR_ISAMSM_OPERATION_NOT_ALLOWED, //нельзя делать load или truncate после того, как P2ReplSharedMem'у были отданы данные для репликации
    P2ERR_ISAMSM_LOAD_TRUNCATE_REQUIRED,
    P2ERR_ISAMSM_FILE_VERSION_MISMATCH,
    P2ERR_ISAMSM_LAST
};

enum
{
    P2ERR_LRPCQ_WRITE_ALREADY = P2ERR_LRPCQ_BEGIN,
    P2ERR_LRPCQ_CLOSED,
    P2ERR_LRPCQ_TIMEOUT,
    P2ERR_LRPCQ_WRITE_NOT_STARTED,
    P2ERR_LRPCQ_NOWRITEABLE,
    P2ERR_LRPCQ_NOREADABLE,
    P2ERR_LRPCQ_CORRUPTED,
    P2ERR_LRPCQ_NO_SERVER,
    P2ERR_LRPCQ_VERSION_MISMATCH,
    P2ERR_LRPCQ_ALREADY_CONNECTED,
    P2ERR_LRPCQ_PEER_NOT_ALIVE
};


//
// При добавлении нового кода ошибки не забудь обновить описание!!!
// (Plaza-II\P2SysTypes\Source\P2ErrGetDescr.cpp):44
//

//////////////////////////////////////////////////////////////////////

// Check P2ERR result
#define P2FAILED(res)       ((res) != P2ERR_OK)
#define P2SUCCEEDED(res)    ((res) == P2ERR_OK)

// Save P2ERR result in the specified variable and check it
#define P2FAILEDV(res, var)     P2FAILED((var) = (res))
#define P2SUCCEEDEDV(res, var)  P2SUCCEEDED((var) = (res))

#define P2ERR_MUST_BE_FAILED(err)   \
    (P2FAILED(err) ? err : P2ERR_COMMON_INTERNAL_ERROR)

//////////////////////////////////////////////////////////////////////

#define P2ERR_EXIT_CRITICAL_ERROR   13

#endif // _P2ERRORS_H_
