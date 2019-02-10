#ifndef _PUBCACHE_HPP_
#define _PUBCACHE_HPP_

#include "Ring.hpp"

namespace transport { 
	class PubCache 
	{ 
		DISABLE_CLASS_COPY(PubCache); 
		typedef uint32_t EvOffset;
		typedef uint32_t EvSize;
		Ring< std::pair<EvOffset, EvSize> >	 _positions;	// _postions[n] <=> (offset of the n-tn event in _buffer, n-th event size)
		char *const  _buffer; 
		size_t const _max_size;
		size_t 		 _size; 
		seq_type 	 _seq;			// указывает seq, который будет у следующего эвента. Кэш содержит эвенты в интервале [_seq - _positions.count(), _seq)

		void pop_front()
		{ 
			_size -= _positions.first().second;
			_positions.pop_front(); 
		}

		size_t end()
		{ 
			if (_positions.count() == 0)
				return 0;
			std::pair<EvOffset, EvSize> last(_positions[_positions.count() - 1]);
			return (last.first + last.second) % _max_size; 
		}

		public: 

		PubCache(size_t max_count, size_t max_size, seq_type seq = 1): 
			_positions(max_count), 
			_buffer(new char[max_size]), 
			_max_size(max_size), 
			_size(0),
		    _seq(seq)	
		{
		} 

		~PubCache() 
		{ 
			delete [] _buffer; 
		} 

		void insert(const char* event, size_t evsize); 
		size_t get(seq_type seq, size_t max_count, size_t max_size, const char** data_1, size_t* size_1, const char** data_2, size_t* size_2);

		size_t count() const    
		{ 
			return _positions.count();
		} 

		size_t max_size() const 
		{
			return _max_size; 
		}

		seq_type seq()
		{
			return _seq; 
		} 

		bool contains(seq_type seq, size_t count)
		{
			return seq >= _seq - _positions.count() && seq + count <= _seq; 
		} 
	}; 




} // namespace transport 
#endif //_PUBCACHE_HPP_

