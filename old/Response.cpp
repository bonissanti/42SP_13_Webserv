#include "../include/Response.hpp"

Response::Response() : _statusCode(200), _statusMessage("OK"), _body("") {}

Response::~Response() {}

Response::Response(Request& request) {
    status_code_ = request.getStatusCode();
    status_message_ = getStatusMessage(status_code_);

    string filePath = request.getPath();
    cout << filePath << endl;
    headers_["Content-Type"] = getMimeType(filePath);
    cout << headers_["Content-Type"] << endl;
    body_ = readFileContent(filePath);
    
    stringstream ss;
    ss << body_.size();
    headers_["Content-Length"] = ss.str();
}

bool ends_with(const string& str, const string& suffix) {
    if (suffix.size() > str.size()) return false;
    return equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

string Response::getMimeType(const string& path) const {
    if (ends_with(path, ".html")) return "text/html";
    if (ends_with(path, ".css")) return "text/css";
    if (ends_with(path, ".js")) return "application/javascript";
    if (ends_with(path, ".jpg")) return "image/jpeg";
    if (ends_with(path, ".ico")) return "image/x-icon";
    if (ends_with(path, ".txt")) return "text/plain";
    return "application/octet-stream";
}

string Response::readFileContent(const string& path) {
    cout << "Here" << endl;
    string filePath = root_;
    filePath.append(path);
    ifstream file(filePath.c_str());

    cout << "Here again" << endl;
    if (!file.is_open()) {
        cout << "File isn't open" << endl;
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

void Response::setBody(const string& body) {
    body_ = body;
    
    stringstream ss;
    ss << body_.size();
    headers_["Content-Length"] = ss.str();
}

void Response::setHeader(const string& field, const string& value) {
    headers_[field] = value;
}

 void Response::setRoot(string& root) {
    root_ = root;
 }

string Response::generateResponse() const {
    ostringstream response;

    // Start with the status line
    if (status_code_ == 400) {
        response << "HTTP/1.0 400 Bad Request\r\n";
    } else if (status_code_ == 404) {
        response << "HTTP/1.0 404 Not Found\r\n";
    } else {
        response << "HTTP/1.1 200 OK\r\n";
    }

    // Add headers
    for (map<string, string>::const_iterator header = headers_.begin(); header != headers_.end(); ++header) {
        response << header->first << ": " << header->second << "\r\n";
    }

    // Add a blank line to separate headers from the body
    response << "\r\n";

    // Add the body
    response << body_;

    return response.str();
}

string Response::toString() const {
    string response;

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