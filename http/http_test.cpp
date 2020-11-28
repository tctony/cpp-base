#include <gtest/gtest.h>

#include <iostream>

#include "request_formatter.hpp"
#include "request_parser.hpp"
#include "response_parser.hpp"
#include "url_decode.hpp"
#include "url_encode.hpp"

using namespace base::http;
using base::Error;

const std::string host = "httpbin.org";

void ExpectEQRequest(const Request& a, const Request& b) {
  EXPECT_EQ(a.method, b.method);
  EXPECT_EQ(a.uri, b.uri);
  EXPECT_EQ(a.http_version_major, b.http_version_major);
  EXPECT_EQ(a.http_version_minor, b.http_version_minor);
  EXPECT_EQ(a.headerList.headers.size(), b.headerList.headers.size());
  EXPECT_EQ(a.body(), b.body());
}

TEST(Http, get_format_and_parse) {
  Request request("/get");
  request.headerList.setHost(host);
  request.headerList.setConnectionClose();

  std::string reqBuf = RequestFormatter::format(request);
  std::cout << reqBuf << std::endl;

  auto [error, parsedRequest] = RequestParser::parse(reqBuf);
  ExpectEQRequest(request, parsedRequest);
}

TEST(Http, post_format_and_parse) {
  Request request("/post", Request::POST);
  request.headerList.setHost(host);
  request.headerList.setConnectionClose();
  request.setJSONBody("{}");

  std::string reqBuf = RequestFormatter::format(request);
  std::cout << reqBuf << std::endl;

  auto [error, parsedRequest] = RequestParser::parse(reqBuf);
  ExpectEQRequest(request, parsedRequest);
}

TEST(Url, encode_decode) {
  std::string str("1aA_-.~ \n");

  auto encoded = urlEncode(str);
  std::cout << encoded << std::endl;

  auto decoded = urlDecode(encoded);
  std::cout << decoded << std::endl;

  EXPECT_EQ(str, decoded);
}

TEST(Http, formData) {
  Request request("/post");
  request.headerList.setHost(host);
  request.headerList.setConnectionClose();
  // will set method to POST
  std::map<std::string, std::string> data{{"name", "tony tang"},
                                          {"other", "$123"}};
  request.setFormData(data);

  std::string reqBuf = RequestFormatter::format(request);
  std::cout << reqBuf << std::endl;

  auto [error, parsedRequest] = RequestParser::parse(reqBuf);
  ExpectEQRequest(request, parsedRequest);

  auto parsedFormData = parsedRequest.getFormData();
  EXPECT_EQ(data, parsedFormData);
}

TEST(Http, parse_response_1) {
  std::string response =
      "HTTP/1.1 200 ok\r\n"
      "Connection: keep-alive\r\n"
      "Content-Length: 0\r\n"
      "\r\n";

  Response resp;
  ResponseParser parser(resp);

  auto [status, offset] = parser.parse(response.begin(), response.end());

  EXPECT_EQ(status, ResponseParser::good);
  EXPECT_EQ(offset, response.end());
}

TEST(Http, parse_response_2) {
  std::string response =
      "HTTP/1.1 200 ok\r\n"
      "Connection: keep-alive\r\n"
      "Content-Length: 5\r\n"
      "\r\n"
      "abcde";

  Response resp;
  ResponseParser parser(resp);

  auto [status, offset] = parser.parse(response.begin(), response.end());

  EXPECT_EQ(status, ResponseParser::good);
  EXPECT_EQ(offset, response.end());
}
