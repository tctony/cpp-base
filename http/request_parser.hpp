#ifndef BASE_HTTP_REQUEST_PARSER_HPP
#define BASE_HTTP_REQUEST_PARSER_HPP

#include <string>
#include <tuple>

#include "base/types/error.hpp"
#include "request.hpp"

namespace base {
namespace http {

class Request;

class RequestParser {
 public:
  using ParseResult = std::tuple<Error, Request>;
  static ParseResult parse(const std::string& buffer);

 public:
  RequestParser(Request& request);

  void reset(Request& request);

  enum ParseStatus { indeterminate, good, bad };

  template <typename InputIterator>
  std::tuple<ParseStatus, InputIterator> parse(InputIterator begin,
                                               InputIterator end) {
    while (begin != end) {
      ParseStatus result = consume(*begin++);
      if (result == good || result == bad)
        return std::make_tuple(result, begin);
    }
    return std::make_tuple(indeterminate, begin);
  }

 private:
  ParseStatus consume(char input);

 private:
  Request& request_;
  enum {
    start = 0,
    method,
    uri,
    http_version_h,
    http_version_t_1,
    http_version_t_2,
    http_version_p,
    http_version_slash,
    http_version_major_start,
    http_version_major,
    http_version_minor_start,
    http_version_minor,
    expecting_newline_1,
    header_line_start,
    header_line,
    expecting_newline_2,
    expecting_newline_3,
    expecting_body,
  } state_;
  std::string headerLine_;
  int bodySize_;
};

}  // namespace http
}  // namespace base

#endif
