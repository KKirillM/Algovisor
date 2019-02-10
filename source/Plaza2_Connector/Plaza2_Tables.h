#pragma once

#define CG_SHORT_NAMES
#include "cgate.h"

//��� ������������ ����� ��� ������������ �������� schmemetool �� SDK Plaza2
#include "schemes\FORTS_CLR_REPL.h"
#include "schemes\FORTS_DEALS_REPL.h"
#include "schemes\FORTS_FEE_REPL.h"
#include "schemes\FORTS_FUTAGGR20_REPL.h"
#include "schemes\FORTS_FUTAGGR50_REPL.h"
#include "schemes\FORTS_FUTAGGR5_REPL.h"
#include "schemes\FORTS_FUTCOMMON_REPL.h"
#include "schemes\FORTS_FUTINFO_REPL.h"
#include "schemes\FORTS_FUTORDERBOOK_REPL.h"
#include "schemes\FORTS_FUTTRADE_REPL.h"
#include "schemes\FORTS_INFO_REPL.h"
#include "schemes\FORTS_MISCINFO_REPL.h"
#include "schemes\FORTS_MM_REPL.h"
#include "schemes\FORTS_OPTCOMMON_REPL.h"
#include "schemes\FORTS_OPTINFO_REPL.h"
#include "schemes\FORTS_OPTORDERBOOK_REPL.h"
#include "schemes\FORTS_OPTTRADE_REPL.h"
#include "schemes\FORTS_ORDLOG_REPL.h"
#include "schemes\FORTS_ORDBOOK_REPL.h"
#include "schemes\FORTS_PART_REPL.h"
#include "schemes\FORTS_POS_REPL.h"
#include "schemes\FORTS_TNPENALTY_REPL.h"
#include "schemes\FORTS_VM_REPL.h"
#include "schemes\FORTS_VOLAT_REPL.h"		//������ � �����, �� �������������
#include "schemes\MOEX_RATES_REPL.h"
#include "schemes\RTS_INDEX_REPL.h"
#include "schemes\RTS_INDEXLOG_REPL.h"

//������ ������ ���� ��������� ���� ����������
//#define orders_log
//#define multileg_orders_log
//#define deal
//#define multileg_deal - ������ ������ �� �������
//#define heartbeat - ��������� ������� c�������� �����
//#define sys_events - ������� �������
//#define user_deal - ������ ������ ������������
//#define user_multileg_deal
//#define orders_log - ������ ������
//#define multileg_orders_log - ������ ������ �� �������
//#define adjusted_fee - �������� �����
//#define orders - ������� �������� ������
//#define info
//#define common
//#define orders_aggr
//#define position
//#define part - ��������, ������ � ��������� ����-���������� ��������
//#define part_sa
//#define delivery_report - ����� � ��������
//#define fut_rejected_orders - ����������� � ������� ������
//#define fut_intercl_info - ���������� � ��, ������������ �� ����������� ����. ��������
//#define fut_bond_registry - ���������� ���������� ���������
//#define fut_bond_isin - ���������� ������������ ���������
//#define fut_bond_nkd - ��� �� ���� ������ �������
//#define fut_bond_nominal - ������� ������ ����������� ��������� ���������
//#define usd_online - ���������� ����� USD
//#define fut_vcb - ���������� ��������� �������
//#define session - ���������� � �������� ������
//#define multileg_dict - ���������� ������
//#define fut_sess_contents - C��������� ��������� ������������
//#define fut_instruments - C��������� ������������
//#define diler - ���������� ���� ����
//#define investr - ���������� ��������
//#define fut_sess_settl - ���������� ��������: ��������� ����
//#define sys_messages - ��������� �������� �������
//#define fut_settlement_account - ��������� ���
//#define fut_margin_type - ��� ������������
//#define prohibition - �������
//#define rates
//#define opt_rejected_orders - ����������� � ������� ������
//#define opt_intercl_info - ���������� � ��, ������������ �� ����������� ����. ��������
//#define opt_exp_orders - ������ �� ����������
//#define opt_vcb - ���������� ��������� �������
//#define opt_sess_contents - C��������� ��������� ������������
//#define opt_sess_settl
//#define volat_coeff
//#define fut_MM_info - ������������� �� �� ���������
//#define opt_MM_info - ������������� �� �� ��������
//#define cs_mm_rule - ������� � ������������� ��� ��������� ������������ �������
//#define mm_agreement_filter
//#define money_clearing - ���������� ������ � ��������
//#define money_clearing_sa - ���������� ������ � ��������
//#define clr_rate - ����� ����� � ��������
//#define fut_pos - ���������� � ����������� ��������� �� ������ ��������� �������� �� ���������
//#define opt_pos - ���������� � ����������� ��������� �� ������ ��������� �������� �� ��������
//#define fut_pos_sa - ���������� � ����������� ��������� �� ������ ��������� �������� �� ���������
//#define opt_pos_sa - ���������� � ����������� ��������� �� ������ ��������� �������� �� ��������
//#define fut_sess_settl - ��������� ���� �� ���������
//#define opt_sess_settl - ��������� ���� �� ��������
//#define pledge_details
//#define rts_index
//#define rts_index_log
//#define fut_vm - ������������ ����� �� ���������
//#define opt_vm - ������������ ����� �� ��������
//#define fut_vm_sa - ������������ ����� �� ���������
//#define opt_vm_sa
//#define volat
//#define base_contracts_params - ��������� ������� ����������
//#define futures_params - ��������� ���������
//#define virtual_futures_params - ��������� ����������� ���������
//#define options_params - ��������� ��������
//#define broker_params - ���������� � ���������� ����
//#define client_params
//#define fee_all - ���������� � ���������� ����������� ������
//#define fee_tn - ���������������� ���������� �� ���������� ������������ ����������
//#define curr_online - �������� ������ �����