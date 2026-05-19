#ifndef LIST_HPP
#define LIST_HPP

#include <cstddef>
#include <utility>
#include <algorithm>

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

      Node(const T &value, Node< T > *next);
      Node(T &&value, Node< T > *next);
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

    void pushFront(const T &val);
    void pushFront(T &&val);
    void popFront();

    LIter< T > insertAfter(LIter< T > pos, const T &val);
    LIter< T > insertAfter(LIter< T > pos, T &&val);
    LIter< T > eraseAfter(LIter< T > pos);

    void clear();
    void swap(List< T > &other);

    LIter< T > begin();
    LIter< T > end();
    LCIter< T > cbegin() const;
    LCIter< T > cend() const;

  private:
    detail::Node< T > *head_;
    size_t size_;
  };
}
namespace burukov
{
  namespace detail
  {
    template< class T >
    Node< T >::Node(const T &value, Node< T > *next) :
      val_(value),
      next_(next)
    {
    }

    template< class T >
    Node< T >::Node(T &&value, Node< T > *next) :
      val_(std::move(value)),
      next_(next)
    {
    }
  }

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
  void List< T >::pushFront(const T &val)
  {
    head_ = new detail::Node< T >(val, head_);
    ++size_;
  }

  template< class T >
  void List< T >::pushFront(T &&val)
  {
    head_ = new detail::Node< T >(std::move(val), head_);
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
  LIter< T > List< T >::insertAfter(LIter< T > pos, const T &val)
  {
    detail::Node< T > *created = new detail::Node< T >(val, pos.ptr_->next_);
    pos.ptr_->next_ = created;
    ++size_;
    return LIter< T >(created);
  }

  template< class T >
  LIter< T > List< T >::insertAfter(LIter< T > pos, T &&val)
  {
    detail::Node< T > *created = new detail::Node< T >(std::move(val), pos.ptr_->next_);
    pos.ptr_->next_ = created;
    ++size_;
    return LIter< T >(created);
  }

  template< class T >
  LIter< T > List< T >::eraseAfter(LIter< T > pos)
  {
    detail::Node< T > *victim = pos.ptr_->next_;
    pos.ptr_->next_ = victim->next_;
    delete victim;
    --size_;
    return LIter< T >(pos.ptr_->next_);
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
}

#endif
