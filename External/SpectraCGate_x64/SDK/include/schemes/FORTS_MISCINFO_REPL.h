#pragma pack(push, 4)

#ifndef _FORTS_MISCINFO_REPL_H_
#define _FORTS_MISCINFO_REPL_H_

// Scheme "FORTS_MISCINFO_REPL" description


namespace FORTS_MISCINFO_REPL
{



    struct volat_coeff
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int isin_id; // i4
        char a[10]; // d16.10
        char b[10]; // d16.10
        char c[10]; // d16.10
        char d[10]; // d16.10
        char e[10]; // d16.10
        signed int id; // i4
        char s[10]; // d16.10
        
    };

    const size_t sizeof_volat_coeff = 96;
    const size_t volat_coeff_index = 0;



}

#endif //_FORTS_MISCINFO_REPL_H_

#pragma pack(pop)
