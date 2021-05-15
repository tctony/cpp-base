#include "file_descriptor.h"

#include <unistd.h>

#include <gtest/gtest.h>

using namespace base::file;

TEST(FileDescriptor, open_exist) {
  system("touch /tmp/fdtest");
  auto fd = FileDescriptor("/tmp/fdtest");
}

TEST(FileDescriptor, open_create) {
  system("rm -f /tmp/fdtest2");
  auto fd = FileDescriptor("/tmp/fdtest2", O_RDWR | O_CREAT);
  std::string buf = "hello";
  write(fd.get(), buf.c_str(), buf.size());
}

TEST(FileDescriptor, open_throw) {
  EXPECT_THROW(FileDescriptor("/test"), std::exception);
}
