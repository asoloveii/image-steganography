#include "core.h"

auto execute(std::string const& flag, std::string const& path, std::string const& msg) -> void {
    switch(hashFlag(flag)) {
        case FlagAlias::INFO:
            info(path);
            break;
        case FlagAlias::ENCRYPT:
            encrypt(path, msg);
            break;
        case FlagAlias::DECRYPT:
            decrypt(path);
            break;
        case FlagAlias::CHECK:
            check(path, msg);
            break;
        case FlagAlias::HELP:
        default: help();
    }
}

auto info(std::string const& filepath) -> void {
    auto img = Image(filepath);
    fmt::println("File format: {}", img.imageType2String(img.getType()));
    fmt::println("Memory usage: {} (bytes)", img.getSize());
    fmt::println("Size: {}x{}", img.getWidth(), img.getHeight());
    auto formattedDate = std::format("{}",  img.getLastModified());
    fmt::println("Last modified: {}", formattedDate);
};

auto encrypt(std::string const& filepath, std::string const& msg) -> void {
    auto img = Image(filepath);
    img.encrypt(msg);
    img.writeData(filepath);
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
    fmt::println("This is an image steganography project from Arseni Salavei, s31726, 39c\n\n"
                 "Program supports .BMP, .PNG file formats\n"
                 "[FILE] - accepts an absolute path to the file\n"
                 "[MESSAGE] - if a message contains backspaces, should be enclosed in quotes\n");

    fmt::println("Functionalities: ");
    fmt::println("-i, -info [FILE]\n\tprints information about an image");
    fmt::println("-e, -encrypt [FILE] [MESSAGE]\n\tencrypts a message into the file");
    fmt::println("-d, -decrypt [FILE]\n\tprints a secret message from the file");
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
