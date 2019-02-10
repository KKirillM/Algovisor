#ifndef _P2MQTRACERT_H_
#define _P2MQTRACERT_H_

/*

  ��� ���������� ����������� ����� ������� �� ������ �������� ����
  Post()-�� ��������� ����:

    Category:   P2MSG_CATEGORY_SYSTEM
    Type:       P2MSG_TYPE_SYSTEM_TRACERT
    To:         <������� ����>.router
    tracert_id: <tracert-id (���������������� ����� �����������)>
    flags:      [������������ ����� �����������]
    ttl:        [opt. TTL]

  � ����� � ������� ���� �� �������� ����� ��������� ��������� ����:

    Category:   P2MSG_CATEGORY_SYSTEM
    Type:       P2MSG_TYPE_SYSTEM_TRACERT_REPLY
    tracert_id: <tracert-id (���������������� ����� �����������)>
    flags:
        P2MQTRACERT_REPLY_FLAG_LAST
            - ���� ��� ��������� ������ (������ � ���������� ���� ��������)
              (�� ����� ��������� ������ ������ ��� ����� ������ ������
               � ����� ������ �����, ���� ��� ��� �� ������� ��������.
               ��� ������ ����� �������� ����� ����, ��� ���-�� �������
               �������� �� ��������� <hop number> ������ � ���������� ����.
               ����� ������ ���������� ���������, ���� ������� TTL.
               � ��������� ������ ������ � ������ LAST ������� �� ����� ������.)
    body:
        <node-info>: �������� ���� ������� P2MQTRACERT_FORMAT_NODE

      ��� (���� ��� ������ ��� ����� ���� P2MQTRACERT_FLAG_ACCUMULATE)

        <accumulated node-info>: <node-info1>[<node-info2>[<node-info3>[...]]]
*/

//////////////////////////////////////////////////////////////////////
// ���� ���������

// �����
// [UINT32]
#define P2MQTRACERT_PROP_FLAGS          "P2MQTRACERT_Flags"
// ���������������� ����� �����������
// [UINT32]
#define P2MQTRACERT_PROP_TRACERT_ID     "P2MQTRACERT_Id"
// TTL
// [UINT32]
#define P2MQTRACERT_PROP_TTL            "P2MQTRACERT_TTL"

//////////////////////////////////////////////////////////////////////
// ����� �����������

// ��������� ����������� (�������� ��� ������)
enum
{
    // ����������� ����������� �� ������ �������, �� � ��������� ��������
    P2MQTRACERT_FLAG_TRACE_BACK         = 0x00000001,
    // ����������� ������ � ��������
    P2MQTRACERT_FLAG_ACCUMULATE         = 0x00000002
};

// ����� ���������� (������). ������������� �� ���������������!
enum
{
    // ������� ���������� ������ �����������, ����������� ���������
    P2MQTRACERT_REPLY_FLAG_LAST         = 0x00020000
};

//////////////////////////////////////////////////////////////////////

// ��������� ������ ���������� ������ ���� (<node-info>)
// <time>,<node name>,<hop number>,<direction>
#define P2MQTRACERT_FORMAT_NODE "tcu4u1"

#define P2MQTRACERT_DEFAULT_TTL     15

#endif // _P2MQTRACERT_H_
