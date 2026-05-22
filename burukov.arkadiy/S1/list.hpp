#ifndef LIST_HPP
#define LIST_HPP

#include <cstddef>
#include <utility>
#include <functional>
#include <stdexcept>

namespace burukov
{
  template< class T >
  class List;

  namespace detail
  {
    template< class T >
    struct Node
    {
      T value_;
      Node< T > *next_;

      Node():
        value_(),
        next_(nullptr)
      {}

      template< class U >
      Node(U &&value, Node< T > *next):
        value_(std::forward< U >(value)),
        next_(next)
      {}
    };
  }

  template< class T >
  class LIter
  {
  public:
    LIter();
    explicit LIter(detail::Node< T > *ptr);

    T &operator*();
    T *operator->();

    LIter< T > &operator++();
    LIter< T > operator++(int);

    bool operator==(const LIter< T > &rhs) const;
    bool operator!=(const LIter< T > &rhs) const;

    detail::Node< T > *get() const;

  private:
    detail::Node< T > *ptr_;

    friend class List< T >;
  };

  template< class T >
  class LCIter
  {
  public:
    LCIter();
    explicit LCIter(const detail::Node< T > *ptr);

    const T &operator*() const;
    const T *operator->() const;

    LCIter< T > &operator++();
    LCIter< T > operator++(int);

    bool operator==(const LCIter< T > &rhs) const;
    bool operator!=(const LCIter< T > &rhs) const;

  private:
    const detail::Node< T > *ptr_;
  };

  template< class T >
  class List
  {
  public:
    List();
    List(const List< T > &other);
    List(List< T > &&other) noexcept;
    ~List();

    List< T > &operator=(const List< T > &other);
    List< T > &operator=(List< T > &&other) noexcept;

    bool empty() const;
    size_t size() const;

    T &front();
    const T &front() const;

    template< class U >
    void pushFront(U &&value);

    void popFront();

    template< class U >
    LIter< T > insertAfter(LIter< T > pos, U &&value);

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

    template< class Predicate >
    LIter< T > partition(Predicate pred);

  private:
    detail::Node< T > *head_;
    detail::Node< T > *tail_;
    size_t size_;

    detail::Node< T > *getBefore(LIter< T > it);
  };

  template< class T >
  LIter< T >::LIter():
    ptr_(nullptr)
  {}

  template< class T >
  LIter< T >::LIter(detail::Node< T > *ptr):
    ptr_(ptr)
  {}

  template< class T >
  T &LIter< T >::operator*()
  {
    return ptr_->value_;
  }

  template< class T >
  T *LIter< T >::operator->()
  {
    return &(ptr_->value_);
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
    ++(*this);
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
    return !(*this == rhs);
  }

  template< class T >
  detail::Node< T > *LIter< T >::get() const
  {
    return ptr_;
  }

  template< class T >
  LCIter< T >::LCIter():
    ptr_(nullptr)
  {}

  template< class T >
  LCIter< T >::LCIter(const detail::Node< T > *ptr):
    ptr_(ptr)
  {}

  template< class T >
  const T &LCIter< T >::operator*() const
  {
    return ptr_->value_;
  }

  template< class T >
  const T *LCIter< T >::operator->() const
  {
    return &(ptr_->value_);
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
    ++(*this);
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
    return !(*this == rhs);
  }

  template< class T >
  List< T >::List():
    head_(nullptr),
    tail_(nullptr),
    size_(0)
  {}

  template< class T >
  List< T >::List(const List< T > &other):
    head_(nullptr),
    tail_(nullptr),
    size_(0)
  {
    detail::Node< T > *last = nullptr;

    try
    {
      for (LCIter< T > it = other.cbegin(); it != other.cend(); ++it)
      {
        detail::Node< T > *created =
            new detail::Node< T >(*it, nullptr);

        if (!head_)
        {
          head_ = created;
        }
        else
        {
          last->next_ = created;
        }

        last = created;
        ++size_;
      }

      tail_ = last;
    }
    catch (...)
    {
      clear();
      throw;
    }
  }

  template< class T >
  List< T >::List(List< T > &&other) noexcept:
    head_(std::exchange(other.head_, nullptr)),
    tail_(std::exchange(other.tail_, nullptr)),
    size_(std::exchange(other.size_, 0))
  {}

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
  List< T > &List< T >::operator=(List< T > &&other) noexcept
  {
    if (this != &other)
    {
      clear();

      head_ = std::exchange(other.head_, nullptr);
      tail_ = std::exchange(other.tail_, nullptr);
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
    if (!head_)
    {
      throw std::logic_error("empty list");
    }

    return head_->value_;
  }

  template< class T >
  const T &List< T >::front() const
  {
    if (!head_)
    {
      throw std::logic_error("empty list");
    }

    return head_->value_;
  }

  template< class T >
  template< class U >
  void List< T >::pushFront(U &&value)
  {
    head_ = new detail::Node< T >(
        std::forward< U >(value),
        head_);

    if (!tail_)
    {
      tail_ = head_;
    }

    ++size_;
  }

  template< class T >
  void List< T >::popFront()
  {
    if (!head_)
    {
      return;
    }

    detail::Node< T > *old = head_;
    head_ = head_->next_;

    delete old;
    --size_;

    if (!head_)
    {
      tail_ = nullptr;
    }
  }

  template< class T >
  template< class U >
  LIter< T > List< T >::insertAfter(
      LIter< T > pos,
      U &&value)
  {
    if (pos == end())
    {
      return end();
    }

    detail::Node< T > *created =
        new detail::Node< T >(
            std::forward< U >(value),
            pos.get()->next_);

    pos.get()->next_ = created;

    if (tail_ == pos.get())
    {
      tail_ = created;
    }

    ++size_;

    return LIter< T >(created);
  }

  template< class T >
  LIter< T > List< T >::eraseAfter(LIter< T > pos)
  {
    if (pos == end() || pos.get()->next_ == nullptr)
    {
      return end();
    }

    detail::Node< T > *victim = pos.get()->next_;

    pos.get()->next_ = victim->next_;

    if (victim == tail_)
    {
      tail_ = pos.get();
    }

    delete victim;
    --size_;

    return LIter< T >(pos.get()->next_);
  }

  template< class T >
  void List< T >::clear()
  {
    while (head_)
    {
      popFront();
    }
  }

  template< class T >
  void List< T >::swap(List< T > &other)
  {
    std::swap(head_, other.head_);
    std::swap(tail_, other.tail_);
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
  detail::Node< T > *List< T >::getBefore(LIter< T > it)
  {
    if (it == begin())
    {
      return nullptr;
    }

    detail::Node< T > *current = head_;

    while (current && current->next_ != it.get())
    {
      current = current->next_;
    }

    return current;
  }

  template< class T >
  void List< T >::splice(LIter< T > pos, List< T > &other)
  {
    if (other.empty() || this == &other)
    {
      return;
    }

    if (pos == begin())
    {
      other.tail_->next_ = head_;
      head_ = other.head_;
    }
    else
    {
      detail::Node< T > *prev = getBefore(pos);

      other.tail_->next_ = prev->next_;
      prev->next_ = other.head_;
    }

    if (!tail_)
    {
      tail_ = other.tail_;
    }

    size_ += other.size_;

    other.head_ = nullptr;
    other.tail_ = nullptr;
    other.size_ = 0;
  }

  template< class T >
  void List< T >::splice(LIter< T > pos,List< T > &other,LIter< T > it)
  {
    LIter< T > next = it;
    ++next;

    splice(pos, other, it, next);
  }

  template< class T >
  void List< T >::splice(
      LIter< T > pos,
      List< T > &other,
      LIter< T > first,
      LIter< T > last)
  {
    if (first == last || other.empty())
    {
      return;
    }

    detail::Node< T > *firstNode = first.get();
    detail::Node< T > *lastNode = last.get();

    size_t moved = 1;
    detail::Node< T > *rangeTail = firstNode;
    while (rangeTail->next_ != lastNode)
    {
      rangeTail = rangeTail->next_;
      ++moved;
    }

    if (lastNode == nullptr && moved > 1)
    {
      detail::Node< T > *newTail = firstNode;
      size_t newMoved = 1;
      while (newTail->next_ != rangeTail)
      {
        newTail = newTail->next_;
        ++newMoved;
      }
      rangeTail = newTail;
      moved = newMoved;
      lastNode = rangeTail->next_;
    }

    detail::Node< T > *prevFirst = other.getBefore(first);

    if (prevFirst)
    {
      prevFirst->next_ = lastNode;
    }
    else
    {
      other.head_ = lastNode;
    }

    if (rangeTail == other.tail_)
    {
      other.tail_ = prevFirst;
    }

    other.size_ -= moved;

    if (pos == begin())
    {
      rangeTail->next_ = head_;
      head_ = firstNode;
      if (!tail_)
      {
        tail_ = rangeTail;
      }
    }
    else
    {
      detail::Node< T > *prev = getBefore(pos);
      rangeTail->next_ = prev->next_;
      prev->next_ = firstNode;
      if (prev == tail_)
      {
        tail_ = rangeTail;
      }
    }

    size_ += moved;
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

    for (detail::Node< T > *i = head_; i; i = i->next_)
    {
      for (detail::Node< T > *j = i->next_; j; j = j->next_)
      {
        if (comp(j->value_, i->value_))
        {
          std::swap(i->value_, j->value_);
        }
      }
    }
  }

  template< class T >
  void List< T >::merge(List< T > &other)
  {
    merge(other, std::less< T >());
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
    detail::Node< T > *current = &dummy;

    detail::Node< T > *left = head_;
    detail::Node< T > *right = other.head_;

    while (left && right)
    {
      if (comp(left->value_, right->value_))
      {
        current->next_ = left;
        left = left->next_;
      }
      else
      {
        current->next_ = right;
        right = right->next_;
      }

      current = current->next_;
    }

    current->next_ = left ? left : right;

    head_ = dummy.next_;

    tail_ = head_;

    while (tail_ && tail_->next_)
    {
      tail_ = tail_->next_;
    }

    size_ += other.size_;

    other.head_ = nullptr;
    other.tail_ = nullptr;
    other.size_ = 0;
  }

  template< class T >
  template< class Predicate >LIter< T > List< T >::partition(Predicate pred)
  {
    if (!head_)
    {
      return end();
    }

    detail::Node< T > *trueHead = nullptr;
    detail::Node< T > *trueTail = nullptr;

    detail::Node< T > *falseHead = nullptr;
    detail::Node< T > *falseTail = nullptr;

    detail::Node< T > *current = head_;

    while (current)
    {
      detail::Node< T > *next = current->next_;
      current->next_ = nullptr;

      if (pred(current->value_))
      {
        if (!trueHead)
        {
          trueHead = current;
          trueTail = current;
        }
        else
        {
          trueTail->next_ = current;
          trueTail = current;
        }
      }
      else
      {
        if (!falseHead)
        {
          falseHead = current;
          falseTail = current;
        }
        else
        {
          falseTail->next_ = current;
          falseTail = current;
        }
      }

      current = next;
    }

    if (trueTail)
    {
      trueTail->next_ = falseHead;
      head_ = trueHead;
      tail_ = falseTail ? falseTail : trueTail;
    }
    else
    {
      head_ = falseHead;
      tail_ = falseTail;
    }

    return begin();
  }
}

#endif
