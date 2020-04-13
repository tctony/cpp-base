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
  const std::string& body() const { return body_; }

 private:
  std::string body_;

  friend class RequestParser;
  friend class RequestFormatter;

 public:
  Request() : Request("", "") {}
  Request(const std::string& _uri, const std::string& _method = GET)
      : method(_method),
        uri(_uri),
        http_version_major(1),
        http_version_minor(1) {}

  void setBody(const std::string& data,
               const std::string contentType = "text/plain");
  void setJSONBody(const std::string& data);

  using FormDataType = std::map<std::string, std::string>;
  void setFormData(const FormDataType pairs);
  FormDataType getFormData();
};

}  // namespace http
}  // namespace base

#endif  // BASE_HTTP_REQUEST_HPP
