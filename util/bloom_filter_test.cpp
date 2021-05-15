#include "bloom_filter.h"

#include <gtest/gtest.h>

#include "base/util/random.h"

using base::util::genRandomID;
using base::util::MMapBasedBloomFilter;

#define BLOOM_FILTER_FILE "/tmp/bloom_filter_file"

TEST(bloom_filter, bloom_filter) {
  system("rm -rf " BLOOM_FILTER_FILE);
  MMapBasedBloomFilter filter(BLOOM_FILTER_FILE,
                              MMapBasedBloomFilter::Config());
  std::vector<std::string> testCases;
  testCases.reserve(1000000);
  for (int i = 0; i < 1000000; ++i) {
    testCases.emplace_back(genRandomID(8));
  }
  for (auto &test : testCases) {
    EXPECT_FALSE(filter.lookUp(test));
  }
  {
    for (auto &test : testCases) {
      filter.add(test);
    }
  }
  for (auto &test : testCases) {
    EXPECT_TRUE(filter.lookUp(test));
  }

  {
    int count = 0;
    for (int i = 0; i < 1000000; ++i) {
      if (filter.lookUp(genRandomID(9))) {
        ++count;
      }
    }
    // 0.01%, but why?
    EXPECT_TRUE(count < 1000000 * 0.0001);
  }
}
