#pragma once
#include <functional>
#include <string>

class CommandRouter {
public:
    using BackendFunc = std::function<std::string(const std::string&)>;
    explicit CommandRouter(BackendFunc backend);
    std::string route(const std::string& input);
private:
    BackendFunc backend_;
};
