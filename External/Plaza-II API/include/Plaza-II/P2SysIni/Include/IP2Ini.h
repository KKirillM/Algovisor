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

    //Функция открытия "файла"
    //+name      - [IN] имя "файла"
    //+options   - [IN] битовая маска; на данный момент поддерживаются флаги:
    //             P2INI_OPT_CREATE_IF_ABSENT   - если установлен - функция должна создать "файл" в случае его 
    //                                            отстутствия и вернуть P2ERR_OK
    //                                          - иначе - функция должна вернуть ошибку P2ERR_INI_FILE_NOT_FOUND
    // return    - код возврата (P2ERR_OK, если успешно, или код ошибки)
    virtual P2ERR   Open(const char* name, UINT32 options) = 0;

    //Функция предоставляет возможность определить - открыт "файл" на момент вызова или нет
    // return    - код возврата (TRUE - "файл" открыт, иначе - FALSE)
    virtual BOOL    IsOpened() = 0;

    //Функция закрытия "файла"
    // return    - код возврата (P2ERR_OK, если успешно, или код ошибки)
    virtual P2ERR   Close() = 0;


    //Функция читает из "файла" значение типа int ключа (key) секции (section), или возвращает значение (defValue) в случае ошибки
    //+section   - [IN] имя секции
    //+key       - [IN] имя ключа
    //+defValue  - [IN] значение, возвращаемое в случае ошибки
    // return    - искомое значение
    virtual int     GetInt(const char* section, const char* key, int defValue) = 0;

    //Функция читает из "файла" строку (char*) хранящуюся в ключе (key) секции (section), или строку (defValue) в случае ошибки
    //+pRet      - [OUT] указатель на выходной буфер (выделенный перед вызовом), куда должна быть помещена искомая строка
    //+maxSize   - [IN] размер выделенного выходного буфера
    //+section   - [IN] имя секции
    //+key       - [IN] имя ключа
    //+defValue  - [IN] строка, возвращаемая в случае ошибки
    // return    - реальная длина строки, помещенной в выходной буфер
    virtual size_t  GetStr(char* pRet, size_t maxSize, 
        const char* section, const char* key, const char* defValue) = 0;

    //Функция читает из "файла" содержимое (в виде строки char*) всей секции (section)
    //+pRet      - [OUT] указатель на выходной буфер (выделенный перед вызовом), куда должно быть помещено содержимое секции
    //+pSize     - [IN/OUT] указатель на размер выделенного выходного буфера; в случае его нехватки - сюда же должен быть 
    //             помещен минимально необходимый размер буфера для успешного выполнения функции, а сама функция должна
    //             вернуть код ошибки P2ERR_COMMON_REAL_SIZE_RETURNED
    //+section   - [IN] имя секции
    // return    - код возврата (P2ERR_OK, если успешно, или код ошибки)
    virtual P2ERR   GetSection(char* pRet, size_t* pSize, const char* section) = 0;


    //Функция записывает в "файл" значение (value) типа int в ключ (key) секции (section)
    //+section   - [IN] имя секции
    //+key       - [IN] имя ключа
    //+value     - [IN] значение, которое необходимо записать
    // return    - код возврата (P2ERR_OK, если успешно, или код ошибки)
    virtual P2ERR   WriteInt(const char* section, const char* key, int value) = 0;

    //Функция записывает в "файл" строку (value) типа char* в ключ (key) секции (section)
    //+section   - [IN] имя секции
    //+key       - [IN] имя ключа
    //+value     - [IN] строка, которую необходимо записать
    // return    - код возврата (P2ERR_OK, если успешно, или код ошибки)
    virtual P2ERR   WriteStr(const char* section, const char* key, const char* value) = 0;

    //Функция записывает в "файл" полное содержимое (value) секции (section)
    //+section   - [IN] имя секции
    //+value     - [IN] полное содержимое секции, которое необходимо записать в виде строки char*
    // return    - код возврата (P2ERR_OK, если успешно, или код ошибки)
    virtual P2ERR   WriteSection(const char* section, const char* value) = 0;

    
    //Функция перечисляет все ключи, находящиеся в секции (section) "файла"
    //+section   - [IN] имя секции
    //+cbFunc    - [IN] указатель на коллбэк функцию, которая должна вызываться для каждого из найденных ключей
    //+cbParams  - [IN] указатель на пользовательские параметры, который должен передаваться в коллбэк функцию
    // return    - код возврата (P2ERR_OK, если успешно, или код ошибки)
    virtual P2ERR   GetSectKeys(const char* section, P2INI_ENUMKEYS_CALLBACK cbFunc, void* cbParams) = 0;

    //Функция перечисляет все секции, находящиеся в "файле"
    //+cbFunc    - [IN] указатель на коллбэк функцию, которая должна вызываться для каждой из найденных секций
    //+cbParams  - [IN] указатель на пользовательские параметры, который должен передаваться в коллбэк функцию
    // return    - код возврата (P2ERR_OK, если успешно, или код ошибки)
    virtual P2ERR   GetSections(P2INI_ENUMSECTIONS_CALLBACK cbFunc, void* cbParams) = 0;

    //Функция позволяет получить имя "файла", переданное в функцию Open
    //+pFileName - [OUT] указатель на выходной буфер (выделенный перед вызовом), куда должно быть помещено имя "файла"
    //+pBufSize  - [IN/OUT] указатель на размер выделенного выходного буфера; в случае его нехватки - сюда же должен быть 
    //             помещен минимально необходимый размер буфера для успешного выполнения функции, а сама функция должна
    //             вернуть код ошибки P2ERR_COMMON_REAL_SIZE_RETURNED
    // return    - код возврата (P2ERR_OK, если успешно, или код ошибки)
    virtual P2ERR   GetFileName(char * pFileName, size_t* pBufSize) const = 0;

};

//////////////////////////////////////////////////////////////////////

#endif // !defined(_IP2INI_H_INCLUDED_)
