#include <string>
#include <vector>
#include <fstream>
#include <cstring>
#include <filesystem>
#include <bitset>
#include <stdexcept>
#include <chrono>
#include <algorithm>
#include <fmt/core.h>
#include <fmt/ranges.h>

// supported file formats
enum class ImageType {PNG, BMP};

class Image {
private:
    std::vector<char> p_data;
    ImageType p_type;
    std::filesystem::file_time_type p_last_modified;
    std::filesystem::path p_path;
    size_t p_begin_data_idx;
    size_t p_end_data_idx;
    unsigned int p_size;
    unsigned int p_width=0;
    unsigned int p_height=0;

    std::vector<uint32_t> crcTable;

    // funciton to find the IDAT indices in a PNG file
    auto findIDATChunk() -> void;

    auto recalculateCRC() -> void;
    auto calculateCRC(const char* data, size_t length) -> uint32_t;
    auto static generateCRCTable() -> std::vector<uint32_t>;

    // function to find the data start index in a BMP file
    auto findBMPDataIndices() -> void;

    // function to set the least significant bit of a byte to a given value
    auto setLSB(char& byte, bool bit) -> void;

    // function to read the least significant bit of a byte
    auto getLSB(char byte) const -> bool;

public:
    explicit Image(std::string const& filepath);
    ~Image() = default;

    // getters
    auto getData() const -> std::vector<char>;
    auto getType() const -> ImageType;
    auto getPath() const -> std::filesystem::path;
    auto getLastModified() const -> std::filesystem::file_time_type;
    auto getWidth() const -> unsigned int;
    auto getHeight() const -> unsigned int;
    auto getSize() const -> unsigned int;

    // writes all data to filepath (will be used for encryption)
    auto saveData() -> void;
    // checks if message can be fit inside an image
    auto check(std::string const& message) const -> bool;
    // encrypts a secret message into an image
    auto encrypt(std::string const& msg) -> void;
    // reads a secret message from an image
    auto decrypt() const -> std::string;


    // functions used only by Image class for converting file format and enum
    auto static hashImageType(std::filesystem::path const& path) -> ImageType;
    auto static imageTypeToString(ImageType const& type) -> std::string;

    // functions to convert strings to binary and otherwise
    auto static stringToBinary(std::string const& message) -> std::vector<std::bitset<8>>;
    auto static binaryToString(std::vector<std::bitset<8>> const& msg) -> std::string;
};
