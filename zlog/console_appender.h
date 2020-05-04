/*
 * console_appender.h
 *
 *  Created on: 2017年4月24日
 *      Author: peterfan
 */

#ifndef _ZLOG_CONSOLE_APPENDER_H_
#define _ZLOG_CONSOLE_APPENDER_H_

#include <map>
#include <string>

#include "zlog.h"

namespace zlog {

// A ConsoleAppender write the log to console(logcat for android)
#ifdef __ANDROID__
class ConsoleAppender : public ILogAppender {
 public:
  ConsoleAppender();
  virtual ~ConsoleAppender();
  virtual void doWrite(LogEntry& e);

 private:
  LogString log_;
};

typedef ConsoleAppender ColoredConsoleAppender;
#else
class ConsoleAppender : public LogAppenderBase {
 public:
  ConsoleAppender();
  virtual ~ConsoleAppender();
  virtual void doWriteLog(const LogEntry& e, LogString& log);
};

class ColoredConsoleAppender : public LogAppenderBase {
 public:
  ColoredConsoleAppender();
  virtual ~ColoredConsoleAppender();
  virtual void doWriteLog(const LogEntry& e, LogString& log);

 private:
  bool enable_color_;
  std::map<int, std::string> level_color_map_;
};
#endif

}  // namespace zlog

#endif /* _ZLOG_CONSOLE_APPENDER_H_ */
