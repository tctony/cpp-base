#include "time_util.hpp"

#include <gtest/gtest.h>

#include <iostream>

using base::util::timeCostInMicroseconds;
using base::util::timeCostInMilliseconds;

TEST(time_util, timeCostInMilliseconds) {
  auto cost = timeCostInMilliseconds([] { sleep(1); });
  std::cout << cost << std::endl;

  cost = timeCostInMilliseconds([](int t) { sleep(t); }, 1);
  std::cout << cost << std::endl;

  cost = timeCostInMicroseconds([](int t) { sleep(t); }, 1);
  std::cout << cost << std::endl;
}