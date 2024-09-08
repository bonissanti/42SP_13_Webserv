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

bool Client::saveUploadedFile(const string& filename, const std::vector<char>& fileContent, const string& directory) {
	if (!directoryExists(directory)) {
        if (mkdir(directory.c_str(), 0777) == -1) {
            setResponseData(INTERNAL_SERVER_ERROR, "", "text/plain", "500 Internal Server Error");
            std::cerr << "Error creating directory: " << directory << std::endl;
            return false;
        }
	}

    if (access(directory.c_str(), W_OK) == -1) {
        setResponseData(FORBIDDEN, "", "text/plain", "403 Forbidden");
        return false;
    }

	string finalFilename = filename.empty() ? "default_filename.txt" : filename;

	string path = directory + "/" + finalFilename;
    ofstream outFile(path.c_str(), ios::binary);
    if (!outFile) {
        setResponseData(INTERNAL_SERVER_ERROR, "", "text/plain", "500 Internal Server Error");
        std::cerr << "Error opening file for writing: " << path << std::endl; //remove later
        return false;
    }

    outFile.write(fileContent.data(), fileContent.size());
    outFile.close();
    if (!outFile.good()) {
        setResponseData(INTERNAL_SERVER_ERROR, "", "text/plain", "500 Internal Server Error");
        return false;
    }
    return true;
}

int Client::runPostMethod() {
    string contentType = _request.getHeader("content-type");
    _response.setHeader("Content-type", "text/plain");

    if (contentType.find("multipart/form-data") != string::npos) {
        map<string, std::vector<char> > formData = _request.getFormData();

        if (formData.find("filename") == formData.end() || formData.find("fileContent") == formData.end()) {
            setResponseData(BAD_REQUEST, "", "text/plain", "Missing form data");
            return BAD_REQUEST;
        }

        string filename = std::string(formData["filename"].begin(), formData["filename"].end());
        string uri = _request.getURI();
        string directory = "content" + uri; // Change this to your desired directory structure

        std::vector<char> fileContent = formData["fileContent"];
        string fileContentType = std::string(formData["contentType"].begin(), formData["contentType"].end());

        if (!saveUploadedFile(filename, fileContent, directory)) {
            return FORBIDDEN;
        }

        // Set the correct content type in the response
        _response.setHeader("Content-type", fileContentType);
    } else if (contentType.empty()) {
        setResponseData(BAD_REQUEST, "", "text/plain", "Missing Content-Type header");
        return BAD_REQUEST;
    } else {
        setResponseData(BAD_REQUEST, "", "text/plain", "Unsupported Content-Type");
        return BAD_REQUEST;
    }

    setResponseData(OK, "", "text/plain", "File uploaded successfully");
    return OK;
}
