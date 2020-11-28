#include "response_parser.hpp"

#include "absl/strings/str_split.h"
#include "base/util/string_util.hpp"

namespace base {
namespace http {

ResponseParser::ResponseParser(Response& response)
    : response_(response), state_(http_version_h) {}

void ResponseParser::reset(Response& response) {
  response_ = response;
  state_ = http_version_h;
}

ResponseParser::ParseStatus ResponseParser::consume(char input) {
  switch (state_) {
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
        return indeterminate;
      } else {
        return bad;
      }
    }
    case http_version_major_start: {
      if (isdigit(input)) {
        response_.http_version_major = input - '0';
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
        response_.http_version_major =
            response_.http_version_major * 10 + (input - '0');
        return indeterminate;
      } else {
        return bad;
      }
    }
    case http_version_minor_start: {
      if (isdigit(input)) {
        response_.http_version_minor = input - '0';
        state_ = http_version_minor;
        return indeterminate;
      } else {
        return bad;
      }
    }
    case http_version_minor: {
      if (' ' == input) {
        state_ = code_start;
        return indeterminate;
      } else if (isdigit(input)) {
        response_.http_version_minor =
            response_.http_version_minor * 10 + (input - '0');
        return indeterminate;
      } else {
        return bad;
      }
    }
    case code_start: {
      if (isdigit(input)) {
        response_.code = input - '0';
        state_ = code;
        return indeterminate;
      } else {
        return bad;
      }
    }
    case code: {
      if (' ' == input) {
        state_ = msg_start;
        return indeterminate;
      } else if (isdigit(input)) {
        response_.code = response_.code * 10 + (input - '0');
        return indeterminate;
      } else {
        return bad;
      }
    }
    case msg_start: {
      if (isalpha(input)) {
        response_.msg = "";
        response_.msg.push_back(input);
        state_ = msg;
        return indeterminate;
      } else {
        return bad;
      }
    }
    case msg: {
      if ('\r' == input) {
        state_ = expecting_newline_1;
        return indeterminate;
      } else if (isalnum(input) || isblank(input)) {
        response_.msg.push_back(input);
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
        response_.headerList.add(name, util::trim(parts[1]));

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
        bodySize_ = std::atoi(response_.headerList.contentLength().c_str());
        if (bodySize_ < 0) {
          return bad;
        } else if (bodySize_ == 0) {
          return good;
        } else {
          state_ = expecting_body;
          return indeterminate;
        }
      } else {
        return bad;
      }
    }
    case expecting_body: {
      response_.body.push_back(input);
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
