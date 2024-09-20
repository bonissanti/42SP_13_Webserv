#include "Response.hpp"
#include "../Utils/Utils.hpp"

Response::Response() 
    : _statusCode(0), _index(""), _statusMessage(""), _responseBody(""),
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
    _statusMessage.clear();
    _responseBody.clear();
    _filePath.clear();
    _contentType.clear();
    _contentLength.clear();
    _headers.clear();
}

string Response::setCreatedBody(const string& filePath) {
    string body = "Created Successfully\n"
				 + filePath;
    return body;
}

string Response::getStatusPage(int statusCode) {
	switch(statusCode)
    {
		case 200:
            return BODY_200;
        case 201:
            return BODY_201;
        case 204:
            return BODY_204;
        case 301:
            return Utils::readFile(ERROR301);
        case 304:
            return Utils::readFile(ERROR304);
        case 400:
            return Utils::readFile(ERROR400);
        case 403:
            return Utils::readFile(ERROR403);
        case 404:
            return Utils::readFile(ERROR404);
        case 405:
            return Utils::readFile(ERROR405);
        case 413:
            return Utils::readFile(ERROR413);
        case 500:
            return Utils::readFile(ERROR500);
        case 501:
            return Utils::readFile(ERROR501);
        case 502:
            return Utils::readFile(ERROR502);
        case 505:
            return Utils::readFile(ERROR505);
        default:
            return "<html><body><h1>Unknown Status</h1></body></html>";
    }
}