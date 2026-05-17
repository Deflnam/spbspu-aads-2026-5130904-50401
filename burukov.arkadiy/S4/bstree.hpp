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
  using iterator = BSTIterator< Key, Value >;
  using const_iterator = BSTConstIterator< Key, Value >;

  BSTree();
  BSTree(const BSTree& other);
  BSTree(BSTree&& other) noexcept;
  ~BSTree();

  BSTree& operator=(const BSTree& other);
  BSTree& operator=(BSTree&& other) noexcept;

  bool empty() const;
  size_t size() const;
  void swap(BSTree& other) noexcept;
  void clear() noexcept;

  const Value& at(const Key& k) const;
  Value& at(const Key& k);
  void push(const Key& k, const Value& v);
  void push(Key&& k, Value&& v);
  Value drop(const Key& k);


  bool hasKey(const Key& k) const;

  iterator begin() noexcept;
  iterator end() noexcept;
  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;

  size_t height() const;
  size_t height(const_iterator it) const;

  const_iterator rotateLeft(const_iterator it);
  const_iterator rotateRight(const_iterator it);
  const_iterator rotateLargeLeft(const_iterator it);
  const_iterator rotateLargeRight(const_iterator it);

private:
  using Node = detail::TreeNode< Key, Value >;

  Node* fake_root_;
  Node* fake_leaf_;
  size_t size_;
  Compare comp_;

  static void initFakes();

  Node* findNode(const Key& k) const;
  Node* getLeftmost(Node* node) const;
  Node* cloneSubtree(Node* src, Node* parent);
  void deleteSubtree(Node* node) noexcept;
  size_t computeHeight(Node* node) const;
  void replaceNode(Node* u, Node* v);
  void rotateLeftNode(Node* y);
  void rotateRightNode(Node* x);
};

}


template< class Key, class Value, class Compare >
void burukov::BSTree< Key, Value, Compare >::initFakes()
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
burukov::BSTree< Key, Value, Compare >::BSTree():
  fake_root_(nullptr),
  fake_leaf_(nullptr),
  size_(0),
  comp_()
{
  initFakes();
  fake_leaf_ = Node::fakeLeaf;
  fake_root_ = new Node(Key(), Value(), nullptr);
  fake_root_->left_ = fake_leaf_;
  fake_root_->right_ = fake_leaf_;
  fake_root_->parent_ = fake_leaf_;
}

template< class Key, class Value, class Compare >
burukov::BSTree< Key, Value, Compare >::BSTree(const BSTree& other):
  fake_root_(nullptr),
  fake_leaf_(nullptr),
  size_(0),
  comp_(other.comp_)
{
  initFakes();
  fake_leaf_ = Node::fakeLeaf;
  fake_root_ = new Node(Key(), Value(), nullptr);
  fake_root_->left_ = fake_leaf_;
  fake_root_->right_ = cloneSubtree(other.fake_root_->right_, fake_root_);
  fake_root_->parent_ = fake_leaf_;
  size_ = other.size_;
}

template< class Key, class Value, class Compare >
burukov::BSTree< Key, Value, Compare >::BSTree(BSTree&& other) noexcept:
  fake_root_(other.fake_root_),
  fake_leaf_(other.fake_leaf_),
  size_(other.size_),
  comp_(std::move(other.comp_))
{
  other.fake_root_ = nullptr;
  other.fake_leaf_ = nullptr;
  other.size_ = 0;
}

template< class Key, class Value, class Compare >
burukov::BSTree< Key, Value, Compare >::~BSTree()
{
  clear();
  delete fake_root_;
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
    delete fake_root_;
    fake_root_ = other.fake_root_;
    fake_leaf_ = other.fake_leaf_;
    size_ = other.size_;
    comp_ = std::move(other.comp_);
    other.fake_root_ = nullptr;
    other.fake_leaf_ = nullptr;
    other.size_ = 0;
  }
  return *this;
}

template< class Key, class Value, class Compare >
bool burukov::BSTree< Key, Value, Compare >::empty() const
{
  return size_ == 0;
}

template< class Key, class Value, class Compare >
size_t burukov::BSTree< Key, Value, Compare >::size() const
{
  return size_;
}

template< class Key, class Value, class Compare >
void burukov::BSTree< Key, Value, Compare >::swap(BSTree& other) noexcept
{
  std::swap(fake_root_, other.fake_root_);
  std::swap(fake_leaf_, other.fake_leaf_);
  std::swap(size_, other.size_);
  std::swap(comp_, other.comp_);
}

template< class Key, class Value, class Compare >
void burukov::BSTree< Key, Value, Compare >::deleteSubtree(Node* node) noexcept
{
  if (node->isFake())
  {
    return;
  }
  deleteSubtree(node->left_);
  deleteSubtree(node->right_);
  delete node;
}

template< class Key, class Value, class Compare >
void burukov::BSTree< Key, Value, Compare >::clear() noexcept
{
  deleteSubtree(fake_root_->right_);
  fake_root_->right_ = fake_leaf_;
  size_ = 0;
}

template< class Key, class Value, class Compare >
burukov::detail::TreeNode< Key, Value >*
burukov::BSTree< Key, Value, Compare >::cloneSubtree(Node* src, Node* parent)
{
  if (src->isFake())
  {
    return fake_leaf_;
  }
  Node* n = new Node(src->key_, src->value_, parent);
  n->left_ = cloneSubtree(src->left_, n);
  n->right_ = cloneSubtree(src->right_, n);
  return n;
}

template< class Key, class Value, class Compare >
burukov::detail::TreeNode< Key, Value >*
burukov::BSTree< Key, Value, Compare >::findNode(const Key& k) const
{
  Node* cur = fake_root_->right_;
  while (!cur->isFake())
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
  Node* parent = fake_root_;
  Node* cur = fake_root_->right_;

  while (!cur->isFake())
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
  Node* parent = fake_root_;
  Node* cur = fake_root_->right_;

  while (!cur->isFake())
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
burukov::BSTree< Key, Value, Compare >::getLeftmost(Node* node) const
{
  while (!node->left_->isFake())
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
    Node* succ = getLeftmost(n->right_);
    n->key_ = std::move(succ->key_);
    n->value_ = std::move(succ->value_);
    n = succ;
  }

  Node* child = (!n->left_->isFake()) ? n->left_ : n->right_;

  if (n->parent_->left_ == n)
  {
    n->parent_->left_ = child;
  }
  else
  {
    n->parent_->right_ = child;
  }
  child->parent_ = n->parent_;

  delete n;
  --size_;
  return res;
}

template< class Key, class Value, class Compare >
typename burukov::BSTree< Key, Value, Compare >::iterator
burukov::BSTree< Key, Value, Compare >::begin() noexcept
{
  if (empty())
  {
    return iterator(nullptr);
  }
  return iterator(getLeftmost(fake_root_->right_));
}

template< class Key, class Value, class Compare >
typename burukov::BSTree< Key, Value, Compare >::iterator
burukov::BSTree< Key, Value, Compare >::end() noexcept
{
  return iterator(nullptr);
}

template< class Key, class Value, class Compare >
typename burukov::BSTree< Key, Value, Compare >::const_iterator
burukov::BSTree< Key, Value, Compare >::begin() const noexcept
{
  if (empty())
  {
    return const_iterator(nullptr);
  }
  return const_iterator(getLeftmost(fake_root_->right_));
}

template< class Key, class Value, class Compare >
typename burukov::BSTree< Key, Value, Compare >::const_iterator
burukov::BSTree< Key, Value, Compare >::end() const noexcept
{
  return const_iterator(nullptr);
}

template< class Key, class Value, class Compare >
typename burukov::BSTree< Key, Value, Compare >::const_iterator
burukov::BSTree< Key, Value, Compare >::cbegin() const noexcept
{
  return begin();
}

template< class Key, class Value, class Compare >
typename burukov::BSTree< Key, Value, Compare >::const_iterator
burukov::BSTree< Key, Value, Compare >::cend() const noexcept
{
  return end();
}

template< class Key, class Value, class Compare >
size_t burukov::BSTree< Key, Value, Compare >::computeHeight(Node* node) const
{
  if (node->isFake())
  {
    return 0;
  }
  size_t l = computeHeight(node->left_);
  size_t r = computeHeight(node->right_);
  return 1 + ((l > r) ? l : r);
}

template< class Key, class Value, class Compare >
size_t burukov::BSTree< Key, Value, Compare >::height() const
{
  return computeHeight(fake_root_->right_);
}

template< class Key, class Value, class Compare >
size_t burukov::BSTree< Key, Value, Compare >::height(const_iterator it) const
{
  return computeHeight(const_cast< Node* >(it.node_));
}

template< class Key, class Value, class Compare >
void burukov::BSTree< Key, Value, Compare >::replaceNode(Node* u, Node* v)
{
  if (u->parent_->isFake())
  {
    fake_root_->right_ = v;
  }
  else if (u == u->parent_->left_)
  {
    u->parent_->left_ = v;
  }
  else
  {
    u->parent_->right_ = v;
  }
  v->parent_ = u->parent_;
}

template< class Key, class Value, class Compare >
void burukov::BSTree< Key, Value, Compare >::rotateLeftNode(Node* y)
{
  Node* x = y->right_;
  y->right_ = x->left_;
  if (!x->left_->isFake())
  {
    x->left_->parent_ = y;
  }

  replaceNode(y, x);
  x->left_ = y;
  y->parent_ = x;
}

template< class Key, class Value, class Compare >
void burukov::BSTree< Key, Value, Compare >::rotateRightNode(Node* x)
{
  Node* y = x->left_;
  x->left_ = y->right_;
  if (!y->right_->isFake())
  {
    y->right_->parent_ = x;
  }

  replaceNode(x, y);
  y->right_ = x;
  x->parent_ = y;
}

template< class Key, class Value, class Compare >
typename burukov::BSTree< Key, Value, Compare >::const_iterator
burukov::BSTree< Key, Value, Compare >::rotateLeft(const_iterator it)
{
  Node* y = const_cast< Node* >(it.node_);
  if (y->isFake() || y->right_->isFake())
  {
    return it;
  }

  rotateLeftNode(y);
  return const_iterator(y->parent_);
}

template< class Key, class Value, class Compare >
typename burukov::BSTree< Key, Value, Compare >::const_iterator
burukov::BSTree< Key, Value, Compare >::rotateRight(const_iterator it)
{
  Node* x = const_cast< Node* >(it.node_);
  if (x->isFake() || x->left_->isFake())
  {
    return it;
  }

  rotateRightNode(x);
  return const_iterator(x->parent_);
}

template< class Key, class Value, class Compare >
typename burukov::BSTree< Key, Value, Compare >::const_iterator
burukov::BSTree< Key, Value, Compare >::rotateLargeLeft(const_iterator it)
{
  Node* node = const_cast< Node* >(it.node_);
  if (node->isFake() || node->left_->isFake() || node->left_->right_->isFake())
  {
    return it;
  }

  rotateRightNode(node->left_);
  rotateLeftNode(node);
  return const_iterator(node->parent_);
}

template< class Key, class Value, class Compare >
typename burukov::BSTree< Key, Value, Compare >::const_iterator
burukov::BSTree< Key, Value, Compare >::rotateLargeRight(const_iterator it)
{
  Node* node = const_cast< Node* >(it.node_);
  if (node->isFake() || node->right_->isFake() || node->right_->left_->isFake())
  {
    return it;
  }

  rotateLeftNode(node->right_);
  rotateRightNode(node);
  return const_iterator(node->parent_);
}

#endif
