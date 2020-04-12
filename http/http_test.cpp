#include <gtest/gtest.h>

#include <iostream>

#include "request_formatter.hpp"
#include "request_parser.hpp"

using namespace base::http;
using base::Error;

const std::string host = "httpbin.org";

void ExpectEQRequest(const Request& a, const Request& b) {
  EXPECT_EQ(a.method, b.method);
  EXPECT_EQ(a.uri, b.uri);
  EXPECT_EQ(a.http_version_major, b.http_version_major);
  EXPECT_EQ(a.http_version_minor, b.http_version_minor);
  EXPECT_EQ(a.headerList.headers.size(), b.headerList.headers.size());
}

TEST(Request, get_format_and_parse) {
  Request request(Request::GET, "/get");
  request.headerList.setHost(host);
  request.headerList.setConnectionClose();

  std::string reqBuf = RequestFormatter::format(request);
  std::cout << reqBuf << std::endl;

  auto [error, parsedRequest] = RequestParser::parse(reqBuf);
  ExpectEQRequest(request, parsedRequest);
}

TEST(Request, post_format_and_parse) {
  Request request(Request::POST, "/post");
  request.headerList.setHost(host);
  request.headerList.setConnectionClose();
  request.setJSONBody("{}");

  std::string reqBuf = RequestFormatter::format(request);
  std::cout << reqBuf << std::endl;

  auto [error, parsedRequest] = RequestParser::parse(reqBuf);
  ExpectEQRequest(request, parsedRequest);
}