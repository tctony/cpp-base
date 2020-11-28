#if !defined(BASE_HTTP_RESPONSE_PARSER_HPP)
#define BASE_HTTP_RESPONSE_PARSER_HPP

#include "base/types/error.hpp"
#include "response.hpp"

namespace base {
namespace http {

class ResponseParser {
 public:
  ResponseParser(Response& response);

  void reset(Response& request);

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
  Response& response_;
  enum {
    http_version_h = 0,
    http_version_t_1,
    http_version_t_2,
    http_version_p,
    http_version_slash,
    http_version_major_start,
    http_version_major,
    http_version_minor_start,
    http_version_minor,
    code_start,
    code,
    msg_start,
    msg,
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

#endif  // BASE_HTTP_RESPONSE_PARSER_HPP
