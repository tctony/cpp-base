#include "base/linked_list/linked_list.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <numeric>

struct ListNode {
  int val;
  ListNode *next;
  ListNode() : val(0), next(nullptr) {}
  ListNode(int x) : val(x), next(nullptr) {}
  ListNode(int x, ListNode *next) : val(x), next(next) {}
};

namespace base {
template <>
class LinkedListNodeAdaptor<ListNode>
    : public LinkedListNodeInterface<ListNode> {
public:
  using Pointer = ListNode *;

  ListNode *next(const ListNode *ptr) const {
    if (ptr == nullptr)
      return nullptr;
    return ptr->next;
  }

  void setNext(ListNode *ptr, ListNode *next) const {
    if (ptr == nullptr)
      return;
    ptr->next = next;
  }

  bool less(const ListNode *left, const ListNode *right) const {
    return left->val < right->val;
  }

  bool greater(const ListNode *left, const ListNode *right) const {
    return left->val > right->val;
  }

  bool equal(const ListNode *left, const ListNode *right) const {
    if (left == nullptr && right == nullptr)
      return true;
    if (left == nullptr || right == nullptr)
      return false;
    return left->val == right->val;
  }
};
} // namespace base

using LinkedList = typename base::LinkedList<ListNode>;

std::vector<int> iotaNumbers(int n) {
  std::vector<int> nums(n);
  std::iota(nums.begin(), nums.end(), 1);
  return nums;
}

ListNode *listFromNums(const std::vector<int> &nums) {
  std::vector<LinkedList::NodeVisitor> visitors(nums.size());
  std::transform(nums.begin(), nums.end(), visitors.begin(), [](int num) {
    return [=](ListNode *node) { node->val = num; };
  });
  return LinkedList::createList(visitors);
}

void printList(ListNode *head) {
  LinkedList::forEach(head,
                      [](ListNode *node) { std::cout << node->val << " "; });
  std::cout << std::endl;
}

TEST(linked_list, create_destroy) {
  auto nums = iotaNumbers(10);
  auto head = listFromNums(nums);
  ASSERT_TRUE(head != nullptr);
  auto itr = head;
  for (auto num : nums) {
    ASSERT_EQ(itr->val, num);
    itr = itr->next;
  }
  LinkedList::destroyList(head);
}

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

  ASSERT_TRUE(LinkedList::adaptor.less(&n0, &n1));
  ASSERT_FALSE(LinkedList::adaptor.less(&n0, &n0));
  ASSERT_TRUE(LinkedList::adaptor.greater(&n1, &n0));
  ASSERT_FALSE(LinkedList::adaptor.greater(&n0, &n0));
}

TEST(linked_list, eq) {
  ListNode l1[5];
  ListNode l2[5];

  for (int i = 0; i < 5; ++i) {
    l1[i].val = i;
    l1[i].next = (i + 1) < 5 ? (l1 + i + 1) : nullptr;

    l2[i].val = i;
    l2[i].next = (i + 1) < 5 ? (l2 + i + 1) : nullptr;
  }

  ASSERT_TRUE(LinkedList::equal(l1, l2));
  ASSERT_FALSE(LinkedList::equal(l1 + 1, l2));
  ASSERT_TRUE(LinkedList::equal(l1 + 4, l2 + 4));
  ASSERT_FALSE(LinkedList::equal(l1 + 4, (l2 + 4)->next));
  ASSERT_TRUE(LinkedList::equal((l1 + 4)->next, (l2 + 4)->next));
}

TEST(linked_list, merge) {
  ListNode l1[5];
  ListNode l2[5];

  for (int i = 0; i < 5; ++i) {
    l1[i].val = i * 2;
    l1[i].next = (i + 1) < 5 ? (l1 + i + 1) : nullptr;

    l2[i].val = i * 2 + 1;
    l2[i].next = (i + 1) < 5 ? (l2 + i + 1) : nullptr;
  }

  ListNode l3[10];
  for (int i = 0; i < 10; ++i) {
    l3[i].val = i;
    l3[i].next = (i + 1) < 10 ? (l3 + i + 1) : nullptr;
  }

  ListNode *l4 = LinkedList::mergeOrderedList(l1, l2);
  ASSERT_TRUE(LinkedList::equal(l3, l4));
}

TEST(linked_list, partition) {
  auto deleter = &LinkedList::destroyList;

  std::vector<int> nums{1, 4, 3, 2, 5, 2};
  std::unique_ptr<ListNode, decltype(deleter)> head(listFromNums(nums),
                                                    deleter);

  ListNode *hitList, *missList;
  std::tie(hitList, missList) = LinkedList::partition(
      head.release(), [](ListNode *node) { return node->val < 3; });
  head.reset(LinkedList::concat(hitList, missList));

  std::vector<int> results{1, 2, 2, 4, 3, 5};
  std::unique_ptr<ListNode, decltype(deleter)> other(listFromNums(results),
                                                     deleter);

  ASSERT_TRUE(LinkedList::equal(head.get(), other.get()));
}

auto deleter = &LinkedList::destroyList;
using ListUniquePtr =
    std::unique_ptr<ListNode, std::function<void(ListNode *)>>;

TEST(linked_list, removeKthFromEnd) {
  auto nums = iotaNumbers(5);
  ListUniquePtr head(listFromNums(nums), deleter);

  int k = 2; // 删除倒数第二个

  // 头部加上虚拟节点
  ListNode foo;
  LinkedList::concat(&foo, head.get());
  ListNode *p = &foo;
  ListNode *q = LinkedList::getNth(head.get(), k);
  LinkedList::findFirst(q, [&](ListNode *node) {
    p = p->next;
    if (node->next == nullptr) {
      return true;
    }
    return false;
  });
  delete LinkedList::removeNext(p);

  head.release();
  head.reset(foo.next);
  ListUniquePtr result(listFromNums({1, 2, 3, 5}), deleter);
  ASSERT_TRUE(LinkedList::equal(head.get(), result.get()));
}

TEST(linked_list, loop) {
  auto breakLoopBeforeDelete = [=](ListNode *head) {
    auto end = LinkedList::getNth(head, 3);
    end->next = nullptr;
    deleter(head);
  };
  ListUniquePtr head(listFromNums(iotaNumbers(4)), breakLoopBeforeDelete);
  // make loop
  auto end = LinkedList::getNth(head.get(), 3);
  end->next = LinkedList::getNth(head.get(), 1);

  auto entryInfo = LinkedList::getLoopEntry(head.get());
  ASSERT_TRUE(entryInfo.has_value());

  auto loopEntry = std::get<0>(*entryInfo);
  auto entryLength = std::get<1>(*entryInfo);
  auto loopLength = LinkedList::getLoopLength(loopEntry);

  ASSERT_EQ(loopEntry->val, 2);
  ASSERT_EQ(entryLength, 2);
  ASSERT_EQ(loopLength, 3);
}
