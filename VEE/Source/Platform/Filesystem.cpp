//
// Created by Square on 9/29/2024.
//

#include "Filesystem.hpp"

#include <fstream>

namespace Vee::Platform::Filesystem {
std::vector<char> read_binary_file(const char* filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    const size_t file_size = file.tellg();
    std::vector<char> result(file_size);
    file.seekg(0);
    file.read(result.data(), static_cast<std::streamsize>(file_size));
    file.close();

    return result;
}
} // namespace Vee::Platform::Filesystem