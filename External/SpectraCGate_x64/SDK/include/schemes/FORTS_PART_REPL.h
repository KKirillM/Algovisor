#pragma pack(push, 4)

#ifndef _FORTS_PART_REPL_H_
#define _FORTS_PART_REPL_H_

// Scheme "FORTS_PART_REPL" description


namespace FORTS_PART_REPL
{



    struct part
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        char client_code[8]; // c7
        char money_free[15]; // d26.2
        char money_blocked[15]; // d26.2
        char pledge_free[15]; // d26.2
        char pledge_blocked[15]; // d26.2
        char vm_reserve[15]; // d26.2
        char fee[15]; // d26.2
        char balance_money[15]; // d26.2
        char coeff_go[11]; // d16.5
        char coeff_liquidity[11]; // d16.5
        signed char limits_set; // i1
        char money_old[15]; // d26.2
        char money_amount[15]; // d26.2
        char pledge_old[15]; // d26.2
        char pledge_amount[15]; // d26.2
        char money_pledge_amount[15]; // d26.2
        char vm_intercl[15]; // d26.2
        signed char is_auto_update_limit; // i1
        signed char no_fut_discount; // i1
        signed int num_clr_2delivery; // i4
        char exp_weight[4]; // d3.2
        
    };

    const size_t sizeof_part = 260;
    const size_t part_index = 0;


    struct part_sa
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        char settlement_account[13]; // c12
        char money_amount[15]; // d26.2
        char money_free[15]; // d26.2
        char pledge_amount[15]; // d26.2
        char money_pledge_amount[15]; // d26.2
        char liquidity_ratio[11]; // d16.5
        
    };

    const size_t sizeof_part_sa = 108;
    const size_t part_sa_index = 1;


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
    const size_t sys_events_index = 2;



}

#endif //_FORTS_PART_REPL_H_

#pragma pack(pop)
