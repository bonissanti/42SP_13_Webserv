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
        _formData = other._formData;
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
    size_t content_length = 0;
    while (getline(header_stream, line) && !line.empty()) {
        size_t colon_pos = line.find(':');
        if (colon_pos != string::npos) {
            string header_name = line.substr(0, colon_pos);
            string header_value = line.substr(colon_pos + 1);
            _headers[header_name] = header_value;

            if (header_name == "Content-Length") {
                content_length = Utils::strtoi(header_value);
            }
        }
    }

    // Check if the entire body is received
    size_t body_start = header_end + 4; // Move past "\r\n\r\n"
    size_t body_length = _buffer.size() - body_start;
    return body_length >= content_length;
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
    const size_t buffer_size = 65535;
    char buffer[buffer_size];
    vector<char> part_content;
    string filename;
    string content_type;
    bool is_file_content = false;
    bool is_header = true;

    while (request_stream.read(buffer, buffer_size) || request_stream.gcount() > 0) {
        size_t bytes_read = request_stream.gcount();
        string chunk(buffer, bytes_read);

        size_t pos = 0;
        while ((pos = chunk.find(boundary, pos)) != string::npos) {
            if (!part_content.empty() && !filename.empty()) {
                _formData["filename"] = vector<char>(filename.begin(), filename.end());
                _formData["fileContent"] = part_content;
                _formData["contentType"] = vector<char>(content_type.begin(), content_type.end());
            }
            part_content.clear();
            filename.clear();
            content_type.clear();
            is_file_content = false;
            is_header = true;
            pos += boundary.length();
        }

        if (is_header) {
            size_t header_end = chunk.find("\r\n\r\n");
            if (header_end != string::npos) {
                string headers = chunk.substr(0, header_end);
                chunk = chunk.substr(header_end + 4);
                is_header = false;

                size_t disposition_pos = headers.find("Content-Disposition: form-data; name=\"");
                if (disposition_pos != string::npos) {
                    size_t filename_pos = headers.find("filename=\"", disposition_pos);
                    if (filename_pos != string::npos) {
                        filename_pos += 10; // Move past 'filename="'
                        size_t filename_end = headers.find("\"", filename_pos);
                        filename = headers.substr(filename_pos, filename_end - filename_pos);
                    }
                }

                size_t type_pos = headers.find("Content-Type:");
                if (type_pos != string::npos) {
                    type_pos += 14; // Move past 'Content-Type: '
                    size_t type_end = headers.find("\r\n", type_pos);
                    content_type = headers.substr(type_pos, type_end - type_pos);
                    is_file_content = true;
                }
            }
        }

        if (is_file_content && !is_header) {
            part_content.insert(part_content.end(), chunk.begin(), chunk.end());
        }
    }

    if (!part_content.empty() && !filename.empty()) {
        _formData["filename"] = vector<char>(filename.begin(), filename.end());
        _formData["fileContent"] = part_content;
        _formData["contentType"] = vector<char>(content_type.begin(), content_type.end());
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
    for (map<string, vector<char> >::const_iterator it = _formData.begin(); it != _formData.end(); ++it) {
        cout << it->first << ": ";
        for (map<string, vector<char> >::const_iterator it = _formData.begin(); it != _formData.end(); ++it) {
            cout << it->first << ": ";
            for (vector<char>::const_iterator iter = it->second.begin(); iter != it->second.end(); ++iter) {
                char c = *iter;
                cout << c;
            }
            cout << endl;
        }
        cout << endl;
    }
    cout << _body << endl;
}

void Request::readRequest(struct pollfd &actualFd)
{
    char buffer[65535];
    ssize_t bytesReceived = recv(actualFd.fd, buffer, sizeof(buffer), 0);

    if (bytesReceived > 0) {
        parseRequest(string(buffer, bytesReceived));
    }
    else if (bytesReceived == 0) {
        _readyForResponse =
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