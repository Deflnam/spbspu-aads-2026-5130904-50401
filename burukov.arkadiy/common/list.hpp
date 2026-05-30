#ifndef LIST_HPP
#define LIST_HPP

#include <cstddef>
#include <utility>
#include <functional>
#include <stdexcept>
#include <memory>

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

      Node() : value_(), next_(nullptr) {}

      template< class U >
      Node(U &&value, Node< T > *next = nullptr);

      template< class... Args >
      explicit Node(Args &&...args, Node< T > *next = nullptr);
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

    LIter< T > begin();
    LIter< T > end();
    LCIter< T > cbegin() const;
    LCIter< T > cend() const;

    template< class U >
    void pushFront(U &&value);

    template< class U >
    void pushBack(U &&value);

    template< class... Args >
    void emplaceFront(Args &&...args);

    template< class... Args >
    void emplaceBack(Args &&...args);

    template< class... Args >
    LIter< T > emplaceAfter(LIter< T > pos, Args &&...args);

    void popFront();

    template< class U >
    LIter< T > insertAfter(LIter< T > pos, U &&value);

    LIter< T > eraseAfter(LIter< T > pos);

    void clear();
    void swap(List< T > &other);

    void splice(LIter< T > pos, List< T > &other);
    void splice(LIter< T > pos, List< T > &other, LIter< T > it);
    void splice(LIter< T > pos, List< T > &other,
                LIter< T > first, LIter< T > last);

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
    detail::Node< T > *getNodeAt(size_t index);

    template< class... Args >
    detail::Node< T > *createNode(Args &&...args);

    template< class... Args >
    detail::Node< T > *createNodeWithNext(detail::Node< T > *next, Args &&...args);
  };
}

namespace burukov::detail
{
  template< class T >
  template< class U >
  Node< T >::Node(U &&value, Node< T > *next)
    : value_(std::forward< U >(value)), next_(next)
  {}

  template< class T >
  template< class... Args >
  Node< T >::Node(Args &&...args, Node< T > *next)
    : value_(std::forward< Args >(args)...), next_(next)
  {}
}

namespace burukov
{
  template< class T >
  LIter< T >::LIter() : ptr_(nullptr) {}

  template< class T >
  LIter< T >::LIter(detail::Node< T > *ptr) : ptr_(ptr) {}

  template< class T >
  T &LIter< T >::operator*()
  {
    return ptr_->value_;
  }

  template< class T >
  T *LIter< T >::operator->()
  {
    return std::addressof(ptr_->value_);
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
}

namespace burukov
{
  template< class T >
  LCIter< T >::LCIter() : ptr_(nullptr) {}

  template< class T >
  LCIter< T >::LCIter(const detail::Node< T > *ptr) : ptr_(ptr) {}

  template< class T >
  const T &LCIter< T >::operator*() const
  {
    return ptr_->value_;
  }

  template< class T >
  const T *LCIter< T >::operator->() const
  {
    return std::addressof(ptr_->value_);
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
}

namespace burukov
{
  template< class T >
  List< T >::List() : head_(nullptr), tail_(nullptr), size_(0) {}

  template< class T >
  List< T >::List(const List< T > &other)
    : head_(nullptr), tail_(nullptr), size_(0)
  {
    try
    {
      detail::Node< T > *current = other.head_;
      while (current)
      {
        pushBack(current->value_);
        current = current->next_;
      }
    }
    catch (...)
    {
      clear();
      throw;
    }
  }

  template< class T >
  List< T >::List(List< T > &&other) noexcept
    : head_(std::exchange(other.head_, nullptr)),
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
    if (this != std::addressof(other))
    {
      List< T > temp(other);
      swap(temp);
    }
    return *this;
  }

  template< class T >
  List< T > &List< T >::operator=(List< T > &&other) noexcept
  {
    if (this != std::addressof(other))
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
  template< class... Args >
  detail::Node< T > *List< T >::createNode(Args &&...args)
  {
    return new detail::Node< T >(std::forward< Args >(args)...);
  }

  template< class T >
  template< class... Args >
  detail::Node< T > *List< T >::createNodeWithNext(detail::Node< T > *next, Args &&...args)
  {
    return new detail::Node< T >(std::forward< Args >(args)..., next);
  }

  template< class T >
  template< class U >
  void List< T >::pushFront(U &&value)
  {
    head_ = createNodeWithNext(head_, std::forward< U >(value));
    if (!tail_)
    {
      tail_ = head_;
    }
    ++size_;
  }

  template< class T >
  template< class U >
  void List< T >::pushBack(U &&value)
  {
    detail::Node< T > *created = createNode(std::forward< U >(value));
    if (!head_)
    {
      head_ = created;
      tail_ = created;
    }
    else
    {
      tail_->next_ = created;
      tail_ = created;
    }
    ++size_;
  }

  template< class T >
  template< class... Args >
  void List< T >::emplaceFront(Args &&...args)
  {
    head_ = createNodeWithNext(head_, std::forward< Args >(args)...);
    if (!tail_)
    {
      tail_ = head_;
    }
    ++size_;
  }

  template< class T >
  template< class... Args >
  void List< T >::emplaceBack(Args &&...args)
  {
    detail::Node< T > *created = createNode(std::forward< Args >(args)...);
    if (!head_)
    {
      head_ = created;
      tail_ = created;
    }
    else
    {
      tail_->next_ = created;
      tail_ = created;
    }
    ++size_;
  }

  template< class T >
  template< class... Args >
  LIter< T > List< T >::emplaceAfter(LIter< T > pos, Args &&...args)
  {
    if (pos == end())
    {
      return end();
    }
    detail::Node< T > *created = createNodeWithNext(pos.get()->next_,
                                                    std::forward< Args >(args)...);
    pos.get()->next_ = created;
    if (tail_ == pos.get())
    {
      tail_ = created;
    }
    ++size_;
    return LIter< T >(created);
  }

  template< class T >
  void List< T >::popFront()
  {
    if (!head_)
    {
      return;
    }
    detail::Node< T > *temp = head_;
    head_ = head_->next_;
    delete temp;
    --size_;
    if (!head_)
    {
      tail_ = nullptr;
    }
  }

  template< class T >
  template< class U >
  LIter< T > List< T >::insertAfter(LIter< T > pos, U &&value)
  {
    return emplaceAfter(pos, std::forward< U >(value));
  }

  template< class T >
  LIter< T > List< T >::eraseAfter(LIter< T > pos)
  {
    if (pos == end() || !pos.get()->next_)
    {
      return end();
    }
    detail::Node< T > *victim = pos.get()->next_;
    pos.get()->next_ = victim->next_;
    if (tail_ == victim)
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
      detail::Node< T > *tmp = head_;
      head_ = head_->next_;
      delete tmp;
    }
    tail_ = nullptr;
    size_ = 0;
  }

  template< class T >
  void List< T >::swap(List< T > &other)
  {
    std::swap(head_, other.head_);
    std::swap(tail_, other.tail_);
    std::swap(size_, other.size_);
  }

  template< class T >
  void List< T >::splice(LIter< T > pos, List< T > &other)
  {
    if (other.empty() || this == std::addressof(other))
    {
      return;
    }
    if (empty())
    {
      head_ = other.head_;
      tail_ = other.tail_;
      other.head_ = other.tail_ = nullptr;
      size_ = other.size_;
      other.size_ = 0;
      return;
    }
    if (pos == begin())
    {
      other.tail_->next_ = head_;
      head_ = other.head_;
    }
    else if (pos == end())
    {
      tail_->next_ = other.head_;
      tail_ = other.tail_;
    }
    else
    {
      detail::Node< T > *before = getBefore(pos);
      other.tail_->next_ = before->next_;
      before->next_ = other.head_;
    }
    size_ += other.size_;
    other.head_ = other.tail_ = nullptr;
    other.size_ = 0;
  }

  template< class T >
  void List< T >::splice(LIter< T > pos, List< T > &other, LIter< T > it)
  {
    if (it == other.end() || other.empty())
    {
      return;
    }
    detail::Node< T > *node = it.get();
    detail::Node< T > *prev = nullptr;
    if (other.head_ != node)
    {
      prev = other.head_;
      while (prev && prev->next_ != node)
      {
        prev = prev->next_;
      }
    }
    if (prev)
    {
      prev->next_ = node->next_;
    }
    else
    {
      other.head_ = node->next_;
    }
    if (node == other.tail_)
    {
      other.tail_ = prev;
    }
    other.size_--;
    if (empty())
    {
      head_ = node;
      tail_ = node;
      node->next_ = nullptr;
    }
    else if (pos == begin())
    {
      node->next_ = head_;
      head_ = node;
    }
    else if (pos == end())
    {
      node->next_ = nullptr;
      tail_->next_ = node;
      tail_ = node;
    }
    else
    {
      detail::Node< T > *before = getBefore(pos);
      node->next_ = before->next_;
      before->next_ = node;
    }
    size_++;
  }

  template< class T >
  void List< T >::splice(LIter< T > pos, List< T > &other,
                         LIter< T > first, LIter< T > last)
  {
    if (first == last || other.empty() || this == std::addressof(other))
    {
      return;
    }
    detail::Node< T > *firstNode = first.get();
    detail::Node< T > *lastNode = last.get();
    detail::Node< T > *lastNodePrev = nullptr;
    if (lastNode == nullptr)
    {
      lastNodePrev = other.tail_;
    }
    else
    {
      lastNodePrev = other.head_;
      while (lastNodePrev && lastNodePrev->next_ != lastNode)
      {
        lastNodePrev = lastNodePrev->next_;
      }
    }
    size_t count = 0;
    detail::Node< T > *rangeTail = firstNode;
    while (rangeTail != lastNode)
    {
      rangeTail = rangeTail->next_;
      ++count;
    }
    rangeTail = firstNode;
    for (size_t i = 1; i < count; ++i)
    {
      rangeTail = rangeTail->next_;
    }
    detail::Node< T > *beforeFirst = nullptr;
    if (other.head_ != firstNode)
    {
      beforeFirst = other.head_;
      while (beforeFirst && beforeFirst->next_ != firstNode)
      {
        beforeFirst = beforeFirst->next_;
      }
    }
    detail::Node< T > *afterLast = (lastNode == nullptr) ? nullptr : lastNode;
    if (beforeFirst)
    {
      beforeFirst->next_ = afterLast;
    }
    else
    {
      other.head_ = afterLast;
    }
    if (rangeTail == other.tail_)
    {
      other.tail_ = beforeFirst;
    }
    other.size_ -= count;
    if (empty())
    {
      head_ = firstNode;
      tail_ = rangeTail;
      rangeTail->next_ = nullptr;
    }
    else if (pos == begin())
    {
      rangeTail->next_ = head_;
      head_ = firstNode;
    }
    else if (pos == end())
    {
      rangeTail->next_ = nullptr;
      tail_->next_ = firstNode;
      tail_ = rangeTail;
    }
    else
    {
      detail::Node< T > *beforePos = getBefore(pos);
      rangeTail->next_ = beforePos->next_;
      beforePos->next_ = firstNode;
      if (beforePos == tail_)
      {
        tail_ = rangeTail;
      }
    }
    size_ += count;
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
    if (this == std::addressof(other) || other.empty())
    {
      return;
    }
    detail::Node< T > dummy;
    detail::Node< T > *curr = &dummy;
    detail::Node< T > *left = head_;
    detail::Node< T > *right = other.head_;
    while (left && right)
    {
      if (comp(right->value_, left->value_))
      {
        curr->next_ = right;
        right = right->next_;
      }
      else
      {
        curr->next_ = left;
        left = left->next_;
      }
      curr = curr->next_;
    }
    curr->next_ = left ? left : right;
    head_ = dummy.next_;
    curr = head_;
    while (curr && curr->next_)
    {
      curr = curr->next_;
    }
    tail_ = curr;
    size_ += other.size_;
    other.head_ = other.tail_ = nullptr;
    other.size_ = 0;
  }

  template< class T >
  template< class Predicate >
  LIter< T > List< T >::partition(Predicate pred)
  {
    if (!head_)
    {
      return end();
    }
    detail::Node< T > *trueHead = nullptr;
    detail::Node< T > *trueTail = nullptr;
    detail::Node< T > *falseHead = nullptr;
    detail::Node< T > *falseTail = nullptr;
    detail::Node< T > *curr = head_;
    while (curr)
    {
      detail::Node< T > *next = curr->next_;
      curr->next_ = nullptr;
      if (pred(curr->value_))
      {
        if (!trueHead)
        {
          trueHead = trueTail = curr;
        }
        else
        {
          trueTail = trueTail->next_ = curr;
        }
      }
      else
      {
        if (!falseHead)
        {
          falseHead = falseTail = curr;
        }
        else
        {
          falseTail = falseTail->next_ = curr;
        }
      }
      curr = next;
    }
    if (trueHead)
    {
      trueTail->next_ = falseHead;
      head_ = trueHead;
      tail_ = falseTail ? falseTail : trueTail;
      size_ = 0;
      for (detail::Node< T > *p = head_; p; p = p->next_)
      {
        ++size_;
      }
      return LIter< T >(trueHead);
    }
    head_ = falseHead;
    tail_ = falseTail;
    size_ = 0;
    for (detail::Node< T > *p = head_; p; p = p->next_)
    {
      ++size_;
    }
    return begin();
  }

  template< class T >
  detail::Node< T > *List< T >::getBefore(LIter< T > it)
  {
    if (it == begin())
    {
      return nullptr;
    }
    detail::Node< T > *curr = head_;
    while (curr && curr->next_ != it.get())
    {
      curr = curr->next_;
    }
    return curr;
  }

  template< class T >
  detail::Node< T > *List< T >::getNodeAt(size_t index)
  {
    detail::Node< T > *curr = head_;
    for (size_t i = 0; i < index && curr; ++i)
    {
      curr = curr->next_;
    }
    return curr;
  }
}

#endif
