/*
 * console_appender.cpp
 *
 *  Created on: 2017年4月24日
 *      Author: peterfan
 */

#include "console_appender.h"

#include <algorithm>
#include <cstdio>

#ifdef _WIN32
#include "owl/windows/unistd.h"
#else
#include <unistd.h>
#endif

#ifdef __ANDROID__
#include <android/log.h>
#endif

namespace zlog {

#ifdef __ANDROID__
static const char* kLogcatFormat = "%s %d: %s\n";

ConsoleAppender::ConsoleAppender() {}

ConsoleAppender::~ConsoleAppender() {}

void ConsoleAppender::doWrite(LogEntry& e) {
  log_.format(kLogcatFormat, e.file.c_str(), e.line, e.log.str().c_str());
  __android_log_write(e.level + 2, e.tag.c_str(), log_.str().c_str());
}
#else
ConsoleAppender::ConsoleAppender() {
  // setvbuf(stdout, NULL, _IONBF, 0);
}

ConsoleAppender::~ConsoleAppender() {}

void ConsoleAppender::doWriteLog(const LogEntry& e, LogString& log) {
  std::fputs(log.str().c_str(), stdout);
  fflush(stdout);
}

// Determine if the terminal supports colors
// Source: https://github.com/agauniyal/rang/
static bool is_color_terminal() {
#ifdef _WIN32
  return true;
#else
  static constexpr const char* Terms[] = {
      "ansi",  "color", "console", "cygwin", "gnome",  "konsole", "kterm",
      "linux", "msys",  "putty",   "rxvt",   "screen", "vt100",   "xterm"};

  const char* env_p = std::getenv("TERM");
  if (env_p == nullptr) {
    return false;
  }

  bool result = std::any_of(
      std::begin(Terms), std::end(Terms),
      [&](const char* term) { return std::strstr(env_p, term) != nullptr; });
  return result;
#endif
}

// Detrmine if the terminal attached
// Source: https://github.com/agauniyal/rang/
static bool in_terminal(FILE* file) {
#ifdef _WIN32
  return _isatty(_fileno(file)) != 0;
#else
  return isatty(fileno(file)) != 0;
#endif
}

// Source: https://misc.flogisoft.com/bash/tip_colors_and_formatting
// Formatting codes
static const std::string kFormatReset = "\033[m";
static const std::string kFormatBold = "\033[1m";
static const std::string kFormatDim = "\033[2m";
static const std::string kFormatUnderline = "\033[4m";
static const std::string kFormatBlink = "\033[5m";
static const std::string kFormatReverse = "\033[7m";
static const std::string kFormatHidden = "\033[8m";
static const std::string kFormatClearLine = "\033[K";

// Foreground colors
static const std::string kForgroundBlack = "\033[30m";
static const std::string kForgroundRed = "\033[31m";
static const std::string kForgroundGreen = "\033[32m";
static const std::string kForgroundYellow = "\033[33m";
static const std::string kForgroundBlue = "\033[34m";
static const std::string kForgroundMagenta = "\033[35m";
static const std::string kForgroundCyan = "\033[36m";
static const std::string kForgroundGray = "\033[37m";
static const std::string kForgroundDefault = "\033[39m";

// Background colors
static const std::string kBackgroundBlack = "\033[40m";
static const std::string kBackgroundRed = "\033[41m";
static const std::string kBackgroundGreen = "\033[42m";
static const std::string kBackgroundYellow = "\033[43m";
static const std::string kBackgroundBlue = "\033[44m";
static const std::string kBackgroundMagenta = "\033[45m";
static const std::string kBackgroundCyan = "\033[46m";
static const std::string kBackgroundGray = "\033[47m";
static const std::string kBackgroundDefault = "\033[49m";

ColoredConsoleAppender::ColoredConsoleAppender() {
  // setvbuf(stdout, NULL, _IONBF, 0);
  enable_color_ = is_color_terminal() && in_terminal(stdout);
  level_color_map_[zlog::kLevelVerbose] = kForgroundGray;
  level_color_map_[zlog::kLevelDebug] = kForgroundCyan;
  level_color_map_[zlog::kLevelInfo] = kForgroundGreen;
  level_color_map_[zlog::kLevelWarn] = kForgroundYellow;
  level_color_map_[zlog::kLevelError] = kForgroundRed + kFormatBold;
  level_color_map_[zlog::kLevelFatal] =
      kForgroundGray + kBackgroundRed + kFormatBold;
  level_color_map_[zlog::kLevelNone] = kFormatReset;
}

ColoredConsoleAppender::~ColoredConsoleAppender() {}

void ColoredConsoleAppender::doWriteLog(const LogEntry& e, LogString& log) {
  if (enable_color_) {
    const std::string& color = level_color_map_[e.level];
    std::fputs(color.c_str(), stdout);
    std::fputs(log.str().c_str(), stdout);
    std::fputs(kFormatReset.c_str(), stdout);
  } else {
    std::fputs(log.str().c_str(), stdout);
  }
  fflush(stdout);
}
#endif

}  // namespace zlog
