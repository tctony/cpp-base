#ifndef _ZLOG_TO_CONSOLE_H_
#define _ZLOG_TO_CONSOLE_H_

#include "zlog.h"

namespace zlog {

int zlogToConsole();

}  // namespace zlog

// clang-format off
#define ZlogToConsoleSetup ___ZlogToConsoleSetup(__LINE__)
#define ___ZlogToConsoleSetup(_line) ___ZlogToConsoleSetup_I(_line)
#define ___ZlogToConsoleSetup_I(_line) \
static int __zlog_to_console_setup_##_line = zlog::zlogToConsole()
// clang-formt on

ZlogToConsoleSetup;

#endif
