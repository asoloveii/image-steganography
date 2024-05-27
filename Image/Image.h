#ifndef IMAGE_STEGANOGRAPHY_IMAGE_H
#define IMAGE_STEGANOGRAPHY_IMAGE_H
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include "../utils/core.cpp"

class Image {
private:
    std::vector<char> p_data;
    unsigned int p_size=0;
    unsigned int p_width;
    unsigned int p_height;
    unsigned int p_depth;                // number of channels

public:
    Image(std::string const& filepath);
    Image(Image const& img);
    ~Image() = default;

    // getters
    auto getWidth() const -> unsigned int;
    auto getHeight() const -> unsigned int;
    auto getDepth() const -> unsigned int;
    auto getSize() const -> unsigned int;

    // methods
    auto check(std::string const& message) const -> bool;
    auto encrypt(std::string const& message) -> void;
    auto decrypt() const -> std::string;
};

#endif
