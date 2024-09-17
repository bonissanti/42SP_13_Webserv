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

string Response::getStatusPage(int statusCode) {
	switch(statusCode)
    {
        case 201:
            return BODY_201;
        case 204:
            return BODY_204;
        case 301:
            return BODY_301;
        case 304:
            return BODY_304;
        case 400:
            return BODY_400;
        case 403:
            return BODY_403;
        case 404:
            return BODY_404;
        case 405:
            return BODY_405;
        case 500:
            return BODY_500;
        case 501:
            return BODY_501;
        case 502:
            return BODY_502;
        case 505:
            return BODY_505;
        default:
            return "<html><body><h1>Unknown Status</h1></body></html>";
    }
}

