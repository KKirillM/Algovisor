#ifndef _ADDR_PARSER_H_
#define _ADDR_PARSER_H_

#include "../../../Plaza-II/P2SysTypes/Include/P2SysTypes.h"
#include "../../../Plaza-II/P2Utils/Include/CS.h"

namespace RouterRoute
{

class RTSAddr
{
public:
	RTSAddr(const char *s, bool resolveNode = true);
	RTSAddr(void);
	~RTSAddr(){}

	inline bool Set1(const char *s, bool resolveNode = true);

	inline const char *addr1(void)const; 
	inline const char *car1(void)const;
	inline const char *cdr1(void)const;

	inline bool operator == (const RTSAddr& adr) const 
	{ return strcmp(adr.m_addr1, m_addr1) == 0; }

	static void SetNodeAddress1(const char *node);
    static void GetNodeAddress(char *node);
private:
	char m_addr1[P2_MAX_POSSIBLE_ADDRESS_SIZE];
	char m_car1[P2_MAX_POSSIBLE_ADDRESS_SIZE];
	char m_cdr1[P2_MAX_POSSIBLE_ADDRESS_SIZE];

    static CCS ms_cs;
	static char ms_node_addr_1[P2_MAX_POSSIBLE_ADDRESS_SIZE];
};
}
#endif // _ADDR_PARSER_H_
