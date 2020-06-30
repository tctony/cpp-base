#if !defined(UTIL_TIME_UTIL_H)
#define UTIL_TIME_UTIL_H

#include <chrono>

namespace base {
namespace util {

template <typename Func, typename... Args>
inline uint64_t timeCostInMilliseconds(Func&& f, Args&&... args) {
  auto start = std::chrono::steady_clock::now();
  f(args...);
  auto end = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
      .count();
}

template <typename Func, typename... Args>
inline uint64_t timeCostInMicroseconds(Func&& f, Args&&... args) {
  auto start = std::chrono::steady_clock::now();
  f(args...);
  auto end = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<std::chrono::microseconds>(end - start)
      .count();
}

}  // namespace util
}  // namespace base

#endif  // UTIL_TIME_UTIL_H
