#include "log_case.h"
#include "zlog/zlog.h"

class LogCaseSimple : public LogCase {
 public:
  const char* name() { return "simple"; }
  void run() {
    for (int i = 0; i < 1000000; ++i) {
      std::string str(620, 's');
      zinfo("%_", str);
    }
  }
};

LogCase* GetLogCase() {
  static LogCaseSimple instance_;
  return static_cast<LogCase*>(&instance_);
}