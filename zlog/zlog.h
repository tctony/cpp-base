/*
 ============================================================================
 Name        : zlog.h
 Author      : peterfan
 Version     : 2.0
 Created on  : 2009-7-11
 Copyright   : Copyright (C) 1998 - 2009 TENCENT Inc. All Right Reserved
 Description : zlog declarations

 Usage:
        zverbose(const char* format, ...);
        zdebug(const char* format, ...);
        zinfo(const char* format, ...);
        zwarn(const char* format, ...);
        zerror(const char* format, ...);
        zfatal(const char* format, ...);

        zverbose_function();
        zdebug_function();
        zinfo_function();

        zverbose_scope(const char* format, ...);
        zdebug_scope(const char* format, ...);
        zinfo_scope(const char* format, ...);

        zassert(bool condition);

 Examples:
 ============================================================================
 */

// To set log tag in compiler time, define ZLOG_TAG before #include "zlog.h"
#undef __ZTAG__
#ifndef ZLOG_TAG
#define __ZTAG__ "zlog"
#else
#define __ZTAG__ ZLOG_TAG
#endif

// To enable/disable zlog in compiler time, define ZLOG_ENABLED before #include
// "zlog.h"
#undef HAS_ZLOG
#ifndef ZLOG_ENABLED
// Not specified by client, zlog is enabled by default
#define HAS_ZLOG 1
#else
// Specified by client
#define HAS_ZLOG ZLOG_ENABLED
#endif

#ifndef _ZLOG_ZLOG_H_
#define _ZLOG_ZLOG_H_

#include <errno.h>

#include <cassert>
#include <chrono>
#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

#if __cplusplus >= 201703L
#include <variant>
#endif

#include "preprocessor.h"

#define __ZFUNC__ __func__

// clang-format off
#define ZLOG_FOREACH_M(i, item) PP_COMMA_IF(i) item = %_
#define ZLOG_MAKE_FORMAT_S(...)                 \
  PP_S(PP_FOREACH(ZLOG_FOREACH_M, __VA_ARGS__)) \
  PP_COMMA_IF(PP_VARIADIC_SIZE(__VA_ARGS__)) __VA_ARGS__

#define ZLOG_FOREACH_M2(i, item) PP_COMMA() item = %_
#define ZLOG_MAKE_FORMAT_S2(...)                 \
  PP_S(PP_FOREACH(ZLOG_FOREACH_M2, __VA_ARGS__)) \
  PP_COMMA_IF(PP_VARIADIC_SIZE(__VA_ARGS__)) __VA_ARGS__
// clang-format on

#define zlog_impl(level)                                                       \
  zlog_write_0(level, __ZTAG__, __ZFUNC__, __FILE__, __LINE__)

#define zlog_fatal_impl(level)                                                 \
  for (zlog::Logger *logger = zlog::Logger::instance(level); logger != NULL;   \
       logger = NULL, assert(0))                                               \
  zlog::g_unused_wtf = logger                                                  \
                           ->prepare(level, __ZTAG__, __ZFUNC__, __FILE__,     \
                                     __LINE__, zlog::Sentry(logger))           \
                           .__ZLOG_B__

#define zlog_scope_impl(level, ...)                                            \
  zlog::ScopedLog PP_LINE_VAR(_scoped_log_)(                                   \
      level, __ZTAG__, __ZFUNC__, __FILE__, __LINE__,                          \
      std::move(zlog::typesafeFormatString(__VA_ARGS__)), "")

#define zlog_function_impl(level, ...)                                         \
  zlog::ScopedLog PP_LINE_VAR(_scoped_log_)(                                   \
      level, __ZTAG__, __ZFUNC__, __FILE__, __LINE__, __ZFUNC__,               \
      zlog::typesafeFormatString(ZLOG_MAKE_FORMAT_S(__VA_ARGS__)))

#define zlog_write_0(level, tag, func, file, line)                             \
  if (zlog::Wrapper logger = zlog::Logger::instance(level))                    \
    ;                                                                          \
  else                                                                         \
    zlog::g_unused_wtf =                                                       \
        logger->prepare(level, tag, func, file, line, zlog::Sentry(logger))    \
            .__ZLOG_B__

#define zverbose_function_0(...)                                               \
  zlog_function_impl(zlog::kLevelVerbose, __VA_ARGS__)
#define zdebug_function_0(...)                                                 \
  zlog_function_impl(zlog::kLevelDebug, __VA_ARGS__)
#define zinfo_function_0(...) zlog_function_impl(zlog::kLevelInfo, __VA_ARGS__)

#define zverbose_scope_0(...) zlog_scope_impl(zlog::kLevelVerbose, __VA_ARGS__)
#define zdebug_scope_0(...) zlog_scope_impl(zlog::kLevelDebug, __VA_ARGS__)
#define zinfo_scope_0(...) zlog_scope_impl(zlog::kLevelInfo, __VA_ARGS__)

#define zverbose_0 zlog_impl(zlog::kLevelVerbose)
#define zdebug_0 zlog_impl(zlog::kLevelDebug)
#define zinfo_0 zlog_impl(zlog::kLevelInfo)
#define zwarn_0 zlog_impl(zlog::kLevelWarn)
#define zerror_0 zlog_impl(zlog::kLevelError)
#define zfatal_0 zlog_fatal_impl(zlog::kLevelFatal)

#define zassert_0(e)                                                           \
  if (e)                                                                       \
    ;                                                                          \
  else                                                                         \
    for (zlog::Logger *logger = zlog::Logger::instance(zlog::kLevelFatal);     \
         logger != NULL; logger = NULL, assert(0))                             \
    logger                                                                     \
        ->prepare(zlog::kLevelFatal, __ZTAG__, __ZFUNC__, __FILE__, __LINE__,  \
                  zlog::Sentry(logger))("Assertion failed: (%_)", #e)          \
        .__ZLOG_A__

#define zlog_write_1(level, tag, func, file, line) ZLOG_NULL

#define zverbose_function_1(...)
#define zdebug_function_1(...)
#define zinfo_function_1(...)

#define zverbose_scope_1(format, ...)
#define zdebug_scope_1(format, ...)
#define zinfo_scope_1(format, ...)

#define zverbose_1 ZLOG_NULL
#define zdebug_1 ZLOG_NULL
#define zinfo_1 ZLOG_NULL
#define zwarn_1 ZLOG_NULL
#define zerror_1 ZLOG_NULL
#define zfatal_1 ZLOG_NULL

#define ZLOG_NULL                                                              \
  if (1)                                                                       \
    ;                                                                          \
  else                                                                         \
    zlog::g_unused_wtf = zlog::g_null_logger.__ZLOG_B__

#define zassert_1(e)                                                           \
  if (e)                                                                       \
    ;                                                                          \
  else                                                                         \
    for (int i = 0; i < 1; ++i, assert(e))                                     \
    zlog::g_null_logger.__ZLOG_A__

#define zlog_write PP_IF(HAS_ZLOG, zlog_write_0, zlog_write_1)
#define zverbose_function                                                      \
  PP_IF(HAS_ZLOG, zverbose_function_0, zverbose_function_1)
#define zdebug_function PP_IF(HAS_ZLOG, zdebug_function_0, zdebug_function_1)
#define zinfo_function PP_IF(HAS_ZLOG, zinfo_function_0, zinfo_function_1)

#define zverbose_scope PP_IF(HAS_ZLOG, zverbose_scope_0, zverbose_scope_1)
#define zdebug_scope PP_IF(HAS_ZLOG, zdebug_scope_0, zdebug_scope_1)
#define zinfo_scope PP_IF(HAS_ZLOG, zinfo_scope_0, zinfo_scope_1)

#define zverbose PP_IF(HAS_ZLOG, zverbose_0, zverbose_1)
#define zdebug PP_IF(HAS_ZLOG, zdebug_0, zdebug_1)
#define zinfo PP_IF(HAS_ZLOG, zinfo_0, zinfo_1)
#define zwarn PP_IF(HAS_ZLOG, zwarn_0, zwarn_1)
#define zerror PP_IF(HAS_ZLOG, zerror_0, zerror_1)
#define zfatal PP_IF(HAS_ZLOG, zfatal_0, zfatal_1)

#define zassert PP_IF(HAS_ZLOG, zassert_0, zassert_1)

#define zerror_sys(s) zerror(s " error %_: %_", errno, strerror(errno));

namespace zlog {

struct NullLogger {
  NullLogger()
      : this_(*this), __ZLOG_A__(*this), __ZLOG_B__(*this), __ZLOG_C__(*this) {}

  template <typename T> NullLogger &operator<<(const T &) { return *this; }

  template <typename... Args>
  NullLogger &operator()(const char *, const Args &...) {
    return *this;
  }

  NullLogger &operator()() { return *this; }
  NullLogger &hex(const char *) { return *this; }
  NullLogger &hex(const std::string &) { return *this; }
  NullLogger &hex(const void *, size_t) { return *this; }
  operator std::nullptr_t() const { return nullptr; }

  NullLogger &this_;
  NullLogger &__ZLOG_A__;
  NullLogger &__ZLOG_B__;
  NullLogger &__ZLOG_C__;
};

#define __ZLOG_A__(...) this_(ZLOG_MAKE_FORMAT_S2(__VA_ARGS__))
#define __ZLOG_B__(...) this_(__VA_ARGS__).__ZLOG_C__
#define __ZLOG_C__(...) this_(ZLOG_MAKE_FORMAT_S(__VA_ARGS__))

extern std::nullptr_t g_unused_wtf;
extern NullLogger g_null_logger;

class Variant {
public:
  Variant() : type_(kTypeInt) { value_.i32 = 0; }
  Variant(bool b) : type_(kTypeBool) { value_.b = b; }
  Variant(char c) : type_(kTypeChar) { value_.c = c; }
  Variant(signed char x) : type_(kTypeSChar) { value_.s8 = x; }
  Variant(unsigned char x) : type_(kTypeUChar) { value_.u8 = x; }
  Variant(short x) : type_(kTypeShort) { value_.s16 = x; }
  Variant(unsigned short x) : type_(kTypeUShort) { value_.u16 = x; }
  Variant(int x) : type_(kTypeInt) { value_.i32 = x; }
  Variant(unsigned int x) : type_(kTypeUInt) { value_.u32 = x; }
  Variant(long x) : type_(kTypeLong) { value_.l = x; }
  Variant(unsigned long x) : type_(kTypeULong) { value_.ul = x; }
  Variant(long long x) : type_(kTypeLongLong) { value_.ll = x; }
  Variant(unsigned long long x) : type_(kTypeULongLong) { value_.ull = x; }
  Variant(float x) : type_(kTypeFloat) { value_.f = x; }
  Variant(double x) : type_(kTypeDouble) { value_.d = x; }
  Variant(long double x) : type_(kTypeLongDouble) { value_.ld = x; }
  Variant(const void *p) : type_(kTypeVoidPtr) { value_.p = p; }
  Variant(const char *s) : type_(kTypeCharPtr) { value_.s = s; }
  Variant(const std::string &s) : type_(kTypeCharPtr) { value_.s = s.c_str(); }
  Variant(std::nullptr_t np) : type_(kTypeNullPtr) { value_.np = np; }
  ~Variant() {}

public:
  union Value {
    bool b;
    char c;
    signed char s8;
    unsigned char u8;
    short s16;
    unsigned short u16;
    int i32;
    unsigned int u32;
    long l;
    unsigned long ul;
    long long ll;
    unsigned long long ull;
    float f;
    double d;
    long double ld;
    const void *p;
    const char *s;
    std::nullptr_t np;
  };

  enum Type {
    kTypeBool,
    kTypeChar,
    kTypeSChar,
    kTypeUChar,
    kTypeShort,
    kTypeUShort,
    kTypeInt,
    kTypeUInt,
    kTypeLong,
    kTypeULong,
    kTypeLongLong,
    kTypeULongLong,
    kTypeFloat,
    kTypeDouble,
    kTypeLongDouble,
    kTypeVoidPtr,
    kTypeCharPtr,
    kTypeNullPtr
  };

  size_t size() const;
  const Value &value() const { return value_; }
  Type type() const { return type_; }

private:
  Value value_;
  Type type_;
};

enum LogLevel {
  kLevelVerbose, // More detailed information.
  kLevelDebug,   // Detailed information on the flow through the system.
  kLevelInfo,    // Interesting runtime events (startup/shutdown), should be
                 // conservative and keep to a minimum.
  kLevelWarn,    // Other runtime situations that are undesirable or unexpected,
                 // but not necessarily "wrong".
  kLevelError,   // Other runtime errors or unexpected conditions.
  kLevelFatal,   // Severe errors that cause premature termination.
  kLevelNone     // Special level used to disable all log messages.
};

class LogString {
public:
  LogString();
  ~LogString();

  void appendFormat(const char *format, ...);
  void appendFormatV(const char *format, va_list vl);
  void format(const char *format, ...);
  void formatV(const char *format, va_list vl);

  void append(bool val);
  void append(char val);
  void append(const void *val);
  void append(const char *val);
  void append(const std::string &val);
  void append(std::nullptr_t np);
  void append(const char *val, size_t n);

  template <typename T, typename std::enable_if<
                            std::is_arithmetic<T>::value>::type * = nullptr>
  void append(T val) {
    str_.append(std::to_string(val));
  }

  void appendVariant(const Variant &v);
  void appendVariantFormat(char format, const Variant &v);

  std::string &str();
  const std::string &str() const;
  void clear();

private:
  void appendVariantBinFormat_(const Variant &v);
  void appendVariantHexFormat_(const Variant &v, bool uppercase);

private:
  std::string str_;
};

bool typesafeFormat(LogString *log, const char *format, const char *func,
                    const Variant *args, size_t num_args);

template <typename... Args>
std::string typesafeFormatString(const char *format, const Args &...args) {
  LogString log;
  Variant v_args[sizeof...(args) + 1] = {args...};
  typesafeFormat(&log, format, "", v_args, sizeof...(args));
  return log.str();
}

#if __cplusplus >= 201703L && defined(ENABLE_STRING_VIEW)
struct String {
  std::variant<std::string_view, std::string> str_;

  String() : str_(std::string_view("")) {}

  // can't make sure null-terminated
  // String(const char *p, size_t len) : str_(std::string_view(p, len)) {}

  // wrap null-terminated str
  String(const char *cstr) : str_(std::string_view(cstr)) {}
  String &operator=(const char *cstr) {
    str_ = std::string_view(cstr);
    return *this;
  }

  // copy from std::string
  String(const std::string &str) : str_(str) {}
  String &operator=(const std::string &str) {
    str_ = str;
    return *this;
  }
  // move from std::string
  String(const std::string &&str) : str_(str) {}
  String &operator=(std::string &&str) {
    str_ = str;
    return *this;
  }

  const char *c_str() {
    return std::visit([](const auto &arg) { return arg.data(); }, str_);
  }
};
#else
using String = std::string;
#endif

// Represents a single log statement
struct LogEntry {
  using time_point = std::chrono::system_clock::time_point;

  LogLevel level;
  time_point time;
  LogString log;
  String tag;
  String func;
  String file;
  int line;
  intmax_t pid;
  intmax_t tid;
};

class ILogFormatter {
public:
  virtual ~ILogFormatter() = default;
  virtual void format(LogEntry &e, LogString &log) = 0;
};

class DefaultLogFormatter : public ILogFormatter {
public:
  // 2020-04-08 16:17:52.311[14519/0][D][tag][main.cpp,83,test_func]log content
  static constexpr const char *kDefaultFormat =
      "$time[$pid/$tid][$level][$tag][$file,$func,$line]$log";
  // 2020-04-08 16:17:52.311
  static constexpr const char *kDefaultTimeFormat = "%Y-%m-%d %H:%M:%S";

public:
  DefaultLogFormatter(const std::string &format = kDefaultFormat,
                      const std::string &time_format = kDefaultTimeFormat);

  virtual ~DefaultLogFormatter();
  virtual void format(LogEntry &e, LogString &log);

  void setFormat(const std::string &format);
  void setTimeFormat(const std::string &format) { time_format_ = format; }
  std::string format() const { return format_; }
  std::string timeFormat() const { return time_format_; }
  std::string parsedFormat() const { return parsed_format_; }

private:
  static std::string parseFormat_(const std::string &format);

  template <typename... Args>
  void doFormat_(LogString *log, const char *format, const Args &...args) {
    Variant v_args[sizeof...(args) + 1] = {args...};
    typesafeFormat(log, format, "", v_args, sizeof...(args));
  }

private:
  std::string format_;
  std::string time_format_;
  std::string parsed_format_;
};

// A ILogAppender represents a device where the log will be written to
class ILogAppender {
public:
  virtual ~ILogAppender() = default;

  // Overwrite this method to write log when a log is fired
  virtual void doWrite(LogEntry &e) = 0;
};

class ILogFilter {
public:
  virtual ~ILogFilter() = default;

  // return true if the message passes through the filter and should be
  // distributed, false otherwise
  virtual bool filter(const LogEntry &e) = 0;
};

// Base class for all appender
class LogAppenderBase : public ILogAppender {
public:
  LogAppenderBase();
  virtual ~LogAppenderBase();

  void setFilter(std::function<bool(const LogEntry &e)> filter);
  void setFilter(ILogFilter *filter);
  void setFormatter(ILogFormatter *formatter);

public:
  // from ILogAppender
  virtual void doWrite(LogEntry &e);

  // Overwrite this template method
  virtual void doWriteLog(const LogEntry &e, LogString &log) = 0;

private:
  LogString log_;
  ILogFilter *filter_;
  ILogFormatter *formatter_;
};

class Sentry;

class Logger {
public:
  Logger &this_;
  Logger &__ZLOG_A__;
  Logger &__ZLOG_B__;
  Logger &__ZLOG_C__;

public:
  static Logger *instance(LogLevel level);
  static Logger *instance();
  static void reset();

  Logger &prepare(LogLevel level, const char *tag, const char *func,
                  const char *file, int line, const Sentry &sentry);
  Logger &prepare(LogLevel level, const char *tag, const char *func,
                  const char *file, int line);

  Logger &logV(const char *format, va_list vlist);
  Logger &log(const char *format, ...);

  template <typename... Args>
  Logger &operator()(const char *format, const Args &...args) {
    Variant v_args[sizeof...(args) + 1] = {args...};
    typesafeFormat_(format, v_args, sizeof...(args));
    return *this;
  }

  Logger &operator()() { return *this; }
  Logger &hex(const char *cstr);
  Logger &hex(const std::string &str);
  Logger &hex(const void *data, size_t size);
  operator std::nullptr_t() const { return nullptr; }

  void addAppender(ILogAppender *appender);
  void setLevel(LogLevel level);
  void commit();

  template <typename T> Logger &operator<<(const T &value) {
    s_tss_log.log.append(value);
    return *this;
  }

private:
  Logger();
  ~Logger();
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

  bool isEnabledFor_(LogLevel level) const;
  void writeToAllAppender_();
  void typesafeFormat_(const char *format, const Variant *args,
                       size_t num_args);
  void reset_();

private:
  static thread_local LogEntry s_tss_log;
  static thread_local unsigned s_tss_flags;

private:
  LogLevel level_;
  std::vector<ILogAppender *> appenders_;
  std::mutex mutex_;
};

// A wrapper class which use some tricks to avoid
// unnecessary function call overhead associate with Logger::instance().
class Wrapper {
public:
  Wrapper(Logger *logger) : logger_(logger) {}

  operator bool() const { return logger_ == NULL; }
  Logger *operator->() const { return logger_; }
  operator Logger *() const { return logger_; }
  operator std::nullptr_t() const { return nullptr; }

private:
  Logger *logger_;
};

// Auto call commit()
class Sentry {
public:
  Sentry(Logger *logger) : logger_(logger) {}
  ~Sentry() { logger_->commit(); }

private:
  Logger *logger_;
};

// A simple scope tracer that writes the scope Enter and Exit to zlog.
class ScopedLog {
  using time_point = std::chrono::steady_clock::time_point;

public:
  // Use constructor to write log of entering a scope.
  ScopedLog(LogLevel level, const char *tag, const char *func, const char *file,
            int line, const std::string &&name, const std::string &args);

  // Use destructor to write log of exiting a scope normally or with a
  // exception.
  ~ScopedLog();

private:
  Logger *logger_;
  LogLevel level_;
  String name_;
  String tag_;
  String func_;
  String file_;
  int line_;
  time_point time_;
};

inline void addAppender(ILogAppender *appender) {
  Logger::instance()->addAppender(appender);
}
inline void setLevel(LogLevel level) { Logger::instance()->setLevel(level); }
inline void reset() { Logger::reset(); }

std::string hex(const void *buf, size_t size);
std::string hex(const char *cstr);
std::string hex(const std::string &str);

} // namespace zlog

#endif /* _ZLOG_ZLOG_H_ */
