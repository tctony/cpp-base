#ifndef BASE_UTIL_STRING_UTIL_HPP
#define BASE_UTIL_STRING_UTIL_HPP

#include <algorithm>
#include <string>

#include "absl/strings/str_split.h"

namespace base {
namespace util {

struct edit_in_place {};

/// case

std::string& toLowerCase(std::string& str, edit_in_place);

std::string toLowerCase(const std::string& str);

std::string& toUpperCase(std::string& str, edit_in_place);

std::string toUpperCase(const std::string& str);

/// split

// return at most limit+1 parts if limit > 0
template <typename Container>
size_t split(Container& parts, const std::string& str,
             const std::string& delims = " ", int limit = 0) {
  if (0 == limit)
    parts = absl::StrSplit(str, delims);
  else
    parts = absl::StrSplit(str, absl::MaxSplits(delims, limit));
  return parts.size();
}

/// trim

std::string& trim(std::string& str, edit_in_place,
                  const std::string& chars = " ");

std::string& rtrim(std::string& str, edit_in_place,
                   const std::string& chars = " ");

std::string& ltrim(std::string& str, edit_in_place,
                   const std::string& chars = " ");

std::string trim(const std::string& str, const std::string& chars = " ");

std::string rtrim(const std::string& str, const std::string& chars = " ");

std::string ltrim(const std::string& str, const std::string& chars = " ");

}  // namespace util
}  // namespace base

#endif