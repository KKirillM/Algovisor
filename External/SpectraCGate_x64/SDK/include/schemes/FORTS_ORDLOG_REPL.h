#pragma pack(push, 4)

#ifndef _FORTS_ORDLOG_REPL_H_
#define _FORTS_ORDLOG_REPL_H_

// Scheme "FORTS_ORDLOG_REPL" description


namespace FORTS_ORDLOG_REPL
{



    struct orders_log
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed long long id_ord; // i8
        signed int sess_id; // i4
        signed int isin_id; // i4
        signed int amount; // i4
        signed long long xamount; // i8
        signed int amount_rest; // i4
        signed long long xamount_rest; // i8
        signed long long id_deal; // i8
        signed long long xstatus; // i8
        signed int status; // i4
        char price[11]; // d16.5
        struct cg_time_t moment; // t
        unsigned long long moment_ns; // u8
        signed char dir; // i1
        signed char action; // i1
        char deal_price[11]; // d16.5
        
    };

    const size_t sizeof_orders_log = 132;
    const size_t orders_log_index = 0;


    struct multileg_orders_log
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed long long id_ord; // i8
        signed int sess_id; // i4
        signed int isin_id; // i4
        signed int amount; // i4
        signed long long xamount; // i8
        signed int amount_rest; // i4
        signed long long xamount_rest; // i8
        signed long long id_deal; // i8
        signed long long xstatus; // i8
        signed int status; // i4
        char price[11]; // d16.5
        struct cg_time_t moment; // t
        unsigned long long moment_ns; // u8
        signed char dir; // i1
        signed char action; // i1
        char deal_price[11]; // d16.5
        char rate_price[11]; // d16.5
        char swap_price[11]; // d16.5
        
    };

    const size_t sizeof_multileg_orders_log = 152;
    const size_t multileg_orders_log_index = 1;


    struct heartbeat
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        struct cg_time_t server_time; // t
        
    };

    const size_t sizeof_heartbeat = 36;
    const size_t heartbeat_index = 2;


    struct sys_events
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed long long event_id; // i8
        signed int sess_id; // i4
        signed int event_type; // i4
        char message[65]; // c64
        
    };

    const size_t sizeof_sys_events = 108;
    const size_t sys_events_index = 3;



}

#endif //_FORTS_ORDLOG_REPL_H_

#pragma pack(pop)
