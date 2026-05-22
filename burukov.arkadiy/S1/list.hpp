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
      Node(U &&value, Node< T > *next = nullptr) :
        value_(std::forward< U >(value)),
        next_(next)
      {}
    };
  }

  template< class T >
  class LIter
  {
  public:
    LIter() : ptr_(nullptr) {}
    explicit LIter(detail::Node< T > *ptr) : ptr_(ptr) {}

    T &operator*() { return ptr_->value_; }
    T *operator->() { return std::addressof(ptr_->value_); }

    LIter< T > &operator++()
    {
      ptr_ = ptr_->next_;
      return *this;
    }

    LIter< T > operator++(int)
    {
      LIter< T > temp(*this);
      ++(*this);
      return temp;
    }

    bool operator==(const LIter< T > &rhs) const { return ptr_ == rhs.ptr_; }
    bool operator!=(const LIter< T > &rhs) const { return !(*this == rhs); }

    detail::Node< T > *get() const { return ptr_; }

  private:
    detail::Node< T > *ptr_;
    friend class List< T >;
  };

  template< class T >
  class LCIter
  {
  public:
    LCIter() : ptr_(nullptr) {}
    explicit LCIter(const detail::Node< T > *ptr) : ptr_(ptr) {}

    const T &operator*() const { return ptr_->value_; }
    const T *operator->() const { return std::addressof(ptr_->value_); }

    LCIter< T > &operator++()
    {
      ptr_ = ptr_->next_;
      return *this;
    }

    LCIter< T > operator++(int)
    {
      LCIter< T > temp(*this);
      ++(*this);
      return temp;
    }

    bool operator==(const LCIter< T > &rhs) const { return ptr_ == rhs.ptr_; }
    bool operator!=(const LCIter< T > &rhs) const { return !(*this == rhs); }

  private:
    const detail::Node< T > *ptr_;
  };

  template< class T >
  class List
  {
  public:
    List() : head_(nullptr), tail_(nullptr), size_(0) {}

    List(const List< T > &other) : head_(nullptr), tail_(nullptr), size_(0)
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

    List(List< T > &&other) noexcept :
      head_(std::exchange(other.head_, nullptr)),
      tail_(std::exchange(other.tail_, nullptr)),
      size_(std::exchange(other.size_, 0)) {}

    ~List() { clear(); }

    List< T > &operator=(const List< T > &other)
    {
      if (this != std::addressof(other))
      {
        List< T > temp(other);
        swap(temp);
      }
      return *this;
    }

    List< T > &operator=(List< T > &&other) noexcept
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

    bool empty() const { return size_ == 0; }
    size_t size() const { return size_; }

    T &front()
    {
      if (!head_) throw std::logic_error("empty list");
      return head_->value_;
    }

    const T &front() const
    {
      if (!head_) throw std::logic_error("empty list");
      return head_->value_;
    }

    LIter< T > begin() { return LIter< T >(head_); }
    LIter< T > end() { return LIter< T >(nullptr); }
    LCIter< T > cbegin() const { return LCIter< T >(head_); }
    LCIter< T > cend() const { return LCIter< T >(nullptr); }

    template< class U >
    void pushFront(U &&value)
    {
      head_ = new detail::Node< T >(std::forward< U >(value), head_);
      if (!tail_) tail_ = head_;
      ++size_;
    }

    template< class U >
    void pushBack(U &&value)
    {
      detail::Node< T > *created = new detail::Node< T >(std::forward< U >(value));
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

    void popFront()
    {
      if (!head_) return;
      detail::Node< T > *temp = head_;
      head_ = head_->next_;
      delete temp;
      --size_;
      if (!head_) tail_ = nullptr;
    }

    template< class U >
    LIter< T > insertAfter(LIter< T > pos, U &&value)
    {
      if (pos == end()) return end();
      detail::Node< T > *created = new detail::Node< T >(std::forward< U >(value), pos.get()->next_);
      pos.get()->next_ = created;
      if (tail_ == pos.get()) tail_ = created;
      ++size_;
      return LIter< T >(created);
    }

    LIter< T > eraseAfter(LIter< T > pos)
    {
      if (pos == end() || !pos.get()->next_) return end();
      detail::Node< T > *victim = pos.get()->next_;
      pos.get()->next_ = victim->next_;
      if (tail_ == victim) tail_ = pos.get();
      delete victim;
      --size_;
      return LIter< T >(pos.get()->next_);
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

    void splice(LIter< T > pos, List< T > &other)
    {
      if (other.empty() || this == std::addressof(other)) return;

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

    void splice(LIter< T > pos, List< T > &other, LIter< T > it)
    {
      if (it == other.end() || other.empty()) return;

      detail::Node< T > *node = it.get();
      detail::Node< T > *prev = nullptr;
      if (other.head_ != node)
      {
        prev = other.head_;
        while (prev && prev->next_ != node) prev = prev->next_;
      }

      if (prev) prev->next_ = node->next_;
      else other.head_ = node->next_;

      if (node == other.tail_) other.tail_ = prev;

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

    void splice(LIter< T > pos, List< T > &other, LIter< T > first, LIter< T > last)
    {
      if (first == last || other.empty() || this == std::addressof(other)) return;

      detail::Node< T > *firstNode = first.get();
      detail::Node< T > *lastNode = last.get();
      detail::Node< T > *rangeTail = firstNode;
      size_t moved = 1;
      while (rangeTail->next_ != lastNode)
      {
        rangeTail = rangeTail->next_;
        ++moved;
      }

      detail::Node< T > *beforeFirst = nullptr;
      if (other.head_ != firstNode)
      {
        beforeFirst = other.head_;
        while (beforeFirst->next_ != firstNode) beforeFirst = beforeFirst->next_;
      }

      detail::Node< T > *afterLast = rangeTail->next_;

      if (beforeFirst) beforeFirst->next_ = afterLast;
      else other.head_ = afterLast;

      if (rangeTail == other.tail_) other.tail_ = beforeFirst;

      other.size_ -= moved;

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
        if (beforePos == tail_) tail_ = rangeTail;
      }

      size_ += moved;
    }

    void sort()
    {
      sort(std::less< T >());
    }

    template< class Compare >
    void sort(Compare comp)
    {
      if (size_ < 2) return;
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

    void merge(List< T > &other)
    {
      merge(other, std::less< T >());
    }

    template< class Compare >
    void merge(List< T > &other, Compare comp)
    {
      if (this == std::addressof(other) || other.empty()) return;

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
      while (curr && curr->next_) curr = curr->next_;
      tail_ = curr;

      size_ += other.size_;
      other.head_ = other.tail_ = nullptr;
      other.size_ = 0;
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
      while (curr)
      {
        detail::Node< T > *next = curr->next_;
        curr->next_ = nullptr;
        if (pred(curr->value_))
        {
          if (!trueHead) trueHead = trueTail = curr;
          else trueTail = trueTail->next_ = curr;
        }
        else
        {
          if (!falseHead) falseHead = falseTail = curr;
          else falseTail = falseTail->next_ = curr;
        }
        curr = next;
      }

      if (trueHead)
      {
        trueTail->next_ = falseHead;
        head_ = trueHead;
        tail_ = falseTail ? falseTail : trueTail;
        size_ = 0;
        for (detail::Node< T > *p = head_; p; p = p->next_) ++size_;
        return LIter< T >(trueHead);
      }

      head_ = falseHead;
      tail_ = falseTail;
      size_ = 0;
      for (detail::Node< T > *p = head_; p; p = p->next_) ++size_;
      return begin();
    }

  private:
    detail::Node< T > *head_;
    detail::Node< T > *tail_;
    size_t size_;

    detail::Node< T > *getBefore(LIter< T > it)
    {
      if (it == begin()) return nullptr;
      detail::Node< T > *curr = head_;
      while (curr && curr->next_ != it.get()) curr = curr->next_;
      return curr;
    }
  };
}

#endif
