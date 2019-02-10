#pragma pack(push, 4)

#ifndef _MOEX_RATES_REPL_H_
#define _MOEX_RATES_REPL_H_

// Scheme "MOEX_RATES_REPL" description


namespace MOEX_RATES_REPL
{



    struct curr_online
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int rate_id; // i4
        char value[11]; // d16.5
        struct cg_time_t moment; // t
        
    };

    const size_t sizeof_curr_online = 52;
    const size_t curr_online_index = 0;



}

#endif //_MOEX_RATES_REPL_H_

#pragma pack(pop)
