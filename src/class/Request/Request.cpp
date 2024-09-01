#include "Request.hpp"

Request::Request(Server& server) : _server(server), _isCgi(false), _readyForResponse (false), _statusCode(OK) {}
Request::Request() {}
Request::~Request() {}

Request& Request::operator=(const Request &other)
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

bool Request::isRequestComplete() {
    size_t header_end = _buffer.find("\r\n\r\n");
    if (header_end == string::npos) {
        return false;
    }

    // Parse headers to find Content-Length
    istringstream header_stream(_buffer.substr(0, header_end));
    string line;
    while (getline(header_stream, line) && !line.empty()) {
        size_t colon_pos = line.find(':');
        if (colon_pos != string::npos) {
            string header_name = line.substr(0, colon_pos);
            string header_value = line.substr(colon_pos + 1);
            _headers[header_name] = header_value;
        }
    }

    // Check if Content-Length header is present
    map<string, string>::iterator it = _headers.find("Content-Length");
    if (it != _headers.end()) {
        int content_length = Utils::strtoi(it->second);
        if (_buffer.size() < header_end + 4 + content_length) {
            return false; // Request is not complete yet
        }
    }

    return true;
}

void Request::parseRequest(const string &raw_request) {
    _buffer.append(raw_request);
    static int i;

    if (!isRequestComplete()) {
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

    cout << "Count of requests: " << ++i << endl;

    parseRequestLine(line);
    parseHeaders(request_stream);
    parseBody(request_stream);

    if (!validateRequest()) {
        _statusCode = BAD_REQUEST;
    }

    // printRequest();

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

    while (getline(request_stream, line) && line != "\r" && line != "") {
        // Skip boundary extraction
        size_t colon_pos = line.find(':');
        if (colon_pos != string::npos) {
            string key = line.substr(0, colon_pos);
            string value = line.substr(colon_pos + 2, line.length() - colon_pos - 3);
            transform(key.begin(), key.end(), key.begin(), ::tolower);
            _headers[key] = value;
        }
        else 
        {
            _statusCode = BAD_REQUEST;
            break;
        }
    }
}

void Request::parseMultidata(istringstream &request_stream, const string &boundary) {
    string line;
    string part_content;
    string filename;

    while (getline(request_stream, line)) {
        if (line.find(boundary) != string::npos) {
            if (!part_content.empty() && !filename.empty()) {
                _formData[filename] = part_content;
            }
            part_content.clear();
            filename.clear();
        } else if (line.find("Content-Disposition: form-data; name=\"") != string::npos) {
            size_t start_pos = line.find("filename=\"");
            if (start_pos != string::npos) {
                start_pos += 10; // Move past 'filename="'
                size_t end_pos = line.find("\"", start_pos);
                filename = line.substr(start_pos, end_pos - start_pos);
            }
        } else {
            part_content += line + "\n";
        }
    }

    if (!part_content.empty() && !filename.empty()) {
        _formData[filename] = part_content;
    }
    
    // Print formData
    cout << "FormData:" << endl;
    for (map<string, string>::const_iterator it = _formData.begin(); it != _formData.end(); ++it) {
        cout << it->first << ": " << it->second << endl;
    }
}

void Request::parseBody(istringstream &request_stream) {
    string content_type = getHeader("content-type");
    if (content_type.find("multipart/form-data") != string::npos) {
        cout << "Parsing multipart/form-data body" << endl;
        size_t boundary_pos = content_type.find("boundary=");
        if (boundary_pos == string::npos) {
            _statusCode = BAD_REQUEST;
            std::cout << "Boundary not found in Content-Type" << std::endl;
            return;
        }
        string boundary = "--" + content_type.substr(boundary_pos + 9);
        parseMultidata(request_stream, boundary);
    } else {
        getline(request_stream, _body, '\0');
    }
}

bool Request::validateRequest()
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
    
    if (_uri.substr(0, 4) == "/cgi")
    	_isCgi = true;
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
    for (map<string, string>::const_iterator it = _formData.begin(); it != _formData.end(); ++it) {
        cout << it->first << ": " << it->second << endl;
    }
    cout << _body << endl;
}

void Request::readRequest(vector<struct pollfd> &pollFds, int i, map<int, Request> &requests, Server server)
{
    char buffer[65535];
    ssize_t bytesReceived = recv(pollFds[i].fd, buffer, sizeof(buffer), 0);
    if (bytesReceived > 0) {
        int fd = pollFds[i].fd;
        if (requests.find(fd) == requests.end())
            requests[fd] = Request(server);
        requests[fd].parseRequest(string(buffer, bytesReceived));
    } else if (bytesReceived == 0) {
        cout << "Connection closed" << endl;
        close(pollFds[i].fd);
        pollFds.erase(pollFds.begin() + i);
        requests.erase(pollFds[i].fd);
    } else {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return;
        perror("Error: recv failed");
        close(pollFds[i].fd);
        pollFds.erase(pollFds.begin() + i);
        requests.erase(pollFds[i].fd);
    }
}

bool Request::isReadyForResponse() const {
    return _readyForResponse;
}

void Request::clear() {
    _headers.clear();
    _method.clear();
    _uri.clear();
    _version.clear();
    _body.clear();
    _buffer.clear();
    _isCgi = false;
    _readyForResponse = false;
    _statusCode = HttpStatus(); // Assuming HttpStatus has a default constructor
}