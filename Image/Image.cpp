#include "Image.h"


Image::Image(std::string const& filepath) {
    auto path = std::filesystem::path(filepath);
    auto size = std::filesystem::file_size(path);
    auto image = std::fstream(path, std::ios::in | std::ios::binary);

    // check if we can open the file
    if (!image.is_open()) {
        throw std::runtime_error("Error opening file: " + filepath);
    }

    // save a filepath
    p_path = path;

    // extract file format and convert it into enum
    p_type = hashImageType(path);

    // last modified file time
    p_last_modified = std::filesystem::last_write_time(filepath);

    // vector to store all bytes from an image
    auto bytes = std::vector<char>(size);

    // write image's data to bytes vector
    image.read(bytes.data(), bytes.size());
    image.close();

    // for a specified file format extract width and height
    switch(p_type) {
        case ImageType::PNG:
            /*
            unsigned char buf[8];
            image.seekg(16);
            image.read(reinterpret_cast<char*>(&buf), 8);
            p_width = (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + (buf[3] << 0);
            p_height = (buf[4] << 24) + (buf[5] << 16) + (buf[6] << 8) + (buf[7] << 0);
            */
            p_width = static_cast<int>(bytes[16])
                    | static_cast<int>(bytes[21] << 8)
                    | static_cast<int>(bytes[22] << 16)
                    | static_cast<int>(bytes[23] << 24);
            p_height = static_cast<int>(bytes[20] | (bytes[21] << 8) | (bytes[22] << 16) | (bytes[23] << 24));
            break;
        case ImageType::BMP:
            p_width = static_cast<int>(bytes[18] | (bytes[19] << 8));
            p_height = static_cast<int>(bytes[22] | (bytes[23] << 8));
            break;
    }

    p_data = bytes;
    p_size = size;
}

auto Image::getData() const -> std::vector<char> { return p_data; }
auto Image::getType() const -> ImageType { return p_type; }
auto Image::getPath() const -> std::filesystem::path { return p_path; }
auto Image::getLastModified() const -> std::filesystem::file_time_type { return p_last_modified; }
auto Image::getSize() const -> unsigned int { return p_size; }
auto Image::getWidth() const -> unsigned int { return p_width; }
auto Image::getHeight() const -> unsigned int { return p_height; }

auto Image::saveData() -> void {
    auto file = std::ofstream(p_path, std::ios::out | std::ios::binary);

    // check if the file was opened successfully
    if (!file.is_open()) {
        throw std::runtime_error("Error while opening a file...");
    }

    // write the data to the file
    file.write(p_data.data(), p_size);

    file.close();
}

auto Image::check(std::string const& message) const -> bool {
    // convert message to binary
    auto binary_message = stringToBinary(message);

    // check if message length can fit in the image
    return p_size >= binary_message.size() * 8;
}

auto Image::encrypt(std::string const& msg) -> void {
    // check if a message can fit inside an image
    if (!check(msg)) {
        fmt::println("Message cannot fit inside an image");
        return;
    }

    auto message = stringToBinary(msg);

    size_t message_index = 0;
    for (size_t image_index = 0; image_index < p_size && message_index < message.size(); ++image_index) {
        // extract a bit from the current message byte:
        char current_bit = message[message_index][7] ? 1 : 0; // extract the least significant bit

        // modify the least significant bit of the current image byte:
        p_data[image_index] &= ~1; // clear the least significant bit
        p_data[image_index] |= current_bit; // set the least significant bit with the message bit

        // move to the next message bit:
        if (++message_index % 8 == 0) {
            // reached the end of a byte in the message, move to the next message byte
        }
    }

    // save modifications to the file
    saveData();
}

auto Image::decrypt() const -> std::string {
    auto message = std::vector<std::bitset<8>>();

    for (size_t image_index = 0; image_index < p_data.size(); ++image_index) {
        // extract the least significant bit:
        char current_bit = p_data[image_index] & 1;

        // create a new bitset for the message byte if needed:
        if (message.empty() || message.back().size() == 8) {
            message.push_back(std::bitset<8>());
        }

        // add the extracted bit to the current message byte:
        message.back() <<= 1;
        message.back().set(0, current_bit);
    }

    return binaryToString(message);
}

auto Image::hashImageType(std::filesystem::path const& path) -> ImageType {
    if (path.extension() == ".png") return ImageType::PNG;
    else if (path.extension() == ".bmp") return ImageType::BMP;
    else throw std::runtime_error("Unsupported file format...");
}

auto Image::imageTypeToString(ImageType const& type) -> std::string {
    switch(type) {
        case ImageType::PNG:
            return "PNG";
        case ImageType::BMP:
            return "BMP";
    }
}

auto Image::stringToBinary(std::string const& message) -> std::vector<std::bitset<8>> {
    auto out = std::vector<std::bitset<8>>();
    for (std::size_t i = 0; i < message.size(); ++i)
    {
        out.emplace_back(message.c_str()[i]);
    }
    return out;
}

auto Image::binaryToString(std::vector<std::bitset<8>> const& msg) -> std::string {
    std::string result;

    // Iterate through each bitset in the message
    for (const auto& bitset : msg) {
        // Convert the bitset to a string representation (1s and 0s)
        int char_value = bitset.to_ulong();

        // Check for valid ASCII range (0 - 127 for printable characters)
        if (char_value >= 0 && char_value <= 127) {
            // Cast the value to a char and append it to the result
            result += static_cast<char>(char_value);
        }
    }

    return result;
}
