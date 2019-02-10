#ifndef _P2MAP_H_
#define _P2MAP_H_

#include <vector>
#include <map>

template <class _Kty, class _Ty, class _Idx = INT32>
class CP2Map
{
public:
    typedef _Kty                         key_type;
    typedef _Ty                          mapped_type;
    typedef std::pair<_Kty, _Ty>         value_type;
    typedef value_type                    type_name;
    
private:
    typedef std::map<_Kty, _Idx>                  index_map_t;
    index_map_t                                     m_index_map;
    typedef typename index_map_t::iterator        index_iterator;
    
    typedef std::vector<_Ty>            value_vec_t;
    value_vec_t                            m_value_vec;
    
    typedef std::vector<_Idx>            free_idx_vec_t;
    free_idx_vec_t                        m_free_idx_vec;
    
public:

    class iterator
    {
        index_iterator  curr;
        value_vec_t&    data_vec;

        typedef std::bidirectional_iterator_tag iterator_category;
        typedef type_name value_type;
        typedef ptrdiff_t difference_type;
        typedef ptrdiff_t distance_type;    // retained
        typedef type_name* pointer;
        typedef type_name& reference;
        
        mutable value_type val;
    public:
        friend class CP2Map<_Kty, _Ty, _Idx>;

        iterator(index_iterator _curr, value_vec_t& _data_vec) 
            : curr (_curr), data_vec (_data_vec) 
        {}
        
        iterator(const iterator& it) 
            : curr (it.curr), data_vec (it.data_vec)
        {}

        iterator& operator++() 
        {
            ++curr;
            return *this;
        }

        iterator operator++(int) 
        {
            iterator tmp(*this); 
            operator++(); 
            return tmp;
        }

        iterator& operator--()
        {    
            --curr;
            return (*this);
        }

        iterator operator--(int)
        {    
            iterator _Tmp = *this;
            --*this;
            return (_Tmp);
        }

        bool operator==(const iterator& rhs) 
        {
            return curr==rhs.curr;
        }

        bool operator!=(const iterator& rhs) 
        {
            return curr!=rhs.curr;
        }

        reference operator*() const
        {
            value_type tmp(curr->first, data_vec[curr->second]);
            
            val.first  = tmp.first;
            val.second = tmp.second;
            return val;
        }

        pointer operator->() const
        {
            return (&**this);
        }
    };

    typedef std::pair<iterator, bool>     _Pairib;

public:

    CP2Map()
    {}
    
    virtual ~CP2Map()
    {}
    
    iterator        find(const key_type& _Keyval)
    {
        return iterator(m_index_map.find(_Keyval), m_value_vec);
    }
    
    iterator         begin()
    {
        return iterator(m_index_map.begin(), m_value_vec);
    }
    
    iterator         end()
    {
        return iterator(m_index_map.end(), m_value_vec);
    }
    
    _Pairib         insert(const value_type& _Val)
    {
        iterator it_end = end();
        iterator it = insert(it_end, _Val);

        return _Pairib(it, it != it_end);
    }

    iterator         insert(iterator _dummy, const value_type& _Val)
    {
        _Idx index;
        index_iterator idx_it = m_index_map.find(_Val.first);

        if( idx_it != m_index_map.end() )
            return end();

        if( m_free_idx_vec.empty() )
        {
            index = static_cast<_Idx>(m_value_vec.size());

            idx_it = m_index_map.insert(m_index_map.end(), index_map_t::value_type(_Val.first, index));
            m_value_vec.push_back(_Val.second);
        }
        else
        {
            index = m_free_idx_vec[m_free_idx_vec.size()-1];
            m_free_idx_vec.pop_back();

            idx_it = m_index_map.insert(m_index_map.end(), index_map_t::value_type(_Val.first, index));
            m_value_vec[index] = _Val.second;
        }

        return iterator(idx_it, m_value_vec);
    }
    
    void             clear()
    {
        m_index_map.clear();
        m_free_idx_vec.clear();
        m_free_idx_vec.resize(m_value_vec.size());
        
        for(size_t i = m_free_idx_vec.size(); i > 0; --i)
        {
            m_free_idx_vec[i] = i;
        }
    }
    
    void             erase(iterator _Where)
    {
        index_iterator it = m_index_map.find(_Where);

        if( it == m_index_map.end() )
            return;

        _Idx index = it->second;

        m_free_idx_vec.push_back(index);
        m_index_map.erase(it);
    }

    size_t             erase(const key_type& _Keyval)
    {
        index_iterator it = m_index_map.find(_Keyval);

        if( it == m_index_map.end() )
            return 0;

        _Idx index = it->second;

        m_free_idx_vec.push_back(index);
        return m_index_map.erase(_Keyval);
    }

    mapped_type&     operator[](const key_type& _Keyval)
    {
        iterator it = find(_Keyval);

        if( it != end() )
        {
            return it->second;
        }
        
        it = insert(it, value_type(_Keyval, mapped_type()));
       
        return it->second;
    }

    void reserve(size_t _Count)
    {
        if( m_index_map.size() != 0 )
            return;

        m_value_vec.resize(_Count);
        m_free_idx_vec.reserve(_Count);

        for(size_t i=_Count; i > 0; --i)
        {
            m_free_idx_vec.push_back(static_cast<_Idx>(i-1));
        }
    }

    size_t capacity()
    {
        return m_value_vec.capacity();
    }
    
    void            extractData(void** pOutData, size_t& szOutDataSize, 
                                void** pOutFreeIndexes, size_t& szOutIndexesSize)
    {
        *pOutData            = NULL;
        szOutDataSize       = m_value_vec.size();
        *pOutFreeIndexes     = NULL;
        szOutIndexesSize    = m_free_idx_vec.size();

        if( szOutDataSize > 0 )
        {
            *pOutData = &m_value_vec[0];
        }

        if( szOutIndexesSize > 0 )
        {
            *pOutFreeIndexes = &m_free_idx_vec[0];
        }
    }
}; 

#endif // _P2MAP_H_