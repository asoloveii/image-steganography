#ifndef IMAGE_STEGANOGRAPHY_CORE_H
#define IMAGE_STEGANOGRAPHY_CORE_H
#include <fmt/core.h>
#include <string>

// DECLARATION
enum ImageType {PNG, JPG, BMP};
enum FlagAlias {INFO, ENCRYPT, DECRYPT, HELP, CHECK};

// fundamental functions, when running in a console
auto execute(std::string const& flag="", std::string const& path="", std::string const& msg="") -> void;
auto info(std::string const& filepath) -> void;
auto encrypt(std::string const& filepath, std::string const& msg) -> void;
auto decrypt(std::string const& filepath) -> void;
auto check(std::string const& path, std::string const& msg) -> void;
auto help() -> void;

// useful functions
auto encodeMessage(std::string const& message) -> char;
auto hashFlag(std::string const& flag) -> FlagAlias;

#endif //IMAGE_STEGANOGRAPHY_CORE_H
