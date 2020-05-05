#include "console_appender.h"
#include "zlog.h"

namespace zlog {

int zlogToConsole() {
  static ConsoleAppender *appender = nullptr;
  // not thread safe
  if (appender != nullptr) return 0;

  appender = new ConsoleAppender();
  zlog::addAppender(appender);
  zinfo("add console appender finished");
  return 1;
}

}  // namespace zlog
