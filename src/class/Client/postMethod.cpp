#include "../Response/Response.hpp"
#include "../Utils/Utils.hpp"
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

bool directoryExists(const string& path) {
    struct stat sb;
    if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)) {
        return true;
    }
    return false;
}

bool Client::saveUploadedFile(const string& filename, const vector<char>& fileContent, const string& directory) {
	if (!directoryExists(directory)) {
        if (mkdir(directory.c_str(), 0777) == -1) {
            setResponseData(INTERNAL_SERVER_ERROR, "", "text/plain", "500 Internal Server Error", "");
            cerr << "Error creating directory: " << directory << endl;
            return false;
        }
	}

    if (access(directory.c_str(), W_OK) == -1) {
        setResponseData(FORBIDDEN, "", "text/plain", "403 Forbidden", "");
        return false;
    }

	string finalFilename = filename.empty() ? "default_filename.txt" : filename;

	string path = directory + "/" + finalFilename;
    ofstream outFile(path.c_str(), ios::binary);
    if (!outFile) {
        setResponseData(INTERNAL_SERVER_ERROR, "", "text/plain", "500 Internal Server Error", "");
        cerr << "Error opening file for writing: " << path << endl; //remove later
        return false;
    }

    outFile.write(fileContent.data(), fileContent.size());
    outFile.close();
    if (!outFile.good()) {
        setResponseData(INTERNAL_SERVER_ERROR, "", "text/plain", "500 Internal Server Error", "");
        return false;
    }
    return true;
}

int Client::runPostMethod() {
    string contentType = _request->getHeader("content-type");
    _response->setHeader("Content-type", "text/plain");

    if (contentType.find("multipart/form-data") != string::npos) {
        map<string, vector<char> > formData = _request->getFormData();

        if (formData.find("filename") == formData.end() || formData.find("fileContent") == formData.end()) {
            setResponseData(BAD_REQUEST, "", "text/plain", "Missing form data", "");
            return BAD_REQUEST;
        }

        string filename = string(formData["filename"].begin(), formData["filename"].end());
        string uri = _request->getURI();
        string directory = "content" + uri; // Change this to your desired directory structure

        vector<char> fileContent = formData["fileContent"];
        string fileContentType = string(formData["contentType"].begin(), formData["contentType"].end());

        if (!saveUploadedFile(filename, fileContent, directory)) {
            return FORBIDDEN;
        }

        // Set the correct content type in the response
        _response->setHeader("Content-type", fileContentType);
    } else if (contentType.empty()) {
        setResponseData(BAD_REQUEST, "", "text/plain", "Missing Content-Type header", "");
        return BAD_REQUEST;
    } else {
        setResponseData(BAD_REQUEST, "", "text/plain", "Unsupported Content-Type", "");
        return BAD_REQUEST;
    }

    setResponseData(CREATED, "", "text/plain", "File uploaded successfully", "");
    return CREATED;
}
