#ifndef _P2ERR_H_
#define _P2ERR_H_
#include <system_error> 
#include <cassert>
#include "../../../Plaza-II/P2Common/Include/P2Errors.h" 
#include "../../../Plaza-II/P2Utils/Include/stuff.h"  
#include "../../../Plaza-II/P2SysLog/Include/P2SysLog.h"
#include "../../../Plaza-II/P2SysTypes/Include/P2SysTypes.h"


// Класс pubqueue, subqueue создают исключения типа std::system_error и категории p2err_category (в случае p2-ошибки) или std::system_category (ошибка ОС).
// i.e. assert(ex.code().category() == p2err_category() || ex.code().category() == std::system_category());
std::error_category& p2err_category(); 

namespace p2err { 

	inline void trace_exception(const char* log_cat, const char* str, const std::system_error& ex)
	{
		if (ex.code().category() == std::system_category())
		{ 
			P2LogErrorWithCodeEx(P2ERR_COMMON_SYSCALL_FAILED, log_cat, " %s %s; '%r'", str, ex.what(), ex.code().value()); 
		} 
		else if (*ex.what() != '\0') 
		{
			P2LogErrorWithCodeEx(ex.code().value(), log_cat, " %s %s", str, ex.what()); 
		}
	} 

	inline void trace_exception(const char* log_cat, const std::system_error& ex)
	{
		trace_exception(log_cat, "", ex);
	} 

	inline P2ERR code(const std::system_error& ex)
	{
		assert(ex.code().category() == p2err_category() || ex.code().category() == std::system_category());
		if (ex.code().category() == std::system_category())
			return P2ERR_COMMON_SYSCALL_FAILED; 
		return ex.code().value(); 
	} 

	// TODO: enum errc 
	//
} // namespace p2err

#define THROW_IFERR(x)											  						\
	{															  						\
		P2ERR __res__ = (x);									  						\
		if (__res__ != 0)										  						\
			throw std::system_error(__res__, 					  						\
									p2err_category(), 			  						\
									std::string("unexpected error ") + 					\
									std::string(__FILE__) + std::string(":") + 			\
									std::to_string(static_cast<int>(__LINE__)));	  	\
	}







#endif //_P2ERR_H_

