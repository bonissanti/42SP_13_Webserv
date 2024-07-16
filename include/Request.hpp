#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include <string>

class Request {
    public:
        Request(const std::string &raw_request);
        std::string getMethod() const;
        std::string getPath() const;
        std::string getHeader(const std::string &field) const;
        std::string getBody() const;

    private:
        void parseRequest(const std::string &raw_request);

        std::string                        method_;
        std::string                        path_;
        std::map<std::string, std::string> headers_;
        std::string                        body_;
};

#endif  // REQUEST_HPP
