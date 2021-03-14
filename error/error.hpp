#if !defined(BASE_ERROR_ERROR_HPP)
#define BASE_ERROR_ERROR_HPP

#include <array>
#include <limits>
#include <system_error>
#include <iostream>

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



template <int N=1>
class AdaptiveCategory : public std::error_category {
 public:
  using AdaptedCategories = std::array<const std::error_category*, N>;

  constexpr AdaptiveCategory(AdaptedCategories cates) : categories_(cates) {}

  CLASS_DISABLE_COPY(AdaptiveCategory);

  virtual const char* name() const noexcept override = 0;
  virtual std::string message(int value) const override = 0;

  virtual bool equivalent(const std::error_code& code,
                          int condition) const noexcept override {
    for(auto cate: categories_) {
      if (*cate == code.category()) {
        return condition == code.value();
      }
    }
    return false;
  }

 private:
  AdaptedCategories categories_;
};

extern const std::error_category& defaultCategroy();

inline std::error_condition make_error_condition(ErrorCode ec) {
  return std::error_condition(static_cast<int>(ec), defaultCategroy());
}

}  // namespace error
}  // namespace base

#endif  // BASE_ERROR_ERROR_HPP
