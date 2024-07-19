#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <map>
#include <string>

class Response {
    public:
        Response();
        void        setStatusCode(int code);
        void        setBody(const std::string& body);
        void        setHeader(const std::string& field, const std::string& value);
        std::string toString() const;

    private:
        int                                status_code_;
        std::string                        status_message_;
        std::map<std::string, std::string> headers_;
        std::string                        body_;

        std::string getStatusMessage(int code) const;
};

#endif  // RESPONSE_HPP
