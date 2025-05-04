//
// Created by Square on 9/29/2024.
//

#include "Platform/Filesystem.hpp"

#include <fstream>

namespace vee::platform::filesystem {
std::vector<std::byte> read_binary_file(const char* filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    const size_t file_size = file.tellg();
    std::vector<std::byte> result(file_size);
    file.seekg(0);
    file.read(reinterpret_cast<char*>(result.data()), static_cast<std::streamsize>(file_size));
    file.close();

    return result;
}
} // namespace vee::platform::filesystem