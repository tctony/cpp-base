#include "zlog.h"

#include "console_appender.h"

static int __reg = [] {
  zlog::addAppender(new zlog::ConsoleAppender());
  return 0;
}();

int main(int argc, char const *argv[]) {
  int a = 1;
  double b = 1.0;
  std::string c = "str";

  zverbose_function(a);
  zdebug_function(a, b);
  zinfo_function(a, b, c);

  {
    zverbose_scope("hahahah: %_", 1);
    zdebug_scope("aaaa");
    zinfo_scope("bbb");
  }

  zverbose("verbose: ")(a, b, c);
  zdebug("debug: ")(a, b, c);
  zinfo("info: ")(a, b, c);
  zwarn("warn: ")(a, b, c);
  zerror("error: ")(a, b, c);
  return 0;
}
