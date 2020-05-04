#include <string>

#include "log_case.h"
#include "zlog/zlog.h"

class LogCaseSimple : public LogCase {
 public:
  const char* name() { return "worsest"; }
  void run() {
    for (int i = 0; i < 1000000; ++i) {
      // make length of the str double each time
      // so memory reallocation might occur?
      std::string str1(20, '1');
      std::string str2(40, '2');
      std::string str3(80, '3');
      std::string str4(160, '4');
      std::string str5(320, '5');
      zinfo("|%_|%_|%_|%_|%_", str1, str2, str3, str4, str5);
    }
  }
};

LogCase* GetLogCase() {
  static LogCaseSimple instance_;
  return static_cast<LogCase*>(&instance_);
}