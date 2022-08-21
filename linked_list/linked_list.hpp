#if !defined(__BASE_LIKED_LIST_LINKED_LIST_HPP__)
#define __BASE_LIKED_LIST_LINKED_LIST_HPP__

#include <cstddef>
#include <functional>
#include <optional>
#include <queue>
#include <tuple>
#include <type_traits>
#include <vector>

namespace base {
template <typename T> class LinkedListNodeInterface {
public:
  virtual T *alloc() { return new T(); }
  virtual void dealloc(T *ptr) {
    if (ptr == nullptr)
      return;
    delete ptr;
  }
  // instantiation should implement following methods
  virtual T *next(const T *ptr) const = 0;
  virtual void setNext(T *ptr, T *next) const = 0;
  virtual bool less(const T *left, const T *right) const = 0;
  virtual bool greater(const T *left, const T *right) const = 0;
  virtual bool equal(const T *left, const T *right) const = 0;
};

template <typename T>
class LinkedListNodeAdaptor : public LinkedListNodeInterface<T> {
  using Pointer = T *;
};

using std::placeholders::_1;
using std::placeholders::_2;

template <typename T> class LinkedList {
private:
  using ThisClass = LinkedList<T>;
  using AdaptorClass = LinkedListNodeAdaptor<T>;

public:
  static inline AdaptorClass adaptor;
  using NodePointer = typename AdaptorClass::Pointer;
  using NodeVisitor = std::function<void(T *)>;
  using NodePredicator = std::function<bool(T *)>;

#define LLA ThisClass::adaptor

  static T *createList(int n, const std::vector<NodeVisitor> &visitors = {}) {
    T foo;
    T *tail = &foo;
    for (int i = 0; i < n; ++i) {
      auto ptr = LLA.alloc();
      LLA.setNext(tail, ptr);
      tail = LLA.next(tail);

      if (i < visitors.size())
        visitors[i](ptr);
    }
    return LLA.next(&foo);
  }
  static T *createList(const std::vector<NodeVisitor> &visitors) {
    return ThisClass::createList(visitors.size(), visitors);
  }

  static void destroyList(T *list) {
    while (list != nullptr) {
      auto ptr = list;
      list = LLA.next(list);
      LLA.dealloc(ptr);
    }
  }

  //-----

  static void forEach(T *list, NodeVisitor visitor) {
    auto ptr = list;
    while (ptr != nullptr) {
      visitor(ptr);
      ptr = LLA.next(ptr);
    }
  }

  static T *findFirst(T *list, NodePredicator predicator) {
    auto ptr = list;
    while (ptr != nullptr) {
      if (predicator(ptr)) {
        return ptr;
      } else {
        ptr = LLA.next(ptr);
      }
    }
    return nullptr;
  }

  static size_t length(T *list) {
    size_t len = 0;
    ThisClass::forEach(list, [&len](T *) { ++len; });
    return len;
  }

  static T *getTail(T *list) {
    T *tail = nullptr;
    ThisClass::forEach(list, [&tail](T *node) { tail = node; });
    return tail;
  }

  // start from 0th
  static T *getNth(T *list, size_t n) {
    size_t count = n;
    return ThisClass::findFirst(list, [&count](T *node) { return !(count--); });
  }

  static std::tuple<NodePointer, size_t, NodePointer, size_t>
  fastSlowWalk(NodePointer list) {
    if (list == nullptr)
      return {nullptr, 0, nullptr, 0};
    if (LLA.next(list) == nullptr)
      return {list, 1, nullptr, 0};

    NodePointer fast = LLA.next(list), slow = list;
    size_t fastStep = 2, slowStep = 1;
    while (true) {
      if (fast == slow)
        break;

      fast = LLA.next(fast);
      fastStep += 1;
      if (fast == nullptr)
        break;

      slow = LLA.next(slow);
      slowStep += 1;
      fast = LLA.next(fast);
      fastStep += 1;
      if (fast == nullptr)
        break;
    }
    return {fast, fastStep, slow, slowStep};
  }

  static bool hasLoop(NodePointer list) {
    NodePointer fast, slow;
    std::tie(fast, std::ignore, slow, std::ignore) = fastSlowWalk(list);
    if (slow == nullptr)
      return false;
    if (fast == nullptr)
      return false;
    return true;
  }

  static std::optional<std::tuple<NodePointer, size_t>>
  getLoopEntry(NodePointer list) {
    NodePointer fast, slow;
    size_t slowStep = 0;
    std::tie(fast, std::ignore, slow, slowStep) = fastSlowWalk(list);
    if (slow == nullptr)
      return {};
    if (fast == nullptr)
      return {};
    if (slowStep == 1)
      return std::make_tuple(list, 1);

    size_t count = 1;
    auto ptr = list;
    slow = LLA.next(slow);
    while (ptr != slow) {
      ++count;
      ptr = LLA.next(ptr);
      slow = LLA.next(slow);
    }
    return std::make_tuple(ptr, count);
  }

  static size_t getLoopLength(NodePointer loopEntry) {
    size_t length = 0;
    ThisClass::findFirst(LLA.next(loopEntry), [&](NodePointer node) {
      length += 1;
      return node == loopEntry;
    });
    return length;
  }

  //-----

  static T *removeNext(T *list) {
    if (list == nullptr)
      return nullptr;

    auto next = LLA.next(list);
    if (next == nullptr)
      return nullptr;

    LLA.setNext(list, LLA.next(next));
    LLA.setNext(next, nullptr);
    return next;
  }

  //-----

  static bool equal(const T *list, const T *otherList) {
    const T *p = list, *q = otherList;
    while (p != nullptr && q != nullptr) {
      if (!LLA.equal(p, q))
        return false;
      p = LLA.next(p);
      q = LLA.next(q);
    }
    return LLA.equal(p, q);
  }

  static T *mergeOrderedList(T *list, T *otherList) {
    return ThisClass::mergeOrderedList({list, otherList});
  }

  static T *mergeOrderedList(const std::vector<T *> &lists) {
    T foo;
    T *tail = &foo;

    auto cmp = std::bind(&AdaptorClass::greater, &LLA, _1, _2);
    std::priority_queue<T *, std::vector<T *>, decltype(cmp)> queue(cmp);
    int count = 0;
    for (auto p : lists) {
      if (p != nullptr) {
        queue.push(p);
        ++count;
      }
    }

    while (!queue.empty()) {
      auto p = queue.top();
      queue.pop();

      LLA.setNext(tail, p);
      if (count == 1) {
        break;
      }

      tail = p;
      if (LLA.next(tail) != nullptr) {
        queue.push(LLA.next(tail));
        LLA.setNext(tail, nullptr);
      } else {
        --count;
      }
    }

    return foo.next;
  }

  static std::tuple<T *, T *> partition(T *list, NodePredicator predicator) {
    T hit, miss;
    T *itr = list, *hitTail = &hit, *missTail = &miss;

    while (itr != nullptr) {
      auto &targetTail = predicator(itr) ? hitTail : missTail;
      LLA.setNext(targetTail, itr);
      targetTail = itr;
      itr = LLA.next(itr);
      LLA.setNext(targetTail, nullptr);
    }

    return std::make_tuple(LLA.next(&hit), LLA.next(&miss));
  }

  static T *concat(T *list, T *otherList) {
    return ThisClass::concat({list, otherList});
  }
  static T *concat(const std::vector<T *> &lists) {
    if (lists.size() == 0)
      return nullptr;
    T *head = lists[lists.size() - 1];
    for (int index = lists.size() - 2; index >= 0; --index) {
      auto cur = lists[index];
      if (cur != nullptr) {
        auto tail = ThisClass::getTail(cur);
        LLA.setNext(tail, head);
        head = cur;
      }
    }
    return head;
  }

#undef LLA
};

} // namespace base

#endif // __BASE_LIKED_LIST_LINKED_LIST_HPP__
