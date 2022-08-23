#if !defined(__BASE_BINARY_TREE_BINARY_TREE_HPP__)
#define __BASE_BINARY_TREE_BINARY_TREE_HPP__

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
  virtual void moveToLeftChild() { path.push_back(false); }
  virtual void moveToRightChild() { path.push_back(true); }
  virtual void moveToParent() {
    if (path.size())
      path.pop_back();
  }
};

template <typename T> class BinaryTree {
private:
  using ThisClass = BinaryTree<T>;
  using AdaptorClass = BinaryTreeNodeAdaptor<T>;

public:
  static inline AdaptorClass adaptor;
  using NodePointer = typename AdaptorClass::Pointer;
  using TravelContext = BinaryTreeTravelContext;
  template <typename C>
  using NodeVisitor = std::function<void(NodePointer, C &)>;
  template <typename C>
  using NodePredicator = std::function<bool(NodePointer, C &)>;

#define BTA ThisClass::adaptor

  template <typename C>
  static void preOrderTravel(NodePointer root, C &ctx, NodeVisitor<C> visitor) {
    ThisClass::travelWithOrder<C>(ePreOrderTravel, root, ctx, visitor);
  }

  template <typename C>
  static void midOrderTravel(NodePointer root, C &ctx, NodeVisitor<C> visitor) {
    ThisClass::travelWithOrder<C>(eMidOrderTravel, root, ctx, visitor);
  }

  template <typename C>
  static void postOrderTravel(NodePointer root, C &ctx,
                              NodeVisitor<C> visitor) {
    ThisClass::travelWithOrder<C>(ePostOrderTravel, root, ctx, visitor);
  }

private:
  enum TravelOrder {
    ePreOrderTravel = 1,
    eMidOrderTravel,
    ePostOrderTravel,
  };

  template <typename C>
  static void travelWithOrder(TravelOrder order, NodePointer root, C &ctx,
                              NodeVisitor<C> visitor) {
    if (root) {
      if (order == ePreOrderTravel)
        visitor(root, ctx);

      auto left = BTA.left(root);
      if (left) {
        ctx.moveToLeftChild();
        travelWithOrder(order, left, ctx, visitor);
        ctx.moveToParent();
      }

      if (order == eMidOrderTravel)
        visitor(root, ctx);

      auto right = BTA.right(root);
      if (right) {
        ctx.moveToRightChild();
        travelWithOrder(order, right, ctx, visitor);
        ctx.moveToParent();
      }

      if (order == ePostOrderTravel)
        visitor(root, ctx);
    }
  }
};

#undef BTA

} // namespace base

#endif // __BASE_BINARY_TREE_BINARY_TREE_HPP__
