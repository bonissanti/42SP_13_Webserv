#include "Request.hpp"

Request::Request(const string &raw_request) {
    parseRequest(raw_request);
}

Request::Request() {}

Request::~Request() {}

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

void Request::parseRequest(const string &raw_request) {
    istringstream request_stream(raw_request);
    string line;
    getline(request_stream, line);
    
    istringstream line_stream(line);
    line_stream >> method_ >> path_ >> version_;
    // getline(line_stream, version_, ' ');

    while (getline(request_stream, line) && line != "\r") {
        size_t colon_pos = line.find(':');
        if (colon_pos != string::npos) {
            string key = line.substr(0, colon_pos);
            string value = line.substr(colon_pos + 2, line.length() - colon_pos - 3);
            headers_[key] = value;
        }
    }

    //Get body
    getline(request_stream, body_, '\0');
}

bool Request::validateMethod() const {
    if (method_ != "GET" && method_ != "POST" && method_ != "DELETE") {
        cout << "Error: invalid method" << endl;
        return false;
    }

    return true;
}

bool Request::validateHeaders() const {
    if (headers_.find("Host") == headers_.end()) {
        cout << "Error: missing Host" << endl;
        return false;
    }

    if (method_.compare("POST") == 0 && headers_.find("Content-Length") == headers_.end()) {
        cout << "Error: missing Content-Length" << endl;
        return false;
    }

    return true;
}

bool Request::checkVersion() const {
    return true;
}

void Request::printRequest() const {
    cout << method_ << " " << path_ << " " << version_ << endl;
    vector<string> key, value;
    for (map<string, string>::const_iterator it = headers_.begin(); it != headers_.end(); ++it) {
        cout << it->first << ": " << it->second << endl;
    } 
    cout << body_ << endl;
}
      