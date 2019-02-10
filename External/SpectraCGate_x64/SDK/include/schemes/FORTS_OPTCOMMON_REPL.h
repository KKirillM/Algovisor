#pragma pack(push, 4)

#ifndef _FORTS_OPTCOMMON_REPL_H_
#define _FORTS_OPTCOMMON_REPL_H_

// Scheme "FORTS_OPTCOMMON_REPL" description


namespace FORTS_OPTCOMMON_REPL
{



    struct common
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int sess_id; // i4
        signed int isin_id; // i4
        char best_buy[11]; // d16.5
        signed int amount_buy; // i4
        signed long long xamount_buy; // i8
        signed int orders_buy_qty; // i4
        signed int orders_buy_amount; // i4
        signed long long xorders_buy_amount; // i8
        char best_sell[11]; // d16.5
        signed int amount_sell; // i4
        signed long long xamount_sell; // i8
        signed int orders_sell_qty; // i4
        signed int orders_sell_amount; // i4
        signed long long xorders_sell_amount; // i8
        char open_price[11]; // d16.5
        char close_price[11]; // d16.5
        char price[11]; // d16.5
        char trend[11]; // d16.5
        signed int amount; // i4
        signed long long xamount; // i8
        struct cg_time_t deal_time; // t
        unsigned long long deal_time_ns; // u8
        char min_price[11]; // d16.5
        char max_price[11]; // d16.5
        char avr_price[11]; // d16.5
        signed int contr_count; // i4
        signed long long xcontr_count; // i8
        char capital[15]; // d26.2
        signed int deal_count; // i4
        char old_kotir[11]; // d16.5
        signed int pos; // i4
        signed long long xpos; // i8
        struct cg_time_t mod_time; // t
        unsigned long long mod_time_ns; // u8
        struct cg_time_t local_time; // t
        
    };

    const size_t sizeof_common = 312;
    const size_t common_index = 0;



}

#endif //_FORTS_OPTCOMMON_REPL_H_

#pragma pack(pop)
