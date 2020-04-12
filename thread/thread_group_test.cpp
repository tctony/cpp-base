#include "thread_group.hpp"

#include <gtest/gtest.h>

#include <iostream>

using base::ThreadGroup;

TEST(thread_group, create) {
  ThreadGroup tg;
  tg.createThread([] {
    for (int i = 0; i < 3; ++i) {
      std::cout << i << std::endl;
      sleep(1);
    }
  });
}

TEST(thread_group, creates) {
  ThreadGroup tg;
  tg.createThreads(
      [] {
        for (int i = 0; i < 3; ++i) {
          std::cout << i << std::endl;
          sleep(1);
        }
      },
      3);
}