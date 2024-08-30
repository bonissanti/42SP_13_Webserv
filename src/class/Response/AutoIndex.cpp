#include <dirent.h>
#include "Response.hpp"

static string generateDirectoryList(string& list){
	string response = 
	"<!DOCTYPE html>\n"
	"<html lang='en'>\n"
	"<head>\n"
	"<meta charset='UTF=8'>\n"
	"<h1><b>Directory listing (autoindex)</b></h1>\n"
	"</head>\n"
	"<h5>Files: \n</h5>"
 	+ list;
	
	return (response);
}

inline static string applyHiperlink(const string& href, const string& fileName) {
    string buffer;
    
    buffer = "<li><a href=\"" + href + fileName + "\">" + fileName + "</a></li>";
    return buffer;
}

string Response::handleAutoIndex(const string& filePath, const string& uri){
	
	string list, href;
	DIR *dir = opendir(filePath.c_str());
	if (!dir){
		cerr << RED << "Error: opendir failed" << RESET << endl;
		_statusCode = INTERNAL_SERVER_ERROR;
		return ("");
	}
	href = uri;
	if (uri[uri.length() - 1] != '/')
		href += "/";
	struct dirent *entry;
	while ((entry = readdir(dir))){
		if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")){
			list += applyHiperlink(href, entry->d_name);
		}
	}
	closedir(dir);
	return(generateDirectoryList(list));
}
