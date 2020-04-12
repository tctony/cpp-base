#if !defined(BASE_WORKER_H)
#define BASE_WORKER_H

#include "asio.hpp"
#include "thread_group.hpp"

namespace base {

class Worker {
 public:
  Worker(int numOfthreads = 1);
  ~Worker();

 public:
  asio::io_context *context();

 private:
  asio::io_context context_;
  asio::executor_work_guard<asio::io_context::executor_type> workGuard_;
  ThreadGroup threadGroup_;
};

}  // namespace base

#endif  // BASE_WORKER_H
