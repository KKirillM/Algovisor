#ifndef _P2MQTRACERT_H_
#define _P2MQTRACERT_H_

/*

  ƒл€ проведени€ трассировки нужно послать по адресу целевого узла
  Post()-ом сообщение вида:

    Category:   P2MSG_CATEGORY_SYSTEM
    Type:       P2MSG_TYPE_SYSTEM_TRACERT
    To:         <целевой узел>.router
    tracert_id: <tracert-id (пользовательский номер трассировки)>
    flags:      [опциональные флаги трассировки]
    ttl:        [opt. TTL]

  ¬ ответ с каждого узла по маршруту будут поступать сообщени€ вида:

    Category:   P2MSG_CATEGORY_SYSTEM
    Type:       P2MSG_TYPE_SYSTEM_TRACERT_REPLY
    tracert_id: <tracert-id (пользовательский номер трассировки)>
    flags:
        P2MQTRACERT_REPLY_FLAG_LAST
            - если это последний реплай (реплай с последнего узла маршрута)
              (Ќо после получени€ такого репла€ еще могут прийти реплаи
               с более ранних узлов, если они шли по другому маршруту.
               ¬се реплаи будут получены после того, как кол-во реплаев
               совпадет со значением <hop number> репла€ с последнего узла.
               “акже реплаи перестанут приходить, если истечет TTL.
               ¬ последнем случае реплай с флагом LAST никогда не будет выслан.)
    body:
        <node-info>: бинарный блок формата P2MQTRACERT_FORMAT_NODE

      или (если при старте был задан флаг P2MQTRACERT_FLAG_ACCUMULATE)

        <accumulated node-info>: <node-info1>[<node-info2>[<node-info3>[...]]]
*/

//////////////////////////////////////////////////////////////////////
// ѕол€ сообщений

// ‘лаги
// [UINT32]
#define P2MQTRACERT_PROP_FLAGS          "P2MQTRACERT_Flags"
// ѕользовательский номер тарссировки
// [UINT32]
#define P2MQTRACERT_PROP_TRACERT_ID     "P2MQTRACERT_Id"
// TTL
// [UINT32]
#define P2MQTRACERT_PROP_TTL            "P2MQTRACERT_TTL"

//////////////////////////////////////////////////////////////////////
// ‘лаги трассировки

// ѕараметры трассировки (задаютс€ при старте)
enum
{
    // ѕроизводить трассировку не только пр€мого, но и обратного маршрута
    P2MQTRACERT_FLAG_TRACE_BACK         = 0x00000001,
    // Ќакапливать данные о маршруте
    P2MQTRACERT_FLAG_ACCUMULATE         = 0x00000002
};

// ‘лаги результата (репла€). ѕользователем на устанавливаютс€!
enum
{
    // ѕризнак последнего репла€ трассировки, трассировка завершена
    P2MQTRACERT_REPLY_FLAG_LAST         = 0x00020000
};

//////////////////////////////////////////////////////////////////////

// ‘орматна€ строка информации одного узла (<node-info>)
// <time>,<node name>,<hop number>,<direction>
#define P2MQTRACERT_FORMAT_NODE "tcu4u1"

#define P2MQTRACERT_DEFAULT_TTL     15

#endif // _P2MQTRACERT_H_
