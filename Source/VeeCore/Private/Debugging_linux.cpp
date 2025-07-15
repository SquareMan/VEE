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
