#pragma once

#ifndef BAN_COPY_CONSTRUCTOR
#if (__cplusplus >= 201103L)
#define BAN_COPY_CONSTRUCTOR(class_name) \
    class_name(const class_name& ) = delete;\
    class_name& operator=(const class_name& ) = delete;
#else
#define BAN_COPY_CONSTRUCTOR(class_name) \
    private:\
    class_name(const class_name& );\
    class_name& operator=(const class_name& );\
    public:
#endif //(__cplusplus >= 201103L)
#endif

// ��������������� ������ ��� ������ ������� CGate
// � ������ � ������ ������
#define CHECK_PLAZA2_FAIL(f) \
	{ \
		uint32_t res = f; \
		if (res != CG_ERR_OK) \
		{ \
			char errStr[100];	\
			sprintf_s(errStr, 100, "Client gate error: %x.", res);	\
			log_error(errStr); \
		} \
	} 

// ��������������� ������ ��� ������ ������� CGate
// � ������ �������������� � ������ ������
#define WARN_FAIL(f) \
	{ \
		uint32_t res = f; \
		if (res != CG_ERR_OK) \
			log_error("Error: %x", res); \
	} 


