#if !defined(__BASE_UTIL_DEFER_H__)
#define __BASE_UTIL_DEFER_H__

#include <memory>

#include "base/util/define.h"
#include "base/util/disablecopy.h"

namespace base::util {

template <typename F> class DeferGuard {
public:
  explicit DeferGuard(F &&f) : f_(std::move_if_noexcept(f)) {}

  CLASS_DISABLE_COPY(DeferGuard);

  DeferGuard(DeferGuard &&o) noexcept : f_(std::move_if_noexcept(o.f_)) {
    o.dismissed_ = true;
  }

  ~DeferGuard() {
    if (!dismissed_) {
      f_();
    }
  }

private:
  bool dismissed_{false};
  F f_;
};

enum class DeferHelper {};
template <typename F> DeferGuard<F> operator+(DeferHelper, F &&f) {
  return DeferGuard<F>(std::move_if_noexcept(f));
}

} // namespace base::util

#define DEFER                                                                  \
  auto _ANONYMOUS_VARIABLE(defer_guard) = ::base::util::DeferHelper() + [&]()

#endif // __BASE_UTIL_DEFER_H__
