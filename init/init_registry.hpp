#if !defined(BASE_INIT_REGISTRY_HPP)
#define BASE_INIT_REGISTRY_HPP

#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "absl/types/any.h"
#include "base/types/error.hpp"

namespace base {

using InitArgs = std::unordered_map<std::string, absl::any>;
using InitFunc = std::function<Error(const InitArgs&)>;
using DestroyFunc = std::function<void()>;

// a init registry and destroy class. init action can be registered seperately
// and then init all together with certain priority. destroy action would
// happened in reverse order of init.
class InitRegistry {
 public:
  InitRegistry(const std::string& label = "default") : label_(label) {}

 public:
  const std::string& label() const { return label_; }

  int add(InitFunc init, DestroyFunc destroy, int priority,
          const std::string& entryLabel);
  Error init(const InitArgs& args);
  void destory();

 private:
  const std::string label_;

  struct Entry {
    InitFunc init_;
    DestroyFunc destroy_;
    int priority_;
    std::string label_;

    Entry(InitFunc init, DestroyFunc destroy, int priority,
          const std::string label)
        : init_(init), destroy_(destroy), priority_(priority), label_(label) {}

    bool operator<(const Entry& entry) const {
      return priority_ < entry.priority_;
    };
  };

  std::vector<Entry> entries_;
  int state_;  // 0 registry, 1 inited, 2 destroyed

  void sort();
};

}  // namespace base

#endif  // BASE_INIT_REGISTRY_HPP
