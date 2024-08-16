#include "Response.hpp"

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
	(void)req;
	
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

