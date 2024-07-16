#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <map>
#include <string>

class Config {
    public:
        Config();
        void        loadFromFile(const std::string& filepath);
        int         getPort() const;
        std::string getDocumentRoot() const;

    private:
        int         port_;
        std::string document_root_;
};

#endif  // CONFIG_HPP
