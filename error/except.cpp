#include "except.h"

namespace base::error {

const char *Exception::tag() const { return "common"; }

std::string Exception::error() const { return error_; }

} // namespace base::error
