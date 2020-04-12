#include "string_util.hpp"

namespace base {

namespace util {

/// case

std::string& toLowerCase(std::string& str, edit_in_place) {
  std::transform(str.begin(), str.end(), str.begin(), ::tolower);
  return str;
}

std::string toLowerCase(const std::string& str) {
  std::string result(str);
  return toLowerCase(result, edit_in_place());
}

std::string& toUpperCase(std::string& str, edit_in_place) {
  std::transform(str.begin(), str.end(), str.begin(), ::toupper);
  return str;
}

std::string toUpperCase(const std::string& str) {
  std::string result(str);
  return toUpperCase(result, edit_in_place{});
}

/// trim

std::string& trim(std::string& str, edit_in_place, const std::string& chars) {
  auto pos = str.find_last_not_of(chars);
  if (pos == std::string::npos) {
    str.clear();
    return str;
  } else if (pos + 1 < str.size()) {
    str.erase(pos + 1);
  }

  pos = str.find_first_not_of(chars);
  if (pos == std::string::npos) {
    str.clear();
  } else {
    str.erase(0, pos);
  }

  return str;
}

std::string& rtrim(std::string& str, edit_in_place, const std::string& chars) {
  auto pos = str.find_last_not_of(chars);
  if (pos == std::string::npos) {
    str.clear();
  } else if (pos + 1 < str.size()) {
    str.erase(pos + 1);
  }

  return str;
}

std::string& ltrim(std::string& str, edit_in_place, const std::string& chars) {
  auto pos = str.find_first_not_of(chars);
  if (pos == std::string::npos) {
    str.clear();
  } else {
    str.erase(0, pos);
  }

  return str;
}

std::string trim(const std::string& str, const std::string& chars) {
  std::string result(str);
  return trim(result, edit_in_place(), chars);
}

std::string rtrim(const std::string& str, const std::string& chars) {
  std::string result(str);
  return rtrim(result, edit_in_place(), chars);
}

std::string ltrim(const std::string& str, const std::string& chars) {
  std::string result(str);
  return ltrim(result, edit_in_place(), chars);
}

}  // namespace util

}  // namespace base
