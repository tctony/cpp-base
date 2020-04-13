#include "request.hpp"

#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "url_decode.hpp"
#include "url_encode.hpp"

namespace base {
namespace http {

const std::string Request::GET = "GET";
const std::string Request::POST = "POST";

void Request::setBody(const std::string& data, const std::string contentType) {
  method = POST;
  body_ = data;
  headerList.setContentType(contentType);
  headerList.setContentLength(absl::StrCat(data.size()));
}

void Request::setJSONBody(const std::string& data) {
  setBody(data, "application/json");
}

void Request::setFormData(const FormDataType data) {
  std::vector<std::string> pairs(data.size());
  std::transform(data.begin(), data.end(), pairs.begin(),
                 [](decltype(data)::const_reference p) {
                   return absl::StrCat(p.first, "=", urlEncode(p.second));
                 });
  setBody(absl::StrJoin(pairs, "&"),
          "application/x-www-form-urlencoded;charset=utf-8");
}

Request::FormDataType Request::getFormData() {
  FormDataType data;

  auto pairs = absl::StrSplit(body_, "&");
  for (auto p : pairs) {
    auto kv = absl::StrSplit(p, absl::MaxSplits("=", 1));
    auto key = kv.begin();
    if ((*key).size() > 0) {
      auto value = ++kv.begin();
      data[std::string(*key)] =
          (value != kv.end() ? urlDecode(std::string(*value)) : "");
    }
  }

  return data;
}

}  // namespace http
}  // namespace base