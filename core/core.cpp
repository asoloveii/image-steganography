#include "core.h"

auto execute(std::string const& flag, std::string const& path, std::string const& msg) -> void {
    switch(hashFlag(flag)) {
        case FlagAlias::INFO:
            if (path.empty()) throw std::invalid_argument("No filepath is given");
            info(path);
            // if first two arguments are correct, the info prints and for any other arguments the message will appear
            if (!msg.empty()) throw std::invalid_argument("Too many arguments");
            break;
        case FlagAlias::ENCRYPT:
            if (path.empty() || msg.empty()) throw std::invalid_argument("Not enough arguments");
            encrypt(path, msg);
            break;
        case FlagAlias::DECRYPT:
            if (path.empty()) throw std::invalid_argument("No filepath is given");
            decrypt(path);
            // if first two arguments are correct, the info prints and for any other arguments the message will appear
            if (!msg.empty()) throw std::invalid_argument("Too many arguments");
            break;
        case FlagAlias::CHECK:
            if (path.empty() || msg.empty()) throw std::invalid_argument("Not enough arguments");
            check(path, msg);
            break;
        case FlagAlias::HELP:
        default:
            if (!path.empty() || !msg.empty()) throw std::invalid_argument("Too many arguments");
            help();
    }
}

auto info(std::string const& filepath) -> void {
    auto img = Image(filepath);
    fmt::println("File format: {}", img.imageTypeToString(img.getType())); // will not work with Image.method
    fmt::println("Memory usage: {} (bytes)", img.getSize());
    fmt::println("Size: {}x{}", img.getWidth(), img.getHeight());
    auto formattedDate = std::format("{}",  img.getLastModified());
    fmt::println("Last modified: {}", formattedDate);
};

auto encrypt(std::string const& filepath, std::string const& msg) -> void {
    auto img = Image(filepath);
    img.encrypt(msg);
    fmt::println("Message has been encrypted");
};

auto decrypt(std::string const& filepath) -> void {
    auto img = Image(filepath);
    fmt::println("The secret message: {}", img.decrypt());
};

auto check(std::string const& filepath, std::string const& msg) -> void {
    auto img = Image(filepath);
    fmt::println("The message {} fit inside the image", ((img.check(msg) ? "can" : "cannot")));
};

auto help() -> void {
    fmt::println("\nThis is an Image Steganography project, created by Arseni Salavei, s31726, 39c\n\n"
                 "Program supports .BMP, .PNG file formats\n"
                 "[FILE] - accepts an absolute path to the file\n"
                 "[MESSAGE] - if a message contains backspaces, then it should be enclosed in quotes \"\"\n");

    fmt::println("Functionalities: ");
    fmt::println("-i, -info [FILE]\n\treturns information about an image");
    fmt::println("-e, -encrypt [FILE] [MESSAGE]\n\tencrypts a message into the file");
    fmt::println("-d, -decrypt [FILE]\n\treturns a secret message from the file");
    fmt::println("-c, -check [FILE] [MESSAGE]\n\tchecks if message can be encrypted into the file");
}


auto hashFlag(std::string const& flag) -> FlagAlias {
    if (flag == "-i" || flag == "-info") return FlagAlias::INFO;
    else if (flag == "-e" || flag == "-encrypt") return FlagAlias::ENCRYPT;
    else if (flag == "-d" || flag == "-decrypt") return FlagAlias::DECRYPT;
    else if (flag == "-c" || flag == "-check") return FlagAlias::CHECK;
    else if (flag == "-h" || flag == "-help" || flag.empty()) return FlagAlias::HELP;
    else throw std::invalid_argument("Undefined command, try -help or -h for information about the program");
}
