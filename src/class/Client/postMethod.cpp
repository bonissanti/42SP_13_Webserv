#include "../Response/Response.hpp"
#include "../../../include/Utils.hpp"
#include "Client.hpp"
#include <sys/stat.h>

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

map<string, string> parseMultipartData(const string& body, const string& boundary) {
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
            if (!name.empty()) {
                formData["name"] = name;
            }
            if (!name.empty()) {
                formData[name] = content;
            }
            if (!filename.empty()) {
                formData["filename"] = filename;
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

bool Client::saveUploadedFile(const string& filename, const string& fileContent, const string& directory) {
	if (!directoryExists(directory)) {
        if (mkdir(directory.c_str(), 0777) == -1) {
            std::cerr << "Error creating directory: " << directory << std::endl;
            return false;
        }
	}

    if (access(directory.c_str(), W_OK) == -1) {
        _response.setStatusCode(FORBIDDEN);
        _response.setStatusMessage("Forbidden");
        _response.setResponseBody("Directory is not writable");
        _response.setContentLength(_response.defineContentLength("Directory is not writable"));
        _response.setHeader("Content-length", _response.defineContentLength("Directory is not writable"));
        _response.setHeader("Content-type", "text/plain");
        return false;
    }

	string finalFilename = filename.empty() ? "default_filename.txt" : filename;
	cout << "Final filename: " << finalFilename << endl; //remove later

	string path = directory + "/" + finalFilename;
    ofstream outFile(path.c_str(), ios::binary);
    if (!outFile) {
        std::cerr << "Error opening file for writing: " << path << std::endl; //remove later
        return false;
    }

    outFile.write(fileContent.c_str(), fileContent.size());
    outFile.close();
    if (!outFile) {
        return false;
    }
    return true;
}

int Client::runPostMethod() {
    string contentType = _request.getHeader("content-type");
    _response.setHeader("Content-type", "text/plain");
    if (contentType.find("multipart/form-data") != string::npos || contentType.find("multipart/form") != string::npos) {
        string boundary = contentType.substr(contentType.find("boundary=") + 9);
        map<string, string> formData = parseMultipartData(_request.getBody(), boundary);

		string filename = formData["filename"];
        string uri = _request.getURI();
		string directory = "content" + uri + "uploads";
        if (!saveUploadedFile(filename, formData[formData["name"]], directory)) {
            return FORBIDDEN;
        }
    } else {
        // Handle other POST data
        string responseBody = "Unsupported Content-Type";
        _response.setStatusCode(BAD_REQUEST);
        _response.setStatusMessage("Bad Request");
        _response.setResponseBody(responseBody);
        _response.setHeader("Content-length", Utils::toString(responseBody.length()));
        return BAD_REQUEST;
    }

    string responseBody = "File uploaded successfully";
    _response.setStatusCode(OK);
    _response.setStatusMessage("OK");
    _response.setResponseBody(responseBody);
    _response.setHeader("Content-length", Utils::toString(responseBody.length()));
    return OK;
}