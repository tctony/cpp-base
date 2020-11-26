#include "time_util.hpp"

#include <gtest/gtest.h>

#include <iostream>

using base::util::timeCostInMicroseconds;
using base::util::timeCostInMilliseconds;
using base::util::timeCostInNanoseconds;

TEST(time_util, timeCostInMilliseconds) {
  auto cost = timeCostInMilliseconds([] { sleep(1); });
  std::cout << cost << std::endl;

  cost = timeCostInMilliseconds([](int t) { sleep(t); }, 1);
  std::cout << cost << std::endl;

  cost = timeCostInMicroseconds([](int t) { sleep(t); }, 1);
  std::cout << cost << std::endl;

  cost = timeCostInNanoseconds([](int t) { sleep(t); }, 1);
  std::cout << cost << std::endl;
}

TEST(time_util, SimpleClock) {
  auto clock = base::util::SimpleClock("simple_clock");
  sleep(1);
  std::cout << "seconds " << clock.elapsed<std::chrono::seconds>()
            << " miliseconds " << clock.elapsed() << " microseconds "
            << clock.elapsed<std::chrono::microseconds>() << " nanoseconds "
            << clock.elapsed<std::chrono::nanoseconds>() << std::endl;
  sleep(1);
  auto cost = clock.end();
  std::cout << "seconds " << clock.cost<std::chrono::seconds>()
            << " miliseconds " << cost << " microseconds "
            << clock.cost<std::chrono::microseconds>() << " nanoseconds "
            << clock.cost<std::chrono::nanoseconds>() << std::endl;
}
