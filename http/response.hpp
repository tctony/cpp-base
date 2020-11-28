#if !defined(BASE_HTTP_RESPONSE_HPP)
#define BASE_HTTP_RESPONSE_HPP

#include <string>

#include "header.hpp"

namespace base {
namespace http {

class Response {
 public:
  int http_version_major;
  int http_version_minor;
  int code;
  std::string msg;
  HeaderList headerList;
  std::string body;

  friend class ResponseParser;

 public:
  Response() {}
};

}  // namespace http
}  // namespace base

#endif  // BASE_HTTP_RESPONSE_HPP
