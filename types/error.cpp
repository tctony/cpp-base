#include "error.hpp"

#include <sstream>

namespace base {

std::string ErrorCode::what() {
  std::stringstream ss;
  ss << code_ << ',' << msg_;
  return ss.str();
}

}  // namespace base
