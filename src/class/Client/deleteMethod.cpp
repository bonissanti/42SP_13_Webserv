#include "../Response/Response.hpp"
#include "../../../include/Utils.hpp"
#include "Client.hpp"
#include <sys/stat.h>

int Client::runDeleteMethod() {
    struct stat buffer;
    Response response;

    // Check if the file exists
    if (stat(_response.getFilePath().c_str(), &buffer) != 0) {
        // File does not exist, return 404 Not Found
        _response.setStatusCode(NOT_FOUND);
        _response.setResponseBody("File not found");
        _response.setHeader("Content-Type", "text/plain");
        _response.setHeader("Content-Length", Utils::toString(_response.getBody().length()));
        _response.setHeader("Connection", "close");
        _response.setContentLength(response.getBody().length());
        return NOT_FOUND;
    }

    // Attempt to delete the file
    if (remove(_response.getFilePath().c_str()) != 0) {
        // Failed to delete the file, return 500 Internal Server Error
        _response.setStatusCode(INTERNAL_SERVER_ERROR);
        _response.setResponseBody("Failed to delete file");
        _response.setHeader("Content-Type", "text/plain");
        _response.setHeader("Content-Length", Utils::toString(response.getBody().length()));
        _response.setHeader("Connection", "close");
        _response.setContentLength(response.getBody().length());
        return INTERNAL_SERVER_ERROR;
    }

    // File successfully deleted, return 200 OK
    _response.setStatusCode(OK);
    _response.setResponseBody("File deleted successfully");
    _response.setHeader("Content-Type", "text/plain");
    _response.setHeader("Content-Length", Utils::toString(response.getBody().length()));
    _response.setHeader("Connection", "close");
    _response.setContentLength(response.getBody().length());
    return OK;
}