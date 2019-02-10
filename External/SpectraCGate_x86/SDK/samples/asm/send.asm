; #########################################################################

            .586
            .model flat, stdcall
            option casemap :none   ; case sensitive

; #########################################################################

            include     \masm32\include\windows.inc
            include     \masm32\include\kernel32.inc
            include     ..\..\include\cgate.inc

            includelib  \masm32\lib\kernel32.lib
            includelib  ..\..\lib\cgate.lib

; #########################################################################

FutAddOrder_t STRUCT 4
  _broker_code      BYTE 5    dup(?)
  _isin             BYTE 26   dup(?)
  _client_code      BYTE 4    dup(?)
  _type             DWORD     ?
  _dir              DWORD     ?
  _amount           DWORD     ?
  _price            BYTE 18   dup(?)
  _comment          BYTE 21   dup(?)
  _broker_to        BYTE 21   dup(?)
  _ext_id           DWORD     ?
  _du               DWORD     ?
  _date_exp         BYTE 9    dup(?)
  _hedge            DWORD     ?
  _dont_check_money DWORD     ?
  _local_stamp      cg_time_t <>
FutAddOrder_t ends

; #########################################################################

            DUMP_MESSAGE_BUFFER_SIZE equ 2048
            MESSAGE_RESEND_INTERVAL equ 1000   ; send message once per second

; #########################################################################

            log_info MACRO fmt, args:VARARG
              LOCAL Txt
              .data
                Txt db fmt,0
              .code
              IFB <args>
                invoke cg_log_infostr, ADDR Txt
              ELSE
                invoke cg_log_info, ADDR Txt, args
              ENDIF
            ENDM

            set_string MACRO dest, source
              LOCAL Txt
              LOCAL @@set01
              .data
                Txt db source, 0
              .code
                        lea    edi, dest
                        lea    esi, Txt
                        cld
              @@set01:  lodsb
                        stosb 
                        test   al, al
                        jnz    @@set01
            ENDM

            dump_message MACRO msg, schemefunc, hobject, message_text
              LOCAL scheme
              LOCAL bufsize
              .data
                scheme  dd ?
                bufsize dd ?
              .code
                        mov    bufsize, DUMP_MESSAGE_BUFFER_SIZE

                        mov    scheme, 0
                        invoke schemefunc, hobject, ADDR scheme

                        push   esi
                        sub    esp, DUMP_MESSAGE_BUFFER_SIZE
                        mov    esi, esp
                        
                        invoke cg_msg_dump, msg, scheme, esi, ADDR bufsize
                        .if eax == CG_ERR_OK
                                log_info message_text, esi
                        .endif
                      
                        add    esp, DUMP_MESSAGE_BUFFER_SIZE
                        pop    esi
            ENDM
            
            main        proto

; #########################################################################

.data
            fexit     dd FALSE

; #########################################################################

.code

start:      invoke main
            invoke ExitProcess, 0

; #########################################################################

            envopenparams   db 'ini=send.ini;subsystems=mq,replclient;key=11111111', 0
            connopenparams  db 'p2tcp://127.0.0.1:4001;app_name=test1', 0
            lsnopenparams   db 'p2mqreply://;ref=srvlink', 0
            pubopenparams   db 'p2mq://FORTS_SRV;category=FORTS_MSG;name=srvlink;timeout=5000;scheme=|FILE|forts_messages.ini|message', 0

; #########################################################################

CtrlHandler proc C CtrlType:DWORD

            log_info "Break signaled"

            mov    eax, TRUE
            mov    fexit, eax
            ret

CtrlHandler endp

; #########################################################################

MessageCallback proc C hconn:DWORD, hlistener:DWORD, msg:DWORD, data:DWORD

            mov    eax, msg
            .if [eax].cg_msg_t.msg_type == CG_MSG_DATA
                    dump_message msg, cg_lsn_getscheme, hlistener, "Reply message dump: %s"

            .elseif [eax].cg_msg_t.msg_type == CG_MSG_P2MQ_TIMEOUT
                    log_info "Client reply TIMEOUT"

            .else
                    log_info "Message: 0x%X", eax

            .endif

            mov    eax, CG_ERR_OK
            ret

MessageCallback endp

; #########################################################################

            addordertrsname db 'FutAddOrder', 0

send_request proc C hPub :DWORD, counter :DWORD

            LOCAL msg       :DWORD

            invoke cg_pub_msgnew, hPub, CG_KEY_NAME, OFFSET addordertrsname, ADDR msg

            push   esi
            push   edi

            mov    edx, msg
            mov    eax, counter
            mov    [edx].cg_msg_data_t.user_id, eax

            mov    edx, [edx].cg_msg_data_t.data

            set_string [edx].FutAddOrder_t._isin, "RTS-6.12"
            set_string [edx].FutAddOrder_t._client_code, "000"
            mov    [edx].FutAddOrder_t._type, 1
            mov    [edx].FutAddOrder_t._dir, 1
            mov    [edx].FutAddOrder_t._amount, 1
            set_string [edx].FutAddOrder_t._price, "155050"
            mov    [edx].FutAddOrder_t._ext_id, 0;

            pop    edi
            pop    esi

            dump_message msg, cg_pub_getscheme, hPub, "Outgoing message dump: %s"

            invoke cg_pub_post, hPub, msg, CG_PUB_NEEDREPLY
            invoke cg_pub_msgfree, hPub, msg

            ret
send_request endp


; #########################################################################

main        proc

            LOCAL hConn          :DWORD
            LOCAL connstate      :DWORD
            LOCAL hLsn           :DWORD
            LOCAL hPub           :DWORD
            LOCAL state          :DWORD
            LOCAL last_send_time :DWORD
            LOCAL mesage_counter :DWORD

            invoke SetConsoleCtrlHandler, CtrlHandler, TRUE
            invoke SetConsoleOutputCP, 1251

            invoke cg_env_open, OFFSET envopenparams

            log_info "Program started..."

            mov    hConn, 0
            mov    hLsn, 0
            mov    hPub, 0
            mov    mesage_counter, 0
            mov    last_send_time, 0

@@main01:   .if hConn == 0
                    invoke cg_conn_new, OFFSET connopenparams, ADDR hConn
            .else
                    invoke cg_conn_getstate, hConn, ADDR connstate

                    mov    eax, connstate
                    .if eax == CG_STATE_ACTIVE
                            invoke cg_conn_process, hConn, 100, NULL

                            .if hLsn == 0
                                invoke cg_lsn_new, hConn, OFFSET lsnopenparams, MessageCallback, NULL, ADDR hLsn
                            .endif

                            .if hPub == 0
                                invoke cg_pub_new, hConn, OFFSET pubopenparams, ADDR hPub
                            .else
                                invoke cg_pub_getstate, hPub, ADDR state
                                mov eax, state
                                .if eax == CG_STATE_ACTIVE
                                        .if hLsn != 0

                                                invoke cg_lsn_getstate, hLsn, ADDR state
                                                mov eax, state
                                                .if eax == CG_STATE_ACTIVE
                                                        invoke GetTickCount
                                                        mov    ecx, eax
                                                        sub    ecx, [last_send_time]
                                                        cmp    ecx, MESSAGE_RESEND_INTERVAL
                                                        jl     @@main02
                                                        mov    [last_send_time], eax
                                                        inc    [mesage_counter]
                                                        invoke send_request, hPub, mesage_counter
@@main02:
                                                .elseif eax == CG_STATE_CLOSED
                                                        invoke cg_lsn_open, hLsn, NULL
                                                .elseif eax == CG_STATE_ERROR
                                                        invoke cg_lsn_close, hLsn
                                                .endif

                                        .endif
                                .elseif eax == CG_STATE_CLOSED
                                        invoke cg_pub_open, hPub, NULL
                                .elseif eax == CG_STATE_ERROR
                                        invoke cg_pub_close, hPub
                                .endif
                            .endif

                    .elseif eax == CG_STATE_CLOSED
                            invoke cg_conn_open, hConn, NULL
                    .elseif eax == CG_STATE_ERROR
                            invoke cg_conn_close, hConn
                    .elseif eax == CG_STATE_OPENING
                            invoke cg_conn_process, hConn, 100, NULL
                    .endif
            .endif

            cmp    fexit, TRUE
            jnz    @@main01

            .if hPub != 0
                    invoke cg_pub_close, hPub
                    invoke cg_pub_destroy, hPub
            .endif

            .if hLsn != 0
                    invoke cg_lsn_close, hLsn
                    invoke cg_lsn_destroy, hLsn
            .endif

            .if hConn != 0
                    invoke cg_conn_close, hConn
                    invoke cg_conn_destroy, hConn
            .endif

            log_info "Closing environment and quit..."

            invoke cg_env_close

            ret

main        endp

end start
