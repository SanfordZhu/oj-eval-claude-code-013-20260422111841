/**
* implement a container like std::map
*/
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
   class Key,
   class T,
   class Compare = std::less <Key>
   > class map {
 public:
  typedef pair<const Key, T> value_type;

 private:
  enum Color { RED, BLACK };
  struct Node {
    value_type *data;
    Node *left, *right, *parent;
    Color color;
    int height;

    Node() : data(nullptr), left(nullptr), right(nullptr), parent(nullptr), color(RED), height(1) {}
    Node(const value_type &v) : left(nullptr), right(nullptr), parent(nullptr), color(RED), height(1) {
      data = new value_type(v);
    }
    ~Node() {
      delete data;
    }
  };

  Node *root;
  Node *nil;
  size_t num_elements;
  Compare cmp;

  void init() {
    nil = new Node();
    nil->color = BLACK;
    nil->height = 0;
    nil->left = nil->right = nil->parent = nil;
    root = nil;
    num_elements = 0;
  }

  void clearNode(Node *node) {
    if (node != nil) {
      clearNode(node->left);
      clearNode(node->right);
      delete node;
    }
  }

  int getHeight(Node *node) const {
    return node == nil ? 0 : node->height;
  }

  void updateHeight(Node *node) {
    if (node != nil) {
      node->height = 1 + (getHeight(node->left) > getHeight(node->right) ? getHeight(node->left) : getHeight(node->right));
    }
  }

  void leftRotate(Node *x) {
    Node *y = x->right;
    x->right = y->left;
    if (y->left != nil) {
      y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == nil) {
      root = y;
    } else if (x == x->parent->left) {
      x->parent->left = y;
    } else {
      x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
    updateHeight(x);
    updateHeight(y);
  }

  void rightRotate(Node *y) {
    Node *x = y->left;
    y->left = x->right;
    if (x->right != nil) {
      x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == nil) {
      root = x;
    } else if (y == y->parent->right) {
      y->parent->right = x;
    } else {
      y->parent->left = x;
    }
    x->right = y;
    y->parent = x;
    updateHeight(y);
    updateHeight(x);
  }

  void insertFixup(Node *z) {
    while (z->parent->color == RED) {
      if (z->parent == z->parent->parent->left) {
        Node *y = z->parent->parent->right;
        if (y->color == RED) {
          z->parent->color = BLACK;
          y->color = BLACK;
          z->parent->parent->color = RED;
          z = z->parent->parent;
        } else {
          if (z == z->parent->right) {
            z = z->parent;
            leftRotate(z);
          }
          z->parent->color = BLACK;
          z->parent->parent->color = RED;
          rightRotate(z->parent->parent);
        }
      } else {
        Node *y = z->parent->parent->left;
        if (y->color == RED) {
          z->parent->color = BLACK;
          y->color = BLACK;
          z->parent->parent->color = RED;
          z = z->parent->parent;
        } else {
          if (z == z->parent->left) {
            z = z->parent;
            rightRotate(z);
          }
          z->parent->color = BLACK;
          z->parent->parent->color = RED;
          leftRotate(z->parent->parent);
        }
      }
    }
    root->color = BLACK;
  }

  void transplant(Node *u, Node *v) {
    if (u->parent == nil) {
      root = v;
    } else if (u == u->parent->left) {
      u->parent->left = v;
    } else {
      u->parent->right = v;
    }
    v->parent = u->parent;
  }

  void deleteFixup(Node *x) {
    while (x != root && x->color == BLACK) {
      if (x == x->parent->left) {
        Node *w = x->parent->right;
        if (w->color == RED) {
          w->color = BLACK;
          x->parent->color = RED;
          leftRotate(x->parent);
          w = x->parent->right;
        }
        if (w->left->color == BLACK && w->right->color == BLACK) {
          w->color = RED;
          x = x->parent;
        } else {
          if (w->right->color == BLACK) {
            w->left->color = BLACK;
            w->color = RED;
            rightRotate(w);
            w = x->parent->right;
          }
          w->color = x->parent->color;
          x->parent->color = BLACK;
          w->right->color = BLACK;
          leftRotate(x->parent);
          x = root;
        }
      } else {
        Node *w = x->parent->left;
        if (w->color == RED) {
          w->color = BLACK;
          x->parent->color = RED;
          rightRotate(x->parent);
          w = x->parent->left;
        }
        if (w->right->color == BLACK && w->left->color == BLACK) {
          w->color = RED;
          x = x->parent;
        } else {
          if (w->left->color == BLACK) {
            w->right->color = BLACK;
            w->color = RED;
            leftRotate(w);
            w = x->parent->left;
          }
          w->color = x->parent->color;
          x->parent->color = BLACK;
          w->left->color = BLACK;
          rightRotate(x->parent);
          x = root;
        }
      }
    }
    x->color = BLACK;
  }

  Node *treeMinimum(Node *node) const {
    while (node->left != nil) {
      node = node->left;
    }
    return node;
  }

  Node *treeMaximum(Node *node) const {
    while (node->right != nil) {
      node = node->right;
    }
    return node;
  }

  Node *findNode(const Key &key) const {
    Node *current = root;
    while (current != nil) {
      if (cmp(key, current->data->first)) {
        current = current->left;
      } else if (cmp(current->data->first, key)) {
        current = current->right;
      } else {
        return current;
      }
    }
    return nil;
  }

  Node *copyTree(Node *other_node, Node *other_nil, Node *parent) {
    if (other_node == other_nil) return nil;
    Node *new_node = new Node(*other_node->data);
    new_node->color = other_node->color;
    new_node->height = other_node->height;
    new_node->parent = parent;
    new_node->left = copyTree(other_node->left, other_nil, new_node);
    new_node->right = copyTree(other_node->right, other_nil, new_node);
    return new_node;
  }

 public:
  class const_iterator;
  class iterator {
   private:
    Node *node;
    Node *nil;
    const map *container;

   public:
    iterator() : node(nullptr), nil(nullptr), container(nullptr) {}

    iterator(Node *n, Node *n_nil, const map *c) : node(n), nil(n_nil), container(c) {}

    iterator(const iterator &other) : node(other.node), nil(other.nil), container(other.container) {}

    iterator operator++(int) {
      if (node == nil) {
        throw invalid_iterator();
      }
      iterator temp = *this;
      if (node->right != nil) {
        node = node->right;
        while (node->left != nil) {
          node = node->left;
        }
      } else {
        Node *parent = node->parent;
        while (parent != nil && node == parent->right) {
          node = parent;
          parent = parent->parent;
        }
        node = parent;
      }
      return temp;
    }

    iterator &operator++() {
      if (node == nil) {
        throw invalid_iterator();
      }
      if (node->right != nil) {
        node = node->right;
        while (node->left != nil) {
          node = node->left;
        }
      } else {
        Node *parent = node->parent;
        while (parent != nil && node == parent->right) {
          node = parent;
          parent = parent->parent;
        }
        node = parent;
      }
      return *this;
    }

    iterator operator--(int) {
      iterator temp = *this;
      if (node == nil) {
        if (container && container->root != nil) {
          node = container->root;
          while (node->right != nil) {
            node = node->right;
          }
        } else {
          throw invalid_iterator();
        }
      } else {
        if (node->left != nil) {
          node = node->left;
          while (node->right != nil) {
            node = node->right;
          }
        } else {
          Node *parent = node->parent;
          while (parent != nil && node == parent->left) {
            node = parent;
            parent = parent->parent;
          }
          node = parent;
        }
      }
      return temp;
    }

    iterator &operator--() {
      if (node == nil) {
        if (container && container->root != nil) {
          node = container->root;
          while (node->right != nil) {
            node = node->right;
          }
        } else {
          throw invalid_iterator();
        }
      } else {
        if (node->left != nil) {
          node = node->left;
          while (node->right != nil) {
            node = node->right;
          }
        } else {
          Node *parent = node->parent;
          while (parent != nil && node == parent->left) {
            node = parent;
            parent = parent->parent;
          }
          node = parent;
        }
      }
      return *this;
    }

    value_type &operator*() const {
      return *(node->data);
    }

    bool operator==(const iterator &rhs) const {
      return node == rhs.node && nil == rhs.nil;
    }

    bool operator==(const const_iterator &rhs) const {
      return node == rhs.node && nil == rhs.nil;
    }

    bool operator!=(const iterator &rhs) const {
      return !(*this == rhs);
    }

    bool operator!=(const const_iterator &rhs) const {
      return !(*this == rhs);
    }

    value_type *operator->() const noexcept {
      return node->data;
    }

    friend class map;
    friend class const_iterator;
  };

  class const_iterator {
   private:
    Node *node;
    Node *nil;
    const map *container;

   public:
    const_iterator() : node(nullptr), nil(nullptr), container(nullptr) {}

    const_iterator(Node *n, Node *n_nil, const map *c) : node(n), nil(n_nil), container(c) {}

    const_iterator(const const_iterator &other) : node(other.node), nil(other.nil), container(other.container) {}

    const_iterator(const iterator &other) : node(other.node), nil(other.nil), container(other.container) {}

    const_iterator operator++(int) {
      if (node == nil) {
        throw invalid_iterator();
      }
      const_iterator temp = *this;
      if (node->right != nil) {
        node = node->right;
        while (node->left != nil) {
          node = node->left;
        }
      } else {
        Node *parent = node->parent;
        while (parent != nil && node == parent->right) {
          node = parent;
          parent = parent->parent;
        }
        node = parent;
      }
      return temp;
    }

    const_iterator &operator++() {
      if (node == nil) {
        throw invalid_iterator();
      }
      if (node->right != nil) {
        node = node->right;
        while (node->left != nil) {
          node = node->left;
        }
      } else {
        Node *parent = node->parent;
        while (parent != nil && node == parent->right) {
          node = parent;
          parent = parent->parent;
        }
        node = parent;
      }
      return *this;
    }

    const_iterator operator--(int) {
      const_iterator temp = *this;
      if (node == nil) {
        if (container && container->root != nil) {
          node = container->root;
          while (node->right != nil) {
            node = node->right;
          }
        } else {
          throw invalid_iterator();
        }
      } else {
        if (node->left != nil) {
          node = node->left;
          while (node->right != nil) {
            node = node->right;
          }
        } else {
          Node *parent = node->parent;
          while (parent != nil && node == parent->left) {
            node = parent;
            parent = parent->parent;
          }
          node = parent;
        }
      }
      return temp;
    }

    const_iterator &operator--() {
      if (node == nil) {
        if (container && container->root != nil) {
          node = container->root;
          while (node->right != nil) {
            node = node->right;
          }
        } else {
          throw invalid_iterator();
        }
      } else {
        if (node->left != nil) {
          node = node->left;
          while (node->right != nil) {
            node = node->right;
          }
        } else {
          Node *parent = node->parent;
          while (parent != nil && node == parent->left) {
            node = parent;
            parent = parent->parent;
          }
          node = parent;
        }
      }
      return *this;
    }

    const value_type &operator*() const {
      return *(node->data);
    }

    bool operator==(const iterator &rhs) const {
      return node == rhs.node && nil == rhs.nil;
    }

    bool operator==(const const_iterator &rhs) const {
      return node == rhs.node && nil == rhs.nil;
    }

    bool operator!=(const iterator &rhs) const {
      return !(*this == rhs);
    }

    bool operator!=(const const_iterator &rhs) const {
      return !(*this == rhs);
    }

    const value_type *operator->() const noexcept {
      return node->data;
    }

    friend class map;
  };

  map() : cmp(Compare()) {
    init();
  }

  map(const map &other) : cmp(other.cmp) {
    init();
    root = copyTree(other.root, other.nil, nil);
    num_elements = other.num_elements;
  }

  map &operator=(const map &other) {
    if (this != &other) {
      clear();
      root = copyTree(other.root, other.nil, nil);
      num_elements = other.num_elements;
    }
    return *this;
  }

  ~map() {
    clearNode(root);
    delete nil;
  }

  T &at(const Key &key) {
    Node *node = findNode(key);
    if (node == nil) {
      throw index_out_of_bound();
    }
    return node->data->second;
  }

  const T &at(const Key &key) const {
    Node *node = findNode(key);
    if (node == nil) {
      throw index_out_of_bound();
    }
    return node->data->second;
  }

  T &operator[](const Key &key) {
    Node *node = findNode(key);
    if (node != nil) {
      return node->data->second;
    }
    value_type new_pair(key, T());
    pair<iterator, bool> result = insert(new_pair);
    return result.first->second;
  }

  const T &operator[](const Key &key) const {
    return at(key);
  }

  iterator begin() {
    if (root == nil) {
      return iterator(nil, nil, this);
    }
    return iterator(treeMinimum(root), nil, this);
  }

  const_iterator cbegin() const {
    if (root == nil) {
      return const_iterator(nil, nil, this);
    }
    return const_iterator(treeMinimum(root), nil, this);
  }

  iterator end() {
    return iterator(nil, nil, this);
  }

  const_iterator cend() const {
    return const_iterator(nil, nil, this);
  }

  bool empty() const {
    return num_elements == 0;
  }

  size_t size() const {
    return num_elements;
  }

  void clear() {
    clearNode(root);
    root = nil;
    num_elements = 0;
  }

  pair<iterator, bool> insert(const value_type &value) {
    Node *y = nil;
    Node *x = root;
    while (x != nil) {
      y = x;
      if (cmp(value.first, x->data->first)) {
        x = x->left;
      } else if (cmp(x->data->first, value.first)) {
        x = x->right;
      } else {
        return pair<iterator, bool>(iterator(x, nil, this), false);
      }
    }
    Node *z = new Node(value);
    z->parent = y;
    if (y == nil) {
      root = z;
    } else if (cmp(value.first, y->data->first)) {
      y->left = z;
    } else {
      y->right = z;
    }
    z->left = nil;
    z->right = nil;
    z->color = RED;
    insertFixup(z);
    num_elements++;
    return pair<iterator, bool>(iterator(z, nil, this), true);
  }

  void erase(iterator pos) {
    if (pos.node == nil || pos.nil != nil) {
      throw invalid_iterator();
    }
    Node *z = pos.node;
    Node *y = z;
    Node *x;
    Color y_original_color = y->color;
    if (z->left == nil) {
      x = z->right;
      transplant(z, z->right);
    } else if (z->right == nil) {
      x = z->left;
      transplant(z, z->left);
    } else {
      y = treeMinimum(z->right);
      y_original_color = y->color;
      x = y->right;
      if (y->parent == z) {
        x->parent = y;
      } else {
        transplant(y, y->right);
        y->right = z->right;
        y->right->parent = y;
      }
      transplant(z, y);
      y->left = z->left;
      y->left->parent = y;
      y->color = z->color;
    }
    delete z;
    if (y_original_color == BLACK) {
      deleteFixup(x);
    }
    num_elements--;
  }

  size_t count(const Key &key) const {
    return findNode(key) != nil ? 1 : 0;
  }

  iterator find(const Key &key) {
    Node *node = findNode(key);
    return iterator(node, nil, this);
  }

  const_iterator find(const Key &key) const {
    Node *node = findNode(key);
    return const_iterator(node, nil, this);
  }
};

}

#endif
