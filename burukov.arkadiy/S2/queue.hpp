#ifndef QUEUE_HPP
#define QUEUE_HPP

#include "list.hpp"

namespace burukov
{
  template< class T >
  class Queue
  {
  public:
    Queue() = default;

    Queue(const Queue &other);
    Queue(Queue &&other) noexcept;

    Queue &operator=(const Queue &other);
    Queue &operator=(Queue &&other) noexcept;

    T &front();
    const T &front() const;

    bool empty() const noexcept;
    size_t size() const noexcept;

    void push(const T &value);
    void push(T &&value);

    template< class... Args >
    void emplace(Args &&...args);

    void pop() noexcept;

    void swap(Queue &other) noexcept;

    void clear() noexcept;

  private:
    List< T > list_;
  };
}

template< class T >
burukov::Queue< T >::Queue(const Queue &other)
  : list_(other.list_)
{}

template< class T >
burukov::Queue< T >::Queue(Queue &&other) noexcept
  : list_(std::move(other.list_))
{}

template< class T >
burukov::Queue< T > &burukov::Queue< T >::operator=(const Queue &other)
{
  if (this != &other)
  {
    Queue temp(other);
    swap(temp);
  }
  return *this;
}

template< class T >
burukov::Queue< T > &burukov::Queue< T >::operator=(Queue &&other) noexcept
{
  if (this != &other)
  {
    Queue temp(std::move(other));
    swap(temp);
  }
  return *this;
}

template< class T >
T &burukov::Queue< T >::front()
{
  return list_.front();
}

template< class T >
const T &burukov::Queue< T >::front() const
{
  return list_.front();
}

template< class T >
bool burukov::Queue< T >::empty() const noexcept
{
  return list_.size() == 0;
}

template< class T >
size_t burukov::Queue< T >::size() const noexcept
{
  return list_.size();
}

template< class T >
void burukov::Queue< T >::push(const T &value)
{
  list_.pushBack(value);
}

template< class T >
void burukov::Queue< T >::push(T &&value)
{
  list_.pushBack(std::move(value));
}

template< class T >
template< class... Args >
void burukov::Queue< T >::emplace(Args &&...args)
{
  list_.emplaceBack(std::forward< Args >(args)...);
}

template< class T >
void burukov::Queue< T >::pop() noexcept
{
  list_.popFront();
}

template< class T >
void burukov::Queue< T >::swap(Queue &other) noexcept
{
  list_.swap(other.list_);
}

template< class T >
void burukov::Queue< T >::clear() noexcept
{
  list_.clear();
}

#endif
