#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"
#include "Request.hpp"
#include <iostream>

class RequestTest {
protected:
    Request request;
};

TEST_CASE_METHOD(RequestTest, "ParsesValidRequest") {
    std::string raw_request = "GET /index.html HTTP/1.1\r\n"
                              "Host: example.com\r\n"
                              "Connection: close\r\n\r\n";
    request.parseRequest(raw_request);

    REQUIRE(request.getMethod() == "GET");
    REQUIRE(request.getPath() == "/index.html");
    REQUIRE(request.getHeader("Host") == "example.com");
    REQUIRE(request.getHeader("Connection") == "close");
	
	request.printRequest();
}

TEST_CASE_METHOD(RequestTest, "ParsesRequestWithBody") {
    std::string raw_request = "POST /submit HTTP/1.1\r\n"
                              "Host: example.com\r\n"
                              "Content-Length: 11\r\n\r\n"
                              "Hello World";
    request.parseRequest(raw_request);

    REQUIRE(request.getMethod() == "POST");
    REQUIRE(request.getPath() == "/submit");
    REQUIRE(request.getHeader("Host") == "example.com");
    REQUIRE(request.getHeader("Content-Length") == "11");
    REQUIRE(request.getBody() == "Hello World");
	
	request.printRequest();
}