#include "Request.hpp"

Request::Request(const string &raw_request, Server &server)
    : _server(server), _isCgi(false), _readyForResponse(false), _statusCode(OK)
{
    parseRequest(raw_request);
}

Request::Request(const Request &toCopy)
{
    _headers = toCopy._headers;
    _method = toCopy._method;
    _uri = toCopy._uri;
    _version = toCopy._version;
    _body = toCopy._body;
    _buffer = toCopy._buffer;
    _server = toCopy._server;
    _isCgi = toCopy._isCgi;
    _readyForResponse = toCopy._readyForResponse;
    _statusCode = toCopy._statusCode;
}

Request::Request() : _isCgi(false), _readyForResponse(false), _statusCode(OK) {}

Request::~Request() {}

Request &Request::operator=(const Request &other)
{
    if (this != &other) {
        _server = other._server;
        _isCgi = other._isCgi;
        _statusCode = other._statusCode;
        _uri = other._uri;
        _version = other._version;
        _method = other._method;
        _headers = other._headers;
        _body = other._body;
    }
    return *this;
}

void Request::parseRequest(const string &raw_request)
{
    _buffer.append(raw_request);

    if (_buffer.find("\r\n\r\n") == string::npos) {
        return;
    }

    cout << "Processing request..." << endl;

    istringstream request_stream(_buffer);
    string line;

    if (!getline(request_stream, line) || line.empty()) {
        _statusCode = BAD_REQUEST;
        _buffer.clear();
        return;
    }

    parseRequestLine(line);
    parseHeaders(request_stream);
    parseBody(request_stream);

    if (!validateRequest()) {
        _statusCode = BAD_REQUEST;
    }

    _readyForResponse = true;
    _buffer.clear();
}

void Request::parseRequestLine(const string &firstLine)
{
    istringstream line_stream(firstLine);
    if (!(line_stream >> _method >> _uri >> _version)) {
        _statusCode = BAD_REQUEST;
    }
    transform(_method.begin(), _method.end(), _method.begin(), ::toupper);
}

void Request::parseHeaders(istringstream &request_stream)
{
    string line;
    while (getline(request_stream, line) && line != "\r") {
        size_t colon_pos = line.find(':');
        if (colon_pos != string::npos) {
            string key = line.substr(0, colon_pos);
            string value = line.substr(colon_pos + 2, line.length() - colon_pos - 3);
            transform(key.begin(), key.end(), key.begin(), ::tolower);
            _headers[key] = value;
        }
        else {
            _statusCode = BAD_REQUEST;
            // throw runtime_error("Invalid header format");
        }
    }
}

void Request::parseBody(istringstream &request_stream)
{
    _body.clear();
    getline(request_stream, _body, '\0');
}

bool Request::validateRequest() const
{
    static vector<string> valid_methods;
    valid_methods.push_back("GET");
    valid_methods.push_back("POST");
    valid_methods.push_back("DELETE");

    if (find(valid_methods.begin(), valid_methods.end(), _method) == valid_methods.end()) {
        // cout << "Error: invalid method" << endl;
        return false;
    }
    if (_version != "HTTP/1.1" && _version != "HTTP/1.0") {
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

void Request::printRequest() const
{
    cout << _method << " " << _uri << " " << _version << endl;

    vector<string> key, value;
    for (map<string, string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it) {
        cout << it->first << ": " << it->second << endl;
    }
    cout << _body << endl;
}

void Request::readRequest(struct pollfd &actualFd, Request &newRequest)
{
    char buffer[65535];
    ssize_t bytesReceived = recv(actualFd.fd, buffer, sizeof(buffer), 0);

    if (bytesReceived > 0) {
        newRequest.parseRequest(string(buffer, bytesReceived));
    }
    else if (bytesReceived == 0) {
        cout << "Connection closed" << endl;
        close(actualFd.fd);
    }
    else {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return;
        perror("Error: recv failed");
        close(actualFd.fd);
    }
}

bool Request::isReadyForResponse() const
{
    return _readyForResponse;
}

void Request::clear()
{
    _headers.clear();
    _method.clear();
    _uri.clear();
    _version.clear();
    _body.clear();
    _buffer.clear();
    _isCgi = false;
    _readyForResponse = false;
    _statusCode = HttpStatus();  // Assuming HttpStatus has a default constructor
}