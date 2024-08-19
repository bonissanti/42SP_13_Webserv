#include "Request.hpp"

Request::Request(const string &raw_request, Server& server) {
    _isCgi = false;
    _statusCode = OK;
   _server = server; 
    parseRequest(raw_request);
}
    
Request::Request() : _isCgi(false) {
    _statusCode = OK;
}

Request::~Request() {}

string Request::getHeader(const string &field) const {
    map<string, string>::const_iterator it = _headers.find(field);
    if (it != _headers.end()) {
        return it->second;
    }
    return "";
}

void Request::parseRequest(const string &raw_request) {
    istringstream request_stream(raw_request);
    string line;

    if (!getline(request_stream, line) || line.empty()) {
        _statusCode = BAD_REQUEST;
        throw runtime_error("Invalid request line");
    }
    
    parseRequestLine(line);
    parseHeaders(request_stream);
    parseBody(request_stream);
    
    if (!validateRequest()) {
    	_statusCode = BAD_REQUEST;
    }
}

void Request::parseRequestLine(const string &firstLine) {
	
	istringstream line_stream(firstLine);
    if (!(line_stream >> _method >> _uri >> _version)) {
        _statusCode = BAD_REQUEST;
        // throw runtime_error("Invalid request line format");
    }
    transform(_method.begin(), _method.end(), _method.begin(), ::toupper);
}

void Request::parseHeaders(istringstream &request_stream) {
    string line;
    while (getline(request_stream, line) && line != "\r") {
        size_t colon_pos = line.find(':');
        if (colon_pos != string::npos) {
            string key = line.substr(0, colon_pos);
            string value = line.substr(colon_pos + 2, line.length() - colon_pos - 3);
            transform(key.begin(), key.end(), key.begin(), ::tolower);
            _headers[key] = value;
        } else {
        	_statusCode = BAD_REQUEST;
            // throw runtime_error("Invalid header format");
        }
    }
}

void Request::parseBody(istringstream &request_stream) {
    getline(request_stream, _body, '\0');
}

void Request::isCgiRequest() {
    if (_uri.find("/cgi-bin/") != std::string::npos)
        _isCgi = true;
}

// string generateErrorResponse(int statusCode)     
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
    static vector<string> valid_methods;
    valid_methods.push_back("GET");
    valid_methods.push_back("POST");
    valid_methods.push_back("DELETE");

    if (find(valid_methods.begin(), valid_methods.end(), _method) == valid_methods.end()) {
        // cout << "Error: invalid method" << endl;
        return false;
    }
    if (_version != "HTTP/1.1") {
        // cout << "Error: invalid HTTP version" << endl;
        return false;
    }
        if (_headers.find("host") == _headers.end()) {
        // cout << "Error: missing Host" << endl;
        return false;
    }

    if (_method.compare("POST") == 0 && _headers.find("content-length") == _headers.end()) {
        // cout << "Error: missing Content-Length" << endl;
        return false;
    }

    if (_headers.find("host") == _headers.end()) {
        // cout << "Error: missing Host" << endl;
        return false;
    }

    if (_method.compare("POST") == 0 && _headers.find("content-length") == _headers.end()) {
        // cout << "Error: missing Content-Length" << endl;
        return false;
    }
    return true;
}

//-----------UTILS-------------

void Request::printRequest() const {
    cout << _method << " " << _uri << " " << _version << endl;

    vector<string> key, value;
    for (map<string, string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it) {
        cout << it->first << ": " << it->second << endl;
    }
    cout << _body << endl;
}

void Request::readRequest(vector<struct pollfd>& pollFds, int i, map<int, Request>& requests, Server& server)
{
    char buffer[65535];
    ssize_t bytesReceived = recv(pollFds[i].fd, buffer, sizeof(buffer), 0);
    if (bytesReceived > 0) {
      	requests[pollFds[i].fd] = Request(buffer, server);
    } 
    else if (bytesReceived == 0) {
        cout << "Connection closed" << endl;
        close(pollFds[i].fd);
        pollFds.erase(pollFds.begin() + i);
        requests.erase(pollFds[i].fd);
    } 
    else {
    	cerr << "Error: recv failed" << endl;
        requests.erase(pollFds[i].fd);
    }
    close(pollFds[i].fd);
    pollFds.erase(pollFds.begin() + i);
}

