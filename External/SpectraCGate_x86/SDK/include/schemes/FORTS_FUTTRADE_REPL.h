#pragma pack(push, 4)

#ifndef _FORTS_FUTTRADE_REPL_H_
#define _FORTS_FUTTRADE_REPL_H_

// Scheme "FORTS_FUTTRADE_REPL" description


namespace FORTS_FUTTRADE_REPL
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
        char client_code[8]; // c7
        char login_from[21]; // c20
        char comment[21]; // c20
        signed char hedge; // i1
        signed char trust; // i1
        signed int ext_id; // i4
        char broker_to[8]; // c7
        char broker_to_rts[8]; // c7
        char broker_from_rts[8]; // c7
        struct cg_time_t date_exp; // t
        signed long long id_ord1; // i8
        signed char is_gate; // i1
        struct cg_time_t local_stamp; // t
        
    };

    const size_t sizeof_orders_log = 244;
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
        char client_code[8]; // c7
        char login_from[21]; // c20
        char comment[21]; // c20
        signed char hedge; // i1
        signed char trust; // i1
        signed int ext_id; // i4
        char broker_to[8]; // c7
        char broker_to_rts[8]; // c7
        char broker_from_rts[8]; // c7
        struct cg_time_t date_exp; // t
        signed long long id_ord1; // i8
        signed char is_gate; // i1
        signed int isin_id_repo; // i4
        signed int isin_id_rd; // i4
        signed int isin_id_rb; // i4
        signed int duration; // i4
        struct cg_time_t local_stamp; // t
        
    };

    const size_t sizeof_multileg_orders_log = 284;
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


    struct user_deal
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int sess_id; // i4
        signed int isin_id; // i4
        signed long long id_deal; // i8
        signed long long id_deal_multileg; // i8
        signed long long id_repo; // i8
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
        signed long long xstatus_buy; // i8
        signed long long xstatus_sell; // i8
        signed int status_buy; // i4
        signed int status_sell; // i4
        signed int ext_id_buy; // i4
        signed int ext_id_sell; // i4
        char code_buy[8]; // c7
        char code_sell[8]; // c7
        char comment_buy[21]; // c20
        char comment_sell[21]; // c20
        signed char trust_buy; // i1
        signed char trust_sell; // i1
        signed char hedge_buy; // i1
        signed char hedge_sell; // i1
        char fee_buy[15]; // d26.2
        char fee_sell[15]; // d26.2
        char login_buy[21]; // c20
        char login_sell[21]; // c20
        char code_rts_buy[8]; // c7
        char code_rts_sell[8]; // c7
        
    };

    const size_t sizeof_user_deal = 316;
    const size_t user_deal_index = 4;


    struct user_multileg_deal
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int sess_id; // i4
        signed int isin_id; // i4
        signed int isin_id_rd; // i4
        signed int isin_id_rb; // i4
        signed int isin_id_repo; // i4
        signed int duration; // i4
        signed long long id_deal; // i8
        signed long long id_deal_rd; // i8
        signed long long id_deal_rb; // i8
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
        signed long long xstatus_buy; // i8
        signed long long xstatus_sell; // i8
        signed int status_buy; // i4
        signed int status_sell; // i4
        signed int ext_id_buy; // i4
        signed int ext_id_sell; // i4
        char code_buy[8]; // c7
        char code_sell[8]; // c7
        char comment_buy[21]; // c20
        char comment_sell[21]; // c20
        signed char trust_buy; // i1
        signed char trust_sell; // i1
        signed char hedge_buy; // i1
        signed char hedge_sell; // i1
        char login_buy[21]; // c20
        char login_sell[21]; // c20
        char code_rts_buy[8]; // c7
        char code_rts_sell[8]; // c7
        
    };

    const size_t sizeof_user_multileg_deal = 320;
    const size_t user_multileg_deal_index = 5;



}

#endif //_FORTS_FUTTRADE_REPL_H_

#pragma pack(pop)
