#pragma pack(push, 4)

#ifndef _FORTS_FUTAGGR50_REPL_H_
#define _FORTS_FUTAGGR50_REPL_H_

// Scheme "FORTS_FUTAGGR50_REPL" description


namespace FORTS_FUTAGGR50_REPL
{



    struct orders_aggr
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int isin_id; // i4
        char price[11]; // d16.5
        signed long long volume; // i8
        struct cg_time_t moment; // t
        signed char dir; // i1
        struct cg_time_t timestamp; // t
        signed int sess_id; // i4
        
    };

    const size_t sizeof_orders_aggr = 76;
    const size_t orders_aggr_index = 0;



}

#endif //_FORTS_FUTAGGR50_REPL_H_

#pragma pack(pop)
