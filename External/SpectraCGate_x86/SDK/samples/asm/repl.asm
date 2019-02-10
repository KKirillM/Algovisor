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

            DUMP_MESSAGE_BODY = 1
            DUMP_MESSAGE_BUFFER_SIZE equ 2048

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

            fexit  dd FALSE

; #########################################################################

.code

start:      invoke main
            invoke ExitProcess, 0

; #########################################################################

            envopenparams   db 'ini=repl.ini;subsystems=mq,replclient;key=11111111', 0
            connopenparams  db 'p2tcp://127.0.0.1:4001;app_name=test1', 0
            lsnopenparams   db 'p2repl://FORTS_FUTINFO_REPL', 0

; #########################################################################

CtrlHandler proc C CtrlType:DWORD

            log_info "Break signaled"

            mov    eax, TRUE
            mov    fexit, eax
            ret

CtrlHandler endp

; #########################################################################

MessageCallback proc C hconn:DWORD, hlistener:DWORD, msg:DWORD, data:DWORD

            LOCAL scheme    :DWORD

            mov    eax, msg
            .if [eax].cg_msg_t.msg_type == CG_MSG_STREAM_DATA
                    IFDEF DUMP_MESSAGE_BODY
                            dump_message msg, cg_lsn_getscheme, hlistener, "Message dump: %s"
                            .if eax == CG_ERR_BUFFERTOOSMALL
                                    mov    eax, msg
                                    log_info "DATA message too large for dump: [table:[%d-%s], dataSize:%d]", [eax].cg_msg_streamdata_t.msg_index, [eax].cg_msg_streamdata_t.msg_name, [eax].cg_msg_streamdata_t.data_size
                            .endif
                    ELSE
                            log_info "DATA message [table:[%d-%s], dataSize:%d]", [eax].cg_msg_streamdata_t.msg_index, [eax].cg_msg_streamdata_t.msg_name, [eax].cg_msg_streamdata_t.data_size
                    ENDIF

            .elseif [eax].cg_msg_t.msg_type == CG_MSG_OPEN
                    log_info "CG_MSG_OPEN"

                    mov    scheme, 0
                    invoke cg_lsn_getscheme, hlistener, ADDR scheme

                    .if scheme != 0

                            push   esi
                            push   edi

                            mov    esi, [scheme]
                            mov    esi, [esi].cg_scheme_desc_t.messages

                            .while esi != 0
                                    log_info "Message %s, block size = %d", [esi].cg_message_desc_t.msg_name, [esi].cg_message_desc_t.msg_size

                                    mov    edi, [esi].cg_message_desc_t.fields
                                    .while edi != 0
                                            log_info "        Field %s = %s", [edi].cg_field_desc_t.field_name, [edi].cg_field_desc_t.field_type
                                            mov    edi, [edi].cg_field_desc_t.next
                                    .endw

                                    mov    esi, [esi].cg_message_desc_t.next
                            .endw

                            pop    edi
                            pop    esi

                    .endif

            .elseif [eax].cg_msg_t.msg_type == CG_MSG_TN_BEGIN
                    log_info "CG_MSG_TN_BEGIN"

            .elseif [eax].cg_msg_t.msg_type == CG_MSG_TN_COMMIT
                    log_info "CG_MSG_TN_COMMIT"

            .elseif [eax].cg_msg_t.msg_type == CG_MSG_P2REPL_ONLINE
                    log_info "CG_MSG_P2REPL_ONLINE"

            .elseif [eax].cg_msg_t.msg_type == CG_MSG_P2REPL_LIFENUM
                    mov    eax, [eax].cg_msg_t.data
                    log_info "Life number changed to: %d", dword ptr [eax]

            .elseif [eax].cg_msg_t.msg_type == CG_MSG_P2REPL_REPLSTATE
                    log_info "Replica state: %s", [eax].cg_msg_t.data

            .elseif [eax].cg_msg_t.msg_type == CG_MSG_CLOSE
                    log_info "CG_MSG_CLOSE"

            .else
                    log_info "Message: 0x%X", eax

            .endif

            mov    eax, CG_ERR_OK
            ret

MessageCallback endp

; #########################################################################

main        proc

            LOCAL hConn     :DWORD
            LOCAL connstate :DWORD
            LOCAL hLsn      :DWORD
            LOCAL lsnstate  :DWORD

            invoke SetConsoleCtrlHandler, CtrlHandler, TRUE
            invoke SetConsoleOutputCP, 1251

            invoke cg_env_open, OFFSET envopenparams

            log_info "Program started..."

            mov    hConn, 0
            mov    hLsn, 0

@@main01:   .if hConn == 0
                    invoke cg_conn_new, OFFSET connopenparams, ADDR hConn
            .else
                    invoke cg_conn_getstate, hConn, ADDR connstate

                    mov    eax, connstate
                    .if eax == CG_STATE_ACTIVE
                            invoke cg_conn_process, hConn, 100, NULL

                            .if hLsn == 0
                                invoke cg_lsn_new, hConn, OFFSET lsnopenparams, MessageCallback, NULL, ADDR hLsn
                            .else
                                invoke cg_lsn_getstate, hLsn, ADDR lsnstate
                                mov eax, lsnstate
                                .if eax == CG_STATE_CLOSED
                                        invoke cg_lsn_open, hLsn, NULL
                                .elseif eax == CG_STATE_ERROR
                                        invoke cg_lsn_close, hLsn
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
