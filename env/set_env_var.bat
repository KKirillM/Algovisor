@echo off
REM устанавливаем переменные окружения для внешних заголовочных файлов и lib-файлов

popd

set ALGOVISOR_EXTERNAL_PATH=%CD%\External\

set STX_INCLUDE=%ALGOVISOR_EXTERNAL_PATH%stx\
set GOOGLE_INCLUDE=%ALGOVISOR_EXTERNAL_PATH%google\

set ASPROTECT_INCLUDE=%ALGOVISOR_EXTERNAL_PATH%ASProtect\include\
set ASPROTECT_LIB=%ALGOVISOR_EXTERNAL_PATH%ASProtect\lib\

set CURL_INCLUDE_X86=%ALGOVISOR_EXTERNAL_PATH%Curl\curl-7.38.0-win32\include\curl\
set CURL_LIB_X86=%ALGOVISOR_EXTERNAL_PATH%Curl\curl-7.38.0-win32\lib\
set CURL_INCLUDE_X64=%ALGOVISOR_EXTERNAL_PATH%Curl\curl-7.38.0-win64\curl_include\curl\
set CURL_LIB_X64=%ALGOVISOR_EXTERNAL_PATH%Curl\curl-7.38.0-win64\curl_lib\

rem set RITHMIC_INCLUDE=%ALGOVISOR_EXTERNAL_PATH%Rithmic API\8.5.0.0\include\
rem set RITHMIC_LIB=%ALGOVISOR_EXTERNAL_PATH%Rithmic API\8.5.0.0\win7\lib\
set RITHMIC_INCLUDE=%ALGOVISOR_EXTERNAL_PATH%Rithmic API\9.5.0.0\include\
rem set RITHMIC_LIB=%ALGOVISOR_EXTERNAL_PATH%Rithmic API\9.5.0.0\win7\lib\
set RITHMIC_LIB=%ALGOVISOR_EXTERNAL_PATH%Rithmic API\9.5.0.0\win10\lib\

set CRUSHREPORT_INCLUDE=%ALGOVISOR_EXTERNAL_PATH%CrashReport\include\
set CRUSHREPORT_LIB_X86=%ALGOVISOR_EXTERNAL_PATH%CrashReport\lib\
set CRUSHREPORT_LIB_X64=%ALGOVISOR_EXTERNAL_PATH%CrashReport\lib\x64\

set OPENSSL_INCLUDE=%ALGOVISOR_EXTERNAL_PATH%openssl\

set RAPIDJSON_INCLUDE=%ALGOVISOR_EXTERNAL_PATH%rapidjson\

set PLAZA2_CGATE_INCLUDE_X64=%ALGOVISOR_EXTERNAL_PATH%SpectraCGate_x64\SDK\include\
set PLAZA2_CGATE_LIB_X64=%ALGOVISOR_EXTERNAL_PATH%SpectraCGate_x64\SDK\lib\
set PLAZA2_CGATE_INCLUDE_x86=%ALGOVISOR_EXTERNAL_PATH%SpectraCGate_x86\SDK\include\
set PLAZA2_CGATE_LIB_X86=%ALGOVISOR_EXTERNAL_PATH%SpectraCGate_x86\SDK\lib\

set PLAZA2_API_INCLUDE=%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II\OptStr\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II\P2ACL\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II\P2Common\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II\P2LRPC\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II\P2LRPCQ\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II\P2Sys\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II\P2SysExt\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II\P2SysIni\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II\P2SysLog\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II\P2SysMsg\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II\P2SysP1Parser\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II\P2SysParser\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II\P2SysRecord\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II\P2SysRecordParser\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II\P2SysScheme\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II\P2SysStructParser\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II\P2SysTypes\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II\P2Utils\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II\P2VM\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II\P2WatchDog\Include\
set PLAZA2_API_APPS_INCLUDE=%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-Apps\P2BaseApp\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-Apps\P2NewEventServer\Include\
set PLAZA2_API_CRYPT_INCLUDE=%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-Crypt\P2AuthServerNS2\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-Crypt\P2Crypt\Include\
set PLAZA2_API_DB_INCLUDE=%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-DB\P2DB\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-DB\P2DBODBC\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-DB\P2DBSQLite3\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-DB\P2DBUtils\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-DB\P2ISAM\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-DB\P2ISAMSharedMem\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-DB\P2ReplISAMCache\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-DB\P2ReplISAMFFDB\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-DB\P2Tbl\Include\
set PLAZA2_API_EREPL_INCLUDE=%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-eRepl\P2eReplClient\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-eRepl\P2eReplCommon\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-eRepl\P2eReplRecorder\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-eRepl\P2eReplServer\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-eRepl\P2eReplStorage\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-eRepl\P2eReplUtils\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-eRepl\P2eSharedMem\Include\
set PLAZA2_API_P2MQ_INCLUDE=%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-MQ\EventQueue\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-MQ\P2MQ\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-MQ\P2MQLocPwds\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-MQ\P2MQRouter\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-MQ\P2MQRouterCrypto\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-MQ\P2MQRouterDll\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-MQ\P2MQRouterPM\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-MQ\P2MQRouterReplica\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-MQ\P2MQRouterReply\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-MQ\P2MQRouterRoute\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-MQ\P2MQRouterServices\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-MQ\P2MQRouterTimeSync\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-MQ\P2MQRouterUser\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-MQ\P2MQRouterUtils\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-MQ\P2MQStat\Include\
set PLAZA2_API_REPL_INCLUDE=%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-Repl\P2ACLReplClient\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-Repl\P2ReplClient2\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-Repl\P2ReplClientTools\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-Repl\P2ReplCommon\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-Repl\P2ReplServer\Include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\Plaza-II-Repl\P2ReplUtils\Include\
set PLAZA2_API_THIRDPARTY_INCLUDE=%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\ThirdParty\Lzo2\include\;%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\include\ThirdParty\Snappy\Include\
set PLAZA2_API_LIB_X86=%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\lib\
set PLAZA2_API_LIB_X64=%ALGOVISOR_EXTERNAL_PATH%Plaza-II API\lib\

set ASTS_API_LIB_X86=%ALGOVISOR_EXTERNAL_PATH%astsbridge-4.2.3.1135\mtesrl\win32\
set ASTS_API_LIB_X64=%ALGOVISOR_EXTERNAL_PATH%astsbridge-4.2.3.1135\mtesrl\win64\
set ASTS_API_EMBEDDED_LIB_X86=%ALGOVISOR_EXTERNAL_PATH%astsbridge-4.2.3.1135\embedded\win86\
set ASTS_API_EMBEDDED_LIB_X64=%ALGOVISOR_EXTERNAL_PATH%astsbridge-4.2.3.1135\embedded\win64\

@echo on