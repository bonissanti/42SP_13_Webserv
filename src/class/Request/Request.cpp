#include "Request.hpp"

Request::Request(Server& server) : _server(server), _isCgi(false), _readyForResponse (false), _statusCode(OK) {}

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

int hexStringToInt(const std::string& hexStr) {
    int result;
    std::istringstream iss(hexStr);

    if (hexStr.size() > 2 && (hexStr[0] == '0' && (hexStr[1] == 'x' || hexStr[1] == 'X'))) {
        iss.ignore(2);
    }

    iss >> std::hex >> result;

    if (iss.fail() || !iss.eof()) {
        throw std::invalid_argument("Invalid hexadecimal string: " + hexStr);
    }

    return result;
}

bool Request::isRequestComplete() {
    size_t header_end = _buffer.find("\r\n\r\n");
    if (header_end == string::npos) {
        return false;
    }

    istringstream header_stream(_buffer.substr(0, header_end));
    string line;
    size_t content_length = 0;
    bool is_chunked = false;
    while (getline(header_stream, line) && !line.empty()) {
        size_t colon_pos = line.find(':');
        if (colon_pos != string::npos) {
            string header_name = line.substr(0, colon_pos);
            string header_value = line.substr(colon_pos + 1);

            header_value.erase(0, header_value.find_first_not_of(" \t"));
            header_value.erase(header_value.find_last_not_of(" \t") + 1);
            _headers[header_name] = header_value;

            if (header_name == "Content-Length") {
                content_length = Utils::strtoi(header_value);
            } else if (header_name == "Transfer-Encoding" && header_value.find("chunked") != string::npos) {
                is_chunked = true;
            }
        }
    }

    size_t body_start = header_end + 4;
    if (is_chunked) {
        _totalChunkedLength = 0;

        size_t pos = body_start;
        while (true) {
            size_t chunk_size_end = _buffer.find("\r\n", pos);
            if (chunk_size_end == string::npos) {
                return false;
            }

            string chunk_size_str = _buffer.substr(pos, chunk_size_end - pos);

            size_t chunk_size = 0;
            try {
                chunk_size = hexStringToInt(chunk_size_str);
            } catch (const std::invalid_argument &e) {
                return true;
            }
            pos = chunk_size_end + 2;
            if (chunk_size == 0) {
                size_t last_chunk_end = _buffer.find("\r\n", pos);
                if (last_chunk_end != string::npos) {
                    if (_headers.find("content-length") == _headers.end()) {
						std::stringstream ss;
						ss << _totalChunkedLength;
						_headers["content-length"] = ss.str();
                    }
                    return true;
                }
                return false;
            }

            if (pos + chunk_size + 2 > _buffer.size()) {
                return false;
            }

            _totalChunkedLength += chunk_size;
            pos += chunk_size + 2;
        }
    } else {
        size_t body_length = _buffer.size() - body_start;
        return body_length >= content_length;
    }
}

void Request::parseRequest(const string &raw_request) {
    _buffer.append(raw_request);

    if (!isRequestComplete()) {
        return;
    }

    cout << GREEN << "Processing request..." << RESET << endl;

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

    validateRequest();

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
    vector<char> partContent;
    string filename;
    string contentType;
    bool isFileContent = false;
    bool isHeader = true;

    std::string accumulated_content;

    while (request_stream.read(buffer, buffer_size) || request_stream.gcount() > 0) {
        size_t bytes_read = request_stream.gcount();
        string chunk(buffer, bytes_read);
        accumulated_content += chunk;

        size_t boundaryPos = accumulated_content.find(boundary);
        while (boundaryPos != string::npos) {

            if (!partContent.empty() && !filename.empty()) {
                _formData["filename"] = vector<char>(filename.begin(), filename.end());
                _formData["fileContent"] = partContent;
                _formData["contentType"] = vector<char>(contentType.begin(), contentType.end());
            }

            partContent.clear();
            filename.clear();
            contentType.clear();
            isFileContent = false;
            isHeader = true;

            string content_up_to_boundary = accumulated_content.substr(0, boundaryPos + boundary.size());

            size_t header_end = content_up_to_boundary.find("\r\n\r\n");
            if (header_end != string::npos) {
                string headers = content_up_to_boundary.substr(0, header_end);
                isHeader = false;

                size_t filename_pos = headers.find("filename=\"");
                if (filename_pos != string::npos) {
                    filename_pos += 10;
                    size_t filename_end = headers.find("\"", filename_pos);
                    filename = headers.substr(filename_pos, filename_end - filename_pos);
                }

                size_t type_pos = headers.find("Content-Type:");
                if (type_pos != string::npos) {
                    type_pos += 14;
                    size_t type_end = headers.find("\r\n", type_pos);
                    contentType = headers.substr(type_pos, type_end - type_pos);
                    isFileContent = true;
                }
            }

            if (isFileContent && !isHeader) {
                size_t start_of_file_content = header_end + 4;
                partContent.insert(partContent.end(),
                    content_up_to_boundary.begin() + start_of_file_content,
                    content_up_to_boundary.end());
            }

            accumulated_content = accumulated_content.substr(boundaryPos + boundary.size());
            boundaryPos = accumulated_content.find(boundary);
        }
    }

    if (!accumulated_content.empty() && isFileContent && !isHeader) {
        partContent.insert(partContent.end(), accumulated_content.begin(), accumulated_content.end());
    }

    if (!partContent.empty() && !filename.empty()) {
        _formData["filename"] = vector<char>(filename.begin(), filename.end());
        _formData["fileContent"] = partContent;
        _formData["contentType"] = vector<char>(contentType.begin(), contentType.end());
    }
}

void Request::parseBody(istringstream &requestStream) {
    string transferEncoding = getHeader("transfer-encoding");
    if (transferEncoding.find("chunked") != string::npos) {
        parseChunkedBody(requestStream);
    } else {
        string contentType = getHeader("content-type");
        if (contentType.find("multipart/form-data") != string::npos) {
            size_t boundaryPos = contentType.find("boundary=");
            if (boundaryPos == string::npos) {
                _statusCode = BAD_REQUEST;
                std::cout << "Boundary not found in Content-Type" << std::endl;
                return;
            }
            string boundary = "--" + contentType.substr(boundaryPos + 9);
            parseMultidata(requestStream, boundary);
        } else {
            getline(requestStream, _body, '\0');
        }
    }
}

void Request::parseChunkedBody(istringstream &requestStream) {
    string line;
    while (getline(requestStream, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r') {
            line.erase(line.size() - 1);
        }

        size_t chunkSize = 0;
        try {
            chunkSize = hexStringToInt(line);
        } catch (const std::invalid_argument &e) {
            _statusCode = BAD_REQUEST;
            return;
        }

        if (chunkSize == 0) {
            break;
        }

        char *buffer = new char[chunkSize];
        requestStream.read(buffer, chunkSize);
        _body.append(buffer, chunkSize);
        delete[] buffer;

        getline(requestStream, line);
        if (line != "\r" && line != "") {
            _statusCode = BAD_REQUEST;
            return;
        }
    }
}

bool Request::validateRequest() //mudar para void ou HttpCode
{
    static vector<string> validMethods;
    validMethods.push_back("GET");
    validMethods.push_back("POST");
    validMethods.push_back("DELETE");

    if (find(validMethods.begin(), validMethods.end(), _method) == validMethods.end()) {
        _statusCode = NOT_IMPLEMENTED;
        return false;
    }
    if (_version != "HTTP/1.1" && _version != "HTTP/1.0") {
        _statusCode = VERSION_NOT_SUPPORTED;
        return false;
    }

    if (_headers.find("host") == _headers.end()) {
        _statusCode = BAD_REQUEST;
        return false;
    }

    if (_headers.find("content-length") != _headers.end() && Utils::strtoi(_headers["content-length"]) > _server.getMaxBodySize()) {
        _statusCode = PAYLOAD_TOO_LARGE;
        return false;
    }

    if (_method.compare("POST") == 0 && _headers.find("content-length") == _headers.end() && _headers.find("transfer-encoding") == _headers.end()) {
        _statusCode = BAD_REQUEST;
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
    // for (map<string, vector<char> >::const_iterator it = _formData.begin(); it != _formData.end(); ++it) {
    //     cout << it->first << ": ";
    //     for (map<string, vector<char> >::const_iterator it = _formData.begin(); it != _formData.end(); ++it) {
    //         cout << it->first << ": ";
    //         for (vector<char>::const_iterator iter = it->second.begin(); iter != it->second.end(); ++iter) {
    //             char c = *iter;
    //             cout << c;
    //         }
    //         cout << endl;
    //     }
    //     cout << endl;
    // }
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
    _statusCode = HttpStatus();
}