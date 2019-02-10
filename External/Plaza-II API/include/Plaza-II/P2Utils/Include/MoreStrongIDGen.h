#ifndef _MORESTRONGIDGEN_H_
#define _MORESTRONGIDGEN_H_

#include "../../P2Utils/Include/WeakMsgID.h"

// high bit                            low bit
// ----------------------------------------
// |   24 bit   |   24 bit   |   16 bit   |
// ----------------------------------------
//    counter        rnd        session

class MoreStrongIDGenerator
{
public:
	MoreStrongIDGenerator() : m_cnt(0), m_session(static_cast< UINT32 >(rdtsc()) & 0xFFFF) { }

	void Get(WeakID& id)
	{
        id.m_bignum = (static_cast< UINT64 >(InterlockedIncrement(reinterpret_cast< long volatile* >(&m_cnt))) << 40) |
            (rdtsc() & 0xFFFFFF) << 16 |
            m_session;
	}

private:
	UINT32 m_cnt;
	UINT32 m_session;
};

#endif // _MORESTRONGIDGEN_H_
