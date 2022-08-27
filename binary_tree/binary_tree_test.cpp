#include <algorithm>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

#include "base/binary_tree/binary_tree.hpp"

struct TreeNode {
  int val;
  TreeNode *left;
  TreeNode *right;
  TreeNode() : val(0), left(nullptr), right(nullptr) {}
  TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
  TreeNode(int x, TreeNode *left, TreeNode *right)
      : val(x), left(left), right(right) {}
};

namespace base {
template <>
class BinaryTreeNodeAdaptor<TreeNode>
    : public BinaryTreeNodeInterface<TreeNode> {
public:
  using Pointer = TreeNode *;

  TreeNode *left(const TreeNode *ptr) const { return ptr->left; }

  void setLeft(TreeNode *ptr, TreeNode *left) { ptr->left = left; }

  TreeNode *right(const TreeNode *ptr) const { return ptr->right; }

  void setRight(TreeNode *ptr, TreeNode *right) { ptr->right = right; }

  bool less(const TreeNode *left, const TreeNode *right) const {
    return left->val < right->val;
  }

  bool greater(const TreeNode *left, const TreeNode *right) const {
    return left->val > right->val;
  }

  bool equal(const TreeNode *left, const TreeNode *right) const {
    return left->val == right->val;
  }
};
} // namespace base

using BinaryTree = base::BinaryTree<TreeNode>;

TEST(binary_tree, demo) {
  TreeNode *root = nullptr;

  BinaryTree::DFSTravelContext context;
  BinaryTree::preOrderTravel<decltype(context)>(root, context,
                                                [&](TreeNode *node, auto &ctx) {
                                                  // preorder dfs travel
                                                });

  BinaryTree::BFSTravelContext bfsContext;
  BinaryTree::bfsTravel<decltype(bfsContext)>(root, bfsContext,
                                              [&](TreeNode *node, auto &ctx) {
                                                // bfs travel
                                              });
}

// https://leetcode.cn/problems/print-binary-tree/
std::vector<std::vector<std::string>> printTree(TreeNode *root) {
  using NodeInfo = std::tuple<int, std::vector<bool>, int>; // height, path, val
  std::vector<NodeInfo> nodes;
  int height = 0;

  BinaryTree::DFSTravelContext context;
  BinaryTree::preOrderTravel<decltype(context)>(
      root, context, [&](TreeNode *node, auto &ctx) {
        nodes.emplace_back(ctx.path.size(), ctx.path, node->val);
        height = std::max(height, (int)ctx.path.size());
      });

  // 构造字符串矩阵
  int n = (1 << (height + 1)) - 1;
  std::vector<std::vector<std::string>> result(height + 1,
                                               std::vector<std::string>(n, ""));

  for (auto node : nodes) {
    int col = (n - 1) / 2;
    int treeSize = col;
    for (auto isRight : std::get<1>(node)) {
      col += (isRight ? 1 : -1) * (treeSize + 1) / 2;
      treeSize = (treeSize - 1) / 2;
    }

    result[std::get<0>(node)][col] = std::to_string(std::get<2>(node));
  }

  return result;
}
