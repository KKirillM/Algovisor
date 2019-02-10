#ifndef _SB_ALLOCATOR_HPP_
#define _SB_ALLOCATOR_HPP_

#include "../../P2Utils/Include/CS.h"

class SBAllocator 
{
	struct Link
	{
		Link* next;
	};

	Link* m_head;
	size_t  const m_blockSz; 
	size_t	const m_bufSz;
	char* 	const m_buf;
	CCS	m_cs;

	void push(void *block)
	{ 
		assert(((char*)block - m_buf) % m_blockSz == 0); 
		Link* l = static_cast<Link*>(block);
		l->next = m_head;
		m_head = l; 
	}

	void* pop()
	{
		assert(m_head != NULL);
		Link* l = m_head;
		m_head = m_head->next;
		return l; 
	}

	public:
	SBAllocator(size_t blockSz, size_t nBlocks):
		m_head(NULL),
		m_blockSz(blockSz),
		m_bufSz(blockSz * nBlocks),
		m_buf(new char[blockSz * nBlocks]) 
	{
		assert(nBlocks >= 1 && blockSz >= sizeof(Link));
		memset(m_buf, 0xCC, m_bufSz);
		size_t n = nBlocks;
		do
		{
			--n;
			push(m_buf + n * blockSz);
		}
		while (n != 0); 
	}

	~SBAllocator()
	{
		delete [] m_buf;
	}

	void* allocate(size_t sz)
	{
		void *ptr;
		m_cs.Lock();
		if (sz != 0 && sz <= m_blockSz && m_head != NULL)
		{
			ptr = pop();
			m_cs.Unlock();
		}
		else
		{
			m_cs.Unlock();
			ptr = malloc(sz);
		}
		return ptr;
	}

	void deallocate(void* ptr)
	{
		if (ptr >= m_buf && ptr < m_buf + m_bufSz)
		{
			m_cs.Lock();
			push(ptr); 
			m_cs.Unlock();
		}
		else
		{
			free(ptr);
		}
	} 
};




#endif // _SB_ALLOCATOR_HPP_
