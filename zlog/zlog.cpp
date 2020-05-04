/*
 ============================================================================
 Name        : zlog.cpp
 Author      : peterfan
 Version     : 2.0
 Created on  : 2009-7-11
 Copyright   : Copyright (C) 1998 - 2009 TENCENT Inc. All Right Reserved
 Description : zlog implementations
 ============================================================================
 */

#include "zlog.h"

#include <atomic>
#include <cstdio>

#ifdef _WIN32
#include <io.h>
#include <process.h>
#define getpid _getpid
#else
#include <unistd.h>
#endif

#ifdef LIGHT_WEIGHT_STRING_ENABLED
#include <forward_list>
#endif

namespace zlog {

#define CONST_STRING(name, value) static const char* name = value

CONST_STRING(kStringTrue, "true");
CONST_STRING(kStringFalse, "false");
CONST_STRING(kStringNullptr, "nullptr");
CONST_STRING(kFormatPointer, "%p");

std::nullptr_t g_unused_wtf = nullptr;
NullLogger g_null_logger;

std::string hex(const void* buf, size_t size) {
  static const char* HEX = "0123456789abcdef";

  const uint8_t* input = (const uint8_t*)buf;
  std::string str;
  str.reserve(size << 1);

  uint8_t t, a, b;
  for (size_t i = 0; i < size; ++i) {
    t = input[i];
    a = t >> 4;    // a = t / 16;
    b = t & 0x0f;  // b = t % 16;
    str.append(1, HEX[a]);
    str.append(1, HEX[b]);
  }
  return str;
}

std::string hex(const char* cstr) { return hex(cstr, strlen(cstr)); }

std::string hex(const std::string& str) { return hex(str.c_str(), str.size()); }

const char* zfunc_(const char* func, const char* pretty_func) {
  size_t func_len = 0;
  const char* p = pretty_func;
  while (true) {
    p = strstr(p, func);
    if (p == NULL) break;
    if (func_len == 0) func_len = strlen(func);
    if (p[func_len] == '(') return p;
    p += func_len;
  }
  return func;
}

std::string get_simple_funcname_(const char* fullname) {
  const char* p = strchr(fullname, '(');
  if (p == NULL) return fullname;
  return std::string(fullname, p - fullname);
}

static const char* get_simple_filename_(const char* path) {
#ifdef _WIN32
  static const char kSepChar = '\\';
#else
  static const char kSepChar = '/';
#endif
  const char* pch = strrchr(path, kSepChar);
  return pch == NULL ? path : pch + 1;
}

static const char* vsformat_(const char* format, va_list vl) {
  int size = 512;
  char* buf = (char*)malloc(size);
  if (buf != NULL) {
    memset(buf, 0, size);
    va_list vlcopy;
    va_copy(vlcopy, vl);
    int n = vsnprintf(buf, size, format, vl);
    if (n >= size) {
      size = n + 1;
      buf = (char*)realloc(buf, size);
      if (buf != NULL) {
        n = vsnprintf(buf, size, format, vlcopy);
      }
    }
    va_end(vlcopy);
  }
  return buf;
}

static const char* loglevel_to_string_(LogLevel level) {
  static const char* const kLevelStrings[] = {
      "V",  // verbose
      "D",  // debug
      "I",  // info
      "W",  // warn
      "E",  // error
      "F"   // fatal
  };
  return kLevelStrings[level];
}

static size_t format_timeval_(std::chrono::system_clock::time_point& tp,
                              const char* format, char* buf, size_t sz) {
  size_t written = -1;
  time_t tp_sec = std::chrono::system_clock::to_time_t(tp);
  struct tm* gm = localtime(&tp_sec);
  if (gm) {
    written = strftime(buf, sz, format, gm);
    if ((written > 0) && (written < sz)) {
      auto tp_msec = std::chrono::duration_cast<std::chrono::milliseconds>(
                         tp.time_since_epoch())
                         .count() %
                     1000;
      int w =
          snprintf(buf + written, sz - (size_t)written, ".%03d", (int)tp_msec);
      written = (w > 0) ? written + w : -1;
    }
  }
  return written;
}

static int get_thread_id_() {
  static std::atomic<int> s_thread_id;
  static thread_local int s_tls_thread_id = -1;

  if (s_tls_thread_id == -1) {
    s_tls_thread_id = s_thread_id;
    s_thread_id += 1;
  }
  return s_tls_thread_id;
}

size_t Variant::size() const {
  switch (type_) {
    case kTypeBool:
      return sizeof(bool);
    case kTypeChar:
    case kTypeSChar:
    case kTypeUChar:
      return sizeof(char);
    case kTypeShort:
    case kTypeUShort:
      return sizeof(short);
    case kTypeInt:
    case kTypeUInt:
      return sizeof(int);
    case kTypeLong:
    case kTypeULong:
      return sizeof(long);
    case kTypeLongLong:
    case kTypeULongLong:
      return sizeof(long long);
    case kTypeFloat:
      return sizeof(float);
    case kTypeDouble:
      return sizeof(double);
    case kTypeLongDouble:
      return sizeof(long double);
    case kTypeVoidPtr:
    case kTypeCharPtr:
      return sizeof(void*);
    case kTypeNullPtr:
      return sizeof(std::nullptr_t);
    default:
      return 0;
  }
}

static const char* const kBinStringTable[256] = {
    "00000000", "00000001", "00000010", "00000011", "00000100", "00000101",
    "00000110", "00000111", "00001000", "00001001", "00001010", "00001011",
    "00001100", "00001101", "00001110", "00001111", "00010000", "00010001",
    "00010010", "00010011", "00010100", "00010101", "00010110", "00010111",
    "00011000", "00011001", "00011010", "00011011", "00011100", "00011101",
    "00011110", "00011111", "00100000", "00100001", "00100010", "00100011",
    "00100100", "00100101", "00100110", "00100111", "00101000", "00101001",
    "00101010", "00101011", "00101100", "00101101", "00101110", "00101111",
    "00110000", "00110001", "00110010", "00110011", "00110100", "00110101",
    "00110110", "00110111", "00111000", "00111001", "00111010", "00111011",
    "00111100", "00111101", "00111110", "00111111", "01000000", "01000001",
    "01000010", "01000011", "01000100", "01000101", "01000110", "01000111",
    "01001000", "01001001", "01001010", "01001011", "01001100", "01001101",
    "01001110", "01001111", "01010000", "01010001", "01010010", "01010011",
    "01010100", "01010101", "01010110", "01010111", "01011000", "01011001",
    "01011010", "01011011", "01011100", "01011101", "01011110", "01011111",
    "01100000", "01100001", "01100010", "01100011", "01100100", "01100101",
    "01100110", "01100111", "01101000", "01101001", "01101010", "01101011",
    "01101100", "01101101", "01101110", "01101111", "01110000", "01110001",
    "01110010", "01110011", "01110100", "01110101", "01110110", "01110111",
    "01111000", "01111001", "01111010", "01111011", "01111100", "01111101",
    "01111110", "01111111", "10000000", "10000001", "10000010", "10000011",
    "10000100", "10000101", "10000110", "10000111", "10001000", "10001001",
    "10001010", "10001011", "10001100", "10001101", "10001110", "10001111",
    "10010000", "10010001", "10010010", "10010011", "10010100", "10010101",
    "10010110", "10010111", "10011000", "10011001", "10011010", "10011011",
    "10011100", "10011101", "10011110", "10011111", "10100000", "10100001",
    "10100010", "10100011", "10100100", "10100101", "10100110", "10100111",
    "10101000", "10101001", "10101010", "10101011", "10101100", "10101101",
    "10101110", "10101111", "10110000", "10110001", "10110010", "10110011",
    "10110100", "10110101", "10110110", "10110111", "10111000", "10111001",
    "10111010", "10111011", "10111100", "10111101", "10111110", "10111111",
    "11000000", "11000001", "11000010", "11000011", "11000100", "11000101",
    "11000110", "11000111", "11001000", "11001001", "11001010", "11001011",
    "11001100", "11001101", "11001110", "11001111", "11010000", "11010001",
    "11010010", "11010011", "11010100", "11010101", "11010110", "11010111",
    "11011000", "11011001", "11011010", "11011011", "11011100", "11011101",
    "11011110", "11011111", "11100000", "11100001", "11100010", "11100011",
    "11100100", "11100101", "11100110", "11100111", "11101000", "11101001",
    "11101010", "11101011", "11101100", "11101101", "11101110", "11101111",
    "11110000", "11110001", "11110010", "11110011", "11110100", "11110101",
    "11110110", "11110111", "11111000", "11111001", "11111010", "11111011",
    "11111100", "11111101", "11111110", "11111111"};

static std::string bytes_to_binstring_(const void* buffer, size_t length) {
  const uint8_t* input = (const uint8_t*)buffer;
  std::string str;
  if (length > 0) {
    str.reserve(9 * length - 1);
  }
  for (size_t i = 0; i < length; ++i) {
    if (i > 0) {
      str.append(1, ' ');
    }
    str.append(kBinStringTable[input[i]]);
  }
  return str;
}

static std::string bytes_to_hexstring_(const void* buffer, size_t length,
                                       bool uppercase) {
  static const char* kLowercaseHex = "0123456789abcdef";
  static const char* kUppercaseHex = "0123456789ABCDEF";

  const char* hex_string = uppercase ? kUppercaseHex : kLowercaseHex;
  const uint8_t* input = (const uint8_t*)buffer;

  std::string str("0x");
  str.reserve(2 + length * 2);
  for (size_t i = 0; i < length; ++i) {
    uint8_t x = input[i];
    uint8_t a = x >> 4;
    uint8_t b = x & 0x0f;
    str.append(1, hex_string[a]);
    str.append(1, hex_string[b]);
  }
  return str;
}

template <typename T>
static std::string int_to_binstring_(T x) {
  unsigned char bytes[sizeof(T)] = {0};
  for (size_t i = 0; i < sizeof(T); ++i) {
    bytes[i] = (x >> ((sizeof(T) - 1 - i) << 3)) & 0xFF;
  }
  return bytes_to_binstring_(bytes, sizeof(T));
}

template <typename T>
static std::string int_to_hexstring_(T x, bool uppercase) {
  unsigned char bytes[sizeof(T)] = {0};
  for (size_t i = 0; i < sizeof(T); ++i) {
    bytes[i] = (x >> ((sizeof(T) - 1 - i) << 3)) & 0xFF;
  }
  return bytes_to_hexstring_(bytes, sizeof(T), uppercase);
}

#ifdef LIGHT_WEIGHT_STRING_ENABLED
struct LightWeightStringAllocator {
  std::forward_list<void*> list;
  LightWeightStringAllocator() {
    const static int count = 50;
    for (int i = 0; i < count; ++i) {
      list.push_front(malloc(sizeof(LightWeightString)));
    }
  }
  ~LightWeightStringAllocator() {
    while (!list.empty()) {
      void* p = list.front();
      std::free(p);
      list.pop_front();
    }
  }
  void* alloc(size_t size) {
    void* p = nullptr;
    if (!list.empty()) {
      p = list.front();
      list.pop_front();
    } else {
      assert(0);
      p = std::malloc(size);
    }
    return p;
  }
  void free(void* p) {
    if (p != nullptr) {
      list.push_front(p);
    }
  }
};
thread_local LightWeightStringAllocator s_lws_allocator;
void* lws_allocate(std::size_t size) { return s_lws_allocator.alloc(size); }
void lws_deallocate(void* p) { s_lws_allocator.free(p); }

void* LightWeightString::operator new(size_t size) {
  void* p = s_lws_allocator.alloc(size);
  return p;
}
void LightWeightString::operator delete(void* p) {
  if (p != nullptr) s_lws_allocator.free(p);
}
#endif

LogString::LogString() {
#ifdef LIGHT_WEIGHT_STRING_ENABLED
  tail_ = pieces_.end();
  total_length_ = 0;
#endif
}

LogString::~LogString() {}

void LogString::appendFormat(const char* format, ...) {
  va_list vl;
  va_start(vl, format);
  appendFormatV(format, vl);
  va_end(vl);
}

void LogString::appendFormatV(const char* format, va_list vl) {
  const char* str = vsformat_(format, vl);
  if (str != NULL) {
#ifdef LIGHT_WEIGHT_STRING_EANBLED
    appendPiece(str, hand_over_memory());
#else
    str_.append(str);
    free((void*)str);
#endif
  }
}

void LogString::format(const char* format, ...) {
  va_list vl;
  va_start(vl, format);
  formatV(format, vl);
  va_end(vl);
}

void LogString::formatV(const char* format, va_list vl) {
  clear();
  appendFormatV(format, vl);
}

void LogString::append(bool val) {
#ifdef LIGHT_WEIGHT_STRING_EANBLED
  appendPiece(val ? kStringTrue : kStringFalse);
#else
  str_.append(val ? kStringTrue : kStringFalse);
#endif
}

void LogString::append(char val) {
#ifdef LIGHT_WEIGHT_STRING_ENABLED
  appendPiece(std::string(1, val));
#else
  str_.append(1, val);
#endif
}

void LogString::append(const void* val) { appendFormat(kFormatPointer, val); }

void LogString::append(const char* val) {
#ifdef LIGHT_WEIGHT_STRING_ENABLED
  appendPiece(val == nullptr ? kStringNullptr : val);
#else
  str_.append(val == nullptr ? kStringNullptr : val);
#endif
}

void LogString::append(const std::string& val) {
#ifdef LIGHT_WEIGHT_STRING_ENABLED
  std::string copy(val);
  appendPiece(std::move(copy));
#else
  str_.append(val);
#endif
}

void LogString::append(std::nullptr_t np) { append(kStringNullptr); }

void LogString::append(const char* val, size_t n) {
#ifdef LIGHT_WEIGHT_STRING_ENABLED
  appendPiece(val, n);
#else
  str_.append(val, n);
#endif
}

#ifdef LIGHT_WEIGHT_STRING_ENABLED
void LogString::mergePieces() {
  if (total_length_ == 0) return;
  if (str_.capacity() - str_.size() <= total_length_) {
    str_.reserve(str_.size() + total_length_);
  }
  for (const auto& p : pieces_) {
    str_.append(p.data, p.length);
  }
  pieces_.clear();
  tail_ = pieces_.end();
  total_length_ = 0;
}
#endif

std::string& LogString::str() {
#ifdef LIGHT_WEIGHT_STRING_ENABLED
  mergePieces();
#endif
  return str_;
}

const std::string& LogString::str() const {
#ifdef LIGHT_WEIGHT_STRING_ENABLED
  // we can't call mergePieces here..., so the str_ is incomplete
  assert(0);
  return str_;
#endif
  return str_;
}

void LogString::clear() {
  return;
#ifdef LIGHT_WEIGHT_STRING_ENABLED
  pieces_.clear();
  total_length_ = 0;
#endif
  str_.clear();
}

void LogString::appendVariant(const Variant& v) {
  switch (v.type()) {
    case Variant::kTypeBool:
      append(v.value().b);
      break;
    case Variant::kTypeChar:
      append(v.value().c);
      break;
    case Variant::kTypeSChar:
      append(v.value().s8);
      break;
    case Variant::kTypeUChar:
      append(v.value().u8);
      break;
    case Variant::kTypeShort:
      append(v.value().s16);
      break;
    case Variant::kTypeUShort:
      append(v.value().u16);
      break;
    case Variant::kTypeInt:
      append(v.value().i32);
      break;
    case Variant::kTypeUInt:
      append(v.value().u32);
      break;
    case Variant::kTypeLong:
      append(v.value().l);
      break;
    case Variant::kTypeULong:
      append(v.value().ul);
      break;
    case Variant::kTypeLongLong:
      append(v.value().ll);
      break;
    case Variant::kTypeULongLong:
      append(v.value().ull);
      break;
    case Variant::kTypeFloat:
      append(v.value().f);
      break;
    case Variant::kTypeDouble:
      append(v.value().d);
      break;
    case Variant::kTypeLongDouble:
      append(v.value().ld);
      break;
    case Variant::kTypeVoidPtr:
      append(v.value().p);
      break;
    case Variant::kTypeCharPtr:
      append(v.value().s);
      break;
    case Variant::kTypeNullPtr:
      append(v.value().np);
      break;
    default:
      break;
  }
}

void LogString::appendVariantFormat(char format, const Variant& v) {
  switch (format) {
    case '_': {
      appendVariant(v);
      break;
    }
    case 'b': {
      appendVariantBinFormat_(v);
      break;
    }
    case 'p':
    case 'x':
    case 'X': {
      bool uppercase = (format == 'X' ? true : false);
      appendVariantHexFormat_(v, uppercase);
      break;
    }
    default:
      break;
  };
}

void LogString::appendVariantBinFormat_(const Variant& v) {
  switch (v.type()) {
    case Variant::kTypeBool:
    case Variant::kTypeChar:
    case Variant::kTypeSChar:
    case Variant::kTypeUChar:
      append(kBinStringTable[v.value().u8]);
      break;
    case Variant::kTypeShort:
    case Variant::kTypeUShort:
      append(int_to_binstring_(v.value().u16));
      break;
    case Variant::kTypeInt:
    case Variant::kTypeUInt:
      append(int_to_binstring_(v.value().u32));
      break;
    case Variant::kTypeLong:
    case Variant::kTypeULong:
      append(int_to_binstring_(v.value().ul));
      break;
    case Variant::kTypeLongLong:
    case Variant::kTypeULongLong:
      append(int_to_binstring_(v.value().ull));
      break;
    case Variant::kTypeFloat:
    case Variant::kTypeDouble:
    case Variant::kTypeLongDouble:
    case Variant::kTypeVoidPtr:
    case Variant::kTypeCharPtr:
    case Variant::kTypeNullPtr:
      assert(false);
      break;
    default:
      break;
  }
}

void LogString::appendVariantHexFormat_(const Variant& v, bool uppercase) {
  switch (v.type()) {
    case Variant::kTypeBool:
    case Variant::kTypeChar:
    case Variant::kTypeSChar:
    case Variant::kTypeUChar:
      append(int_to_hexstring_(v.value().u8, uppercase));
      break;
    case Variant::kTypeShort:
    case Variant::kTypeUShort:
      append(int_to_hexstring_(v.value().u16, uppercase));
      break;
    case Variant::kTypeInt:
    case Variant::kTypeUInt:
      append(int_to_hexstring_(v.value().u32, uppercase));
      break;
    case Variant::kTypeLong:
    case Variant::kTypeULong:
      append(int_to_hexstring_(v.value().ul, uppercase));
      break;
    case Variant::kTypeLongLong:
    case Variant::kTypeULongLong:
      append(int_to_hexstring_(v.value().ull, uppercase));
      break;
    case Variant::kTypeFloat:
    case Variant::kTypeDouble:
    case Variant::kTypeLongDouble:
      assert(false);
      break;
    case Variant::kTypeVoidPtr:
    case Variant::kTypeCharPtr:
      append(v.value().p);
      break;
    case Variant::kTypeNullPtr:
      append(v.value().np);
      break;
    default:
      break;
  }
}

class DefaultLogFilter : public ILogFilter {
 public:
  template <typename F>
  DefaultLogFilter(F&& f) : filter_(std::forward<F>(f)) {}
  virtual ~DefaultLogFilter() {}
  virtual bool filter(const LogEntry& e) {
    return filter_ ? filter_(e) : false;
  }

 private:
  std::function<bool(const LogEntry& e)> filter_;
};

LogAppenderBase::LogAppenderBase() {
  filter_ = NULL;
  formatter_ = new DefaultLogFormatter();
}

LogAppenderBase::~LogAppenderBase() {
  delete filter_;
  delete formatter_;
}

void LogAppenderBase::setFilter(std::function<bool(const LogEntry& e)> filter) {
  setFilter(new DefaultLogFilter(std::move(filter)));
}

void LogAppenderBase::setFilter(ILogFilter* filter) {
  if (filter != filter_) {
    delete filter_;
    filter_ = filter;
  }
}

void LogAppenderBase::setFormatter(ILogFormatter* formatter) {
  if (formatter != formatter_) {
    delete formatter_;
    formatter_ = formatter;
  }
}

void LogAppenderBase::doWrite(LogEntry& e) {
  if (filter_ != NULL && !filter_->filter(e)) {
    return;
  }
  if (formatter_ != NULL) {
    log_.clear();
    formatter_->format(e, log_);
    doWriteLog(e, log_);
  }
}

DefaultLogFormatter::DefaultLogFormatter(const std::string& format,
                                         const std::string& time_format)
    : format_(format), time_format_(time_format) {
  parsed_format_ = parseFormat_(format);
}

DefaultLogFormatter::~DefaultLogFormatter() {}

void DefaultLogFormatter::setFormat(const std::string& format) {
  format_ = format;
  parsed_format_ = parseFormat_(format);
}

static std::string& string_replace_(std::string& str,
                                    const std::string& old_str,
                                    const std::string& new_str) {
  size_t old_len = old_str.size(), new_len = new_str.size();
  std::string::size_type curr_pos = 0, prev_pos = 0;
  while ((curr_pos = str.find(old_str, prev_pos)) != std::string::npos) {
    str.replace(curr_pos, old_len, new_str);
    prev_pos = curr_pos + new_len;
  }
  return str;
}

std::string DefaultLogFormatter::parseFormat_(const std::string& format) {
  std::string new_format(format);
  string_replace_(new_format, "%", "%%");
  string_replace_(new_format, "$$", "$");
  string_replace_(new_format, "$level", "%0");
  string_replace_(new_format, "$time", "%1");
  string_replace_(new_format, "$log", "%2");
  string_replace_(new_format, "$tag", "%3");
  string_replace_(new_format, "$func", "%4");
  string_replace_(new_format, "$file", "%5");
  string_replace_(new_format, "$line", "%6");
  string_replace_(new_format, "$pid", "%7");
  string_replace_(new_format, "$tid", "%8");
  return new_format;
}

void DefaultLogFormatter::format(LogEntry& e, LogString& log) {
  char time_buf[128] = {0};
  format_timeval_(e.time, time_format_.c_str(), time_buf, 128);
  const char* level = loglevel_to_string_(e.level);

  doFormat_(&log, parsed_format_.c_str(), level, time_buf, e.log.str().c_str(),
            e.tag.c_str(), e.func.c_str(), e.file.c_str(), e.line, e.pid,
            e.tid);

  log.append('\n');
}

class HexDumper {
 public:
  const std::string& toHexString(const void* data, size_t size);

 private:
  void dumpHeader_(size_t size);
  int dumpLine_(size_t line, const unsigned char* data, size_t size);
  void dumpData_(const void* data, size_t size);

 private:
  enum { kLineBufferSize = 128 };

  LogString buffer_;
  char line_buffer_[kLineBufferSize];
};

const std::string& HexDumper::toHexString(const void* data, size_t size) {
  buffer_.clear();
  dumpData_(data, size);
  return buffer_.str();
}

void HexDumper::dumpHeader_(size_t size) {
  snprintf(line_buffer_, kLineBufferSize,
           "\n\n%-8u   0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n",
           (unsigned int)size);
  buffer_.append(line_buffer_);
}

static inline bool is_visible_char_(unsigned char ch) {
  return 32 <= ch && ch <= 126;
}

int HexDumper::dumpLine_(size_t line, const unsigned char* data, size_t size) {
  if (data == NULL || size == 0) return 0;

  int offset = 0;
  offset += snprintf(line_buffer_ + offset, kLineBufferSize - offset,
                     "%08x: ", (unsigned int)(line * 16));
  for (size_t i = 0; i < size; ++i) {
    offset += snprintf(line_buffer_ + offset, kLineBufferSize - offset, "%02X ",
                       (unsigned char)data[i]);
  }
  if (size < 16) {
    for (size_t i = 0; i < 16 - size; ++i) {
      offset +=
          snprintf(line_buffer_ + offset, kLineBufferSize - offset, "   ");
    }
  }
  offset += snprintf(line_buffer_ + offset, kLineBufferSize - offset, "| ");
  for (size_t i = 0; i < size; ++i) {
    unsigned char ch = data[i];
    if (ch == '\r') {
      offset +=
          snprintf(line_buffer_ + offset, kLineBufferSize - offset, "\\r");
    } else if (ch == '\n') {
      offset +=
          snprintf(line_buffer_ + offset, kLineBufferSize - offset, "\\n");
    } else {
      offset += snprintf(line_buffer_ + offset, kLineBufferSize - offset, "%c",
                         (is_visible_char_(ch) ? ch : '.'));
    }
  }
  offset += snprintf(line_buffer_ + offset, kLineBufferSize - offset, "\n");
  buffer_.append(line_buffer_);
  return offset;
}

void HexDumper::dumpData_(const void* data, size_t size) {
  if (data != NULL && size > 0) {
    dumpHeader_(size);
    const unsigned char* begin = static_cast<const unsigned char*>(data);
    size_t line = size / 16;
    for (size_t i = 0; i < line; ++i) {
      dumpLine_(i, begin, 16);
      begin += 16;
    }
    dumpLine_(line, begin, size % 16);
  }
}

#undef __ZLOG_A__
#undef __ZLOG_B__
#undef __ZLOG_C__

thread_local LogEntry Logger::s_tss_log;
thread_local unsigned Logger::s_tss_flags;

enum { kFlagWriting = 0x1 };

Logger::Logger()
    : this_(*this),
      __ZLOG_A__(*this),
      __ZLOG_B__(*this),
      __ZLOG_C__(*this),
      level_(kLevelVerbose) {}

Logger::~Logger() { reset_(); }

Logger* Logger::instance(LogLevel level) {
  Logger* logger = instance();
  if (logger->isEnabledFor_(level)) {
    return logger;
  }
  return NULL;
}

Logger* Logger::instance() {
  static Logger s_instance;
  return &s_instance;
}

void Logger::reset() { instance()->reset_(); }

void Logger::reset_() {
  std::lock_guard<std::mutex> lock(mutex_);
  level_ = kLevelVerbose;
  for (auto appender : appenders_) {
    delete appender;
  }
  appenders_.clear();
}

Logger& Logger::prepare(LogLevel level, const char* tag, const char* func,
                        const char* file, int line, const Sentry& sentry) {
  return prepare(level, tag, func, file, line);
}

Logger& Logger::prepare(LogLevel level, const char* tag, const char* func,
                        const char* file, int line) {
  LogEntry& e = s_tss_log;
  e.level = level;
  e.tag = tag;
  e.func = func;
  e.file = get_simple_filename_(file);
  e.line = line;
  e.log.clear();
  e.pid = getpid();
  e.tid = get_thread_id_();
  e.time = std::chrono::system_clock::now();
  return *this;
}

Logger& Logger::logV(const char* format, va_list vlist) {
  s_tss_log.log.formatV(format, vlist);
  return *this;
}

Logger& Logger::log(const char* format, ...) {
  va_list vlist;
  va_start(vlist, format);
  logV(format, vlist);
  va_end(vlist);
  return *this;
}

Logger& Logger::hex(const char* cstr) { return hex(cstr, strlen(cstr)); }

Logger& Logger::hex(const std::string& str) {
  return hex(str.data(), str.size());
}

Logger& Logger::hex(const void* data, size_t size) {
  static thread_local HexDumper s_tss_hex;
  const std::string& str = s_tss_hex.toHexString(data, size);
  s_tss_log.log.append(str);
  return *this;
}

void Logger::addAppender(ILogAppender* appender) {
  assert(appender != NULL);
  std::lock_guard<std::mutex> lock(mutex_);
  appenders_.push_back(appender);
}

void Logger::setLevel(LogLevel level) { level_ = level; }

bool Logger::isEnabledFor_(LogLevel level) const {
  return level >= level_ && !(s_tss_flags & kFlagWriting);
}

void Logger::commit() { writeToAllAppender_(); }

void Logger::writeToAllAppender_() {
  LogEntry& e = s_tss_log;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    s_tss_flags |= kFlagWriting;
    for (auto appender : appenders_) {
      if (appender) {
        appender->doWrite(e);
      }
    }
    s_tss_flags &= ~kFlagWriting;
  }
  e.log.clear();
}

void Logger::typesafeFormat_(const char* format, const Variant* args,
                             size_t num_args) {
  LogEntry& e = s_tss_log;
  typesafeFormat(&e.log, format, e.func.c_str(), args, num_args);
}

static inline bool is_format_char_(char c) {
  return c == '_' || c == 'x' || c == 'X' || c == 'b' || c == 'p';
}

static inline bool is_format_index_(char c) { return '0' <= c && c <= '9'; }

bool typesafeFormat(LogString* log, const char* format, const char* func,
                    const Variant* args, size_t num_args) {
  const char* start = format;
  const char* current = start;
  size_t arg_index = 0;

  while (char ch = *current) {
    if (ch != '%') {
      ++current;
      continue;
    }
    char nextch = *++current;
    if (is_format_char_(nextch)) {
      if (current - 1 > start) {
        log->append(start, current - start - 1);
      }

      if (arg_index >= num_args) {
        // TODO: no enough arguments
        assert(false);
        return false;
      }

      log->appendVariantFormat(nextch, args[arg_index]);
      ++arg_index;
      start = ++current;
    } else if (is_format_index_(nextch)) {
      if (current - 1 > start) {
        log->append(start, current - start - 1);
      }

      size_t index = nextch - '0';
      if (index >= num_args) {
        // TODO: no enough arguments
        assert(false);
        return false;
      }

      log->appendVariant(args[index]);
      start = ++current;
    } else if (nextch == '%') {
      if (current > start) {
        log->append(start, current - start);
      }
      start = ++current;
    } else if (nextch == '@') {
      if (current - 1 > start) {
        log->append(start, current - start - 1);
      }
      log->append(func);
      start = ++current;
    } else {
      // TODO: unknown format specifier
      assert(false);
      return false;
    }
  }

  if (current > start) {
    log->append(start, current - start);
  }
  return true;
}

ScopedLog::ScopedLog(LogLevel level, const char* tag, const char* func,
                     const char* file, int line, const char* name,
                     const std::string& args) {
  logger_ = Logger::instance(level);
  if (logger_ != NULL) {
    level_ = level;
    name_ = name;
    tag_ = tag;
    func_ = func;
    file_ = file;
    line_ = line;
    logger_->prepare(level_, tag, func, file, line);
    if (args.empty()) {
      logger_->log("-> %s", name_.c_str());
    } else {
      logger_->log("-> %s(%s)", name_.c_str(), args.c_str());
    }
    logger_->commit();
    time_ = std::chrono::steady_clock::now();
  }
}

ScopedLog::~ScopedLog() {
  if (logger_ != NULL) {
    auto time = std::chrono::steady_clock::now();
    auto time_span =
        std::chrono::duration_cast<std::chrono::milliseconds>(time - time_)
            .count();
    logger_->prepare(level_, tag_.c_str(), func_.c_str(), file_.c_str(), line_);
    logger_->log("<- %s +%lld", name_.c_str(), time_span);
    logger_->commit();
  }
}

}  // namespace zlog
