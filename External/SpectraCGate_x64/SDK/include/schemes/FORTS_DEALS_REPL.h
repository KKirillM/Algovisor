#pragma pack(push, 4)

#ifndef _FORTS_DEALS_REPL_H_
#define _FORTS_DEALS_REPL_H_

// Scheme "FORTS_DEALS_REPL" description


namespace FORTS_DEALS_REPL
{



    struct deal
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int sess_id; // i4
        signed int isin_id; // i4
        signed long long id_deal; // i8
        signed int pos; // i4
        signed long long xpos; // i8
        signed int amount; // i4
        signed long long xamount; // i8
        signed long long id_ord_buy; // i8
        signed long long id_ord_sell; // i8
        char price[11]; // d16.5
        struct cg_time_t moment; // t
        unsigned long long moment_ns; // u8
        signed char nosystem; // i1
        
    };

    const size_t sizeof_deal = 116;
    const size_t deal_index = 0;


    struct multileg_deal
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int sess_id; // i4
        signed int isin_id; // i4
        signed long long id_deal; // i8
        signed long long id_ord_buy; // i8
        signed long long id_ord_sell; // i8
        signed int amount; // i4
        signed long long xamount; // i8
        char price[11]; // d16.5
        char rate_price[11]; // d16.5
        char swap_price[11]; // d16.5
        char buyback_amount[10]; // d16.2
        struct cg_time_t moment; // t
        unsigned long long moment_ns; // u8
        signed char nosystem; // i1
        
    };

    const size_t sizeof_multileg_deal = 136;
    const size_t multileg_deal_index = 1;


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

#endif //_FORTS_DEALS_REPL_H_

#pragma pack(pop)
