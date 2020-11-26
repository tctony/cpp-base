#if !defined(UTIL_TIME_UTIL_H)
#define UTIL_TIME_UTIL_H

#include <chrono>
#include <string>

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

template <typename Func, typename... Args>
inline uint64_t timeCostInNanoseconds(Func&& f, Args&&... args) {
  auto start = std::chrono::steady_clock::now();
  f(args...);
  auto end = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
      .count();
}

// a simple clock to count time interval
class SimpleClock {
 public:
  SimpleClock(const std::string& name, bool autoStart = true)
      : name_(name), state_(not_started) {
    if (autoStart) {
      start();
    }
  }

  const std::string& name() { return name_; }

  void start() {
    assert(state_ == not_started && "can't start clock twice");
    start_ = std::chrono::steady_clock::now();
    state_ = running;
  }

  template <typename T = std::chrono::milliseconds>
  uint64_t elapsed() {
    assert(state_ == running &&
           "can't get elapsed time interval when clock is not running");
    return std::chrono::duration_cast<T>(std::chrono::steady_clock::now() -
                                         start_)
        .count();
  }

  template <typename T = std::chrono::milliseconds>
  uint64_t end() {
    assert(state_ == running &&
           "can't not end clock when clock is not running");
    end_ = std::chrono::steady_clock::now();
    state_ = ended;
    return cost<T>();
  }

  template <typename T = std::chrono::milliseconds>
  uint64_t cost() {
    assert(state_ == ended &&
           "can't get cost time interval when clock is not ended");
    return std::chrono::duration_cast<T>(std::chrono::steady_clock::now() -
                                         start_)
        .count();
  }

 private:
  const std::string name_;
  enum {
    not_started = 0,
    running,
    ended,
  } state_;
  std::chrono::steady_clock::time_point start_;
  std::chrono::steady_clock::time_point end_;
};

}  // namespace util
}  // namespace base

#endif  // UTIL_TIME_UTIL_H
