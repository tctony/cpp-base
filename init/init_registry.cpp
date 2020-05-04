#include "init_registry.hpp"

#include <algorithm>
#include <cassert>

#include "zlog/zlog.h"

namespace base {

int InitRegistry::add(InitFunc init, DestroyFunc destroy, int priority,
                      const std::string& entryLabel) {
  zdebug_function(label_, entryLabel);
  zassert(state_ == 0);

  entries_.emplace_back(std::move(init), std::move(destroy), priority,
                        entryLabel);
  return 0;
}

Error InitRegistry::init(const InitArgs& args) {
  zdebug_function(label_);
  zassert(state_ == 0);
  state_ = 1;
  sort();

  size_t i;
  Error err;
  for (i = 0; i < entries_.size(); ++i) {
    zdebug("%_ init entry %_", label_, entries_[i].label_);
    err = entries_[i].init_(args);
    if (err.has_value()) {
      break;
    }
  }

  if (err.has_value()) {
    for (size_t j = i; j > 0; j--) {
      zdebug("%_ destroy entry %_", label_, entries_[j - 1].label_);
      entries_[j - 1].destroy_();
    }
    zerror("%_ init failed: %_", label_, err.value().what());
  }

  return err;
}

void InitRegistry::destory() {
  zdebug_function(label_);
  zassert(state_ == 1);
  state_ = 2;
  std::for_each(entries_.rbegin(), entries_.rend(), [](const Entry& e) {
    zdebug("destroy entry %_", e.label_);
    e.destroy_();
  });
}

void InitRegistry::sort() {
  zdebug_function(label_);
  zassert(state_ == 1);
  std::sort(entries_.begin(), entries_.end());
}

}  // namespace base
