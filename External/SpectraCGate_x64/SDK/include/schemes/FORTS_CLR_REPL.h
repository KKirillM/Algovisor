#pragma pack(push, 4)

#ifndef _FORTS_CLR_REPL_H_
#define _FORTS_CLR_REPL_H_

// Scheme "FORTS_CLR_REPL" description


namespace FORTS_CLR_REPL
{



    struct money_clearing
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        char client_code[8]; // c7
        signed char share; // i1
        char amount_beg[10]; // d16.2
        char vm[10]; // d16.2
        char premium[10]; // d16.2
        char pay[10]; // d16.2
        char fee_fut[10]; // d16.2
        char fee_opt[10]; // d16.2
        char go[10]; // d16.2
        char amount_end[13]; // d21.2
        char free[13]; // d22.2
        char pay_cl_limit[10]; // d16.2
        
    };

    const size_t sizeof_money_clearing = 140;
    const size_t money_clearing_index = 0;


    struct clr_rate
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        char id[13]; // c12
        char rate[11]; // d16.5
        struct cg_time_t moment; // t
        signed char signs; // i1
        signed int sess_id; // i4
        signed int rate_id; // i4
        
    };

    const size_t sizeof_clr_rate = 68;
    const size_t clr_rate_index = 1;


    struct fut_pos
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int isin_id; // i4
        signed int sess_id; // i4
        char isin[26]; // c25
        char client_code[8]; // c7
        signed char account; // i1
        signed int pos_beg; // i4
        signed long long xpos_beg; // i8
        signed int pos_end; // i4
        signed long long xpos_end; // i8
        char vm[10]; // d16.2
        char fee[10]; // d16.2
        char accum_go[10]; // d16.2
        char fee_ex[10]; // d16.2
        char vat_ex[10]; // d16.2
        char fee_cc[10]; // d16.2
        char vat_cc[10]; // d16.2
        char pos_exec[8]; // d11.0
        char sbor_exec[10]; // d16.2
        char sbor_nosys[10]; // d16.2
        char fee_exec[10]; // d16.2
        char fine_exec[10]; // d16.2
        char fee_trans[10]; // d16.2
        
    };

    const size_t sizeof_fut_pos = 220;
    const size_t fut_pos_index = 2;


    struct opt_pos
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int isin_id; // i4
        signed int sess_id; // i4
        char isin[26]; // c25
        char client_code[8]; // c7
        signed char account; // i1
        signed int pos_beg; // i4
        signed long long xpos_beg; // i8
        signed int pos_end; // i4
        signed long long xpos_end; // i8
        char vm[10]; // d16.2
        char fee[10]; // d16.2
        char fee_ex[10]; // d16.2
        char vat_ex[10]; // d16.2
        char fee_cc[10]; // d16.2
        char vat_cc[10]; // d16.2
        char pos_exec[8]; // d11.0
        char sbor_exec[10]; // d16.2
        char sbor_nosys[10]; // d16.2
        
    };

    const size_t sizeof_opt_pos = 180;
    const size_t opt_pos_index = 3;


    struct fut_sess_settl
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int sess_id; // i4
        struct cg_time_t date_clr; // t
        char isin[26]; // c25
        signed int isin_id; // i4
        char settl_price[11]; // d16.5
        char min_step[11]; // d16.5
        char step_price[11]; // d16.5
        char step_price_curr[11]; // d16.5
        
    };

    const size_t sizeof_fut_sess_settl = 112;
    const size_t fut_sess_settl_index = 4;


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
    const size_t opt_sess_settl_index = 5;


    struct pledge_details
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        char client_code[8]; // c7
        char pledge_name[11]; // c10
        char amount_beg[7]; // d10.0
        char xamount_beg[15]; // d26.2
        char pay[7]; // d10.0
        char xpay[15]; // d26.2
        char amount[7]; // d10.0
        char xamount[15]; // d26.2
        char rate[11]; // d16.5
        char amount_beg_money[10]; // d16.2
        char xamount_beg_money[15]; // d26.2
        char pay_money[10]; // d16.2
        char xpay_money[15]; // d26.2
        char amount_money[10]; // d16.2
        char xamount_money[15]; // d26.2
        signed char com_ensure; // i1
        
    };

    const size_t sizeof_pledge_details = 196;
    const size_t pledge_details_index = 6;


    struct money_clearing_sa
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        char settlement_account[13]; // c12
        signed char share; // i1
        char amount_beg[15]; // d26.2
        char vm[15]; // d26.2
        char premium[15]; // d26.2
        char pay[15]; // d26.2
        char fee_fut[15]; // d26.2
        char fee_opt[15]; // d26.2
        char go[15]; // d26.2
        char amount_end[15]; // d26.2
        char free[15]; // d26.2
        
    };

    const size_t sizeof_money_clearing_sa = 176;
    const size_t money_clearing_sa_index = 7;


    struct fut_pos_sa
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int isin_id; // i4
        signed int sess_id; // i4
        char isin[26]; // c25
        char settlement_account[13]; // c12
        signed int pos_beg; // i4
        signed long long xpos_beg; // i8
        signed int pos_end; // i4
        signed long long xpos_end; // i8
        char vm[15]; // d26.2
        char fee[15]; // d26.2
        char fee_ex[15]; // d26.2
        char vat_ex[15]; // d26.2
        char fee_cc[15]; // d26.2
        char vat_cc[15]; // d26.2
        
    };

    const size_t sizeof_fut_pos_sa = 188;
    const size_t fut_pos_sa_index = 8;


    struct opt_pos_sa
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int isin_id; // i4
        signed int sess_id; // i4
        char isin[26]; // c25
        char settlement_account[13]; // c12
        signed int pos_beg; // i4
        signed long long xpos_beg; // i8
        signed int pos_end; // i4
        signed long long xpos_end; // i8
        char vm[15]; // d26.2
        char fee[15]; // d26.2
        char fee_ex[15]; // d26.2
        char vat_ex[15]; // d26.2
        char fee_cc[15]; // d26.2
        char vat_cc[15]; // d26.2
        
    };

    const size_t sizeof_opt_pos_sa = 188;
    const size_t opt_pos_sa_index = 9;


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
    const size_t sys_events_index = 10;



}

#endif //_FORTS_CLR_REPL_H_

#pragma pack(pop)
