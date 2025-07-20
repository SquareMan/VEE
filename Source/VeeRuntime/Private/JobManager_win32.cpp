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


#include "Logging.hpp"


#include <Windows.h>

#include <thread>

namespace vee {
void lock_thread_to_core(std::thread& thread, std::size_t core_num) {
    if (core_num >= sizeof(DWORD_PTR) * 8) {
        log_error("Can't set affinity for core number greater than {}", sizeof(DWORD_PTR) * 8);
        return;
    }
    if (SetThreadAffinityMask(thread.native_handle(), 1 << core_num) == 0) {
        log_error("SetThreadAffinityMask failed for thread {} on core {}. GetLastError: {}", thread.native_handle(), core_num, GetLastError());
    }
}
} // namespace vee