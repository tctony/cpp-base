#include "console_appender.h"
#include "zlog.h"

// There is no gurantine setup would happen before any log call, since global
// variables initialization order is undefined. So this might just be an
// convenient setup, be careful to use on your needs.
static int __setup = [] {
  zlog::addAppender(new zlog::ConsoleAppender());
  zinfo("setup console appender finished");
  return 0;
}();
