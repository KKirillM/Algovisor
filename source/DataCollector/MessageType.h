#pragma once

namespace MessageType
{
//#define FORTS_DEALS_REPL_START      0x00000100
//#define FORTS_ORDLOG_REPL_START     0x00000200
//#define FORTS_DEALS_REPL_START      0x00000300
//#define FORTS_ORDLOG_REPL_START     0x00000400
//#define FORTS_FUTINFO_REPL_START    0x00000500
//#define FORTS_OPTINFO_REPL_START    0x00000600
//#define FORTS_FUTCOMMON_REPL_START  0x00000700
//#define FORTS_OPTCOMMON_REPL_START  0x00000800
//#define FORTS_MM_REPL_START         0x00000900
//#define RTS_INDEX_REPL_START        0x00000A00
//#define FORTS_MISCINFO_REPL_START   0x00000B00
//#define FORTS_VOLAT_REPL_START      0x00000C00

	enum MessageType : UINT
	{
        MT_FORTS_UNKNOWN,

		MT_FORTS_DEALS_REPL_deal /*= FORTS_DEALS_REPL_START*/,
		MT_FORTS_DEALS_REPL_multileg_deal,
		MT_FORTS_DEALS_REPL_heartbeat,
		MT_FORTS_DEALS_REPL_sys_events,

		MT_FORTS_ORDLOG_REPL_orders_log /*= FORTS_ORDLOG_REPL_START*/,
		MT_FORTS_ORDLOG_REPL_multileg_orders_log,
		MT_FORTS_ORDLOG_REPL_heartbeat,
		MT_FORTS_ORDLOG_REPL_sys_events,

        MT_FORTS_FUTINFO_REPL_delivery_report,
        MT_FORTS_FUTINFO_REPL_fut_rejected_orders,
        MT_FORTS_FUTINFO_REPL_fut_intercl_info,
        MT_FORTS_FUTINFO_REPL_fut_bond_registry,
        MT_FORTS_FUTINFO_REPL_fut_bond_isin,
        MT_FORTS_FUTINFO_REPL_fut_bond_nkd,
        MT_FORTS_FUTINFO_REPL_fut_bond_nominal,
        MT_FORTS_FUTINFO_REPL_fut_ts_cons,
        MT_FORTS_FUTINFO_REPL_usd_online,
        MT_FORTS_FUTINFO_REPL_fut_vcb,
        MT_FORTS_FUTINFO_REPL_session,
        MT_FORTS_FUTINFO_REPL_multileg_dict,
        MT_FORTS_FUTINFO_REPL_trade_modes,
        MT_FORTS_FUTINFO_REPL_fut_sess_contents,
        MT_FORTS_FUTINFO_REPL_fut_instruments,
        MT_FORTS_FUTINFO_REPL_diler,
        MT_FORTS_FUTINFO_REPL_investr,
        MT_FORTS_FUTINFO_REPL_fut_sess_settl,
        MT_FORTS_FUTINFO_REPL_sys_messages,
        MT_FORTS_FUTINFO_REPL_trf_accounts,
        MT_FORTS_FUTINFO_REPL_money_transfers,
        MT_FORTS_FUTINFO_REPL_trf_accounts_map,
        MT_FORTS_FUTINFO_REPL_prohibition,
        MT_FORTS_FUTINFO_REPL_rates,
        MT_FORTS_FUTINFO_REPL_fut_settlement_account,
        MT_FORTS_FUTINFO_REPL_fut_margin_type,
        MT_FORTS_FUTINFO_REPL_sys_events,

        MT_FORTS_OPTINFO_REPL_opt_rejected_orders,
        MT_FORTS_OPTINFO_REPL_opt_intercl_info,
        MT_FORTS_OPTINFO_REPL_opt_exp_orders,
        MT_FORTS_OPTINFO_REPL_opt_vcb,
        MT_FORTS_OPTINFO_REPL_opt_sess_contents,
        MT_FORTS_OPTINFO_REPL_opt_sess_settl,
        MT_FORTS_OPTINFO_REPL_opt_exprule,
        MT_FORTS_OPTINFO_REPL_sys_events,

        MT_FORTS_FUTCOMMON_REPL_common,
        
        MT_FORTS_OPTCOMMON_REPL_common,

        MT_FORTS_MM_REPL_fut_MM_info,
        MT_FORTS_MM_REPL_opt_MM_info,
        MT_FORTS_MM_REPL_cs_mm_rule,
        MT_FORTS_MM_REPL_mm_agreement_filter,
      
        MT_RTS_INDEX_REPL_rts_index,

        MT_FORTS_MISCINFO_REPL_volat_coeff,

        MT_FORTS_VOLAT_REPL_volat

	}; // MessageType enum
}