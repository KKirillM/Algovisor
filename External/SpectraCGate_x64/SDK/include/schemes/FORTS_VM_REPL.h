#pragma pack(push, 4)

#ifndef _FORTS_VM_REPL_H_
#define _FORTS_VM_REPL_H_

// Scheme "FORTS_VM_REPL" description


namespace FORTS_VM_REPL
{



    struct fut_vm
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int isin_id; // i4
        signed int sess_id; // i4
        char client_code[8]; // c7
        char vm[11]; // d16.5
        char vm_real[11]; // d16.5
        
    };

    const size_t sizeof_fut_vm = 64;
    const size_t fut_vm_index = 0;


    struct opt_vm
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int isin_id; // i4
        signed int sess_id; // i4
        char client_code[8]; // c7
        char vm[11]; // d16.5
        char vm_real[11]; // d16.5
        
    };

    const size_t sizeof_opt_vm = 64;
    const size_t opt_vm_index = 1;


    struct fut_vm_sa
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int isin_id; // i4
        signed int sess_id; // i4
        char settlement_account[13]; // c12
        char vm[15]; // d26.2
        char vm_real[15]; // d26.2
        
    };

    const size_t sizeof_fut_vm_sa = 76;
    const size_t fut_vm_sa_index = 2;


    struct opt_vm_sa
    {
        signed long long replID; // i8
        signed long long replRev; // i8
        signed long long replAct; // i8
        signed int isin_id; // i4
        signed int sess_id; // i4
        char settlement_account[13]; // c12
        char vm[15]; // d26.2
        char vm_real[15]; // d26.2
        
    };

    const size_t sizeof_opt_vm_sa = 76;
    const size_t opt_vm_sa_index = 3;



}

#endif //_FORTS_VM_REPL_H_

#pragma pack(pop)
