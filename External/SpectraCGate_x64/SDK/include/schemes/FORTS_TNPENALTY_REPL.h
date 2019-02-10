#pragma pack(push, 4)

#ifndef _FORTS_TNPENALTY_REPL_H_
#define _FORTS_TNPENALTY_REPL_H_

// Scheme "FORTS_TNPENALTY_REPL" description


namespace FORTS_TNPENALTY_REPL
{



    struct fee_all
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed long long time; // i8
        char p2login[65]; // c64
        signed int sess_id; // i4
        signed int points; // i4
        signed long long s1; // i8
        signed long long s2; // i8
        char fee[10]; // d16.2
        
    };

    const size_t sizeof_fee_all = 136;
    const size_t fee_all_index = 0;


    struct fee_tn
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed long long time; // i8
        char p2login[65]; // c64
        signed int sess_id; // i4
        signed int tn_type; // i4
        signed int err_code; // i4
        signed int count; // i4
        
    };

    const size_t sizeof_fee_tn = 116;
    const size_t fee_tn_index = 1;



}

#endif //_FORTS_TNPENALTY_REPL_H_

#pragma pack(pop)
