#include "error.hpp"

namespace base {
namespace error {

const std::error_category& getErrorCategroy() {
  static ErrorCategory instance;
  return instance;
}

}  // namespace error
}  // namespace base
