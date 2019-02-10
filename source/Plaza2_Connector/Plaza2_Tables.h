#pragma once

#define CG_SHORT_NAMES
#include "cgate.h"

//эти заголовочные файлы все генерируются утилитой schmemetool из SDK Plaza2
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
#include "schemes\FORTS_VOLAT_REPL.h"		//ошибка в файле, не компилируется
#include "schemes\MOEX_RATES_REPL.h"
#include "schemes\RTS_INDEX_REPL.h"
#include "schemes\RTS_INDEXLOG_REPL.h"

//список таблиц всех доступных схем репликаций
//#define orders_log
//#define multileg_orders_log
//#define deal
//#define multileg_deal - Журнал сделок по связкам
//#define heartbeat - Служебная таблица cерверных часов
//#define sys_events - Таблица событий
//#define user_deal - Журнал сделок пользователя
//#define user_multileg_deal
//#define orders_log - Журнал заявок
//#define multileg_orders_log - Журнал заявок по связкам
//#define adjusted_fee - комиссии биржи
//#define orders - Таблица активных заявок
//#define info
//#define common
//#define orders_aggr
//#define position
//#define part - Средства, лимиты и настройки риск-параметров клиентов
//#define part_sa
//#define delivery_report - Отчет о поставке
//#define fut_rejected_orders - Отвергнутые в клиринг заявки
//#define fut_intercl_info - информация о ВМ, рассчитанной по результатам пром. клиринга
//#define fut_bond_registry - Справочник параметров облигаций
//#define fut_bond_isin - Справочник инструментов облигаций
//#define fut_bond_nkd - НКД на даты выплат купонов
//#define fut_bond_nominal - Размеры выплат номинальной стоимости облигации
//#define usd_online - Трансляция курса USD
//#define fut_vcb - Справочник торгуемых активов
//#define session - Информация о торговой сессии
//#define multileg_dict - Справочник связок
//#define fut_sess_contents - Cправочник торгуемых инструментов
//#define fut_instruments - Cправочник инструментов
//#define diler - Справочник имен фирм
//#define investr - Справочник клиентов
//#define fut_sess_settl - Результаты клиринга: расчетные цены
//#define sys_messages - Сообщения торговой системы
//#define fut_settlement_account - Расчетный Код
//#define fut_margin_type - Тип маржирования
//#define prohibition - Запреты
//#define rates
//#define opt_rejected_orders - Отвергнутые в клиринг заявки
//#define opt_intercl_info - информация о ВМ, рассчитанной по результатам пром. клиринга
//#define opt_exp_orders - Заявки на экспирацию
//#define opt_vcb - Справочник торгуемых активов
//#define opt_sess_contents - Cправочник торгуемых инструментов
//#define opt_sess_settl
//#define volat_coeff
//#define fut_MM_info - Обязательства ММ по фьючерсам
//#define opt_MM_info - Обязательства ММ по опционам
//#define cs_mm_rule - Таблица с инструментами для пересчета центрального страйка
//#define mm_agreement_filter
//#define money_clearing - Клиентские деньги в клиринге
//#define money_clearing_sa - Клиентские деньги в клиринге
//#define clr_rate - Курсы валют и индексов
//#define fut_pos - информация о позиционном состоянии на момент вечернего клиринга по фьючерсам
//#define opt_pos - информация о позиционном состоянии на момент вечернего клиринга по опционам
//#define fut_pos_sa - информация о позиционном состоянии на момент вечернего клиринга по фьючерсам
//#define opt_pos_sa - информация о позиционном состоянии на момент вечернего клиринга по опционам
//#define fut_sess_settl - Расчетные цены по фьючерсам
//#define opt_sess_settl - расчетные цены по опционам
//#define pledge_details
//#define rts_index
//#define rts_index_log
//#define fut_vm - Вариационная маржа по фьючерсам
//#define opt_vm - Вариационная маржа по опционам
//#define fut_vm_sa - Вариационная маржа по фьючерсам
//#define opt_vm_sa
//#define volat
//#define base_contracts_params - Параметры базовых контрактов
//#define futures_params - Параметры фьючерсов
//#define virtual_futures_params - Параметры виртуальных фьючерсов
//#define options_params - Параметры опционов
//#define broker_params - Информация о параметрах фирм
//#define client_params
//#define fee_all - Информация о количестве начисленных баллов
//#define fee_tn - Детализированная информация по количеству некорректных транзакций
//#define curr_online - Значения курсов валют