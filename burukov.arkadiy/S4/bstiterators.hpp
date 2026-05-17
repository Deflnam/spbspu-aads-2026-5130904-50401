#ifndef BSTITERATORS_HPP
#define BSTITERATORS_HPP

#include <utility>
#include "treenode.hpp"

namespace burukov
{

template< class Key, class Value, class Compare >
class BSTree;

namespace detail
{

  template< class Key, class Value, bool IsConst >
  class BSTIteratorBase
  {
  public:
    using NodeType = typename std::conditional< IsConst,
                                                 const TreeNode< Key, Value >,
                                                 TreeNode< Key, Value > >::type;

    explicit BSTIteratorBase(NodeType* node = nullptr);

    std::pair< const Key, Value > operator*() const;

    BSTIteratorBase& operator++();
    BSTIteratorBase operator++(int);
    BSTIteratorBase& operator--();
    BSTIteratorBase operator--(int);

    bool operator==(const BSTIteratorBase& other) const;
    bool operator!=(const BSTIteratorBase& other) const;

    NodeType* node_;

  private:
    void goToNext();
    void goToPrev();
  };

}

template< class Key, class Value >
using BSTIterator = detail::BSTIteratorBase< Key, Value, false >;

template< class Key, class Value >
using BSTConstIterator = detail::BSTIteratorBase< Key, Value, true >;

}


template< class Key, class Value, bool IsConst >
burukov::detail::BSTIteratorBase< Key, Value, IsConst >::BSTIteratorBase(NodeType* node):
  node_(node)
{}

template< class Key, class Value, bool IsConst >
std::pair< const Key, Value >
burukov::detail::BSTIteratorBase< Key, Value, IsConst >::operator*() const
{
  return {node_->key_, node_->value_};
}

template< class Key, class Value, bool IsConst >
void burukov::detail::BSTIteratorBase< Key, Value, IsConst >::goToNext()
{
  if (node_->isFake())
  {
    return;
  }

  if (!node_->right_->isFake())
  {
    node_ = node_->right_;
    while (!node_->left_->isFake())
    {
      node_ = node_->left_;
    }
  }
  else
  {
    NodeType* p = node_->parent_;
    while (!p->isFake() && node_ == p->right_)
    {
      node_ = p;
      p = node_->parent_;
    }
    node_ = p;
    if (node_->isFake())
    {
      node_ = nullptr;
    }
  }
}

template< class Key, class Value, bool IsConst >
void burukov::detail::BSTIteratorBase< Key, Value, IsConst >::goToPrev()
{
  if (node_ == nullptr)
  {
    return;
  }

  if (!node_->left_->isFake())
  {
    node_ = node_->left_;
    while (!node_->right_->isFake())
    {
      node_ = node_->right_;
    }
  }
  else
  {
    NodeType* p = node_->parent_;
    while (!p->isFake() && node_ == p->left_)
    {
      node_ = p;
      p = node_->parent_;
    }
    node_ = p;
    if (node_->isFake())
    {
      node_ = nullptr;
    }
  }
}

template< class Key, class Value, bool IsConst >
burukov::detail::BSTIteratorBase< Key, Value, IsConst >&
burukov::detail::BSTIteratorBase< Key, Value, IsConst >::operator++()
{
  goToNext();
  return *this;
}

template< class Key, class Value, bool IsConst >
burukov::detail::BSTIteratorBase< Key, Value, IsConst >
burukov::detail::BSTIteratorBase< Key, Value, IsConst >::operator++(int)
{
  BSTIteratorBase tmp = *this;
  goToNext();
  return tmp;
}

template< class Key, class Value, bool IsConst >
burukov::detail::BSTIteratorBase< Key, Value, IsConst >&
burukov::detail::BSTIteratorBase< Key, Value, IsConst >::operator--()
{
  goToPrev();
  return *this;
}

template< class Key, class Value, bool IsConst >
burukov::detail::BSTIteratorBase< Key, Value, IsConst >
burukov::detail::BSTIteratorBase< Key, Value, IsConst >::operator--(int)
{
  BSTIteratorBase tmp = *this;
  goToPrev();
  return tmp;
}

template< class Key, class Value, bool IsConst >
bool burukov::detail::BSTIteratorBase< Key, Value, IsConst >::operator==(
    const BSTIteratorBase& other) const
{
  return node_ == other.node_;
}

template< class Key, class Value, bool IsConst >
bool burukov::detail::BSTIteratorBase< Key, Value, IsConst >::operator!=(
    const BSTIteratorBase& other) const
{
  return node_ != other.node_;
}

#endif
