#include "Response.hpp"

Response::Response(){

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

void Response::callMethod(Request req){

	switch(getMethodIndex(req.getMethod())){
		case GET:
			runGetMethod(req);
		// case POST:
		// 	runPostMethod();
		// case DELETE:
		// 	runDeleteMethod();
	}
}

void Response::runGetMethod(Request req){
	string filePath = req._path;
	
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