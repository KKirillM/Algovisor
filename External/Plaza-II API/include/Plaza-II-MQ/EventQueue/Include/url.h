#ifndef _UTILS_H_
#define _UTILS_H_ 

#include <cstdint>
#include <cassert>
#include <map>
#include <string>

#ifndef DISABLE_CLASS_COPY
#define DISABLE_CLASS_COPY(TypeName) TypeName(const TypeName&); TypeName& operator=(const TypeName&)
#endif //DISABLE_CLASS_COPY

#ifndef DISABLE_ASSIGNMENT
#define DISABLE_ASSIGNMENT(TypeName) TypeName& operator=(const TypeName&)
#endif // DISABLE_ASSIGNMENT

namespace transport { 
	// url format:
	// <type>:://<host>:<port>;option1=value2;option2=value2
	class url
	{
		std::map<std::string, std::string> _m; 
		public:
		explicit url(const std::string& opt);  // не бросает исключений
		std::string get(const std::string& key) const;
		std::string get(const std::string& key, const std::string& def) const;
		int get_int(const std::string& key) const;
		int get_int(const std::string& key, int def) const;
		uint16_t port() const;
		std::string type() const;
		std::string host() const;
		static std::string type(const std::string& url);
	}; 

	// Читает из url'a адрес мультикастовой группы.
	// Проверяет, что используется адрес из области "Organization-Local Scope" 239.0.0.0 - 239.255.255.255
	// (http://www.iana.org/assignments/multicast-addresses/multicast-addresses.xhtml)
	// Выбрасывается исключение, если адрес группы неправильный.
	uint32_t url_get_multicast_group(const url& u);

	// Читает из url'a ip-адрес. Б
	// Бросается исключение, если ip-адрес неправильно сформирован.
	uint32_t url_get_ip(const url& u, const std::string& name, uint32_t def); 

} //namespace transport

#endif //_UTILS_H_














