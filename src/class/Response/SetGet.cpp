#include "Response.hpp"

void Response::setStatusCode(int code){
    _statusCode = code;
}

void Response::setFilePath(string filePath){
    _filePath = filePath;
}

void Response::setContentType(string contentType){
    _contentType = contentType;
}

void Response::setContentLength(size_t length){
    _contentLength = length;
}

void Response::setHeaders(map<string, string> headers){
    _headers = headers;
}

void Response::setResponseBody(string responseBody){
    _responseBody = responseBody;
}

string Response::getFilePath(void) const{
    return (_filePath);
}