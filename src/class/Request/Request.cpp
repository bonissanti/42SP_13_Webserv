#include "Request.hpp"

Request::Request(const string &raw_request) {
    isCgi_ = false;
    statusCode_ = 200;
    parseRequest(raw_request);
}
    
Request::Request() : isCgi_(false) {
    statusCode_ = 200;
}

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

bool Request::getIsCgi() const {
    return isCgi_;
}

int Request::getStatusCode() const {
    return statusCode_;
}

map<int, Request> Request::getRequest() const {
	return requests_;
}

void Request::parseRequest(const string &raw_request) {
    istringstream request_stream(raw_request);
    string line;

    if (!getline(request_stream, line) || line.empty()) {
        statusCode_ = 400;
        throw runtime_error("Invalid request line");
    }

    parseRequestLine(line);
    parseHeaders(request_stream);
    parseBody(request_stream);
    
    if (!validateRequest()) {
        statusCode_ = 400;
    }
}

void Request::parseRequestLine(const string &line) {
    istringstream line_stream(line);
    if (!(line_stream >> method_ >> path_ >> version_)) {
        statusCode_ = 400;
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
            statusCode_ = 400;
            throw runtime_error("Invalid header format");
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
    static vector<string> valid_methods;
    valid_methods.push_back("GET");
    valid_methods.push_back("POST");
    valid_methods.push_back("DELETE");

    if (find(valid_methods.begin(), valid_methods.end(), method_) == valid_methods.end()) {
        // cout << "Error: invalid method" << endl;
        return false;
    }
    if (version_ != "HTTP/1.1") {
        // cout << "Error: invalid HTTP version" << endl;
        return false;
    }
        if (headers_.find("host") == headers_.end()) {
        // cout << "Error: missing Host" << endl;
        return false;
    }

    if (method_.compare("POST") == 0 && headers_.find("content-length") == headers_.end()) {
        // cout << "Error: missing Content-Length" << endl;
        return false;
    }

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

// bool Request::validateRequest(string& errorResponse) const {
//     if (!validateMethod() || !validateVersion() || !validateHeaders()) {
//         errorResponse = generateErrorResponse(400);
//         return false;
//     }
//     return true;
// }

//-----------UTILS-------------

void Request::printRequest() const {
    cout << method_ << " " << path_ << " " << version_ << endl;

    vector<string> key, value;
    for (map<string, string>::const_iterator it = headers_.begin(); it != headers_.end(); ++it) {
        cout << it->first << ": " << it->second << endl;
    }
    cout << body_ << endl;
}

void Request::readRequest(vector<struct pollfd>& pollFds, int i)
{
    char buffer[65535];
    ssize_t bytesReceived = recv(pollFds[i].fd, buffer, sizeof(buffer), 0);


    if (bytesReceived > 0) {
        // Concatena a nova parte da requisição à existente
        requests_[pollFds[i].fd].append(buffer, bytesReceived);
        
        // Verifica se a requisição está completa (isso depende do protocolo usado, por exemplo, verificar headers HTTP)
        if (isRequestComplete(requests_[pollFds[i].fd])) {
            // Processa a requisição completa
            processRequest(requests_[pollFds[i].fd]);
            
            // Remove a requisição do mapa após o processamento
            requests_.erase(pollFds[i].fd);
            
            // Fecha o socket
            close(pollFds[i].fd);
            pollFds.erase(pollFds.begin() + i);
        }
    } else if (bytesReceived == 0) {
        cout << "Connection closed" << endl;
        requests_.erase(pollFds[i].fd);
        close(pollFds[i].fd);
        pollFds.erase(pollFds.begin() + i);
    } else {
        perror("Error: recv failed");
        requests_.erase(pollFds[i].fd);
        close(pollFds[i].fd);
        pollFds.erase(pollFds.begin() + i);
    }
}

bool Request::isRequestComplete(const std::string& request) {
    size_t headerEnd = request.find("\r\n\r\n");
    if (headerEnd == std::string::npos) {
        // Cabeçalho ainda não está completo
        return false;
    }

    // Se não há corpo na requisição, ela está completa
    if (headerEnd + 4 == request.size()) {
        return true;
    }

    // Procura pelo cabeçalho Content-Length para determinar o tamanho do corpo esperado
    size_t contentLengthPos = request.find("Content-Length:");
    if (contentLengthPos != std::string::npos) {
        // Extrai o valor de Content-Length
        size_t valueStart = contentLengthPos + 15; // 15 é o comprimento de "Content-Length:"
        size_t valueEnd = request.find("\r\n", valueStart);
        std::string contentLengthStr = request.substr(valueStart, valueEnd - valueStart);
        int contentLength = std::atoi(contentLengthStr.c_str());

        // Verifica se o corpo completo foi recebido
        size_t totalLength = headerEnd + 4 + contentLength;
        if (request.size() >= totalLength) {
            return true;
        } else {
            return false;
        }
    }

    // Caso não tenha Content-Length, verifica se há chunked transfer encoding
    size_t transferEncodingPos = request.find("Transfer-Encoding: chunked");
    if (transferEncodingPos != std::string::npos) {
        return true;
    }

    return true;
}