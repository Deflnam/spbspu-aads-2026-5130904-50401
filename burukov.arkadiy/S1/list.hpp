#ifndef LIST_HPP
#define LIST_HPP

#include <cstddef>
#include <utility>
#include <algorithm>
#include <functional>

namespace burukov
{
  template< class T >
  class List;

  namespace detail
  {
    template< class T >
    struct Node
    {
      T val_;
      Node< T > *next_;

      Node() :
        val_(),
        next_(nullptr)
      {
      }

      template< class U >
      Node(U &&value, Node< T > *next = nullptr) :
        val_(std::forward< U >(value)),
        next_(next)
      {
      }
    };
  }

  template< class T >
  class LIter
  {
  public:
    LIter();
    explicit LIter(detail::Node< T > *p);

    T &operator*();
    T *operator->();
    LIter< T > &operator++();
    LIter< T > operator++(int);
    bool operator==(const LIter< T > &rhs) const;
    bool operator!=(const LIter< T > &rhs) const;

    detail::Node< T > *getNode() const;

  private:
    detail::Node< T > *ptr_;

    friend class List< T >;
  };

  template< class T >
  class LCIter
  {
  public:
    LCIter();
    explicit LCIter(const detail::Node< T > *p);

    const T &operator*() const;
    const T *operator->() const;
    LCIter< T > &operator++();
    LCIter< T > operator++(int);
    bool operator==(const LCIter< T > &rhs) const;
    bool operator!=(const LCIter< T > &rhs) const;

  private:
    const detail::Node< T > *ptr_;

    friend class List< T >;
  };

  template< class T >
  class List
  {
  public:
    List();
    List(const List< T > &other);
    List(List< T > &&other);
    ~List();

    List< T > &operator=(const List< T > &other);
    List< T > &operator=(List< T > &&other);

    bool empty() const;
    size_t size() const;

    T &front();
    const T &front() const;

    template< class U >
    void pushFront(U &&val);

    void popFront();

    template< class U >
    LIter< T > insertAfter(LIter< T > pos, U &&val);

    LIter< T > eraseAfter(LIter< T > pos);

    void clear();
    void swap(List< T > &other);

    LIter< T > begin();
    LIter< T > end();
    LCIter< T > cbegin() const;
    LCIter< T > cend() const;

    void splice(LIter< T > pos, List< T > &other);
    void splice(LIter< T > pos, List< T > &other, LIter< T > it);
    void splice(LIter< T > pos, List< T > &other, LIter< T > first, LIter< T > last);

    void sort();
    template< class Compare >
    void sort(Compare comp);

    void merge(List< T > &other);
    template< class Compare >
    void merge(List< T > &other, Compare comp);

    LIter< T > partition(bool (*pred)(const T &));
    template< class Predicate >
    LIter< T > partition(Predicate pred);

  private:
    detail::Node< T > *head_;
    size_t size_;

    detail::Node< T > *getNodeBefore(LIter< T > it);
  };
}

namespace burukov
{
  template< class T >
  LIter< T >::LIter() :
    ptr_(nullptr)
  {
  }

  template< class T >
  LIter< T >::LIter(detail::Node< T > *p) :
    ptr_(p)
  {
  }

  template< class T >
  T &LIter< T >::operator*()
  {
    return ptr_->val_;
  }

  template< class T >
  T *LIter< T >::operator->()
  {
    return &(ptr_->val_);
  }

  template< class T >
  LIter< T > &LIter< T >::operator++()
  {
    ptr_ = ptr_->next_;
    return *this;
  }

  template< class T >
  LIter< T > LIter< T >::operator++(int)
  {
    LIter< T > temp(*this);
    ptr_ = ptr_->next_;
    return temp;
  }

  template< class T >
  bool LIter< T >::operator==(const LIter< T > &rhs) const
  {
    return ptr_ == rhs.ptr_;
  }

  template< class T >
  bool LIter< T >::operator!=(const LIter< T > &rhs) const
  {
    return ptr_ != rhs.ptr_;
  }

  template< class T >
  detail::Node< T > *LIter< T >::getNode() const
  {
    return ptr_;
  }

  template< class T >
  LCIter< T >::LCIter() :
    ptr_(nullptr)
  {
  }

  template< class T >
  LCIter< T >::LCIter(const detail::Node< T > *p) :
    ptr_(p)
  {
  }

  template< class T >
  const T &LCIter< T >::operator*() const
  {
    return ptr_->val_;
  }

  template< class T >
  const T *LCIter< T >::operator->() const
  {
    return &(ptr_->val_);
  }

  template< class T >
  LCIter< T > &LCIter< T >::operator++()
  {
    ptr_ = ptr_->next_;
    return *this;
  }

  template< class T >
  LCIter< T > LCIter< T >::operator++(int)
  {
    LCIter< T > temp(*this);
    ptr_ = ptr_->next_;
    return temp;
  }

  template< class T >
  bool LCIter< T >::operator==(const LCIter< T > &rhs) const
  {
    return ptr_ == rhs.ptr_;
  }

  template< class T >
  bool LCIter< T >::operator!=(const LCIter< T > &rhs) const
  {
    return ptr_ != rhs.ptr_;
  }

  template< class T >
  List< T >::List() :
    head_(nullptr),
    size_(0)
  {
  }

  template< class T >
  List< T >::List(const List< T > &other) :
    head_(nullptr),
    size_(0)
  {
    detail::Node< T > *src = other.head_;
    detail::Node< T > *dst = nullptr;
    while (src)
    {
      detail::Node< T > *created = nullptr;
      try
      {
        created = new detail::Node< T >(src->val_, nullptr);
      }
      catch (...)
      {
        clear();
        throw;
      }
      if (!head_)
      {
        head_ = created;
      }
      else
      {
        dst->next_ = created;
      }
      dst = created;
      ++size_;
      src = src->next_;
    }
  }

  template< class T >
  List< T >::List(List< T > &&other) :
    head_(std::exchange(other.head_, nullptr)),
    size_(std::exchange(other.size_, 0))
  {
  }

  template< class T >
  List< T >::~List()
  {
    clear();
  }

  template< class T >
  List< T > &List< T >::operator=(const List< T > &other)
  {
    if (this != &other)
    {
      List< T > temp(other);
      swap(temp);
    }
    return *this;
  }

  template< class T >
  List< T > &List< T >::operator=(List< T > &&other)
  {
    if (this != &other)
    {
      clear();
      head_ = std::exchange(other.head_, nullptr);
      size_ = std::exchange(other.size_, 0);
    }
    return *this;
  }

  template< class T >
  bool List< T >::empty() const
  {
    return size_ == 0;
  }

  template< class T >
  size_t List< T >::size() const
  {
    return size_;
  }

  template< class T >
  T &List< T >::front()
  {
    return head_->val_;
  }

  template< class T >
  const T &List< T >::front() const
  {
    return head_->val_;
  }

  template< class T >
  template< class U >
  void List< T >::pushFront(U &&val)
  {
    head_ = new detail::Node< T >(std::forward< U >(val), head_);
    ++size_;
  }

  template< class T >
  void List< T >::popFront()
  {
    detail::Node< T > *old = head_;
    head_ = head_->next_;
    delete old;
    --size_;
  }

  template< class T >
  template< class U >
  LIter< T > List< T >::insertAfter(LIter< T > pos, U &&val)
  {
    detail::Node< T > *created = new detail::Node< T >(std::forward< U >(val), pos.getNode()->next_);
    pos.getNode()->next_ = created;
    ++size_;
    return LIter< T >(created);
  }

  template< class T >
  LIter< T > List< T >::eraseAfter(LIter< T > pos)
  {
    detail::Node< T > *victim = pos.getNode()->next_;
    pos.getNode()->next_ = victim->next_;
    delete victim;
    --size_;
    return LIter< T >(pos.getNode()->next_);
  }

  template< class T >
  void List< T >::clear()
  {
    while (head_)
    {
      detail::Node< T > *tmp = head_;
      head_ = head_->next_;
      delete tmp;
    }
    size_ = 0;
  }

  template< class T >
  void List< T >::swap(List< T > &other)
  {
    std::swap(head_, other.head_);
    std::swap(size_, other.size_);
  }

  template< class T >
  LIter< T > List< T >::begin()
  {
    return LIter< T >(head_);
  }

  template< class T >
  LIter< T > List< T >::end()
  {
    return LIter< T >(nullptr);
  }

  template< class T >
  LCIter< T > List< T >::cbegin() const
  {
    return LCIter< T >(head_);
  }

  template< class T >
  LCIter< T > List< T >::cend() const
  {
    return LCIter< T >(nullptr);
  }

  template< class T >
  detail::Node< T > *List< T >::getNodeBefore(LIter< T > it)
  {
    if (it == begin())
    {
      return nullptr;
    }
    detail::Node< T > *prev = head_;
    while (prev && prev->next_ != it.getNode())
    {
      prev = prev->next_;
    }
    return prev;
  }

  template< class T >
  void List< T >::splice(LIter< T > pos, List< T > &other)
  {
    if (other.empty())
    {
      return;
    }
    if (pos == begin())
    {
      detail::Node< T > *otherTail = other.head_;
      while (otherTail->next_)
      {
        otherTail = otherTail->next_;
      }
      otherTail->next_ = head_;
      head_ = other.head_;
    }
    else
    {
      detail::Node< T > *prev = getNodeBefore(pos);
      detail::Node< T > *otherTail = other.head_;
      while (otherTail->next_)
      {
        otherTail = otherTail->next_;
      }
      otherTail->next_ = prev->next_;
      prev->next_ = other.head_;
    }
    size_ += other.size_;
    other.head_ = nullptr;
    other.size_ = 0;
  }

  template< class T >
  void List< T >::splice(LIter< T > pos, List< T > &other, LIter< T > it)
  {
    if (other.empty())
    {
      return;
    }
    detail::Node< T > *node = it.getNode();
    detail::Node< T > *prev = other.getNodeBefore(it);
    if (prev)
    {
      prev->next_ = node->next_;
    }
    else
    {
      other.head_ = node->next_;
    }
    if (pos == begin())
    {
      node->next_ = head_;
      head_ = node;
    }
    else
    {
      detail::Node< T > *posPrev = getNodeBefore(pos);
      node->next_ = posPrev->next_;
      posPrev->next_ = node;
    }
    ++size_;
    --other.size_;
  }

  template< class T >
  void List< T >::splice(LIter< T > pos, List< T > &other, LIter< T > first, LIter< T > last)
  {
    if (first == last || other.empty())
    {
      return;
    }
    detail::Node< T > *firstNode = first.getNode();
    detail::Node< T > *lastNode = last.getNode();
    detail::Node< T > *prevFirst = other.getNodeBefore(first);
    size_t count = 0;
    for (LIter< T > it = first; it != last; ++it)
    {
      ++count;
    }
    if (prevFirst)
    {
      prevFirst->next_ = lastNode;
    }
    else
    {
      other.head_ = lastNode;
    }
    if (pos == begin())
    {
      detail::Node< T > *tail = firstNode;
      while (tail->next_ != lastNode)
      {
        tail = tail->next_;
      }
      tail->next_ = head_;
      head_ = firstNode;
    }
    else
    {
      detail::Node< T > *posPrev = getNodeBefore(pos);
      detail::Node< T > *tail = firstNode;
      while (tail->next_ != lastNode)
      {
        tail = tail->next_;
      }
      tail->next_ = posPrev->next_;
      posPrev->next_ = firstNode;
    }
    size_ += count;
    other.size_ -= count;
  }

  template< class T >
  void List< T >::merge(List< T > &other)
  {
    if (this == &other || other.empty())
    {
      return;
    }
    detail::Node< T > dummy;
    dummy.next_ = head_;
    detail::Node< T > *tail = &dummy;
    detail::Node< T > *first = head_;
    detail::Node< T > *second = other.head_;
    while (first && second)
    {
      if (first->val_ <= second->val_)
      {
        tail->next_ = first;
        first = first->next_;
      }
      else
      {
        tail->next_ = second;
        second = second->next_;
      }
      tail = tail->next_;
    }
    tail->next_ = first ? first : second;
    head_ = dummy.next_;
    size_ += other.size_;
    other.head_ = nullptr;
    other.size_ = 0;
  }

  template< class T >
  template< class Compare >
  void List< T >::merge(List< T > &other, Compare comp)
  {
    if (this == &other || other.empty())
    {
      return;
    }
    detail::Node< T > dummy;
    dummy.next_ = head_;
    detail::Node< T > *tail = &dummy;
    detail::Node< T > *first = head_;
    detail::Node< T > *second = other.head_;
    while (first && second)
    {
      if (comp(first->val_, second->val_))
      {
        tail->next_ = first;
        first = first->next_;
      }
      else
      {
        tail->next_ = second;
        second = second->next_;
      }
      tail = tail->next_;
    }
    tail->next_ = first ? first : second;
    head_ = dummy.next_;
    size_ += other.size_;
    other.head_ = nullptr;
    other.size_ = 0;
  }

  template< class T >
  void List< T >::sort()
  {
    sort(std::less< T >());
  }

  template< class T >
  template< class Compare >
  void List< T >::sort(Compare comp)
  {
    if (size_ < 2)
    {
      return;
    }

    size_t step = 1;
    detail::Node< T > *result = nullptr;
    detail::Node< T > *tail = nullptr;

    while (step < size_)
    {
      detail::Node< T > *curr = head_;
      result = nullptr;
      tail = nullptr;

      while (curr)
      {
        detail::Node< T > *first = curr;
        size_t firstCount = 0;
        for (size_t i = 0; i < step && curr; ++i)
        {
          curr = curr->next_;
          ++firstCount;
        }

        detail::Node< T > *second = curr;
        size_t secondCount = 0;
        for (size_t i = 0; i < step && curr; ++i)
        {
          curr = curr->next_;
          ++secondCount;
        }

        while (firstCount > 0 || secondCount > 0)
        {
          detail::Node< T > *nextNode = nullptr;
          if (firstCount == 0)
          {
            nextNode = second;
            if (second)
            {
              second = second->next_;
            }
            --secondCount;
          }
          else if (secondCount == 0)
          {
            nextNode = first;
            if (first)
            {
              first = first->next_;
            }
            --firstCount;
          }
          else if (comp(first->val_, second->val_))
          {
            nextNode = first;
            first = first->next_;
            --firstCount;
          }
          else
          {
            nextNode = second;
            second = second->next_;
            --secondCount;
          }

          if (!result)
          {
            result = nextNode;
            tail = nextNode;
          }
          else
          {
            tail->next_ = nextNode;
            tail = nextNode;
          }
        }
      }

      if (tail)
      {
        tail->next_ = nullptr;
      }
      head_ = result;
      step *= 2;
    }
  }

  template< class T >
  LIter< T > List< T >::partition(bool (*pred)(const T &))
  {
    return partition(pred);
  }

  template< class T >
  template< class Predicate >
  LIter< T > List< T >::partition(Predicate pred)
  {
    if (!head_)
    {
      return end();
    }
    List< T > trueList;
    List< T > falseList;
    while (head_)
    {
      detail::Node< T > *node = head_;
      head_ = head_->next_;
      node->next_ = nullptr;
      if (pred(node->val_))
      {
        if (!trueList.head_)
        {
          trueList.head_ = node;
        }
        else
        {
          detail::Node< T > *tail = trueList.head_;
          while (tail->next_)
          {
            tail = tail->next_;
          }
          tail->next_ = node;
        }
        ++trueList.size_;
      }
      else
      {
        if (!falseList.head_)
        {
          falseList.head_ = node;
        }
        else
        {
          detail::Node< T > *tail = falseList.head_;
          while (tail->next_)
          {
            tail = tail->next_;
          }
          tail->next_ = node;
        }
        ++falseList.size_;
      }
    }
    if (!trueList.empty())
    {
      detail::Node< T > *trueTail = trueList.head_;
      while (trueTail->next_)
      {
        trueTail = trueTail->next_;
      }
      trueTail->next_ = falseList.head_;
      head_ = trueList.head_;
      size_ = trueList.size_ + falseList.size_;
      return LIter< T >(trueList.head_);
    }
    head_ = falseList.head_;
    size_ = falseList.size_;
    return LIter< T >(falseList.head_);
  }
}

#endif
