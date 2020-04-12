#if !defined(BASE_THREAD_GROUP)
#define BASE_THREAD_GROUP

#include <thread>

namespace base {

class ThreadGroup {
 public:
  ThreadGroup() : first_(nullptr) {}
  ~ThreadGroup() { join(); }

  void join() {
    while (first_) {
      first_->thread_.join();
      item* tmp = first_;
      first_ = first_->next_;
      delete tmp;
    }
  }

  template <typename Function>
  void createThread(Function f) {
    first_ = new item(f, first_);
  }

  template <typename Function>
  void createThreads(Function f, std::size_t num_threads) {
    for (std::size_t i = 0; i < num_threads; ++i) {
      createThread(f);
    }
  }

 private:
  struct item {
    template <typename Function>
    explicit item(Function f, item* next) : thread_(f), next_(next) {}

    std::thread thread_;
    item* next_;
  };

  item* first_;
};

}  // namespace base

#endif  // BASE_THREAD_GROUP
