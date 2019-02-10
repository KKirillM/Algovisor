// IP2Ini.h: interface.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_IP2INI_H_INCLUDED_)
#define _IP2INI_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../P2Utils/Include/IRc.h"

//////////////////////////////////////////////////////////////////////
// IP2Ini

struct IP2Ini : public IRc
{
protected:
    virtual ~IP2Ini(){};

public:

    //������� �������� "�����"
    //+name      - [IN] ��� "�����"
    //+options   - [IN] ������� �����; �� ������ ������ �������������� �����:
    //             P2INI_OPT_CREATE_IF_ABSENT   - ���� ���������� - ������� ������ ������� "����" � ������ ��� 
    //                                            ����������� � ������� P2ERR_OK
    //                                          - ����� - ������� ������ ������� ������ P2ERR_INI_FILE_NOT_FOUND
    // return    - ��� �������� (P2ERR_OK, ���� �������, ��� ��� ������)
    virtual P2ERR   Open(const char* name, UINT32 options) = 0;

    //������� ������������� ����������� ���������� - ������ "����" �� ������ ������ ��� ���
    // return    - ��� �������� (TRUE - "����" ������, ����� - FALSE)
    virtual BOOL    IsOpened() = 0;

    //������� �������� "�����"
    // return    - ��� �������� (P2ERR_OK, ���� �������, ��� ��� ������)
    virtual P2ERR   Close() = 0;


    //������� ������ �� "�����" �������� ���� int ����� (key) ������ (section), ��� ���������� �������� (defValue) � ������ ������
    //+section   - [IN] ��� ������
    //+key       - [IN] ��� �����
    //+defValue  - [IN] ��������, ������������ � ������ ������
    // return    - ������� ��������
    virtual int     GetInt(const char* section, const char* key, int defValue) = 0;

    //������� ������ �� "�����" ������ (char*) ���������� � ����� (key) ������ (section), ��� ������ (defValue) � ������ ������
    //+pRet      - [OUT] ��������� �� �������� ����� (���������� ����� �������), ���� ������ ���� �������� ������� ������
    //+maxSize   - [IN] ������ ����������� ��������� ������
    //+section   - [IN] ��� ������
    //+key       - [IN] ��� �����
    //+defValue  - [IN] ������, ������������ � ������ ������
    // return    - �������� ����� ������, ���������� � �������� �����
    virtual size_t  GetStr(char* pRet, size_t maxSize, 
        const char* section, const char* key, const char* defValue) = 0;

    //������� ������ �� "�����" ���������� (� ���� ������ char*) ���� ������ (section)
    //+pRet      - [OUT] ��������� �� �������� ����� (���������� ����� �������), ���� ������ ���� �������� ���������� ������
    //+pSize     - [IN/OUT] ��������� �� ������ ����������� ��������� ������; � ������ ��� �������� - ���� �� ������ ���� 
    //             ������� ���������� ����������� ������ ������ ��� ��������� ���������� �������, � ���� ������� ������
    //             ������� ��� ������ P2ERR_COMMON_REAL_SIZE_RETURNED
    //+section   - [IN] ��� ������
    // return    - ��� �������� (P2ERR_OK, ���� �������, ��� ��� ������)
    virtual P2ERR   GetSection(char* pRet, size_t* pSize, const char* section) = 0;


    //������� ���������� � "����" �������� (value) ���� int � ���� (key) ������ (section)
    //+section   - [IN] ��� ������
    //+key       - [IN] ��� �����
    //+value     - [IN] ��������, ������� ���������� ��������
    // return    - ��� �������� (P2ERR_OK, ���� �������, ��� ��� ������)
    virtual P2ERR   WriteInt(const char* section, const char* key, int value) = 0;

    //������� ���������� � "����" ������ (value) ���� char* � ���� (key) ������ (section)
    //+section   - [IN] ��� ������
    //+key       - [IN] ��� �����
    //+value     - [IN] ������, ������� ���������� ��������
    // return    - ��� �������� (P2ERR_OK, ���� �������, ��� ��� ������)
    virtual P2ERR   WriteStr(const char* section, const char* key, const char* value) = 0;

    //������� ���������� � "����" ������ ���������� (value) ������ (section)
    //+section   - [IN] ��� ������
    //+value     - [IN] ������ ���������� ������, ������� ���������� �������� � ���� ������ char*
    // return    - ��� �������� (P2ERR_OK, ���� �������, ��� ��� ������)
    virtual P2ERR   WriteSection(const char* section, const char* value) = 0;

    
    //������� ����������� ��� �����, ����������� � ������ (section) "�����"
    //+section   - [IN] ��� ������
    //+cbFunc    - [IN] ��������� �� ������� �������, ������� ������ ���������� ��� ������� �� ��������� ������
    //+cbParams  - [IN] ��������� �� ���������������� ���������, ������� ������ ������������ � ������� �������
    // return    - ��� �������� (P2ERR_OK, ���� �������, ��� ��� ������)
    virtual P2ERR   GetSectKeys(const char* section, P2INI_ENUMKEYS_CALLBACK cbFunc, void* cbParams) = 0;

    //������� ����������� ��� ������, ����������� � "�����"
    //+cbFunc    - [IN] ��������� �� ������� �������, ������� ������ ���������� ��� ������ �� ��������� ������
    //+cbParams  - [IN] ��������� �� ���������������� ���������, ������� ������ ������������ � ������� �������
    // return    - ��� �������� (P2ERR_OK, ���� �������, ��� ��� ������)
    virtual P2ERR   GetSections(P2INI_ENUMSECTIONS_CALLBACK cbFunc, void* cbParams) = 0;

    //������� ��������� �������� ��� "�����", ���������� � ������� Open
    //+pFileName - [OUT] ��������� �� �������� ����� (���������� ����� �������), ���� ������ ���� �������� ��� "�����"
    //+pBufSize  - [IN/OUT] ��������� �� ������ ����������� ��������� ������; � ������ ��� �������� - ���� �� ������ ���� 
    //             ������� ���������� ����������� ������ ������ ��� ��������� ���������� �������, � ���� ������� ������
    //             ������� ��� ������ P2ERR_COMMON_REAL_SIZE_RETURNED
    // return    - ��� �������� (P2ERR_OK, ���� �������, ��� ��� ������)
    virtual P2ERR   GetFileName(char * pFileName, size_t* pBufSize) const = 0;

};

//////////////////////////////////////////////////////////////////////

#endif // !defined(_IP2INI_H_INCLUDED_)
