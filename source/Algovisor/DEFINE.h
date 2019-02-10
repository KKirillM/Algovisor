#pragma once

#define BGRNCOLOR RGB(130,130,130)/*RGB(80,80,80)*/	//цвет фона для диалоговых окон

//дефайны для аспротекта
#define NOINLINE /*__declspec(noinline)*/
#define EXPORT_FUNC __declspec(dllexport)
#define PROTECT_FUNC NOINLINE EXPORT_FUNC

//#define GRIGORY_VERSION

#ifdef _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

#define APPLICATION_NAME_A	"Algovisor"
#define APPLICATION_NAME_W	L"Algovisor"
#define APPLICATION_NAME	_T("Algovisor")



#define PLAZA2_ROUTER_NAME_X64 ".\\Plaza2\\P2MQRouter64.exe"
#define PLAZA2_ROUTER_NAME_X86 ".\\Plaza2\\P2MQRouter.exe"

#define PLAZA2_ROUTER_PATH ".\\Plaza2\\"PLAZA2_ROUTER_NAME
#define PLAZA2_INI_PATH ".\\Plaza2\\plaza2_router.ini"
#define PLAZA2_CONNECTION_STRING "p2lrpcq://127.0.0.1:4001;local_timeout=5000;app_name=""APPLICATION_NAME_A"

//#ifdef _DEBUG
//#define PLAZA2_ENVIRONMENT_STRING "ini=.\\ini\\Algovisor.ini;key=11111111;minloglevel=trace"
//#else
#define PLAZA2_ENVIRONMENT_STRING "ini=.\\ini\\Algovisor.ini;key=QQwhCWvjBSqELa0t5keEJnMkxAeHBeH9;minloglevel=trace" // ключ для боевого подключения
//#endif

#define PLAZA2_REPLSTATEDB_PATH "\\Plaza2"


#define SERVER_PIPE_NAME _T("\\\\.\\PIPE\\Global\\ALGOVISOR_PIPE_SERVER")

#ifndef BAN_COPY_CONSTRUCTOR
#if (__cplusplus >= 201103L)
#define BAN_COPY_CONSTRUCTOR(class_name) \
    class_name(const class_name& ) = delete;\
    class_name& operator=(const class_name& ) = delete;
#else
#define BAN_COPY_CONSTRUCTOR(class_name) \
private:\
    class_name(const class_name& );\
    class_name& operator=(const class_name& );\
public:
#endif //(__cplusplus >= 201103L)
#endif

#ifndef PRIVATE_CONSTRUCTOR
#define PRIVATE_CONSTRUCTOR(class_name) \
private:	\
	class_name() {}	\
public:	
#endif

#ifndef DEBUG
//#define CONNECT_THROUGH_SERVER
#endif