#pragma pack(push, 4)

#ifndef _FORTS_OPTINFO_REPL_H_
#define _FORTS_OPTINFO_REPL_H_

// Scheme "FORTS_OPTINFO_REPL" description


namespace FORTS_OPTINFO_REPL
{



    struct opt_exprule
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        char broker_code[5]; // c4
        char code_vcb[26]; // c25
        signed int fut_isin_id; // i4
        signed int X_type; // i4
        char X[11]; // d16.5
        signed int is_deny_out_money; // i4
        char exp_price[11]; // d16.5
        signed int pr_active; // i4
        char stamp[8]; // b8
        
    };

    const size_t sizeof_opt_exprule = 104;
    const size_t opt_exprule_index = 0;


    struct opt_sess_contents
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int sess_id; // i4
        signed int isin_id; // i4
        char isin[26]; // c25
        char short_isin[26]; // c25
        char name[76]; // c75
        char code_vcb[26]; // c25
        signed int fut_isin_id; // i4
        signed char is_limited; // i1
        char limit_up[11]; // d16.5
        char limit_down[11]; // d16.5
        char old_kotir[11]; // d16.5
        char bgo_c[10]; // d16.2
        char bgo_nc[10]; // d16.2
        signed char europe; // i1
        signed char put; // i1
        char strike[11]; // d16.5
        signed int roundto; // i4
        char min_step[11]; // d16.5
        signed int lot_volume; // i4
        char step_price[11]; // d16.5
        struct cg_time_t d_pg; // t
        struct cg_time_t d_exec_beg; // t
        struct cg_time_t d_exec_end; // t
        signed int signs; // i4
        char last_cl_quote[11]; // d16.5
        char bgo_buy[10]; // d16.2
        signed int base_isin_id; // i4
        struct cg_time_t d_start; // t
        signed int sort_order; // i4
        char exch_pay[10]; // d16.2
        char volat_intercl[11]; // d16.5
        
    };

    const size_t sizeof_opt_sess_contents = 396;
    const size_t opt_sess_contents_index = 1;


    struct opt_vcb
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        char code_vcb[26]; // c25
        char name[76]; // c75
        char exec_type[2]; // c1
        char curr[4]; // c3
        char trade_scheme[2]; // c1
        char coeff_out[6]; // d7.3
        signed int min_vol; // i4
        signed int rate_id; // i4
        
    };

    const size_t sizeof_opt_vcb = 148;
    const size_t opt_vcb_index = 2;


    struct opt_rejected_orders
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed long long order_id; // i8
        signed int sess_id; // i4
        struct cg_time_t moment; // t
        signed int isin_id; // i4
        char client_code[8]; // c7
        signed char dir; // i1
        signed int amount; // i4
        signed long long xamount; // i8
        char price[11]; // d16.5
        struct cg_time_t date_exp; // t
        signed long long id_ord1; // i8
        struct cg_time_t moment_reject; // t
        signed int ret_code; // i4
        char ret_message[256]; // c255
        char comment[21]; // c20
        char login_from[21]; // c20
        signed int ext_id; // i4
        
    };

    const size_t sizeof_opt_rejected_orders = 428;
    const size_t opt_rejected_orders_index = 3;


    struct opt_intercl_info
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int isin_id; // i4
        char client_code[8]; // c7
        char vm_intercl[10]; // d16.2
        
    };

    const size_t sizeof_opt_intercl_info = 48;
    const size_t opt_intercl_info_index = 4;


    struct opt_exp_orders
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed long long exporder_id; // i8
        char client_code[8]; // c7
        signed int isin_id; // i4
        signed int amount; // i4
        signed long long xamount; // i8
        signed int sess_id; // i4
        struct cg_time_t date; // t
        signed int amount_apply; // i4
        signed long long xamount_apply; // i8
        
    };

    const size_t sizeof_opt_exp_orders = 84;
    const size_t opt_exp_orders_index = 5;


    struct opt_sess_settl
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int sess_id; // i4
        struct cg_time_t date_clr; // t
        char isin[26]; // c25
        signed int isin_id; // i4
        char volat[11]; // d16.5
        char theor_price[11]; // d16.5
        
    };

    const size_t sizeof_opt_sess_settl = 92;
    const size_t opt_sess_settl_index = 6;


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
    const size_t sys_events_index = 7;



}

#endif //_FORTS_OPTINFO_REPL_H_

#pragma pack(pop)
