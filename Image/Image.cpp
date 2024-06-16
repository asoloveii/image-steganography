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

    p_size = size;
    p_data = bytes;

    if (p_type == ImageType::PNG) {
        findIDATChunk();
    } else if (p_type == ImageType::BMP) {
        findBMPDataIndices();
    }

    crcTable = generateCRCTable();

    // for a specified file format extract width and height
    switch(p_type) {
        case ImageType::PNG:
            // I hate c++
            p_width = (static_cast<unsigned char>(bytes[16]) << 24) |
                      (static_cast<unsigned char>(bytes[17]) << 16) |
                      (static_cast<unsigned char>(bytes[18]) << 8)  |
                      (static_cast<unsigned char>(bytes[19]));
            p_height = (static_cast<unsigned char>(bytes[20]) << 24) |
                       (static_cast<unsigned char>(bytes[21]) << 16) |
                       (static_cast<unsigned char>(bytes[22]) << 8)  |
                       (static_cast<unsigned char>(bytes[23]));
            break;
        case ImageType::BMP:
            p_width = (static_cast<unsigned char>(bytes[21]) << 24) |
                      (static_cast<unsigned char>(bytes[20]) << 16) |
                      (static_cast<unsigned char>(bytes[19]) << 8)  |
                      (static_cast<unsigned char>(bytes[18]));
            p_height = (static_cast<unsigned char>(bytes[25]) << 24) |
                       (static_cast<unsigned char>(bytes[24]) << 16) |
                       (static_cast<unsigned char>(bytes[23]) << 8)  |
                       (static_cast<unsigned char>(bytes[22]));
            break;
    }
}

auto Image::findIDATChunk() -> void {
    const char IDAT[] = { 'I', 'D', 'A', 'T' };
    size_t pos = 8; // Skip PNG signature
    p_begin_data_idx = 0;
    p_end_data_idx = 0;

    while (pos < p_size) {

        uint32_t chunkLength = static_cast<uint8_t>(p_data[pos]) << 24 |
                               static_cast<uint8_t>(p_data[pos + 1]) << 16 |
                               static_cast<uint8_t>(p_data[pos + 2]) << 8 |
                               static_cast<uint8_t>(p_data[pos + 3]);

        if (std::memcmp(&p_data[pos + 4], IDAT, 4) == 0) {
            if (p_begin_data_idx == 0) {
                p_begin_data_idx = pos + 8; // Start after length and type
            }
            p_end_data_idx = pos + 8 + chunkLength; // End at the end of this IDAT chunk
        }
        pos += 8 + chunkLength + 4; // Move to the next chunk (length + type + CRC)
    }

    if (p_begin_data_idx == 0 || p_end_data_idx == 0) {
        fmt::println("Failed to locate IDAT chunk.");

        throw std::runtime_error("IDAT chunk not found");
    }
}

auto Image::findBMPDataIndices() -> void {
    p_begin_data_idx = *reinterpret_cast<uint32_t*>(p_data.data() + 10);
    p_end_data_idx = p_size;
}

auto Image::setLSB(char& byte, bool bit) -> void {
    if (bit)
        byte = (byte & 0xFE) | 0x01; // set lsb to 1
    else
        byte = byte & 0xFE; // set lsb to 0
}

auto Image::getLSB(char byte) const -> bool {
    return byte & 0x01; // Return LSB (bit 0)
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
    size_t messageLength = message.size();
    size_t requiredSpace = 32 + messageLength * 8;  // 32 bits for length + 8 bits per character
    return requiredSpace <= (p_end_data_idx - p_begin_data_idx);
}

auto Image::generateCRCTable() -> std::vector<uint32_t> {
    std::vector<uint32_t> table(256);
    uint32_t polynomial = 0xEDB88320;
    for (uint32_t i = 0; i < 256; ++i) {
        uint32_t crc = i;
        for (uint32_t j = 0; j < 8; ++j) {
            if (crc & 1) {
                crc = (crc >> 1) ^ polynomial;
            } else {
                crc >>= 1;
            }
        }
        table[i] = crc;
    }
    return table;
}

auto Image::calculateCRC(const char* data, size_t length) -> uint32_t {
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < length; ++i) {
        uint8_t byte = static_cast<uint8_t>(data[i]);
        crc = (crc >> 8) ^ crcTable[(crc ^ byte) & 0xFF];
    }
    return crc ^ 0xFFFFFFFF;
}

auto Image::recalculateCRC() -> void {
    if (p_type != ImageType::PNG) {
        throw std::runtime_error("CRC recalculation is only applicable for PNG images.");
    }

    size_t pos = 8; // Skip PNG signature

    while (pos < p_size) {
        uint32_t chunkLength = static_cast<uint8_t>(p_data[pos]) << 24 |
                               static_cast<uint8_t>(p_data[pos + 1]) << 16 |
                               static_cast<uint8_t>(p_data[pos + 2]) << 8 |
                               static_cast<uint8_t>(p_data[pos + 3]);

        if (std::memcmp(&p_data[pos + 4], "IDAT", 4) == 0) {
            uint32_t crc = calculateCRC(&p_data[pos + 4], 4 + chunkLength);
            char* chunkCRC = &p_data[pos + 8 + chunkLength];
            chunkCRC[0] = (crc >> 24) & 0xFF;
            chunkCRC[1] = (crc >> 16) & 0xFF;
            chunkCRC[2] = (crc >> 8) & 0xFF;
            chunkCRC[3] = crc & 0xFF;
        }

        pos += 8 + chunkLength + 4;
        /*
        char* chunkType = &p_data[pos + 4];
        char* chunkData = &p_data[pos + 8];
        char* chunkCRC = &p_data[pos + 8 + chunkLength];

        // Calculate CRC for the chunk type and data
        uint32_t crc = calculateCRC(chunkData, chunkLength);

        // Rewrite CRC value in the file
        chunkCRC[0] = (crc >> 24) & 0xFF;
        chunkCRC[1] = (crc >> 16) & 0xFF;
        chunkCRC[2] = (crc >> 8) & 0xFF;
        chunkCRC[3] = crc & 0xFF;

        // Move to the next chunk (length + type + data + CRC)
        pos += 8 + chunkLength + 4;
         */

    }
}

auto Image::encrypt(std::string const& msg) -> void {
    // check if a message can fit inside an image
    if (!check(msg)) {
        fmt::println("Message cannot fit inside an image");
        return;
    }

    auto message = stringToBinary(msg);

    // embed message length in the first 32 bits (4 bytes) of the image data
    size_t messageLength = message.size();
    for (int i = 0; i < 32; ++i) {
        size_t imageIndex = p_begin_data_idx + i;
        char& byte = p_data[imageIndex];
        bool bit = (messageLength >> (31 - i)) & 1;
        setLSB(byte, bit);
    }

    // embed each bit of the message into the LSB of the image data
    size_t messageIndex = 0;
    for (size_t imageIndex = p_begin_data_idx + 32;
         imageIndex < p_end_data_idx && messageIndex < message.size() * 8;
         ++imageIndex)
    {
        // Extract a bit from the current message byte:
        char currentBit = message[messageIndex / 8][7 - (messageIndex % 8)];
        // modify the lsb of the current image byte
        setLSB(p_data[imageIndex], currentBit);
        // Move to the next message bit:
        ++messageIndex;
    }

    // Update last modified time and size
    p_last_modified = std::filesystem::last_write_time(p_path);
    p_size = p_data.size();

    if (p_type==ImageType::PNG) recalculateCRC();

    // Save modified data back to file
    saveData();
}

auto Image::decrypt() const -> std::string {
    std::string extractedMessage;

    size_t messageLength = 0;
    for (int i = 0; i < 32; ++i) {
        const char& byte = p_data[p_begin_data_idx + i];
        bool bit = getLSB(byte);
        messageLength |= (bit << (31 - i));
    }

    // extract message bits from the image data
    std::vector<std::bitset<8>> messageBits(messageLength);
    size_t messageIndex = 0;
    for (size_t imageIndex = p_begin_data_idx + 32;
         imageIndex < p_end_data_idx && messageIndex < messageLength * 8;
         ++imageIndex)
    {
         // extract the LSB from the current image byte:
        bool bit = getLSB(p_data[imageIndex]);

        // store the bit in the appropriate position in the message bits:
        messageBits[messageIndex / 8][7 - (messageIndex % 8)] = bit;

        // move to the next message bit:
        ++messageIndex;
    }

    fmt::println("{}", p_end_data_idx);
    for (int i=0; i < messageLength; i++) fmt::println("{}", messageBits[i].to_string());

    // convert bitsets back to characters to form the extracted message
    extractedMessage = binaryToString(messageBits);

    return extractedMessage;
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
    std::vector<std::bitset<8>> result;
    for (char ch : message) {
        result.push_back(std::bitset<8>(ch));
    }
    return result;
}

auto Image::binaryToString(std::vector<std::bitset<8>> const& msg) -> std::string {
    std::string result;
    for (auto const& bits : msg) {
        result.push_back(static_cast<char>(bits.to_ulong()));
    }

    return result;
}
