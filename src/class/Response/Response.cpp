#include "Response.hpp"

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

/*  */

void Response::runGetMethod(Request& req){
	if (req._path.empty()){
		_statusCode = BAD_REQUEST;
		return ;
	}
	
	string filePath = req._path;
	if (filePath.length() - 1 == '/')
		filePath += "index.html";
	
	string contentType = setContentType(filePath);
	contentType += "; charset=UTF-8";
	
}

void Response::sendResponse(vector<struct pollfd>& pollFds, int i, map<int, Request>& requests)
{
	Request &req = requests[pollFds[i].fd];
	string hello =
    	"HTTP/1.1 200 OK\r\n"
    	"Content-Type: text/plain\r\n"
    	"Content-Length: 17\r\n"
    	"Connection: close\r\n"
    	"\r\n"
    	"Hello from server";


    Response resp(req);
    //  switch(runMethod(req.getMethod()))
    // {
    // 	case GET:
    //  		Response runGet();
    // 	case POST:
    // 		Response runPost();
    // 	case DELETE:
    // 		Response runDelete();
    // }
    send(pollFds[i].fd, hello.c_str(), hello.size(), 0);
    cout << "Message sent" << endl;
   
    requests.erase(pollFds[i].fd);
    close(pollFds[i].fd);
    pollFds.erase(pollFds.begin() + i);
    (void)req;
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