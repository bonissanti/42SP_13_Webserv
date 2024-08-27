#include <dirent.h>
#include "Response.hpp"

static string generateDirectoryList(string& buffer){
	string response = 
	"<!DOCTYPE html>\n"
	"<html lang='en'>"
	"<head>"
	"<meta charset='UTF=8'"
	"<h1>Directory listing (autoindex)</h1>"
	"</head>"
	"<h5>Files: \n</h5>" 
}

inline static string applyHiperlink(const string& fileName) {
    string buffer;
    
    buffer = "<li><a href=\"" + fileName + "\">" + fileName + "</a></li>";
    return buffer;
}

string Response::handleAutoIndex(Request& req, const string& filePath){
	(void)req;
	(void)filePath;
	string buffer;
	DIR *dir = opendir(".");
	if (!dir){
		cerr << RED << "Error: opendir failed" << RESET << endl;
		_statusCode = INTERNAL_SERVER_ERROR;
		return ("");
	}
	buffer = "<ul>\n";
	struct dirent *entry;
	while ((entry = readdir(dir))){
		if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")){
			buffer += "<li>";
			buffer += entry->d_name;
			buffer += "</li>\n";
		}
	}
	buffer += "</ul>\n";
	closedir(dir);
	return(buffer.substr(0, buffer.length() - 1));
}
