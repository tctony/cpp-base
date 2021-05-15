#include "random.h"

#include <random>
#include <sstream>

namespace base::util {

static std::random_device rd;
static std::mt19937 mt(rd());
int64_t mt19937Rand() { return static_cast<int64_t>(mt()); }

std::string genRandomID(size_t len) {
  std::ostringstream oss;
  for (size_t i = 0; i < len; ++i) {
    oss << (static_cast<char>('A' + (mt19937Rand() % 26)));
  }
  return oss.str();
}

} // namespace base::util
