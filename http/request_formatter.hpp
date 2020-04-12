#if !defined(BASE_HTTP_REQUEST_FORMATTER_HPP)
#define BASE_HTTP_REQUEST_FORMATTER_HPP

#include <string>

#include "request.hpp"

namespace base {
namespace http {

class RequestFormatter {
 public:
  static std::string format(Request& request);

 public:
  RequestFormatter(Request& request) : request_(request) {}

 public:
  std::string toString();

 private:
  Request& request_;
};

}  // namespace http
}  // namespace base

#endif  // BASE_HTTP_REQUEST_FORMATTER_HPP
