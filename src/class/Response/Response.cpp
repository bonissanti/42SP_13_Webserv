#include "Response.hpp"
#include "../Utils/Utils.hpp"

Response::Response() 
    : _statusCode(0), _index(""), _executor(""), _statusMessage(""), _responseBody(""),
      _filePath(""), _contentType(""), _contentLength(""), _location("")
{
}

Response::~Response() {}


string Response::buildMessage(void)
{
    string response =
        "HTTP/1.1 " + Utils::statusCodeToString(_statusCode) + "\r\n"
        "Access-Control-Allow-Credentials: true\r\n"
        "Access-Control-Allow-Headers: Content-Type, Authorization\r\n"
        "Access-Control-Allow-Methods: DELETE, GET, POST\r\n"
        "Content-Length:" + _contentLength +
        "\r\n"
        "Content-Type:" + _contentType +
        "\r\n"
        "Location:" + _location + 
        "\r\n"
        "Connection: close" +
        "\r\n"
        "\r\n" +
        _responseBody +
        "\r\n";

    return (response);
}

void Response::clear() {
    _statusCode = 0;
    _index.clear();
    _executor.clear();
    _statusMessage.clear();
    _responseBody.clear();
    _filePath.clear();
    _contentType.clear();
    _contentLength.clear();
    _headers.clear();
}



