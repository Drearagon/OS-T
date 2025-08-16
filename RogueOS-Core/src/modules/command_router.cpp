#include "command_router.hpp"
#include "../cli/help.h"

CommandRouter::CommandRouter(BackendFunc backend) : backend_(backend) {}

std::string CommandRouter::route(const std::string& input) {
    if (input == "help") {
        return cli_help();
    }
    return backend_(input);
}
