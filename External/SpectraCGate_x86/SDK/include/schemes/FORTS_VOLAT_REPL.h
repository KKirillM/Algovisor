#pragma pack(push, 4)

#ifndef _FORTS_VOLAT_REPL_H_
#define _FORTS_VOLAT_REPL_H_

// Scheme "FORTS_VOLAT_REPL" description


namespace FORTS_VOLAT_REPL
{



    struct volat
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int isin_id; // i4
        signed int sess_id; // i4
        char volat_[11]; // d16.5
        char theor_price[11]; // d16.5
        char theor_price_limit[11]; // d16.5
        char up_prem[11]; // d16.5
        char down_prem[11]; // d16.5
        
    };

    const size_t sizeof_volat = 88;
    const size_t volat_index = 0;



}

#endif //_FORTS_VOLAT_REPL_H_

#pragma pack(pop)
