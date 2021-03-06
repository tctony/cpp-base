#include "zlog.h"

#include <iostream>

#include "base/util/define.h"
#include "base/util/time_util.hpp"

void make_some_log() {
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
}

int main(int argc, char const *argv[]) {
  auto cost = base::util::timeCostInMilliseconds([] {
    _FOR(i, 1000000) { make_some_log(); }
  });
  std::cout << "cost " << cost << "ms\n";

  return 0;
}
