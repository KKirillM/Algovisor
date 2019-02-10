#pragma pack(push, 4)

#ifndef _FORTS_ORDBOOK_REPL_H_
#define _FORTS_ORDBOOK_REPL_H_

// Scheme "FORTS_ORDBOOK_REPL" description


namespace FORTS_ORDBOOK_REPL
{



    struct orders
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed long long id_ord; // i8
        signed int sess_id; // i4
        struct cg_time_t moment; // t
        unsigned long long moment_ns; // u8
        signed long long xstatus; // i8
        signed int status; // i4
        signed char action; // i1
        signed int isin_id; // i4
        signed char dir; // i1
        char price[11]; // d16.5
        signed int amount; // i4
        signed long long xamount; // i8
        signed int amount_rest; // i4
        signed long long xamount_rest; // i8
        signed long long id_deal; // i8
        char deal_price[11]; // d16.5
        struct cg_time_t init_moment; // t
        signed int init_amount; // i4
        signed long long xinit_amount; // i8
        
    };

    const size_t sizeof_orders = 156;
    const size_t orders_index = 0;


    struct info
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed long long infoID; // i8
        signed long long logRev; // i8
        signed int lifeNum; // i4
        struct cg_time_t moment; // t
        
    };

    const size_t sizeof_info = 56;
    const size_t info_index = 1;



}

#endif //_FORTS_ORDBOOK_REPL_H_

#pragma pack(pop)
