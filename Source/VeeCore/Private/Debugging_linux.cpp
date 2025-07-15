//
// Created by Square on 7/5/2025.
//

#include "Debugging.hpp"

#include <fstream>
#include <string>


bool is_debugger_attached() {
    std::ifstream file("/proc/self/status");
    if (!file.is_open()) {
        return false;
    }

    for (std::string line; std::getline(file, line);) {
        if (const std::size_t split_point = line.find('\t'); split_point != std::string::npos && line.starts_with("TracerPid:")) {
            const int pid = std::stoi(line.data() + split_point + 1);
            return pid > 0;
        }
    }

    return false;
}
