#ifndef _P2MQSTAT_HPP_
#define _P2MQSTAT_HPP_

#include "P2MQStat.h"

// класс-обёртка для автоматической регистрации/дерегистрации статистики версии модуля.
// В деструкторе очищается статистика, поэтому опасно использовать в качестве глобального объекта.  
#if defined(__linux__) || defined(__MACH__)
class P2RegisterVersionStat 
{
	P2StatVars SVars; 
	// no copy semanticks
	void operator=(const P2RegisterVersionStat&);
	P2RegisterVersionStat(const P2RegisterVersionStat&);
	public:
		template<typename T>  
		// symbol: символ, по которому ищется адрес модуля. 
		// Для exe используется функция main.
		// Для dll функция-конструктор
		P2RegisterVersionStat(T symbol, const char* version) 
		{
			memset(&SVars, 0x0, sizeof(SVars));
			P2VARVER(symbol, version); 
		}
		~P2RegisterVersionStat()
		{
			P2UNREG_VER(); 
		} 
}; 
#endif // defined(__linux__) || defined(__MACH__)

#endif //_P2MQSTAT_HPP_

