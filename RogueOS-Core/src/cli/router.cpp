#include "router.h"
#include "help.h"
#include <iostream>

namespace router {
    std::string handleCommand(const std::string& cmd) {
        if (cmd == "help") {
            std::string msg = cli_help();
            std::cout << msg;
            return msg;
        }
        std::string msg = "Unknown command: " + cmd + "\n";
        std::cout << msg;
        return msg;
    }
}
