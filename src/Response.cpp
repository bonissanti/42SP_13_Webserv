#include "../include/Response.hpp"

Response::Response() : _statusCode(200), _statusMessage("OK"), _body("") {}

Response::Response(const Request& request) {
    status_code_ = request.getStatusCode();
    status_message_ = getStatusMessage(status_code_);

    body_ = "Response to the request";
    headers_["Content-Type"] = "text/plain";
    
    stringstream ss;
    ss << body_.size();
    headers_["Content-Length"] = ss.str();
}

void Response::setStatusCode(int code) {
    _statusCode = code;
    _statusMessage = getStatusMessage(code);
}

void Response::setBody(const std::string& body) {
    body_ = body;
    
    stringstream ss;
    ss << body_.size();
    headers_["Content-Length"] = ss.str();
}

void Response::setHeader(const std::string& field, const std::string& value) {
    headers_[field] = value;
}

std::string Response::toString() const {
    std::string response;

    stringstream code;
    code << status_code_;

    response += "HTTP/1.1 " + code.str() + " " + status_message_ + "\r\n";
    for (map<string, string>::const_iterator header = headers_.begin(); header != headers_.end(); ++header) {
        response += header->first + ": " + header->second + "\r\n";
    }
    response += "\r\n" + body_;
    return response;
}

string Response::getStatusMessage(int code) const {
    switch (code) {
        case 200:
            return ("OK");
        case 400:
            return ("Bad request");
        case 403:
            return ("Forbidden");
        case 404:
            return ("Not found");
        case 405:
            return ("Method not allowed");
        case 500:
            return ("Internal server error");
        case 502:
            return ("Bad gateway");
        default:
            return ("Unknown status");
    }
}