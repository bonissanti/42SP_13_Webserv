#include "../include/Response.hpp"

Response::Response() : _statusCode(200), _statusMessage("OK"), _body("") {}

Response::~Response() {}

Response::Response(Request& request) {
    status_code_ = request.getStatusCode();
    status_message_ = getStatusMessage(status_code_);

    string filePath = request.getPath();
    headers_["Content-Type"] = getMimeType(filePath);
    body_ = readFileContent(filePath);
    
    stringstream ss;
    ss << body_.size();
    headers_["Content-Length"] = ss.str();
}

bool ends_with(const std::string& str, const std::string& suffix) {
    if (suffix.size() > str.size()) return false;
    return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

string Response::getMimeType(const string& path) const {
    if (ends_with(path, ".html")) return "text/html";
    if (ends_with(path, ".css")) return "text/css";
    if (ends_with(path, ".js")) return "application/javascript";
    if (ends_with(path, ".jpg")) return "image/jpeg";
    if (ends_with(path, ".txt")) return "text/plain";
    return "application/octet-stream";
}

string Response::readFileContent(const string& path) {
    ifstream file(path.c_str());

    if (!file.is_open()) {
        setStatusCode(404);
        return "<html><body><h2>404 Not Found</h2></body></html>";
    }
    ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
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