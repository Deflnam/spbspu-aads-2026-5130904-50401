#ifndef BSTREE_HPP
#define BSTREE_HPP

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>
#include "bstiterators.hpp"

namespace burukov
{
  template< class Key, class Value, class Compare = std::less< Key > >
  class BSTree
  {
  public:
    using iterator = detail::BSTIterator< Key, Value >;

    BSTree();
    BSTree(const BSTree& other);
    BSTree(BSTree&& other) noexcept;
    ~BSTree();

    BSTree& operator=(const BSTree& other);
    BSTree& operator=(BSTree&& other) noexcept;

    bool empty() const { return size_ == 0; }
    size_t size() const { return size_; }
    void swap(BSTree& other) noexcept;
    void clear();

    const Value& at(const Key& k) const;
    Value& at(const Key& k);
    void push(const Key& k, const Value& v);
    void push(Key&& k, Value&& v);
    Value drop(const Key& k);
    bool hasKey(const Key& k) const;

    iterator begin() const;
    iterator end() const { return iterator(nullptr); }

    size_t height() const;
    size_t height(iterator it) const;

    iterator rotateLeft(iterator it);
    iterator rotateRight(iterator it);
    iterator rotateLargeLeft(iterator it);
    iterator rotateLargeRight(iterator it);

  private:
    using Node = detail::TreeNode< Key, Value >;

    Node* root_;
    size_t size_;
    Compare comp_;

    static void initFakeLeaf();
    Node* findNode(const Key& k) const;
    Node* leftmost(Node* node) const;
    Node* cloneTree(Node* src, Node* parent);
    void deleteTree(Node* node);
    size_t getHeight(Node* node) const;
    void transplant(Node* u, Node* v);
  };
}

template< class Key, class Value, class Compare >
void burukov::BSTree< Key, Value, Compare >::initFakeLeaf()
{
  if (Node::fakeLeaf == nullptr)
  {
    Node::fakeLeaf = new Node(Key(), Value(), nullptr);
    Node::fakeLeaf->left_ = Node::fakeLeaf;
    Node::fakeLeaf->right_ = Node::fakeLeaf;
    Node::fakeLeaf->parent_ = Node::fakeLeaf;
  }
}

template< class Key, class Value, class Compare >
burukov::BSTree< Key, Value, Compare >::BSTree() :
  root_(nullptr),
  size_(0),
  comp_()
{
  initFakeLeaf();
  root_ = Node::fakeLeaf;
}

template< class Key, class Value, class Compare >
burukov::BSTree< Key, Value, Compare >::BSTree(const BSTree& other) :
  root_(nullptr),
  size_(0),
  comp_(other.comp_)
{
  initFakeLeaf();
  root_ = cloneTree(other.root_, nullptr);
  size_ = other.size_;
}

template< class Key, class Value, class Compare >
burukov::BSTree< Key, Value, Compare >::BSTree(BSTree&& other) noexcept :
  root_(other.root_),
  size_(other.size_),
  comp_(std::move(other.comp_))
{
  other.root_ = Node::fakeLeaf;
  other.size_ = 0;
}

template< class Key, class Value, class Compare >
burukov::BSTree< Key, Value, Compare >::~BSTree()
{
  clear();
}

template< class Key, class Value, class Compare >
burukov::BSTree< Key, Value, Compare >&
burukov::BSTree< Key, Value, Compare >::operator=(const BSTree& other)
{
  if (this != std::addressof(other))
  {
    BSTree tmp(other);
    swap(tmp);
  }
  return *this;
}

template< class Key, class Value, class Compare >
burukov::BSTree< Key, Value, Compare >&
burukov::BSTree< Key, Value, Compare >::operator=(BSTree&& other) noexcept
{
  if (this != std::addressof(other))
  {
    clear();
    root_ = other.root_;
    size_ = other.size_;
    comp_ = std::move(other.comp_);
    other.root_ = Node::fakeLeaf;
    other.size_ = 0;
  }
  return *this;
}

template< class Key, class Value, class Compare >
void burukov::BSTree< Key, Value, Compare >::swap(BSTree& other) noexcept
{
  std::swap(root_, other.root_);
  std::swap(size_, other.size_);
  std::swap(comp_, other.comp_);
}

template< class Key, class Value, class Compare >
void burukov::BSTree< Key, Value, Compare >::deleteTree(Node* node)
{
  if (node == nullptr || node->isFake())
  {
    return;
  }
  deleteTree(node->left_);
  deleteTree(node->right_);
  delete node;
}

template< class Key, class Value, class Compare >
void burukov::BSTree< Key, Value, Compare >::clear()
{
  deleteTree(root_);
  root_ = Node::fakeLeaf;
  size_ = 0;
}

template< class Key, class Value, class Compare >
burukov::detail::TreeNode< Key, Value >*
burukov::BSTree< Key, Value, Compare >::cloneTree(Node* src, Node* parent)
{
  if (src == nullptr || src->isFake())
  {
    return Node::fakeLeaf;
  }
  Node* n = new Node(src->key_, src->value_, parent);
  n->left_ = cloneTree(src->left_, n);
  n->right_ = cloneTree(src->right_, n);
  return n;
}

template< class Key, class Value, class Compare >
burukov::detail::TreeNode< Key, Value >*
burukov::BSTree< Key, Value, Compare >::findNode(const Key& k) const
{
  Node* cur = root_;
  while (cur != nullptr && !cur->isFake())
  {
    if (comp_(k, cur->key_))
    {
      cur = cur->left_;
    }
    else if (comp_(cur->key_, k))
    {
      cur = cur->right_;
    }
    else
    {
      return cur;
    }
  }
  return nullptr;
}

template< class Key, class Value, class Compare >
bool burukov::BSTree< Key, Value, Compare >::hasKey(const Key& k) const
{
  return findNode(k) != nullptr;
}

template< class Key, class Value, class Compare >
const Value& burukov::BSTree< Key, Value, Compare >::at(const Key& k) const
{
  Node* n = findNode(k);
  if (n == nullptr)
  {
    throw std::out_of_range("key not found");
  }
  return n->value_;
}

template< class Key, class Value, class Compare >
Value& burukov::BSTree< Key, Value, Compare >::at(const Key& k)
{
  Node* n = findNode(k);
  if (n == nullptr)
  {
    throw std::out_of_range("key not found");
  }
  return n->value_;
}

template< class Key, class Value, class Compare >
void burukov::BSTree< Key, Value, Compare >::push(const Key& k, const Value& v)
{
  if (root_->isFake())
  {
    root_ = new Node(k, v, nullptr);
    ++size_;
    return;
  }

  Node* parent = nullptr;
  Node* cur = root_;

  while (cur != nullptr && !cur->isFake())
  {
    parent = cur;
    if (comp_(k, cur->key_))
    {
      cur = cur->left_;
    }
    else if (comp_(cur->key_, k))
    {
      cur = cur->right_;
    }
    else
    {
      cur->value_ = v;
      return;
    }
  }

  Node* n = new Node(k, v, parent);
  if (comp_(k, parent->key_))
  {
    parent->left_ = n;
  }
  else
  {
    parent->right_ = n;
  }
  ++size_;
}

template< class Key, class Value, class Compare >
void burukov::BSTree< Key, Value, Compare >::push(Key&& k, Value&& v)
{
  if (root_->isFake())
  {
    root_ = new Node(std::move(k), std::move(v), nullptr);
    ++size_;
    return;
  }

  Node* parent = nullptr;
  Node* cur = root_;

  while (cur != nullptr && !cur->isFake())
  {
    parent = cur;
    if (comp_(k, cur->key_))
    {
      cur = cur->left_;
    }
    else if (comp_(cur->key_, k))
    {
      cur = cur->right_;
    }
    else
    {
      cur->value_ = std::move(v);
      return;
    }
  }

  Node* n = new Node(std::move(k), std::move(v), parent);
  if (comp_(n->key_, parent->key_))
  {
    parent->left_ = n;
  }
  else
  {
    parent->right_ = n;
  }
  ++size_;
}

template< class Key, class Value, class Compare >
burukov::detail::TreeNode< Key, Value >*
burukov::BSTree< Key, Value, Compare >::leftmost(Node* node) const
{
  if (node == nullptr || node->isFake())
  {
    return Node::fakeLeaf;
  }
  while (node != nullptr && !node->left_->isFake())
  {
    node = node->left_;
  }
  return node;
}

template< class Key, class Value, class Compare >
Value burukov::BSTree< Key, Value, Compare >::drop(const Key& k)
{
  Node* n = findNode(k);
  if (n == nullptr)
  {
    throw std::out_of_range("key not found");
  }

  Value res = std::move(n->value_);

  if (!n->left_->isFake() && !n->right_->isFake())
  {
    Node* succ = leftmost(n->right_);
    n->key_ = std::move(succ->key_);
    n->value_ = std::move(succ->value_);
    n = succ;
  }

  Node* child = (!n->left_->isFake()) ? n->left_ : n->right_;
  if (child->isFake())
  {
    child = Node::fakeLeaf;
  }

  if (n->parent_ == nullptr)
  {
    root_ = child;
    if (root_->isFake())
    {
      root_ = Node::fakeLeaf;
    }
  }
  else if (n == n->parent_->left_)
  {
    n->parent_->left_ = child;
  }
  else
  {
    n->parent_->right_ = child;
  }

  if (child != Node::fakeLeaf)
  {
    child->parent_ = n->parent_;
  }

  delete n;
  --size_;
  return res;
}

template< class Key, class Value, class Compare >
typename burukov::BSTree< Key, Value, Compare >::iterator
burukov::BSTree< Key, Value, Compare >::begin() const
{
  if (root_->isFake())
  {
    return iterator(nullptr);
  }
  Node* l = leftmost(root_);
  if (l->isFake())
  {
    return iterator(nullptr);
  }
  return iterator(l);
}

template< class Key, class Value, class Compare >
size_t burukov::BSTree< Key, Value, Compare >::getHeight(Node* node) const
{
  if (node == nullptr || node->isFake())
  {
    return 0;
  }
  size_t l = getHeight(node->left_);
  size_t r = getHeight(node->right_);
  return 1 + (l > r ? l : r);
}

template< class Key, class Value, class Compare >
size_t burukov::BSTree< Key, Value, Compare >::height() const
{
  return getHeight(root_);
}

template< class Key, class Value, class Compare >
size_t burukov::BSTree< Key, Value, Compare >::height(iterator it) const
{
  Node* n = it.getNode();
  if (n == nullptr || n->isFake())
  {
    return 0;
  }
  return getHeight(n);
}

template< class Key, class Value, class Compare >
void burukov::BSTree< Key, Value, Compare >::transplant(Node* u, Node* v)
{
  if (u->parent_ == nullptr)
  {
    root_ = v;
    if (root_->isFake())
    {
      root_ = Node::fakeLeaf;
    }
  }
  else if (u == u->parent_->left_)
  {
    u->parent_->left_ = v;
  }
  else
  {
    u->parent_->right_ = v;
  }
  if (v != Node::fakeLeaf)
  {
    v->parent_ = u->parent_;
  }
}

template< class Key, class Value, class Compare >
typename burukov::BSTree< Key, Value, Compare >::iterator
burukov::BSTree< Key, Value, Compare >::rotateLeft(iterator it)
{
  Node* y = it.getNode();
  if (y == nullptr || y->isFake() || y->right_->isFake())
  {
    return it;
  }

  Node* x = y->right_;
  y->right_ = x->left_;
  if (!x->left_->isFake())
  {
    x->left_->parent_ = y;
  }

  transplant(y, x);
  x->left_ = y;
  y->parent_ = x;

  return iterator(x);
}

template< class Key, class Value, class Compare >
typename burukov::BSTree< Key, Value, Compare >::iterator
burukov::BSTree< Key, Value, Compare >::rotateRight(iterator it)
{
  Node* x = it.getNode();
  if (x == nullptr || x->isFake() || x->left_->isFake())
  {
    return it;
  }

  Node* y = x->left_;
  x->left_ = y->right_;
  if (!y->right_->isFake())
  {
    y->right_->parent_ = x;
  }

  transplant(x, y);
  y->right_ = x;
  x->parent_ = y;

  return iterator(y);
}

template< class Key, class Value, class Compare >
typename burukov::BSTree< Key, Value, Compare >::iterator
burukov::BSTree< Key, Value, Compare >::rotateLargeLeft(iterator it)
{
  Node* node = it.getNode();
  if (node == nullptr || node->isFake() || node->left_->isFake() || node->left_->right_->isFake())
  {
    return it;
  }

  rotateRight(iterator(node->left_));
  return rotateLeft(it);
}

template< class Key, class Value, class Compare >
typename burukov::BSTree< Key, Value, Compare >::iterator
burukov::BSTree< Key, Value, Compare >::rotateLargeRight(iterator it)
{
  Node* node = it.getNode();
  if (node == nullptr || node->isFake() || node->right_->isFake() || node->right_->left_->isFake())
  {
    return it;
  }

  rotateLeft(iterator(node->right_));
  return rotateRight(it);
}

#endif
