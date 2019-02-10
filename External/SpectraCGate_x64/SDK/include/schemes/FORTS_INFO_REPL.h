#pragma pack(push, 4)

#ifndef _FORTS_INFO_REPL_H_
#define _FORTS_INFO_REPL_H_

// Scheme "FORTS_INFO_REPL" description


namespace FORTS_INFO_REPL
{



    struct currency_params
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int currency_id; // i4
        double radius; // f
        signed char signs; // i1
        
    };

    const size_t sizeof_currency_params = 40;
    const size_t currency_params_index = 0;


    struct base_contracts_params
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        char code_vcb[26]; // c25
        char code_mcs[26]; // c25
        signed char volat_num; // i1
        signed char points_num; // i1
        double subrisk_step; // f
        signed char is_percent; // i1
        signed char has_options; // i1
        char percent_rate[11]; // d16.5
        char currency_volat[11]; // d16.5
        signed char is_usd; // i1
        double somc; // f
        signed char msp_type; // i1
        signed int currency_id; // i4
        
    };

    const size_t sizeof_base_contracts_params = 132;
    const size_t base_contracts_params_index = 1;


    struct futures_params
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        char isin[26]; // c25
        signed int isin_id; // i4
        char code_vcb[26]; // c25
        double limit; // f
        char settl_price[11]; // d16.5
        signed char spread_aspect; // i1
        signed char subrisk; // i1
        double step_price; // f
        struct cg_time_t exp_date; // t
        char settl_price_real[11]; // d16.5
        double min_step; // f
        signed int lot; // i4
        signed char spot_signs; // i1
        
    };

    const size_t sizeof_futures_params = 156;
    const size_t futures_params_index = 2;


    struct virtual_futures_params
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        char isin[26]; // c25
        char isin_base[26]; // c25
        signed char is_net_positive; // i1
        double volat_range; // f
        double t_squared; // f
        double max_addrisk; // f
        double a; // f
        double b; // f
        double c; // f
        double d; // f
        double e; // f
        double s; // f
        struct cg_time_t exp_date; // t
        signed char use_null_volat; // i1
        double strike_step; // f
        
    };

    const size_t sizeof_virtual_futures_params = 172;
    const size_t virtual_futures_params_index = 3;


    struct options_params
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        char isin[26]; // c25
        signed int isin_id; // i4
        char isin_base[26]; // c25
        char strike[11]; // d16.5
        signed char opt_type; // i1
        char settl_price[11]; // d16.5
        
    };

    const size_t sizeof_options_params = 108;
    const size_t options_params_index = 4;


    struct common_params
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int common_rev; // i4
        double edge_coeff; // f
        
    };

    const size_t sizeof_common_params = 36;
    const size_t common_params_index = 5;


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
    const size_t sys_events_index = 6;



}

#endif //_FORTS_INFO_REPL_H_

#pragma pack(pop)
