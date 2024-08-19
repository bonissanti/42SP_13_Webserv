#include "Response.hpp"
#include "../../../include/Utils.hpp"
#include <sys/stat.h>

Response::Response() {}

Response::~Response() {}

static bool verifyPermission(const string& file)
{
    if (access(file.c_str(), F_OK) != 0)
        return (false);
    else if (access(file.c_str(), F_OK) != 0)
        return (false);
    return (true);
}

string Response::defineFilePath(string& uri, Request& req){
	string filePath;
	// (void)req;

    if (uri.find("..") != string::npos) {
        _statusCode = FORBIDDEN;
        return ("");
    }
    _filePath = req.getServer().getRoot() + uri;
	filePath = _filePath; // refactor
	if (uri == "/")
		filePath = "content/html/index.html";	
	return (filePath);	
}

string Response::defineResponseBody(const string& filePath, Request& req)
{
    if (req._isCgi) {
        _index = req.getServer().getRoute()[0].getIndex();

        if (_index.find(".py") != string::npos || _index.find(".php") != string::npos)
            return (this->executeCGI(req));
    }

    ifstream file(filePath.c_str());
    if (!file.is_open()) {
        _statusCode = NOT_FOUND;
        return ("");
    }
    else if (file.fail()) {
        _statusCode = INTERNAL_SERVER_ERROR;
        return ("");
    }
    stringstream buffer;
    buffer << file.rdbuf();

    if (!verifyPermission(filePath)) {
        _statusCode = FORBIDDEN;
        return ("");
    }
    file.close();
    return (buffer.str());
}

size_t Response::defineContentLength(const string& body)
{
    ostringstream oss;

    oss << body;
    return (oss.str().size());
}

string Response::defineContentType(string filePath)
{
    size_t index;
    string extension;
    map<string, string> mimeTypes;
    mimeTypes[".html"] = "text/html";
    mimeTypes[".css"] = "text/css";
    mimeTypes[".txt"] = "text/plain";
    mimeTypes[".png"] = "image/png";
    mimeTypes[".jpg"] = "image/jpg";
    mimeTypes[".gif"] = "image/gif";
    mimeTypes[".js"] = "application/js";
    mimeTypes[".pdf"] = "application/pdf";

    index = filePath.rfind('.');
    if (index != string::npos) {
        extension = filePath.substr(index);
        map<string, string>::iterator it = mimeTypes.begin();

        for (; it != mimeTypes.end(); ++it)
            if (it->first == extension)
            	return (it->second + "; charset=UTF-8");
    }
    // string ret = + "; charset=UTF-8"; 
    return ("text/plain");
}

/*  */

// void Response::runGetMethod(Request& req){
// 	if (req._path.empty()){
// 		_statusCode = BAD_REQUEST;
// 		return ;
// 	}
	
// 	string filePath = req._path;
// 	if (filePath.length() - 1 == '/')
// 		filePath += "index.html";
	
// 	string contentType = defineContentType(filePath);
// 	contentType += "; charset=UTF-8";
// }

//-----------------POST FUNCTIONS----------------------

string getFilename(const string& contentDisposition) {
    size_t pos = contentDisposition.find("filename=");
    if (pos != string::npos) {
        size_t endPos = contentDisposition.find(";", pos);
        if (endPos == string::npos) {
            endPos = contentDisposition.length();
        }
        return contentDisposition.substr(pos + 10, endPos - pos - 10);
    }
    return "";
}

void Response::runPostMethod(Request& req) {

    string contentType = req.getHeader("content-type");
	cout << contentType << endl;
    if (contentType.find("multipart/form-data") != string::npos) {
        string boundary = contentType.substr(contentType.find("boundary=") + 9);
        map<string, string> formData = parseMultipartData(req.getBody(), boundary);

		string filename = getFilename(formData["filename"]);
		string directory = _filePath + formData["directory"]; //needs testing
        for (map<string, string>::const_iterator it = formData.begin(); it != formData.end(); ++it) {
            if (it->first == "file") {
                saveUploadedFile(filename, it->second, directory);
            }
        }

        _statusCode = OK;
        _statusMessage = "OK";
        _responseBody = "File uploaded successfully";
        _headers["Content-length"] = defineContentLength(_responseBody);
    } else {
        // Handle other POST data
        _statusCode = BAD_REQUEST;
        _statusMessage = "Bad Request";
        _responseBody = "Unsupported Content-Type";
        _headers["Content-length"] = defineContentLength(_responseBody);
    }

    _headers["Content-Type"] = "text/plain";
}

map<string, string> Response::parseMultipartData(const string& body, const string& boundary) {
    map<string, string> formData;
    string delimiter = "--" + boundary;
    size_t start = body.find(delimiter) + delimiter.length();
    size_t end = body.find(delimiter, start);

    while (end != string::npos) {
        string part = body.substr(start, end - start);
        size_t headerEnd = part.find("\r\n\r\n");
        string headers = part.substr(0, headerEnd);
        string content = part.substr(headerEnd + 4, part.length() - headerEnd - 6);

        // Parse the Content-Disposition header
        size_t dispositionStart = headers.find("Content-Disposition: ");
        if (dispositionStart != string::npos) {
            size_t dispositionEnd = headers.find("\r\n", dispositionStart);
            string disposition = headers.substr(dispositionStart + 20, dispositionEnd - dispositionStart - 20);

            // Extract the name and filename from the Content-Disposition header
            size_t nameStart = disposition.find("name=\"");
            size_t nameEnd = disposition.find("\"", nameStart + 6);
            string name = disposition.substr(nameStart + 6, nameEnd - nameStart - 6);

            size_t filenameStart = disposition.find("filename=\"");
            size_t filenameEnd = disposition.find("\"", filenameStart + 10);
            string filename = "";
            if (filenameStart != string::npos) {
                filename = disposition.substr(filenameStart + 10, filenameEnd - filenameStart - 10);
            }

            // Add the form field to the formData map
            if (!filename.empty()) {
                formData[name] = filename;
            } else {
                formData[name] = content;
            }
        }

        start = end + delimiter.length();
        end = body.find(delimiter, start);
    }

    return formData;
}

bool directoryExists(const std::string& path) {
    struct stat sb;
    if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)) {
        return true;
    }
    return false;
}

void Response::saveUploadedFile(const string& filename, const string& fileContent, const string& directory) {
	cout << "Saving file" << endl;
	if (!directoryExists(directory)) {
		mkdir(directory.c_str(), 0777);
	}

    if (!access(directory.c_str(), W_OK)) {
        _statusCode = FORBIDDEN;
        _statusMessage = "Forbidden";
        _responseBody = "Directory is not writable";
        _headers["Content-length"] = defineContentLength(_responseBody);
        return;
    }

	string finalFilename = filename.empty() ? "default_filename.txt" : filename;
	cout << "Final filename: " << finalFilename << endl;

	string path = directory + "/" + finalFilename;
    ofstream outFile(path.c_str(), ios::binary);
    outFile.write(fileContent.c_str(), fileContent.size());
    outFile.close();
}

// void Response::sendResponse(vector<struct pollfd>& pollFds, int i, map<int, Request>& requests)
// {
// 	Request &req = requests[pollFds[i].fd];
// 	string hello =
//     	"HTTP/1.1 200 OK\r\n"
//     	"Content-Type: text/plain\r\n"
//     	"Content-Length: 17\r\n"
//     	"Connection: close\r\n"
//     	"\r\n"
//     	"Hello from server";


//     Response resp(req);

// 	resp.setStatusCode(req.getStatusCode());
// 	resp.setBody(req.getBody());
// 	resp.setHeaders(req.getHeaders());

// 	string response = resp.getResponse();

// 	send(pollFds[i].fd, response.c_str(), response.size(), 0);
// 	cout << "Response sent" << endl;
//     // send(pollFds[i].fd, hello.c_str(), hello.size(), 0);
//     // cout << "Message sent" << endl;
   
//     requests.erase(pollFds[i].fd);
//     close(pollFds[i].fd);
//     pollFds.erase(pollFds.begin() + i);
//     (void)req;
// }