#ifndef _TYPESAFESWITCH_H_
#define _TYPESAFESWITCH_H_

#ifdef _DEBUG
#define T_CASE_I case 'i': int __i__;
#define T_CASE_U case 'u': int __u__;
#define T_CASE_A case 'a': int __a__;
#define T_CASE_D case 'd': int __d__;
#define T_CASE_S case 's': int __s__;
#define T_CASE_T case 't': int __t__;
#define T_CASE_B case 'b': int __b__;
#define T_CASE_C case 'c': int __c__;
#define T_CASE_W case 'w': int __w__;
#define T_CASE_Z case 'z': int __z__;
#define T_CASE_F case 'f': int __f__;
//#define T_CHECK int *(__t_check__[]) = {&__i__,&__u__,&__a__,&__d__,&__s__,&__t__,&__b__,&__c__};
#if defined(__linux__) || defined (__MACH__)
#define T_CHECK int *(__t_check__[]) = {&__i__,&__u__,&__a__,&__d__,&__s__,&__t__,&__b__,&__c__,&__w__,&__z__,&__f__}; UNUSED(__t_check__);
#else // defined(__linux__) || defined (__MACH__)
#define T_CHECK int *(__t_check__[]) = {&__i__,&__u__,&__a__,&__d__,&__s__,&__t__,&__b__,&__c__,&__w__,&__z__,&__f__}; UNUSED(__t_check__);
#endif // defined(__linux__) || defined (__MACH__)
#else
#define T_CASE_I case 'i':
#define T_CASE_U case 'u':
#define T_CASE_A case 'a':
#define T_CASE_D case 'd':
#define T_CASE_S case 's':
#define T_CASE_T case 't':
#define T_CASE_B case 'b':
#define T_CASE_C case 'c':
#define T_CASE_W case 'w':
#define T_CASE_Z case 'z':
#define T_CASE_F case 'f':
#define T_CHECK
#endif


/*

���� ���� ������������ ��� ��������������� �������� ����, ��� ���� ������ Plaza-2
(������, ��������������� �� ������� ��������� ������)�������������� �� ���� ������ ����.
�.�. ���� ����������� �������� ����� ���, � ��� ���� �������������� ��� ���
�������� �� �� ��� �����, �� ������� ������ �� ����� ����������.


����� �������� ����� ���, ����
1) �������� ��� #define'� T_CASE_����� ��� ������ ����, ���������� ��� ���������,
   ���� ��� ��������, � ���� ��� �������� ������
2) �������� ������ �� ��������������� ���������� � ������ T_CHECK
3) �� ���� switch'�� �� ����� ��������� ������ �������� ��������� �����, ���������������
   ������ ����, ������ ������     case '�����':     ������������     T_CASE_�����

��� ����� switch'� ����� ��� ������� �� ������� switch'��
1) ������
       case '�����':
         ������������ ������
       T_CASE_�����
2) � ����� switch'� ����������� ������ ������ ������ T_CHECK


������:

    switch( � )
    {
    T_CASE_A // case 'a':
      // code
      // code
      break;
    T_CASE_I // case 'i':
      // code
      // code
      break;

    // ..................
    // ..................

    default:
      T_CHECK
      // code
      // code
      break;
    }

*/

#endif //_TYPESAFESWITCH_H_
