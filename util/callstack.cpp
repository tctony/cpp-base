#include "callstack.h"

#include <dlfcn.h>

namespace base::util {
thread_local std::unique_ptr<CallStack> CallStack::currentExceptionCallstack;
}  // namespace base::util

extern "C" void __cxa_throw(void *thrown_exception, void *pvtinfo,
                            void (*dest)(void *)) {
  static auto orig_cxa_throw =
      (decltype(__cxa_throw) *)(dlsym(RTLD_NEXT, "__cxa_throw"));
  base::util::CallStack::currentExceptionCallstack.reset(
      new base::util::CallStack(3));
  orig_cxa_throw(thrown_exception, pvtinfo, dest);
}
