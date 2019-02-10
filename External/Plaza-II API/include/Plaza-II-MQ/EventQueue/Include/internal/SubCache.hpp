#ifndef _SUBCACHE_HPP_
#define _SUBCACHE_HPP_

#include "Ring.hpp"

namespace transport {

	// Кэш для сохранения эвентов. Используется в subscriber'e.
	//
	// Формат кэша
	// Каждому сохраненному эвенту соответствует запись в кольцевом буфере. Эвенты сохраняются в возрастающем непрерывном порядке с шагом seqnum'a +1 начиная с некоторого 
	// начального seqnum'a. Если есть "дырка" в сохранямых эвентах, она заполняется "пустыми" эвентами (=длина эвента 0).
	//
	// Инвариант кэша:
	// (_seq == 0 && _cache.count() == 0) ||
	// 	(	_cache[0] содержит эвент с seqnum'ом _seq  &&
	// 		forall i: 1 <= i < _cache.count() :  _cache[i] содержит пустой эвент или эвент с seqnum'ом _seq + i
	// 	)
	//
	class SubCache 
	{
		DISABLE_CLASS_COPY(SubCache); 
		struct Page 
		{
			// Инвариант: bytes, last >= 0 && bytes, last <= MAX_BUF && bytes == 0 => availabe_bytes() == MAX_BUF)
			// Если bytes == 0, то страница должна быть удалена или быть текущей. 
			static const uint32_t PAGE_SIZE = 2*1024*1024;  // todo: можно перейти на uint16_t уменьшив размер страницы. Это позволит несколько сократить потребление памяти в кольцевом буфере за счёт целых меньших рамеров.
			static const uint32_t MAX_BUF;  				// PAGE_SIZE - offsetof(Page, data) 
			uint32_t bytes; 								// кол-во занятых байтов
			uint32_t last;                                  // [last, MAX_BUF) свободные байты. (i.e. [data + last, data + MAX_BUF) )
			char data[1]; 
			Page(): bytes(0), last(0) {}   
			void* operator new(size_t) { return ::new char[PAGE_SIZE]; } 
			void operator delete(void* page) { ::delete [] static_cast<char*>(page); } 
			uint32_t available_bytes() const { return MAX_BUF - last; }
		}; 


		struct EventBuf
		{
			// Инвариант: _page == NULL || (_size > 0 && [_page->begin(), +size) содержит корректный эвент
			Page* 		_page;
			uint32_t 	_begin;
			uint32_t 	_size;
			uint32_t 	size() const { return _size; } 
			bool     	empty() const { return _page == NULL; }
			char* 	 	begin() { return _page->data + _begin; }
			EventBuf(): _page(NULL), _begin(0), _size(0) {}
		};

		Page* _page;
		Ring<EventBuf, uint64_t> _events;
		uint64_t _seq; 

		void set_event(EventBuf& e, const void* event, size_t event_size_)
		{
			assert(event && event_size_ && event_size_ <= Page::MAX_BUF);
			uint32_t event_size = static_cast<uint32_t>(event_size_);
			if (_page->available_bytes() < event_size) 
			{
				_page = new Page;
			}
			e._page  = _page;
			e._begin = _page->last;
			e._size  = event_size; 
			memcpy(e.begin(), event, event_size);
			_page->bytes += event_size;
			_page->last  += event_size;
		}

		void clear_event(EventBuf& e)
		{
			if (!e._page) return; 
			e._page->bytes -= e.size();
			if (e._page->bytes == 0)
			{
			   	if (e._page != _page) delete e._page; 
				else _page->last = 0;
			}
			e._page = NULL;
			e._begin = 0;
			e._size = 0;
		}

		void pop_empty_events()
		{
			while (!_events.empty() && _events.first().empty())
			{
				_events.pop_front();
				++_seq;
			}

			if (_events.empty()) _seq = 0; 
		}

		public:

		SubCache(size_t max_events) : _page(new Page), _events(max_events), _seq(0) { }

		~SubCache() 
		{ 
			drop(_events.count());
			delete _page;
		}

		template <typename SizeFunction>
		void insert(uint64_t seq, size_t count, const char* first, SizeFunction size_function) 
		{
			assert(count != 0); 

			if (seq + count > _seq + _events.count())
			{
				if (_events.empty()) _seq = seq; 							// В этой ветке обрабатывается пустой буфер также
				size_t expand = static_cast<size_t>(seq + count - (_seq + _events.count()));
				if (expand + _events.count() > _events.max_count())  return;  	// проверка, что буфер может быть расширен
				_events.push_back(EventBuf(), expand);  						// расширяем буфер
			}

			if (seq < _seq)    													
			{
				size_t expand = static_cast<size_t>(_seq - seq); 
				if (expand + _events.count() > _events.max_count())  return;  	// проверка, что буфер может быть расширен. Если пришёл эвент с очень ранним seqnum'ом, 
																				// то он не будет сохранён. By design  
				_events.push_front(EventBuf(), expand);  						// расширяем буфер
				_seq = seq;
			} 
			// сохраняем эвенты 
			try
			{
				for (; count != 0; --count)
				{ 
					EventBuf& e = _events[seq - _seq];
					if (e.empty()) set_event(e, first, size_function(first));
					++seq;
					first += size_function(first);
				}
			}
			catch (const std::bad_alloc&)
			{
				pop_empty_events();  // не смогли сохранить из-за отсутствия памяти
			} 
		} 

		void take(char* data, size_t size, size_t* bytes, size_t* events);

		void drop(uint64_t n);  // todo: uint64_t ? why not size_t

		uint64_t seq() const
		{ 
			return _seq;
		} 
	}; 




} // namespace transport

#endif //_SUBCACHE_HPP_

