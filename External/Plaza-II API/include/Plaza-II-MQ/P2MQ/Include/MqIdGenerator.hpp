#ifndef _ID_HPP_
#define _ID_HPP_

#include "../../../Plaza-II/P2Utils/Include/MoreStrongIDGen.h"

namespace NS_P2MQ {

inline bool IsGeneratedByP2MQ(WeakID wid)
{
	return ( wid.m_bignum & ((UINT64)1 << 63) ) != 0; 
}

inline UINT32 IDOrder(UINT64 id)
{
	WeakID wid;
	wid.m_bignum = id;
	return wid.m_cnt;
}

class MqIdGenerator		
{
	MoreStrongIDGenerator m_gen;

public:
	MqIdGenerator() { }

	UINT64 generate()
	{
		WeakID id;
		m_gen.Get(id);
		id.m_bignum |= ((UINT64)1 << 63);
		return id.m_bignum; 
	} 
};

}; 

#endif // _ID_HPP_
