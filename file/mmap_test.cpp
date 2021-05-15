#include "mmap.h"

#include <array>
#include <gtest/gtest.h>

#include "base/util/common.h"

using namespace base::file;

TEST(MMap, map) {
  size_t n = 1000;

  {
    auto fd = FileDescriptor("/tmp/mmaptest", O_RDWR | O_CREAT);
    char v = '1';
    for (int i = 0; i < n; ++i) {
      write(fd.get(), &v, 1);
    }
    fd.reset();
  }

  {
    auto mm = MMap("/tmp/mmaptest", O_RDWR);
    EXPECT_EQ(mm.size(), n);
    auto ptr = mm.begin();
    for (int i = 0; i < n; ++i) {
      EXPECT_EQ(*ptr, '1');
      *ptr = '2';
      ptr++;
    }
  }

  {
    auto fd = FileDescriptor("/tmp/mmaptest", O_RDONLY);
    char v;
    read(fd.get(), &v, 1);
    EXPECT_EQ(v, '2');
  }
}

TEST(MMap, memoryBuffer) {
  constexpr size_t n = 1000;
  std::array<char, n> buffer;
  __FOR(i, n) { buffer[i] = '1'; }
  auto span = absl::Span<char>(buffer.begin(), n);
  auto memoryBuffer = FileMemoryBuffer(span);
  auto view = memoryBuffer.view(1, n);
  EXPECT_EQ(view.size(), n - 1);

  auto ptr = view.begin();
  __FOR(i, view.size()) {
    *ptr = '2';
    ++ptr;
  }

  __FOR(i, n) { EXPECT_EQ(buffer[i], i == 0 ? '1' : '2'); }
}

TEST(MMap, fileMemoryBuffer) {
  constexpr size_t n = 1000;
  auto fd = FileDescriptor("/tmp/filememorybuffertest", O_RDWR | O_CREAT);
  char v = '1';
  __FOR(i, n) { write(fd.get(), &v, 1); }
  fd.reset();

  auto mmap = MMap("/tmp/filememorybuffertest", O_RDWR);
  EXPECT_EQ(mmap.size(), n);

  auto fileMemoryBuffer = FileMemoryBuffer(std::move(mmap));
  auto view = fileMemoryBuffer.view(1, n);
  EXPECT_EQ(view.size(), n - 1);

  auto ptr = view.begin();
  __FOR(i, view.size()) {
    *ptr = '2';
    ++ptr;
  }

  std::array<char, n> buffer;
  auto fd2 = FileDescriptor("/tmp/filememorybuffertest", O_RDONLY);
  int nr = read(fd2.get(), buffer.data(), n);
  EXPECT_EQ(nr, n);
  __FOR(i, n) { EXPECT_EQ(buffer[i], i == 0 ? '1' : '2'); }
}
