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
            setResponseData(INTERNAL_SERVER_ERROR, ERROR500, "text/html", _response.getStatusPage(INTERNAL_SERVER_ERROR), directory);
            cerr << "Error creating directory: " << directory << endl;
            return false;
        }
	}

    if (access(directory.c_str(), W_OK) == -1) {
        setResponseData(FORBIDDEN, ERROR403, "text/html", "403 Forbidden", "");
        return false;
    }

	string finalFilename = filename.empty() ? "default_filename.txt" : filename;

	string path = directory + "/" + finalFilename;
    ofstream outFile(path.c_str(), ios::binary);
    if (!outFile) {
        setResponseData(INTERNAL_SERVER_ERROR, ERROR500, "text/html", _response.getStatusPage(INTERNAL_SERVER_ERROR), path);
        return false;
    }

    outFile.write(fileContent.data(), fileContent.size());
    outFile.close();
    if (!outFile.good()) {
        setResponseData(INTERNAL_SERVER_ERROR, ERROR500, "text/html", _response.getStatusPage(INTERNAL_SERVER_ERROR), path);
        return false;
    }
    return true;
}

int Client::runPostMethod(string filePath, Route matchedRoute) {
    string contentType = _request.getHeader("content-type");
    _response.setHeader("Content-type", "text/plain");

    if (contentType.find("multipart/form-data") != string::npos) {
        map<string, vector<char> > formData = _request.getFormData();

        if (formData.find("filename") == formData.end() || formData.find("fileContent") == formData.end()) {
            setResponseData(BAD_REQUEST, ERROR400, "text/html", _response.getStatusPage(BAD_REQUEST), "");
            return BAD_REQUEST;
        }

        string filename = string(formData["filename"].begin(), formData["filename"].end());

        vector<char> fileContent = formData["fileContent"];
        string fileContentType = string(formData["contentType"].begin(), formData["contentType"].end());

        if (!saveUploadedFile(filename, fileContent, filePath)) {
            return FORBIDDEN;
        }

        // Set the correct content type in the response
        _response.setHeader("Content-type", fileContentType);
        setResponseData(CREATED, filePath, "text/html", _response.setCreatedBody(_request.getURI() + "/" + filename), filePath);
        return CREATED;
    } else if (contentType.find("x-www-form-urlencoded") != string::npos) {
		cout << "Body: " << _request.getBody() << endl;
		cout << "Type: " << contentType << endl;
		string contentType = defineContentType(filePath);
    	string responseBody = defineResponseBody(matchedRoute, filePath);
		setResponseData(_response.getStatusCode(), "", contentType, responseBody, filePath);
		return _response.getStatusCode();
    } else {
        setResponseData(BAD_REQUEST, ERROR400, "text/html", _response.getStatusPage(BAD_REQUEST), "");
        return BAD_REQUEST;
    }

    setResponseData(OK, "", "text/html", _response.getStatusPage(OK), filePath);
    return CREATED;
}
