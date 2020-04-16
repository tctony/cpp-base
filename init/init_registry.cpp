#include "init_registry.hpp"

#include <algorithm>
#include <cassert>

namespace base {

int InitRegistry::add(InitFunc init, DestroyFunc destroy, int priority) {
  assert(state_ == 0);

  auto entry = Entry({std::move(init), std::move(destroy), priority});
  entries_.emplace_back(entry);
  return 0;
}

Error InitRegistry::init(const InitArgs& args) {
  assert(state_ == 0);
  state_ = 1;
  sort();

  size_t i;
  Error err;
  for (i = 0; i < entries_.size(); ++i) {
    err = entries_[i].init_(args);
    if (err.has_value()) {
      break;
    }
  }

  if (err.has_value()) {
    for (size_t j = i - 1; j >= 0; j--) {
      entries_[j].destroy_();
    }
  }

  return err;
}

void InitRegistry::destory() {
  assert(state_ == 1);
  state_ = 2;
  std::for_each(entries_.rbegin(), entries_.rend(),
                [](const Entry& e) { e.destroy_(); });
}

void InitRegistry::sort() {
  assert(state_ == 1);
  std::sort(entries_.begin(), entries_.end());
}

}  // namespace base
