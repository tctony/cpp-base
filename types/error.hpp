#if !defined(BASE_TYPES_ERROR_H)
#define BASE_TYPES_ERROR_H

#include <string>

#include "absl/types/optional.h"

namespace base {

struct ErrorCode {
  int code_;
  std::string msg_;
};

using Error = absl::optional<ErrorCode>;

}  // namespace base

#endif  // BASE_TYPES_ERROR_H
