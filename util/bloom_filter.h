#if !defined(__BASE_UTIL_BLOOM_FILTER_H__)
#define __BASE_UTIL_BLOOM_FILTER_H__

#include <string>
#include <vector>

#include "base/util/pimpl.h"

namespace base::util {

class IBloomFilter {
public:
  virtual ~IBloomFilter() = default;
  virtual bool lookUp(std::string_view key) const = 0;
  virtual void add(std::string_view key) = 0;
};

class MMapBasedBloomFilter : IBloomFilter {
public:
  // this default config takes 12M disk space
  struct Config {
    bool create_if_missing_{true};
    uint32_t hash_limit_{32 * 1024 * 1024};
    std::vector<uint64_t> seeds_{63689, 378551, 3};
  };

  MMapBasedBloomFilter(const char *filename, Config cfg);

  bool lookUp(std::string_view key) const override;
  void add(std::string_view key) override;

  PIMPL_DECLARE(MMapBasedBloomFilter);
};

} // namespace base::util

#endif // __BASE_UTIL_BLOOM_FILTER_H__
