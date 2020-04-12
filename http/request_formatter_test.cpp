#include "request_formatter.hpp"

#include <iostream>

#include "asio.hpp"

using namespace std;
using asio::io_context;
using base::http::Request;
using base::http::RequestFormatter;

void sendRequest(string host, Request& request) {
  io_context context;

  asio::ip::tcp::socket socket(context);
  asio::ip::tcp::resolver resolver(context);
  auto endpoints = resolver.resolve(host, "80");
  socket.connect(*(endpoints.begin()));

  std::string reqBuf = RequestFormatter::format(request);
  asio::write(socket, asio::buffer(reqBuf));
  cout << ">sent " << reqBuf.size() << " bytes request:\n"
       << reqBuf << std::endl;

  std::array<char, 8096> buffer;
  asio::error_code ec;
  auto nread = asio::read(socket, asio::buffer(buffer, sizeof(buffer)), ec);
  cout << ">got"
       << " " << nread << " byte response:\n";
  for (std::size_t i = 0; i < nread; ++i) {
    cout << buffer[i];
  }
  cout << "\n---------------------\n";
}

int main(int argc, char const* argv[]) {
  const string host = "httpbin.org";

  {
    Request get("GET", "/get");
    get.headerList.setHost(host);
    get.headerList.setConnectionClose();
    sendRequest(host, get);
  }

  {
    Request post("POST", "/post");
    post.headerList.setHost(host);
    post.headerList.setConnectionClose();
    post.setJSONBody("{}");
    sendRequest(host, post);
  }

  return 0;
}
