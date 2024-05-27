#include "Image.h"


Image::Image(std::string const& filepath) {
    // todo
//    auto path = std::string(filepath);
//    auto size = std::filesystem::file_size(path);
//    auto image = std::fstream(path, std::ios::in | std::ios::binary);
//    auto bytes = std::vector<char>(size);
//
//    image.read(bytes.data(), bytes.size());

}

auto Image::getSize() const -> unsigned int { return p_size; }
auto Image::getDepth() const -> unsigned int { return p_depth; }
auto Image::getWidth() const -> unsigned int { return p_width; }
auto Image::getHeight() const -> unsigned int { return p_height; }
