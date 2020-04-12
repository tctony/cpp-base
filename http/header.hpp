#ifndef BASE_HTTP_HEADER_HPP
#define BASE_HTTP_HEADER_HPP

#include <string>
#include <vector>

#include "absl/types/optional.h"

namespace base {
namespace http {

struct Header {
  std::string name;
  std::string value;
};

struct HeaderList {
  std::vector<Header> headers;

  using HeaderPtr = std::vector<Header>::iterator;

  HeaderPtr find(const std::string& name) {
    return std::find_if(headers.begin(), headers.end(),
                        [&name](const Header& h) { return h.name == name; });
  }

  void set(const std::string& name, const std::string& value) {
    auto old = find(name);
    if (old != headers.end())
      old->value = value;
    else
      headers.push_back({name, value});
  }

  void add(const std::string& name, const std::string& value) {
    headers.push_back({name, value});
  }

  absl::optional<std::string> get(const std::string& name) {
    auto h = find(name);
    if (h != headers.end())
      return h->value;
    else
      return absl::nullopt;
  }

  void setHost(const std::string& host) { set("Host", host); }

  void setContentType(const std::string& content_type) {
    set("Content-Type", content_type);
  }

  void setContentLength(const std::string& content_length) {
    set("Content-Length", content_length);
  }

  void setConnectionClose() { set("Connection", "close"); }
};

}  // namespace http
}  // namespace base

#endif