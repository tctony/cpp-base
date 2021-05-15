#if !defined(__BASE_FILE_FILE_DESCRIPTOR_H__)
#define __BASE_FILE_FILE_DESCRIPTOR_H__

#include <fcntl.h>
#include <unistd.h>

#include "base/error/except.h"
#include "base/util/disablecopy.h"

namespace base::file {

class FileDescriptor {
public:
  explicit FileDescriptor(const char *filename, int oflag = O_RDONLY)
      : fd_(open(filename, oflag, 0600)), own_(true) {
    ENFORCE(fd_ != -1) << "can't open file " << filename << " with oflag "
                       << oflag << " errno " << errno << " " << strerror(errno);
  }

  FileDescriptor(int fd, bool own) : fd_(fd), own_(own) {}

  ~FileDescriptor() { reset(); }

  [[nodiscard]] int get() const { return fd_; }

  void reset() {
    if (fd_ > -1 && own_) {
      close(fd_);
    }
    fd_ = -1;
    own_ = false;
  }

  CLASS_DISABLE_COPY(FileDescriptor);

  FileDescriptor(FileDescriptor &&o) : fd_(o.fd_), own_(o.own_) {
    o.fd_ = -1;
    o.own_ = false;
  }

private:
  int fd_{-1};
  bool own_{false};
};

} // namespace base::file

#endif // __BASE_FILE_FILE_DESCRIPTOR_H__
