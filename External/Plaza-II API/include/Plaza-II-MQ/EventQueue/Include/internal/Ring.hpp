#ifndef _RING_HPP_
#define _RING_HPP_

namespace transport {
	template<typename T, typename size_type = size_t>
		class Ring 
		{
			DISABLE_CLASS_COPY(Ring);
			T *const 	 _buffer;
			size_t const _max_size;
			size_t 		 _begin; 
			size_t 		 _count; 
			public: 

			Ring(size_t max_size) : _buffer(new T[max_size]), _max_size(max_size), _begin(0), _count(0) {} 

			~Ring() 
			{ delete [] _buffer; }

			size_t count() const
			{ return _count; }

			size_t max_count() const
			{ return _max_size; }

			bool empty() const
			{ return _count == 0; }

			bool full() const
			{ return _count == _max_size; }

			const T& first() const 
			{ 
				assert(count() != 0);
				return _buffer[_begin]; 
			}

			T& first()  
			{ 
				assert(count() != 0);
				return _buffer[_begin]; 
			} 

			T& operator[](size_type idx)
			{ 
				assert(idx <= count());
				if (_begin + idx >= _max_size)
					return _buffer[_begin + idx - _max_size];
				return _buffer[_begin + idx];
			} 

			void push_back(const T& item)
			{
				assert(_count != _max_size);
				(*this)[_count] = item;
				++_count;
			}

			void pop_front()
			{
				assert(_count != 0);
				if (++_begin == _max_size)
					_begin = 0;
				--_count; 
			} 

			void pop_back()
			{
				assert(_count != 0);
				--_count; 
			} 

			void push_front(const T& item) 
			{
				assert(_count != _max_size);
				if (_begin == 0) 
					_begin = _max_size;
				--_begin;
				_buffer[_begin] = item; 
			} 

			void push_back(const T& item, size_type n) 
			{
				assert(_count + n <= _max_size); 
				for (; n != 0; --n)
					push_back(item);
			} 

			void push_front(const T& item, size_type n) 
			{
				assert(_count + n <= _max_size); 
				for (; n != 0; --n)
					push_front(item);
			} 

		}; 

} //namespace transport

#endif // _RING_HPP_
