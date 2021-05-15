#if !defined(__BASE_FILE_MMAP_H__)
#define __BASE_FILE_MMAP_H__

#include <sys/mman.h>
#include <sys/stat.h>

#include "absl/types/span.h"
#include "absl/types/variant.h"
#include "base/file/file_descriptor.h"
#include "base/util/disablecopy.h"

namespace base::file {

class MMap {
public:
  MMap(std::unique_ptr<FileDescriptor> fd, int prot) : fd_(std::move(fd)) {
    struct stat info {};
    ENFORCE(fstat(fd_->get(), &info) != -1) << "can't fstat fd";
    size_ = info.st_size;
    addr_ = mmap(nullptr, size_, prot, MAP_SHARED, fd_->get(), 0);
    ENFORCE(addr_ != nullptr) << "can't map file, errno: " << errno;
  }

  MMap(const char *filename, int mode)
      : MMap(std::make_unique<FileDescriptor>(filename, mode),
             ((mode & O_WRONLY) || (mode & O_RDWR)) ? PROT_WRITE : PROT_READ) {}

  ~MMap() {
    if (fd_ != nullptr) {
      munmap(addr_, size_);
    }
  }

  CLASS_DISABLE_COPY(MMap);

  MMap(MMap &&o) noexcept
      : fd_(std::move(o.fd_)), addr_(o.addr_), size_(o.size_) {
    o.addr_ = nullptr;
    o.size_ = 0;
  }

  [[nodiscard]] char *begin() const { return reinterpret_cast<char *>(addr_); }
  [[nodiscard]] char *end() const { return begin() + size_; }
  [[nodiscard]] size_t size() const { return size_; }

private:
  std::unique_ptr<FileDescriptor> fd_;
  void *addr_{nullptr};
  size_t size_{0};
};

class FileMemoryBuffer {
public:
  using Var = absl::variant<MMap, absl::Span<char>>;

  explicit FileMemoryBuffer(Var var) : var_(std::move(var)) {}

  CLASS_DISABLE_COPY(FileMemoryBuffer);

  FileMemoryBuffer(FileMemoryBuffer &&o) : var_(std::move(o.var_)) {}

  [[nodiscard]] char *begin() const {
    return absl::visit([](auto &v) { return v.begin(); }, var_);
  }
  [[nodiscard]] char *end() const {
    return absl::visit([](auto &v) { return v.end(); }, var_);
  }
  [[nodiscard]] size_t size() const {
    return absl::visit([](auto &v) { return v.size(); }, var_);
  }

  [[nodiscard]] absl::Span<char> view(size_t pos, size_t n = -1UL) const {
    char *beg = begin() + pos;
    if (beg > end()) {
      return absl::Span<char>();
    }

    char *e;
    if (n == -1UL || pos + n > size()) {
      e = end();
    } else {
      e = beg + n;
    }

    return absl::Span<char>(beg, e - beg);
  }

private:
  Var var_;
};

} // namespace base::file

#endif // __BASE_FILE_MMAP_H__
