#pragma pack(push, 4)

#ifndef _FORTS_FEE_REPL_H_
#define _FORTS_FEE_REPL_H_

// Scheme "FORTS_FEE_REPL" description


namespace FORTS_FEE_REPL
{



    struct adjusted_fee
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed long long id_deal; // i8
        struct cg_time_t moment; // t
        unsigned long long moment_ns; // u8
        char code_buy[8]; // c7
        char code_sell[8]; // c7
        char initial_fee_buy[15]; // d26.2
        char initial_fee_sell[15]; // d26.2
        char adjusted_fee_buy[15]; // d26.2
        char adjusted_fee_sell[15]; // d26.2
        signed long long id_repo; // i8
        signed long long id_deal_multileg; // i8
        
    };

    const size_t sizeof_adjusted_fee = 144;
    const size_t adjusted_fee_index = 0;


    struct sys_events
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed long long event_id; // i8
        signed int sess_id; // i4
        signed int event_type; // i4
        char message[65]; // c64
        struct cg_time_t server_time; // t
        
    };

    const size_t sizeof_sys_events = 116;
    const size_t sys_events_index = 1;



}

#endif //_FORTS_FEE_REPL_H_

#pragma pack(pop)
