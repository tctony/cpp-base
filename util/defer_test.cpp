#include "defer.h"

#include <gtest/gtest.h>

TEST(defer, defer) {
  int value = 1;
  {
    DEFER { value += 1; };

    EXPECT_EQ(value, 1);
  }

  EXPECT_EQ(value, 2);
}
