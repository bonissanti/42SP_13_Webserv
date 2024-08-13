#include "Response.hpp"
#include <sstream>
#include <string>

Response::Response(Request& req){
	// runRedirect ou..
	callMethod(req);
}

Response::~Response(){

}

int	Response::getMethodIndex(string method){
	string types[] = {"GET", "POST", "DELETE"};

	for (size_t i = 0; i < 3; i++)
		if (types[i] == method)
			return (i);
	return (-1);
}

void Response::callMethod(Request& req){

	switch(getMethodIndex(req.getMethod())){
		case GET:
			runGetMethod(req);
		// case POST:
		// 	runPostMethod();
		// case DELETE:
		// 	runDeleteMethod();
	}
}

string setContentType(string filePath)
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
	if (index != string::npos){
		extension = filePath.substr(index);
		map<string, string>::iterator it = mimeTypes.begin();	
		
		for (; it != mimeTypes.end(); ++it)
		if (it->first == extension)
				cout << it->second << endl;
	}
	return ("text/plain");
}

static bool verifyPermission(const string& file){
	if (access(file.c_str(), F_OK) != 0)
		return (false);
	else if (access(file.c_str(), F_OK) != 0) 
		return (false);
	return (true);
}

string	Response::setResponseBody(Request& req){
	if (req._isCgi){
		_index = req.getServer().getRoute()[0].getIndex();

		if (_index.find(".py") != string::npos || _index.find(".php") != string::npos)
			return (executeCGI(req));	
	}
		
	ifstream file(_filePath.c_str());
	if (!file.is_open()){
		_statusCode = NOT_FOUND;
		return ("");
	}
	else if (file.fail()){
		_statusCode = INTERNAL_SERVER_ERROR;
		return ("");
	}
	stringstream buffer;
	buffer << file.rdbuf();
	
	if (verifyPermission(buffer.str())){
		_statusCode = FORBIDDEN;
		return ("");
	}
	file.close();
	return (buffer.str());	
}

size_t setContentLength(const string& body){
	ostringstream oss;
	
	oss << body;
	return (oss.str().size());
}

/*  */

void Response::runGetMethod(Request& req){
	if (req._path.empty()){
		_statusCode = BAD_REQUEST;
		return ;
	}
	_filePath = req._path;
	if (_filePath.length() - 1 == '/')
		_filePath += "index.html";
	
	_contentType = setContentType(_filePath);
	_contentType += "; charset=UTF-8";
	_body = setResponseBody(req);
	_contentLength = setContentLength(_body);
}

void Response::sendResponse(struct pollfd& pollFds, map<int, Request>& requests)
{
	Request &req = requests[pollFds.fd];
    Response resp(req);
    
    string hello =
    	"HTTP/1.1 200 OK\r\n"
    	"Content-Type: text/plain\r\n"
    	"Content-Length: 17\r\n"
    	"Connection: close\r\n"
    	"\r\n"
    	"Hello from server";

    send(pollFds.fd, hello.c_str(), hello.size(), 0);
    cout << "Message sent" << endl;
   
    requests.erase(pollFds.fd);
    close(pollFds.fd);
}

// void Response::setStatusCode(int code)
// {
//     _statusCode = code;
//     _statusMessage = getStatusMessage(code);
// }

// string Response::getStatusMessage(int code) const
// {
//     switch (code) {
//         // talvez tenha em excesso
//         case 200:
//             return ("OK");
//         case 400:
//             return ("Bad request");
//         case 403:
//             return ("Forbidden");
//         case 404:
//             return ("Not found");
//         case 405:
//             return ("Method not allowed");
//         case 500:
//             return ("Internal server error");
//         case 502:
//             return ("Bad gateway");
//         default:
//             return ("Unknown status");
//     }
// }