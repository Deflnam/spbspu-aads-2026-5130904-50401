#ifndef AVLTREE_HPP
#define AVLTREE_HPP

#include <list.hpp>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>
#include <algorithm>

namespace burukov
{
  namespace detail
  {
    template< class Key, class Value >
    struct AVLNode
    {
      Key key_;
      Value value_;
      AVLNode *left_;
      AVLNode *right_;
      AVLNode *parent_;
      int height_;

      static AVLNode *nilNode;

      AVLNode(const Key &key, const Value &value, AVLNode *parent):
        key_(key),
        value_(value),
        left_(nilNode),
        right_(nilNode),
        parent_(parent),
        height_(1)
      {}

      AVLNode(Key &&key, Value &&value, AVLNode *parent):
        key_(std::forward< Key >(key)),
        value_(std::forward< Value >(value)),
        left_(nilNode),
        right_(nilNode),
        parent_(parent),
        height_(1)
      {}

      bool isNil() const noexcept
      {
        return this == nilNode;
      }

      void updateHeight()
      {
        if (isNil()) {
          height_ = 0;
          return;
        }
        height_ = 1 + std::max(left_->height_, right_->height_);
      }
    };

    template< class Key, class Value >
    AVLNode< Key, Value > *AVLNode< Key, Value >::nilNode = nullptr;
  }

  template< class Key, class Value, class Compare = std::less< Key > >
  class AVLTree
  {
  public:
    using Node = detail::AVLNode< Key, Value >;

    AVLTree();
    ~AVLTree();
    AVLTree(const AVLTree &other);
    AVLTree(AVLTree &&other) noexcept;
    AVLTree &operator=(const AVLTree &other);
    AVLTree &operator=(AVLTree &&other) noexcept;

    bool empty() const;
    size_t size() const;
    void clear();

    const Value &at(const Key &k) const;
    Value &at(const Key &k);
    void push(const Key &k, const Value &v);
    void push(Key &&k, Value &&v);
    Value drop(const Key &k);
    bool hasKey(const Key &k) const;

    template< class Func >
    void traverseInOrder(Func f) const;

  private:
    Node *root_;
    Node *nilNode_;
    size_t size_;
    Compare comp_;

    static void initNil();
    Node *findNode(const Key &k) const;
    void deleteTree(Node *node);
    void copyTree(Node *&dest, Node *src, Node *parent);
    int getBalance(Node *n) const;
    Node *rotateRight(Node *y);
    Node *rotateLeft(Node *x);
    Node *balance(Node *z);
    Node *minValueNode(Node *node) const;
  };

  template< class Key, class Value, class Compare >
  void AVLTree< Key, Value, Compare >::initNil()
  {
    if (Node::nilNode == nullptr) {
      Node::nilNode = new Node(Key(), Value(), nullptr);
      Node::nilNode->left_ = Node::nilNode;
      Node::nilNode->right_ = Node::nilNode;
      Node::nilNode->parent_ = Node::nilNode;
      Node::nilNode->height_ = 0;
    }
  }

  template< class Key, class Value, class Compare >
  AVLTree< Key, Value, Compare >::AVLTree():
    root_(nullptr),
    nilNode_(nullptr),
    size_(0),
    comp_()
  {
    initNil();
    nilNode_ = Node::nilNode;
    root_ = new Node(Key(), Value(), nullptr);
    root_->left_ = nilNode_;
    root_->right_ = nilNode_;
    root_->parent_ = nilNode_;
  }

  template< class Key, class Value, class Compare >
  AVLTree< Key, Value, Compare >::~AVLTree()
  {
    if (root_ != nullptr) {
      if (root_->right_ != nullptr && !root_->right_->isNil()) {
        deleteTree(root_->right_);
      }
      delete root_;
    }
  }

  template< class Key, class Value, class Compare >
  void AVLTree< Key, Value, Compare >::copyTree(Node *&dest, Node *src,
    Node *parent)
  {
    if (src == nullptr || src->isNil()) {
      dest = nilNode_;
      return;
    }
    dest = new Node(src->key_, src->value_, parent);
    copyTree(dest->left_, src->left_, dest);
    copyTree(dest->right_, src->right_, dest);
  }

  template< class Key, class Value, class Compare >
  AVLTree< Key, Value, Compare >::AVLTree(const AVLTree &other):
    root_(nullptr),
    nilNode_(nullptr),
    size_(other.size_),
    comp_(other.comp_)
  {
    initNil();
    nilNode_ = Node::nilNode;
    root_ = new Node(Key(), Value(), nullptr);
    root_->left_ = nilNode_;
    root_->right_ = nilNode_;
    root_->parent_ = nilNode_;
    if (other.root_ != nullptr && other.root_->right_ != nullptr &&
      !other.root_->right_->isNil()) {
      copyTree(root_->right_, other.root_->right_, root_);
    }
  }

  template< class Key, class Value, class Compare >
  AVLTree< Key, Value, Compare >::AVLTree(AVLTree &&other) noexcept:
    root_(std::exchange(other.root_, nullptr)),
    nilNode_(std::exchange(other.nilNode_, nullptr)),
    size_(std::exchange(other.size_, 0)),
    comp_(std::move(other.comp_))
  {
    other.root_ = new Node(Key(), Value(), nullptr);
    other.root_->left_ = nilNode_;
    other.root_->right_ = nilNode_;
    other.root_->parent_ = nilNode_;
  }

  template< class Key, class Value, class Compare >
  AVLTree< Key, Value, Compare > &
  AVLTree< Key, Value, Compare >::operator=(const AVLTree &other)
  {
    if (this != &other) {
      AVLTree temp(other);
      std::swap(root_, temp.root_);
      std::swap(nilNode_, temp.nilNode_);
      std::swap(size_, temp.size_);
      std::swap(comp_, temp.comp_);
    }
    return *this;
  }

  template< class Key, class Value, class Compare >
  AVLTree< Key, Value, Compare > &
  AVLTree< Key, Value, Compare >::operator=(AVLTree &&other) noexcept
  {
    if (this != &other) {
      std::swap(root_, other.root_);
      std::swap(nilNode_, other.nilNode_);
      std::swap(size_, other.size_);
      std::swap(comp_, other.comp_);
    }
    return *this;
  }

  template< class Key, class Value, class Compare >
  void AVLTree< Key, Value, Compare >::deleteTree(Node *node)
  {
    if (node == nullptr || node->isNil()) {
      return;
    }
    if (node->left_ != nullptr && !node->left_->isNil()) {
      deleteTree(node->left_);
    }
    if (node->right_ != nullptr && !node->right_->isNil()) {
      deleteTree(node->right_);
    }
    delete node;
  }

  template< class Key, class Value, class Compare >
  void AVLTree< Key, Value, Compare >::clear()
  {
    if (root_ != nullptr && root_->right_ != nullptr &&
      !root_->right_->isNil()) {
      deleteTree(root_->right_);
      root_->right_ = nilNode_;
    }
    size_ = 0;
  }

  template< class Key, class Value, class Compare >
  bool AVLTree< Key, Value, Compare >::empty() const
  {
    return size_ == 0;
  }

  template< class Key, class Value, class Compare >
  size_t AVLTree< Key, Value, Compare >::size() const
  {
    return size_;
  }

  template< class Key, class Value, class Compare >
  typename AVLTree< Key, Value, Compare >::Node *
  AVLTree< Key, Value, Compare >::findNode(const Key &k) const
  {
    if (root_ == nullptr || root_->right_ == nullptr ||
      root_->right_->isNil()) {
      return nullptr;
    }
    Node *cur = root_->right_;
    while (cur != nullptr && !cur->isNil()) {
      if (comp_(k, cur->key_)) {
        cur = cur->left_;
      } else if (comp_(cur->key_, k)) {
        cur = cur->right_;
      } else {
        return cur;
      }
    }
    return nullptr;
  }

  template< class Key, class Value, class Compare >
  bool AVLTree< Key, Value, Compare >::hasKey(const Key &k) const
  {
    return findNode(k) != nullptr;
  }

  template< class Key, class Value, class Compare >
  const Value &AVLTree< Key, Value, Compare >::at(const Key &k) const
  {
    Node *n = findNode(k);
    if (n == nullptr) {
      throw std::out_of_range("key not found");
    }
    return n->value_;
  }

  template< class Key, class Value, class Compare >
  Value &AVLTree< Key, Value, Compare >::at(const Key &k)
  {
    Node *n = findNode(k);
    if (n == nullptr) {
      throw std::out_of_range("key not found");
    }
    return n->value_;
  }

  template< class Key, class Value, class Compare >
  int AVLTree< Key, Value, Compare >::getBalance(Node *n) const
  {
    if (n == nullptr || n->isNil()) {
      return 0;
    }
    return n->left_->height_ - n->right_->height_;
  }

  template< class Key, class Value, class Compare >
  typename AVLTree< Key, Value, Compare >::Node *
  AVLTree< Key, Value, Compare >::rotateRight(Node *y)
  {
    Node *x = y->left_;
    Node *t2 = x->right_;
    x->right_ = y;
    y->left_ = t2;
    if (!t2->isNil()) {
      t2->parent_ = y;
    }
    Node *pParent = y->parent_;
    x->parent_ = pParent;
    y->parent_ = x;
    if (pParent == root_) {
      root_->right_ = x;
    } else if (y == pParent->left_) {
      pParent->left_ = x;
    } else {
      pParent->right_ = x;
    }
    y->updateHeight();
    x->updateHeight();
    return x;
  }

  template< class Key, class Value, class Compare >
  typename AVLTree< Key, Value, Compare >::Node *
  AVLTree< Key, Value, Compare >::rotateLeft(Node *x)
  {
    Node *y = x->right_;
    Node *t2 = y->left_;
    y->left_ = x;
    x->right_ = t2;
    if (!t2->isNil()) {
      t2->parent_ = x;
    }
    Node *pParent = x->parent_;
    y->parent_ = pParent;
    x->parent_ = y;
    if (pParent == root_) {
      root_->right_ = y;
    } else if (x == pParent->left_) {
      pParent->left_ = y;
    } else {
      pParent->right_ = y;
    }
    x->updateHeight();
    y->updateHeight();
    return y;
  }

  template< class Key, class Value, class Compare >
  typename AVLTree< Key, Value, Compare >::Node *
  AVLTree< Key, Value, Compare >::balance(Node *z)
  {
    z->updateHeight();
    int bal = getBalance(z);
    if (bal > 1) {
      if (getBalance(z->left_) >= 0) {
        return rotateRight(z);
      } else {
        z->left_ = rotateLeft(z->left_);
        return rotateRight(z);
      }
    }
    if (bal < -1) {
      if (getBalance(z->right_) <= 0) {
        return rotateLeft(z);
      } else {
        z->right_ = rotateRight(z->right_);
        return rotateLeft(z);
      }
    }
    return z;
  }

  template< class Key, class Value, class Compare >
  void AVLTree< Key, Value, Compare >::push(const Key &k, const Value &v)
  {
    Node *parent = root_;
    Node *cur = root_->right_;
    while (cur != nullptr && !cur->isNil()) {
      parent = cur;
      if (comp_(k, cur->key_)) {
        cur = cur->left_;
      } else if (comp_(cur->key_, k)) {
        cur = cur->right_;
      } else {
        cur->value_ = v;
        return;
      }
    }
    Node *n = new Node(k, v, parent);
    if (parent == root_) {
      root_->right_ = n;
    } else if (comp_(k, parent->key_)) {
      parent->left_ = n;
    } else {
      parent->right_ = n;
    }
    ++size_;
    Node *fix = n;
    while (fix != nullptr && !fix->isNil() && fix != root_) {
      fix = balance(fix);
      fix = fix->parent_;
    }
  }

  template< class Key, class Value, class Compare >
  void AVLTree< Key, Value, Compare >::push(Key &&k, Value &&v)
  {
    Node *parent = root_;
    Node *cur = root_->right_;
    while (cur != nullptr && !cur->isNil()) {
      parent = cur;
      if (comp_(k, cur->key_)) {
        cur = cur->left_;
      } else if (comp_(cur->key_, k)) {
        cur = cur->right_;
      } else {
        cur->value_ = std::forward< Value >(v);
        return;
      }
    }
    Node *n = new Node(std::forward< Key >(k), std::forward< Value >(v), parent);
    if (parent == root_) {
      root_->right_ = n;
    } else if (comp_(k, parent->key_)) {
      parent->left_ = n;
    } else {
      parent->right_ = n;
    }
    ++size_;
    Node *fix = n;
    while (fix != nullptr && !fix->isNil() && fix != root_) {
      fix = balance(fix);
      fix = fix->parent_;
    }
  }

  template< class Key, class Value, class Compare >
  typename AVLTree< Key, Value, Compare >::Node *
  AVLTree< Key, Value, Compare >::minValueNode(Node *node) const
  {
    Node *current = node;
    while (!current->left_->isNil()) {
      current = current->left_;
    }
    return current;
  }

  template< class Key, class Value, class Compare >
  Value AVLTree< Key, Value, Compare >::drop(const Key &k)
  {
    Node *z = findNode(k);
    if (z == nullptr) {
      throw std::out_of_range("key not found");
    }
    Value res = z->value_;
    Node *y = z;
    Node *x = nullptr;
    if (z->left_->isNil() || z->right_->isNil()) {
      y = z;
    } else {
      y = minValueNode(z->right_);
    }
    if (!y->left_->isNil()) {
      x = y->left_;
    } else {
      x = y->right_;
    }
    x->parent_ = y->parent_;
    if (y->parent_ == root_) {
      root_->right_ = x;
    } else if (y == y->parent_->left_) {
      y->parent_->left_ = x;
    } else {
      y->parent_->right_ = x;
    }
    if (y != z) {
      z->key_ = y->key_;
      z->value_ = std::move(y->value_);
    }
    Node *fixStart = y->parent_;
    if (fixStart == root_) {
      fixStart = x->isNil() ? nullptr : x;
    }
    delete y;
    --size_;
    while (fixStart != nullptr && !fixStart->isNil() && fixStart != root_) {
      fixStart = balance(fixStart);
      fixStart = fixStart->parent_;
    }
    return res;
  }

  template< class Key, class Value, class Compare >
  template< class Func >
  void AVLTree< Key, Value, Compare >::traverseInOrder(Func f) const
  {
    if (root_ == nullptr || root_->right_ == nullptr ||
      root_->right_->isNil()) {
      return;
    }
    List< Node * > stack;
    Node *curr = root_->right_;
    while ((curr != nullptr && !curr->isNil()) || !stack.empty()) {
      while (curr != nullptr && !curr->isNil()) {
        stack.pushBack(curr);
        curr = curr->left_;
      }
      curr = stack.front();
      stack.popFront();
      f(curr->key_, curr->value_);
      curr = curr->right_;
    }
  }
}

#endif
