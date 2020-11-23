#include "worker.hpp"

#include <gtest/gtest.h>

using base::Worker;

TEST(worker, do_simple_work) {
  Worker w;
  w.context()->post([] {
    for (int i = 0; i < 3; ++i) {
      std::cout << i << std::endl;
      sleep(1);
    }
  });

  sleep(5);
}
