#include "Image.h"


Image::Image(std::string const& filepath) {
    auto path = std::filesystem::path(filepath);
    auto size = std::filesystem::file_size(path);
    auto image = std::fstream(path, std::ios::in | std::ios::binary);

    // extract file format and convert it into enum
    p_type = hashImageType(path);
    // check if we can open the file
    if (!image.is_open()) {
        throw std::runtime_error("Error opening file: " + filepath);
    }

    // last modified file time
    p_last_modified = std::filesystem::last_write_time(filepath);

    // vector to store all bytes from an image
    auto bytes = std::vector<char>(size);

    // write image's data to bytes vector
    image.read(bytes.data(), bytes.size());
    image.close();

    // for a specified file format extract info about the image
    switch(p_type) {
        case ImageType::PNG:
            unsigned char buf[8];
            image.seekg(16);
            image.read(reinterpret_cast<char*>(&buf), 8);
            p_width = (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + (buf[3] << 0);
            p_height = (buf[4] << 24) + (buf[5] << 16) + (buf[6] << 8) + (buf[7] << 0);
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
auto Image::getLastModified() const -> std::filesystem::file_time_type { return p_last_modified; }
auto Image::getSize() const -> unsigned int { return p_size; }
auto Image::getWidth() const -> unsigned int { return p_width; }
auto Image::getHeight() const -> unsigned int { return p_height; }

auto Image::writeData(std::string const& filepath) -> void {
    auto file = std::ofstream(filepath, std::ios::out | std::ios::binary);

    // check if the file was opened successfully
    if (!file.is_open()) {
        throw std::runtime_error("Error while opening a file...");
    }

    // write the data to the file
    file.write(p_data.data(), p_size);

    file.close();
}

auto Image::check(std::string const& message) const -> bool {
    size_t available_bits = p_size * 8;  // 8 bits per byte

    // convert message to binary
    auto binary_message = stringToBinary(message);

    // check if message length can fit in available LSBs
    return binary_message.size() <= available_bits;
}


auto Image::encrypt(std::string const& msg) -> void {
    // check if a message can fit inside an image
    if (!check(msg)) {
        fmt::println("Message cannot fit inside an image");
        return;
    }

    auto message = stringToBinary(msg);
    size_t message_index = 0;

    size_t image_begin_index=0;
    size_t image_end_index=0;

    switch(p_type) {
        case ImageType::PNG:
            image_begin_index = 33;
            image_end_index = 56;
            break;
        case ImageType::BMP:
            image_begin_index = 53;
            image_end_index = p_data.size();
    }

    for (size_t image_index = image_begin_index;
            image_index < image_end_index && message_index < message.size();
            ++image_index) {
        // 4. Extract a bit from the current message byte:
        char current_bit = message[message_index][7] ? 1 : 0; // Extract the least significant bit

        // 5. Modify the least significant bit of the current image byte:
        p_data[image_index] &= ~1; // Clear the least significant bit
        p_data[image_index] |= current_bit; // Set the least significant bit with the message bit

        // 6. Move to the next message bit:
        if (++message_index % 8 == 0) {
            // Reached the end of a byte in the message, move to the next message byte
        }
    }
}


auto Image::decrypt() const -> std::string {
    auto message = std::vector<std::bitset<8>>();

    for (size_t image_index = 0; image_index < p_data.size(); ++image_index) {
        // 2. Extract the least significant bit:
        char current_bit = p_data[image_index] & 1;

        // 3. Create a new bitset for the message byte if needed:
        if (message.empty() || message.back().size() == 8) {
            message.push_back(std::bitset<8>());
        }

        // 4. Add the extracted bit to the current message byte:
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

auto Image::imageType2String(ImageType type) -> std::string {
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


auto Image::binaryToString(std::vector<std::bitset<8>> msg) -> std::string {
    return "";
}
