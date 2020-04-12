#include "worker.hpp"

namespace base {

Worker::Worker(int numOfThreads) : workGuard_(asio::make_work_guard(context_)) {
  assert(numOfThreads > 0);
  threadGroup_.createThreads([this] { context_.run(); }, numOfThreads);
}

Worker::~Worker() {
  workGuard_.reset();
  context_.stop();
}

asio::io_context *Worker::context() { return &context_; }

}  // namespace base
