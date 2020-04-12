#if !defined(BASE_HTTP_REQUEST_HPP)
#define BASE_HTTP_REQUEST_HPP

#include <map>
#include <set>
#include <string>
#include <vector>

#include "header.hpp"

namespace base {
namespace http {

class Request {
 public:
  static const std::string GET;
  static const std::string POST;

 public:
  std::string method;
  std::string uri;
  int http_version_major;
  int http_version_minor;
  HeaderList headerList;

 private:
  std::string body;

  friend class RequestParser;
  friend class RequestFormatter;

 public:
  Request() : Request("", "") {}
  Request(std::string _method, std::string _uri)
      : method(_method),
        uri(_uri),
        http_version_major(1),
        http_version_minor(1) {}

  void setBody(const std::string &data,
               const std::string contentType = "text/plain");
  void setJSONBody(const std::string &data);
  void setFormData(std::map<std::string, std::string> pairs);
};

}  // namespace http
}  // namespace base

#endif  // BASE_HTTP_REQUEST_HPP
