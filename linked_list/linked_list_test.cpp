#include "base/linked_list/linked_list.hpp"
#include <gtest/gtest.h>

struct ListNode {
  int val;
  ListNode *next;
  ListNode() : val(0), next(nullptr) {}
  ListNode(int x) : val(x), next(nullptr) {}
  ListNode(int x, ListNode *next) : val(x), next(next) {}
};

namespace base {
template <> class LinkedListNodeAdaptor<ListNode> {
public:
  ListNode *next(ListNode *ptr) { return ptr->next; }
  bool compare(ListNode *left, ListNode *right) {
    return left->val <= right->val;
  }
};
} // namespace base

using LinkedList = typename base::LinkedList<ListNode>;

TEST(linked_list, length) {
  ListNode n0, n1, n2;
  n0.next = &n1;
  n1.next = &n2;
  n2.next = nullptr;

  ASSERT_EQ(LinkedList::length(&n0), 3);
  ASSERT_EQ(LinkedList::length(&n1), 2);
  ASSERT_EQ(LinkedList::length(&n2), 1);
  ASSERT_EQ(LinkedList::length(n2.next), 0);
}

TEST(linked_list, compare) {
  ListNode n0(3), n1(5);

  ASSERT_TRUE(LinkedList::adaptor.compare(&n0, &n1));
  ASSERT_TRUE(LinkedList::adaptor.compare(&n0, &n0));
  ASSERT_FALSE(LinkedList::adaptor.compare(&n1, &n0));
}
