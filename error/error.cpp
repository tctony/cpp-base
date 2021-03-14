#include "error.hpp"
#include "absl/strings/str_cat.h"

namespace base {
namespace error {

class DefaultCategory : public AdaptiveCategory<2> {
 public:
  /* constexpr */ DefaultCategory()
      : AdaptiveCategory({
            &std::system_category(),
            &std::generic_category(),
        }) {}

  virtual const char* name() const noexcept override {
    return "system+generic";
  }
  virtual std::string message(int value) const override {
    if (value == noError) {
      return "no error";
    } if (value == unknownError) {
      return "unknown error";
    } else {
      return absl::StrCat("unspecified ", this->name(), " error");
    }
  }
};

const std::error_category& defaultCategroy() {
  static DefaultCategory instance;
  return instance;
}

}  // namespace error
}  // namespace base
