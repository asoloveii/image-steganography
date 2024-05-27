#include "core.h"

auto execute(std::string const& flag, std::string const& path, std::string const& msg) -> void {
    switch(hashFlag(flag)) {
        case INFO:
            help();
            break;
        case ENCRYPT:
            encrypt(path, msg);
            break;
        case DECRYPT:
            decrypt(path);
            break;
        case CHECK:
            check(path, msg);
            break;
        case HELP:
        default: help();
    }
}

auto info(std::string const& filepath) -> void {
    fmt::println("info");
};

auto encrypt(std::string const& filepath, std::string const& msg) -> void {
    fmt::println("encrypt");
};

auto decrypt(std::string const& filepath) -> void {
    fmt::println("decrypt");
};

auto check(std::string const& path, std::string const& msg) -> void {
    fmt::println("check");
};

auto help() -> void {
    fmt::println("help");
}


// useful
auto hashFlag(std::string const& flag) -> FlagAlias {
    if (flag == "-i" || flag == "-info") return INFO;
    else if (flag == "-e" || flag == "-encrypt") return ENCRYPT;
    else if (flag == "-d" || flag == "-decrypt") return DECRYPT;
    else if (flag == "-c" || flag == "-check") return CHECK;
    else if (flag == "-h" || flag == "-help" || flag.empty()) return HELP;
    else throw std::invalid_argument("Undefined command, try -help or -h for information about the project");
}


