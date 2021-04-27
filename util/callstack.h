#if !defined(__BASE_UTIL_CALLSTACK__)
#define __BASE_UTIL_CALLSTACK__

#include <iomanip>
#include <memory>
#include <sstream>
#include <string>

#include "absl/debugging/stacktrace.h"
#include "absl/debugging/symbolize.h"

namespace base::util {

class CallStack {
  enum { kStackSize = 40 };

 public:
  CallStack(int skip_count = 2) {
    size_ = absl::GetStackTrace(stack_, kStackSize, skip_count);
  }

  std::string debugString() {
    std::ostringstream oss;
    oss << "Callstack: \n";
    for (int i = 0; i < size_; ++i) {
      char tmp[1024];
      const char* symbol = "(unknown)";
      if (absl::Symbolize(stack_[i], tmp, sizeof(tmp))) {
        symbol = tmp;
      }
      oss << std::setw(2) << i << " " << std::setw(18) << stack_[i] << " " << symbol << "\n";
    }
    oss << "\n";
    return oss.str();
  }

  static thread_local std::unique_ptr<CallStack> currentExceptionCallstack;

 private:
  void *stack_[kStackSize];
  int size_;
};

}  // namespace base::util

#endif  // __BASE_UTIL_CALLSTACK__
