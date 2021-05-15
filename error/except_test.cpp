#include "except.h"

#include <gtest/gtest.h>

#include <chrono>

using base::error::Exception;

TEST(except, enforce) {
  EXPECT_NO_THROW(ENFORCE(true) << "enforce must not fail");
  EXPECT_THROW(ENFORCE(false) << "enforce must fail", Exception);
}

TEST(except, throw) {
  try {
    THROW() << "test throw";
  } catch (Exception &e) {
    std::cerr << e.what() << std::endl;
  }
}

int test_if_branch(bool flag) {
  int i = 0;
  if (flag) {
    i += 1;
  }
  return i;
}

int test_if_branch_with_likely(bool flag) {
  int i = 0;
  if (LIKELY(flag)) {
    i += 1;
  }
  return i;
}

// bazel run -c opt :except_test -- --gtest_filter=except.likely
// 貌似并没有变快
TEST(except, likely) {
  bool test[4][2] = {
      {true, true},
      {true, false},
      {false, true},
      {false, false},
  };
  for (int i = 0; i < 4; ++i) {
    auto branch = test[i][0];
    auto likely = test[i][1];
    auto f = test_if_branch;
    if (likely) {
      f = test_if_branch_with_likely;
    }
    auto start = std::chrono::steady_clock::now();
    for (int j = 0; j < 1e8; ++j) {
      f(branch);
    }
    auto end = std::chrono::steady_clock::now();
    std::cout << "branch " << branch << ", likely " << likely << ", cost "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       start)
                     .count()
              << "ms\n";
  }
}
