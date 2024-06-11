#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <bitset>
#include <stdexcept>
#include <chrono>
#include <fmt/core.h>

// supported file formats
enum class ImageType {PNG, BMP};

class Image {
private:
    std::vector<char> p_data;
    ImageType p_type;
    std::filesystem::file_time_type p_last_modified;
    unsigned int p_size;
    unsigned int p_width;
    unsigned int p_height;

public:
    explicit Image(std::string const& filepath);
    ~Image() = default;

    // getters
    auto getData() const -> std::vector<char>;
    auto getType() const -> ImageType;
    auto getLastModified() const -> std::filesystem::file_time_type;
    auto getWidth() const -> unsigned int;
    auto getHeight() const -> unsigned int;
    auto getSize() const -> unsigned int;

    // writes all data from to file(will be used for encryption)
    auto writeData(std::string const& filepath) -> void;
    // checks if message can be fit inside an image
    auto check(std::string const& message) const -> bool;
    // encrypts a secret message into an image
    auto encrypt(std::string const& msg) -> void;
    // reads a secret message from an image
    auto decrypt() const -> std::string;

    // functions used only by Image class for converting file format and enum
    auto static hashImageType(std::filesystem::path const& path) -> ImageType;
    auto static imageType2String(ImageType type) -> std::string;

    // functions to convert strings to binary and otherwise
    auto static stringToBinary(std::string const& message) -> std::vector<std::bitset<8>>;
    auto static binaryToString(std::vector<std::bitset<8>> msg) -> std::string;
};

