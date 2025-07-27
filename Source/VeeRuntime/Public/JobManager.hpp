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

#pragma once

#include "Name.hpp"


namespace vee {

struct JobDecl {
    Name name;
    void (*entry)();
    std::atomic<uint32_t>* signal_counter = nullptr;
};

namespace JobManager {
    void init();
    void shutdown();

    void queue_job(JobDecl decl, std::atomic<uint32_t>* wait_counter = nullptr);
    std::size_t num_workers();

    void yield();
    void terminate();
    void wait_for_counter(std::atomic<uint32_t>* counter);
};
} // namespace vee
