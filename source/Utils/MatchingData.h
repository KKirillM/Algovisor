#pragma once
#include "Price.h"
#include "Logger.h"

#pragma warning(push)
#pragma warning(disable: 4715)

#ifndef _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#include <google/dense_hash_map>
#include <google/dense_hash_set>
#undef _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#else
#include <google/dense_hash_map>
#include <google/dense_hash_set>
#endif

#pragma warning(pop)

#include <stx\btree_map.h>
#include <stx\btree_set.h>
#include "pcharhash.h"


extern "C" void * __cdecl memset(void * const s, int c, size_t n);
extern "C" void * __cdecl memcpy(void * dst, const void * src, size_t count);
extern "C" int    __cdecl memcmp(const void * buf1, const void * buf2, size_t count);
extern "C" char * __cdecl strcpy ( char * destination, const char * source );
#pragma intrinsic( memset, memcpy, memcmp, strcpy )
// про интрисик функции: https://msdn.microsoft.com/ru-ru/library/26td21ds.aspx



template <typename _Key>
struct btree_custom_set_traits
{
    /// If true, the tree will self verify it's invariants after each insert()
    /// or erase(). The header must have been compiled with BTREE_DEBUG defined.
    static const bool   selfverify = false;

    /// If true, the tree will print out debug information and a tree dump
    /// during insert() or erase() operation. The header must have been
    /// compiled with BTREE_DEBUG defined and key_type must be std::ostream
    /// printable.
    static const bool   debug = false;

    /// Number of slots in each leaf of the tree. Estimated so that each node
    /// has a size of about 256 bytes.
    static const int    leafslots = BTREE_MAX( 8, 1024 / (sizeof(_Key)) );

    /// Number of slots in each inner node of the tree. Estimated so that each node
    /// has a size of about 256 bytes.
    static const int    innerslots = BTREE_MAX( 8, 1024 / (sizeof(_Key) + sizeof(void*)) );
};


#if defined(_MSC_VER)
typedef unsigned __int64 t_id;
#elif defined(__GNUC__)
typedef unsigned long long t_id;
#endif

struct SOrder
{
    t_id orderID;
    Price price;
    unsigned int mRemainedQuantity;      // оставшийся размер заявки
	unsigned int mInitialQuantity;       // исходный размер заявки
    int64_t time;

    ~SOrder() {}
   
	void SetInitialQuantity(unsigned int Q)
	{
       mInitialQuantity = mRemainedQuantity =  Q;
	}

	unsigned int GetRemainedQuantity() const
    {
        return mRemainedQuantity;
    }

	int GetReducedQuantity() const
    {
        return mInitialQuantity - mRemainedQuantity;
    }
    
    void reduce(unsigned int volume );
	unsigned int getReducedResult(unsigned int volume ) const;
    void SetReducedResult (unsigned int volume);
    
    static void* free_list;
    static std::vector<void*> newPointerList;
    static int counter;
    void* operator new (size_t size)
    {        
        if (free_list != NULL)                
        {                                    
            void * res = free_list;             
            free_list = *(void **)free_list;  
            memset(res, 0, size);    
            return res;                           
        }    
        ++counter;

        void* ptr = ::operator new (size);
        newPointerList.push_back(ptr);

        return ptr;
    }                                               
    void operator delete (void * addr)
    {        
        if (addr == NULL) return;                       
        *(void **)addr = free_list;                      
        free_list = addr;                        
    }
    static void DeleteAllSOrder()
    {
        for (void* ptr : newPointerList)
        {
            ::operator delete(ptr);
        }
        free_list = NULL;
        newPointerList.clear();
        newPointerList.shrink_to_fit();

        /*while(SOrder::free_list != NULL)
        {
            ptr = *(void **)SOrder::free_list;
            ::operator delete(SOrder::free_list);
            SOrder::free_list = ptr;
        }*/
    }

    friend SOrder* createSOrder(t_id _orderID, const Price& _price, unsigned int _quantity, int64_t _time);

private:
    SOrder(t_id _orderID, const Price& _price, unsigned int _quantity, int64_t _time)
        : orderID(_orderID), price(_price), mInitialQuantity(_quantity), mRemainedQuantity(_quantity), time(_time)
    {
    }

    SOrder()
    {
        memset(this, 0, sizeof(*this));
    }

    SOrder(const SOrder& other)
    {
        memcpy(this, &other, sizeof(*this));
    }
};
//-------------------------------------------------------------------------------
struct BOrder 
{
   BOrder() : pOrder(0) , price(0) , orderID(0) {}
   BOrder(const BOrder& other) : pOrder(other.pOrder), price(other.price), orderID(other.orderID) {}
   BOrder(SOrder * o) : pOrder(o), price(o->price), orderID(o->orderID) {}
   
   SOrder* pOrder;
   Price price;
   t_id orderID;
};

// functor for operator<
class BestBuy 
{	
public:
    inline bool operator()(const BOrder& o1, const BOrder& o2) const
    {	
        return o1.price == o2.price ?	o1.orderID < o2.orderID : o1.price > o2.price;        
    }

    static bool LowerPricesFirst() { return false;}

	static bool PriceComp(const BOrder& o1, const BOrder& o2)
	{
       return o1.price >= o2.price;
	}
 };

  // functor for operator>
 class BestSell 
 {	
public:
    inline bool operator()(const BOrder& o1, const BOrder& o2) const
    {	
        return o1.price == o2.price ? o1.orderID < o2.orderID : o1.price < o2.price;        
    }

    static bool LowerPricesFirst() { return true;}

	static bool PriceComp(const BOrder& o1, const BOrder& o2)
	{
       return o1.price <= o2.price;
	}
};

struct OrderPriceType
{
    Price price;
    unsigned int orders_count;
	unsigned int orders_size;

    OrderPriceType() {}
    OrderPriceType(const Price& _price, const unsigned int _orders_count, const unsigned int _orders_size) :
        price(_price), 
        orders_count(_orders_count), 
        orders_size(_orders_size)
    {
    }
    OrderPriceType(const OrderPriceType& rhs) : 
        price(rhs.price), 
        orders_count(rhs.orders_count), 
        orders_size(rhs.orders_size)
    {
    }

    OrderPriceType(SOrder* o) : 
        price(o->price), 
        orders_count(1), 
        orders_size(o->mInitialQuantity)
    {
    }

    bool operator< (const OrderPriceType& rhs) const 
    {
        return price < rhs.price;
    }
    bool operator== (const OrderPriceType& rhs) const 
    {
        return price == rhs.price;
    }  
};

typedef std::vector<OrderPriceType> PriceSet;
class COrdersSetBase;

struct AggregatedBook
{
    PriceSet bidPriceSet;
    PriceSet askPriceSet;
    Price tradePrice;
    int tradeQuantity;
    int sizeBeforeTrade;            // размер лимита до трейда (т.е. размер трейда плюс размер лимита по которому он ударил)
    int lastOrderTransactionSize;   // сайз последней транзакции (если > 0, то добавили, если < 0, то убрали)
    char agressorSide;              // types of agressor side: B - Buy trade, S - Sell trade, ? - implied trade, N - not defined, a - ask event, b - bid event
    char bidPriceLevelEvent;        // ценовой уровень бидов на котором произошло событие, первый уровень = 1
    char askPriceLevelEvent;
    __int64 timestamp;
    //long long id;

    AggregatedBook() : tradeQuantity(0), agressorSide('N'), timestamp(0), bidPriceLevelEvent(0), askPriceLevelEvent(0), lastOrderTransactionSize(0), sizeBeforeTrade(0)/*, id(0)*/ {}
    
    void setPriceSets(COrdersSetBase* bidSet, COrdersSetBase* askSet, int numberOfPriceLevels, __int64 moment);
    void setTrade(const Price& price, int quantity, int beforeTradeSize = 0)
    {
        tradePrice = price; tradeQuantity = quantity; sizeBeforeTrade = beforeTradeSize;
    }
    void setEventInfo(char bidEventLevel, char askEventLevel, char side, int transactionSize/*, long long transactionID*/)
    {
        bidPriceLevelEvent = bidEventLevel;
        askPriceLevelEvent = askEventLevel;
        agressorSide = side;
        lastOrderTransactionSize = transactionSize;
        //id = transactionID;
    }

    std::string GetBidString() const;
    std::string GetAskString() const;
    std::string GetTradeString() const;
    std::string GetInfoString() const;
};

class COrdersBookBase;

template <class B >
class OrderBookSet : public stx::btree_set<BOrder, B, btree_custom_set_traits<BOrder> >
{
public:
      typedef BOrder OrderType;
      typedef stx::btree_set<BOrder, B, btree_custom_set_traits<BOrder> > BTree;
      typedef typename BTree::iterator                                    iterator;
      typedef typename BTree::const_iterator                              const_iterator;
      typedef typename BTree::reverse_iterator                            reverse_iterator;
      typedef typename BTree::const_reverse_iterator                      const_reverse_iterator;

      OrderBookSet() : ordersBook(0) {}
      void SetOrdersBook( COrdersBookBase* _ordersBook ) { ordersBook = _ordersBook; }

      void Add( SOrder * order )
      {
          this->insert( BOrder(order) );
          InsertOrderIntoIndexes( ordersBook,order );
      }
      void Erase( SOrder * order )
      {
          this->erase(BOrder(order));
      }
      COrdersBookBase* ordersBook;
protected:
      COrdersBookBase* GetOrdersBook (){ return ordersBook;}
      bool PriceComp(const BOrder& o1, const BOrder& o2)
      {
          return B::PriceComp(o1, o2);
      }
private:
      void InsertOrderIntoIndexes( COrdersBookBase* ordersBook, SOrder* order );
};

class COrdersSetBase //<T> : public OrderBookSet<T>
{
public:
    virtual ~COrdersSetBase() {}
    virtual void    Add(SOrder* order) = 0;
    virtual void    Erase(SOrder* order) = 0;
    virtual void    Reduce(int bestCount) = 0;  
    virtual void    Begin() = 0;
    virtual SOrder* GetOrder() = 0;
    virtual void    GetOrdersRange(PriceSet& orderList, int numberOfPriceLevels) = 0;
    virtual void    Next() = 0;
    virtual size_t  OrdersCount() const = 0;
    virtual size_t  OrdersSize() = 0;
    virtual void    Clear() = 0;
    virtual bool    CheckPrice(SOrder* order) = 0;
    virtual bool    Eof() = 0;
    virtual bool    LowerPricesFirst() = 0;
    virtual std::string GetLogString(const int levelsCount = INT_MAX) const = 0;
};

 template <class T>
 class AskBidSetT : public OrderBookSet<T>,  public COrdersSetBase
 {
 public:
     virtual void Add(SOrder * order)
     {
         OrderBookSet<T>::Add(order);
     }

     virtual void Erase(SOrder * order)
     {
         OrderBookSet<T>::Erase(order);
     }

     virtual void Reduce(int bestCount);

     virtual void Begin()
     {
         mDeletionIT = this->begin();
         mIT = this->begin();
     }

     virtual bool Eof()
     {
         return  mIT == this->end();
     }

     virtual void Next()
     {
         ++mIT;
     }

     virtual size_t OrdersCount() const
     {
         return this->size();
     }

     virtual size_t OrdersSize()
     {
         uint64_t size = 0;
         for (typename OrderBookSet<T>::iterator it = this->begin(), end = this->end(); it != end; ++it)
         {
             size += it->pOrder->mRemainedQuantity;
         }
         return static_cast<size_t>(size);
     }

     virtual void Clear()
     {
         this->clear();
     }

     virtual bool CheckPrice(SOrder* order)
     {
         if (mIT == this->end())
             return false;

         return this->PriceComp(*mIT, order);
     }

     virtual SOrder* GetOrder()
     {
         if (mIT == this->end())
             return 0;

         return mIT->pOrder;
     }

     void GetOrdersRange(PriceSet& ordersSet, int numberOfPriceLevels)
     {
         Price price = (mIT != this->end()) ? (mIT->price) : Price();
         unsigned int orders_count = 0;
		 unsigned int orders_size = 0;

         for (typename OrderBookSet<T>::iterator it = mIT, end = this->end(); it != end && numberOfPriceLevels != 0; ++it)
         {
             if (price == it->price)
             {
                 ++orders_count;
                 orders_size += it->pOrder->mRemainedQuantity;
             }
             else
             {
                 ordersSet.push_back(OrderPriceType(price, orders_count, orders_size));
                 price = it->price;
                 orders_count = 1;
                 orders_size = it->pOrder->mRemainedQuantity;
                 --numberOfPriceLevels;
             }
         }

         if (price.IsPrice() && numberOfPriceLevels > 0)
         {
             ordersSet.push_back(OrderPriceType(price, orders_count, orders_size));
         }
     }

     virtual std::string GetLogString(const int levelsCount = INT_MAX) const;

 protected:
     typename OrderBookSet<T>::iterator mIT, mDeletionIT;
     std::vector<t_id> OrdersToDelete;
     bool LowerPricesFirst()
     {
         return T::LowerPricesFirst();
     }
 };

template <class T>
std::string AskBidSetT<T>::GetLogString(const int levelsCount/* = INT_MAX*/) const
{
    char str[128] = {0};
    std::string sString;
    int counter = -1;
	if (this->begin() == this->end())
		return "";

    Price prevPrice(INT_MAX, 0, 0, this->begin()->price._precision);
    if (T::LowerPricesFirst())
    {
        for (auto it = this->begin(), end = this->end(); it != end; ++it)
        {
            if (prevPrice != it->price)
            {
                ++counter;
                if (counter == levelsCount)
                {
                    break;
                }
                prevPrice = it->price;
            }

            sprintf_s(str, sizeof(str), "%s(%d) ", it->price.GetString().c_str(), it->pOrder->mRemainedQuantity);
            sString += str;
        }
    }
    else
    {
        for (auto it = this->begin(), end = this->end(); it != end; ++it)
        {
            if (prevPrice != it->price)
            {
                ++counter;
                if (counter == levelsCount)
                {
                    break;
                }
                prevPrice = it->price;
            }

            sprintf_s(str, sizeof(str), "%s(%d) ", it->price.GetString().c_str(), it->pOrder->mRemainedQuantity);
            sString = str + sString;
        }
    }

    return sString;
}

typedef AskBidSetT<BestSell> AskSet;
typedef AskBidSetT<BestBuy>  BidSet;

template <class Record>
class OrderBookIndex : public stx::btree_set<Record,std::less<Record>, btree_custom_set_traits<Record>>
{
public:
    void Insert(SOrder * order)
    {
        this->insert(Record(order));
    }

    void Erase(const Record ix)
    {
        this->erase(ix);
    }
};

struct OrderIDType
{
    t_id    OrderID;
    SOrder* Order;

    OrderIDType():OrderID(0),Order(0){}
    OrderIDType(const OrderIDType& rhs):OrderID(rhs.OrderID),Order(rhs.Order){}
    OrderIDType(SOrder* o) : OrderID(o->orderID),Order(o){}
    OrderIDType(t_id  OrderID_): OrderID(OrderID_),Order(0){}
 
    bool operator< (const OrderIDType& rhs) const 
    {
        return OrderID<rhs.OrderID;
    }
    bool operator== (const OrderIDType& rhs) const 
    {
        return OrderID==rhs.OrderID;
    }  
};

typedef OrderBookIndex<OrderIDType> AllIdSet;

 class COrdersBookBase
 {
 public:
     COrdersBookBase() {}
     virtual ~COrdersBookBase();

     virtual SOrder* DelOrder( t_id ordNum );
     virtual SOrder* DelBidOrder( t_id ordNum );
     virtual SOrder* DelAskOrder( t_id ordNum );
     virtual void InsertOrderIntoIndexes(SOrder* order);
             void ClearBook();

     virtual COrdersSetBase* GetAskSet() = 0;
     virtual COrdersSetBase* GetBidSet() = 0;

     size_t Size() const { return idSet.size(); }

 private:
     AllIdSet idSet;        // ордера отсортированные по OrderID, этот набор нужен для быстрого поиска нужного ордера 
 };

 template<class T>
 inline void OrderBookSet<T>::InsertOrderIntoIndexes(COrdersBookBase *aOrdersBook, SOrder *order)
 {
     return aOrdersBook->InsertOrderIntoIndexes(order);
 }

 template<class T>
 inline void AskBidSetT<T>::Reduce(int bestCount)
 {
     if (bestCount == 0)
         return;

     COrdersBookBase* aOrdersBook = this->GetOrdersBook();
     /*
       Итератор btree хранит листовой элемент и номер в этом элементе
       Видимо, при удалении может происходить инвалидация итераторов
     */

     OrdersToDelete.clear();
     OrdersToDelete.reserve(bestCount);
     for(int i = 0; i < bestCount; ++i)
     {
         OrdersToDelete.push_back((*mDeletionIT++).orderID);
     }

     for(int i = 0; i < bestCount; ++i)
     {
         SOrder* s = aOrdersBook->DelOrder(OrdersToDelete[i]);
         delete s;
     }
 }

class COrdersBook : public COrdersBookBase
{
public:
    COrdersBook();

	COrdersSetBase*  GetAskSet()
	{
       return static_cast<COrdersSetBase*> (&askSet);
	};
	
	COrdersSetBase*  GetBidSet()
	{
       return static_cast<COrdersSetBase*> (&bidSet);
	};

protected:  
	 AskSet askSet;
     BidSet bidSet;
};

//----------------------------------------------------------------------
class CMatchingData
{ 
public:
    CMatchingData();
    ~CMatchingData(); 

    typedef int InstrumentIDType;

    COrdersBookBase* getOrdersBook(InstrumentIDType InstrumentID);

    void PushOrdersBook(InstrumentIDType InstrumentID, COrdersBookBase* orderBookBase);
    void PopOrdersBook(InstrumentIDType InstrumentID);

    void ClearAllBooks();

    COrdersBookBase* StartEnumOrderBooks()
    {
        m_Iter = m_ordersBookMap.begin();
        if (m_Iter == m_ordersBookMap.end())
            return 0;
        return m_Iter->second.get();
    }

    COrdersBookBase* ContinueEnumOrderBooks()
    {
        ++m_Iter;
        if (m_Iter == m_ordersBookMap.end())
            return 0;
        return m_Iter->second.get();
    }

protected:
    typedef std::unordered_map<InstrumentIDType, std::shared_ptr<COrdersBookBase> > OrderBooksMap;
    typedef std::unordered_map<InstrumentIDType, std::shared_ptr<COrdersBookBase> >::iterator OrderBookMapIter;

    OrderBooksMap              m_ordersBookMap;
    OrderBookMapIter           m_Iter;     
};