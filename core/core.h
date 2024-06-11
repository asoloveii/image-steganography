#ifndef IMAGE_STEGANOGRAPHY_CORE_H
#define IMAGE_STEGANOGRAPHY_CORE_H
#include <fmt/core.h>
#include <vector>
#include <sys/stat.h>
#include <string>
#include <fstream>
#include <format>
#include <iostream>
#include "../Image/Image.h"

enum class FlagAlias {INFO, ENCRYPT, DECRYPT, HELP, CHECK};

// fundamental function, when running in a console
auto execute(std::string const& flag="", std::string const& path="", std::string const& msg="") -> void;

// functions will be used in execute() for a specific flag alias
auto info(std::string const& filepath) -> void;
auto encrypt(std::string const& filepath, std::string const& msg) -> void;
auto decrypt(std::string const& filepath) -> void;
auto check(std::string const& filepath, std::string const& msg) -> void;
auto help() -> void;

// returns an enum for a given flag alias from the console
auto hashFlag(std::string const& flag) -> FlagAlias;
#endif //IMAGE_STEGANOGRAPHY_CORE_H
