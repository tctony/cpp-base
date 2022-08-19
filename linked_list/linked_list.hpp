#if !defined(__BASE_LIKED_LIST_LINKED_LIST_HPP__)
#define __BASE_LIKED_LIST_LINKED_LIST_HPP__

#include <cstddef>
#include <type_traits>

namespace base {

template <typename T> class LinkedListNodeAdaptor {
public:
  // instantiation should implement following methods
  virtual T *next(T *ptr) = 0;
  virtual bool compare(T *left, T *right) = 0;
};

template <typename T> class LinkedList {
private:
  using ThisClass = LinkedList<T>;

public:
  static inline LinkedListNodeAdaptor<T> adaptor;

  static size_t length(T *ptr) {
    size_t len = 0;
    while (ptr != nullptr) {
      ++len;
      ptr = ThisClass::adaptor.next(ptr);
    }
    return len;
  }
};

} // namespace base

#endif // __BASE_LIKED_LIST_LINKED_LIST_HPP__
