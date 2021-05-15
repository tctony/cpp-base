#include "random.h"

#include <gtest/gtest.h>

using base::util::genRandomID;

TEST(random, random_id) {
  auto r1 = genRandomID();
  EXPECT_EQ(r1.size(), 16);
  auto r2 = genRandomID();
  EXPECT_NE(r1, r2);
}
