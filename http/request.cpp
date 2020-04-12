#include "request.hpp"

#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"

namespace base {

namespace http {

const std::string Request::GET = "GET";
const std::string Request::POST = "POST";

void Request::setBody(const std::string& data, const std::string contentType) {
  method = POST;
  body = data;
  headerList.setContentType(contentType);
  headerList.setContentLength(absl::StrCat(data.size()));
}

void Request::setJSONBody(const std::string& data) {
  setBody(data, "application/json");
}

void Request::setFormData(std::map<std::string, std::string> pairs) {
  // TODO
}

}  // namespace http

}  // namespace base