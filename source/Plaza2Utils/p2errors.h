//----------------------------------------------------------------------------
// Copyright: (c) 2010 RTS, Russia
//----------------------------------------------------------------------------
// Created: 30 March 2010 16:19
// File:errors
// Author:Mikhail_Navernyuk
//----------------------------------------------------------------------------
// Description: declaration of the base xp errors
//----------------------------------------------------------------------------

#pragma once

#ifdef FASTCALL
#undef FASTCALL
#endif

#ifndef EIF_LEAN
#include "P2Errors.h"
#else
#include <stdint.h>
#define P2ERR_CORE_BEGIN 0x17000
#define P2ERR_OK 0
#define P2ERR_COMMON_WRONG_ARGUMENTS 2
#define P2ERR_COMMON_INTERNAL_ERROR 3
typedef const  char *PCSTR;
typedef uint32_t P2ERR;
#endif

enum P2ERR_CORE
{
    P2ERR_COMMON_ERROR =  (P2ERR_CORE_BEGIN + 1), // 17001 
    P2ERR_BRGO_NOT_ENOUGH,
    P2ERR_CLGO_NOT_ENOUGH,
    P2ERR_SETTLEMENT_ACCOUNT_GO_NOT_ENOUGH,
    P2ERR_RFGO_NOT_ENOUGH,
    P2ERR_FASTCALCBCALL_NOT_CALLED,
    P2ERR_CLIENT_BUY_SPOT_LIMIT_RAISED,
    P2ERR_CLIENT_SELL_SPOT_LIMIT_RAISED,
    P2ERR_BROKER_BUY_SPOT_LIMIT_RAISED,
    P2ERR_BROKER_SELL_SPOT_LIMIT_RAISED,
    P2ERR_CROSS_TRADE,
    P2ERR_DEL_ORDER_NOT_EXIST,
    P2ERR_MOVE_ORDER_NOT_EXIST,
    P2ERR_NOT_FULL_REDUCTION,

    //ошибки, выбрасываемые функции передачи денег и залогов Приходит в SQL
    P2ERR_TRANSFER_BROKER_NOT_ENOUGH_FREE_MONEY,
    P2ERR_TRANSFER_BROKER_NOT_ENOUGH_FREE_PLEDGE,
    P2ERR_TRANSFER_BROKER_NOT_ENOUGH_MONEY_ALLOWED_FOR_WITHDRAWAL,
    P2ERR_TRANSFER_BROKER_NOT_ENOUGH_AMOUNT_PLEDGE,
    P2ERR_TRANSFER_BROKER_NOT_ENOUGH_AMOUNT_MONEY_PLEDGE,
    P2ERR_TRANSFER_SEPARATED_BROKER_NOT_ENOUGH_BALANCE_MONEY,
    P2ERR_TRANSFER_SEPARATED_BROKER_NOT_ENOUGH_BALANCE_PLEDGE,
    P2ERR_TRANSFER_SETTLEMENT_ACCOUNT_NOT_ENOUGH_FREE_MONEY,
    P2ERR_TRANSFER_SETTLEMENT_ACCOUNT_NOT_ENOUGH_FREE_PLEDGE,
    P2ERR_TRANSFER_SETTLEMENT_ACCOUNT_NOT_ENOUGH_MONEY_ALLOWED_FOR_WITHDRAWAL,
    P2ERR_TRANSFER_RF_NOT_ENOUGH_FREE_MONEY,
    P2ERR_TRANSFER_RF_NOT_ENOUGH_MONEY_ALLOWED_FOR_WITHDRAWAL,
    P2ERR_TRANSFER_SENDER_NOT_FOUND,
    P2ERR_TRANSFER_RECIPIENT_NOT_FOUND,
    P2ERR_TRANSFER_BETWEEN_DIFFERENT_RF,
    P2ERR_TRANSFER_BROKER_NOT_FOUND,
    P2ERR_TRANSFER_NOT_ALLOWED,
    P2ERR_TRANSFER_ATTEMPT_TO_CLEARING_MONEY_TRANSFER_FROM_SEPARATED_BROKER,
    P2ERR_TRANSFER_ATTEMPT_TO_TRANSFER_FROM_NON_PROPER_SETTLEMENT_ACCOUNT,
    P2ERR_TRANSFER_ATTEMPT_TO_TRANSFER_PLEDGE,
    P2ERR_TRANSFER_ATTEMPT_TO_TRANSFER_MONEY_PLEDGE,

    //Проверка кол-ва активных заявок
    P2ERR_PRE_CLIENT_ORDER_COUNT_RAISED,

    P2ERR_FP_DIVISION_BY_ZERO,
    P2ERR_FP_OVERFLOW,
    P2ERR_ACCESS_VIOLATION,
    P2ERR_COMMON_SEH_EXCEPTION,

    //Ошибки moveorder'a
    P2ERR_ORDER_IS_BEING_MOVED,
    P2ERR_MULTILEG_ORDER_MOVING,
    P2ERR_ADDRESS_ORDER_MOVING,
    P2ERR_ORDERS_HAVE_DIFFERENT_INSTRUMENTID, //
    P2ERR_POSSIBLE_CROSSTRADE, //При двойном муве заявок в разных направлениях, проверяем, что цена покупки меньше цены продажи. (в легаси и матчинге)
    P2ERR_CANNOT_CHANGE_DIRECTION,
    P2ERR_FOREIGN_ORDER_MOVING,
    P2ERR_ORDERS_FROM_DIFFERENT_CLIENTS, 
    P2ERR_MOVE_SAME_ORDER,
    P2ERR_TRADE_CHK_OPERATION_NOT_SUPPORTED,

    //Ошибки reserve_fee
    P2ERR_CLIENT_ABSENT,
    P2ERR_INSTRUMENT_ABSENT,
    P2ERR_ISIN_ABSENT,
    P2ERR_RESERVEFEE_TRADEMODEID_ALREADY_EXIST,
    P2ERR_RESERVEFEE_INSTRUMENTID_ALREADY_EXIST,
    P2ERR_RESERVEFEE_ISIN_ALREADY_EXIST,
    P2ERR_RESERVEFEE_TRADEMODEID_NOT_FOUND,
    P2ERR_RESERVEFEE_INSTRUMENTID_NOT_FOUND,
    P2ERR_RESERVEFEE_ISIN_NOT_FOUND,
    //Ошибки SetTradingHalt
    P2ERR_TRADEMODEID_AND_GROUPID_SPECIFIED,
    P2ERR_BAD_TRADINGHALT_REGIME,
    P2ERR_CHECK_DATEEXP_IS_OUT = (P2ERR_CORE_BEGIN + 100),// 2.2.2 Неверно задана дата истечения заявки
    P2ERR_CHECK_CLIENT_NOT_FOUND, // 
    P2ERR_CHECK_USER_NOT_FOUND, // 
    P2ERR_CHECK_USER_IS_OUT_BY_DATE,// 2.3.2 Вне срока доверенности
    P2ERR_CHECK_BROKER_NOT_FOUND_OR_DELETED,// 2.4.3 Нет такой БФ или удален
    P2ERR_CHECK_BROKER_IS_NOT_LINKED_TO_SETTLEMENT_ACCOUNT, // отсуствует привязка брокера к рассчётному коду
    P2ERR_CHECK_INVALID_SETTLEMENT_ACCOUNT_ID, // неверный идентификатор РК
    P2ERR_CHECK_ADMIN_OPER_INPUT,// 2.4.4-2.4.5 Админ активировал режим операторного ввода
    P2ERR_CHECK_ADMIN_OPER_INPUT_NOT_SPECIFIED, // 2.4.6 Админ пытается посылать торговые приказы, не выставив галку операторного ввода – по РФ и БФ operator_input = 0 и (user.group = 2) user, BF, RF
    P2ERR_CHECK_MAINTRADER_OPER_INPUT, // 2.4.7 Главный трейдер активировал режим операторного ввода по данной БФ
    P2ERR_CHECK_MAINTRADER_OPER_RF_INPUT, // 2.4.8 Главный трейдер активировал режим операторного ввода по данной БФ
    P2ERR_CHECK_TRADER_OPER_INPUT, // 2.4.9 Трейдер активировал режим операторного ввода по данной БФ
    P2ERR_CHECK_NOT_ENOUGH_RIGHTS,// 2.4.10 Нет прав на посылку основных торговых приказов
    P2ERR_CHECK_LOGIN_FORBIDDEN,// 2.4.11 C данного логина нельзя посылать приказы по указанному клиентскому коду
    P2ERR_CHECK_WRONG_LOGIN_GROUP,// 2.4.12 Неверная группа логина
    P2ERR_CHECK_DU_FORBIDDEN, // 2.4.14 Ошибка указания ДУ (для добавления заявки)
    P2ERR_CHECK_ADDRESS_ORDER,// 2.4.15 Внесистемная заявка от клиентского логина
    P2ERR_CHECK_RTSCODE_IS_EMPTY, // 2.3.17 У БФ не проставлен код РТС
    P2ERR_CHECK_WRONG_DONTCHECKMONEY_MODE,// 2.4.18 Нет прав на выставление заявки без проверки денег по клиенту (для добавления заявки)
    P2ERR_CHECK_TRADEMODES_NOT_FOUND, // 2.4.1 Нет указанного режима торгов – по указанному режиму не найдено записи в таблице trade_modes
    P2ERR_CHECK_GENERAL_TRADER_FORBIDDEN, // 2.4.2 Запрет главного трейдера на подачу заявок в указанном режиме
    P2ERR_CHECK_TRADER_FORBIDDEN, // 2.4.3 Запрет БФ на подачу заявок в указанном режиме 
    P2ERR_CHECK_CLIENT_FORBIDDEN, // 2.4.4 Запрет БФ по клиентскому регистру на подачу заявок в указанном режиме
    P2ERR_CHECK_INSTRUMENT_NOT_FOUND, // 2.4.6 Данный инструмент не торгуется в указанном режиме торгов 
    P2ERR_CHECK_INSTRUMENT_TRADE_OFF, // 2.4.7 По данному инструменту в указанном режиме нет торгов 
    P2ERR_CHECK_ORDER_WRONG_PARAMETERS, // 2.4.8 Ошибка вводимых параметров 2.4.1 Ошибка вводимых параметров в неторговых приказах
    P2ERR_CHECK_ORDER_WRONG_PRICE,// 2.4.9 Слишком большое по модулю значение в поле цена 
    P2ERR_CHECK_ORDER_WRONG_QUANTITY, // 2.4.10 Слишком большой объем заявки
    P2ERR_CHECK_RTSMONEY_DEPO_NOT_FOUND,// 2.4.11 Нет прикрепленного к клиентскому регистру счета Депо для маней 
    P2ERR_CHECK_NON_MULTIPLICITY_PRICE, // 2.4.13 Некратность цены (премии, ставки, своп-цены) минимальному шагу цены
    P2ERR_CHECK_OUT_OF_LIMITS,// 2.5.1 Цена заявки вне лимитов по инструменту
    P2ERR_CHECK_TOO_MUCH_MONEY,// 2.4 Слишком большое число денежных средств
    P2ERR_CHECK_LICENSE_NOT_FOUND,// 2.4.14 Цена заявки вне лимитов по инструменту
    P2ERR_CHECK_PROHIBITION_NOT_FOUND,// 2.4.14 Цена заявки вне лимитов по инструменту
    P2ERR_CHECK_DEALER_LICENSE_NOT_FOUND, // 2.6.1 Запрет всех торговых операций по причине отзыва/приостановки дилерской лицензии данной РФ
    P2ERR_CHECK_DEALER_LICENSE_FOR_OPEN_NOT_FOUND,// 2.6.2 Запрет открытия позиций по причине отзыва/приостановки дилерской лицензии данной РФ
    P2ERR_CHECK_BROKER_LICENSE_NOT_FOUND, // 2.6.3 Запрет всех торговых операций по причине отзыва/приостановки брокерской лицензии данной РФ
    P2ERR_CHECK_BROKER_LICENSE_FOR_OPEN_NOT_FOUND,// 2.6.4 Запрет открытия позиций по причине отзыва/приостановки брокерской лицензии данной РФ
    P2ERR_CHECK_STOCK_BROKER_LICENSE_NOT_FOUND, // 2.6.5 Запрет всех торговых операций по причине отзыва/приостановки лицензии биржевого посредника данной РФ
    P2ERR_CHECK_STOCK_BROKER_LICENSE_FOR_OPEN_NOT_FOUND,// 2.6.6 Запрет открытия позиций по причине отзыва/приостановки лицензии биржевого посредника данной РФ
    P2ERR_CHECK_DU_LICENSE_NOT_FOUND, // 2.6.7 Запрет всех торговых операций по причине отзыва/приостановки лицензии ДУ данной РФ
    P2ERR_CHECK_DU_LICENSE_FOR_OPEN_NOT_FOUND,// 2.6.8 Запрет открытия позиций по причине отзыва/приостановки лицензии ДУ данной РФ
    P2ERR_CHECK_ADMIN_TS_PROHIBITION,// 2.11.1 Запрет Администратора торгов на все торговые операции
    P2ERR_CHECK_ADMIN_TS_POS_OPENING_PROHIBITION,// 2.11.2 Запрет Администратора торгов на открытие позиций
    P2ERR_CHECK_ADMIN_TS_SHORT_POS_OPENING_PROHIBITION,// 2.11.3 Запрет Администратора торгов на открытие позиций на Продажу
    P2ERR_CHECK_ADMIN_CC_PROHIBITION,// 2.11.4 Запрет Администратора Клирингового Центра на все торговые операции
    P2ERR_CHECK_ADMIN_CC_POS_OPENING_PROHIBITION,// 2.11.5 Запрет Администратора Клирингового Центра на открытие позиций
    P2ERR_CHECK_ADMIN_CC_SHORT_POS_OPENING_PROHIBITION,// 2.11.6 Запрет Администратора Клирингового Центра на открытие позиций на Продажу
    P2ERR_CHECK_RF_PROHIBITION, // 2.11.7 Запрет Главного трейдера РФ на все торговые операции
    P2ERR_CHECK_RF_POS_OPENING_PROHIBITION, // 2.11.8 Запрет Главного трейдера РФ на открытие позиций
    P2ERR_CHECK_RF_SHORT_POS_OPENING_PROHIBITION, // 2.11.9 Запрет Главного трейдера РФ на открытие позиций на Продажу
    P2ERR_CHECK_BF_PROHIBITION, // 2.11.10 Запрет Трейдера БФ на все торговые операции
    P2ERR_CHECK_BF_POS_OPENING_PROHIBITION, // 2.11.11 Запрет Трейдера БФ на открытие позиций
    P2ERR_CHECK_BF_SHORT_POS_OPENING_PROHIBITION, // 2.11.12 Запрет Трейдера БФ на открытие позиций на Продажу
    P2ERR_CHECK_INVALID_USERLEVEL,
    P2ERR_CHECK_INVALID_USERGROUP,
    P2ERR_CHECK_INVALID_USERTRADEDATETIMES,
    P2ERR_CHECK_INVALID_USEROPERATORINPUT,
    P2ERR_FOREIGN_ORDER_DELETION, // В MoveOrder пришла команда на удаление заявки другого клиента
    P2ERR_CORE_LRPCQ_STOPPED, // Ядро находится в состоянии Stopped и выдает ошибку
    P2ERR_CORE_PARTIAL_MASS_DELETION, // Запрет на снятие заявок: часть заявок не снята".
    P2ERR_CHECK_BAD_MODE,
    P2ERR_CHECK_BAD_INITIATOR,
    P2ERR_CHECK_BAD_TRADE_MODE_MASK,
    P2ERR_ISIN_SHOULD_NOT_BE_SPECIFIED,

    P2ERR_CHECK_CONTRABROKER_NOT_FOUND,
    P2ERR_VOLAT_NOT_FOUND,
    P2ERR_PRICE_ISNT_POSITIVE,

    P2ERR_PROHIBITION_NOT_FOUND,
    P2ERR_PROHIBITION_ALREADY_EXISTS,

    //Ошибки констистентности данных в модуле проверок в LS 
    P2ERR_LSCHECKS_FUT_NOT_FOUND, 
    P2ERR_LSCHECKS_LEG_NOT_FOUND,
    P2ERR_LSCHECKS_TOO_MANY_LEGS,
    P2ERR_LSCHECKS_OPTION_IN_MULTILEG,
    P2ERR_LSCHECKS_CLEARING_INSTR_NOT_FOUND,
    P2ERR_LSCHECKS_UNDERDETERMINED_MULTILEG,
    P2ERR_LSCHECKS_BASECONTRACT_NOT_FOUND,
    P2ERR_LSCHECKS_WRONG_BASECONTRACTID,
    P2ERR_LSCHECKS_PROHIBITION_NOT_FOUND,
    P2ERR_LSCHECKS_COMMIT_REQUIRED,
    P2ERR_LSCHECKS_CLEAR_REQUIRED,
    P2ERR_LSCHECKS_RF_NOT_FOUND,
    P2ERR_LSCHECKS_NON_MULTIPLICITY_SWAP_PRICE_OR_RATE,
    P2ERR_CONSISTENCY_VIRTUAL_FUTURES_NOT_FOUND,
    P2ERR_CONSISTENCY_CANNOT_REINSERT_PROHIBITION,
    P2ERR_POSITION_CALCULATOR_COMMON_ERROR,
    P2ERR_DATALOAD_RF_NOT_FOUND,
    P2ERR_DATALOAD_SETTLEMENT_ACCOUNT_NOT_FOUND,  // рассчётный код с заданным ID не найден
    P2ERR_NONTRADE_CHK_OPERATION_NOT_ALLOWED,
    P2ERR_NONTRADE_CHK_BASECONTRACT_NOT_FOUND,
    
    //Ошибки в модуле позиционилки
    P2ERR_POSITION_CALCULATOR_INSTR_NOT_FOUND = (P2ERR_CORE_BEGIN + 300),
    P2ERR_POSITION_CALCULATOR_BASECONTRACT_NOT_FOUND,
    P2ERR_POSITION_CALCULATOR_FUTURES_NOT_FOUND,
    P2ERR_POSITION_CALCULATOR_DIFFERENT_FUTURES_MONEYCOEFF_IN_MULTILEG,// попытка вставки в мультилег инструмента, у которого отношение lot*minstepprice/minstep отличается от такого же отношения в уже существующих ногах мультилега
    P2ERR_POSITION_CALCULATOR_INVESTOR_NOT_FOUND,
    P2ERR_POSITION_CALCULATOR_MULTILEG_ISIN_IN_POSITION,
    P2ERR_POSITION_CALCULATOR_ISIN_NOT_FOUND,
    P2ERR_POSITION_CALCULATOR_VIRTFUTURES_NOT_FOUND,
    P2ERR_POSITION_CALCULATOR_TRADEMODEID_NOT_FOUND,
    P2ERR_POSITION_CALCULATOR_BASECONTRACT_ALREADY_EXISTS,
    P2ERR_POSITION_CALCULATOR_TRADEMODE_ALREADY_EXISTS,
    P2ERR_POSITION_CALCULATOR_VIRTFUTURES_ALREADY_EXISTS,
    P2ERR_POSITION_CALCULATOR_FUTURES_ALREADY_EXISTS,
    P2ERR_POSITION_CALCULATOR_OPTION_ALREADY_EXISTS,
    P2ERR_POSITION_CALCULATOR_ISIN_ALREADY_EXISTS_IN_MULTILEG,
    P2ERR_POSITION_CALCULATOR_INSTRUMENT_ALREADY_EXISTS,
    P2ERR_POSITION_CALCULATOR_POSITION_ALREADY_EXISTS,
    P2ERR_POSITION_CALCULATOR_INCORRECT_PRICE_TYPE,
    P2ERR_POSITION_CALCULATOR_PAIR_INSTRUMENTID_EXPECTED,
    P2ERR_POSITION_CALCULATOR_MULTILEG_NOT_FOUND,
    P2ERR_POSITION_CALCULATOR_OPTION_NOT_FOUND,
    P2ERR_POSITION_CALCULATOR_ADDRESS_REPO,

    // новые ошибки в MainThread - торговый функционал
    P2ERR_CHECK_LEGACY_BADORDERTYPE= (P2ERR_CORE_BEGIN + 400),
    P2ERR_CHECK_LEGACY_BADCOTIRCONTR,
    P2ERR_CHECK_LEGACY_USERNOTFOUND,
    P2ERR_CHECK_LEGACY_BROKERNOTFOUND,
    P2ERR_CHECK_LEGACY_CLIENTNOTFOUND,
    P2ERR_CHECK_LEGACY_BROKERCODE_LOGIN_MISMATCH,
    P2ERR_CHECK_LEGACY_CLIENTCODE_LOGIN_MISMATCH,

    // ошибки CalcGO
    P2ERR_CALCGO_API_COMMON_ERROR = (P2ERR_CORE_BEGIN + 500),
    P2ERR_CALCGO_API_ERROR_BC_ALREADY_EXIST,
    P2ERR_CALCGO_API_ERROR_FUTURES_ALREADY_EXIST,
    P2ERR_CALCGO_API_ERROR_VIRTFUTURES_ALREADY_EXIST,
    P2ERR_CALCGO_API_ERROR_OPTION_ALREADY_EXIST,
    P2ERR_CALCGO_API_ERROR_WRONG_HASOPTIONS,
    P2ERR_CALCGO_API_ERROR_BC_NOT_FOUND,
    P2ERR_CALCGO_API_ERROR_FUTURES_NOT_FOUND,
    P2ERR_CALCGO_API_ERROR_VIRTFUTURES_NOT_FOUND,
    P2ERR_CALCGO_API_ERROR_PART_ALREADY_EXIST,
    P2ERR_CALCGO_API_ERROR_PART_NOT_FOUND,
    P2ERR_CALCGO_API_ERROR_FIRM_ALREADY_EXIST,
    P2ERR_CALCGO_API_ERROR_FIRM_NOT_FOUND,
    P2ERR_CALCGO_API_ERROR_FUTPRICE_LESS_2LIM,
    P2ERR_CALCGO_API_ERROR_WRONG_POINTSNUM,
    P2ERR_CALCGO_API_ERROR_WRONG_INPUT_PARAMETERS,
    P2ERR_CALCGO_API_ERROR_CURRENCY_NOT_FOUND,

    P2ERR_PROH_CLIENT_NOT_FOUND,
    P2ERR_FUTURES_IS_MULTILEG,
    P2ERR_TRADING_CLOSED,
    P2ERR_LSCHECKS_WRONG_BASECONTRACT,
    P2ERR_NONTRADE_WRONG_INPUT_PARAMETERS,

    P2ERR_CHANGECLIENTMONEY_NOT_ENOUGH_MONEY,
    P2ERR_CHANGEBROKERVCB_BROKER_NOT_FOUND,
    P2ERR_CHANGEBROKERVCB_BASECONTRACT_NOT_FOUND,
    P2ERR_CHANGEBROKERVCB_OPERATION_NOT_ALLOWED,
    P2ERR_T0_NOT_ALLOWED,
    P2ERR_FIRSTPRICE_AND_DELIVERYPRICE_ARE_DIFF,
    P2ERR_ADDRESS_TRADEMODE_ONLY,
    P2ERR_NON_REPO_ISIN,
    P2ERR_CHECK_CLRINSTRUMENT_NOT_FOUND,

    P2ERR_CALCGO_API_ERROR_CURRENCY_ALREADY_EXIST,           /// @note FORTS-1250  М2шный код ошибки был заменен на код ошибки  CALCGO_API для того чтобы не разъехались номера ошибок
    P2ERR_CALCGO_API_ERROR_SETTLEMENT_ACCOUNT_ALREADY_EXIST, /// @note неиспользуемый более М2шный код ошибки был заменен на код ошибки CALCGO_API для того чтобы не разъехались номера ошибок
    P2ERR_M2_NOTREADY_4CLEARING_FOUND_ACTIVE_ORDERS,

    // <FOSRV-2379>
    P2ERR_TRCLPOS_POSITION_NOT_FOUND,
    P2ERR_TRCLPOS_NOT_ENOUGH_POSITION,
    P2ERR_TRCLPOS_INN_MISMATCH,
    P2ERR_TRCLPOS_RF_MISMATCH,
    P2ERR_TRCLPOS_TARGET_IS_ZERO_SECTION,
    P2ERR_TRCLPOS_BF_ADMIN_OPERINPUT,
    P2ERR_TRCLPOS_PROHIBITED_INSTRUMENT,
    // </FOSRV-2379>

    P2ERR_CALCGO_API_ERROR_OPTION_NOT_FOUND,
    P2ERR_POSITION_QUANTITY_OVERFLOW,
    P2ERR_AGGREGATED_BUY_ORDERS_QUANTITY_OVERFLOW,
    P2ERR_AGGREGATED_SELL_ORDERS_QUANTITY_OVERFLOW,

	// ошибки SPECTRA_IM
	P2ERR_SPECTRA_IM_COMMON_ERROR = (P2ERR_CORE_BEGIN + 600),
	P2ERR_SPECTRA_IM_ALREADY_INITIALIZED,
	P2ERR_SPECTRA_IM_END_OF_DATA,
	P2ERR_SPECTRA_IM_WRONG_ARGUMENTS,
	P2ERR_SPECTRA_IM_NOT_ENOUGH_MEMORY,
	P2ERR_SPECTRA_IM_NOT_FOUND,
	P2ERR_SPECTRA_IM_STARTUP_ERROR,
	P2ERR_SPECTRA_IM_CONNECT_ERROR,
	P2ERR_SPECTRA_IM_LOGIN_ERROR,
	P2ERR_SPECTRA_IM_GET_USER_INFO_ERROR,
	P2ERR_SPECTRA_IM_EXCEEDED_LIMIT_OF_PARTICIPANTS,
	P2ERR_SPECTRA_IM_NOT_SUPPORTED,
	P2ERR_SPECTRA_IM_NOT_CONNECTED,
	P2ERR_SPECTRA_IM_NOT_INITIALIZED,
	P2ERR_SPECTRA_IM_WRONG_PORT,
	P2ERR_SPECTRA_IM_WRONG_MQ_TIMOUT,
	P2ERR_SPECTRA_IM_WRONG_PART_COUNT,

    P2ERR_LAST_ERROR
};

inline PCSTR Fo2ErrGetDescrStr(P2ERR err)
{
PCSTR ret = 0;
switch (err) 
{
    case P2ERR_COMMON_WRONG_ARGUMENTS: ret="Wrong arguments"; break;
    case P2ERR_COMMON_ERROR: ret="Common PII error"; break;
    case P2ERR_BRGO_NOT_ENOUGH: ret="Broker GO not enough"; break;
    case P2ERR_CLGO_NOT_ENOUGH: ret="Client GO not enough"; break;
    case P2ERR_SETTLEMENT_ACCOUNT_GO_NOT_ENOUGH: ret="Settlement Account GO not enough"; break;
    case P2ERR_RFGO_NOT_ENOUGH: ret="RF GO not enough"; break;
    case P2ERR_FASTCALCBCALL_NOT_CALLED: ret="FASTCALCBCALL_NOT_CALLED"; break;
    case P2ERR_CLIENT_BUY_SPOT_LIMIT_RAISED: ret="CLIENT_BUY_SPOT_LIMIT_RAISED"; break;
    case P2ERR_CLIENT_SELL_SPOT_LIMIT_RAISED: ret="CLIENT_SELL_SPOT_LIMIT_RAISED"; break;
    case P2ERR_BROKER_BUY_SPOT_LIMIT_RAISED: ret="BROKER_BUY_SPOT_LIMIT_RAISED"; break;
    case P2ERR_BROKER_SELL_SPOT_LIMIT_RAISED: ret="BROKER_SELL_SPOT_LIMIT_RAISED"; break;
    case P2ERR_CROSS_TRADE: ret="Crosstrade revealed"; break;
    case P2ERR_DEL_ORDER_NOT_EXIST: ret="Order not found"; break;
    case P2ERR_MOVE_ORDER_NOT_EXIST: ret="Order not found"; break;
    case P2ERR_NOT_FULL_REDUCTION: ret="Not full reduction"; break;
    case P2ERR_TRANSFER_BROKER_NOT_ENOUGH_FREE_MONEY: ret="TRANSFER_BROKER_NOT_ENOUGH_FREE_MONEY"; break;
    case P2ERR_TRANSFER_BROKER_NOT_ENOUGH_FREE_PLEDGE: ret="TRANSFER_BROKER_NOT_ENOUGH_FREE_PLEDGE"; break;
    case P2ERR_TRANSFER_BROKER_NOT_ENOUGH_MONEY_ALLOWED_FOR_WITHDRAWAL: ret="TRANSFER_BROKER_NOT_ENOUGH_MONEY_ALLOWED_FOR_WITHDRAWAL"; break;
    case P2ERR_TRANSFER_BROKER_NOT_ENOUGH_AMOUNT_PLEDGE: ret="TRANSFER_BROKER_NOT_ENOUGH_AMOUNT_PLEDGE"; break;
    case P2ERR_TRANSFER_BROKER_NOT_ENOUGH_AMOUNT_MONEY_PLEDGE: ret="TRANSFER_BROKER_NOT_ENOUGH_AMOUNT_MONEY_PLEDGE"; break;
    case P2ERR_TRANSFER_SEPARATED_BROKER_NOT_ENOUGH_BALANCE_MONEY: ret="TRANSFER_SEPARATED_BROKER_NOT_ENOUGH_BALANCE_MONEY"; break;
    case P2ERR_TRANSFER_SEPARATED_BROKER_NOT_ENOUGH_BALANCE_PLEDGE: ret="TRANSFER_SEPARATED_BROKER_NOT_ENOUGH_BALANCE_PLEDGE"; break;
    case P2ERR_TRANSFER_SETTLEMENT_ACCOUNT_NOT_ENOUGH_FREE_MONEY: ret="TRANSFER_SETTLEMENT_ACCOUNT_NOT_ENOUGH_FREE_MONEY"; break;
    case P2ERR_TRANSFER_SETTLEMENT_ACCOUNT_NOT_ENOUGH_FREE_PLEDGE: ret="TRANSFER_SETTLEMENT_ACCOUNT_NOT_ENOUGH_FREE_PLEDGE"; break;
    case P2ERR_TRANSFER_SETTLEMENT_ACCOUNT_NOT_ENOUGH_MONEY_ALLOWED_FOR_WITHDRAWAL: ret="TRANSFER_SETTLEMENT_ACCOUNT_NOT_ENOUGH_MONEY_ALLOWED_FOR_WITHDRAWAL"; break;
    case P2ERR_TRANSFER_RF_NOT_ENOUGH_FREE_MONEY: ret="TRANSFER_RF_NOT_ENOUGH_FREE_MONEY"; break;
    case P2ERR_TRANSFER_RF_NOT_ENOUGH_MONEY_ALLOWED_FOR_WITHDRAWAL: ret="TRANSFER_RF_NOT_ENOUGH_MONEY_ALLOWED_FOR_WITHDRAWAL"; break;
    case P2ERR_TRANSFER_SENDER_NOT_FOUND: ret="TRANSFER_SENDER_NOT_FOUND"; break;
    case P2ERR_TRANSFER_RECIPIENT_NOT_FOUND: ret="TRANSFER_RECIPIENT_NOT_FOUND"; break;
    case P2ERR_TRANSFER_BETWEEN_DIFFERENT_RF: ret="TRANSFER_BETWEEN_DIFFERENT_RF"; break;
    case P2ERR_TRANSFER_BROKER_NOT_FOUND: ret="TRANSFER_BROKER_NOT_FOUND"; break;
    case P2ERR_TRANSFER_NOT_ALLOWED: ret="TRANSFER_NOT_ALLOWED"; break;
    case P2ERR_TRANSFER_ATTEMPT_TO_CLEARING_MONEY_TRANSFER_FROM_SEPARATED_BROKER: ret="ATTEMPT_TO_CLEARING_MONEY_TRANSFER_FROM_SEPARATED_BROKER"; break;
    case P2ERR_TRANSFER_ATTEMPT_TO_TRANSFER_FROM_NON_PROPER_SETTLEMENT_ACCOUNT: ret="ATTEMPT_TO_TRANSFER_FROM_NON_PROPER_SETTLEMENT_ACCOUNT"; break;
    case P2ERR_TRANSFER_ATTEMPT_TO_TRANSFER_PLEDGE: ret="ATTEMPT_TO_TRANSFER_PLEDGE"; break;
    case P2ERR_TRANSFER_ATTEMPT_TO_TRANSFER_MONEY_PLEDGE: ret="ATTEMPT_TO_TRANSFER_MONEY_PLEDGE"; break;
    case P2ERR_PRE_CLIENT_ORDER_COUNT_RAISED: ret="PRE_CLIENT_ORDER_COUNT_RAISED"; break;
    case P2ERR_FP_DIVISION_BY_ZERO: ret="FP_DIVISION_BY_ZERO"; break;
    case P2ERR_FP_OVERFLOW: ret="FP_OVERFLOW"; break;
    case P2ERR_ACCESS_VIOLATION: ret="Access violation"; break;
    case P2ERR_COMMON_SEH_EXCEPTION: ret="Common SEH error"; break;
    case P2ERR_ORDER_IS_BEING_MOVED: ret="Order is being moved"; break;
    case P2ERR_TRADE_CHK_OPERATION_NOT_SUPPORTED: ret = "TRADE_CHK_OPERATION_NOT_SUPPORTED";break;
    case P2ERR_POSITION_QUANTITY_OVERFLOW: ret = "POSITION_QUANTITY_OVERFLOW";break;
    case P2ERR_AGGREGATED_BUY_ORDERS_QUANTITY_OVERFLOW: ret = "AGGREGATED_BUY_ORDERS_QUANTITY_OVERFLOW";break;
    case P2ERR_AGGREGATED_SELL_ORDERS_QUANTITY_OVERFLOW: ret = "AGGREGATED_SELL_ORDERS_QUANTITY_OVERFLOW";break;
    case P2ERR_MULTILEG_ORDER_MOVING: ret="Multileg order moving"; break;
    case P2ERR_ADDRESS_ORDER_MOVING: ret="Address order moving"; break; 
    case P2ERR_ORDERS_HAVE_DIFFERENT_INSTRUMENTID: ret="Orders have different InstrumentID"; break;
    case P2ERR_POSSIBLE_CROSSTRADE: ret="Possible crosstrade while moving"; break; //При двойном муве заявок в разных направлениях, проверяем, что цена покупки меньше цены продажи. (в легаси и матчинге)
    case P2ERR_CANNOT_CHANGE_DIRECTION: ret="Can't change order direction"; break;
    case P2ERR_FOREIGN_ORDER_MOVING: ret="Foreign order moving"; break;
    case P2ERR_ORDERS_FROM_DIFFERENT_CLIENTS: ret="Orders from different clients"; break;
    case P2ERR_MOVE_SAME_ORDER: ret="Moving the same order"; break;
    case P2ERR_TRADEMODEID_AND_GROUPID_SPECIFIED: ret="Simultaneous TrademodeID and GroupID masks specification"; break;
    case P2ERR_BAD_TRADINGHALT_REGIME: ret="Bad regime in SetTradingHalts"; break;        
    case P2ERR_CHECK_DATEEXP_IS_OUT: ret="CHECK_DATEEXP_IS_OUT"; break;
    case P2ERR_CHECK_CLIENT_NOT_FOUND: ret="Client not found"; break;
    case P2ERR_CHECK_USER_NOT_FOUND: ret="CHECK_USER_NOT_FOUND"; break;
    case P2ERR_CHECK_USER_IS_OUT_BY_DATE: ret="CHECK_USER_IS_OUT_BY_DATE"; break;
    case P2ERR_CHECK_BROKER_NOT_FOUND_OR_DELETED: ret="Broker not found"; break;
    case P2ERR_CHECK_ADMIN_OPER_INPUT: ret="Check: Admin operator input"; break;
    case P2ERR_CHECK_ADMIN_OPER_INPUT_NOT_SPECIFIED: ret="Check: Admin operator input not specified"; break;
    case P2ERR_CHECK_MAINTRADER_OPER_INPUT: ret="Check: Maintrader operator input"; break;
    case P2ERR_CHECK_MAINTRADER_OPER_RF_INPUT: ret="Check: Maintrader operator input"; break;
    case P2ERR_CHECK_TRADER_OPER_INPUT: ret="Check: Trader operator input"; break;
    case P2ERR_CHECK_NOT_ENOUGH_RIGHTS: ret="Not enough rights"; break;
    case P2ERR_CHECK_LOGIN_FORBIDDEN: ret="CHECK_LOGIN_FORBIDDEN"; break;
    case P2ERR_CHECK_WRONG_LOGIN_GROUP: ret="CHECK_WRONG_LOGIN_GROUP"; break;
    case P2ERR_CHECK_DU_FORBIDDEN: ret="CHECK_DU_FORBIDDEN"; break;
    case P2ERR_CHECK_ADDRESS_ORDER: ret="CHECK_ADDRESS_ORDER"; break;
    case P2ERR_CHECK_RTSCODE_IS_EMPTY: ret="CHECK_RTSCODE_IS_EMPTY"; break;
    case P2ERR_CHECK_WRONG_DONTCHECKMONEY_MODE: ret="WRONG DONTCHECKMONEY MODE"; break;
    case P2ERR_CHECK_TRADEMODES_NOT_FOUND: ret="TRADEMODE NOT FOUND"; break;
    case P2ERR_CHECK_GENERAL_TRADER_FORBIDDEN: ret="RF trade mode prohibition"; break;
    case P2ERR_CHECK_TRADER_FORBIDDEN: ret="BF trade mode prohibition"; break;
    case P2ERR_CHECK_CLIENT_FORBIDDEN: ret="BF trade mode prohibition"; break;
    case P2ERR_CHECK_INSTRUMENT_NOT_FOUND: ret="Not traded instrument"; break;
    case P2ERR_CHECK_INSTRUMENT_TRADE_OFF: ret="CHECK_INSTRUMENT_TRADE_OFF"; break;
    case P2ERR_CHECK_ORDER_WRONG_PARAMETERS: ret="Wrong arguments"; break;
    case P2ERR_CHECK_ORDER_WRONG_PRICE: ret="Price absolute value too big"; break;
    case P2ERR_CHECK_ORDER_WRONG_QUANTITY: ret="Volume too big"; break;
    case P2ERR_CHECK_RTSMONEY_DEPO_NOT_FOUND: ret="No attached money account"; break;
    case P2ERR_CHECK_NON_MULTIPLICITY_PRICE: ret="Price is not multiple of minstep"; break;
    case P2ERR_CHECK_OUT_OF_LIMITS: ret="Price out of limits"; break;
    case P2ERR_CHECK_TOO_MUCH_MONEY: ret="Large money amount"; break;
    case P2ERR_CHECK_LICENSE_NOT_FOUND: ret="CHECK_LICENSE_NOT_FOUND"; break;
    case P2ERR_CHECK_PROHIBITION_NOT_FOUND: ret="CHECK_PROHIBITION_NOT_FOUND"; break;
    case P2ERR_CHECK_DEALER_LICENSE_NOT_FOUND:ret="Absence of dealer license"; break;
    case P2ERR_CHECK_DEALER_LICENSE_FOR_OPEN_NOT_FOUND: ret="Absence of dealer license for position opening"; break;
    case P2ERR_CHECK_BROKER_LICENSE_NOT_FOUND:ret="Absence of broker license"; break;
    case P2ERR_CHECK_BROKER_LICENSE_FOR_OPEN_NOT_FOUND: ret="Absence of broker license for position opening"; break;
    case P2ERR_CHECK_STOCK_BROKER_LICENSE_NOT_FOUND:ret="Absence of stock broker license"; break;
    case P2ERR_CHECK_STOCK_BROKER_LICENSE_FOR_OPEN_NOT_FOUND: ret="Absence of stock broker license for position opening"; break;
    case P2ERR_CHECK_DU_LICENSE_NOT_FOUND:ret="Absence of trusted broker license"; break;
    case P2ERR_CHECK_DU_LICENSE_FOR_OPEN_NOT_FOUND: ret="Absence of trusted broker license for position opening"; break;

    case P2ERR_CHECK_ADMIN_TS_PROHIBITION: ret="Administrator TS prohibition";break;
    case P2ERR_CHECK_ADMIN_TS_POS_OPENING_PROHIBITION: ret="Administrator TS position opening prohibition"; break;
    case P2ERR_CHECK_ADMIN_TS_SHORT_POS_OPENING_PROHIBITION: ret="Administrator TS short position opening prohibition"; break;
    case P2ERR_CHECK_ADMIN_CC_PROHIBITION: ret="Administrator CC prohibition";break;
    case P2ERR_CHECK_ADMIN_CC_POS_OPENING_PROHIBITION: ret="Administrator CC position opening prohibition"; break;
    case P2ERR_CHECK_ADMIN_CC_SHORT_POS_OPENING_PROHIBITION: ret="Administrator CC short position opening prohibition"; break;
    case P2ERR_CHECK_RF_PROHIBITION:ret="RF prohibition";break;
    case P2ERR_CHECK_RF_POS_OPENING_PROHIBITION:ret="RF position opening prohibition"; break;
    case P2ERR_CHECK_RF_SHORT_POS_OPENING_PROHIBITION:ret="RF short position opening prohibition"; break;
    case P2ERR_CHECK_BF_PROHIBITION:ret="Broker prohibition";break;
    case P2ERR_CHECK_BF_POS_OPENING_PROHIBITION:ret="Broker position opening prohibition"; break;
    case P2ERR_CHECK_BF_SHORT_POS_OPENING_PROHIBITION:ret="Broker short position opening prohibition"; break;

    case P2ERR_CHECK_INVALID_USERLEVEL: ret="Invalid UserLevel";break;
    case P2ERR_CHECK_INVALID_USERGROUP: ret="Invalid Grupo value";break;
    case P2ERR_CHECK_INVALID_USERTRADEDATETIMES: ret="Invalid user TradeBegin or TradeEnd";break;
    case P2ERR_CHECK_INVALID_USEROPERATORINPUT: ret="Invalid OperatorInput";break;

    case P2ERR_CHECK_BAD_MODE: ret="Invalid mode value";break;
    case P2ERR_CHECK_BAD_INITIATOR: ret="Invalid initiator value";break;
    case P2ERR_CHECK_BAD_TRADE_MODE_MASK: ret="Invalid trade mode mask value";break;
    case P2ERR_ISIN_SHOULD_NOT_BE_SPECIFIED: ret="Isin should not be specified while IsinVCB=Any"; break;

    case P2ERR_FOREIGN_ORDER_DELETION: ret="Foreign order deletion"; break;
    case P2ERR_CORE_LRPCQ_STOPPED: ret="CORE_LRPCQ_STOPPED"; break;
    case P2ERR_CORE_PARTIAL_MASS_DELETION: ret="CORE_PARTIAL_MASS_DELETION"; break;

    case P2ERR_CHECK_CONTRABROKER_NOT_FOUND: ret="Counter-broker not found"; break;
    case P2ERR_VOLAT_NOT_FOUND:ret="Premium limits not specified"; break;
    case P2ERR_PRICE_ISNT_POSITIVE: ret="Premium isn't positive"; break; 

    case P2ERR_PROHIBITION_NOT_FOUND: ret="Prohibition not found"; break; 
    case P2ERR_PROHIBITION_ALREADY_EXISTS: ret="Prohibition already exists"; break; 

    case P2ERR_LSCHECKS_FUT_NOT_FOUND: ret="Futures not found"; break;
    case P2ERR_LSCHECKS_LEG_NOT_FOUND: ret="Leg not found"; break; 
    case P2ERR_LSCHECKS_TOO_MANY_LEGS: ret="Too many legs"; break;
    case P2ERR_LSCHECKS_OPTION_IN_MULTILEG:ret="Option in multileg disallowed"; break; 
    case P2ERR_LSCHECKS_CLEARING_INSTR_NOT_FOUND:ret="Clearing instrument not found"; break;
    case P2ERR_LSCHECKS_UNDERDETERMINED_MULTILEG:ret="Underdetermined multileg";break;
    case P2ERR_LSCHECKS_BASECONTRACT_NOT_FOUND:ret="Base contract not found"; break;
    case P2ERR_LSCHECKS_WRONG_BASECONTRACTID:ret="Base contract's ID not equal to Base contract's ID in instrument"; break;
    case P2ERR_LSCHECKS_PROHIBITION_NOT_FOUND: ret="Prohibition not found"; break;
    case P2ERR_LSCHECKS_COMMIT_REQUIRED: ret="Commit required"; break;
    case P2ERR_LSCHECKS_CLEAR_REQUIRED:ret="Clear required";break; 
    case P2ERR_LSCHECKS_RF_NOT_FOUND:ret="RF not found in Dealer"; break; 
    case P2ERR_LSCHECKS_NON_MULTIPLICITY_SWAP_PRICE_OR_RATE: ret="Swap or rate is not multiple of minstep"; break;
    case P2ERR_CONSISTENCY_VIRTUAL_FUTURES_NOT_FOUND:ret="VirtFutures not found. Consistency error."; break;
    case P2ERR_CONSISTENCY_CANNOT_REINSERT_PROHIBITION:ret="Can not reinsert prohibition."; break;
    case P2ERR_POSITION_CALCULATOR_COMMON_ERROR: ret="Position calculator common error."; break;
     
    case P2ERR_DATALOAD_RF_NOT_FOUND:ret="RF not found";break;
    case P2ERR_DATALOAD_SETTLEMENT_ACCOUNT_NOT_FOUND:ret="Settlement Account not found";break;

    case P2ERR_POSITION_CALCULATOR_INSTR_NOT_FOUND:ret="Instrument not found"; break;
    case P2ERR_POSITION_CALCULATOR_BASECONTRACT_NOT_FOUND: ret="BaseContract not found"; break;
    case P2ERR_POSITION_CALCULATOR_FUTURES_NOT_FOUND:ret="Futures not found"; break;
    case P2ERR_POSITION_CALCULATOR_DIFFERENT_FUTURES_MONEYCOEFF_IN_MULTILEG: ret="Legs with different moneyCoeff(lot*minstepprice/minstep) occurs in multileg"; break;
    case P2ERR_POSITION_CALCULATOR_INVESTOR_NOT_FOUND: ret="Investor not found"; break;
    case P2ERR_POSITION_CALCULATOR_MULTILEG_ISIN_IN_POSITION:ret="Multileg isin in position occurs"; break;
    case P2ERR_POSITION_CALCULATOR_ISIN_NOT_FOUND: ret="Isin not found"; break;
    case P2ERR_POSITION_CALCULATOR_VIRTFUTURES_NOT_FOUND:ret="VirtFutures not found"; break;
    case P2ERR_POSITION_CALCULATOR_TRADEMODEID_NOT_FOUND:ret="TradeModeID not found"; break;
    case P2ERR_POSITION_CALCULATOR_BASECONTRACT_ALREADY_EXISTS:ret="BaseContract already exists, can not reinsert"; break;
    case P2ERR_POSITION_CALCULATOR_TRADEMODE_ALREADY_EXISTS: ret="TradeMode already exists, can not reinsert"; break;
    case P2ERR_POSITION_CALCULATOR_VIRTFUTURES_ALREADY_EXISTS: ret="VirtFutures already exists, can not reinsert"; break;
    case P2ERR_POSITION_CALCULATOR_FUTURES_ALREADY_EXISTS: ret="Futures already exists, can not reinsert"; break;
    case P2ERR_POSITION_CALCULATOR_OPTION_ALREADY_EXISTS:ret="Option already exists, can not reinsert"; break;
    case P2ERR_POSITION_CALCULATOR_ISIN_ALREADY_EXISTS_IN_MULTILEG:ret="Isin already exists in multileg, can not reinsert"; break;
    case P2ERR_POSITION_CALCULATOR_INSTRUMENT_ALREADY_EXISTS:ret="Instrument already exists, can not reinsert"; break;
    case P2ERR_POSITION_CALCULATOR_POSITION_ALREADY_EXISTS:ret="Position already exists, can not reinsert"; break;
    case P2ERR_POSITION_CALCULATOR_INCORRECT_PRICE_TYPE: ret="Incorrect priceType"; break;
    case P2ERR_POSITION_CALCULATOR_PAIR_INSTRUMENTID_EXPECTED: ret="Pair instrument expected"; break;
    case P2ERR_POSITION_CALCULATOR_MULTILEG_NOT_FOUND:ret="Multileg not found"; break;
    case P2ERR_POSITION_CALCULATOR_OPTION_NOT_FOUND:ret="Option not found"; break;
    case P2ERR_POSITION_CALCULATOR_ADDRESS_REPO:ret="Address REPO"; break;

    case P2ERR_CHECK_LEGACY_BADORDERTYPE: ret="Bad OrderType parameter"; break;
    case P2ERR_CHECK_LEGACY_BADCOTIRCONTR: ret="Bad CotirContr parameter"; break;
    case P2ERR_CHECK_LEGACY_USERNOTFOUND: ret="User login not found"; break;
    case P2ERR_CHECK_LEGACY_BROKERNOTFOUND: ret="Broker not found"; break;
    case P2ERR_CHECK_LEGACY_CLIENTNOTFOUND: ret="Client not found"; break;
    case P2ERR_CHECK_LEGACY_BROKERCODE_LOGIN_MISMATCH: ret="BrokerCode and Login mismatch"; break;
    case P2ERR_CHECK_LEGACY_CLIENTCODE_LOGIN_MISMATCH: ret="ClientCode and Login mismatch"; break;

    case P2ERR_CALCGO_API_COMMON_ERROR: ret = "CalcGO unknown error"; break;
    case P2ERR_CALCGO_API_ERROR_BC_ALREADY_EXIST: ret = "Base contract already exists"; break;
    case P2ERR_CALCGO_API_ERROR_FUTURES_ALREADY_EXIST:ret = "Futures already exists"; break;
    case P2ERR_CALCGO_API_ERROR_VIRTFUTURES_ALREADY_EXIST:ret = "Virtual futures already exists"; break;
    case P2ERR_CALCGO_API_ERROR_OPTION_ALREADY_EXIST: ret = "Option already exists"; break;
    case P2ERR_CALCGO_API_ERROR_WRONG_HASOPTIONS: ret = "Base contract has options but parameter HasOptions is false"; break;
    case P2ERR_CALCGO_API_ERROR_BC_NOT_FOUND: ret = "Base contract not found"; break;
    case P2ERR_CALCGO_API_ERROR_FUTURES_NOT_FOUND:ret = "Futures not found"; break;
    case P2ERR_CALCGO_API_ERROR_VIRTFUTURES_NOT_FOUND:ret = "Virtual futures not found"; break;
    case P2ERR_CALCGO_API_ERROR_PART_ALREADY_EXIST: ret = "Participant already exists"; break;
    case P2ERR_CALCGO_API_ERROR_PART_NOT_FOUND: ret = "Participant not found"; break;
    case P2ERR_CALCGO_API_ERROR_FIRM_ALREADY_EXIST: ret = "Firm already exists"; break;
    case P2ERR_CALCGO_API_ERROR_FIRM_NOT_FOUND: ret = "Firm not found"; break;
    case P2ERR_CALCGO_API_ERROR_SETTLEMENT_ACCOUNT_ALREADY_EXIST: ret = "Settlement Account already exists"; break;
    case P2ERR_CALCGO_API_ERROR_WRONG_POINTSNUM: ret = "Base contracts from the same MCS has different PointsNum"; break;
    case P2ERR_PROH_CLIENT_NOT_FOUND: ret = "Client not found"; break;
    case P2ERR_FUTURES_IS_MULTILEG: ret = "Futures is multileg"; break;
    case P2ERR_TRADING_CLOSED: ret = "Trading closed"; break;
    case P2ERR_NONTRADE_WRONG_INPUT_PARAMETERS: ret = "Wrong parameters"; break;
    case P2ERR_LSCHECKS_WRONG_BASECONTRACT: ret = ""; break;
    
    case P2ERR_CHANGECLIENTMONEY_NOT_ENOUGH_MONEY: ret = "Change_ClientMoney: Client has not enough money"; break;
    case P2ERR_CHANGEBROKERVCB_BROKER_NOT_FOUND: ret = "Change_BrokerVcb: Broker not found"; break;
    case P2ERR_CHANGEBROKERVCB_BASECONTRACT_NOT_FOUND: ret = "Change_BrokerVcb: BA not found"; break;
    case P2ERR_CHANGEBROKERVCB_OPERATION_NOT_ALLOWED: ret = "Change_BrokerVcb: Operation not allowed"; break;
    case P2ERR_T0_NOT_ALLOWED: ret = "T0 not allowed"; break;
    case P2ERR_FIRSTPRICE_AND_DELIVERYPRICE_ARE_DIFF: ret = "First price differs from delivery price"; break;
    case P2ERR_ADDRESS_TRADEMODE_ONLY: ret = "Anonym instrument not found"; break;
    case P2ERR_NON_REPO_ISIN: ret = "P2ERR_REPO_FORBIDDEN"; break;
    case P2ERR_CHECK_CLRINSTRUMENT_NOT_FOUND: ret = "Clearing instrument not found"; break;

    // <FOSRV-2379>
    case P2ERR_TRCLPOS_POSITION_NOT_FOUND: ret = "No positions to transfer for the selected instrument"; break;
    case P2ERR_TRCLPOS_NOT_ENOUGH_POSITION: ret = "Insufficient position quantity"; break;
    case P2ERR_TRCLPOS_INN_MISMATCH: ret = "Attempt to transfer between accounts with different VAT numbers"; break;
    case P2ERR_TRCLPOS_RF_MISMATCH: ret = "Prohibited to transfer positions between BF accounts of different CFs"; break;
    case P2ERR_TRCLPOS_TARGET_IS_ZERO_SECTION: ret = "Prohibited to transfer positions to a zero BF account"; break;
    case P2ERR_TRCLPOS_BF_ADMIN_OPERINPUT: ret = "Position transfer for this BF account is prohibited by the Trading Administrator"; break;
    case P2ERR_TRCLPOS_PROHIBITED_INSTRUMENT: ret = "Position transfer for this instrument is prohibited"; break;
    // </FOSRV-2379>

    // <FOSRV-2838>
    case P2ERR_CLIENT_ABSENT: ret = "Client is absent"; break;
    case P2ERR_INSTRUMENT_ABSENT: ret = "Instrument is absent"; break;
    case P2ERR_ISIN_ABSENT: ret = "Isin is absent"; break;
    case P2ERR_RESERVEFEE_TRADEMODEID_ALREADY_EXIST: ret = "Not used"; break;
    case P2ERR_RESERVEFEE_INSTRUMENTID_ALREADY_EXIST: ret = "Not used"; break;
    case P2ERR_RESERVEFEE_ISIN_ALREADY_EXIST: ret = "Not used"; break;
    case P2ERR_RESERVEFEE_TRADEMODEID_NOT_FOUND: ret = "Not used"; break;
    case P2ERR_RESERVEFEE_INSTRUMENTID_NOT_FOUND: ret = "Not used"; break;
    case P2ERR_RESERVEFEE_ISIN_NOT_FOUND: ret = "Not used"; break;
    case P2ERR_NONTRADE_CHK_OPERATION_NOT_ALLOWED: ret = "Nontrade check operation is not allowed"; break;
    case P2ERR_NONTRADE_CHK_BASECONTRACT_NOT_FOUND: ret = "Nontrade check base contract is not fount"; break;
    case P2ERR_CALCGO_API_ERROR_FUTPRICE_LESS_2LIM: ret = "CalcGo API error futprice less 2 limit"; break;
    case P2ERR_CALCGO_API_ERROR_WRONG_INPUT_PARAMETERS: ret = "CalcGo API error wrong input parameters"; break;
    case P2ERR_CALCGO_API_ERROR_CURRENCY_ALREADY_EXIST: ret = "CalcGo API Currency already exists"; break;
    case P2ERR_SPECTRA_IM_COMMON_ERROR: ret = "SpectraIM common error"; break;
    case P2ERR_SPECTRA_IM_ALREADY_INITIALIZED: ret = "SpectraIM already initialized"; break;
    case P2ERR_SPECTRA_IM_END_OF_DATA: ret = "SpectraIM end of data"; break;
    case P2ERR_SPECTRA_IM_WRONG_ARGUMENTS: ret = "SpectraIM wrong arguments"; break;
    case P2ERR_SPECTRA_IM_NOT_ENOUGH_MEMORY: ret = "SpectraIM not enough memory"; break;
    case P2ERR_SPECTRA_IM_NOT_FOUND: ret = "SpectraIM not found"; break;
    case P2ERR_SPECTRA_IM_STARTUP_ERROR: ret = "SpectraIM startup error"; break;
    case P2ERR_SPECTRA_IM_CONNECT_ERROR: ret = "SpectraIM connect error"; break;
    case P2ERR_SPECTRA_IM_LOGIN_ERROR: ret = "SpectraIM login error"; break;
    case P2ERR_SPECTRA_IM_GET_USER_INFO_ERROR: ret = "SpectraIM get user info error"; break;
    case P2ERR_SPECTRA_IM_EXCEEDED_LIMIT_OF_PARTICIPANTS: ret = "SpectraIM exceeded limit of participants"; break;
    case P2ERR_SPECTRA_IM_NOT_SUPPORTED: ret = "SpectraIM operation is not supported"; break;
    case P2ERR_SPECTRA_IM_NOT_CONNECTED: ret = "SpectraIM is not connected"; break;
    case P2ERR_SPECTRA_IM_NOT_INITIALIZED: ret = "SpectraIM is not initialized"; break;
    case P2ERR_SPECTRA_IM_WRONG_PORT: ret = "SpectraIM wrong port"; break;
    case P2ERR_SPECTRA_IM_WRONG_MQ_TIMOUT: ret = "SpectraIM wrong MQ timeout"; break;
    case P2ERR_SPECTRA_IM_WRONG_PART_COUNT: ret = "SpectraIM wrong part count"; break;
    // </FOSRV-2838>

    case P2ERR_CALCGO_API_ERROR_OPTION_NOT_FOUND: ret = "Option not found"; break;

    default: ret="No description"; break;
    }
    return ret;
}

#define E2S(e) case e: return #e ;

inline const char* GetErrorFromEnum(int error)
{
    switch (error)
    {
    E2S(P2ERR_COMMON_ERROR);
    E2S(P2ERR_BRGO_NOT_ENOUGH);
    E2S(P2ERR_CLGO_NOT_ENOUGH);
    E2S(P2ERR_SETTLEMENT_ACCOUNT_GO_NOT_ENOUGH);
    E2S(P2ERR_RFGO_NOT_ENOUGH);
    E2S(P2ERR_FASTCALCBCALL_NOT_CALLED);
    E2S(P2ERR_CLIENT_BUY_SPOT_LIMIT_RAISED);
    E2S(P2ERR_CLIENT_SELL_SPOT_LIMIT_RAISED);
    E2S(P2ERR_BROKER_BUY_SPOT_LIMIT_RAISED);
    E2S(P2ERR_BROKER_SELL_SPOT_LIMIT_RAISED);
    E2S(P2ERR_CROSS_TRADE);
    E2S(P2ERR_DEL_ORDER_NOT_EXIST);
    E2S(P2ERR_MOVE_ORDER_NOT_EXIST);
    E2S(P2ERR_NOT_FULL_REDUCTION);
    E2S(P2ERR_TRANSFER_BROKER_NOT_ENOUGH_FREE_MONEY);
    E2S(P2ERR_TRANSFER_BROKER_NOT_ENOUGH_FREE_PLEDGE);
    E2S(P2ERR_TRANSFER_BROKER_NOT_ENOUGH_MONEY_ALLOWED_FOR_WITHDRAWAL);
    E2S(P2ERR_TRANSFER_BROKER_NOT_ENOUGH_AMOUNT_PLEDGE);
    E2S(P2ERR_TRANSFER_BROKER_NOT_ENOUGH_AMOUNT_MONEY_PLEDGE);
    E2S(P2ERR_TRANSFER_SEPARATED_BROKER_NOT_ENOUGH_BALANCE_MONEY);
    E2S(P2ERR_TRANSFER_SEPARATED_BROKER_NOT_ENOUGH_BALANCE_PLEDGE);
    E2S(P2ERR_TRANSFER_SETTLEMENT_ACCOUNT_NOT_ENOUGH_FREE_MONEY);
    E2S(P2ERR_TRANSFER_SETTLEMENT_ACCOUNT_NOT_ENOUGH_FREE_PLEDGE);
    E2S(P2ERR_TRANSFER_SETTLEMENT_ACCOUNT_NOT_ENOUGH_MONEY_ALLOWED_FOR_WITHDRAWAL);
    E2S(P2ERR_TRANSFER_RF_NOT_ENOUGH_FREE_MONEY);
    E2S(P2ERR_TRANSFER_RF_NOT_ENOUGH_MONEY_ALLOWED_FOR_WITHDRAWAL);
    E2S(P2ERR_TRANSFER_SENDER_NOT_FOUND);
    E2S(P2ERR_TRANSFER_RECIPIENT_NOT_FOUND);
    E2S(P2ERR_TRANSFER_BETWEEN_DIFFERENT_RF);
    E2S(P2ERR_TRANSFER_BROKER_NOT_FOUND);
    E2S(P2ERR_TRANSFER_NOT_ALLOWED);
    E2S(P2ERR_TRANSFER_ATTEMPT_TO_CLEARING_MONEY_TRANSFER_FROM_SEPARATED_BROKER);
    E2S(P2ERR_TRANSFER_ATTEMPT_TO_TRANSFER_FROM_NON_PROPER_SETTLEMENT_ACCOUNT);
    E2S(P2ERR_TRANSFER_ATTEMPT_TO_TRANSFER_PLEDGE);
    E2S(P2ERR_TRANSFER_ATTEMPT_TO_TRANSFER_MONEY_PLEDGE);
    E2S(P2ERR_PRE_CLIENT_ORDER_COUNT_RAISED);
    E2S(P2ERR_FP_DIVISION_BY_ZERO);
    E2S(P2ERR_FP_OVERFLOW);
    E2S(P2ERR_ACCESS_VIOLATION);
    E2S(P2ERR_COMMON_SEH_EXCEPTION);
    E2S(P2ERR_ORDER_IS_BEING_MOVED);
    E2S(P2ERR_MULTILEG_ORDER_MOVING);
    E2S(P2ERR_ADDRESS_ORDER_MOVING);
    E2S(P2ERR_ORDERS_HAVE_DIFFERENT_INSTRUMENTID);
    E2S(P2ERR_POSSIBLE_CROSSTRADE);
    E2S(P2ERR_CANNOT_CHANGE_DIRECTION);
    E2S(P2ERR_FOREIGN_ORDER_MOVING);
    E2S(P2ERR_ORDERS_FROM_DIFFERENT_CLIENTS);
    E2S(P2ERR_MOVE_SAME_ORDER);
    E2S(P2ERR_TRADE_CHK_OPERATION_NOT_SUPPORTED);
    E2S(P2ERR_POSITION_QUANTITY_OVERFLOW);
    E2S(P2ERR_AGGREGATED_BUY_ORDERS_QUANTITY_OVERFLOW);
    E2S(P2ERR_AGGREGATED_SELL_ORDERS_QUANTITY_OVERFLOW);
    E2S(P2ERR_CLIENT_ABSENT);
    E2S(P2ERR_INSTRUMENT_ABSENT);
    E2S(P2ERR_ISIN_ABSENT);
    E2S(P2ERR_RESERVEFEE_TRADEMODEID_ALREADY_EXIST);
    E2S(P2ERR_RESERVEFEE_INSTRUMENTID_ALREADY_EXIST);
    E2S(P2ERR_RESERVEFEE_ISIN_ALREADY_EXIST);
    E2S(P2ERR_RESERVEFEE_TRADEMODEID_NOT_FOUND);
    E2S(P2ERR_RESERVEFEE_INSTRUMENTID_NOT_FOUND);
    E2S(P2ERR_RESERVEFEE_ISIN_NOT_FOUND);
    E2S(P2ERR_TRADEMODEID_AND_GROUPID_SPECIFIED);
    E2S(P2ERR_BAD_TRADINGHALT_REGIME);
    E2S(P2ERR_CHECK_DATEEXP_IS_OUT);
    E2S(P2ERR_CHECK_CLIENT_NOT_FOUND);
    E2S(P2ERR_CHECK_USER_NOT_FOUND);
    E2S(P2ERR_CHECK_USER_IS_OUT_BY_DATE);
    E2S(P2ERR_CHECK_BROKER_NOT_FOUND_OR_DELETED);
    E2S(P2ERR_CHECK_ADMIN_OPER_INPUT);
    E2S(P2ERR_CHECK_ADMIN_OPER_INPUT_NOT_SPECIFIED);
    E2S(P2ERR_CHECK_MAINTRADER_OPER_INPUT);
    E2S(P2ERR_CHECK_MAINTRADER_OPER_RF_INPUT);
    E2S(P2ERR_CHECK_TRADER_OPER_INPUT);
    E2S(P2ERR_CHECK_NOT_ENOUGH_RIGHTS);
    E2S(P2ERR_CHECK_LOGIN_FORBIDDEN);
    E2S(P2ERR_CHECK_WRONG_LOGIN_GROUP);
    E2S(P2ERR_CHECK_DU_FORBIDDEN);
    E2S(P2ERR_CHECK_ADDRESS_ORDER);
    E2S(P2ERR_CHECK_RTSCODE_IS_EMPTY); 
    E2S(P2ERR_CHECK_WRONG_DONTCHECKMONEY_MODE);
    E2S(P2ERR_CHECK_TRADEMODES_NOT_FOUND); 
    E2S(P2ERR_CHECK_GENERAL_TRADER_FORBIDDEN); 
    E2S(P2ERR_CHECK_TRADER_FORBIDDEN); 
    E2S(P2ERR_CHECK_CLIENT_FORBIDDEN); 
    E2S(P2ERR_CHECK_INSTRUMENT_NOT_FOUND); 
    E2S(P2ERR_CHECK_INSTRUMENT_TRADE_OFF); 
    E2S(P2ERR_CHECK_ORDER_WRONG_PARAMETERS); 
    E2S(P2ERR_CHECK_ORDER_WRONG_PRICE);
    E2S(P2ERR_CHECK_ORDER_WRONG_QUANTITY); 
    E2S(P2ERR_CHECK_RTSMONEY_DEPO_NOT_FOUND);
    E2S(P2ERR_CHECK_NON_MULTIPLICITY_PRICE); 
    E2S(P2ERR_CHECK_OUT_OF_LIMITS);
    E2S(P2ERR_CHECK_LICENSE_NOT_FOUND);
    E2S(P2ERR_CHECK_PROHIBITION_NOT_FOUND);
    E2S(P2ERR_CHECK_DEALER_LICENSE_NOT_FOUND); 
    E2S(P2ERR_CHECK_DEALER_LICENSE_FOR_OPEN_NOT_FOUND);
    E2S(P2ERR_CHECK_BROKER_LICENSE_NOT_FOUND); 
    E2S(P2ERR_CHECK_BROKER_LICENSE_FOR_OPEN_NOT_FOUND);
    E2S(P2ERR_CHECK_STOCK_BROKER_LICENSE_NOT_FOUND); 
    E2S(P2ERR_CHECK_STOCK_BROKER_LICENSE_FOR_OPEN_NOT_FOUND);
    E2S(P2ERR_CHECK_DU_LICENSE_NOT_FOUND); 
    E2S(P2ERR_CHECK_DU_LICENSE_FOR_OPEN_NOT_FOUND);
    E2S(P2ERR_CHECK_ADMIN_TS_PROHIBITION);
    E2S(P2ERR_CHECK_ADMIN_TS_POS_OPENING_PROHIBITION);
    E2S(P2ERR_CHECK_ADMIN_TS_SHORT_POS_OPENING_PROHIBITION);
    E2S(P2ERR_CHECK_ADMIN_CC_PROHIBITION);
    E2S(P2ERR_CHECK_ADMIN_CC_POS_OPENING_PROHIBITION);
    E2S(P2ERR_CHECK_ADMIN_CC_SHORT_POS_OPENING_PROHIBITION);
    E2S(P2ERR_CHECK_RF_PROHIBITION); 
    E2S(P2ERR_CHECK_RF_POS_OPENING_PROHIBITION); 
    E2S(P2ERR_CHECK_RF_SHORT_POS_OPENING_PROHIBITION); 
    E2S(P2ERR_CHECK_BF_PROHIBITION); 
    E2S(P2ERR_CHECK_BF_POS_OPENING_PROHIBITION); 
    E2S(P2ERR_CHECK_BF_SHORT_POS_OPENING_PROHIBITION); 
    E2S(P2ERR_CHECK_BAD_MODE); 
    E2S(P2ERR_CHECK_BAD_INITIATOR); 
    E2S(P2ERR_CHECK_BAD_TRADE_MODE_MASK); 
    E2S(P2ERR_CHECK_INVALID_USERLEVEL);
    E2S(P2ERR_CHECK_INVALID_USERGROUP);
    E2S(P2ERR_CHECK_INVALID_USERTRADEDATETIMES);
    E2S(P2ERR_CHECK_INVALID_USEROPERATORINPUT);
    E2S(P2ERR_ISIN_SHOULD_NOT_BE_SPECIFIED);
    E2S(P2ERR_FOREIGN_ORDER_DELETION); 
    E2S(P2ERR_CORE_LRPCQ_STOPPED); 
    E2S(P2ERR_CORE_PARTIAL_MASS_DELETION); 
    E2S(P2ERR_CHECK_CONTRABROKER_NOT_FOUND);
    E2S(P2ERR_VOLAT_NOT_FOUND);
    E2S(P2ERR_PRICE_ISNT_POSITIVE);
    E2S(P2ERR_PROHIBITION_NOT_FOUND); 
    E2S(P2ERR_PROHIBITION_ALREADY_EXISTS);
    E2S(P2ERR_LSCHECKS_FUT_NOT_FOUND); 
    E2S(P2ERR_LSCHECKS_LEG_NOT_FOUND);
    E2S(P2ERR_LSCHECKS_TOO_MANY_LEGS);
    E2S(P2ERR_LSCHECKS_OPTION_IN_MULTILEG);
    E2S(P2ERR_LSCHECKS_CLEARING_INSTR_NOT_FOUND);
    E2S(P2ERR_LSCHECKS_UNDERDETERMINED_MULTILEG);
    E2S(P2ERR_LSCHECKS_BASECONTRACT_NOT_FOUND);
    E2S(P2ERR_LSCHECKS_WRONG_BASECONTRACTID);
    E2S(P2ERR_LSCHECKS_WRONG_BASECONTRACT);
    E2S(P2ERR_LSCHECKS_PROHIBITION_NOT_FOUND);
    E2S(P2ERR_LSCHECKS_COMMIT_REQUIRED);
    E2S(P2ERR_LSCHECKS_CLEAR_REQUIRED);
    E2S(P2ERR_LSCHECKS_RF_NOT_FOUND);
    E2S(P2ERR_LSCHECKS_NON_MULTIPLICITY_SWAP_PRICE_OR_RATE);
    E2S(P2ERR_CONSISTENCY_VIRTUAL_FUTURES_NOT_FOUND);
    E2S(P2ERR_CONSISTENCY_CANNOT_REINSERT_PROHIBITION);
    E2S(P2ERR_POSITION_CALCULATOR_COMMON_ERROR);
    E2S(P2ERR_DATALOAD_RF_NOT_FOUND);
    E2S(P2ERR_DATALOAD_SETTLEMENT_ACCOUNT_NOT_FOUND);
    E2S(P2ERR_NONTRADE_CHK_OPERATION_NOT_ALLOWED);
    E2S(P2ERR_NONTRADE_CHK_BASECONTRACT_NOT_FOUND);
    E2S(P2ERR_POSITION_CALCULATOR_INSTR_NOT_FOUND);
    E2S(P2ERR_POSITION_CALCULATOR_BASECONTRACT_NOT_FOUND);
    E2S(P2ERR_POSITION_CALCULATOR_FUTURES_NOT_FOUND);
    E2S(P2ERR_POSITION_CALCULATOR_DIFFERENT_FUTURES_MONEYCOEFF_IN_MULTILEG);
    E2S(P2ERR_POSITION_CALCULATOR_INVESTOR_NOT_FOUND);
    E2S(P2ERR_POSITION_CALCULATOR_MULTILEG_ISIN_IN_POSITION);
    E2S(P2ERR_POSITION_CALCULATOR_ISIN_NOT_FOUND);
    E2S(P2ERR_POSITION_CALCULATOR_VIRTFUTURES_NOT_FOUND);
    E2S(P2ERR_POSITION_CALCULATOR_TRADEMODEID_NOT_FOUND);
    E2S(P2ERR_POSITION_CALCULATOR_BASECONTRACT_ALREADY_EXISTS);
    E2S(P2ERR_POSITION_CALCULATOR_TRADEMODE_ALREADY_EXISTS);
    E2S(P2ERR_POSITION_CALCULATOR_VIRTFUTURES_ALREADY_EXISTS);
    E2S(P2ERR_POSITION_CALCULATOR_FUTURES_ALREADY_EXISTS);
    E2S(P2ERR_POSITION_CALCULATOR_OPTION_ALREADY_EXISTS);
    E2S(P2ERR_POSITION_CALCULATOR_ISIN_ALREADY_EXISTS_IN_MULTILEG);
    E2S(P2ERR_POSITION_CALCULATOR_INSTRUMENT_ALREADY_EXISTS);
    E2S(P2ERR_POSITION_CALCULATOR_POSITION_ALREADY_EXISTS);
    E2S(P2ERR_POSITION_CALCULATOR_INCORRECT_PRICE_TYPE);
    E2S(P2ERR_POSITION_CALCULATOR_PAIR_INSTRUMENTID_EXPECTED);
    E2S(P2ERR_POSITION_CALCULATOR_MULTILEG_NOT_FOUND);
    E2S(P2ERR_POSITION_CALCULATOR_OPTION_NOT_FOUND);
    E2S(P2ERR_POSITION_CALCULATOR_ADDRESS_REPO);
    E2S(P2ERR_CHECK_LEGACY_BADORDERTYPE);
    E2S(P2ERR_CHECK_LEGACY_BADCOTIRCONTR);
    E2S(P2ERR_CHECK_LEGACY_USERNOTFOUND);
    E2S(P2ERR_CHECK_LEGACY_BROKERNOTFOUND);
    E2S(P2ERR_CHECK_LEGACY_CLIENTNOTFOUND);
    E2S(P2ERR_CHECK_LEGACY_BROKERCODE_LOGIN_MISMATCH);
    E2S(P2ERR_CHECK_LEGACY_CLIENTCODE_LOGIN_MISMATCH);
    E2S(P2ERR_CALCGO_API_COMMON_ERROR);
    E2S(P2ERR_CALCGO_API_ERROR_BC_ALREADY_EXIST);
    E2S(P2ERR_CALCGO_API_ERROR_FUTURES_ALREADY_EXIST);
    E2S(P2ERR_CALCGO_API_ERROR_VIRTFUTURES_ALREADY_EXIST);
    E2S(P2ERR_CALCGO_API_ERROR_OPTION_ALREADY_EXIST);
    E2S(P2ERR_CALCGO_API_ERROR_WRONG_HASOPTIONS);
    E2S(P2ERR_CALCGO_API_ERROR_BC_NOT_FOUND);
    E2S(P2ERR_CALCGO_API_ERROR_FUTURES_NOT_FOUND);
    E2S(P2ERR_CALCGO_API_ERROR_VIRTFUTURES_NOT_FOUND);
    E2S(P2ERR_CALCGO_API_ERROR_PART_ALREADY_EXIST);
    E2S(P2ERR_CALCGO_API_ERROR_PART_NOT_FOUND);
    E2S(P2ERR_CALCGO_API_ERROR_FIRM_ALREADY_EXIST);
    E2S(P2ERR_CALCGO_API_ERROR_FIRM_NOT_FOUND);
    E2S(P2ERR_CALCGO_API_ERROR_SETTLEMENT_ACCOUNT_ALREADY_EXIST);
    E2S(P2ERR_CALCGO_API_ERROR_WRONG_POINTSNUM);
    E2S(P2ERR_PROH_CLIENT_NOT_FOUND);
    E2S(P2ERR_FUTURES_IS_MULTILEG);
    E2S(P2ERR_TRADING_CLOSED);
    E2S(P2ERR_NONTRADE_WRONG_INPUT_PARAMETERS);
    E2S(P2ERR_CHANGECLIENTMONEY_NOT_ENOUGH_MONEY);
    E2S(P2ERR_CHANGEBROKERVCB_BROKER_NOT_FOUND);
    E2S(P2ERR_CHANGEBROKERVCB_BASECONTRACT_NOT_FOUND);
    E2S(P2ERR_CHANGEBROKERVCB_OPERATION_NOT_ALLOWED);
    E2S(P2ERR_T0_NOT_ALLOWED);
    E2S(P2ERR_FIRSTPRICE_AND_DELIVERYPRICE_ARE_DIFF);
    E2S(P2ERR_ADDRESS_TRADEMODE_ONLY);
    E2S(P2ERR_NON_REPO_ISIN);
    E2S(P2ERR_CHECK_CLRINSTRUMENT_NOT_FOUND);
    // <FOSRV-2379>
    E2S(P2ERR_TRCLPOS_POSITION_NOT_FOUND);
    E2S(P2ERR_TRCLPOS_NOT_ENOUGH_POSITION);
    E2S(P2ERR_TRCLPOS_INN_MISMATCH);
    E2S(P2ERR_TRCLPOS_RF_MISMATCH);
    E2S(P2ERR_TRCLPOS_TARGET_IS_ZERO_SECTION);
    E2S(P2ERR_TRCLPOS_BF_ADMIN_OPERINPUT);
    E2S(P2ERR_TRCLPOS_PROHIBITED_INSTRUMENT);
    // </FOSRV-2379>
    E2S(P2ERR_CALCGO_API_ERROR_OPTION_NOT_FOUND);
    }
    return "";
}


enum
{
	RPT_M2_BEGIN = 10000,
	RPT_M2_END = 11000
};

enum
{
    RPT_ERROR = -1, //Ошибка при выполнении операции.
    RPT_NOERROR = 0, //Операция выполнена успешно.
    RPT_USER_NOT_FOUND = 1, //Нет такого пользователя.
    RPT_DEALER_NOT_FOUND = 2, //Нет такого Дилера.
    RPT_SESSION_NOT_ACTIVE = 3,
    RPT_OPER_ERROR = 5, //Ошибка при выполнении операции.
    RPT_NOT_ENOUGH_RIGHTS = 6, //Нет прав на выполнение операции.
    RPT_CHECK_LOGIN_FORBIDDEN = 7, //Попытка доступа к чужому счету дилера.
    RPT_FOREIGN_ORDER_DELETION = 8, //Нет прав на удаление заявки другого клиента/пользователя своей фирмы
    RPT_CHECK_ADMIN_OPER_INPUT = 9, //Фирме операции с заявками заблокированы Клиринговым Центром.
    RPT_DEL_ORDER_NOT_EXIST = 14, //Не найдена заявка для удаления
    RPT_CHECK_ADMIN_TS_PROHIBITION = 25, //Запрет Администратора торгов на постановку любых заявок.
    RPT_CHECK_ADMIN_TS_POS_OPENING_PROHIBITION = 26, //Запрет Администратора торгов на открытие позиций.
    RPT_CHECK_ADMIN_TS_SHORT_POS_OPENING_PROHIBITION = 27, //Запрет Администратора торгов на открытие позиций на Продажу.
    RPT_CROSS_TRADE =  31, //Не разрешена встречная заявка на один счет и/или ИНН.
    RPT_PRICE_OUT_OF_LIMITS  = 32, //Цена сделки вне лимита
    RPT_CLIENT_NOT_FOUND = 34, //Попытка операции на несуществующий код клиента.
    RPT_WRONG_INPUT_PARAMETERS = 35, //Ошибка в задании входных параметров.
    RPT_WRONG_BASECONTRACT = 36, //Попытка операции по несуществующему базовому активу.
    RPT_MULTILEG_ORDER_MOVING = 37, //Перестановка заявок по Связкам недопустима
    RPT_ADDRESS_ORDER_MOVING = 38, //Перестановка адресных заявок недопустима
    RPT_NON_MULTIPLICITY_PRICE = 39, //Цена не кратна минимальному шагу цены.
    RPT_CONTRABROKER_NOT_FOUND = 40, //Попытка адресовать внесистемную заявку несуществующему контрагенту.
    RPT_USER_IS_OUT_BY_DATE = 41, //Не наступил или истек срок доверенности пользователя.
    RPT_MAINTRADER_OPER_RF_INPUT = 42, //Запрещена работа Главным трейдером РФ.
    RPT_ADMIN_OPER_INPUT_NOT_SPECIFIED = 43, //Администратор не поставил признак Операторного ввода.
    RPT_RTSCODE_IS_EMPTY = 45, //Попытка поставить внесистемную заявку от фирмы, у которой не прописан код РТС.
    RPT_ANONYM_INSTRUMENT_NOT_FOUND = 46, //По этому инструменту разрешены только внесистемные заявки.
    RPT_TRADING_CLOSED = 47, //В назначенной сессии по этому инструменту торгов нет.
    RPT_CHECK_ADDRESS_ORDER = 49, //Попытка поставить внесистемную заявку от трейдера одного клиентского счета, а не от кода фирмы.
    RPT_MOVE_ORDER_NOT_EXIST = 50, //Не найдена заявка для перестановки.
    RPT_ORDER_WRONG_QUANTITY = 53, //Ошибка в задании входного параметра - количество. Слишком велико.
    RPT_CHECK_WRONG_DONTCHECKMONEY_MODE = 58, //Нет прав на выставление заявки без проверки достаточности средств на уровне клиента.
    RPT_CHECK_BF_TRADEMODE_PROHIBITION = 68,  //68 Запрет брокера на постановку любых заявок на рынке РТС Стандарт.
    RPT_CHECK_RF_TRADEMODE_PROHIBITION = 69,  //69 Запрет Главного трейдера на постановку любых заявок на рынке РТС Стандарт.
    RPT_POSITION_QUANTITY_OVERFLOW = 71, // 71 Переполнение количества в позиции
    RPT_AGGREGATED_BUY_ORDERS_QUANTITY_OVERFLOW = 73, // 73 Суммарное количество контрактов в заявках на покупку превышает допустимый предел
    RPT_AGGREGATED_SELL_ORDERS_QUANTITY_OVERFLOW = 74, // 74 Суммарное количество контрактов в заявках на продажу превышает допустимый предел
    RPT_TRADE_OPERATION_NOT_SUPPORTED = 70,     //70 Операция не поддерживается для данного инструмента
    RPT_ORDER_IS_BEING_MOVED = 72,              // Ордер уже в процессе мува, ждите реплая для следущего мува.
    RPT_WRONG_ORDER = 201,  // Неверный порядок сообщений
    RPT_CORE_TIMEOUT =202,  // Истекло время ожидания ответа на пакет
    RPT_HANDSHAKE_NOT_ESTABLISHED = 203,   // Не произведена процедура handshake
    RPT_CONNECTION_NOT_INITED = 204,  // Соединение агента с SQL не проинициализировано
    RPT_AGENT_INVALID_SESSION = 205,  // Неверная сессия агента
    RPT_AGENT_PACKET_MISMATCH = 206,  // Потерян пакет
    RPT_AGENT_INTERNAL_ERROR = 207, // Внутренняя ошибка в агенте
    RPT_AGENT_BUSINESS_PROCESS_HAS_BEEN_BROKEN = 208, // В текущем алгортме была вызвана процедура BreakCurrnetBusinessProcess
    RPT_CHECK_ADMIN_CC_PROHIBITION = 310, //Запрет Администратора Клирингового Центра на постановку любых заявок.
    RPT_CHECK_ADMIN_CC_POS_OPENING_PROHIBITION = 311, //Запрет Администратора Клирингового Центра на открытие позиций.
    RPT_CHECK_ADMIN_CC_SHORT_POS_OPENING_PROHIBITION = 312, //Запрет Администратора Клирингового Центра на открытие позиций на Продажу.
    RPT_CHECK_BF_PROHIBITION = 314, //Запрет Трейдера на постановку любых заявок по клиентскому счету.
    RPT_CHECK_BF_POS_OPENING_PROHIBITION = 315, //Запрет Трейдера на открытие позиций по клиентскому счету.
    RPT_CHECK_BF_SHORT_POS_OPENING_PROHIBITION = 316, //Запрет Трейдера на открытие позиций на Продажу по клиентскому счету.
    RPT_CHECK_RTSMONEY_DEPO_NOT_FOUND = 318, //Запрет Клиринга на постановку любых заявок по клиентскому счету: для клиентского регистра нет Депо счета, разрешённого для Поставки по инструментам RTS Money.
    RPT_PRE_CLIENT_ORDER_COUNT_RAISED = 320, //Превышен допустимый предел числа активных заявок с клиентского регистра по инструменту.
    RPT_SETTLEMENT_ACCOUNT_GO_NOT_ENOUGH = 331, //Нехватка средств по расчетному коду.
    RPT_CLGO_NOT_ENOUGH = 332, //Нехватка средств по лимитам клиента.
    RPT_BRGO_NOT_ENOUGH = 333, //Нехватка средств по брокерской фирме.
    RPT_RFGO_NOT_ENOUGH = 334, //Нехватка средств по расчетной фирме.
    RPT_CLIENT_BUY_SPOT_LIMIT_RAISED = 335, //Превышен лимит клиента на покупку бумаг.
    RPT_BROKER_BUY_SPOT_LIMIT_RAISED = 336, //Превышен лимит брокера на покупку бумаг.
    RPT_CLIENT_SELL_SPOT_LIMIT_RAISED = 337, //Превышен лимит клиента на продажу бумаг.
    RPT_BROKER_SELL_SPOT_LIMIT_RAISED = 338, //Превышен лимит брокера на продажу бумаг.
    RPT_WRONG_NONTRADE_INPUT_PARAMETERS = 4000, //Ошибка во входных параметрах в неторговых операциях
    RPT_NONTRADE_OPERATION_NOT_ALLOWED = 4001, //У пользователя нет прав на выполнение операции.
    RPT_CHANGECLIENTMONEY_NOT_ENOUGH_MONEY = 4005, // Нехватка средств при изменении клиентского лимита
    RPT_COMMON_ERROR = 4018, //Администратор производит изменение параметров расчета ГО.
    RPT_CHANGEBROKERVCB_BROKER_NOT_FOUND = 4031, //Невозможно изменить параметры по Брокеру. Кода нет в таблице клиентов.
    RPT_CHANGEBROKERVCB_BASECONTRACT_NOT_FOUND = 4032, //Невозможно изменить параметры по Брокеру. Кода БА нет в таблице базовых активов.
    RPT_CHANGEBROKERVCB_OPERATION_NOT_ALLOWED = 4033, //Невозможно изменить параметры по Брокеру. Нет прав на работу с этим базовым активом.
    RPT_TRANSFER_ATTEMPT_TO_TRANSFER_PLEDGE = 4035, //Перевод залоговых средств частичного обеспечения запрещен.
    RPT_PROH_CLIENT_NOT_FOUND = 4091, //Кода нет в таблице клиентов.
    RPT_BASECONTRACT_NOT_FOUND = 4092, //Кода БА нет в таблице базовых активов.
    RPT_FUTURES_NOT_FOUND = 4093, //Не найден указанный фьючерсный инструмент.
    RPT_WRONG_BASECONTRACTID = 4094, //Указанный фьючерсный инструмент не соответствует указанному БА.
    RPT_ISIN_SHOULD_NOT_BE_SPECIFIED =     4095, //Не м.б. указан конкретный фьючерс, когда БА указан - Для всех.
    RPT_PROHIBITION_NOT_FOUND = 4096, //Не найдено ограничение для удаления.
    //     4097, //Ограничение Главного трейдера нельзя удалить простому трейдеру.
    RPT_INSTRUMENT_NOT_FOUND = 4098, //Инструмент отсутствует в текущей сессии.
    RPT_NOT_FULL_REDUCTION = 4103, //Неполное сведение FOK заявки.
    RPT_REPO_FORBIDDEN = 4105, // Заявка по Анонимному Репо на РТС Стандарте должна быть только с указанием типа "Репо".
    RPT_T0_NOT_ALLOWED = 4108, //Нет прав на торговлю по Т0 инструментам рынка РТС Стандарт.
    RPT_NON_MULTIPLICITY_SWAP_PRICE_OR_RATE = 4109, //Ставка (или Своп-цена) не кратна минимальному шагу.
    RPT_FIRSTPRICE_AND_DELIVERYPRICE_ARE_DIFF = 4110, //Цена первой части сделки не совпадает с ценой поставки.
    RPT_FUTURES_IS_MULTILEG = 4112, //Указанный фьючерсный инструмент - Связка, по нему Ограничения не ставятся.
    RPT_TRANSFER_SENDER_NOT_FOUND = 4116, //Невозможно перевести денежные средства с БФ на БФ. БФ-донор не зарегистрирована в торгах.
    RPT_TRANSFER_RECIPIENT_NOT_FOUND = 4117, //Невозможно перевести денежные средства с БФ на БФ. БФ-получатель не зарегистрирована в торгах.
    RPT_TRANSFER_BROKER_NOT_ENOUGH_FREE_MONEY = 4118, //Не хватает свободных денежных средств у Брокерской Фирмы.
    RPT_TRANSFER_BROKER_NOT_ENOUGH_FREE_PLEDGE = 4119, //Не хватает свободных залоговых средств частичного обеспечения у Брокерской Фирмы.
    RPT_TRANSFER_SEPARATED_BROKER_NOT_ENOUGH_BALANCE_MONEY = 4120, //Не хватает сальдо денежных средств у Обособленного раздела.
    RPT_TRANSFER_SEPARATED_BROKER_NOT_ENOUGH_BALANCE_PLEDGE = 4121, //Не хватает сальдо залоговых средств частичного обеспечения у Обособленного раздела.
    RPT_TRANSFER_RF_NOT_ENOUGH_FREE_MONEY = 4122, //Не хватает свободных денежных средств на РФ.
    RPT_TRANSFER_BROKER_NOT_ENOUGH_AMOUNT_PLEDGE = 4123, //Не хватает в наличии залоговых средств частичного обеспечения у Брокерской Фирмы.
    RPT_TRANSFER_BROKER_NOT_FOUND = 4124, //Не найден код указанной Брокерской Фирмы.
    RPT_TRANSFER_BETWEEN_DIFFERENT_RF = 4125, //Попытка перевода между разделами различных Расчетных Фирм.
    RPT_TRANSFER_NOT_ALLOWED = 4126, //Перевод запрещен. Ошибка логики переводов.
    RPT_TRANSFER_SETTLEMENT_ACCOUNT_NOT_ENOUGH_FREE_MONEY = 4127, //Не хватает свободных денежных средств на РК.    
    RPT_TRANSFER_BROKER_NOT_ENOUGH_MONEY_ALLOWED_FOR_WITHDRAWAL = 4134, //Не хватает в наличии денежных средств у Брокерской Фирмы.
    RPT_TRANSFER_RF_NOT_ENOUGH_MONEY_ALLOWED_FOR_WITHDRAWAL = 4135, //Не хватает в наличии денежных средств, разрешённых для вывода с РФ.
    RPT_TRANSFER_ATTEMPT_TO_TRANSFER_MONEY_PLEDGE = 4136, //Перевод залоговых средств полного обеспечения запрещен.
    RPT_TRANSFER_BROKER_NOT_ENOUGH_AMOUNT_MONEY_PLEDGE = 4137, //Не хватает в наличии залоговых средств полного обеспечения у Брокерской Фирмы.
    RPT_TRANSFER_SETTLEMENT_ACCOUNT_NOT_ENOUGH_MONEY_ALLOWED_FOR_WITHDRAWAL = 4138, //Не хватает в наличии денежных средств, разрешённых для вывода с РК.
    RPT_TRANSFER_SETTLEMENT_ACCOUNT_NOT_ENOUGH_FREE_PLEDGE = 4139, //Не хватает свободных залоговых средств частичного обеспечения у РК.
    // <FOSRV-2379>
    RPT_TRCLPOS_POSITION_NOT_FOUND = 4140, // Нет позиций для переноса по выбранному инструменту
    RPT_TRCLPOS_NOT_ENOUGH_POSITION = 4141, // Не хватает открытых позиций для переноса
    RPT_TRCLPOS_INN_MISMATCH = 4142, // Попытка переноса позиций с клиентского счёта на чужой счёт (с другим ИНН)
    RPT_TRCLPOS_RF_MISMATCH = 4143, // Нельзя переносить позиции между БФ принадлежащим разным РФ
    RPT_TRCLPOS_TARGET_IS_ZERO_SECTION = 4144, // Нельзя переносить позиции на нулевой раздел БФ
    RPT_TRCLPOS_BF_ADMIN_OPERINPUT = 4145, // Запрет Администратора торгов на перенос позиций по указанной БФ
    RPT_TRCLPOS_PROHIBITED_INSTRUMENT = 4146, // Запрет на перенос позиции по указанному инструменту
    // </FOSRV-2379>
    RPT_OPTION_NOT_FOUND = 4147, //Не найден указанный опционный инструмент.

    RPT_LAST
};

inline int ReturnRptError(int p2err)
{
    switch (p2err)
    {
    case P2ERR_COMMON_ERROR:                    return RPT_COMMON_ERROR;
    case P2ERR_BRGO_NOT_ENOUGH:                 return RPT_BRGO_NOT_ENOUGH;
    case P2ERR_CLGO_NOT_ENOUGH:                 return RPT_CLGO_NOT_ENOUGH;
    case P2ERR_SETTLEMENT_ACCOUNT_GO_NOT_ENOUGH:    return RPT_SETTLEMENT_ACCOUNT_GO_NOT_ENOUGH;
    case P2ERR_RFGO_NOT_ENOUGH:                 return RPT_RFGO_NOT_ENOUGH;
    case P2ERR_FASTCALCBCALL_NOT_CALLED:        return RPT_COMMON_ERROR;
    case P2ERR_CLIENT_BUY_SPOT_LIMIT_RAISED:    return RPT_CLIENT_BUY_SPOT_LIMIT_RAISED;
    case P2ERR_CLIENT_SELL_SPOT_LIMIT_RAISED:   return RPT_CLIENT_SELL_SPOT_LIMIT_RAISED;
    case P2ERR_BROKER_BUY_SPOT_LIMIT_RAISED:    return RPT_BROKER_BUY_SPOT_LIMIT_RAISED;
    case P2ERR_BROKER_SELL_SPOT_LIMIT_RAISED:   return RPT_BROKER_SELL_SPOT_LIMIT_RAISED;
    case P2ERR_CROSS_TRADE:                     return RPT_CROSS_TRADE;
    case P2ERR_DEL_ORDER_NOT_EXIST:             return RPT_DEL_ORDER_NOT_EXIST;
    case P2ERR_MOVE_ORDER_NOT_EXIST:            return RPT_MOVE_ORDER_NOT_EXIST;
    case P2ERR_NOT_FULL_REDUCTION:              return RPT_NOT_FULL_REDUCTION;
    case P2ERR_TRADE_CHK_OPERATION_NOT_SUPPORTED: return RPT_TRADE_OPERATION_NOT_SUPPORTED;
    case P2ERR_POSITION_QUANTITY_OVERFLOW:      return RPT_POSITION_QUANTITY_OVERFLOW;
    case P2ERR_AGGREGATED_BUY_ORDERS_QUANTITY_OVERFLOW: return RPT_AGGREGATED_BUY_ORDERS_QUANTITY_OVERFLOW;
    case P2ERR_AGGREGATED_SELL_ORDERS_QUANTITY_OVERFLOW: return RPT_AGGREGATED_SELL_ORDERS_QUANTITY_OVERFLOW;
    
    case P2ERR_TRANSFER_BROKER_NOT_ENOUGH_FREE_MONEY:                             return RPT_TRANSFER_BROKER_NOT_ENOUGH_FREE_MONEY;
    case P2ERR_TRANSFER_BROKER_NOT_ENOUGH_FREE_PLEDGE:                            return RPT_TRANSFER_BROKER_NOT_ENOUGH_FREE_PLEDGE;
    case P2ERR_TRANSFER_BROKER_NOT_ENOUGH_MONEY_ALLOWED_FOR_WITHDRAWAL:           return RPT_TRANSFER_BROKER_NOT_ENOUGH_MONEY_ALLOWED_FOR_WITHDRAWAL;
    case P2ERR_TRANSFER_BROKER_NOT_ENOUGH_AMOUNT_PLEDGE:                          return RPT_TRANSFER_BROKER_NOT_ENOUGH_AMOUNT_PLEDGE;
    case P2ERR_TRANSFER_BROKER_NOT_ENOUGH_AMOUNT_MONEY_PLEDGE:                    return RPT_TRANSFER_BROKER_NOT_ENOUGH_AMOUNT_MONEY_PLEDGE;
    case P2ERR_TRANSFER_SEPARATED_BROKER_NOT_ENOUGH_BALANCE_MONEY:                return RPT_TRANSFER_SEPARATED_BROKER_NOT_ENOUGH_BALANCE_MONEY;
    case P2ERR_TRANSFER_SEPARATED_BROKER_NOT_ENOUGH_BALANCE_PLEDGE:               return RPT_TRANSFER_SEPARATED_BROKER_NOT_ENOUGH_BALANCE_PLEDGE;
    case P2ERR_TRANSFER_SETTLEMENT_ACCOUNT_NOT_ENOUGH_FREE_MONEY:                 return RPT_TRANSFER_SETTLEMENT_ACCOUNT_NOT_ENOUGH_FREE_MONEY;
    case P2ERR_TRANSFER_SETTLEMENT_ACCOUNT_NOT_ENOUGH_FREE_PLEDGE:                return RPT_TRANSFER_SETTLEMENT_ACCOUNT_NOT_ENOUGH_FREE_PLEDGE;
    case P2ERR_TRANSFER_SETTLEMENT_ACCOUNT_NOT_ENOUGH_MONEY_ALLOWED_FOR_WITHDRAWAL: return RPT_TRANSFER_SETTLEMENT_ACCOUNT_NOT_ENOUGH_MONEY_ALLOWED_FOR_WITHDRAWAL;
    case P2ERR_TRANSFER_RF_NOT_ENOUGH_FREE_MONEY:                                 return RPT_TRANSFER_RF_NOT_ENOUGH_FREE_MONEY;
    case P2ERR_TRANSFER_RF_NOT_ENOUGH_MONEY_ALLOWED_FOR_WITHDRAWAL:               return RPT_TRANSFER_RF_NOT_ENOUGH_MONEY_ALLOWED_FOR_WITHDRAWAL;
    case P2ERR_TRANSFER_SENDER_NOT_FOUND:                                         return RPT_TRANSFER_SENDER_NOT_FOUND;
    case P2ERR_TRANSFER_RECIPIENT_NOT_FOUND:                                      return RPT_TRANSFER_RECIPIENT_NOT_FOUND;
    case P2ERR_TRANSFER_BETWEEN_DIFFERENT_RF:                                     return RPT_TRANSFER_BETWEEN_DIFFERENT_RF;
    case P2ERR_TRANSFER_BROKER_NOT_FOUND:                                         return RPT_TRANSFER_BROKER_NOT_FOUND;
    case P2ERR_TRANSFER_NOT_ALLOWED:                                              return RPT_TRANSFER_NOT_ALLOWED;
    case P2ERR_TRANSFER_ATTEMPT_TO_CLEARING_MONEY_TRANSFER_FROM_SEPARATED_BROKER: return RPT_TRANSFER_NOT_ALLOWED;
    case P2ERR_TRANSFER_ATTEMPT_TO_TRANSFER_FROM_NON_PROPER_SETTLEMENT_ACCOUNT:   return RPT_TRANSFER_NOT_ALLOWED;
    case P2ERR_TRANSFER_ATTEMPT_TO_TRANSFER_PLEDGE:                               return RPT_TRANSFER_ATTEMPT_TO_TRANSFER_PLEDGE;
    case P2ERR_TRANSFER_ATTEMPT_TO_TRANSFER_MONEY_PLEDGE:                         return RPT_TRANSFER_ATTEMPT_TO_TRANSFER_MONEY_PLEDGE;
    case P2ERR_MOVE_SAME_ORDER:                                                   return RPT_WRONG_INPUT_PARAMETERS;

    case P2ERR_MULTILEG_ORDER_MOVING:               return RPT_MULTILEG_ORDER_MOVING;
    case P2ERR_ADDRESS_ORDER_MOVING:                return RPT_ADDRESS_ORDER_MOVING;
    case P2ERR_ORDERS_HAVE_DIFFERENT_INSTRUMENTID:  return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_POSSIBLE_CROSSTRADE:                 return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_CANNOT_CHANGE_DIRECTION:             return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_FOREIGN_ORDER_MOVING:                return RPT_FOREIGN_ORDER_DELETION;
    case P2ERR_ORDERS_FROM_DIFFERENT_CLIENTS:       return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_CLIENT_ABSENT:                       return RPT_CLIENT_NOT_FOUND;
    case P2ERR_INSTRUMENT_ABSENT:                   return RPT_INSTRUMENT_NOT_FOUND;
    case P2ERR_RESERVEFEE_ISIN_NOT_FOUND:           return RPT_WRONG_INPUT_PARAMETERS;   
    case P2ERR_RESERVEFEE_INSTRUMENTID_NOT_FOUND:   return RPT_INSTRUMENT_NOT_FOUND;
    case P2ERR_CHECK_DATEEXP_IS_OUT:                return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;
    case P2ERR_CHECK_CLIENT_NOT_FOUND:              return RPT_CLIENT_NOT_FOUND;
    case P2ERR_CHECK_USER_NOT_FOUND:                return RPT_USER_NOT_FOUND;
    case P2ERR_CHECK_USER_IS_OUT_BY_DATE:           return RPT_USER_IS_OUT_BY_DATE;
    case P2ERR_CHECK_BROKER_NOT_FOUND_OR_DELETED:   return RPT_DEALER_NOT_FOUND;
    case P2ERR_CHECK_ADMIN_OPER_INPUT_NOT_SPECIFIED:return RPT_ADMIN_OPER_INPUT_NOT_SPECIFIED;
    case P2ERR_CHECK_MAINTRADER_OPER_INPUT:         return RPT_MAINTRADER_OPER_RF_INPUT;
    case P2ERR_CHECK_MAINTRADER_OPER_RF_INPUT:      return RPT_MAINTRADER_OPER_RF_INPUT;
    case P2ERR_CHECK_ADMIN_OPER_INPUT:              return RPT_CHECK_ADMIN_OPER_INPUT;
    case P2ERR_CHECK_TRADER_OPER_INPUT:             return RPT_COMMON_ERROR;
    case P2ERR_CHECK_NOT_ENOUGH_RIGHTS:             return RPT_NOT_ENOUGH_RIGHTS;
    case P2ERR_CHECK_LOGIN_FORBIDDEN:               return RPT_CHECK_LOGIN_FORBIDDEN;
    case P2ERR_CHECK_DU_FORBIDDEN:                  return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_CHECK_RTSCODE_IS_EMPTY:              return RPT_RTSCODE_IS_EMPTY;
    case P2ERR_CHECK_CLIENT_FORBIDDEN:              return RPT_CHECK_BF_PROHIBITION;
    case P2ERR_CHECK_INSTRUMENT_NOT_FOUND:          return RPT_INSTRUMENT_NOT_FOUND;
    case P2ERR_CHECK_INSTRUMENT_TRADE_OFF:          return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_CHECK_ORDER_WRONG_PARAMETERS:        return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_CHECK_ORDER_WRONG_PRICE:             return RPT_PRICE_OUT_OF_LIMITS;
    case P2ERR_CHECK_ORDER_WRONG_QUANTITY:          return RPT_ORDER_WRONG_QUANTITY;
    case P2ERR_CHECK_RTSMONEY_DEPO_NOT_FOUND:       return RPT_CHECK_RTSMONEY_DEPO_NOT_FOUND;
    case P2ERR_CHECK_NON_MULTIPLICITY_PRICE:        return RPT_NON_MULTIPLICITY_PRICE;
    case P2ERR_CHECK_OUT_OF_LIMITS:                 return RPT_PRICE_OUT_OF_LIMITS;
    case P2ERR_CHECK_ADMIN_TS_PROHIBITION:                     return RPT_CHECK_ADMIN_TS_PROHIBITION;
    case P2ERR_CHECK_ADMIN_TS_POS_OPENING_PROHIBITION:         return RPT_CHECK_ADMIN_TS_POS_OPENING_PROHIBITION;
    case P2ERR_CHECK_ADMIN_TS_SHORT_POS_OPENING_PROHIBITION:   return RPT_CHECK_ADMIN_TS_SHORT_POS_OPENING_PROHIBITION;
    case P2ERR_CHECK_ADMIN_CC_PROHIBITION:                     return RPT_CHECK_ADMIN_CC_PROHIBITION;
    case P2ERR_CHECK_ADMIN_CC_POS_OPENING_PROHIBITION:         return RPT_CHECK_ADMIN_CC_POS_OPENING_PROHIBITION;
    case P2ERR_CHECK_ADMIN_CC_SHORT_POS_OPENING_PROHIBITION:   return RPT_CHECK_ADMIN_CC_SHORT_POS_OPENING_PROHIBITION;
    case P2ERR_CHECK_RF_PROHIBITION:                    return RPT_CHECK_BF_PROHIBITION;
    case P2ERR_CHECK_RF_POS_OPENING_PROHIBITION:        return RPT_CHECK_BF_POS_OPENING_PROHIBITION;
    case P2ERR_CHECK_RF_SHORT_POS_OPENING_PROHIBITION:  return RPT_CHECK_BF_SHORT_POS_OPENING_PROHIBITION;
    case P2ERR_CHECK_BF_PROHIBITION:                    return RPT_CHECK_BF_PROHIBITION;
    case P2ERR_CHECK_BF_POS_OPENING_PROHIBITION:        return RPT_CHECK_BF_POS_OPENING_PROHIBITION;
    case P2ERR_CHECK_BF_SHORT_POS_OPENING_PROHIBITION:  return RPT_CHECK_BF_SHORT_POS_OPENING_PROHIBITION;
    case P2ERR_CHECK_INVALID_USERLEVEL:                 return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_CHECK_INVALID_USERGROUP:                 return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_CHECK_INVALID_USERTRADEDATETIMES:        return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_CHECK_INVALID_USEROPERATORINPUT:         return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_CHECK_BAD_MODE:                          return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_CHECK_BAD_INITIATOR:                     return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_CHECK_BAD_TRADE_MODE_MASK:               return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_ISIN_SHOULD_NOT_BE_SPECIFIED:            return RPT_ISIN_SHOULD_NOT_BE_SPECIFIED;

    case P2ERR_FOREIGN_ORDER_DELETION:                  return RPT_FOREIGN_ORDER_DELETION;
    case P2ERR_CHECK_LEGACY_BADORDERTYPE:               return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_CHECK_LEGACY_BADCOTIRCONTR:              return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_CHECK_LEGACY_USERNOTFOUND:               return RPT_USER_NOT_FOUND;
    case P2ERR_CHECK_LEGACY_BROKERNOTFOUND:             return RPT_DEALER_NOT_FOUND;
    case P2ERR_CHECK_LEGACY_CLIENTNOTFOUND:             return RPT_CLIENT_NOT_FOUND;

    case P2ERR_CHECK_LEGACY_BROKERCODE_LOGIN_MISMATCH:         return RPT_USER_NOT_FOUND;
    case P2ERR_CHECK_LEGACY_CLIENTCODE_LOGIN_MISMATCH:         return RPT_USER_NOT_FOUND;
    case P2ERR_CALCGO_API_COMMON_ERROR:                        return RPT_COMMON_ERROR;
    case P2ERR_CALCGO_API_ERROR_BC_ALREADY_EXIST:              return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_CALCGO_API_ERROR_FUTURES_ALREADY_EXIST:         return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_CALCGO_API_ERROR_VIRTFUTURES_ALREADY_EXIST:     return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_CALCGO_API_ERROR_OPTION_ALREADY_EXIST:          return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_CALCGO_API_ERROR_WRONG_HASOPTIONS:              return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_CALCGO_API_ERROR_BC_NOT_FOUND:                  return RPT_BASECONTRACT_NOT_FOUND;
    case P2ERR_CALCGO_API_ERROR_FUTURES_NOT_FOUND:             return RPT_FUTURES_NOT_FOUND;
    case P2ERR_CALCGO_API_ERROR_VIRTFUTURES_NOT_FOUND:         return RPT_INSTRUMENT_NOT_FOUND;
    case P2ERR_CALCGO_API_ERROR_PART_ALREADY_EXIST:            return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_CALCGO_API_ERROR_PART_NOT_FOUND:                return RPT_CLIENT_NOT_FOUND;
    case P2ERR_CALCGO_API_ERROR_FIRM_ALREADY_EXIST:            return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_CALCGO_API_ERROR_FIRM_NOT_FOUND:                return RPT_COMMON_ERROR;
    case P2ERR_CALCGO_API_ERROR_SETTLEMENT_ACCOUNT_ALREADY_EXIST:	return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_CALCGO_API_ERROR_FUTPRICE_LESS_2LIM:            return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_CALCGO_API_ERROR_WRONG_POINTSNUM:               return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_CORE_PARTIAL_MASS_DELETION:              return RPT_COMMON_ERROR;
    case P2ERR_CHECK_CONTRABROKER_NOT_FOUND:            return RPT_CONTRABROKER_NOT_FOUND;
    case P2ERR_VOLAT_NOT_FOUND:                         return RPT_COMMON_ERROR;
    case P2ERR_PRICE_ISNT_POSITIVE:                            return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_PROHIBITION_NOT_FOUND:                   return RPT_PROHIBITION_NOT_FOUND;
    case P2ERR_PROHIBITION_ALREADY_EXISTS:              return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_LSCHECKS_FUT_NOT_FOUND:                  return RPT_FUTURES_NOT_FOUND;
    case P2ERR_LSCHECKS_LEG_NOT_FOUND:                  return RPT_COMMON_ERROR;
    case P2ERR_LSCHECKS_TOO_MANY_LEGS:                  return RPT_COMMON_ERROR;
    case P2ERR_LSCHECKS_OPTION_IN_MULTILEG:             return RPT_COMMON_ERROR;
    case P2ERR_LSCHECKS_CLEARING_INSTR_NOT_FOUND:       return RPT_INSTRUMENT_NOT_FOUND;
    case P2ERR_LSCHECKS_UNDERDETERMINED_MULTILEG:       return RPT_COMMON_ERROR;
    case P2ERR_LSCHECKS_WRONG_BASECONTRACT:             return RPT_WRONG_BASECONTRACT; 
    case P2ERR_LSCHECKS_WRONG_BASECONTRACTID:           return RPT_WRONG_BASECONTRACTID;
    case P2ERR_LSCHECKS_BASECONTRACT_NOT_FOUND:         return RPT_BASECONTRACT_NOT_FOUND;
    case P2ERR_LSCHECKS_RF_NOT_FOUND:                           return RPT_DEALER_NOT_FOUND;
    case P2ERR_LSCHECKS_NON_MULTIPLICITY_SWAP_PRICE_OR_RATE:    return RPT_NON_MULTIPLICITY_SWAP_PRICE_OR_RATE;
    case P2ERR_CONSISTENCY_VIRTUAL_FUTURES_NOT_FOUND:           return RPT_FUTURES_NOT_FOUND;
    case P2ERR_NONTRADE_CHK_BASECONTRACT_NOT_FOUND:             return RPT_BASECONTRACT_NOT_FOUND;
    case P2ERR_POSITION_CALCULATOR_INSTR_NOT_FOUND:             return RPT_INSTRUMENT_NOT_FOUND;
    case P2ERR_POSITION_CALCULATOR_BASECONTRACT_NOT_FOUND:      return RPT_BASECONTRACT_NOT_FOUND;
    case P2ERR_POSITION_CALCULATOR_FUTURES_NOT_FOUND:           return RPT_FUTURES_NOT_FOUND;
    case P2ERR_PROH_CLIENT_NOT_FOUND:                   return RPT_PROH_CLIENT_NOT_FOUND;
    case P2ERR_FUTURES_IS_MULTILEG:                     return RPT_FUTURES_IS_MULTILEG;
    case P2ERR_TRADING_CLOSED:                          return RPT_TRADING_CLOSED;
    case P2ERR_NONTRADE_WRONG_INPUT_PARAMETERS:         return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_PRE_CLIENT_ORDER_COUNT_RAISED:           return RPT_PRE_CLIENT_ORDER_COUNT_RAISED;
    case P2ERR_CHECK_ADDRESS_ORDER:                     return RPT_CHECK_ADDRESS_ORDER;
    case P2ERR_CHECK_WRONG_DONTCHECKMONEY_MODE:         return RPT_CHECK_WRONG_DONTCHECKMONEY_MODE;
    case P2ERR_CHANGECLIENTMONEY_NOT_ENOUGH_MONEY:      return RPT_CHANGECLIENTMONEY_NOT_ENOUGH_MONEY;
    case P2ERR_CHANGEBROKERVCB_BROKER_NOT_FOUND:        return RPT_CHANGEBROKERVCB_BROKER_NOT_FOUND;
    case P2ERR_CHANGEBROKERVCB_BASECONTRACT_NOT_FOUND:  return RPT_CHANGEBROKERVCB_BASECONTRACT_NOT_FOUND;
    case P2ERR_CHANGEBROKERVCB_OPERATION_NOT_ALLOWED:   return RPT_CHANGEBROKERVCB_OPERATION_NOT_ALLOWED;
    case P2ERR_LSCHECKS_PROHIBITION_NOT_FOUND:          return RPT_PROHIBITION_NOT_FOUND;
    case P2ERR_T0_NOT_ALLOWED:                          return RPT_T0_NOT_ALLOWED;
    case P2ERR_FIRSTPRICE_AND_DELIVERYPRICE_ARE_DIFF:   return RPT_FIRSTPRICE_AND_DELIVERYPRICE_ARE_DIFF;
    case P2ERR_ADDRESS_TRADEMODE_ONLY:                  return RPT_ANONYM_INSTRUMENT_NOT_FOUND;
    case P2ERR_NON_REPO_ISIN:                           return RPT_REPO_FORBIDDEN;
    case P2ERR_CHECK_CLRINSTRUMENT_NOT_FOUND:           return RPT_INSTRUMENT_NOT_FOUND;

    case P2ERR_FP_DIVISION_BY_ZERO:                                        return RPT_COMMON_ERROR;
    case P2ERR_FP_OVERFLOW:                                                return RPT_COMMON_ERROR;
    case P2ERR_ACCESS_VIOLATION:                                           return RPT_COMMON_ERROR;
    case P2ERR_COMMON_SEH_EXCEPTION:                                       return RPT_COMMON_ERROR;
    case P2ERR_ORDER_IS_BEING_MOVED:                                       return RPT_ORDER_IS_BEING_MOVED;
    case P2ERR_ISIN_ABSENT:                                                return RPT_TRADING_CLOSED;
    case P2ERR_TRADEMODEID_AND_GROUPID_SPECIFIED:                          return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;
    case P2ERR_BAD_TRADINGHALT_REGIME:                                     return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_RESERVEFEE_TRADEMODEID_ALREADY_EXIST:                       return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;
    case P2ERR_RESERVEFEE_INSTRUMENTID_ALREADY_EXIST:                      return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;
    case P2ERR_RESERVEFEE_ISIN_ALREADY_EXIST:                              return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;
    case P2ERR_RESERVEFEE_TRADEMODEID_NOT_FOUND:                           return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;
    case P2ERR_CHECK_WRONG_LOGIN_GROUP:                                    return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;
    case P2ERR_CHECK_TRADEMODES_NOT_FOUND:                                 return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;
    case P2ERR_CHECK_GENERAL_TRADER_FORBIDDEN:                             return RPT_CHECK_RF_TRADEMODE_PROHIBITION;
    case P2ERR_CHECK_TRADER_FORBIDDEN:                                     return RPT_CHECK_BF_TRADEMODE_PROHIBITION;
    case P2ERR_CHECK_TOO_MUCH_MONEY:                                       return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;
    case P2ERR_CHECK_LICENSE_NOT_FOUND:                                    return RPT_WRONG_INPUT_PARAMETERS;
    case P2ERR_CHECK_PROHIBITION_NOT_FOUND:                                return RPT_PROHIBITION_NOT_FOUND;
    case P2ERR_CHECK_DEALER_LICENSE_NOT_FOUND:
    case P2ERR_CHECK_DEALER_LICENSE_FOR_OPEN_NOT_FOUND:
    case P2ERR_CHECK_BROKER_LICENSE_NOT_FOUND:
    case P2ERR_CHECK_BROKER_LICENSE_FOR_OPEN_NOT_FOUND:
    case P2ERR_CHECK_STOCK_BROKER_LICENSE_NOT_FOUND:
    case P2ERR_CHECK_STOCK_BROKER_LICENSE_FOR_OPEN_NOT_FOUND:
    case P2ERR_CHECK_DU_LICENSE_NOT_FOUND:
    case P2ERR_CHECK_DU_LICENSE_FOR_OPEN_NOT_FOUND:
    case P2ERR_CORE_LRPCQ_STOPPED:                                         return RPT_COMMON_ERROR;
    case P2ERR_LSCHECKS_COMMIT_REQUIRED:                                   return RPT_COMMON_ERROR;
    case P2ERR_LSCHECKS_CLEAR_REQUIRED:                                    return RPT_COMMON_ERROR;
    case P2ERR_CONSISTENCY_CANNOT_REINSERT_PROHIBITION:                    return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;
    case P2ERR_POSITION_CALCULATOR_COMMON_ERROR:                           return RPT_COMMON_ERROR;
    case P2ERR_DATALOAD_RF_NOT_FOUND:                                      return RPT_COMMON_ERROR;
    case P2ERR_DATALOAD_SETTLEMENT_ACCOUNT_NOT_FOUND:                      return RPT_COMMON_ERROR; // TODO: [avolkov] добавить отдельный RPT код для этой ошибки
    case P2ERR_NONTRADE_CHK_OPERATION_NOT_ALLOWED:                         return RPT_NONTRADE_OPERATION_NOT_ALLOWED;
    case P2ERR_POSITION_CALCULATOR_DIFFERENT_FUTURES_MONEYCOEFF_IN_MULTILEG: return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;
    case P2ERR_POSITION_CALCULATOR_INVESTOR_NOT_FOUND:                     return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;
    case P2ERR_POSITION_CALCULATOR_MULTILEG_ISIN_IN_POSITION:              return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;
    case P2ERR_POSITION_CALCULATOR_ISIN_NOT_FOUND:                         return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;
    case P2ERR_POSITION_CALCULATOR_VIRTFUTURES_NOT_FOUND:                  return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;
    case P2ERR_POSITION_CALCULATOR_TRADEMODEID_NOT_FOUND:                  return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;
    case P2ERR_POSITION_CALCULATOR_BASECONTRACT_ALREADY_EXISTS:            return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;
    case P2ERR_POSITION_CALCULATOR_TRADEMODE_ALREADY_EXISTS:               return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;
    case P2ERR_POSITION_CALCULATOR_VIRTFUTURES_ALREADY_EXISTS:             return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;
    case P2ERR_POSITION_CALCULATOR_FUTURES_ALREADY_EXISTS:                 return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;
    case P2ERR_POSITION_CALCULATOR_OPTION_ALREADY_EXISTS:                  return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;
    case P2ERR_POSITION_CALCULATOR_ISIN_ALREADY_EXISTS_IN_MULTILEG:        return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;
    case P2ERR_POSITION_CALCULATOR_INSTRUMENT_ALREADY_EXISTS:              return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;
    case P2ERR_POSITION_CALCULATOR_POSITION_ALREADY_EXISTS:                return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;
    case P2ERR_POSITION_CALCULATOR_INCORRECT_PRICE_TYPE:                   return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;
    case P2ERR_POSITION_CALCULATOR_PAIR_INSTRUMENTID_EXPECTED:             return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;
    case P2ERR_POSITION_CALCULATOR_MULTILEG_NOT_FOUND:                     return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;
    case P2ERR_POSITION_CALCULATOR_OPTION_NOT_FOUND:                       return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;
    case P2ERR_POSITION_CALCULATOR_ADDRESS_REPO:                           return RPT_WRONG_NONTRADE_INPUT_PARAMETERS;

    // <FOSRV-2379>
    case P2ERR_TRCLPOS_POSITION_NOT_FOUND:                                 return RPT_TRCLPOS_POSITION_NOT_FOUND;
    case P2ERR_TRCLPOS_NOT_ENOUGH_POSITION:                                return RPT_TRCLPOS_NOT_ENOUGH_POSITION;
    case P2ERR_TRCLPOS_INN_MISMATCH:                                       return RPT_TRCLPOS_INN_MISMATCH;
    case P2ERR_TRCLPOS_RF_MISMATCH:                                        return RPT_TRCLPOS_RF_MISMATCH;
    case P2ERR_TRCLPOS_TARGET_IS_ZERO_SECTION:                             return RPT_TRCLPOS_TARGET_IS_ZERO_SECTION;
    case P2ERR_TRCLPOS_BF_ADMIN_OPERINPUT:                                 return RPT_TRCLPOS_BF_ADMIN_OPERINPUT;
    case P2ERR_TRCLPOS_PROHIBITED_INSTRUMENT:                              return RPT_TRCLPOS_PROHIBITED_INSTRUMENT;
    // </FOSRV-2379>

    case P2ERR_CALCGO_API_ERROR_OPTION_NOT_FOUND:                          return RPT_OPTION_NOT_FOUND;

    default:
        if (p2err < P2ERR_CORE_BEGIN)  return p2err;
    }
    return RPT_COMMON_ERROR;
}
