#pragma pack(push, 4)

#ifndef _FORTS_FUTINFO_REPL_H_
#define _FORTS_FUTINFO_REPL_H_

// Scheme "FORTS_FUTINFO_REPL" description


namespace FORTS_FUTINFO_REPL
{



    struct rates
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int rate_id; // i4
        char curr_base[16]; // c15
        char curr_coupled[16]; // c15
        char radius[11]; // d16.5
        
    };

    const size_t sizeof_rates = 72;
    const size_t rates_index = 0;


    struct fut_sess_contents
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int sess_id; // i4
        signed int isin_id; // i4
        char short_isin[26]; // c25
        char isin[26]; // c25
        char name[76]; // c75
        signed int inst_term; // i4
        char code_vcb[26]; // c25
        signed char is_limited; // i1
        char limit_up[11]; // d16.5
        char limit_down[11]; // d16.5
        char old_kotir[11]; // d16.5
        char buy_deposit[10]; // d16.2
        char sell_deposit[10]; // d16.2
        signed int roundto; // i4
        char min_step[11]; // d16.5
        signed int lot_volume; // i4
        char step_price[11]; // d16.5
        struct cg_time_t d_pg; // t
        signed char is_spread; // i1
        struct cg_time_t d_exp; // t
        signed char is_percent; // i1
        char percent_rate[5]; // d6.2
        char last_cl_quote[11]; // d16.5
        signed int signs; // i4
        signed char is_trade_evening; // i1
        signed int ticker; // i4
        signed int state; // i4
        signed char price_dir; // i1
        signed int multileg_type; // i4
        signed int legs_qty; // i4
        char step_price_clr[11]; // d16.5
        char step_price_interclr[11]; // d16.5
        char step_price_curr[11]; // d16.5
        struct cg_time_t d_start; // t
        signed int sort_order; // i4
        signed char vm_calc_type; // i1
        char old_step_price[11]; // d16.5
        char min_step_curr[11]; // d16.5
        char volat_min[13]; // d20.15
        char volat_max[13]; // d20.15
        signed int isin_id_rd; // i4
        signed int isin_id_rb; // i4
        char exch_pay[11]; // d16.5
        char price_intercl[11]; // d16.5
        char pctyield_coeff[11]; // d16.5
        char pctyield_total[11]; // d16.5
        
    };

    const size_t sizeof_fut_sess_contents = 496;
    const size_t fut_sess_contents_index = 1;


    struct fut_vcb
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        char code_vcb[26]; // c25
        char name[76]; // c75
        char exec_type[2]; // c1
        char curr[4]; // c3
        char trade_scheme[2]; // c1
        char section[51]; // c50
        signed int rate_id; // i4
        char SECCODE[13]; // c12
        signed char is_foreign; // i1
        
    };

    const size_t sizeof_fut_vcb = 208;
    const size_t fut_vcb_index = 2;


    struct fut_instruments
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int isin_id; // i4
        char short_isin[26]; // c25
        char isin[26]; // c25
        char name[76]; // c75
        signed int inst_term; // i4
        char code_vcb[26]; // c25
        signed char is_limited; // i1
        char old_kotir[11]; // d16.5
        signed int roundto; // i4
        char min_step[11]; // d16.5
        signed int lot_volume; // i4
        char step_price[11]; // d16.5
        struct cg_time_t d_pg; // t
        signed char is_spread; // i1
        struct cg_time_t d_exp; // t
        signed char is_percent; // i1
        char percent_rate[5]; // d6.2
        char last_cl_quote[11]; // d16.5
        signed int signs; // i4
        char volat_min[13]; // d20.15
        char volat_max[13]; // d20.15
        signed char price_dir; // i1
        signed int multileg_type; // i4
        signed int legs_qty; // i4
        char step_price_clr[11]; // d16.5
        char step_price_interclr[11]; // d16.5
        char step_price_curr[11]; // d16.5
        struct cg_time_t d_start; // t
        signed char vm_calc_type; // i1
        char old_step_price[11]; // d16.5
        char min_step_curr[11]; // d16.5
        signed char is_limit_opt; // i1
        char limit_up_opt[5]; // d5.2
        char limit_down_opt[5]; // d5.2
        char adm_lim[11]; // d16.5
        char adm_lim_offmoney[11]; // d16.5
        signed char apply_adm_limit; // i1
        char pctyield_coeff[11]; // d16.5
        char pctyield_total[11]; // d16.5
        char exec_name[2]; // c1
        
    };

    const size_t sizeof_fut_instruments = 440;
    const size_t fut_instruments_index = 3;


    struct fut_bond_registry
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int bond_id; // i4
        char small_name[26]; // c25
        char short_isin[26]; // c25
        char name[76]; // c75
        struct cg_time_t date_redempt; // t
        char nominal[11]; // d16.5
        signed int bond_type; // i4
        signed short year_base; // i2
        
    };

    const size_t sizeof_fut_bond_registry = 188;
    const size_t fut_bond_registry_index = 4;


    struct diler
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        char client_code[8]; // c7
        char name[201]; // c200
        char rts_code[51]; // c50
        signed int signs; // i4
        signed int status; // i4
        char transfer_code[8]; // c7
        char exp_weight[4]; // d3.2
        signed int num_clr_2delivery; // i4
        signed char margin_type; // i1
        signed char calendar_spread_margin_type; // i1
        signed int num_clr_2delivery_client_default; // i4
        char exp_weight_client_default[4]; // d3.2
        char go_ratio[11]; // d16.5
        signed char check_limit_on_withdrawal; // i1
        signed char limit_tied_money; // i1
        signed char limits_set; // i1
        signed char no_fut_discount; // i1
        signed char no_fut_discount_client_default; // i1
        
    };

    const size_t sizeof_diler = 336;
    const size_t diler_index = 5;


    struct sys_messages
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int msg_id; // i4
        struct cg_time_t moment; // t
        char lang_code[9]; // c8
        signed char urgency; // i1
        signed char status; // i1
        char text[256]; // c255
        struct cg_time_t cancel_moment; // t
        char message_body[4001]; // c4000
        
    };

    const size_t sizeof_sys_messages = 4320;
    const size_t sys_messages_index = 6;


    struct prohibition
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int prohib_id; // i4
        char client_code[8]; // c7
        signed int initiator; // i4
        char section[51]; // c50
        char code_vcb[26]; // c25
        signed int isin_id; // i4
        signed int priority; // i4
        signed long long group_mask; // i8
        signed int type; // i4
        signed int is_legacy; // i4
        
    };

    const size_t sizeof_prohibition = 144;
    const size_t prohibition_index = 7;


    struct RF
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        char code[3]; // c2
        signed int op_input; // i4
        signed int rf_input; // i4
        
    };

    const size_t sizeof_RF = 36;
    const size_t RF_index = 8;


    struct trade_modes
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int trade_mode_id; // i4
        signed int status; // i4
        
    };

    const size_t sizeof_trade_modes = 32;
    const size_t trade_modes_index = 9;


    struct money_transfers
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int id; // i4
        char client_code[8]; // c7
        char amount[10]; // d16.2
        struct cg_time_t moment; // t
        signed int state; // i4
        struct cg_time_t moment_accepted; // t
        char rcv_name[201]; // c200
        char tn_account[21]; // c20
        char bank[201]; // c200
        char corr_account[21]; // c20
        char bic[21]; // c20
        char vatin[13]; // c12
        signed int sess_id; // i4
        signed char type; // i1
        
    };

    const size_t sizeof_money_transfers = 556;
    const size_t money_transfers_index = 10;


    struct trf_accounts
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        char client_code[8]; // c7
        char name[201]; // c200
        char tn_account[35]; // c34
        char bank[201]; // c200
        char corr_account[35]; // c34
        char bic[21]; // c20
        char vatin[51]; // c50
        signed char is_multicode; // i1
        
    };

    const size_t sizeof_trf_accounts = 580;
    const size_t trf_accounts_index = 11;


    struct trf_accounts_map
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        char client_code[8]; // c7
        char tn_account[35]; // c34
        
    };

    const size_t sizeof_trf_accounts_map = 68;
    const size_t trf_accounts_map_index = 12;


    struct multileg_dict
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int sess_id; // i4
        signed int isin_id; // i4
        signed int isin_id_leg; // i4
        signed int qty_ratio; // i4
        unsigned char leg_order_no; // u1
        
    };

    const size_t sizeof_multileg_dict = 44;
    const size_t multileg_dict_index = 13;


    struct fut_ts_cons
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        char code_ts[3]; // c2
        struct cg_time_t date_last_clear; // t
        struct cg_time_t date_last_open_day; // t
        
    };

    const size_t sizeof_fut_ts_cons = 48;
    const size_t fut_ts_cons_index = 14;


    struct fut_rejected_orders
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

    const size_t sizeof_fut_rejected_orders = 428;
    const size_t fut_rejected_orders_index = 15;


    struct fut_intercl_info
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int isin_id; // i4
        char client_code[8]; // c7
        char vm_intercl[10]; // d16.2
        
    };

    const size_t sizeof_fut_intercl_info = 48;
    const size_t fut_intercl_info_index = 16;


    struct fut_bond_nkd
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int bond_id; // i4
        struct cg_time_t date; // t
        char nkd[11]; // d16.7
        signed char is_cupon; // i1
        
    };

    const size_t sizeof_fut_bond_nkd = 52;
    const size_t fut_bond_nkd_index = 17;


    struct fut_bond_nominal
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int bond_id; // i4
        struct cg_time_t date; // t
        char nominal[11]; // d16.5
        char face_value[11]; // d16.5
        char coupon_nominal[7]; // d8.5
        signed char is_nominal; // i1
        
    };

    const size_t sizeof_fut_bond_nominal = 68;
    const size_t fut_bond_nominal_index = 18;


    struct fut_bond_isin
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int isin_id; // i4
        signed int bond_id; // i4
        char coeff_conversion[5]; // d5.4
        
    };

    const size_t sizeof_fut_bond_isin = 40;
    const size_t fut_bond_isin_index = 19;


    struct usd_online
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed long long id; // i8
        char rate[10]; // d16.4
        struct cg_time_t moment; // t
        
    };

    const size_t sizeof_usd_online = 52;
    const size_t usd_online_index = 20;


    struct investr
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        char client_code[8]; // c7
        char name[201]; // c200
        signed int status; // i4
        signed char calendar_spread_margin_type; // i1
        
    };

    const size_t sizeof_investr = 244;
    const size_t investr_index = 21;


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
        
    };

    const size_t sizeof_fut_sess_settl = 80;
    const size_t fut_sess_settl_index = 22;


    struct fut_margin_type
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        char code[13]; // c12
        signed char type; // i1
        signed char margin_type; // i1
        signed char UCP_type; // i1
        char prohibit_coeff[10]; // d16.2
        signed int prohibit_type; // i4
        signed char settlement_account_type; // i1
        
    };

    const size_t sizeof_fut_margin_type = 60;
    const size_t fut_margin_type_index = 23;


    struct fut_settlement_account
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        char code[8]; // c7
        signed char type; // i1
        char settlement_account[13]; // c12
        
    };

    const size_t sizeof_fut_settlement_account = 48;
    const size_t fut_settlement_account_index = 24;


    struct fut_view_market_data_spot_instrument
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        char code_vcb[26]; // c25
        char code_CLRL[26]; // c25
        char code_md[13]; // c12
        
    };

    const size_t sizeof_fut_view_market_data_spot_instrument = 92;
    const size_t fut_view_market_data_spot_instrument_index = 25;


    struct session
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int sess_id; // i4
        struct cg_time_t begin; // t
        struct cg_time_t end; // t
        signed int state; // i4
        signed int opt_sess_id; // i4
        struct cg_time_t inter_cl_begin; // t
        struct cg_time_t inter_cl_end; // t
        signed int inter_cl_state; // i4
        signed char eve_on; // i1
        struct cg_time_t eve_begin; // t
        struct cg_time_t eve_end; // t
        signed char mon_on; // i1
        struct cg_time_t mon_begin; // t
        struct cg_time_t mon_end; // t
        struct cg_time_t pos_transfer_begin; // t
        struct cg_time_t pos_transfer_end; // t
        signed char cleared; // i1
        
    };

    const size_t sizeof_session = 148;
    const size_t session_index = 26;


    struct dealer
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        char client_code[8]; // c7
        char name[201]; // c200
        char rts_code[51]; // c50
        signed int signs; // i4
        signed int status; // i4
        char transfer_code[8]; // c7
        char exp_weight[4]; // d3.2
        signed int num_clr_2delivery; // i4
        signed char margin_type; // i1
        signed char calendar_spread_margin_type; // i1
        signed int num_clr_2delivery_client_default; // i4
        char exp_weight_client_default[4]; // d3.2
        char go_ratio[11]; // d16.5
        signed char check_limit_on_withdrawal; // i1
        signed char limit_tied_money; // i1
        signed char limits_set; // i1
        signed char no_fut_discount; // i1
        signed char no_fut_discount_client_default; // i1
        
    };

    const size_t sizeof_dealer = 336;
    const size_t dealer_index = 27;


    struct investor
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        char client_code[8]; // c7
        char name[201]; // c200
        signed int status; // i4
        signed char calendar_spread_margin_type; // i1
        
    };

    const size_t sizeof_investor = 244;
    const size_t investor_index = 28;


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
    const size_t sys_events_index = 29;



}

#endif //_FORTS_FUTINFO_REPL_H_

#pragma pack(pop)
