//    Copyright 2025 Steven Casper
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.


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