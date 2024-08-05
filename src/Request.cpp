#include "../include/Request.hpp"
#include <algorithm>
#include <set>
#include <string>
#include <sstream> // Add this line to include the <sstream> header
#include <string> // Add this line to include the <string> header

// ...

//-------CONSTRUCTORS---------

Request::Request(const string &raw_request) {
    parseRequest(raw_request);
}

Request::Request() {}

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

//---------MEMBER FUNCTIONS----------
//---------PARSER FUNCTIONS----------

// Member functions
void Request::parseRequest(const string &raw_request) {
    istringstream request_stream(raw_request);
    string line;

    if (!getline(request_stream, line) || line.empty()) {
        throw runtime_error("Invalid request line");
    }

    parseRequestLine(line);
    parseHeaders(request_stream);
    parseBody(request_stream);
}

void Request::parseRequestLine(const string &line) {
    istringstream line_stream(line);
    if (!(line_stream >> method_ >> path_ >> version_)) {
        throw runtime_error("Invalid request line format");
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
            throw runtime_error("Invalid header format");
        }
    }
}

void Request::parseBody(istringstream &request_stream) {
    getline(request_stream, body_, '\0');
}

//---------VALIDATION FUNCTIONS----------

bool Request::validateMethod() const {
    static const set<string> valid_methods = {"GET", "POST", "DELETE"};
    if (valid_methods.find(method_) == valid_methods.end()) {
        cout << "Error: invalid method" << endl;
        return false;
    }
    return true;
}

bool Request::validateHeaders() const {
    if (headers_.find("host") == headers_.end()) {
        cout << "Error: missing Host" << endl;
        return false;
    }

    if (method_.compare("POST") == 0 && headers_.find("content-length") == headers_.end()) {
        cout << "Error: missing Content-Length" << endl;
        return false;
    }
    return true;
}

bool Request::validateVersion() const {
    if (version_ != "HTTP/1.1") {
        cout << "Error: invalid HTTP version" << endl;
        return false;
    }
    return true;
}

string generateErrorResponse(int statusCode) {
    
    map<int, string> statusMessages;
    statusMessages.insert(std::make_pair(400, "Bad Request"));
    statusMessages.insert(std::make_pair(404, "Not Found"));
    statusMessages.insert(std::make_pair(500, "Internal Server Error"));

    string statusMessage = statusMessages[statusCode];
    string response = "HTTP/1.1 " + to_string(statusCode) + " " + statusMessage + "\r\n";
    response += "Content-Type: text/plain\r\n";
    response += "Content-Length: " + to_string(statusMessage.length()) + "\r\n";
    response += "\r\n";
    response += statusMessage;
    return response;
}

bool Request::validateRequest(string& errorResponse) const {
    if (!validateMethod()) {
        errorResponse = generateErrorResponse(400);
        return false;
    }
    if (!validateVersion()) {
        errorResponse = generateErrorResponse(400);
        return false;
    }
    if (!validateHeaders()) {
        errorResponse = generateErrorResponse(400);
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
