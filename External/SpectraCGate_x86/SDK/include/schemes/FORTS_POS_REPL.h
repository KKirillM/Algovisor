#pragma pack(push, 4)

#ifndef _FORTS_POS_REPL_H_
#define _FORTS_POS_REPL_H_

// Scheme "FORTS_POS_REPL" description


namespace FORTS_POS_REPL
{



    struct position
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        char client_code[8]; // c7
        signed int isin_id; // i4
        signed int pos; // i4
        signed int buys_qty; // i4
        signed int sells_qty; // i4
        signed int open_qty; // i4
        char waprice[11]; // d16.5
        char net_volume[16]; // d26.5
        char net_volume_rur[15]; // d26.2
        signed char opt_type; // i1
        signed long long last_deal_id; // i8
        
    };

    const size_t sizeof_position = 104;
    const size_t position_index = 0;


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
    const size_t sys_events_index = 1;



}

#endif //_FORTS_POS_REPL_H_

#pragma pack(pop)
