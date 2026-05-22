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

      Node() : val_(), next_(nullptr) {}

      template< class U >
      Node(U &&value, Node< T > *next = nullptr) :
        val_(std::forward< U >(value)), next_(next) {}
    };
  }

  template< class T >
  class LIter
  {
  public:
    LIter() : ptr_(nullptr) {}
    explicit LIter(detail::Node< T > *p) : ptr_(p) {}

    T &operator*() { return ptr_->val_; }
    T *operator->() { return &(ptr_->val_); }
    LIter< T > &operator++() { ptr_ = ptr_->next_; return *this; }
    LIter< T > operator++(int) { LIter< T > temp(*this); ptr_ = ptr_->next_; return temp; }
    bool operator==(const LIter< T > &rhs) const { return ptr_ == rhs.ptr_; }
    bool operator!=(const LIter< T > &rhs) const { return ptr_ != rhs.ptr_; }

    detail::Node< T > *getNode() const { return ptr_; }

  private:
    detail::Node< T > *ptr_;
    friend class List< T >;
  };

  template< class T >
  class LCIter
  {
  public:
    LCIter() : ptr_(nullptr) {}
    explicit LCIter(const detail::Node< T > *p) : ptr_(p) {}

    const T &operator*() const { return ptr_->val_; }
    const T *operator->() const { return &(ptr_->val_); }
    LCIter< T > &operator++() { ptr_ = ptr_->next_; return *this; }
    LCIter< T > operator++(int) { LCIter< T > temp(*this); ptr_ = ptr_->next_; return temp; }
    bool operator==(const LCIter< T > &rhs) const { return ptr_ == rhs.ptr_; }
    bool operator!=(const LCIter< T > &rhs) const { return ptr_ != rhs.ptr_; }

  private:
    const detail::Node< T > *ptr_;
    friend class List< T >;
  };

  template< class T >
  class List
  {
  public:
    List() : head_(nullptr), tail_(nullptr), size_(0) {}

    List(const List< T > &other) : head_(nullptr), tail_(nullptr), size_(0)
    {
      detail::Node< T > **curr = &head_;
      for (LIter< T > it = other.begin(); it != other.end(); ++it)
      {
        *curr = new detail::Node< T >(*it, nullptr);
        curr = &((*curr)->next_);
        ++size_;
      }
      tail_ = head_ ? *curr : nullptr;
    }

    List(List< T > &&other) :
      head_(std::exchange(other.head_, nullptr)),
      tail_(std::exchange(other.tail_, nullptr)),
      size_(std::exchange(other.size_, 0)) {}

    ~List() { clear(); }

    List< T > &operator=(const List< T > &other)
    {
      if (this != &other)
      {
        List< T > temp(other);
        swap(temp);
      }
      return *this;
    }

    List< T > &operator=(List< T > &&other)
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

    bool empty() const { return size_ == 0; }
    size_t size() const { return size_; }

    T &front() { return head_->val_; }
    const T &front() const { return head_->val_; }

    template< class U >
    void pushFront(U &&val)
    {
      head_ = new detail::Node< T >(std::forward< U >(val), head_);
      if (!tail_) tail_ = head_;
      ++size_;
    }

    void popFront()
    {
      detail::Node< T > *old = head_;
      head_ = head_->next_;
      if (!head_) tail_ = nullptr;
      delete old;
      --size_;
    }

    template< class U >
    LIter< T > insertAfter(LIter< T > pos, U &&val)
    {
      detail::Node< T > *node = pos.getNode();
      detail::Node< T > *created = new detail::Node< T >(std::forward< U >(val), node->next_);
      node->next_ = created;
      if (node == tail_) tail_ = created;
      ++size_;
      return LIter< T >(created);
    }

    LIter< T > eraseAfter(LIter< T > pos)
    {
      detail::Node< T > *node = pos.getNode();
      detail::Node< T > *victim = node->next_;
      node->next_ = victim->next_;
      if (victim == tail_) tail_ = node;
      delete victim;
      --size_;
      return LIter< T >(node->next_);
    }

    void clear()
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

    void swap(List< T > &other)
    {
      std::swap(head_, other.head_);
      std::swap(tail_, other.tail_);
      std::swap(size_, other.size_);
    }

    LIter< T > begin() { return LIter< T >(head_); }
    LIter< T > end() { return LIter< T >(nullptr); }
    LCIter< T > cbegin() const { return LCIter< T >(head_); }
    LCIter< T > cend() const { return LCIter< T >(nullptr); }

  private:
    detail::Node< T > *head_;
    detail::Node< T > *tail_;
    size_t size_;

    detail::Node< T > *getNodeBefore(LIter< T > it)
    {
      if (it == begin()) return nullptr;
      detail::Node< T > *prev = head_;
      while (prev && prev->next_ != it.getNode()) prev = prev->next_;
      return prev;
    }

  public:
    void splice(LIter< T > pos, List< T > &other)
    {
      if (other.empty()) return;
      if (pos == begin())
      {
        other.tail_->next_ = head_;
        head_ = other.head_;
        if (!tail_) tail_ = other.tail_;
      }
      else
      {
        detail::Node< T > *prev = getNodeBefore(pos);
        other.tail_->next_ = prev->next_;
        prev->next_ = other.head_;
        if (prev == tail_) tail_ = other.tail_;
      }
      size_ += other.size_;
      other.head_ = other.tail_ = nullptr;
      other.size_ = 0;
    }

    void splice(LIter< T > pos, List< T > &other, LIter< T > it)
    {
      if (other.empty()) return;
      detail::Node< T > *node = it.getNode();
      detail::Node< T > *prev = other.getNodeBefore(it);
      if (prev) prev->next_ = node->next_;
      else other.head_ = node->next_;
      if (node == other.tail_) other.tail_ = prev;
      if (pos == begin())
      {
        node->next_ = head_;
        head_ = node;
        if (!tail_) tail_ = node;
      }
      else
      {
        detail::Node< T > *posPrev = getNodeBefore(pos);
        node->next_ = posPrev->next_;
        posPrev->next_ = node;
        if (posPrev == tail_) tail_ = node;
      }
      ++size_;
      --other.size_;
    }

    void splice(LIter< T > pos, List< T > &other, LIter< T > first, LIter< T > last)
    {
      if (first == last || other.empty()) return;

      detail::Node< T > *firstNode = first.getNode();
      detail::Node< T > *lastNode = last.getNode();

      size_t count = 0;
      for (LIter< T > it = first; it != last; ++it) ++count;

      detail::Node< T > *prevFirst = other.getNodeBefore(first);
      detail::Node< T > *rangeTail = firstNode;
      for (size_t i = 0; i < count - 1; ++i) rangeTail = rangeTail->next_;

      if (prevFirst) prevFirst->next_ = lastNode;
      else other.head_ = lastNode;
      if (rangeTail == other.tail_) other.tail_ = prevFirst;

      if (pos == begin())
      {
        rangeTail->next_ = head_;
        head_ = firstNode;
        if (!tail_) tail_ = rangeTail;
      }
      else
      {
        detail::Node< T > *posPrev = getNodeBefore(pos);
        rangeTail->next_ = posPrev->next_;
        posPrev->next_ = firstNode;
        if (posPrev == tail_) tail_ = rangeTail;
      }

      size_ += count;
      other.size_ -= count;
    }

    void merge(List< T > &other)
    {
      merge(other, std::less< T >());
    }

    template< class Compare >
    void merge(List< T > &other, Compare comp)
    {
      if (this == &other || other.empty()) return;
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
      while (tail->next_) tail = tail->next_;
      tail_ = tail;
      size_ += other.size_;
      other.head_ = other.tail_ = nullptr;
      other.size_ = 0;
    }

    void sort()
    {
      sort(std::less< T >());
    }

    template< class Compare >
    void sort(Compare comp)
    {
      if (size_ < 2) return;
      size_t step = 1;
      detail::Node< T > *result = nullptr;
      detail::Node< T > *resultTail = nullptr;
      while (step < size_)
      {
        detail::Node< T > *curr = head_;
        result = nullptr;
        resultTail = nullptr;
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
              if (second) second = second->next_;
              --secondCount;
            }
            else if (secondCount == 0)
            {
              nextNode = first;
              if (first) first = first->next_;
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
              resultTail = nextNode;
            }
            else
            {
              resultTail->next_ = nextNode;
              resultTail = nextNode;
            }
          }
        }
        if (resultTail) resultTail->next_ = nullptr;
        head_ = result;
        tail_ = resultTail;
        step *= 2;
      }
    }

    LIter< T > partition(bool (*pred)(const T &))
    {
      return partition(pred);
    }

    template< class Predicate >
    LIter< T > partition(Predicate pred)
    {
      if (!head_) return end();

      detail::Node< T > *trueHead = nullptr;
      detail::Node< T > *trueTail = nullptr;
      detail::Node< T > *falseHead = nullptr;
      detail::Node< T > *falseTail = nullptr;

      detail::Node< T > *curr = head_;
      size_t trueCount = 0, falseCount = 0;

      while (curr)
      {
        detail::Node< T > *next = curr->next_;
        curr->next_ = nullptr;
        if (pred(curr->val_))
        {
          if (!trueHead) trueHead = trueTail = curr;
          else trueTail = trueTail->next_ = curr;
          ++trueCount;
        }
        else
        {
          if (!falseHead) falseHead = falseTail = curr;
          else falseTail = falseTail->next_ = curr;
          ++falseCount;
        }
        curr = next;
      }

      if (trueHead)
      {
        trueTail->next_ = falseHead;
        head_ = trueHead;
        tail_ = falseTail ? falseTail : trueTail;
        size_ = trueCount + falseCount;
        return LIter< T >(trueHead);
      }

      head_ = falseHead;
      tail_ = falseTail;
      size_ = falseCount;
      return LIter< T >(falseHead);
    }
  };
}

#endif
