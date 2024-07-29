#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <map>
#include <string>

class Response {
    private:
        int                                _statusCode;
        std::string                        _statusMessage;
        std::map<std::string, std::string> _headers;
        std::string                        _body;

        std::string getStatusMessage(int code) const;

    public:
        Response();
        void        setStatusCode(int code);
        void        setBody(const std::string& body);
        void        setHeader(const std::string& field, const std::string& value);
        std::string toString() const;
};

#endif  // RESPONSE_HPP
