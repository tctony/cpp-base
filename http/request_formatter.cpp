#include "request_formatter.hpp"

#include <sstream>

#include "util/string_util.hpp"

namespace base {
namespace http {

std::string RequestFormatter::format(Request& request) {
  RequestFormatter formatter(request);
  return formatter.toString();
}

std::string RequestFormatter::toString() {
  std::stringstream ss;

  // request line
  ss << request_.method << " " << request_.uri << " HTTP/"
     << request_.http_version_major << "." << request_.http_version_minor
     << "\r\n";

  for (auto h : request_.headerList.headers) {
    ss << h.name << ": " << h.value << "\r\n";
  }
  ss << "\r\n";
  if ("POST" == util::toUpperCase(request_.method) &&
      request_.body_.size() > 0) {
    ss << request_.body_;
  }
  return ss.str();
}

}  // namespace http
}  // namespace base
