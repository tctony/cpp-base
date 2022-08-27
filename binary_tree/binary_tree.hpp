#if !defined(__BASE_BINARY_TREE_BINARY_TREE_HPP__)
#define __BASE_BINARY_TREE_BINARY_TREE_HPP__

#include <queue>
#include <tuple>
#include <vector>

namespace base {

template <typename T> class BinaryTreeNodeInterface {
public:
  virtual T *alloc() { return new T(); }
  virtual void dealloc(T *ptr) {
    if (ptr != nullptr)
      delete ptr;
  }
  // instantiation should implement following methods
  virtual T *left(const T *ptr) const = 0;
  virtual void setLeft(T *ptr, T *left) = 0;
  virtual T *right(const T *ptr) const = 0;
  virtual void setRight(T *ptr, T *right) = 0;
  virtual bool less(const T *left, const T *right) const = 0;
  virtual bool greater(const T *left, const T *right) const = 0;
  virtual bool equal(const T *left, const T *right) const = 0;
};

template <typename T>
class BinaryTreeNodeAdaptor : public BinaryTreeNodeInterface<T> {
public:
  using Pointer = T *;
};

class BinaryTreeTravelContext {
public:
  std::vector<bool> path;
  BinaryTreeTravelContext() = default;
  BinaryTreeTravelContext(const BinaryTreeTravelContext &) = delete;
  BinaryTreeTravelContext &operator=(const BinaryTreeTravelContext &) = delete;
};

class BinaryTreeDFSTravelContext : public BinaryTreeTravelContext {
public:
  BinaryTreeDFSTravelContext() = default;

  virtual void moveToLeftChild() { path.push_back(false); }
  virtual void moveToRightChild() { path.push_back(true); }
  virtual void moveToParent() {
    if (path.size())
      path.pop_back();
  }
};

class BinaryTreeBFSTravelContext : public BinaryTreeTravelContext {
public:
  BinaryTreeBFSTravelContext() = default;
};

template <typename T> class BinaryTree {
private:
  using ThisClass = BinaryTree<T>;
  using AdaptorClass = BinaryTreeNodeAdaptor<T>;

public:
  static inline AdaptorClass adaptor;
  using NodePointer = typename AdaptorClass::Pointer;
  using DFSTravelContext = BinaryTreeDFSTravelContext;
  using BFSTravelContext = BinaryTreeBFSTravelContext;
  template <typename C>
  using NodeVisitor = std::function<void(NodePointer, C &)>;
  template <typename C>
  using NodePredicator = std::function<bool(NodePointer, C &)>;

#define BTA ThisClass::adaptor

  template <typename C>
  static void preOrderTravel(NodePointer root, C &ctx,
                             const NodeVisitor<C> &visitor) {
    ThisClass::dfsTravelWithVisitors<C>(root, ctx, visitor);
  }

  template <typename C>
  static void midOrderTravel(NodePointer root, C &ctx,
                             const NodeVisitor<C> &visitor) {
    ThisClass::dfsTravelWithVisitors<C>(root, ctx, {}, visitor);
  }

  template <typename C>
  static void postOrderTravel(NodePointer root, C &ctx,
                              const NodeVisitor<C> &visitor) {
    ThisClass::dfsTravelWithVisitors<C>(root, ctx, {}, {}, visitor);
  }

  template <typename C, std::enable_if_t<std::is_base_of_v<DFSTravelContext, C>>
                            * = nullptr>
  static void dfsTravelWithVisitors(NodePointer root, C &ctx,
                                    const NodeVisitor<C> &preVisitor = {},
                                    const NodeVisitor<C> &midVisitor = {},
                                    const NodeVisitor<C> &postVisitor = {}) {
    if (!root)
      return;

    if (preVisitor)
      preVisitor(root, ctx);

    auto left = BTA.left(root);
    if (left) {
      ctx.moveToLeftChild();
      dfsTravelWithVisitors(left, ctx, preVisitor, midVisitor, postVisitor);
      ctx.moveToParent();
    }

    if (midVisitor)
      midVisitor(root, ctx);

    auto right = BTA.right(root);
    if (right) {
      ctx.moveToRightChild();
      dfsTravelWithVisitors(right, ctx, preVisitor, midVisitor, postVisitor);
      ctx.moveToParent();
    }

    if (postVisitor)
      postVisitor(root, ctx);
  }

  template <typename C, std::enable_if_t<std::is_base_of_v<BFSTravelContext, C>>
                            * = nullptr>
  static void bfsTravel(NodePointer root, C &ctx,
                        const NodeVisitor<C> &visitor) {
    if (!root)
      return;

    using NodeInfo = std::tuple<NodePointer, std::vector<bool>>;

    std::queue<NodeInfo> queue;
    queue.push(std::make_tuple(root, std::vector<bool>{}));

    while (!queue.empty()) {
      NodePointer node;
      std::vector<bool> path;
      std::tie(node, path) = queue.front();
      queue.pop();

      if (visitor) {
        ctx.path = path;
        visitor(node, ctx);
      }

      if (node->left) {
        auto newPath = path;
        newPath.push_back(false);
        queue.push(std::make_tuple(node->left, newPath));
      }

      if (node->right) {
        auto newPath = path;
        newPath.push_back(true);
        queue.push(std::make_tuple(node->right, newPath));
      }
    }
  }
};

#undef BTA

} // namespace base

#endif // __BASE_BINARY_TREE_BINARY_TREE_HPP__
