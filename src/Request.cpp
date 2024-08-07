#include "../include/Request.hpp"
#include <algorithm>
#include <string>
#include <sstream> // Add this line to include the <sstream> header

// ...

//-------CONSTRUCTORS---------

Request::Request(const string &raw_request) {
    isCgi_ = false;
    readyForResponse = false;
    statusCode_ = 200;
    buffer_ = "";
    parseRequest(raw_request);
}

Request::Request() {
    isCgi_ = false;
    readyForResponse = false;
    statusCode_ = 200;
    buffer_ = "";
}

//---------DESTRUCTOR---------

Request::~Request() {}

//----------GETTERS-----------

string Request::getMethod() const {
    return method_;
}

string Request::getPath() const {
    return path_;
}

string Request::getVersion() const {
    return version_;
}

string Request::getBody() const {
    return body_;
}

string Request::getHeader(const string &field) const {
    map<string, string>::const_iterator it = headers_.find(field);
    if (it != headers_.end()) {
        return it->second;
    }
    return "";
}

map<string, string> Request::getHeaders() const {
    return headers_;
}

bool Request::getIsCgi() const {
    return isCgi_;
}

int Request::getStatusCode() const {
    return statusCode_;
}

//---------MEMBER FUNCTIONS----------
//---------PARSER FUNCTIONS----------

// Member functions
void Request::parseRequest(const string &raw_request) {
    buffer_ .append(raw_request);
    cout<< buffer_;

    if (buffer_.find("\r\n\r\n") == string::npos) {
        return;
    }

    cout << "Processing request..." << endl;

    istringstream request_stream(buffer_);
    string line;

    if (!getline(request_stream, line) || line.empty()) {
        statusCode_ = 400;
        std::cout << "Invalid request line";
        buffer_.clear();
        return;
    }

    parseRequestLine(line);
    parseHeaders(request_stream);
    parseBody(request_stream);
    
    if (!validateRequest()) {
        std::cout << "Invalid request";
        buffer_.clear();
        statusCode_ = 400;
        return;
    }

    setReadyForResponse(true);
    buffer_.clear();
}

void Request::parseRequestLine(const string &line) {
    istringstream line_stream(line);
    if (!(line_stream >> method_ >> path_ >> version_)) {
        statusCode_ = 400;
        std::cout << "Invalid request line format";
    }
    transform(method_.begin(), method_.end(), method_.begin(), ::toupper);
}

void Request::parseHeaders(istringstream &request_stream) {
    string line;
    while (getline(request_stream, line) && line != "\r") {
        size_t colon_pos = line.find(':');
        if (colon_pos != string::npos) {
            string key = line.substr(0, colon_pos);
            string value = line.substr(colon_pos + 2, line.length() - colon_pos - 3);
            transform(key.begin(), key.end(), key.begin(), ::tolower);
            headers_[key] = value;
        } else {
            statusCode_ = 400;
            std::cout << "Invalid header format";
        }
    }
}

void Request::parseBody(istringstream &request_stream) {
    getline(request_stream, body_, '\0');
}

void Request::isCgiRequest() {
    if (path_.find("/cgi-bin/") != std::string::npos)
        isCgi_ = true;
}

void Request::appendMessage(const string &raw_request) {
    buffer_ += raw_request;
}

//---------VALIDATION FUNCTIONS----------

bool Request::validateMethod() const {
    static vector<string> valid_methods;
    valid_methods.push_back("GET");
    valid_methods.push_back("POST");
    valid_methods.push_back("DELETE");

    if (find(valid_methods.begin(), valid_methods.end(), method_) == valid_methods.end()) {
        // cout << "Error: invalid method" << endl;
        return false;
    }
    return true;
}

bool Request::validateHeaders() const {
    if (headers_.find("host") == headers_.end()) {
        // cout << "Error: missing Host" << endl;
        return false;
    }

    if (method_.compare("POST") == 0 && headers_.find("content-length") == headers_.end()) {
        // cout << "Error: missing Content-Length" << endl;
        return false;
    }
    return true;
}

bool Request::validateVersion() const {
    if (version_ != "HTTP/1.1" && version_ != "HTTP/1.0") {
        // cout << "Error: invalid HTTP version" << endl;
        return false;
    }
    return true;
}

// string generateErrorResponse(int statusCode) {
    
//     map<int, string> statusMessages;
//     statusMessages.insert(std::make_pair(400, "Bad Request"));
//     statusMessages.insert(std::make_pair(404, "Not Found"));
//     statusMessages.insert(std::make_pair(500, "Internal Server Error"));

//     string statusMessage = statusMessages[statusCode];
//     string response = "HTTP/1.1 " + string::to_string(statusCode) + " " + statusMessage + "\r\n";
//     response += "Content-Type: text/plain\r\n";
//     response += "Content-Length: " + string::to_string(statusMessage.length()) + "\r\n";
//     response += "\r\n";
//     response += statusMessage;
//     return response;
// }

bool Request::validateRequest() const {
    if (!validateMethod() || !validateVersion() || !validateHeaders()) {
        return false;
    }
    return true;
}

//-----------UTILS-------------

void Request::printRequest() const {
    cout << method_ << " " << path_ << " " << version_ << endl;

    vector<string> key, value;
    for (map<string, string>::const_iterator it = headers_.begin(); it != headers_.end(); ++it) {
        cout << it->first << ": " << it->second << endl;
    }

    cout << body_ << endl;
}

bool Request::isReadyForResponse() const {
    return readyForResponse;
}

void Request::setReadyForResponse(bool ready) {
    readyForResponse = ready;
}