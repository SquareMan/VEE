//
// Created by Square on 9/29/2024.
//

#pragma once

#include "VeeCore.hpp"
#include <vector>

namespace Vee::Platform::Filesystem {
VEE_NODISCARD std::vector<char> read_binary_file(const char* filename);
}