#include "request_parser.hpp"

#include "absl/strings/str_split.h"
#include "base/util/string_util.hpp"
#include "request.hpp"

namespace base {
namespace http {

RequestParser::ParseResult RequestParser::parse(const std::string& buffer) {
  Request result;
  RequestParser parser(result);
  ParseStatus status;
  std::tie(status, std::ignore) = parser.parse(buffer.begin(), buffer.end());
  switch (status) {
    case good:
      return std::make_tuple(absl::nullopt, result);
    case indeterminate:
      return std::make_tuple(Error({-2, "incomplete request"}), result);
    case bad:
    default:
      return std::make_tuple(Error({-1, "bad request"}), result);
  }
}

RequestParser::RequestParser(Request& request)
    : request_(request), state_(start) {}

void RequestParser::reset(Request& request) {
  request_ = request;
  state_ = start;
}

RequestParser::ParseStatus RequestParser::consume(char input) {
  switch (state_) {
    case start:
      if (!isalpha(input)) {
        return bad;
      } else {
        state_ = method;
        request_.method.push_back(input);
        return indeterminate;
      }
    case method:
      if (' ' == input) {
        if (request_.method != Request::GET && request_.method != Request::POST)
          return bad;

        state_ = uri;
        return indeterminate;
      } else if (!isalpha(input)) {
        return bad;
      } else {
        request_.method.push_back(input);
        return indeterminate;
      }
    case uri: {
      if (' ' == input) {
        state_ = http_version_h;
        return indeterminate;
      } else if (iscntrl(input)) {
        return bad;
      } else {
        request_.uri.push_back(input);
        return indeterminate;
      }
    }
    case http_version_h: {
      if ('H' == input) {
        state_ = http_version_t_1;
        return indeterminate;
      } else {
        return bad;
      }
    }
    case http_version_t_1: {
      if ('T' == input) {
        state_ = http_version_t_2;
        return indeterminate;
      } else {
        return bad;
      }
    }
    case http_version_t_2: {
      if ('T' == input) {
        state_ = http_version_p;
        return indeterminate;
      } else {
        return bad;
      }
    }
    case http_version_p: {
      if ('P' == input) {
        state_ = http_version_slash;
        return indeterminate;
      } else {
        return bad;
      }
    }
    case http_version_slash: {
      if ('/' == input) {
        state_ = http_version_major_start;
        request_.http_version_major = 0;
        request_.http_version_minor = 0;
        return indeterminate;
      } else {
        return bad;
      }
    }
    case http_version_major_start: {
      if (isdigit(input)) {
        request_.http_version_major =
            request_.http_version_major * 10 + (input - '0');
        state_ = http_version_major;
        return indeterminate;
      } else {
        return bad;
      }
    }
    case http_version_major: {
      if ('.' == input) {
        state_ = http_version_minor_start;
        return indeterminate;
      } else if (isdigit(input)) {
        request_.http_version_major =
            request_.http_version_major * 10 + (input - '0');
        return indeterminate;
      } else {
        return bad;
      }
    }
    case http_version_minor_start: {
      if (isdigit(input)) {
        request_.http_version_minor =
            request_.http_version_minor * 10 + (input - '0');
        state_ = http_version_minor;
        return indeterminate;
      } else {
        return bad;
      }
    }
    case http_version_minor: {
      if ('\r' == input) {
        state_ = expecting_newline_1;
        return indeterminate;
      } else if (isdigit(input)) {
        request_.http_version_minor =
            request_.http_version_minor * 10 + (input - '0');
        return indeterminate;
      } else {
        return bad;
      }
    }
    case expecting_newline_1: {
      if ('\n' == input) {
        state_ = header_line_start;
        return indeterminate;
      } else {
        return bad;
      }
    }
    case header_line_start: {
      if ('\r' == input) {
        state_ = expecting_newline_3;
        return indeterminate;
      } else {
        headerLine_ = "";
        headerLine_.push_back(input);
        state_ = header_line;
        return indeterminate;
      }
    }
    case header_line: {
      if ('\r' == input) {
        std::vector<std::string> parts =
            absl::StrSplit(headerLine_, absl::MaxSplits(":", 1));
        if (parts.size() != 2) return bad;
        auto name = util::trim(parts[0]);
        if (name.size() == 0) return bad;
        request_.headerList.add(name, util::trim(parts[1]));

        state_ = expecting_newline_2;
        return indeterminate;
      } else {
        headerLine_.push_back(input);
        return indeterminate;
      }
    }
    case expecting_newline_2: {
      if ('\n' == input) {
        state_ = header_line_start;
        return indeterminate;
      } else {
        return bad;
      }
    }
    case expecting_newline_3: {
      if ('\n' == input) {
        if (Request::GET == request_.method)
          return good;
        else {  // POST
          bodySize_ = std::atoi(request_.headerList.contentLength().c_str());
          if (bodySize_ <= 0)
            return bad;
          else {
            state_ = expecting_body;
            return indeterminate;
          }
        }
      } else {
        return bad;
      }
    }
    case expecting_body: {
      request_.body_.push_back(input);
      if (--bodySize_ == 0)
        return good;
      else
        return indeterminate;
    }
    default:
      return bad;
  }
}

}  // namespace http
}  // namespace base
