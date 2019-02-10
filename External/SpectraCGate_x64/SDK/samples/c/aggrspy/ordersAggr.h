#pragma pack(push, 4)

#ifndef _scheme_H_
#define _scheme_H_

// Scheme "scheme" description




    struct orders_aggr
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed char opt_type; // i1
        signed int isin_id; // i4 ---
        char price[11]; // d16.5 ---
        signed long long volume; // i8 ---
        struct cg_time_t moment; // t
        signed char dir; // i1
        struct cg_time_t timestamp; // t
        signed int sess_id; // i4 ---
        signed long long rev; // i8 ---
        
    };

    const size_t sizeof_orders_aggr = 88;
    const size_t orders_aggr_index = 0;



#endif //_scheme_H_

#pragma pack(pop)
