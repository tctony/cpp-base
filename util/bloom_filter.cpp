#include "bloom_filter.h"

#include <array>
#include <fstream>

#include "absl/hash/internal/city.h"
#include "base/file/mmap.h"
#include "base/util/define.h"
#include "base/util/disablecopy.h"

namespace base::util {

using file::FileMemoryBuffer;
using file::MMap;

using BLOCK_TYPE = uint8_t;
constexpr size_t BLOCK_BITS_LENGTH = sizeof(BLOCK_TYPE) * 8;

static FileMemoryBuffer loadMemoryBuffer(const char *filename,
                                         uint32_t hash_limit,
                                         const std::vector<uint64_t> &seeds,
                                         bool create_if_missing) {
  ENFORCE(hash_limit % BLOCK_BITS_LENGTH == 0)
      << "hash size error: " << hash_limit;
  ENFORCE(!seeds.empty()) << "seeds empty";
  auto fileSize = (hash_limit / 8) * seeds.size();
  std::ifstream fin(filename);
  if (fin) {
    fin.close();
    auto buf = FileMemoryBuffer(MMap(filename, O_RDWR));
    ENFORCE(buf.size() == fileSize);
    return buf;
  }
  ENFORCE(create_if_missing) << "can't open file " << filename;
  {
    std::ofstream fout(filename);
    std::array<char, 1024 * 1024> block;
    memset(block.data(), 0, block.size());
    size_t n_block = fileSize / block.size();
    size_t remainder = fileSize % block.size();
    _FOR(i, n_block) { fout.write(block.data(), block.size()); }
    if (remainder > 0) {
      fout.write(block.data(), remainder);
    }
    fout.close();
  }
  return loadMemoryBuffer(filename, hash_limit, seeds, false);
}

struct MMapBasedBloomFilter::Impl {

  Impl(const char *filename, Config cfg)
      : buffer_(loadMemoryBuffer(filename, cfg.hash_limit_, cfg.seeds_,
                                 cfg.create_if_missing_)),
        config_(std::move(cfg)) {}

  [[nodiscard]] bool lookUp(std::string_view key) const {
    _FOR(i, config_.seeds_.size()) {
      if (!get(i, absl::hash_internal::CityHash64WithSeed(
                      key.data(), key.size(), config_.seeds_[i]))) {
        return false;
      }
    }
    return true;
  }

  void add(std::string_view key) {
    _FOR(i, config_.seeds_.size()) {
      set(i, absl::hash_internal::CityHash64WithSeed(key.data(), key.size(),
                                                     config_.seeds_[i]));
    }
  }

  [[nodiscard]] bool get(size_t table_idx, uint64_t hash) const {
    auto [block, offset] = seekToBits(table_idx, hash);
    return *block & (1U << offset);
  }

  void set(size_t table_idx, uint64_t hash) {
    auto [block, offset] = seekToBits(table_idx, hash);
    *block |= (1U << offset);
  }

  std::tuple<BLOCK_TYPE * /* pointe to block */, uint32_t /* offset */>
  seekToBits(size_t table_idx, uint64_t hash) const {
    char *begin = buffer_.begin() + config_.hash_limit_ / 8 * table_idx;
    hash %= config_.hash_limit_;
    size_t index = hash / BLOCK_BITS_LENGTH;
    size_t offset = hash % BLOCK_BITS_LENGTH;
    return {&reinterpret_cast<BLOCK_TYPE *>(begin)[index], offset};
  }

  FileMemoryBuffer buffer_;
  Config config_;
};

///---

MMapBasedBloomFilter::MMapBasedBloomFilter(const char *filename, Config cfg)
    : m_(new Impl(filename, cfg)) {}

bool MMapBasedBloomFilter::lookUp(std::string_view key) const {
  return m_->lookUp(key);
}

void MMapBasedBloomFilter::add(std::string_view key) { m_->add(key); }

PIMPL_IMPLEMENT(MMapBasedBloomFilter);

} // namespace base::util
