#if !defined(BASE_ERROR_ERROR_HPP)
#define BASE_ERROR_ERROR_HPP

#include <array>
#include <iostream>
#include <limits>
#include <system_error>

#include "absl/strings/str_cat.h"
#include "base/util/disablecopy.h"

namespace base {
namespace error {

using Error = std::error_condition;

enum ErrorCode : int {
  noError = 0,
  unknownError = std::numeric_limits<int>::max(),
};
}  // namespace error
}  // namespace base

namespace std {
template <>
struct is_error_condition_enum<base::error::ErrorCode> : true_type {};
}  // namespace std

namespace base {
namespace error {

class ErrorCategory : public std::error_category {
 public:
  constexpr ErrorCategory() {}

  CLASS_DISABLE_COPY(ErrorCategory);

  virtual const char* name() const noexcept override {
    return "base::error::ErrorCategory";
  }

  virtual std::string message(int value) const override {
    if (value == noError) {
      return "no error";
    }
    if (value == unknownError) {
      return "unknown error";
    } else {
      return absl::StrCat("unspecified ", this->name(), " error: ", value);
    }
  }

  virtual bool equivalent(const std::error_code& code,
                          int condition) const noexcept override {
    if (code.category() == std::system_category() ||
        code.category() == std::generic_category()) {
      return code.value() == condition;
    }
    return false;
  }
};

extern const std::error_category& getErrorCategroy();

inline std::error_condition make_error_condition(ErrorCode ec) {
  return std::error_condition(static_cast<int>(ec), getErrorCategroy());
}

}  // namespace error
}  // namespace base

#endif  // BASE_ERROR_ERROR_HPP
