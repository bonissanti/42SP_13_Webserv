#include "Request.hpp"

Request::Request(const string &raw_request, Server &server) : _server(server), _isCgi(false), _readyForResponse (false), _statusCode(OK)
{
    parseRequest(raw_request);
}

Request::Request() : _isCgi(false), _readyForResponse (false), _statusCode(OK) {}

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

void Request::parseRequest(const string &raw_request) {
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

    printRequest();

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

void Request::parseMultidata(istringstream &request_stream, const string &boundary) {
    string body;
    getline(request_stream, body, '\0');

    size_t pos = 0;
    string part_boundary = "--" + boundary;
    string end_boundary = part_boundary + "--";

    while ((pos = body.find(part_boundary)) != string::npos) {
        body.erase(0, pos + part_boundary.length() + 2); // +2 for \r\n

        if (body.find(end_boundary) == 0) {
            break; // End of multipart data
        }

        size_t part_end = body.find(part_boundary);
        string part = body.substr(0, part_end);
        body.erase(0, part_end);

        istringstream part_stream(part);
        string part_line;
        string filename;
        string part_content;
        // bool in_part = false;

        while (getline(part_stream, part_line)) {
            if (part_line.find("Content-Disposition:") != string::npos) {
                size_t filename_pos = part_line.find("filename=");
                if (filename_pos != string::npos) {
                    filename = part_line.substr(filename_pos + 10);
                    filename = filename.substr(0, filename.length() - 1); // Remove trailing quote
                }
            } else if (part_line == "\r" || part_line == "\n" || part_line.empty()) {
                // Skip empty part_lines
            } else {
                part_content += part_line + "\n";
            }
        }

        if (!part_content.empty() && part_content[part_content.length() - 1] == '\n') {
            part_content.erase(part_content.size() - 1);
        }

        if (!filename.empty()) {
            _formData[filename] = part_content;
        }
    }
}

void Request::parseBody(istringstream &request_stream) {
    string content_type = getHeader("content-type");
    if (content_type.find("multipart/form-data") != string::npos) {
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

void Request::readRequest(vector<struct pollfd> &pollFds, int i, map<int, Request> &requests)
{
    char buffer[65535];
    ssize_t bytesReceived = recv(pollFds[i].fd, buffer, sizeof(buffer), 0);
    if (bytesReceived > 0) {
        int fd = pollFds[i].fd;
        if (requests.find(fd) == requests.end())
            requests[fd] = Request();
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